/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// CLASS HEADER
#include <dali-toolkit/internal/text/rendering/atlas/text-atlas-renderer.h>

// EXTERNAL INCLUDES
#include <dali/dali.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/atlas-manager/atlas-manager.h>
#include <dali-toolkit/internal/text/rendering/atlas/atlas-glyph-manager.h>
#include <dali-toolkit/internal/text/rendering/shaders/text-basic-shader.h>
#include <dali-toolkit/internal/text/rendering/shaders/text-bgra-shader.h>
#include <dali-toolkit/internal/text/rendering/shaders/text-basic-shadow-shader.h>
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::Concise, true, "LOG_TEXT_ATLAS_RENDERER");
#endif

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Toolkit::Text;

namespace
{
  const Vector2 DEFAULT_ATLAS_SIZE( 512.0f, 512.0f );
  const Vector2 DEFAULT_BLOCK_SIZE( 16.0f, 16.0f );
  const Vector2 PADDING( 4.0f, 4.0f ); // Allow for variation in font glyphs
}

struct AtlasRenderer::Impl : public ConnectionTracker
{

  enum Style
  {
    STYLE_NORMAL,
    STYLE_DROP_SHADOW
  };

  struct MeshRecord
  {
    Vector4 mColor;
    uint32_t mAtlasId;
    MeshData mMeshData;
    FrameBufferImage mBuffer;
    bool mIsUnderline;
  };

  struct Extent
  {
    float mBaseLine;
    float mLeft;
    float mRight;
    float mUnderlinePosition;
    float mUnderlineThickness;
    uint32_t mMeshRecordIndex;
  };

  struct AtlasRecord
  {
    uint32_t mImageId;
    Text::GlyphIndex mIndex;
  };

  struct MaxBlockSize
  {
    FontId mFontId;
    Vector2 mNeededBlockSize;
  };

  Impl()
  {
    mGlyphManager = AtlasGlyphManager::Get();
    mFontClient = TextAbstraction::FontClient::Get();
    mGlyphManager.SetNewAtlasSize( DEFAULT_ATLAS_SIZE, DEFAULT_BLOCK_SIZE );
    mBasicShader = BasicShader::New();
    mBgraShader = BgraShader::New();
    mBasicShadowShader = BasicShadowShader::New();

    mFace.reserve( 6u );
    mFace.push_back( 0 ); mFace.push_back( 2u ); mFace.push_back( 1u );
    mFace.push_back( 1u ); mFace.push_back( 2u ); mFace.push_back( 3u );
  }

  void AddGlyphs( const std::vector<Vector2>& positions,
                  const Vector<GlyphInfo>& glyphs,
                  const Vector4& textColor,
                  const Vector2& shadowOffset,
                  const Vector4& shadowColor,
                  float underlineEnabled,
                  const Vector4& underlineColor )
  {
    AtlasManager::AtlasSlot slot;
    std::vector< MeshRecord > meshContainer;
    Vector< Extent > extents;

    float currentUnderlinePosition = 0.0f;
    float currentUnderlineThickness = 0.0f;
    FontId lastFontId = 0;
    Style style = STYLE_NORMAL;

    if ( shadowOffset.x != 0.0f || shadowOffset.y != 0.0f )
    {
      style = STYLE_DROP_SHADOW;
    }

    if ( mImageIds.Size() )
    {
      // Unreference any currently used glyphs
      RemoveText();
    }

    CalculateBlocksSize( glyphs );

    for ( uint32_t i = 0; i < glyphs.Size(); ++i )
    {
      GlyphInfo glyph = glyphs[ i ];

      // No operation for white space
      if ( glyph.width && glyph.height )
      {
        // Are we still using the same fontId as previous
        if ( glyph.fontId != lastFontId )
        {
          // We need to fetch fresh font underline metrics
          FontMetrics fontMetrics;
          mFontClient.GetFontMetrics( glyph.fontId, fontMetrics );
          currentUnderlinePosition = fontMetrics.underlinePosition;
          currentUnderlineThickness = fontMetrics.underlineThickness;

          // Ensure that an underline is at least 1 pixel high
          if ( currentUnderlineThickness < 1.0f )
          {
            currentUnderlineThickness = 1.0f;
          }
        }

        Vector2 position = positions[ i ];
        MeshData newMeshData;
        mGlyphManager.Cached( glyph.fontId, glyph.index, slot );

        if ( slot.mImageId )
        {
          // This glyph already exists so generate mesh data plugging in our supplied position
          mGlyphManager.GenerateMeshData( slot.mImageId, position, newMeshData );
          mImageIds.PushBack( slot.mImageId );
        }
        else
        {

          // Select correct size for new atlas if needed....?
          if ( lastFontId != glyph.fontId )
          {
            for ( uint32_t j = 0; j < mBlockSizes.size(); ++j )
            {
              if ( mBlockSizes[ j ].mFontId == glyph.fontId )
              {
                mGlyphManager.SetNewAtlasSize( DEFAULT_ATLAS_SIZE, mBlockSizes[ j ].mNeededBlockSize );
              }
            }
            lastFontId = glyph.fontId;
          }

          // Glyph doesn't currently exist in atlas so upload
          BufferImage bitmap = mFontClient.CreateBitmap( glyph.fontId, glyph.index );

          // Locate a new slot for our glyph
          mGlyphManager.Add( glyph, bitmap, slot );

          // Generate mesh data for this quad, plugging in our supplied position
          if ( slot.mImageId )
          {
            mGlyphManager.GenerateMeshData( slot.mImageId, position, newMeshData );
            mImageIds.PushBack( slot.mImageId );
          }
        }
        // Find an existing mesh data object to attach to ( or create a new one, if we can't find one using the same atlas)
        StitchTextMesh( meshContainer,
                        newMeshData,
                        extents,
                        textColor,
                        position.y + glyph.yBearing,
                        currentUnderlinePosition,
                        currentUnderlineThickness,
                        slot );
      }
    }

    if ( underlineEnabled )
    {
      // Check to see if any of the text needs an underline
      GenerateUnderlines( meshContainer, extents, underlineColor, textColor );
    }

    // For each MeshData object, create a mesh actor and add to the renderable actor
    if ( meshContainer.size() )
    {
      for ( std::vector< MeshRecord >::iterator mIt = meshContainer.begin(); mIt != meshContainer.end(); ++mIt )
      {
        MeshActor actor = MeshActor::New( Mesh::New( mIt->mMeshData ) );
        actor.SetColor( mIt->mColor );
        if ( mIt->mIsUnderline )
        {
          actor.SetColorMode( USE_OWN_COLOR );
        }
        else
        {
          actor.SetColorMode( USE_OWN_MULTIPLY_PARENT_COLOR );
        }

        // Check to see what pixel format the shader should be
        if ( mGlyphManager.GetPixelFormat( mIt->mAtlasId ) == Pixel::L8 )
        {
          // Create an effect if necessary
          if ( style == STYLE_DROP_SHADOW )
          {
            actor.Add( GenerateShadow( *mIt, shadowOffset, shadowColor ) );
          }
          actor.SetShaderEffect( mBasicShader );
        }
        else
        {
          actor.SetShaderEffect( mBgraShader );
        }

        if ( mActor )
        {
          mActor.Add( actor );
        }
        else
        {
          mActor = actor;
        }
      }
      mActor.OffStageSignal().Connect( this, &AtlasRenderer::Impl::OffStageDisconnect );
    }
#if defined(DEBUG_ENABLED)
    Toolkit::AtlasGlyphManager::Metrics metrics = mGlyphManager.GetMetrics();
    DALI_LOG_INFO( gLogFilter, Debug::Concise, "TextAtlasRenderer::GlyphManager::GlyphCount: %i, AtlasCount: %i, TextureMemoryUse: %iK\n",
                                                metrics.mGlyphCount,
                                                metrics.mAtlasMetrics.mAtlasCount,
                                                metrics.mAtlasMetrics.mTextureMemoryUsed / 1024 );
    for ( uint32_t i = 0; i < metrics.mAtlasMetrics.mAtlasCount; ++i )
    {
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Atlas [%i] %sPixels: %s Size: %ix%i, BlockSize: %ix%i, BlocksUsed: %i/%i\n",
                                                 i + 1, i > 8 ? "" : " ",
                                                 metrics.mAtlasMetrics.mAtlasMetrics[ i ].mPixelFormat == Pixel::L8 ? "L8  " : "BGRA",
                                                 metrics.mAtlasMetrics.mAtlasMetrics[ i ].mWidth,
                                                 metrics.mAtlasMetrics.mAtlasMetrics[ i ].mHeight,
                                                 metrics.mAtlasMetrics.mAtlasMetrics[ i ].mBlockWidth,
                                                 metrics.mAtlasMetrics.mAtlasMetrics[ i ].mBlockHeight,
                                                 metrics.mAtlasMetrics.mAtlasMetrics[ i ].mBlocksUsed,
                                                 metrics.mAtlasMetrics.mAtlasMetrics[ i ].mTotalBlocks );
    }
#endif
  }

  void StitchTextMesh( std::vector< MeshRecord >& meshContainer,
                       MeshData& newMeshData,
                       Vector< Extent >& extents,
                       const Vector4& color,
                       float baseLine,
                       float underlinePosition,
                       float underlineThickness,
                       AtlasManager::AtlasSlot& slot )
  {
    if ( slot.mImageId )
    {
      MeshData::VertexContainer verts = newMeshData.GetVertices();
      float left = verts[ 0 ].x;
      float right = verts[ 1 ].x;

      // Check to see if there's a mesh data object that references the same atlas ?
      uint32_t index = 0;
      for ( std::vector< MeshRecord >::iterator mIt = meshContainer.begin(); mIt != meshContainer.end(); ++mIt, ++index )
      {
        if ( slot.mAtlasId == mIt->mAtlasId )
        {
          // Stitch the mesh to the existing mesh and adjust any extents
          mGlyphManager.StitchMesh( mIt->mMeshData, newMeshData );
          AdjustExtents( extents,
                         meshContainer,
                         index,
                         color,
                         left,
                         right,
                         baseLine,
                         underlinePosition,
                         underlineThickness );
          return;
        }
      }

      // No mesh data object currently exists that references this atlas, so create a new one
      MeshRecord meshRecord;
      meshRecord.mAtlasId = slot.mAtlasId;
      meshRecord.mMeshData = newMeshData;
      meshRecord.mColor = color;
      meshRecord.mIsUnderline = false;
      meshContainer.push_back( meshRecord );

      // Adjust extents for this new meshrecord
      AdjustExtents( extents,
                     meshContainer,
                     meshContainer.size() - 1u,
                     color,
                     left,
                     right,
                     baseLine,
                     underlinePosition,
                     underlineThickness );

    }
  }

  void AdjustExtents( Vector< Extent >& extents,
                      std::vector< MeshRecord>& meshRecords,
                      uint32_t index,
                      const Vector4& color,
                      float left,
                      float right,
                      float baseLine,
                      float underlinePosition,
                      float underlineThickness )
  {
    bool foundExtent = false;
    for ( Vector< Extent >::Iterator eIt = extents.Begin(); eIt != extents.End(); ++eIt )
    {
      if ( Equals( baseLine, eIt->mBaseLine ) )
      {
        // If we've found an extent with the same color then we don't need to create a new extent
        if ( color == meshRecords[ index ].mColor )
        {
          foundExtent = true;
          if ( left < eIt->mLeft )
          {
            eIt->mLeft = left;
          }
          if ( right > eIt->mRight  )
          {
            eIt->mRight = right;
          }
        }
        // Font metrics use negative values for lower underline positions
        if ( underlinePosition < eIt->mUnderlinePosition )
        {
          eIt->mUnderlinePosition = underlinePosition;
        }
        if ( underlineThickness > eIt->mUnderlineThickness )
        {
          eIt->mUnderlineThickness = underlineThickness;
        }
      }
    }
    if ( !foundExtent )
    {
      Extent extent;
      extent.mLeft = left;
      extent.mRight = right;
      extent.mBaseLine = baseLine;
      extent.mUnderlinePosition = underlinePosition;
      extent.mUnderlineThickness = underlineThickness;
      extent.mMeshRecordIndex = index;
      extents.PushBack( extent );
    }
  }

  // Unreference any glyphs that were used with this actor
  void OffStageDisconnect( Dali::Actor actor )
  {
    RemoveText();
  }

  void RemoveText()
  {
    for ( uint32_t i = 0; i < mImageIds.Size(); ++i )
    {
      mGlyphManager.Remove( mImageIds[ i ] );
    }
    mImageIds.Resize( 0 );
  }

  void CalculateBlocksSize( const Vector<GlyphInfo>& glyphs )
  {
    MaxBlockSize maxBlockSize;
    for ( uint32_t i = 0; i < glyphs.Size(); ++i )
    {
      // Get the fontId of this glyph and check to see if a max size exists?
      FontId fontId = glyphs[ i ].fontId;
      float paddedWidth = glyphs[ i ].width + PADDING.x;
      float paddedHeight = glyphs[ i ].height + PADDING.y;
      bool foundFont = false;

      for ( uint32_t j = 0; j < mBlockSizes.size(); ++j )
      {
        if ( mBlockSizes[ j ].mFontId == fontId )
        {
          foundFont = true;
          if ( mBlockSizes[ j ].mNeededBlockSize.x < paddedWidth )
          {
            mBlockSizes[ j ].mNeededBlockSize.x = paddedWidth;
          }
          if ( mBlockSizes[ j ].mNeededBlockSize.y < paddedHeight )
          {
            mBlockSizes[ j ].mNeededBlockSize.y = paddedHeight;
          }
        }
      }

      if ( !foundFont )
      {
        maxBlockSize.mNeededBlockSize = Vector2( paddedWidth, paddedHeight );
        maxBlockSize.mFontId = fontId;
        mBlockSizes.push_back( maxBlockSize );
      }
    }
  }

  void GenerateUnderlines( std::vector< MeshRecord>& meshRecords,
                           Vector< Extent >& extents,
                           const Vector4& underlineColor,
                           const Vector4& textColor )
  {
    MeshData newMeshData;
    const float zero = 0.0f;
    const float half = 0.5f;

    for ( Vector< Extent >::ConstIterator eIt = extents.Begin(); eIt != extents.End(); ++eIt )
    {
      MeshData::VertexContainer newVerts;
      newVerts.reserve( 4u );
      uint32_t index = eIt->mMeshRecordIndex;
      Vector2 uv = mGlyphManager.GetAtlasSize( meshRecords[ index ].mAtlasId );

      // Make sure we don't hit texture edge for single pixel texture ( filled pixel is in top left of every atlas )
      float u = half / uv.x;
      float v = half / uv.y;
      float thickness = eIt->mUnderlineThickness;
      float baseLine = eIt->mBaseLine - eIt->mUnderlinePosition - ( thickness * 0.5f );
      float tlx = eIt->mLeft;
      float brx = eIt->mRight;

      newVerts.push_back( MeshData::Vertex( Vector3( tlx, baseLine, zero ),
                                            Vector2( zero, zero ),
                                            Vector3( zero, zero, zero ) ) );

      newVerts.push_back( MeshData::Vertex( Vector3( brx, baseLine, zero ),
                                            Vector2( u, zero ),
                                            Vector3( zero, zero, zero ) ) );

      newVerts.push_back( MeshData::Vertex( Vector3( tlx, baseLine + thickness, zero ),
                                            Vector2( zero, v ),
                                            Vector3( zero, zero, zero ) ) );

      newVerts.push_back( MeshData::Vertex( Vector3( brx, baseLine + thickness, zero ),
                                            Vector2( u, v ),
                                            Vector3( zero, zero, zero ) ) );

      newMeshData.SetVertices( newVerts );
      newMeshData.SetFaceIndices( mFace );

      if ( underlineColor == textColor )
      {
        mGlyphManager.StitchMesh( meshRecords[ index ].mMeshData, newMeshData );
      }
      else
      {
        MeshRecord record;
        newMeshData.SetMaterial( meshRecords[ index ].mMeshData.GetMaterial() );
        newMeshData.SetHasNormals( true );
        newMeshData.SetHasColor( false );
        newMeshData.SetHasTextureCoords( true );
        record.mMeshData = newMeshData;
        record.mAtlasId = meshRecords[ index ].mAtlasId;
        record.mColor = underlineColor;
        record.mIsUnderline = true;
        meshRecords.push_back( record );
      }
    }
  }

  MeshActor GenerateShadow( MeshRecord& meshRecord,
                            const Vector2& shadowOffset,
                            const Vector4& shadowColor )
  {
    // Scan vertex buffer to determine width and height of effect buffer needed
    MeshData::VertexContainer verts = meshRecord.mMeshData.GetVertices();
    const float one = 1.0f;
    const float zero = 0.0f;
    float tlx = verts[ 0 ].x;
    float tly = verts[ 0 ].y;
    float brx = zero;
    float bry = zero;

    for ( uint32_t i = 0; i < verts.size(); ++i )
    {
      if ( verts[ i ].x < tlx )
      {
        tlx = verts[ i ].x;
      }
      if ( verts[ i ].y < tly )
      {
        tly = verts[ i ].y;
      }
      if ( verts[ i ].x > brx )
      {
        brx = verts[ i ].x;
      }
      if ( verts[ i ].y > bry )
      {
        bry = verts[ i ].y;
      }
    }

    float width = brx - tlx;
    float height = bry - tly;
    float divWidth = 2.0f / width;
    float divHeight = 2.0f / height;

    // Create a buffer to render to
    meshRecord.mBuffer = FrameBufferImage::New( width, height );

    // Create a mesh actor to contain the post-effect render
    MeshData::VertexContainer vertices;
    MeshData::FaceIndices face;

    vertices.push_back( MeshData::Vertex( Vector3( tlx + shadowOffset.x, tly + shadowOffset.y, zero ),
                                          Vector2( zero, zero ),
                                          Vector3( zero, zero, zero ) ) );

    vertices.push_back( MeshData::Vertex( Vector3( brx + shadowOffset.x, tly + shadowOffset.y, zero ),
                                          Vector2( one, zero ),
                                          Vector3( zero, zero, zero ) ) );

    vertices.push_back( MeshData::Vertex( Vector3( tlx + shadowOffset.x, bry + shadowOffset.y, zero ),
                                          Vector2( zero, one ),
                                          Vector3( zero, zero, zero ) ) );

    vertices.push_back( MeshData::Vertex( Vector3( brx + shadowOffset.x, bry + shadowOffset.y, zero ),
                                          Vector2( one, one ),
                                          Vector3( zero, zero, zero ) ) );

    MeshData meshData;
    Material newMaterial = Material::New("effect buffer");
    newMaterial.SetDiffuseTexture( meshRecord.mBuffer );
    meshData.SetMaterial( newMaterial );
    meshData.SetVertices( vertices );
    meshData.SetFaceIndices( mFace );
    meshData.SetHasNormals( true );
    meshData.SetHasColor( false );
    meshData.SetHasTextureCoords( true );
    MeshActor actor = MeshActor::New( Mesh::New( meshData ) );
    actor.SetColorMode( USE_OWN_MULTIPLY_PARENT_COLOR );
    actor.SetShaderEffect( mBgraShader );
    actor.SetFilterMode( FilterMode::LINEAR, FilterMode::LINEAR );
    actor.SetSortModifier( 0.1f ); // force behind main text

    // Create a sub actor to render once with normalized vertex positions
    MeshData newMeshData;
    MeshData::VertexContainer newVerts;
    MeshData::FaceIndices newFaces;
    MeshData::FaceIndices faces = meshRecord.mMeshData.GetFaces();
    for ( uint32_t i = 0; i < verts.size(); ++i )
    {
      MeshData::Vertex vertex = verts[ i ];
      vertex.x = ( ( vertex.x - tlx ) * divWidth ) - one;
      vertex.y = ( ( vertex.y - tly ) * divHeight ) - one;
      newVerts.push_back( vertex );
    }

    // Reverse triangle winding order
    uint32_t faceCount = faces.size() / 3;
    for ( uint32_t i = 0; i < faceCount; ++i )
    {
      uint32_t index = i * 3;
      newFaces.push_back( faces[ index + 2 ] );
      newFaces.push_back( faces[ index + 1 ] );
      newFaces.push_back( faces[ index ] );
    }

    newMeshData.SetMaterial( meshRecord.mMeshData.GetMaterial() );
    newMeshData.SetVertices( newVerts );
    newMeshData.SetFaceIndices( newFaces );
    newMeshData.SetHasNormals( true );
    newMeshData.SetHasColor( false );
    newMeshData.SetHasTextureCoords( true );

    MeshActor subActor = MeshActor::New( Mesh::New( newMeshData ) );
    subActor.SetColorMode( USE_OWN_MULTIPLY_PARENT_COLOR );
    subActor.SetColor( shadowColor );
    subActor.SetShaderEffect( mBasicShadowShader );
    subActor.SetFilterMode( FilterMode::NEAREST, FilterMode::NEAREST );

    // Create a render task to render the effect
    RenderTask task = Stage::GetCurrent().GetRenderTaskList().CreateTask();
    task.SetTargetFrameBuffer( meshRecord.mBuffer );
    task.SetSourceActor( subActor );
    task.SetClearEnabled( true );
    task.SetClearColor( Vector4::ZERO );
    task.SetExclusive( true );
    task.SetRefreshRate( RenderTask::REFRESH_ONCE );
    task.FinishedSignal().Connect( this, &AtlasRenderer::Impl::RenderComplete );
    actor.Add( subActor );
    return actor;
  }

  void RenderComplete( RenderTask& renderTask )
  {
    // Disconnect and remove this single shot render task
    renderTask.FinishedSignal().Disconnect( this, &AtlasRenderer::Impl::RenderComplete );
    Stage::GetCurrent().GetRenderTaskList().RemoveTask( renderTask );

    // Get the actor used for render to buffer and remove it from the parent
    Actor renderActor = renderTask.GetSourceActor();
    if ( renderActor )
    {
      Actor parent = renderActor.GetParent();
      if ( parent )
      {
        parent.Remove( renderActor );
      }
    }
  }

  RenderableActor mActor;                             ///< The actor parent which renders the text
  AtlasGlyphManager mGlyphManager;                    ///< Glyph Manager to handle upload and caching
  Vector< uint32_t > mImageIds;                       ///< A list of imageIDs used by the renderer
  TextAbstraction::FontClient mFontClient;            ///> The font client used to supply glyph information
  ShaderEffect mBasicShader;                          ///> Shader used to render L8 glyphs
  ShaderEffect mBgraShader;                           ///> Shader used to render BGRA glyphs
  ShaderEffect mBasicShadowShader;                    ///> Shader used to render drop shadow into buffer
  std::vector< MaxBlockSize > mBlockSizes;            ///> Maximum size needed to contain a glyph in a block within a new atlas
  std::vector< MeshData::FaceIndex > mFace;           ///> Face indices for a quad
};

Text::RendererPtr AtlasRenderer::New()
{
  return Text::RendererPtr( new AtlasRenderer() );
}

RenderableActor AtlasRenderer::Render( Text::ViewInterface& view )
{

  UnparentAndReset( mImpl->mActor );

  Text::Length numberOfGlyphs = view.GetNumberOfGlyphs();

  if( numberOfGlyphs > 0 )
  {
    Vector<GlyphInfo> glyphs;
    glyphs.Resize( numberOfGlyphs );

    view.GetGlyphs( &glyphs[0], 0, numberOfGlyphs );

    std::vector<Vector2> positions;
    positions.resize( numberOfGlyphs );
    view.GetGlyphPositions( &positions[0], 0, numberOfGlyphs );
    mImpl->AddGlyphs( positions,
                      glyphs,
                      view.GetTextColor(),
                      view.GetShadowOffset(),
                      view.GetShadowColor(),
                      view.IsUnderlineEnabled(),
                      view.GetUnderlineColor() );
  }
  return mImpl->mActor;
}

AtlasRenderer::AtlasRenderer()
{
  mImpl = new Impl();

}

AtlasRenderer::~AtlasRenderer()
{
  delete mImpl;
}
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
  const float ZERO( 0.0f );
  const float HALF( 0.5f );
  const float ONE( 1.0f );
  const float TWO( 2.0f );
  const uint32_t DEFAULT_ATLAS_WIDTH = 512u;
  const uint32_t DEFAULT_ATLAS_HEIGHT = 512u;
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
    uint32_t mNeededBlockWidth;
    uint32_t mNeededBlockHeight;
  };

  Impl()
  {
    mGlyphManager = AtlasGlyphManager::Get();
    mFontClient = TextAbstraction::FontClient::Get();
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
                  bool underlineEnabled,
                  const Vector4& underlineColor,
                  float underlineHeight )
  {
    AtlasManager::AtlasSlot slot;
    std::vector< MeshRecord > meshContainer;
    Vector< Extent > extents;

    float currentUnderlinePosition = ZERO;
    float currentUnderlineThickness = underlineHeight;
    uint32_t currentBlockSize = 0;
    FontId lastFontId = 0;
    Style style = STYLE_NORMAL;

    if ( fabsf( shadowOffset.x ) > Math::MACHINE_EPSILON_1 || fabsf( shadowOffset.y ) > Math::MACHINE_EPSILON_1 )
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
          currentUnderlinePosition = ceil( fabsf( fontMetrics.underlinePosition ) );
          float descender = ceil( fabsf( fontMetrics.descender ) );

          if ( underlineHeight == ZERO )
          {
            currentUnderlineThickness = fontMetrics.underlineThickness;

            // Ensure underline will be at least a pixel high
            if ( currentUnderlineThickness < ONE )
            {
              currentUnderlineThickness = ONE;
            }
            else
            {
              currentUnderlineThickness = ceil( currentUnderlineThickness );
            }
          }

          // Clamp the underline position at the font descender and check for ( as EFL describes it ) a broken font
          if ( currentUnderlinePosition > descender )
          {
            currentUnderlinePosition = descender;
          }
          if ( ZERO == currentUnderlinePosition )
          {
            // Move offset down by one ( EFL behavior )
            currentUnderlinePosition = ONE;
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
                currentBlockSize = j;
                mGlyphManager.SetNewAtlasSize( DEFAULT_ATLAS_WIDTH,
                                               DEFAULT_ATLAS_HEIGHT,
                                               mBlockSizes[ j ].mNeededBlockWidth,
                                               mBlockSizes[ j ].mNeededBlockHeight );
              }
            }
          }

          // Create a new image for the glyph
          BufferImage bitmap = mFontClient.CreateBitmap( glyph.fontId, glyph.index );

          // Ensure that the next image will fit into the current block size
          bool setSize = false;
          if ( bitmap.GetWidth() > mBlockSizes[ currentBlockSize ].mNeededBlockWidth )
          {
            setSize = true;
            mBlockSizes[ currentBlockSize ].mNeededBlockWidth = bitmap.GetWidth();
          }
          if ( bitmap.GetHeight() > mBlockSizes[ currentBlockSize ].mNeededBlockHeight )
          {
            setSize = true;
            mBlockSizes[ currentBlockSize ].mNeededBlockHeight = bitmap.GetHeight();
          }

          if ( setSize )
          {
            mGlyphManager.SetNewAtlasSize( DEFAULT_ATLAS_WIDTH,
                                           DEFAULT_ATLAS_HEIGHT,
                                           mBlockSizes[ currentBlockSize ].mNeededBlockWidth,
                                           mBlockSizes[ currentBlockSize ].mNeededBlockHeight );
          }

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
       lastFontId = glyph.fontId;
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

        // Ensure that text rendering is unfiltered
        actor.SetFilterMode( FilterMode::NEAREST, FilterMode::NEAREST );
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
                                                 metrics.mAtlasMetrics.mAtlasMetrics[ i ].mSize.mWidth,
                                                 metrics.mAtlasMetrics.mAtlasMetrics[ i ].mSize.mHeight,
                                                 metrics.mAtlasMetrics.mAtlasMetrics[ i ].mSize.mBlockWidth,
                                                 metrics.mAtlasMetrics.mAtlasMetrics[ i ].mSize.mBlockHeight,
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
        if ( slot.mAtlasId == mIt->mAtlasId && color == mIt->mColor )
        {
          // Stitch the mesh to the existing mesh and adjust any extents
          mGlyphManager.StitchMesh( mIt->mMeshData, newMeshData );
          AdjustExtents( extents,
                         meshContainer,
                         index,
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
        foundExtent = true;
        if ( left < eIt->mLeft )
        {
          eIt->mLeft = left;
        }
        if ( right > eIt->mRight  )
        {
          eIt->mRight = right;
        }

        if ( underlinePosition > eIt->mUnderlinePosition )
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
      FontId fontId = glyphs[ i ].fontId;
      bool foundFont = false;
      for ( uint32_t j = 0; j < mBlockSizes.size(); ++j )
      {
        if ( mBlockSizes[ j ].mFontId == fontId )
        {
          foundFont = true;
        }
      }
      if ( !foundFont )
      {
        FontMetrics fontMetrics;
        mFontClient.GetFontMetrics( fontId, fontMetrics );
        maxBlockSize.mNeededBlockWidth = static_cast< uint32_t >( fontMetrics.height );
        maxBlockSize.mNeededBlockHeight = static_cast< uint32_t >( fontMetrics.height );
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
    for ( Vector< Extent >::ConstIterator eIt = extents.Begin(); eIt != extents.End(); ++eIt )
    {
      MeshData::VertexContainer newVerts;
      newVerts.reserve( 4u );
      uint32_t index = eIt->mMeshRecordIndex;
      Vector2 uv = mGlyphManager.GetAtlasSize( meshRecords[ index ].mAtlasId );

      // Make sure we don't hit texture edge for single pixel texture ( filled pixel is in top left of every atlas )
      float u = HALF / uv.x;
      float v = HALF / uv.y;
      float thickness = eIt->mUnderlineThickness;
      float baseLine = eIt->mBaseLine + eIt->mUnderlinePosition - ( thickness * HALF );
      float tlx = eIt->mLeft;
      float brx = eIt->mRight;

      newVerts.push_back( MeshData::Vertex( Vector3( tlx, baseLine, ZERO ),
                                            Vector2::ZERO,
                                            Vector3::ZERO ) );

      newVerts.push_back( MeshData::Vertex( Vector3( brx, baseLine, ZERO ),
                                            Vector2( u, ZERO ),
                                            Vector3::ZERO ) );

      newVerts.push_back( MeshData::Vertex( Vector3( tlx, baseLine + thickness, ZERO ),
                                            Vector2( ZERO, v ),
                                            Vector3::ZERO ) );

      newVerts.push_back( MeshData::Vertex( Vector3( brx, baseLine + thickness, ZERO ),
                                            Vector2( u, v ),
                                            Vector3::ZERO ) );

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
    float tlx = verts[ 0 ].x;
    float tly = verts[ 0 ].y;
    float brx = ZERO;
    float bry = ZERO;

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
    float divWidth = TWO / width;
    float divHeight = TWO / height;

    // Create a buffer to render to
    meshRecord.mBuffer = FrameBufferImage::New( width, height );

    // Create a mesh actor to contain the post-effect render
    MeshData::VertexContainer vertices;
    MeshData::FaceIndices face;

    vertices.push_back( MeshData::Vertex( Vector3( tlx + shadowOffset.x, tly + shadowOffset.y, ZERO ),
                                          Vector2::ZERO,
                                          Vector3::ZERO ) );

    vertices.push_back( MeshData::Vertex( Vector3( brx + shadowOffset.x, tly + shadowOffset.y, ZERO ),
                                          Vector2( ONE, ZERO ),
                                          Vector3::ZERO ) );

    vertices.push_back( MeshData::Vertex( Vector3( tlx + shadowOffset.x, bry + shadowOffset.y, ZERO ),
                                          Vector2( ZERO, ONE ),
                                          Vector3::ZERO ) );

    vertices.push_back( MeshData::Vertex( Vector3( brx + shadowOffset.x, bry + shadowOffset.y, ZERO ),
                                          Vector2::ONE,
                                          Vector3::ZERO ) );

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
      vertex.x = ( ( vertex.x - tlx ) * divWidth ) - ONE;
      vertex.y = ( ( vertex.y - tly ) * divHeight ) - ONE;
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
                      view.GetUnderlineColor(),
                      view.GetUnderlineHeight() );
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
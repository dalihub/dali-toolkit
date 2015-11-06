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
#include <dali/integration-api/debug.h>
#include <dali/devel-api/rendering/renderer.h>
#include <dali/devel-api/rendering/geometry.h>
#include <dali/devel-api/text-abstraction/font-client.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/internal/text/glyph-run.h>
#include <dali-toolkit/internal/text/rendering/atlas/atlas-glyph-manager.h>
#include <dali-toolkit/internal/text/rendering/atlas/atlas-mesh-factory.h>
#include <dali-toolkit/internal/text/text-view.h>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Toolkit::Text;

namespace
{
#if defined(DEBUG_ENABLED)
  Debug::Filter* gLogFilter = Debug::Filter::New(Debug::Concise, true, "LOG_TEXT_RENDERING");
#endif

const float ZERO( 0.0f );
const float HALF( 0.5f );
const float ONE( 1.0f );
const uint32_t DEFAULT_ATLAS_WIDTH = 512u;
const uint32_t DEFAULT_ATLAS_HEIGHT = 512u;
}

struct AtlasRenderer::Impl
{
  enum Style
  {
    STYLE_NORMAL,
    STYLE_DROP_SHADOW
  };

  struct MeshRecord
  {
    MeshRecord()
    : mColor( Color::BLACK ),
      mAtlasId( 0 )
    {
    }

    Vector4 mColor;
    uint32_t mAtlasId;
    AtlasManager::Mesh2D mMesh;
    FrameBufferImage mBuffer;
  };

  /**
   * brief Struct used to generate the underline mesh.
   * There is one Extent per line of text.
   */
  struct Extent
  {
    Extent()
    : mBaseLine( 0.0f ),
      mLeft( 0.0f ),
      mRight( 0.0f ),
      mUnderlinePosition( 0.0f ),
      mUnderlineThickness( 0.0f ),
      mMeshRecordIndex( 0 )
    {
    }

    float mBaseLine;
    float mLeft;
    float mRight;
    float mUnderlinePosition;
    float mUnderlineThickness;
    uint32_t mMeshRecordIndex;
  };

  struct MaxBlockSize
  {
    MaxBlockSize()
    : mFontId( 0 ),
      mNeededBlockWidth( 0 ),
      mNeededBlockHeight( 0 )
    {
    }

    FontId mFontId;
    uint32_t mNeededBlockWidth;
    uint32_t mNeededBlockHeight;
  };

  struct CheckEntry
  {
    CheckEntry()
    : mFontId( 0 ),
      mIndex( 0 )
    {
    }

    FontId mFontId;
    Text::GlyphIndex mIndex;
  };

  struct TextCacheEntry
  {
    TextCacheEntry()
    : mFontId( 0 ),
      mIndex( 0 ),
      mImageId( 0 )
    {
    }

    FontId mFontId;
    Text::GlyphIndex mIndex;
    uint32_t mImageId;
  };

  Impl()
  : mDepth( 0 )
  {
    mGlyphManager = AtlasGlyphManager::Get();
    mFontClient = TextAbstraction::FontClient::Get();

    mQuadVertexFormat[ "aPosition" ] = Property::VECTOR2;
    mQuadVertexFormat[ "aTexCoord" ] = Property::VECTOR2;
    mQuadIndexFormat[ "indices" ] = Property::INTEGER;
  }

  bool IsGlyphUnderlined( GlyphIndex index,
                          const Vector<GlyphRun>& underlineRuns )
  {
    for( Vector<GlyphRun>::ConstIterator it = underlineRuns.Begin(),
           endIt = underlineRuns.End();
           it != endIt;
         ++it )
    {
      const GlyphRun& run = *it;

      if( ( run.glyphIndex <= index ) && ( index < run.glyphIndex + run.numberOfGlyphs ) )
      {
        return true;
      }
    }

    return false;
  }

  void AddGlyphs( Text::ViewInterface& view,
                  const Vector<Vector2>& positions,
                  const Vector<GlyphInfo>& glyphs,
                  int depth )
  {
    AtlasManager::AtlasSlot slot;
    std::vector< MeshRecord > meshContainer;
    Vector< Extent > extents;
    TextCacheEntry textCacheEntry;
    mDepth = depth;

    const Vector2& actorSize( view.GetControlSize() );
    const Vector2 halfActorSize( actorSize * 0.5f );
    const Vector4& textColor( view.GetTextColor() );
    const Vector2& shadowOffset( view.GetShadowOffset() );
    const Vector4& shadowColor( view.GetShadowColor() );
    const bool underlineEnabled( view.IsUnderlineEnabled() );
    const Vector4& underlineColor( view.GetUnderlineColor() );
    const float underlineHeight( view.GetUnderlineHeight() );

    // Get the underline runs.
    const Length numberOfUnderlineRuns = view.GetNumberOfUnderlineRuns();
    Vector<GlyphRun> underlineRuns;
    underlineRuns.Resize( numberOfUnderlineRuns );
    view.GetUnderlineRuns( underlineRuns.Begin(),
                           0u,
                           numberOfUnderlineRuns );

    bool thereAreUnderlinedGlyphs = false;

    float currentUnderlinePosition = ZERO;
    float currentUnderlineThickness = underlineHeight;
    uint32_t currentBlockSize = 0;
    FontId lastFontId = 0;
    FontId lastUnderlinedFontId = 0;
    Style style = STYLE_NORMAL;

    if ( fabsf( shadowOffset.x ) > Math::MACHINE_EPSILON_1 || fabsf( shadowOffset.y ) > Math::MACHINE_EPSILON_1 )
    {
      style = STYLE_DROP_SHADOW;
    }

    CalculateBlocksSize( glyphs );

    // Avoid emptying mTextCache (& removing references) until after incremented references for the new text
    Vector< TextCacheEntry > newTextCache;
    const GlyphInfo* const glyphsBuffer = glyphs.Begin();
    const Vector2* const positionsBuffer = positions.Begin();

    for( uint32_t i = 0, glyphSize = glyphs.Size(); i < glyphSize; ++i )
    {
      const GlyphInfo& glyph = *( glyphsBuffer + i );

      const bool underlineGlyph = underlineEnabled || IsGlyphUnderlined( i, underlineRuns );
      thereAreUnderlinedGlyphs = thereAreUnderlinedGlyphs || underlineGlyph;

      // No operation for white space
      if ( glyph.width && glyph.height )
      {
        // Are we still using the same fontId as previous
        if ( underlineGlyph && ( glyph.fontId != lastUnderlinedFontId ) )
        {
          // We need to fetch fresh font underline metrics
          FontMetrics fontMetrics;
          mFontClient.GetFontMetrics( glyph.fontId, fontMetrics );
          currentUnderlinePosition = ceil( fabsf( fontMetrics.underlinePosition ) );
          const float descender = ceil( fabsf( fontMetrics.descender ) );

          if( fabsf( underlineHeight ) < Math::MACHINE_EPSILON_1000 )
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
          if( fabsf( currentUnderlinePosition ) < Math::MACHINE_EPSILON_1000 )
          {
            // Move offset down by one ( EFL behavior )
            currentUnderlinePosition = ONE;
          }

          lastUnderlinedFontId = glyph.fontId;
        } // underline

        if ( !mGlyphManager.IsCached( glyph.fontId, glyph.index, slot ) )
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
          if ( bitmap )
          {
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
          }
        }
        else
        {
          // We have 2+ copies of the same glyph
          mGlyphManager.AdjustReferenceCount( glyph.fontId, glyph.index, 1/*increment*/ );
        }

        // Move the origin (0,0) of the mesh to the center of the actor
        Vector2 position = *( positionsBuffer + i ) - halfActorSize;

        // Generate mesh data for this quad, plugging in our supplied position
        AtlasManager::Mesh2D newMesh;
        mGlyphManager.GenerateMeshData( slot.mImageId, position, newMesh );
        textCacheEntry.mFontId = glyph.fontId;
        textCacheEntry.mImageId = slot.mImageId;
        textCacheEntry.mIndex = glyph.index;
        newTextCache.PushBack( textCacheEntry );

        // Adjust the vertices if the fixed-size font should be down-scaled
        if( glyph.scaleFactor > 0 )
        {
          for( unsigned int i=0; i<newMesh.mVertices.Count(); ++i )
          {
            newMesh.mVertices[i].mPosition.x = position.x + ( ( newMesh.mVertices[i].mPosition.x - position.x ) * glyph.scaleFactor );
            newMesh.mVertices[i].mPosition.y = position.y + ( ( newMesh.mVertices[i].mPosition.y - position.y ) * glyph.scaleFactor );
          }
        }

        // Find an existing mesh data object to attach to ( or create a new one, if we can't find one using the same atlas)
        StitchTextMesh( meshContainer,
                        newMesh,
                        extents,
                        textColor,
                        position.y + glyph.yBearing,
                        underlineGlyph,
                        currentUnderlinePosition,
                        currentUnderlineThickness,
                        slot );
        lastFontId = glyph.fontId;
      }
    } // glyphs

    // Now remove references for the old text
    RemoveText();
    mTextCache.Swap( newTextCache );

    if( thereAreUnderlinedGlyphs )
    {
      // Check to see if any of the text needs an underline
      GenerateUnderlines( meshContainer, extents, underlineColor, textColor );
    }

    // For each MeshData object, create a mesh actor and add to the renderable actor
    if ( meshContainer.size() )
    {
      for ( std::vector< MeshRecord >::iterator mIt = meshContainer.begin(); mIt != meshContainer.end(); ++mIt )
      {
        Actor actor = CreateMeshActor( *mIt, actorSize );

        // Create an effect if necessary
        if ( style == STYLE_DROP_SHADOW )
        {
          // Create a container actor to act as a common parent for text and shadow, to avoid color inheritence issues.
          Actor containerActor = Actor::New();
          containerActor.SetParentOrigin( ParentOrigin::CENTER );
          containerActor.SetSize( actorSize );

          Actor shadowActor = Actor::New();
#if defined(DEBUG_ENABLED)
          shadowActor.SetName( "Text Shadow renderable actor" );
#endif
          // Offset shadow in x and y
          shadowActor.RegisterProperty("uOffset", shadowOffset );
          if ( actor.GetRendererCount() )
          {
            Dali::Renderer renderer( actor.GetRendererAt( 0 ) );
            Geometry geometry = renderer.GetGeometry();
            Material material = renderer.GetMaterial();

            Dali::Renderer shadowRenderer = Dali::Renderer::New( geometry, material );
            shadowRenderer.SetDepthIndex( renderer.GetDepthIndex() - 1 );
            shadowActor.AddRenderer( shadowRenderer );
            shadowActor.SetParentOrigin( ParentOrigin::CENTER );
            shadowActor.SetSize( actorSize );
            shadowActor.SetColor( shadowColor );
            containerActor.Add( shadowActor );
            containerActor.Add( actor );
            actor = containerActor;
          }
        }

        if( mActor )
        {
          mActor.Add( actor );
        }
        else
        {
          mActor = actor;
        }
      }
    }
#if defined(DEBUG_ENABLED)
    Toolkit::AtlasGlyphManager::Metrics metrics = mGlyphManager.GetMetrics();
    DALI_LOG_INFO( gLogFilter, Debug::General, "TextAtlasRenderer::GlyphManager::GlyphCount: %i, AtlasCount: %i, TextureMemoryUse: %iK\n",
                                                metrics.mGlyphCount,
                                                metrics.mAtlasMetrics.mAtlasCount,
                                                metrics.mAtlasMetrics.mTextureMemoryUsed / 1024 );

    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "%s\n", metrics.mVerboseGlyphCounts.c_str() );

    for ( uint32_t i = 0; i < metrics.mAtlasMetrics.mAtlasCount; ++i )
    {
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "   Atlas [%i] %sPixels: %s Size: %ix%i, BlockSize: %ix%i, BlocksUsed: %i/%i\n",
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

  void RemoveText()
  {
    for ( Vector< TextCacheEntry >::Iterator oldTextIter = mTextCache.Begin(); oldTextIter != mTextCache.End(); ++oldTextIter )
    {
      mGlyphManager.AdjustReferenceCount( oldTextIter->mFontId, oldTextIter->mIndex, -1/*decrement*/ );
    }
    mTextCache.Resize( 0 );
  }

  Actor CreateMeshActor( const MeshRecord& meshRecord, const Vector2& actorSize )
  {
    PropertyBuffer quadVertices = PropertyBuffer::New( mQuadVertexFormat, meshRecord.mMesh.mVertices.Size() );
    PropertyBuffer quadIndices = PropertyBuffer::New( mQuadIndexFormat, meshRecord.mMesh.mIndices.Size() );
    quadVertices.SetData( const_cast< AtlasManager::Vertex2D* >( &meshRecord.mMesh.mVertices[ 0 ] ) );
    quadIndices.SetData( const_cast< unsigned int* >( &meshRecord.mMesh.mIndices[ 0 ] ) );

    Geometry quadGeometry = Geometry::New();
    quadGeometry.AddVertexBuffer( quadVertices );
    quadGeometry.SetIndexBuffer( quadIndices );

    Material material = mGlyphManager.GetMaterial( meshRecord.mAtlasId );
    Dali::Renderer renderer = Dali::Renderer::New( quadGeometry, material );
    renderer.SetDepthIndex( CONTENT_DEPTH_INDEX + mDepth );
    Actor actor = Actor::New();
#if defined(DEBUG_ENABLED)
    actor.SetName( "Text renderable actor" );
#endif
    actor.AddRenderer( renderer );
    actor.SetParentOrigin( ParentOrigin::CENTER ); // Keep all of the origins aligned
    actor.SetSize( actorSize );
    actor.SetColor( meshRecord.mColor );
    actor.RegisterProperty("uOffset", Vector2::ZERO );
    return actor;
  }

  void StitchTextMesh( std::vector< MeshRecord >& meshContainer,
                       AtlasManager::Mesh2D& newMesh,
                       Vector< Extent >& extents,
                       const Vector4& color,
                       float baseLine,
                       bool underlineGlyph,
                       float underlinePosition,
                       float underlineThickness,
                       AtlasManager::AtlasSlot& slot )
  {
    if ( slot.mImageId )
    {
      float left = newMesh.mVertices[ 0 ].mPosition.x;
      float right = newMesh.mVertices[ 1 ].mPosition.x;

      // Check to see if there's a mesh data object that references the same atlas ?
      uint32_t index = 0;
      for ( std::vector< MeshRecord >::iterator mIt = meshContainer.begin(),
              mEndIt = meshContainer.end();
            mIt != mEndIt;
            ++mIt, ++index )
      {
        if ( slot.mAtlasId == mIt->mAtlasId && color == mIt->mColor )
        {
          // Append the mesh to the existing mesh and adjust any extents
          Toolkit::Internal::AtlasMeshFactory::AppendMesh( mIt->mMesh, newMesh );

          if( underlineGlyph )
          {
            AdjustExtents( extents,
                           meshContainer,
                           index,
                           left,
                           right,
                           baseLine,
                           underlinePosition,
                           underlineThickness );
          }

          return;
        }
      }

      // No mesh data object currently exists that references this atlas, so create a new one
      MeshRecord meshRecord;
      meshRecord.mAtlasId = slot.mAtlasId;
      meshRecord.mMesh = newMesh;
      meshRecord.mColor = color;
      meshContainer.push_back( meshRecord );

      if( underlineGlyph )
      {
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
    for ( Vector< Extent >::Iterator eIt = extents.Begin(),
            eEndIt = extents.End();
          eIt != eEndIt;
          ++eIt )
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

  void GenerateUnderlines( std::vector< MeshRecord >& meshRecords,
                           Vector< Extent >& extents,
                           const Vector4& underlineColor,
                           const Vector4& textColor )
  {
    AtlasManager::Mesh2D newMesh;
    unsigned short faceIndex = 0;
    for ( Vector< Extent >::ConstIterator eIt = extents.Begin(),
            eEndIt = extents.End();
          eIt != eEndIt;
          ++eIt )
    {
      AtlasManager::Vertex2D vert;
      uint32_t index = eIt->mMeshRecordIndex;
      Vector2 uv = mGlyphManager.GetAtlasSize( meshRecords[ index ].mAtlasId );

      // Make sure we don't hit texture edge for single pixel texture ( filled pixel is in top left of every atlas )
      float u = HALF / uv.x;
      float v = HALF / uv.y;
      float thickness = eIt->mUnderlineThickness;
      float baseLine = eIt->mBaseLine + eIt->mUnderlinePosition - ( thickness * HALF );
      float tlx = eIt->mLeft;
      float brx = eIt->mRight;

      vert.mPosition.x = tlx;
      vert.mPosition.y = baseLine;
      vert.mTexCoords.x = ZERO;
      vert.mTexCoords.y = ZERO;
      newMesh.mVertices.PushBack( vert );

      vert.mPosition.x = brx;
      vert.mPosition.y = baseLine;
      vert.mTexCoords.x = u;
      newMesh.mVertices.PushBack( vert );

      vert.mPosition.x = tlx;
      vert.mPosition.y = baseLine + thickness;
      vert.mTexCoords.x = ZERO;
      vert.mTexCoords.y = v;
      newMesh.mVertices.PushBack( vert );

      vert.mPosition.x = brx;
      vert.mPosition.y = baseLine + thickness;
      vert.mTexCoords.x = u;
      newMesh.mVertices.PushBack( vert );

      // Six indices in counter clockwise winding
      newMesh.mIndices.PushBack( faceIndex + 1u );
      newMesh.mIndices.PushBack( faceIndex );
      newMesh.mIndices.PushBack( faceIndex + 2u );
      newMesh.mIndices.PushBack( faceIndex + 2u );
      newMesh.mIndices.PushBack( faceIndex + 3u );
      newMesh.mIndices.PushBack( faceIndex + 1u );
      faceIndex += 4;

      if ( underlineColor == textColor )
      {
        Toolkit::Internal::AtlasMeshFactory::AppendMesh( meshRecords[ index ].mMesh, newMesh );
      }
      else
      {
        MeshRecord record;
        record.mMesh = newMesh;
        record.mAtlasId = meshRecords[ index ].mAtlasId;
        record.mColor = underlineColor;
        meshRecords.push_back( record );
      }
    }
  }

  Actor mActor;                                       ///< The actor parent which renders the text
  AtlasGlyphManager mGlyphManager;                    ///< Glyph Manager to handle upload and caching
  TextAbstraction::FontClient mFontClient;            ///> The font client used to supply glyph information
  std::vector< MaxBlockSize > mBlockSizes;            ///> Maximum size needed to contain a glyph in a block within a new atlas
  Vector< TextCacheEntry > mTextCache;                ///> Caches data from previous render
  Property::Map mQuadVertexFormat;                    ///> Describes the vertex format for text
  Property::Map mQuadIndexFormat;                     ///> Describes the index format for text
  int mDepth;                                         ///> DepthIndex passed by control when connect to stage
};

Text::RendererPtr AtlasRenderer::New()
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Text::AtlasRenderer::New()\n" );

  return Text::RendererPtr( new AtlasRenderer() );
}

Actor AtlasRenderer::Render( Text::ViewInterface& view, int depth )
{
  UnparentAndReset( mImpl->mActor );

  Length numberOfGlyphs = view.GetNumberOfGlyphs();

  if( numberOfGlyphs > 0u )
  {
    Vector<GlyphInfo> glyphs;
    glyphs.Resize( numberOfGlyphs );

    Vector<Vector2> positions;
    positions.Resize( numberOfGlyphs );

    numberOfGlyphs = view.GetGlyphs( glyphs.Begin(),
                                     positions.Begin(),
                                     0u,
                                     numberOfGlyphs );
    glyphs.Resize( numberOfGlyphs );
    positions.Resize( numberOfGlyphs );

    mImpl->AddGlyphs( view,
                      positions,
                      glyphs,
                      depth );
  }

  return mImpl->mActor;
}

AtlasRenderer::AtlasRenderer()
{
  mImpl = new Impl();

}

AtlasRenderer::~AtlasRenderer()
{
  mImpl->RemoveText();
  delete mImpl;
}

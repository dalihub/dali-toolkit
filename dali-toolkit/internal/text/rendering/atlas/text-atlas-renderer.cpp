/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/rendering/geometry.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/animation/constraints.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
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
  Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_RENDERING");
#endif

#define MAKE_SHADER(A)#A

const char* VERTEX_SHADER = MAKE_SHADER(
attribute mediump vec2    aPosition;
attribute mediump vec2    aTexCoord;
attribute mediump vec4    aColor;
uniform   mediump vec2    uOffset;
uniform     highp mat4    uMvpMatrix;
varying   mediump vec2    vTexCoord;
varying   mediump vec4    vColor;

void main()
{
  mediump vec4 position = vec4( aPosition.xy + uOffset, 0.0, 1.0 );
  gl_Position = uMvpMatrix * position;
  vTexCoord = aTexCoord;
  vColor = aColor;
}
);

const char* FRAGMENT_SHADER_L8 = MAKE_SHADER(
uniform lowp    vec4      uColor;
uniform lowp    vec4      textColorAnimatable;
uniform         sampler2D sTexture;
varying mediump vec2      vTexCoord;
varying mediump vec4      vColor;

void main()
{
  mediump vec4 color = texture2D( sTexture, vTexCoord );
  gl_FragColor = vec4( vColor.rgb * uColor.rgb * textColorAnimatable.rgb, uColor.a * vColor.a * textColorAnimatable.a * color.r );
}
);

const char* FRAGMENT_SHADER_RGBA = MAKE_SHADER(
uniform lowp    vec4      uColor;
uniform lowp    vec4      textColorAnimatable;
uniform         sampler2D sTexture;
varying mediump vec2      vTexCoord;

void main()
{
  gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor * textColorAnimatable;
}
);

const float ZERO( 0.0f );
const float HALF( 0.5f );
const float ONE( 1.0f );
const uint32_t DEFAULT_ATLAS_WIDTH = 512u;
const uint32_t DEFAULT_ATLAS_HEIGHT = 512u;
const uint16_t NO_OUTLINE = 0u;
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
    : mAtlasId( 0u )
    {
    }

    uint32_t mAtlasId;
    AtlasManager::Mesh2D mMesh;
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
      mMeshRecordIndex( 0u )
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
    : mFontId{ 0u },
      mIndex{ 0u },
      mImageId{ 0u },
      mOutlineWidth{ 0u },
      isItalic{ false },
      isBold{ false }
    {
    }

    FontId mFontId;
    Text::GlyphIndex mIndex;
    uint32_t mImageId;
    uint16_t mOutlineWidth;
    bool isItalic:1;
    bool isBold:1;
  };

  Impl()
  : mDepth( 0 )
  {
    mGlyphManager = AtlasGlyphManager::Get();
    mFontClient = TextAbstraction::FontClient::Get();

    mQuadVertexFormat[ "aPosition" ] = Property::VECTOR2;
    mQuadVertexFormat[ "aTexCoord" ] = Property::VECTOR2;
    mQuadVertexFormat[ "aColor" ] = Property::VECTOR4;
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

  void CacheGlyph( const GlyphInfo& glyph, FontId lastFontId, const AtlasGlyphManager::GlyphStyle& style, AtlasManager::AtlasSlot& slot )
  {
    const bool glyphNotCached = !mGlyphManager.IsCached( glyph.fontId, glyph.index, style, slot );  // Check FontGlyphRecord vector for entry with glyph index and fontId

    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "AddGlyphs fontID[%u] glyphIndex[%u] [%s]\n", glyph.fontId, glyph.index, (glyphNotCached)?"not cached":"cached" );

    if( glyphNotCached )
    {
      MaxBlockSize& blockSize = mBlockSizes[0u];

      if ( lastFontId != glyph.fontId )
      {
        uint32_t index = 0u;
        // Looks through all stored block sizes until finds the one which mataches required glyph font it.  Ensures new atlas block size will match existing for same font id.
        // CalculateBlocksSize() above ensures a block size entry exists.
        for( std::vector<MaxBlockSize>::const_iterator it = mBlockSizes.begin(),
               endIt = mBlockSizes.end();
             it != endIt;
             ++it, ++index )
        {
          const MaxBlockSize& blockSizeEntry = *it;
          if( blockSizeEntry.mFontId == glyph.fontId )
          {
            blockSize = mBlockSizes[index];
          }
        }
      }

      // Create a new image for the glyph
      PixelData bitmap;

      // Whether the glyph is an outline.
      const bool isOutline = 0u != style.outline;

      // Whether the current glyph is a color one.
      const bool isColorGlyph = mFontClient.IsColorGlyph( glyph.fontId, glyph.index );

      if( !isOutline || ( isOutline && !isColorGlyph) )
      {
        // Retrieve the emoji's bitmap.
        TextAbstraction::FontClient::GlyphBufferData glyphBufferData;
        glyphBufferData.width = isColorGlyph ? glyph.width : 0;   // Desired width and height.
        glyphBufferData.height = isColorGlyph ? glyph.height : 0;

        mFontClient.CreateBitmap( glyph.fontId,
                                  glyph.index,
                                  glyph.isItalicRequired,
                                  glyph.isBoldRequired,
                                  glyphBufferData,
                                  style.outline );

        // Create the pixel data.
        bitmap = PixelData::New( glyphBufferData.buffer,
                                 glyphBufferData.width * glyphBufferData.height * GetBytesPerPixel( glyphBufferData.format ),
                                 glyphBufferData.width,
                                 glyphBufferData.height,
                                 glyphBufferData.format,
                                 PixelData::DELETE_ARRAY );

        if( bitmap )
        {
          // Ensure that the next image will fit into the current block size
          if( bitmap.GetWidth() > blockSize.mNeededBlockWidth )
          {
            blockSize.mNeededBlockWidth = bitmap.GetWidth();
          }

          if( bitmap.GetHeight() > blockSize.mNeededBlockHeight )
          {
            blockSize.mNeededBlockHeight = bitmap.GetHeight();
          }

          // If CheckAtlas in AtlasManager::Add can't fit the bitmap in the current atlas it will create a new atlas

          // Setting the block size and size of new atlas does not mean a new one will be created. An existing atlas may still surffice.
          mGlyphManager.SetNewAtlasSize( DEFAULT_ATLAS_WIDTH,
                                         DEFAULT_ATLAS_HEIGHT,
                                         blockSize.mNeededBlockWidth,
                                         blockSize.mNeededBlockHeight );

          // Locate a new slot for our glyph
          mGlyphManager.Add( glyph, style, bitmap, slot ); // slot will be 0 is glyph not added
        }
      }
    }
    else
    {
      // We have 2+ copies of the same glyph
      mGlyphManager.AdjustReferenceCount( glyph.fontId, glyph.index, style, 1 ); //increment
    }
  }

  void GenerateMesh( const GlyphInfo& glyph,
                     const Vector2& position,
                     const Vector4& color,
                     uint16_t outline,
                     AtlasManager::AtlasSlot& slot,
                     bool underlineGlyph,
                     float currentUnderlinePosition,
                     float currentUnderlineThickness,
                     std::vector<MeshRecord>& meshContainer,
                     Vector<TextCacheEntry>& newTextCache,
                     Vector<Extent>& extents )
  {
    // Generate mesh data for this quad, plugging in our supplied position
    AtlasManager::Mesh2D newMesh;
    mGlyphManager.GenerateMeshData( slot.mImageId, position, newMesh );

    TextCacheEntry textCacheEntry;
    textCacheEntry.mFontId = glyph.fontId;
    textCacheEntry.mImageId = slot.mImageId;
    textCacheEntry.mIndex = glyph.index;
    textCacheEntry.mOutlineWidth = outline;
    textCacheEntry.isItalic = glyph.isItalicRequired;
    textCacheEntry.isBold = glyph.isBoldRequired;

    newTextCache.PushBack( textCacheEntry );

    AtlasManager::Vertex2D* verticesBuffer = newMesh.mVertices.Begin();

    for( unsigned int index = 0u, size = newMesh.mVertices.Count();
         index < size;
         ++index )
    {
      AtlasManager::Vertex2D& vertex = *( verticesBuffer + index );

      // Set the color of the vertex.
      vertex.mColor = color;
    }

    // Find an existing mesh data object to attach to ( or create a new one, if we can't find one using the same atlas)
    StitchTextMesh( meshContainer,
                    newMesh,
                    extents,
                    position.y + glyph.yBearing,
                    underlineGlyph,
                    currentUnderlinePosition,
                    currentUnderlineThickness,
                    slot );
  }

  void CreateActors( const std::vector<MeshRecord>& meshContainer,
                     const Size& textSize,
                     const Vector4& color,
                     const Vector4& shadowColor,
                     const Vector2& shadowOffset,
                     Actor textControl,
                     Property::Index animatablePropertyIndex,
                     bool drawShadow )
  {
    if( !mActor )
    {
      // Create a container actor to act as a common parent for text and shadow, to avoid color inheritence issues.
      mActor = Actor::New();
      mActor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
      mActor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
      mActor.SetSize( textSize );
      mActor.SetProperty( Actor::Property::COLOR_MODE, USE_OWN_MULTIPLY_PARENT_COLOR );
    }

    for( std::vector< MeshRecord >::const_iterator it = meshContainer.begin(),
           endIt = meshContainer.end();
         it != endIt; ++it )
    {
      const MeshRecord& meshRecord = *it;

      Actor actor = CreateMeshActor( textControl, animatablePropertyIndex, color, meshRecord, textSize, STYLE_NORMAL );

      // Whether the actor has renderers.
      const bool hasRenderer = actor.GetRendererCount() > 0u;

      // Create an effect if necessary
      if( hasRenderer &&
          drawShadow )
      {
        // Change the color of the vertices.
        for( Vector<AtlasManager::Vertex2D>::Iterator vIt =  meshRecord.mMesh.mVertices.Begin(),
               vEndIt = meshRecord.mMesh.mVertices.End();
             vIt != vEndIt;
             ++vIt )
        {
          AtlasManager::Vertex2D& vertex = *vIt;

          vertex.mColor = shadowColor;
        }

        Actor shadowActor = CreateMeshActor(textControl, animatablePropertyIndex, color, meshRecord, textSize, STYLE_DROP_SHADOW );
#if defined(DEBUG_ENABLED)
        shadowActor.SetProperty( Dali::Actor::Property::NAME, "Text Shadow renderable actor" );
#endif
        // Offset shadow in x and y
        shadowActor.RegisterProperty("uOffset", shadowOffset );
        Dali::Renderer renderer( shadowActor.GetRendererAt( 0 ) );
        int depthIndex = renderer.GetProperty<int>(Dali::Renderer::Property::DEPTH_INDEX);
        renderer.SetProperty( Dali::Renderer::Property::DEPTH_INDEX, depthIndex - 1 );
        mActor.Add( shadowActor );
      }

      if( hasRenderer )
      {
        mActor.Add( actor );
      }
    }
  }

  void AddGlyphs( Text::ViewInterface& view,
                  Actor textControl,
                  Property::Index animatablePropertyIndex,
                  const Vector<Vector2>& positions,
                  const Vector<GlyphInfo>& glyphs,
                  const Vector4& defaultColor,
                  const Vector4* const colorsBuffer,
                  const ColorIndex* const colorIndicesBuffer,
                  int depth,
                  float minLineOffset )
  {
    AtlasManager::AtlasSlot slot;
    slot.mImageId = 0u;
    slot.mAtlasId = 0u;

    AtlasManager::AtlasSlot slotOutline;
    slotOutline.mImageId = 0u;
    slotOutline.mAtlasId = 0u;

    std::vector< MeshRecord > meshContainer;
    std::vector< MeshRecord > meshContainerOutline;
    Vector< Extent > extents;
    mDepth = depth;

    const Vector2& textSize( view.GetLayoutSize() );
    const Vector2 halfTextSize( textSize * 0.5f );
    const Vector2& shadowOffset( view.GetShadowOffset() );
    const Vector4& shadowColor( view.GetShadowColor() );
    const bool underlineEnabled = view.IsUnderlineEnabled();
    const Vector4& underlineColor( view.GetUnderlineColor() );
    const float underlineHeight = view.GetUnderlineHeight();
    const uint16_t outlineWidth = view.GetOutlineWidth();
    const Vector4& outlineColor( view.GetOutlineColor() );
    const bool isOutline = 0u != outlineWidth;

    const bool useDefaultColor = ( NULL == colorsBuffer );

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
    const Vector2 lineOffsetPosition( minLineOffset, 0.f );

    for( uint32_t i = 0, glyphSize = glyphs.Size(); i < glyphSize; ++i )
    {
      const GlyphInfo& glyph = *( glyphsBuffer + i );
      const bool isGlyphUnderlined = underlineEnabled || IsGlyphUnderlined( i, underlineRuns );
      thereAreUnderlinedGlyphs = thereAreUnderlinedGlyphs || isGlyphUnderlined;

      // No operation for white space
      if( glyph.width && glyph.height )
      {
        // Are we still using the same fontId as previous
        if( isGlyphUnderlined && ( glyph.fontId != lastUnderlinedFontId ) )
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
          if( currentUnderlinePosition > descender )
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

        AtlasGlyphManager::GlyphStyle style;
        style.isItalic = glyph.isItalicRequired;
        style.isBold = glyph.isBoldRequired;

        // Retrieves and caches the glyph's bitmap.
        CacheGlyph( glyph, lastFontId, style, slot );

        // Retrieves and caches the outline glyph's bitmap.
        if( isOutline )
        {
          style.outline = outlineWidth;
          CacheGlyph( glyph, lastFontId, style, slotOutline );
        }

        // Move the origin (0,0) of the mesh to the center of the actor
        const Vector2 position = *( positionsBuffer + i ) - halfTextSize - lineOffsetPosition;

        if ( 0u != slot.mImageId ) // invalid slot id, glyph has failed to be added to atlas
        {
          Vector2 positionPlusOutlineOffset = position;
          if( isOutline )
          {
            // Add an offset to the text.
            const float outlineWidthOffset = static_cast<float>( outlineWidth );
            positionPlusOutlineOffset += Vector2( outlineWidthOffset, outlineWidthOffset );
          }

          // Get the color of the character.
          const ColorIndex colorIndex = useDefaultColor ? 0u : *( colorIndicesBuffer + i );
          const Vector4& color = ( useDefaultColor || ( 0u == colorIndex ) ) ? defaultColor : *( colorsBuffer + colorIndex - 1u );

          GenerateMesh( glyph,
                        positionPlusOutlineOffset,
                        color,
                        NO_OUTLINE,
                        slot,
                        isGlyphUnderlined,
                        currentUnderlinePosition,
                        currentUnderlineThickness,
                        meshContainer,
                        newTextCache,
                        extents);

          lastFontId = glyph.fontId; // Prevents searching for existing blocksizes when string of the same fontId.
        }

        if( isOutline && ( 0u != slotOutline.mImageId ) ) // invalid slot id, glyph has failed to be added to atlas
        {
          GenerateMesh( glyph,
                        position,
                        outlineColor,
                        outlineWidth,
                        slotOutline,
                        false,
                        currentUnderlinePosition,
                        currentUnderlineThickness,
                        meshContainerOutline,
                        newTextCache,
                        extents);
        }
      }
    } // glyphs

    // Now remove references for the old text
    RemoveText();
    mTextCache.Swap( newTextCache );

    if( thereAreUnderlinedGlyphs )
    {
      // Check to see if any of the text needs an underline
      GenerateUnderlines( meshContainer, extents, underlineColor );
    }

    // For each MeshData object, create a mesh actor and add to the renderable actor
    bool isShadowDrawn = false;
    if( !meshContainerOutline.empty() )
    {
      const bool drawShadow = STYLE_DROP_SHADOW == style;
      CreateActors( meshContainerOutline,
                    textSize,
                    outlineColor,
                    shadowColor,
                    shadowOffset,
                    textControl,
                    animatablePropertyIndex,
                    drawShadow );

      isShadowDrawn = drawShadow;
    }

    // For each MeshData object, create a mesh actor and add to the renderable actor
    if( !meshContainer.empty() )
    {
      const bool drawShadow = !isShadowDrawn && ( STYLE_DROP_SHADOW == style );
      CreateActors( meshContainer,
                    textSize,
                    defaultColor,
                    shadowColor,
                    shadowOffset,
                    textControl,
                    animatablePropertyIndex,
                    drawShadow );
    }

#if defined(DEBUG_ENABLED)
    Toolkit::AtlasGlyphManager::Metrics metrics = mGlyphManager.GetMetrics();
    DALI_LOG_INFO( gLogFilter, Debug::General, "TextAtlasRenderer::GlyphManager::GlyphCount: %i, AtlasCount: %i, TextureMemoryUse: %iK\n",
                                                metrics.mGlyphCount,
                                                metrics.mAtlasMetrics.mAtlasCount,
                                                metrics.mAtlasMetrics.mTextureMemoryUsed / 1024 );

    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "%s\n", metrics.mVerboseGlyphCounts.c_str() );

    for( uint32_t i = 0; i < metrics.mAtlasMetrics.mAtlasCount; ++i )
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
    for( Vector< TextCacheEntry >::Iterator oldTextIter = mTextCache.Begin(); oldTextIter != mTextCache.End(); ++oldTextIter )
    {
      AtlasGlyphManager::GlyphStyle style;
      style.outline = oldTextIter->mOutlineWidth;
      style.isItalic = oldTextIter->isItalic;
      style.isBold = oldTextIter->isBold;
      mGlyphManager.AdjustReferenceCount( oldTextIter->mFontId, oldTextIter->mIndex, style, -1/*decrement*/ );
    }
    mTextCache.Resize( 0 );
  }

  Actor CreateMeshActor( Actor textControl, Property::Index animatablePropertyIndex, const Vector4& defaultColor, const MeshRecord& meshRecord,
                         const Vector2& actorSize, Style style )
  {
    PropertyBuffer quadVertices = PropertyBuffer::New( mQuadVertexFormat );
    quadVertices.SetData( const_cast< AtlasManager::Vertex2D* >( &meshRecord.mMesh.mVertices[ 0 ] ), meshRecord.mMesh.mVertices.Size() );

    Geometry quadGeometry = Geometry::New();
    quadGeometry.AddVertexBuffer( quadVertices );
    quadGeometry.SetIndexBuffer( &meshRecord.mMesh.mIndices[0],  meshRecord.mMesh.mIndices.Size() );

    TextureSet textureSet( mGlyphManager.GetTextures( meshRecord.mAtlasId ) );

    // Choose the shader to use.
    const bool isColorShader = ( STYLE_DROP_SHADOW != style ) && ( Pixel::BGRA8888 == mGlyphManager.GetPixelFormat( meshRecord.mAtlasId ) );
    Shader shader;
    if( isColorShader )
    {
      // The glyph is an emoji and is not a shadow.
      if( !mShaderRgba )
      {
        mShaderRgba = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER_RGBA );
      }
      shader = mShaderRgba;
    }
    else
    {
      // The glyph is text or a shadow.
      if( !mShaderL8 )
      {
        mShaderL8 = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER_L8 );
      }
      shader = mShaderL8;
    }

    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "defaultColor[%f, %f, %f, %f ]\n", defaultColor.r, defaultColor.g, defaultColor.b, defaultColor.a  );

    Dali::Property::Index shaderTextColorIndex = shader.RegisterProperty( "textColorAnimatable", defaultColor );

    if ( animatablePropertyIndex != Property::INVALID_INDEX )
    {
      // create constraint for the animatable text's color Property with textColorAnimatable in the shader.
      if( shaderTextColorIndex  )
      {
        Constraint constraint = Constraint::New<Vector4>( shader, shaderTextColorIndex, EqualToConstraint() );
        constraint.AddSource( Source( textControl, animatablePropertyIndex ) );
        constraint.Apply();
      }
    }
    else
    {
      // If not animating the text colour then set to 1's so shader uses the current vertex color
      shader.RegisterProperty( "textColorAnimatable", Vector4(1.0, 1.0, 1.0, 1.0 ) );
    }

    Dali::Renderer renderer = Dali::Renderer::New( quadGeometry, shader );
    renderer.SetTextures( textureSet );
    renderer.SetProperty( Dali::Renderer::Property::BLEND_MODE, BlendMode::ON );
    renderer.SetProperty( Dali::Renderer::Property::DEPTH_INDEX, DepthIndex::CONTENT + mDepth );

    Actor actor = Actor::New();
#if defined(DEBUG_ENABLED)
    actor.SetProperty( Dali::Actor::Property::NAME, "Text renderable actor" );
#endif
    actor.AddRenderer( renderer );
    // Keep all of the origins aligned
    actor.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
    actor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
    actor.SetSize( actorSize );
    actor.RegisterProperty("uOffset", Vector2::ZERO );
    actor.SetProperty( Actor::Property::COLOR_MODE, USE_OWN_MULTIPLY_PARENT_COLOR );

    return actor;
  }

  void StitchTextMesh( std::vector< MeshRecord >& meshContainer,
                       AtlasManager::Mesh2D& newMesh,
                       Vector< Extent >& extents,
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
        if( slot.mAtlasId == mIt->mAtlasId )
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
    for( Vector<GlyphInfo>::ConstIterator glyphIt = glyphs.Begin(),
           glyphEndIt = glyphs.End();
         glyphIt != glyphEndIt;
         ++glyphIt )
    {
      const FontId fontId = (*glyphIt).fontId;
      bool foundFont = false;

      for( std::vector< MaxBlockSize >::const_iterator blockIt = mBlockSizes.begin(),
             blockEndIt = mBlockSizes.end();
           blockIt != blockEndIt;
           ++blockIt )
      {
        if( (*blockIt).mFontId == fontId )  // Different size fonts will have a different fontId
        {
          DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Text::AtlasRenderer::CalculateBlocksSize match found fontID(%u) glyphIndex(%u)\n", fontId, (*glyphIt).index );
          foundFont = true;
          break;
        }
      }

      if ( !foundFont )
      {
        FontMetrics fontMetrics;
        mFontClient.GetFontMetrics( fontId, fontMetrics );

        MaxBlockSize maxBlockSize;
        maxBlockSize.mNeededBlockWidth = static_cast< uint32_t >( fontMetrics.height );
        maxBlockSize.mNeededBlockHeight = maxBlockSize.mNeededBlockWidth;
        maxBlockSize.mFontId = fontId;
        DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Text::AtlasRenderer::CalculateBlocksSize New font with no matched blocksize, setting blocksize[%u]\n", maxBlockSize.mNeededBlockWidth );
        mBlockSizes.push_back( maxBlockSize );
      }
    }
  }

  void GenerateUnderlines( std::vector< MeshRecord >& meshRecords,
                           Vector< Extent >& extents,
                           const Vector4& underlineColor )
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
      vert.mColor = underlineColor;
      newMesh.mVertices.PushBack( vert );

      vert.mPosition.x = brx;
      vert.mPosition.y = baseLine;
      vert.mTexCoords.x = u;
      vert.mColor = underlineColor;
      newMesh.mVertices.PushBack( vert );

      vert.mPosition.x = tlx;
      vert.mPosition.y = baseLine + thickness;
      vert.mTexCoords.x = ZERO;
      vert.mTexCoords.y = v;
      vert.mColor = underlineColor;
      newMesh.mVertices.PushBack( vert );

      vert.mPosition.x = brx;
      vert.mPosition.y = baseLine + thickness;
      vert.mTexCoords.x = u;
      vert.mColor = underlineColor;
      newMesh.mVertices.PushBack( vert );

      // Six indices in counter clockwise winding
      newMesh.mIndices.PushBack( faceIndex + 1u );
      newMesh.mIndices.PushBack( faceIndex );
      newMesh.mIndices.PushBack( faceIndex + 2u );
      newMesh.mIndices.PushBack( faceIndex + 2u );
      newMesh.mIndices.PushBack( faceIndex + 3u );
      newMesh.mIndices.PushBack( faceIndex + 1u );
      faceIndex += 4;

      Toolkit::Internal::AtlasMeshFactory::AppendMesh( meshRecords[ index ].mMesh, newMesh );
    }
  }

  Actor mActor;                                       ///< The actor parent which renders the text
  AtlasGlyphManager mGlyphManager;                    ///< Glyph Manager to handle upload and caching
  TextAbstraction::FontClient mFontClient;            ///< The font client used to supply glyph information
  Shader mShaderL8;                                   ///< The shader for glyphs and emoji's shadows.
  Shader mShaderRgba;                                 ///< The shader for emojis.
  std::vector< MaxBlockSize > mBlockSizes;            ///< Maximum size needed to contain a glyph in a block within a new atlas
  Vector< TextCacheEntry > mTextCache;                ///< Caches data from previous render
  Property::Map mQuadVertexFormat;                    ///< Describes the vertex format for text
  int mDepth;                                         ///< DepthIndex passed by control when connect to stage
};

Text::RendererPtr AtlasRenderer::New()
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Text::AtlasRenderer::New()\n" );

  return Text::RendererPtr( new AtlasRenderer() );
}

Actor AtlasRenderer::Render( Text::ViewInterface& view,
                             Actor textControl,
                             Property::Index animatablePropertyIndex,
                             float& alignmentOffset,
                             int depth )
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "Text::AtlasRenderer::Render()\n" );

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
                                     alignmentOffset,
                                     0u,
                                     numberOfGlyphs );

    glyphs.Resize( numberOfGlyphs );
    positions.Resize( numberOfGlyphs );

    const Vector4* const colorsBuffer = view.GetColors();
    const ColorIndex* const colorIndicesBuffer = view.GetColorIndices();
    const Vector4& defaultColor = view.GetTextColor();

    mImpl->AddGlyphs( view,
                      textControl,
                      animatablePropertyIndex,
                      positions,
                      glyphs,
                      defaultColor,
                      colorsBuffer,
                      colorIndicesBuffer,
                      depth,
                      alignmentOffset );

    /* In the case where AddGlyphs does not create a renderable Actor for example when glyphs are all whitespace create a new Actor. */
    /* This renderable actor is used to position the text, other "decorations" can rely on there always being an Actor regardless of it is whitespace or regular text. */
    if ( !mImpl->mActor )
    {
      mImpl->mActor = Actor::New();
    }
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

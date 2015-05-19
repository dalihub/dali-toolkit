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
#include <dali-toolkit/internal/text/rendering/basic/text-basic-renderer.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/public-api/actors/image-actor.h>
#include <dali/devel-api/images/atlas.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/line-run.h>
#include <dali-toolkit/internal/text/rendering/shaders/text-basic-shader.h>
#include <dali-toolkit/internal/text/rendering/shaders/text-bgra-shader.h>


using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Toolkit::Text;

namespace
{

#if defined(DEBUG_ENABLED)
  Debug::Filter* gLogFilter = Debug::Filter::New(Debug::Concise, true, "LOG_TEXT_RENDERING");
#endif

const std::size_t PADDING = 2; //< To avoid GL filtering artefacts

struct TextureCoordinates
{
  TextureCoordinates()
  : topLeft( 0.0f, 0.0f ),
    topRight( 1.0f, 0.0f ),
    bottomLeft( 0.0f, 1.0f ),
    bottomRight( 1.0f, 1.0f )
  {
  }

  Vector2 topLeft;
  Vector2 topRight;
  Vector2 bottomLeft;
  Vector2 bottomRight;
};

struct AtlasGlyph
{
  AtlasGlyph()
  : fontId( 0 ),
    index( 0 ),
    xOffset( 0 ),
    width( 0 ),
    height( 0 )
  {
  }

  AtlasGlyph( FontId id,
              GlyphIndex glyphIndex,
              std::size_t offset,
              std::size_t widthPixels,
              std::size_t heightPixels,
              BufferImage bitmap )
  : fontId( id ),
    index( glyphIndex ),
    xOffset( offset ),
    width( widthPixels ),
    height( heightPixels ),
    mBitmap( bitmap )
  {
  }

  FontId fontId;
  GlyphIndex index;
  std::size_t xOffset;
  std::size_t width;
  std::size_t height;
  BufferImage mBitmap;
  TextureCoordinates coords;
};

} // unnamed namespace

struct BasicRenderer::Impl
{
  /**
   * @brief Create the renderer implementation.
   */
  Impl()
  : mWidthL8( 0.0f ),
    mHeightL8( 0.0f ),
    mWidthBGRA8888( 0.0f ),
    mHeightBGRA8888( 0.0f )
  {
    mFontClient = TextAbstraction::FontClient::Get();
  }

  /**
   * @brief Reset the previous glyph calculations.
   *
   * @param[in] size The glyph space to reserve.
   */
  void Reset( std::size_t size )
  {
    mWidthL8 = 0.0f;
    mHeightL8 = 0.0f;
    mWidthBGRA8888 = 0.0f;
    mHeightBGRA8888 = 0.0f;
    mGlyphs.clear();
    mGlyphs.reserve( size );
    mAtlasL8.Reset();
    mAtlasBGRA8888.Reset();
  }

  /**
   * @brief Ccreate an Atlas, uploading the necessary glyph bitmaps
   *
   * @param[in] glyphs The glyphs to upload.
   */
  void CreateAtlases( const Vector<GlyphInfo>& glyphs )
  {
    // Clear previous atlas
    Reset( glyphs.Count() );

    for( unsigned int i=0; i<glyphs.Count(); ++i )
    {
      float width  = glyphs[i].width;
      float height = glyphs[i].height;

      if( width > 0 &&
          height > 0 ) // skip whitespace
      {
        if( !GlyphFound( glyphs[i].fontId, glyphs[i].index ) )
        {
          AddGlyph( glyphs[i] );
        }
      }
    }

    mAtlasL8 = CreateAtlas( mWidthL8, mHeightL8, Pixel::L8 );
    mAtlasBGRA8888 = CreateAtlas( mWidthBGRA8888, mHeightBGRA8888, Pixel::BGRA8888 );
  }

  Atlas CreateAtlas( unsigned int width, unsigned int height, Pixel::Format format )
  {
    Atlas atlas;

    if( width > 0 && height > 0 )
    {
      atlas = Atlas::New( width, height, format );

      for( unsigned int i=0; i<mGlyphs.size(); ++i )
      {
        AtlasGlyph& glyph = mGlyphs[i];

        const Pixel::Format glyphFormat = glyph.mBitmap.GetPixelFormat();

        if( format == glyphFormat )
        {
          atlas.Upload( glyph.mBitmap, glyph.xOffset, 0 );

          TextureCoordinates& coords = glyph.coords;
          coords.topLeft.x     = static_cast<float>(glyph.xOffset) / static_cast<float>(width);
          coords.topLeft.y     = 0.0f;
          coords.topRight.x    = static_cast<float>(glyph.xOffset + glyph.width) / static_cast<float>(width);
          coords.topRight.y    = 0.0f;
          coords.bottomLeft.x  = static_cast<float>(glyph.xOffset) / static_cast<float>(width);
          coords.bottomLeft.y  = static_cast<float>(glyph.height) / static_cast<float>(height);
          coords.bottomRight.x = static_cast<float>(glyph.xOffset + glyph.width) / static_cast<float>(width);
          coords.bottomRight.y = static_cast<float>(glyph.height) / static_cast<float>(height);
        }
      }
    }

    return atlas;
  }

  /**
   * @brief Check whether we already have the glyph.
   */
  bool GlyphFound( FontId fontId, GlyphIndex index ) const
  {
    for( unsigned int i=0; i<mGlyphs.size(); ++i )
    {
      const AtlasGlyph& glyph = mGlyphs[i];

      if( fontId == glyph.fontId &&
          index  == glyph.index )
      {
        return true;
      }
    }

    return false;
  }

  /**
   * @brief Add the glyph.
   */
  void AddGlyph( const GlyphInfo& glyphInfo )
  {
    BufferImage bitmap = mFontClient.CreateBitmap( glyphInfo.fontId, glyphInfo.index );

    const Pixel::Format format = bitmap.GetPixelFormat();

    if( Pixel::L8 == format )
    {
      mGlyphs.push_back( AtlasGlyph( glyphInfo.fontId, glyphInfo.index, mWidthL8, glyphInfo.width, glyphInfo.height, bitmap ) );

      // Increase the Atlas width/height
      mWidthL8 += glyphInfo.width + PADDING;
      if( mHeightL8 < glyphInfo.height + PADDING )
      {
        mHeightL8 = glyphInfo.height + PADDING;
      }
    }
    else if ( Pixel::BGRA8888 == format )
    {
       mGlyphs.push_back( AtlasGlyph( glyphInfo.fontId, glyphInfo.index, mWidthBGRA8888, glyphInfo.width, glyphInfo.height, bitmap ) );

      // A separate Atlas is used for color Emojis
      mWidthBGRA8888 += glyphInfo.width + PADDING;
      if( mHeightBGRA8888 < glyphInfo.height + PADDING )
      {
        mHeightBGRA8888 = glyphInfo.height + PADDING;
      }
    }
  }

  /**
   * @brief Get the texture coordinates for a glyph.
   */
  bool GetTextureCoordinates( Pixel::Format format, FontId fontId, GlyphIndex index, TextureCoordinates& coords )
  {
    for( unsigned int i=0; i<mGlyphs.size(); ++i )
    {
      const AtlasGlyph& glyph = mGlyphs[i];

      const Pixel::Format glyphFormat = glyph.mBitmap.GetPixelFormat();

      if( format == glyphFormat &&
          fontId == glyph.fontId &&
          index  == glyph.index )
      {
        coords = glyph.coords;
        return true;
      }
    }

    return false;
  }

  /**
   * @brief Helper method to create a mesh with one quad per glyph.
   *
   * @param[in] glyphs The glyphs to display.
   * @param[in] positions The 2D positions of the glyphs.
   * @param[in] image The material uses this as a diffuse texture.
   */
  /*
  Mesh CreateMesh( const Vector<GlyphInfo>& glyphs, const std::vector<Vector2>& positions, Pixel::Format format, Image image )
  {
    MeshData::VertexContainer vertices( 4 * glyphs.Count() ); // 1 quad per glyph

    MeshData::FaceIndices faces;
    faces.reserve( 6 * glyphs.Count() ); // 2 triangles per quad

    for( unsigned int i=0; i<glyphs.Count(); ++i )
    {
      float width  = glyphs[i].width;
      float height = glyphs[i].height;

      if( width > 0 &&
          height > 0 ) // skip whitespace
      {
        const Vector2& position = positions[i];

        TextureCoordinates coords;
        if( GetTextureCoordinates( format, glyphs[i].fontId, glyphs[i].index, coords ) )
        {
          vertices[ i*4 + 0 ] = MeshData::Vertex( Vector3( position.x + 0.0f*width, position.y + 0.0f*height, 0.0f ), coords.topLeft,     Vector3( 1.0f, 0.0f, 0.0f ) );
          vertices[ i*4 + 1 ] = MeshData::Vertex( Vector3( position.x + 1.0f*width, position.y + 0.0f*height, 0.0f ), coords.topRight,    Vector3( 1.0f, 1.0f, 0.0f ) );
          vertices[ i*4 + 2 ] = MeshData::Vertex( Vector3( position.x + 0.0f*width, position.y + 1.0f*height, 0.0f ), coords.bottomLeft,  Vector3( 0.0f, 1.0f, 0.0f ) );
          vertices[ i*4 + 3 ] = MeshData::Vertex( Vector3( position.x + 1.0f*width, position.y + 1.0f*height, 0.0f ), coords.bottomRight, Vector3( 0.0f, 0.0f, 1.0f ) );

          faces.push_back( i*4 + 0 ); faces.push_back( i*4 + 3 ); faces.push_back( i*4 + 1 );
          faces.push_back( i*4 + 0 ); faces.push_back( i*4 + 2 ); faces.push_back( i*4 + 3 );
        }
      }
    }

    Material material = Material::New( "Material" );
    material.SetDiffuseTexture( image );

    // Create the mesh data from the vertices and faces
    MeshData meshData;
    meshData.SetHasColor( false );
    meshData.SetMaterial( material );
    meshData.SetVertices( vertices );
    meshData.SetFaceIndices( faces );

    // Create a mesh from the data
    Dali::Mesh mesh = Mesh::New( meshData );
    return mesh;
  }
  */
  Actor mActor; ///< The actor which renders the text

  Atlas mAtlasL8;
  unsigned int mWidthL8;
  unsigned int mHeightL8;

  // A separate Atlas is used for color Emojis
  Atlas mAtlasBGRA8888;
  unsigned int mWidthBGRA8888;
  unsigned int mHeightBGRA8888;

  std::vector<AtlasGlyph> mGlyphs;

  TextAbstraction::FontClient mFontClient;
};

Text::RendererPtr BasicRenderer::New()
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Text::BasicRenderer::New()\n" );

  return Text::RendererPtr( new BasicRenderer() );
}

Actor BasicRenderer::Render( Text::ViewInterface& view )
{
  // Remove the previous text
  UnparentAndReset( mImpl->mActor );

  Length numberOfGlyphs = view.GetNumberOfGlyphs();

  if( numberOfGlyphs > 0u )
  {
    Vector<GlyphInfo> glyphs;
    glyphs.Resize( numberOfGlyphs );

    std::vector<Vector2> positions;
    positions.resize( numberOfGlyphs );

    numberOfGlyphs = view.GetGlyphs( glyphs.Begin(),
                                     &positions[0],
                                     0u,
                                     numberOfGlyphs );

    glyphs.Resize( numberOfGlyphs );
    positions.resize( numberOfGlyphs );

    mImpl->CreateAtlases( glyphs );

    Actor actorL8;
    if( mImpl->mAtlasL8 )
    {
      //actorL8 = MeshActor::New( mImpl->CreateMesh( glyphs, positions, Pixel::L8, mImpl->mAtlasL8 ) );
      actorL8.SetColorMode( USE_OWN_MULTIPLY_PARENT_COLOR );

      ShaderEffect shader = BasicShader::New();
      //actorL8.SetShaderEffect( shader );
    }

    Actor actorBGRA8888;
    if( mImpl->mAtlasBGRA8888 )
    {
      //actorBGRA8888 = MeshActor::New( mImpl->CreateMesh( glyphs, positions, Pixel::BGRA8888, mImpl->mAtlasBGRA8888 ) );
      actorBGRA8888.SetColorMode( USE_OWN_MULTIPLY_PARENT_COLOR );

      ShaderEffect shader = BgraShader::New();
      //actorBGRA8888.SetShaderEffect( shader );
    }

    // If we have both monochrome & color glyphs, two mesh actors are returned in a container
    if( actorL8 && actorBGRA8888 )
    {
      mImpl->mActor = ImageActor::New();
      mImpl->mActor.Add( actorL8 );
      mImpl->mActor.Add( actorBGRA8888 );
    }
    else
    {
      if( actorL8 )
      {
        mImpl->mActor = actorL8;
      }
      else if( actorBGRA8888 )
      {
        mImpl->mActor = actorBGRA8888;
      }
    }
  }

  return mImpl->mActor;
}

BasicRenderer::BasicRenderer()
{
  mImpl = new Impl();
}

BasicRenderer::~BasicRenderer()
{
  delete mImpl;
}

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/rendering/vector-based/vector-based-renderer.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/devel-api/rendering/renderer.h>
#include <dali/devel-api/rendering/geometry.h>
#include <dali/devel-api/text-abstraction/font-client.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/glyph-run.h>
#include <dali-toolkit/internal/text/text-view.h>
#include <dali-toolkit/internal/text/rendering/vector-based/glyphy-shader/glyphy-shader.h>
#include <dali-toolkit/internal/text/rendering/vector-based/vector-blob-atlas.h>
#include <dali-toolkit/internal/text/rendering/vector-based/vector-blob-atlas-share.h>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Toolkit::Text;

namespace
{

#if defined(DEBUG_ENABLED)
  Debug::Filter* gLogFilter = Debug::Filter::New(Debug::Concise, true, "LOG_TEXT_RENDERING");
#endif

const float DEFAULT_POINT_SIZE = 13.f;

struct Vertex2D
{
  float x;
  float y;
  float u;
  float v;
};

void AddVertex( Vector<Vertex2D>& vertices, float x, float y, float u, float v )
{
  Vertex2D meshVertex;
  meshVertex.x = x;
  meshVertex.y = y;
  meshVertex.u = u;
  meshVertex.v = v;
  vertices.PushBack( meshVertex );
}

void AddTriangle( Vector<unsigned int>& indices, unsigned int v0, unsigned int v1, unsigned int v2 )
{
  indices.PushBack( v0 );
  indices.PushBack( v1 );
  indices.PushBack( v2 );
}

bool CreateGeometry( const Vector<GlyphInfo>& glyphs,
                     unsigned int numberOfGlyphs,
                     const Vector<Vector2>& positions,
                     float xOffset,
                     float yOffset,
                     VectorBlobAtlas& atlas,
                     Dali::TextAbstraction::FontClient& fontClient,
                     Vector< Vertex2D >& vertices,
                     Vector< unsigned int >& indices )
{
  bool atlasFull( false );

  for( unsigned int i=0, idx=0; i<numberOfGlyphs && !atlasFull; ++i )
  {
    if( glyphs[i].width  > 0 &&
        glyphs[i].height > 0 )
    {
      bool foundBlob( true );

      BlobCoordinate blobCoords[4];

      if( ! atlas.FindGlyph( glyphs[i].fontId, glyphs[i].index, blobCoords )  )
      {
        // Add blob to atlas
        VectorBlob* blob( NULL );
        unsigned int blobLength( 0 );
        unsigned int nominalWidth( 0 );
        unsigned int nominalHeight( 0 );
        fontClient.CreateVectorBlob( glyphs[i].fontId, glyphs[i].index, blob, blobLength, nominalWidth, nominalHeight );

        if( 0 != blobLength )
        {
          bool glyphAdded = atlas.AddGlyph( glyphs[i].fontId, glyphs[i].index, blob, blobLength, nominalWidth, nominalHeight, blobCoords );

          foundBlob = glyphAdded;
          atlasFull = !glyphAdded;
        }
        else
        {
          foundBlob = false;
        }
      }

      if( foundBlob )
      {
        const float x1( xOffset + positions[i].x );
        const float x2( xOffset + positions[i].x + glyphs[i].width );
        const float y1( yOffset + positions[i].y );
        const float y2( yOffset + positions[i].y + glyphs[i].height );

        AddVertex( vertices, x1, y2, blobCoords[0].u, blobCoords[0].v );
        AddVertex( vertices, x1, y1, blobCoords[1].u, blobCoords[1].v );
        AddVertex( vertices, x2, y2, blobCoords[2].u, blobCoords[2].v );
        AddTriangle( indices, idx, idx+1, idx+2 );
        idx+=3;

        AddVertex( vertices, x1, y1, blobCoords[1].u, blobCoords[1].v );
        AddVertex( vertices, x2, y2, blobCoords[2].u, blobCoords[2].v );
        AddVertex( vertices, x2, y1, blobCoords[3].u, blobCoords[3].v );
        AddTriangle( indices, idx, idx+1, idx+2 );
        idx+=3;
      }
    }
  }

  // If the atlas is still partially empty, all the glyphs were added
  return !atlasFull;
}

} // unnamed namespace

struct VectorBasedRenderer::Impl
{
  Impl()
  {
    mFontClient = TextAbstraction::FontClient::Get();

    mQuadVertexFormat[ "aPosition" ] = Property::VECTOR2;
    mQuadVertexFormat[ "aTexCoord" ] = Property::VECTOR2;
    mQuadIndexFormat[ "indices" ] = Property::INTEGER;
  }

  Actor mActor;                            ///< The actor parent which renders the text

  TextAbstraction::FontClient mFontClient; ///> The font client used to supply glyph information

  Property::Map mQuadVertexFormat;         ///> Describes the vertex format for text
  Property::Map mQuadIndexFormat;          ///> Describes the index format for text

  Shader mShaderEffect;

  IntrusivePtr<VectorBlobAtlas> mAtlas;
};

Text::RendererPtr VectorBasedRenderer::New()
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Text::VectorBasedRenderer::New()\n" );

  return Text::RendererPtr( new VectorBasedRenderer() );
}

Actor VectorBasedRenderer::Render( Text::ViewInterface& view, int /*depth*/ )
{
  UnparentAndReset( mImpl->mActor );

  mImpl->mActor = Actor::New();
  mImpl->mActor.SetParentOrigin( ParentOrigin::CENTER );
  mImpl->mActor.SetSize( view.GetControlSize() );
  mImpl->mActor.SetColor( Color::BLACK );
#if defined(DEBUG_ENABLED)
  mImpl->mActor.SetName( "Text renderable actor" );
#endif

  Length numberOfGlyphs = view.GetNumberOfGlyphs();

  if( numberOfGlyphs > 0u )
  {
    Vector<GlyphInfo> glyphs;
    glyphs.Resize( numberOfGlyphs );

    Vector<Vector2> positions;
    positions.Resize( numberOfGlyphs );

    Vector<Vector4> colors;
    colors.Resize( numberOfGlyphs, view.GetTextColor() );

    numberOfGlyphs = view.GetGlyphs( glyphs.Begin(),
                                     positions.Begin(),
                                     colors.Begin(),
                                     0u,
                                     numberOfGlyphs );
    glyphs.Resize( numberOfGlyphs );
    positions.Resize( numberOfGlyphs );

    Vector< Vertex2D > vertices;
    Vector< unsigned int > indices;

    const Vector2& controlSize = view.GetControlSize();
    float xOffset = controlSize.width  * -0.5f;
    float yOffset = controlSize.height * -0.5f;

    if( ! mImpl->mAtlas ||
          mImpl->mAtlas->IsFull() )
    {
      VectorBlobAtlasShare atlasShare = VectorBlobAtlasShare::Get();
      mImpl->mAtlas = atlasShare.GetCurrentAtlas();
    }

    // First try adding the glyphs to the previous shared atlas
    bool allGlyphsAdded = CreateGeometry( glyphs, numberOfGlyphs, positions, xOffset, yOffset, *mImpl->mAtlas, mImpl->mFontClient, vertices, indices );

    if( ! allGlyphsAdded )
    {
      // The current atlas is full, abandon it and use a new one
      mImpl->mAtlas.Reset();
      vertices.Clear();
      indices.Clear();

      VectorBlobAtlasShare atlasShare = VectorBlobAtlasShare::Get();
      mImpl->mAtlas = atlasShare.GetNewAtlas();

      CreateGeometry( glyphs, numberOfGlyphs, positions, xOffset, yOffset, *mImpl->mAtlas, mImpl->mFontClient, vertices, indices );
      // Return value ignored; using more than an entire new atlas is not supported
    }

    if( 0 != vertices.Count() )
    {
      PropertyBuffer quadVertices = PropertyBuffer::New( mImpl->mQuadVertexFormat );
      PropertyBuffer quadIndices = PropertyBuffer::New( mImpl->mQuadIndexFormat );

      quadVertices.SetData( &vertices[ 0 ], vertices.Size() );
      quadIndices.SetData( &indices[ 0 ], indices.Size() );

      Geometry quadGeometry = Geometry::New();
      quadGeometry.AddVertexBuffer( quadVertices );
      quadGeometry.SetIndexBuffer( quadIndices );

      TextureSet texture = mImpl->mAtlas->GetTextureSet();

      const Vector4 atlasInfo = mImpl->mAtlas->GetInfo();
      mImpl->mShaderEffect = GlyphyShader::New( atlasInfo );

      Dali::Renderer renderer = Dali::Renderer::New( quadGeometry, mImpl->mShaderEffect );
      renderer.SetTextures( texture );
      mImpl->mActor.AddRenderer( renderer );
    }
  }

  return mImpl->mActor;
}

VectorBasedRenderer::VectorBasedRenderer()
{
  mImpl = new Impl();
}

VectorBasedRenderer::~VectorBasedRenderer()
{
  delete mImpl;
}


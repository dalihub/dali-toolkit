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
 */

// CLASS HEADER
#include "visual-factory-cache.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/common/hash.h>
#include <dali/public-api/images/resource-image.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/color/color-visual.h>
#include <dali-toolkit/internal/visuals/svg/svg-visual.h>
#include <dali-toolkit/internal/visuals/image-atlas-manager.h>

namespace
{
const char * const BROKEN_VISUAL_IMAGE_URL( DALI_IMAGE_DIR "broken.png");
}

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

VisualFactoryCache::VisualFactoryCache()
: mSvgRasterizeThread( NULL )
{
}

VisualFactoryCache::~VisualFactoryCache()
{
  SvgRasterizeThread::TerminateThread( mSvgRasterizeThread );
}

Geometry VisualFactoryCache::GetGeometry( GeometryType type )
{
  return mGeometry[type];
}

void VisualFactoryCache::SaveGeometry( GeometryType type, Geometry geometry )
{
  mGeometry[type] = geometry;
}

Shader VisualFactoryCache::GetShader( ShaderType type )
{
  return mShader[type];
}

void VisualFactoryCache::SaveShader( ShaderType type, Shader shader )
{
  mShader[type] = shader;
}

int VisualFactoryCache::FindRenderer( const std::string& key ) const
{
  int hash = Dali::CalculateHash( key );

  HashVector::Iterator startIt = mRendererHashes.Begin();
  HashVector::Iterator it;

  for(;;)
  {
    it = std::find( startIt, mRendererHashes.End(), hash );
    if( it != mRendererHashes.End() )
    {
      int index = it - mRendererHashes.Begin();
      const CachedRenderer* cachedRenderer = mRenderers[ index ];

      if( cachedRenderer && cachedRenderer->mKey == key )
      {
        return index;
      }
    }
    else
    {
      break;
    }
    startIt = it + 1;
  }

  return -1;
}

Renderer VisualFactoryCache::GetRenderer( const std::string& key ) const
{
  int index = FindRenderer( key );
  if( index != -1 )
  {
    return mRenderers[ index ]->mRenderer.GetHandle();
  }
  else
  {
    return Renderer();
  }
}

void VisualFactoryCache::SaveRenderer( const std::string& key, Renderer& renderer )
{
  int hash = Dali::CalculateHash( key );
  const CachedRenderer* cachedRenderer = new CachedRenderer( key, renderer );

  CachedRenderers::Iterator it = std::find( mRenderers.Begin(), mRenderers.End(), static_cast< CachedRenderer* >( NULL ) );
  if( it != mRenderers.End() )
  {
    *it = cachedRenderer;
    int index = it - mRenderers.Begin();
    mRendererHashes[ index ] = hash;
  }
  else
  {
    mRendererHashes.PushBack( hash );
    mRenderers.PushBack( cachedRenderer );
  }
}

bool VisualFactoryCache::CleanRendererCache( const std::string& key )
{
  int index = FindRenderer( key );
  if( index != -1 )
  {
    const CachedRenderer*& cachedRenderer = mRenderers[ index ];
    if( !cachedRenderer->mRenderer.GetHandle() )
    {
      mRendererHashes[ index ] = Dali::INITIAL_HASH_VALUE;

      delete cachedRenderer;
      cachedRenderer = NULL;
      return true;
    }
  }
  return false;
}

void VisualFactoryCache::CacheWireframeRenderer( Renderer& renderer )
{
  mWireframeRenderer = renderer;
}

Renderer VisualFactoryCache::GetWireframeRenderer()
{
  return mWireframeRenderer;
}

Geometry VisualFactoryCache::CreateQuadGeometry()
{
  const float halfWidth = 0.5f;
  const float halfHeight = 0.5f;
  struct QuadVertex { Vector2 position;};
  QuadVertex quadVertexData[4] =
  {
      { Vector2(-halfWidth, -halfHeight) },
      { Vector2(-halfWidth, halfHeight)  },
      { Vector2( halfWidth, -halfHeight) },
      { Vector2( halfWidth, halfHeight)  }
  };

  Property::Map quadVertexFormat;
  quadVertexFormat["aPosition"] = Property::VECTOR2;
  PropertyBuffer quadVertices = PropertyBuffer::New( quadVertexFormat );
  quadVertices.SetData( quadVertexData, 4 );

  // Create the geometry object
  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( quadVertices );
  geometry.SetType( Geometry::TRIANGLE_STRIP );

  return geometry;
}

ImageAtlasManagerPtr VisualFactoryCache::GetAtlasManager()
{
  if( !mAtlasManager )
  {
    mAtlasManager = new ImageAtlasManager();
    mAtlasManager->SetBrokenImage( BROKEN_VISUAL_IMAGE_URL );
  }

  return mAtlasManager;
}

NPatchLoader& VisualFactoryCache::GetNPatchLoader()
{
  return mNPatchLoader;
}

SvgRasterizeThread* VisualFactoryCache::GetSVGRasterizationThread()
{
  if( !mSvgRasterizeThread )
  {
    mSvgRasterizeThread = new SvgRasterizeThread( new EventThreadCallback( MakeCallback( this, &VisualFactoryCache::ApplyRasterizedSVGToSampler ) ) );
    mSvgRasterizeThread->Start();
  }
  return mSvgRasterizeThread;
}

void VisualFactoryCache::ApplyRasterizedSVGToSampler()
{
  while( RasterizingTaskPtr task = mSvgRasterizeThread->NextCompletedTask() )
  {
    task->GetSvgVisual()->ApplyRasterizedImage( task->GetPixelData() );
  }
}

Geometry VisualFactoryCache::CreateGridGeometry( Uint16Pair gridSize )
{
  uint16_t gridWidth = gridSize.GetWidth();
  uint16_t gridHeight = gridSize.GetHeight();

  // Create vertices
  Vector< Vector2 > vertices;
  vertices.Reserve( ( gridWidth + 1 ) * ( gridHeight + 1 ) );

  for( int y = 0; y < gridHeight + 1; ++y )
  {
    for( int x = 0; x < gridWidth + 1; ++x )
    {
      vertices.PushBack( Vector2( (float)x/gridWidth - 0.5f, (float)y/gridHeight  - 0.5f) );
    }
  }

  // Create indices
  Vector< unsigned short > indices;
  indices.Reserve( (gridWidth+2)*gridHeight*2 - 2);

  for( unsigned int row = 0u; row < gridHeight; ++row )
  {
    unsigned int rowStartIndex = row*(gridWidth+1u);
    unsigned int nextRowStartIndex = rowStartIndex + gridWidth +1u;

    if( row != 0u ) // degenerate index on non-first row
    {
      indices.PushBack( rowStartIndex );
    }

    for( unsigned int column = 0u; column < gridWidth+1u; column++) // main strip
    {
      indices.PushBack( rowStartIndex + column);
      indices.PushBack( nextRowStartIndex + column);
    }

    if( row != gridHeight-1u ) // degenerate index on non-last row
    {
      indices.PushBack( nextRowStartIndex + gridWidth );
    }
  }

  Property::Map vertexFormat;
  vertexFormat[ "aPosition" ] = Property::VECTOR2;
  PropertyBuffer vertexPropertyBuffer = PropertyBuffer::New( vertexFormat );
  if( vertices.Size() > 0 )
  {
    vertexPropertyBuffer.SetData( &vertices[ 0 ], vertices.Size() );
  }

  Property::Map indexFormat;
  indexFormat[ "indices" ] = Property::INTEGER;
  PropertyBuffer indexPropertyBuffer = PropertyBuffer::New( indexFormat );


  // Create the geometry object
  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( vertexPropertyBuffer );
  if( indices.Size() > 0 )
  {
    geometry.SetIndexBuffer( &indices[ 0 ], indices.Size() );
  }

  geometry.SetType( Geometry::TRIANGLE_STRIP );

  return geometry;
}

Geometry VisualFactoryCache::CreateBatchQuadGeometry( Vector4 texCoords )
{
  const float halfWidth = 0.5f;
  const float halfHeight = 0.5f;
  struct QuadVertex {
    QuadVertex( const Vector2& vertexPosition, const Vector2& vertexTexCoords )
      : position( vertexPosition ),
        texCoords( vertexTexCoords )
    {}
    Vector2 position;
    Vector2 texCoords;
  };

  // special case, when texture takes whole space
  if( texCoords == Vector4::ZERO )
  {
    texCoords = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
  }

  QuadVertex quadVertexData[6] =
  {
    QuadVertex( Vector2(-halfWidth,   -halfHeight ),   Vector2(texCoords.x, texCoords.y) ),
    QuadVertex( Vector2( halfWidth,   -halfHeight ),   Vector2(texCoords.z, texCoords.y) ),
    QuadVertex( Vector2(-halfWidth,    halfHeight ),   Vector2(texCoords.x, texCoords.w) ),
    QuadVertex( Vector2( halfWidth,   -halfHeight ),   Vector2(texCoords.z, texCoords.y) ),
    QuadVertex( Vector2(-halfWidth,    halfHeight ),   Vector2(texCoords.x, texCoords.w) ),
    QuadVertex( Vector2( halfWidth,    halfHeight ),   Vector2(texCoords.z, texCoords.w) ),
  };

  Property::Map vertexFormat;
  vertexFormat[ "aPosition" ] = Property::VECTOR2;
  vertexFormat[ "aTexCoord" ] = Property::VECTOR2;
  PropertyBuffer vertexBuffer = PropertyBuffer::New( vertexFormat );
  vertexBuffer.SetData( quadVertexData, 6 );

  // create geometry as normal, single quad
  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( vertexBuffer );
  geometry.SetType( Geometry::TRIANGLES );

  return geometry;
}

Image VisualFactoryCache::GetBrokenVisualImage()
{
  return ResourceImage::New( BROKEN_VISUAL_IMAGE_URL );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali


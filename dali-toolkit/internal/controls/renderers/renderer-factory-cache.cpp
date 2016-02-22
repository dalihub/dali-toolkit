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
 */

// CLASS HEADER
#include "renderer-factory-cache.h"

// EXTERNAL HEADER
#include <dali/devel-api/common/hash.h>

// INTERNAL HEADER
#include <dali-toolkit/internal/controls/renderers/color/color-renderer.h>
#include <dali-toolkit/internal/controls/renderers/svg/svg-renderer.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

RendererFactoryCache::RendererFactoryCache()
: mSvgRasterizeThread( NULL )
{
}

RendererFactoryCache::~RendererFactoryCache()
{
  SvgRasterizeThread::TerminateThread( mSvgRasterizeThread );
}

Geometry RendererFactoryCache::GetGeometry( GeometryType type )
{
  return mGeometry[type];
}

void RendererFactoryCache::SaveGeometry( GeometryType type, Geometry geometry )
{
  mGeometry[type] = geometry;
}

Shader RendererFactoryCache::GetShader( ShaderType type )
{
  return mShader[type];
}

void RendererFactoryCache::SaveShader( ShaderType type, Shader shader )
{
  mShader[type] = shader;
}

int RendererFactoryCache::FindRenderer( const std::string& key ) const
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

Renderer RendererFactoryCache::GetRenderer( const std::string& key ) const
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

void RendererFactoryCache::SaveRenderer( const std::string& key, Renderer& renderer )
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

bool RendererFactoryCache::CleanRendererCache( const std::string& key )
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

void RendererFactoryCache::CacheDebugRenderer( Renderer& renderer )
{
  mDebugRenderer = renderer;
}

Renderer RendererFactoryCache::GetDebugRenderer()
{
  return mDebugRenderer;
}

Geometry RendererFactoryCache::CreateQuadGeometry()
{
  const float halfWidth = 0.5f;
  const float halfHeight = 0.5f;
  struct QuadVertex { Vector2 position;};
  QuadVertex quadVertexData[4] =
  {
      { Vector2(-halfWidth, -halfHeight) },
      { Vector2( halfWidth, -halfHeight) },
      { Vector2(-halfWidth, halfHeight)  },
      { Vector2( halfWidth, halfHeight)  }
  };

  Property::Map quadVertexFormat;
  quadVertexFormat["aPosition"] = Property::VECTOR2;
  PropertyBuffer quadVertices = PropertyBuffer::New( quadVertexFormat );
  quadVertices.SetData( quadVertexData, 4 );

  // Create the geometry object
  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( quadVertices );
  geometry.SetGeometryType( Geometry::TRIANGLE_STRIP );

  return geometry;
}

SvgRasterizeThread* RendererFactoryCache::GetSVGRasterizationThread()
{
  if( !mSvgRasterizeThread )
  {
    mSvgRasterizeThread = new SvgRasterizeThread( new EventThreadCallback( MakeCallback( this, &RendererFactoryCache::ApplyRasterizedSVGToSampler ) ) );
    mSvgRasterizeThread->Start();
  }
  return mSvgRasterizeThread;
}

void RendererFactoryCache::ApplyRasterizedSVGToSampler()
{
  while( RasterizingTaskPtr task = mSvgRasterizeThread->NextCompletedTask() )
  {
    task->GetSvgRenderer()->ApplyRasterizedImage( task->GetPixelData() );
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali


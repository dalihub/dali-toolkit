#ifndef DALI_TOOLKIT_VISUAL_FACTORY_CACHE_H
#define DALI_TOOLKIT_VISUAL_FACTORY_CACHE_H

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

// INTERNAL INCLUDES
#include "svg/svg-rasterize-thread.h"

// EXTERNAL INCLUDES
#include <dali/public-api/math/uint-16-pair.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/rendering/geometry.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/shader.h>
#include <dali/devel-api/common/owner-container.h>
#include <dali/devel-api/object/weak-handle.h>


namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * Caches shaders and geometries. Owned by VisualFactory.
 */
class VisualFactoryCache : public RefObject
{
public:

  /**
   * Type of shader for caching.
   */
  enum ShaderType
  {
    COLOR_SHADER,
    BORDER_SHADER,
    BORDER_SHADER_ANTI_ALIASING,
    GRADIENT_SHADER_LINEAR_USER_SPACE,
    GRADIENT_SHADER_LINEAR_BOUNDING_BOX,
    GRADIENT_SHADER_RADIAL_USER_SPACE,
    GRADIENT_SHADER_RADIAL_BOUNDING_BOX,
    IMAGE_SHADER,
    NINE_PATCH_SHADER,
    SVG_SHADER,
    SHADER_TYPE_MAX = SVG_SHADER
  };

  /**
   * Type of geometry for caching.
   */
  enum GeometryType
  {
    QUAD_GEOMETRY,
    BORDER_GEOMETRY,
    NINE_PATCH_GEOMETRY,
    NINE_PATCH_BORDER_GEOMETRY,
    GEOMETRY_TYPE_MAX = NINE_PATCH_BORDER_GEOMETRY
  };

public:

  /**
   * @brief Constructor
   */
  VisualFactoryCache();

  /**
   * Request geometry of the given type.
   * @return The geometry of the required type if it exist in the cache. Otherwise, an empty handle is returned.
   */
  Geometry GetGeometry( GeometryType type );

  /**
   * Cache the geometry of the give type.
   * @param[in] type The geometry type.
   * @param[in] geometry The geometry for caching.
   */
  void SaveGeometry( GeometryType type, Geometry geometry);

  /**
   * Request shader of the given type.
   * @return The shader of the required type if it exist in the cache. Otherwise, an empty handle is returned.
   */
  Shader GetShader( ShaderType type );

  /**
   * Cache the geometry of the give type.
   * @param[in] type The geometry type.
   * @param[in] geometry The geometry for caching.
   */
  void SaveShader( ShaderType type, Shader shader );

  /*
   * Greate the quad geometry.
   * Quad geometry is shared by multiple kind of Renderer, so implement it in the factory-cache.
   */
  static Geometry CreateQuadGeometry();

  /**
   * Create the grid geometry.
   * @param[in] gridSize The size of the grid.
   * @return The created grid geometry.
   */
  static Geometry CreateGridGeometry( Uint16Pair gridSize );

public:

  /**
   * @brief Request renderer from the url
   *
   * @return The cached renderer if exist in the cache. Otherwise an empty handle is returned.
   */
  Renderer GetRenderer( const std::string& key ) const;

  /**
   * @brief Cache the renderer based on the given key.
   *
   * If the key already exists in the cache, then the cache will save an additional renderer to the cache.
   * RemoveRenderer will then need to be called twice to remove both items from the cache.
   *
   * @param[in] key The key to use for caching
   * @param[in] renderer The Renderer to be cached
   */
  void SaveRenderer( const std::string& key, Renderer& renderer );

  /**
   * @brief Cleans the renderer cache by removing the renderer from the cache based on the given key if there are no longer any references to it
   *
   * @param[in] key The key used for caching
   *
   * @return True if the renderer is no longer used anywhere, false otherwise
   */
  bool CleanRendererCache( const std::string& key );

  /**
   * @brief Cache the wireframe renderer
   */
  void CacheWireframeRenderer( Renderer& renderer );

  /**
   * @brief Request the wireframe renderer;
   */
  Renderer GetWireframeRenderer();

  /**
   * Get the SVG rasterization thread.
   * @return A pointer pointing to the SVG rasterization thread.
   */
  SvgRasterizeThread* GetSVGRasterizationThread();

private: // for svg rasterization thread

  /**
   * Applies the rasterized image to material
   */
  void ApplyRasterizedSVGToSampler();

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~VisualFactoryCache();

  /**
   * Undefined copy constructor.
   */
  VisualFactoryCache(const VisualFactoryCache&);

  /**
   * Undefined assignment operator.
   */
  VisualFactoryCache& operator=(const VisualFactoryCache& rhs);

private:
  struct CachedRenderer
  {
    std::string mKey;
    WeakHandle< Renderer > mRenderer;

    CachedRenderer( const std::string& key, Renderer& renderer )
    : mKey( key ),
      mRenderer( renderer)
    {}
  };

  typedef Dali::Vector< std::size_t > HashVector;
  typedef Dali::OwnerContainer< const CachedRenderer* > CachedRenderers;

  /**
   * @brief Finds the first index into the cached visuals from the url
   *
   * @return Returns the first index into the cached renderer from the url if it exists in the cache, otherwise returns -1
   */
  int FindRenderer( const std::string& key ) const;

private:
  Geometry mGeometry[GEOMETRY_TYPE_MAX+1];
  Shader mShader[SHADER_TYPE_MAX+1];

  HashVector mRendererHashes;
  CachedRenderers mRenderers;

  Renderer mWireframeRenderer;

  SvgRasterizeThread*  mSvgRasterizeThread;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_VISUAL_FACTORY_CACHE_H

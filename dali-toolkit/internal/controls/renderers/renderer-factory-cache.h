#ifndef __DALI_TOOLKIT_RENDERER_FACTORY_CACHE_H__
#define __DALI_TOOLKIT_RENDERER_FACTORY_CACHE_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/object/ref-object.h>
#include <dali/devel-api/rendering/geometry.h>
#include <dali/devel-api/rendering/shader.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * Caches shaders and geometries. Owned by RenderFactory.
 */
class RendererFactoryCache : public RefObject
{
public:

  /**
   * Type of shader for caching.
   */
  enum ShaderType
  {
    COLOR_SHADER,
    BORDER_SHADER,
    GRADIENT_SHADER_LINEAR,
    GRADIENT_SHADER_RADIAL,
    IMAGE_SHADER,
    N_PATCH_SHADER,
    SVG_SHADER,
    SHADER_TYPE_MAX = SVG_SHADER
  };

  /**
   * Type of geometry for caching.
   */
  enum GeometryType
  {
    QUAD_GEOMETRY,
    NINE_PATCH_GEOMETRY,
    GEOMETRY_TYPE_MAX = NINE_PATCH_GEOMETRY
  };

public:

  /**
   * @brief Constructor
   */
  RendererFactoryCache();

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

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~RendererFactoryCache();

  /**
   * Undefined copy constructor.
   */
  RendererFactoryCache(const RendererFactoryCache&);

  /**
   * Undefined assignment operator.
   */
  RendererFactoryCache& operator=(const RendererFactoryCache& rhs);

private:

  // ToDo: test whether using the WeakHandle could improve the performance
  //       With WeakHandle, the resource would be released automatically when no control is using it

  Geometry mGeometry[GEOMETRY_TYPE_MAX+1];
  Shader mShader[SHADER_TYPE_MAX+1];
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /*__DALI_TOOLKIT_RENDERER_FACTORY_CACHE_H__ */

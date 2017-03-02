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

// EXTERNAL INCLUDES
#include <dali/public-api/math/uint-16-pair.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/rendering/geometry.h>
#include <dali/public-api/rendering/shader.h>
#include <dali/devel-api/common/owner-container.h>
#include <dali/devel-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/npatch-loader.h>
#include <dali-toolkit/internal/visuals/svg/svg-rasterize-thread.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class ImageAtlasManager;
typedef IntrusivePtr<ImageAtlasManager> ImageAtlasManagerPtr;

class NPatchLoader;

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
    IMAGE_SHADER_ATLAS_DEFAULT_WRAP,
    IMAGE_SHADER_ATLAS_CUSTOM_WRAP,
    NINE_PATCH_SHADER,
    SVG_SHADER,
    TEXT_SHADER,
    WIREFRAME_SHADER,
    SHADER_TYPE_MAX = WIREFRAME_SHADER
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
    WIREFRAME_GEOMETRY,
    GEOMETRY_TYPE_MAX = WIREFRAME_GEOMETRY
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

  /**
   * @brief Returns an image to be used when a visual has failed to correctly render
   * @return The broken image handle.
   */
  static Image GetBrokenVisualImage();

public:
  /**
   * Get the image atlas manager.
   * @return A pointer to the atlas manager
   */
  ImageAtlasManagerPtr GetAtlasManager();

  /**
   * Get the N-Patch texture cache.
   * @return A reference to the N patch loader
   */
  NPatchLoader& GetNPatchLoader();

  /**
   * Get the SVG rasterization thread.
   * @return A raw pointer pointing to the SVG rasterization thread.
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
  Geometry mGeometry[GEOMETRY_TYPE_MAX+1];
  Shader mShader[SHADER_TYPE_MAX+1];

  ImageAtlasManagerPtr mAtlasManager;
  NPatchLoader mNPatchLoader;

  SvgRasterizeThread*  mSvgRasterizeThread;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_VISUAL_FACTORY_CACHE_H

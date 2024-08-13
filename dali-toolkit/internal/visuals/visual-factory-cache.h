#ifndef DALI_TOOLKIT_VISUAL_FACTORY_CACHE_H
#define DALI_TOOLKIT_VISUAL_FACTORY_CACHE_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/common/owner-container.h>
#include <dali/integration-api/adaptor-framework/shader-precompiler.h>
#include <dali/public-api/math/uint-16-pair.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/rendering/geometry.h>
#include <dali/public-api/rendering/shader.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/texture-manager/texture-manager-impl.h>
#include <dali-toolkit/internal/visuals/npatch-loader.h>
#include <dali/devel-api/rendering/renderer-devel.h>

namespace Dali
{
class Texture;

namespace Toolkit
{
namespace Internal
{
class ImageAtlasManager;
class NPatchLoader;
class TextureManager;
class VectorAnimationManager;

typedef IntrusivePtr<ImageAtlasManager> ImageAtlasManagerPtr;

/**
 * Caches shaders and geometries. Owned by VisualFactory.
 */
class VisualFactoryCache
{
public:
  /**
   * Type of shader for caching.
   */
  enum ShaderType
  {
    COLOR_SHADER,
    COLOR_SHADER_ROUNDED_CORNER,
    COLOR_SHADER_BORDERLINE,
    COLOR_SHADER_ROUNDED_BORDERLINE,
    COLOR_SHADER_BLUR_EDGE,
    COLOR_SHADER_ROUNDED_CORNER_BLUR_EDGE,
    BORDER_SHADER,
    BORDER_SHADER_ANTI_ALIASING,
    GRADIENT_SHADER_LINEAR_BOUNDING_BOX,
    GRADIENT_SHADER_LINEAR_BOUNDING_BOX_ROUNDED_CORNER,
    GRADIENT_SHADER_LINEAR_BOUNDING_BOX_BORDERLINE,
    GRADIENT_SHADER_LINEAR_BOUNDING_BOX_ROUNDED_BORDERLINE,
    GRADIENT_SHADER_LINEAR_USER_SPACE,
    GRADIENT_SHADER_LINEAR_USER_SPACE_ROUNDED_CORNER,
    GRADIENT_SHADER_LINEAR_USER_SPACE_BORDERLINE,
    GRADIENT_SHADER_LINEAR_USER_SPACE_ROUNDED_BORDERLINE,
    GRADIENT_SHADER_RADIAL_BOUNDING_BOX,
    GRADIENT_SHADER_RADIAL_BOUNDING_BOX_ROUNDED_CORNER,
    GRADIENT_SHADER_RADIAL_BOUNDING_BOX_BORDERLINE,
    GRADIENT_SHADER_RADIAL_BOUNDING_BOX_ROUNDED_BORDERLINE,
    GRADIENT_SHADER_RADIAL_USER_SPACE,
    GRADIENT_SHADER_RADIAL_USER_SPACE_ROUNDED_CORNER,
    GRADIENT_SHADER_RADIAL_USER_SPACE_BORDERLINE,
    GRADIENT_SHADER_RADIAL_USER_SPACE_ROUNDED_BORDERLINE,
    IMAGE_SHADER,
    IMAGE_SHADER_ROUNDED_CORNER,
    IMAGE_SHADER_BORDERLINE,
    IMAGE_SHADER_ROUNDED_BORDERLINE,
    IMAGE_SHADER_MASKING,
    IMAGE_SHADER_ROUNDED_CORNER_MASKING,
    IMAGE_SHADER_BORDERLINE_MASKING,
    IMAGE_SHADER_ROUNDED_BORDERLINE_MASKING,
    IMAGE_SHADER_ATLAS_DEFAULT_WRAP,
    IMAGE_SHADER_ATLAS_CUSTOM_WRAP,
    IMAGE_SHADER_YUV_TO_RGB,
    IMAGE_SHADER_ROUNDED_CORNER_YUV_TO_RGB,
    IMAGE_SHADER_BORDERLINE_YUV_TO_RGB,
    IMAGE_SHADER_ROUNDED_BORDERLINE_YUV_TO_RGB,
    IMAGE_SHADER_YUV_AND_RGB,
    IMAGE_SHADER_ROUNDED_CORNER_YUV_AND_RGB,
    IMAGE_SHADER_BORDERLINE_YUV_AND_RGB,
    IMAGE_SHADER_ROUNDED_BORDERLINE_YUV_AND_RGB,
    NATIVE_IMAGE_SHADER,
    NATIVE_IMAGE_SHADER_ROUNDED_CORNER,
    NATIVE_IMAGE_SHADER_BORDERLINE,
    NATIVE_IMAGE_SHADER_ROUNDED_BORDERLINE,
    NATIVE_IMAGE_SHADER_MASKING,
    NATIVE_IMAGE_SHADER_ROUNDED_CORNER_MASKING,
    NATIVE_IMAGE_SHADER_BORDERLINE_MASKING,
    NATIVE_IMAGE_SHADER_ROUNDED_BORDERLINE_MASKING,
    NATIVE_IMAGE_SHADER_ATLAS_DEFAULT_WRAP,
    NATIVE_IMAGE_SHADER_ATLAS_CUSTOM_WRAP,
    NINE_PATCH_SHADER,
    NINE_PATCH_MASK_SHADER,
    TEXT_SHADER_SINGLE_COLOR_TEXT,
    TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_STYLE,
    TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_OVERLAY,
    TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_STYLE_AND_OVERLAY,
    TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_EMOJI,
    TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_STYLE_AND_EMOJI,
    TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_OVERLAY_AND_EMOJI,
    TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_STYLE_AND_OVERLAY_AND_EMOJI,
    TEXT_SHADER_MULTI_COLOR_TEXT,
    TEXT_SHADER_MULTI_COLOR_TEXT_WITH_STYLE,
    TEXT_SHADER_MULTI_COLOR_TEXT_WITH_OVERLAY,
    TEXT_SHADER_MULTI_COLOR_TEXT_WITH_STYLE_AND_OVERLAY,
    ANIMATED_GRADIENT_SHADER_LINEAR_BOUNDING_REFLECT,
    ANIMATED_GRADIENT_SHADER_LINEAR_BOUNDING_REPEAT,
    ANIMATED_GRADIENT_SHADER_LINEAR_BOUNDING_CLAMP,
    ANIMATED_GRADIENT_SHADER_LINEAR_USER_REFLECT,
    ANIMATED_GRADIENT_SHADER_LINEAR_USER_REPEAT,
    ANIMATED_GRADIENT_SHADER_LINEAR_USER_CLAMP,
    ANIMATED_GRADIENT_SHADER_RADIAL_BOUNDING_REFLECT,
    ANIMATED_GRADIENT_SHADER_RADIAL_BOUNDING_REPEAT,
    ANIMATED_GRADIENT_SHADER_RADIAL_BOUNDING_CLAMP,
    ANIMATED_GRADIENT_SHADER_RADIAL_USER_REFLECT,
    ANIMATED_GRADIENT_SHADER_RADIAL_USER_REPEAT,
    ANIMATED_GRADIENT_SHADER_RADIAL_USER_CLAMP,
    WIREFRAME_SHADER,
    ARC_BUTT_CAP_SHADER,
    ARC_ROUND_CAP_SHADER,
    SHADER_TYPE_MAX = ARC_ROUND_CAP_SHADER
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
   *
   * @param[in] preMultiplyOnLoad True if image visuals should pre-multiply alpha on image load.
   */
  VisualFactoryCache(bool preMultiplyOnLoad);

  /**
   * @brief Destructor
   */
  ~VisualFactoryCache();

  /**
   * Request geometry of the given type.
   * @return The geometry of the required type if it exist in the cache. Otherwise, an empty handle is returned.
   */
  Geometry GetGeometry(GeometryType type);

  /**
   * Cache the geometry of the give type.
   * @param[in] type The geometry type.
   * @param[in] geometry The geometry for caching.
   */
  void SaveGeometry(GeometryType type, Geometry geometry);

  /**
   * Request shader of the given type.
   * @return The shader of the required type if it exist in the cache. Otherwise, an empty handle is returned.
   */
  Shader GetShader(ShaderType type);

  /**
   * Generate and cache the shader of the give type. The name of shader will be installed to shader.
   * @param[in] type The shder type.
   * @param[in] vertexShader The vertex shader code.
   * @param[in] fragmentShader The fragment shader code.
   * @return The shader created by given vertex and fragment shader code.
   */
  Shader GenerateAndSaveShader(ShaderType type, std::string_view vertexShader, std::string_view fragmentShader);

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
  static Geometry CreateGridGeometry(Uint16Pair gridSize);

  /**
   * @copydoc Toolkit::VisualFactory::SetPreMultiplyOnLoad()
   */
  void SetPreMultiplyOnLoad(bool preMultiply);

  /**
   * @copydoc Toolkit::VisualFactory::GetPreMultiplyOnLoad()
   */
  bool GetPreMultiplyOnLoad() const;

  /**
   * @brief Set an image to be used when a visual has failed to correctly render
   * @param[in] brokenImageUrlList The broken image url list
   */
  void SetBrokenImageUrl(std::string& defaultBrokenUrl, const std::vector<std::string>& brokenImageUrlList);

  /**
   * @brief Update the broken image Renderer object
   * @param[in,out] renderer renderer for broken image
   * @param[in] size the size of actor
   * @param[in] rendererIsImage True if input renderer use image shader already.
   *                            If true, we don't need to create new renderer when broken image is single image.
   *                            Most of user experience use normal images. So It can reduce runtime.
   */
  void UpdateBrokenImageRenderer(Renderer& renderer, const Vector2& size, const bool& rendererIsImage = true);

  /**
   * @brief Get whether we support YUV Planes load or not.
   *
   * @return True if we support to load YUV Planes. False otherwise.
   */
  bool GetLoadYuvPlanes() const;

public:
  /**
   * Get the image atlas manager.
   * @return A pointer to the atlas manager
   */
  ImageAtlasManagerPtr GetAtlasManager();

  /**
   * Get the texture manager
   * @return A reference to the texture manager
   */
  TextureManager& GetTextureManager();

  /**
   * Get the N-Patch texture cache.
   * @return A reference to the N patch loader
   */
  NPatchLoader& GetNPatchLoader();

  /**
   * Get the vector animation manager.
   * @return A reference to the vector animation manager.
   */
  VectorAnimationManager& GetVectorAnimationManager();

  /**
   * @brief Finalize vector animation manager.
   * It will be called when application is terminated.
   */
  void FinalizeVectorAnimationManager();

protected:
  /**
   * Undefined copy constructor.
   */
  VisualFactoryCache(const VisualFactoryCache&);

  /**
   * Undefined assignment operator.
   */
  VisualFactoryCache& operator=(const VisualFactoryCache& rhs);

private:
  /**
   * @brief Returns a cached Texture to use when a visual has failed to correctly render
   * @param[in] brokenIndex The index of broken image
   *
   * @return The broken image texture.
   */
  Texture GetBrokenVisualImage(uint32_t brokenIndex);

  /**
   * @brief Gets the Proper broken image index
   * @param[in] size The size of actor
   *
   * @return The index of broken image
   */
  int32_t GetProperBrokenImageIndex(const Vector2& size);

  /**
   * @brief Apply a texture and uniforms
   *
   * @param[in,out] renderer The renderer for broken image
   * @param[in] index The index of broken image
   */
  void ApplyTextureAndUniforms(Renderer& renderer, int index);

  /**
   * @brief Gets a geometry for npatch image
   *
   * @param[in] index the index of broken image
   * @return The Geometry for NPatch
   */
  Geometry GetNPatchGeometry(int index);

  /**
   * @brief Gets the Npatch Shader object
   *
   * @param[in] index The index of broken image
   * @return The Shader for NPatch
   */
  Shader GetNPatchShader(int index);

  /**
   * @brief Returns a broken image type
   * @param[in] index BrokenImage index
   * @return The broken image type.
   */
  VisualUrl::Type GetBrokenImageVisualType(int index);

private:
  struct BrokenImageInfo
  {
    BrokenImageInfo()
    : visualType(),
      url(""),
      npatchId(NPatchData::INVALID_NPATCH_DATA_ID),
      texture(),
      width(0),
      height(0)
    {
    }

    ~BrokenImageInfo()
    {
    }

    // Data
    VisualUrl::Type          visualType;
    std::string              url;
    NPatchData::NPatchDataId npatchId;
    Texture                  texture;
    uint32_t                 width;
    uint32_t                 height;
  };

  Geometry mGeometry[GEOMETRY_TYPE_MAX + 1];
  Shader   mShader[SHADER_TYPE_MAX + 1];

  bool mLoadYuvPlanes; ///< A global flag to specify if the image should be loaded as yuv planes

  ImageAtlasManagerPtr mAtlasManager;
  TextureManager       mTextureManager;
  NPatchLoader         mNPatchLoader;

  std::unique_ptr<VectorAnimationManager> mVectorAnimationManager;
  bool                                    mPreMultiplyOnLoad;
  std::vector<BrokenImageInfo>            mBrokenImageInfoContainer;
  std::string                             mDefaultBrokenImageUrl;
  bool                                    mUseDefaultBrokenImageOnly;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_VISUAL_FACTORY_CACHE_H

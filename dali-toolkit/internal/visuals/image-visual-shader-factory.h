#ifndef DALI_TOOLKIT_IMAGE_VISUAL_SHADER_FACTORY_H
#define DALI_TOOLKIT_IMAGE_VISUAL_SHADER_FACTORY_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <string_view>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
/**
 * @brief Whether use texture with atlas, or not
 */
enum class TextureAtlas
{
  DISABLED = 0, ///< Image visual use ATLAS
  ENABLED       ///< Image visual doesn't use ATLAS
};

/**
 * @brief Whether apply to texture wraping in default, or not
 */
enum class DefaultTextureWrapMode
{
  DO_NOT_APPLY = 0, ///< Image visual doesn't apply to wraping texture in default
  APPLY             ///< Image visual apply to wraping texture in default
};

/**
 * @brief Whether use rounded corner, or not
 */
enum class RoundedCorner
{
  DISABLED = 0, ///< Image visual doesn't use rounded corner
  ENABLED       ///< Image visual use rounded corner
};

/**
 * @brief Whether use borderline, or not
 */
enum class Borderline
{
  DISABLED = 0, ///< Image visual doesn't use borderline
  ENABLED       ///< Image visual use borderline
};

/**
 * ImageVisualShaderFactory is an object that provides and shares shaders between image visuals
 */
class ImageVisualShaderFactory
{
public:

  /**
   * @brief Constructor
   */
  ImageVisualShaderFactory();

  /**
   * @brief Destructor
   */
  ~ImageVisualShaderFactory();

  /**
   * Get the standard image rendering shader.
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] atlasing Whether texture atlasing is applied.
   * @param[in] defaultTextureWrapping Whether the default texture wrap mode is applied.
   * @param[in] roundedCorner Whether the rounded corder is applied.
   * @param[in] borderline Whether the borderline of visual is applied.
   */
  Shader GetShader(VisualFactoryCache& factoryCache, TextureAtlas atlasing, DefaultTextureWrapMode defaultTextureWrapping, RoundedCorner roundedCorner, Borderline borderline);

  /**
   * Request the default vertex shader source.
   * @return The default vertex shader source.
   */
  std::string_view GetVertexShaderSource();

  /**
   * Request the default fragment shader source.
   * @return The default fragment shader source.
   */
  std::string_view GetFragmentShaderSource();

protected:
  /**
   * Undefined copy constructor.
   */
  ImageVisualShaderFactory(const ImageVisualShaderFactory&);

  /**
   * Undefined assignment operator.
   */
  ImageVisualShaderFactory& operator=(const ImageVisualShaderFactory& rhs);

private:
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_IMAGE_VISUAL_SHADER_FACTORY_H

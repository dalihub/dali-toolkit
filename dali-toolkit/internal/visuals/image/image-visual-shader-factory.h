#ifndef DALI_TOOLKIT_IMAGE_VISUAL_SHADER_FACTORY_H
#define DALI_TOOLKIT_IMAGE_VISUAL_SHADER_FACTORY_H

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
#include <dali/integration-api/adaptor-framework/shader-precompiler.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/image/image-visual-shader-feature-builder.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <string_view>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
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
   * @brief Get the standard image rendering shader.
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] featureBuilder Collection of current image shader's features
   * @return The standard image rendering shader with features.
   */
  Shader GetShader(VisualFactoryCache& factoryCache, ImageVisualShaderFeatureBuilder& featureBuilder);

  /**
   * @brief Request the default vertex shader source.
   * @return The default vertex shader source.
   */
  std::string_view GetVertexShaderSource();

  /**
   * @brief Request the default fragment shader source.
   * @return The default fragment shader source.
   */
  std::string_view GetFragmentShaderSource();

  /**
   * @brief Get the default shader source.
   * @param[in] shaders shaderList for precompile
   */
  void GetPreCompiledShader(RawShaderData& shaders);

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
  /**
   * @brief Cached information whether native image should change fragment shader.
   * Default it is ChangeFragmentShader::UNDECIDED.
   * If we have any chance to check native image source apply fragment shader,
   * this vaule will be changed one of these : ChangeFragmentShader::DONT_CHANGE or ChangeFragmentShader::NEED_CHANGE
   *
   * After result cached, this value will not be changed.
   *
   * If value is DONT_CHANGE, ImageVisualShaderFactory::GetShader never call ApplyNativeFragmentShader.
   * Else, ImageVisualShaderFactory::GetShader will call ApplyNativeFragmentShader if native image source texture come.
   */
  ImageVisualShaderFeature::ChangeFragmentShader::Type mFragmentShaderNeedChange : 3;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_IMAGE_VISUAL_SHADER_FACTORY_H

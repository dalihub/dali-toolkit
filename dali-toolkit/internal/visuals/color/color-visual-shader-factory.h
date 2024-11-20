#ifndef DALI_TOOLKIT_COLOR_VISUAL_SHADER_FACTORY_H
#define DALI_TOOLKIT_COLOR_VISUAL_SHADER_FACTORY_H

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
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-shader-factory-interface.h>
#include <string_view>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace ColorVisualShaderFeature
{
namespace RoundedCorner
{
/**
 * @brief Whether use rounded corner, or squircle corner, or not
 */
enum Type
{
  DISABLED = 0,   ///< Color visual doesn't use rounded corner
  ROUNDED_CORNER, ///< Color visual uses rounded corner
  SQUIRCLE_CORNER ///< Color visual use squircle corner
};
} // namespace RoundedCorner

namespace Blur
{
/**
 * @brief Whether use Blur, or not
 */
enum Type
{
  DISABLED = 0, ///< Color visual doesn't use blur
  ENABLED       ///< Color visual uses blur
};
} // namespace Blur

namespace Borderline
{
/**
 * @brief Whether use Borderline
 */
enum Type
{
  DISABLED = 0, /// Color visual doesn't use Borderline
  ENABLED       /// Color visual uses Borderline
};
} // namespace Borderline

namespace Cutout
{
/**
 * @brief Whether use Cutout, or not
 */
enum Type
{
  DISABLED = 0, ///< Color visual doesn't use Cutout
  ENABLED       ///< Color visual uses Cutout
};
} // namespace Cutout

class FeatureBuilder
{
public:
  FeatureBuilder();
  FeatureBuilder& EnableRoundCorner(bool enableRoundCorner, bool enableSquircleCorner = false);
  FeatureBuilder& EnableBorderLine(bool enableBorderLine);
  FeatureBuilder& EnableBlur(bool enableBlur);
  FeatureBuilder& EnableCutout(bool enableCutout);

  VisualFactoryCache::ShaderType GetShaderType() const;
  void                           GetVertexShaderPrefixList(std::string& vertexShaderPrefixList) const;
  void                           GetFragmentShaderPrefixList(std::string& fragmentShaderPrefixList) const;

private:
  RoundedCorner::Type mColorRoundCorner : 2; ///< Whether use rounded corner, or not. default as RoundedCorner::DISABLED
  Borderline::Type    mColorBorderline : 2;  ///< Whether use border line, or not. default as Borderline::DISABLED
  Blur::Type          mColorBlur : 2;        ///< Whether use blur, or not. default as Blur::DISABLED
  Cutout::Type        mColorCutout : 2;      ///< Whether use cutout, or not. default as Cutout::DISABLED
};
} // namespace ColorVisualShaderFeature

/**
 * ColorVisualShaderFactory is an object that provides and shares shaders between color visuals
 */
class ColorVisualShaderFactory : public VisualShaderFactoryInterface
{
public:
  /**
   * @brief Constructor
   */
  ColorVisualShaderFactory();

  /**
   * @brief Destructor
   */
  ~ColorVisualShaderFactory() override;

public:
  /**
   * @brief Get the standard color rendering shader.
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] featureBuilder Collection of current text shader's features
   * @return The standard text rendering shader with features.
  */
  Shader GetShader(VisualFactoryCache& factoryCache, const ColorVisualShaderFeature::FeatureBuilder& featureBuilder);

public: // Implementation of VisualShaderFactoryInterface
  /**
   * @copydoc Dali::Toolkit::VisualShaderFactoryInterface::AddPrecompiledShader
   */
  bool AddPrecompiledShader(PrecompileShaderOption& option) override;

  /**
   * @copydoc Dali::Toolkit::VisualShaderFactoryInterface::GetPreCompiledShader
   */
  void GetPreCompiledShader(ShaderPreCompiler::RawShaderData& shaders) override;

private:
  /**
   * @brief Create pre-compiled shader for image with builder and option.
   */
  void CreatePrecompileShader(ColorVisualShaderFeature::FeatureBuilder& builder, const ShaderFlagList& option);
  /**
   * @brief Check if cached hash value is valid or not.
   */
  bool SavePrecompileShader(VisualFactoryCache::ShaderType shader, std::string&& vertexPrefix, std::string&& fragmentPrefix);

protected:
  /**
   * Undefined copy constructor.
   */
  ColorVisualShaderFactory(const ColorVisualShaderFactory&) = delete;

  /**
   * Undefined assignment operator.
   */
  ColorVisualShaderFactory& operator=(const ColorVisualShaderFactory& rhs) = delete;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_COLOR_VISUAL_SHADER_FACTORY_H

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

// CLASS HEADER
#include <dali-toolkit/internal/visuals/color/color-visual-shader-factory.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

namespace
{

constexpr VisualFactoryCache::ShaderType SHADER_TYPE_TABLE[] = {
  VisualFactoryCache::COLOR_SHADER,
  VisualFactoryCache::COLOR_SHADER_ROUNDED_CORNER,
  VisualFactoryCache::COLOR_SHADER_BORDERLINE,
  VisualFactoryCache::COLOR_SHADER_ROUNDED_BORDERLINE,
  VisualFactoryCache::COLOR_SHADER_BLUR_EDGE,
  VisualFactoryCache::COLOR_SHADER_ROUNDED_CORNER_BLUR_EDGE,
};

constexpr VisualFactoryCache::ShaderType SHADER_TYPE_WITH_CUTOUT_TABLE[] = {
  VisualFactoryCache::COLOR_SHADER_CUTOUT,
  VisualFactoryCache::COLOR_SHADER_CUTOUT_ROUNDED_CORNER,
  VisualFactoryCache::COLOR_SHADER_CUTOUT_BORDERLINE,
  VisualFactoryCache::COLOR_SHADER_CUTOUT_ROUNDED_BORDERLINE,
  VisualFactoryCache::COLOR_SHADER_CUTOUT_BLUR_EDGE,
  VisualFactoryCache::COLOR_SHADER_CUTOUT_ROUNDED_CORNER_BLUR_EDGE,
};

// enum of required list when we select shader
enum ColorVisualRequireFlag
{
  DEFAULT        = 0,
  ROUNDED_CORNER = 1 << 0,
  BORDERLINE     = 1 << 1,
  BLUR           = 1 << 2,
};

constexpr uint32_t MINIMUM_SHADER_VERSION_SUPPORT_ROUNDED_BLUR = 300;

static constexpr auto PREDEFINED_SHADER_TYPE_COUNT = 2u;

constexpr std::string_view VertexPredefines[PREDEFINED_SHADER_TYPE_COUNT]{
  "",                                     //VisualFactoryCache::COLOR_SHADER
  "#define IS_REQUIRED_ROUNDED_CORNER\n", //VisualFactoryCache::COLOR_SHADER_ROUNDED_CORNER
};
constexpr std::string_view FragmentPredefines[PREDEFINED_SHADER_TYPE_COUNT]{
  "",                                     //VisualFactoryCache::COLOR_SHADER
  "#define IS_REQUIRED_ROUNDED_CORNER\n", //VisualFactoryCache::COLOR_SHADER_ROUNDED_CORNER
};
constexpr VisualFactoryCache::ShaderType ShaderTypePredefines[PREDEFINED_SHADER_TYPE_COUNT]{
  VisualFactoryCache::ShaderType::COLOR_SHADER,
  VisualFactoryCache::ShaderType::COLOR_SHADER_ROUNDED_CORNER,
};
}

ColorVisualShaderFeatureBuilder::ColorVisualShaderFeatureBuilder()
: mColorRoundCorner(ColorVisualShaderFeature::RoundedCorner::DISABLED),
  mColorBorderline(ColorVisualShaderFeature::Borderline::DISABLED),
  mColorBlur(ColorVisualShaderFeature::Blur::DISABLED),
  mColorCutout(ColorVisualShaderFeature::Cutout::DISABLED)
{
}

ColorVisualShaderFeatureBuilder& ColorVisualShaderFeatureBuilder::EnableRoundCorner(bool enableRoundedCorner)
{
  mColorRoundCorner = (enableRoundedCorner ? ColorVisualShaderFeature::RoundedCorner::ENABLED : ColorVisualShaderFeature::RoundedCorner::DISABLED);
  return *this;
}

ColorVisualShaderFeatureBuilder& ColorVisualShaderFeatureBuilder::EnableBorderLine(bool enableBorderLine)
{
  mColorBorderline = (enableBorderLine ? ColorVisualShaderFeature::Borderline::ENABLED : ColorVisualShaderFeature::Borderline::DISABLED);
  return *this;
}

ColorVisualShaderFeatureBuilder& ColorVisualShaderFeatureBuilder::EnableBlur(bool enableBlur)
{
  mColorBlur = (enableBlur ? ColorVisualShaderFeature::Blur::ENABLED : ColorVisualShaderFeature::Blur::DISABLED);
  return *this;
}

ColorVisualShaderFeatureBuilder& ColorVisualShaderFeatureBuilder::EnableCutout(bool enableCutout)
{
  mColorCutout = (enableCutout ? ColorVisualShaderFeature::Cutout::ENABLED : ColorVisualShaderFeature::Cutout::DISABLED);
  return *this;
}

VisualFactoryCache::ShaderType ColorVisualShaderFeatureBuilder::GetShaderType() const
{
  VisualFactoryCache::ShaderType shaderType = VisualFactoryCache::COLOR_SHADER;
  uint32_t  shaderTypeFlag = ColorVisualRequireFlag::DEFAULT;
  if(mColorBlur)
  {
    shaderTypeFlag |= ColorVisualRequireFlag::BLUR;
  }
  if(mColorRoundCorner)
  {
    shaderTypeFlag |= ColorVisualRequireFlag::ROUNDED_CORNER;
  }
  if(mColorBorderline && !mColorBlur)
  {
    shaderTypeFlag |= ColorVisualRequireFlag::BORDERLINE;
  }

  shaderType = mColorCutout ? SHADER_TYPE_WITH_CUTOUT_TABLE[shaderTypeFlag] : SHADER_TYPE_TABLE[shaderTypeFlag];

  return shaderType;
}

void ColorVisualShaderFeatureBuilder::GetVertexShaderPrefixList(std::string& vertexShaderPrefixList) const
{
  if(mColorRoundCorner == ColorVisualShaderFeature::RoundedCorner::ENABLED)
  {
    vertexShaderPrefixList += "#define IS_REQUIRED_ROUNDED_CORNER\n";
  }
  if(mColorBlur == ColorVisualShaderFeature::Blur::ENABLED)
  {
    vertexShaderPrefixList += "#define IS_REQUIRED_BLUR\n";
  }
  if(mColorBorderline == ColorVisualShaderFeature::Borderline::ENABLED && mColorBlur == ColorVisualShaderFeature::RoundedCorner::DISABLED)
  {
    vertexShaderPrefixList += "#define IS_REQUIRED_BORDERLINE\n";
  }
  if(mColorCutout == ColorVisualShaderFeature::Cutout::ENABLED)
  {
    vertexShaderPrefixList += "#define IS_REQUIRED_CUTOUT\n";
  }
}

void ColorVisualShaderFeatureBuilder::GetFragmentShaderPrefixList(std::string& fragmentShaderPrefixList) const
{
  if(mColorRoundCorner == ColorVisualShaderFeature::RoundedCorner::ENABLED)
  {
    fragmentShaderPrefixList += "#define IS_REQUIRED_ROUNDED_CORNER\n";
  }
  if(mColorBlur == ColorVisualShaderFeature::Blur::ENABLED)
  {
    fragmentShaderPrefixList += "#define IS_REQUIRED_BLUR\n";
    // If shader version doesn't support latest blur with corner radius, Let we use legacy code.
    if(DALI_UNLIKELY(Dali::Shader::GetShaderLanguageVersion() < MINIMUM_SHADER_VERSION_SUPPORT_ROUNDED_BLUR))
    {
      fragmentShaderPrefixList += "#define SL_VERSION_LOW\n";
    }
  }
  if(mColorBorderline == ColorVisualShaderFeature::Borderline::ENABLED && mColorBlur == ColorVisualShaderFeature::RoundedCorner::DISABLED)
  {
    fragmentShaderPrefixList += "#define IS_REQUIRED_BORDERLINE\n";
  }
  if(mColorCutout == ColorVisualShaderFeature::Cutout::ENABLED)
  {
    fragmentShaderPrefixList += "#define IS_REQUIRED_CUTOUT\n";
  }
}

ColorVisualShaderFactory::ColorVisualShaderFactory()
{
}

ColorVisualShaderFactory::~ColorVisualShaderFactory()
{
}

Shader ColorVisualShaderFactory::GetShader(VisualFactoryCache& factoryCache, const ColorVisualShaderFeatureBuilder& featureBuilder)
{
  Shader                         shader;
  VisualFactoryCache::ShaderType  shaderType = featureBuilder.GetShaderType();
  shader     = factoryCache.GetShader(shaderType);

  if(!shader)
  {
    std::string vertexShaderPrefixList;
    std::string fragmentShaderPrefixList;
    featureBuilder.GetVertexShaderPrefixList(vertexShaderPrefixList);
    featureBuilder.GetFragmentShaderPrefixList(fragmentShaderPrefixList);

    std::string vertexShader   = std::string(Dali::Shader::GetVertexShaderPrefix() + vertexShaderPrefixList + SHADER_COLOR_VISUAL_SHADER_VERT.data());
    std::string fragmentShader = std::string(Dali::Shader::GetFragmentShaderPrefix() + fragmentShaderPrefixList + SHADER_COLOR_VISUAL_SHADER_FRAG.data());

    shader = factoryCache.GenerateAndSaveShader(shaderType, vertexShader, fragmentShader);
  }
  return shader;

}

void ColorVisualShaderFactory::GetPreCompiledShader(RawShaderData& shaders)
{
  std::vector<std::string_view> vertexPrefix;
  std::vector<std::string_view> fragmentPrefix;
  std::vector<std::string_view> shaderName;
  int                           shaderCount = 0;
  shaders.shaderCount                       = 0;
  for(uint32_t i = 0u; i < PREDEFINED_SHADER_TYPE_COUNT; ++i)
  {
    vertexPrefix.push_back(VertexPredefines[i]);
    fragmentPrefix.push_back(FragmentPredefines[i]);
    shaderName.push_back(Scripting::GetLinearEnumerationName<VisualFactoryCache::ShaderType>(ShaderTypePredefines[i], VISUAL_SHADER_TYPE_TABLE, VISUAL_SHADER_TYPE_TABLE_COUNT));
    shaderCount++;
  }

  shaders.vertexPrefix   = std::move(vertexPrefix);
  shaders.fragmentPrefix = std::move(fragmentPrefix);
  shaders.shaderName     = std::move(shaderName);
  shaders.vertexShader   = SHADER_COLOR_VISUAL_SHADER_VERT;
  shaders.fragmentShader = SHADER_COLOR_VISUAL_SHADER_FRAG;
  shaders.shaderCount    = shaderCount;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

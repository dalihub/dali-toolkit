/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
  VisualFactoryCache::COLOR_SHADER_SQUIRCLE_CORNER,
  VisualFactoryCache::COLOR_SHADER_BORDERLINE,
  VisualFactoryCache::COLOR_SHADER_ROUNDED_BORDERLINE,
  VisualFactoryCache::COLOR_SHADER_SQUIRCLE_BORDERLINE,
  VisualFactoryCache::COLOR_SHADER_BLUR_EDGE,
  VisualFactoryCache::COLOR_SHADER_ROUNDED_CORNER_BLUR_EDGE,
  VisualFactoryCache::COLOR_SHADER_SQUIRCLE_CORNER_BLUR_EDGE,
  VisualFactoryCache::COLOR_SHADER_BORDERLINE_BLUR_EDGE,
  VisualFactoryCache::COLOR_SHADER_ROUNDED_BORDERLINE_BLUR_EDGE,
  VisualFactoryCache::COLOR_SHADER_SQUIRCLE_BORDERLINE_BLUR_EDGE,
  VisualFactoryCache::COLOR_SHADER_CUTOUT,
  VisualFactoryCache::COLOR_SHADER_CUTOUT_ROUNDED_CORNER,
  VisualFactoryCache::COLOR_SHADER_CUTOUT_SQUIRCLE_CORNER,
  VisualFactoryCache::COLOR_SHADER_CUTOUT_BORDERLINE,
  VisualFactoryCache::COLOR_SHADER_CUTOUT_ROUNDED_BORDERLINE,
  VisualFactoryCache::COLOR_SHADER_CUTOUT_SQUIRCLE_BORDERLINE,
  VisualFactoryCache::COLOR_SHADER_CUTOUT_BLUR_EDGE,
  VisualFactoryCache::COLOR_SHADER_CUTOUT_ROUNDED_CORNER_BLUR_EDGE,
  VisualFactoryCache::COLOR_SHADER_CUTOUT_SQUIRCLE_CORNER_BLUR_EDGE,
  VisualFactoryCache::COLOR_SHADER_CUTOUT_BORDERLINE_BLUR_EDGE,
  VisualFactoryCache::COLOR_SHADER_CUTOUT_ROUNDED_BORDERLINE_BLUR_EDGE,
  VisualFactoryCache::COLOR_SHADER_CUTOUT_SQUIRCLE_BORDERLINE_BLUR_EDGE,
};
constexpr uint32_t SHADER_TYPE_TABLE_COUNT = sizeof(SHADER_TYPE_TABLE) / sizeof(SHADER_TYPE_TABLE[0]);

// enum of required list when we select shader
enum ColorVisualRequireFlag
{
  DEFAULT         = 0,
  ROUNDED_CORNER  = 1,
  SQUIRCLE_CORNER = 2,

  BORDERLINE = (1 << 0) * 3,
  BLUR       = (1 << 1) * 3,
  CUTOUT     = (1 << 2) * 3,
};

constexpr uint32_t MINIMUM_SHADER_VERSION_SUPPORT_ROUNDED_BLUR = 300;

static constexpr auto PREDEFINED_SHADER_TYPE_COUNT = 2u;

constexpr std::string_view VertexPredefines[PREDEFINED_SHADER_TYPE_COUNT]{
  "",                                     // VisualFactoryCache::COLOR_SHADER
  "#define IS_REQUIRED_ROUNDED_CORNER\n", // VisualFactoryCache::COLOR_SHADER_ROUNDED_CORNER
};
constexpr std::string_view FragmentPredefines[PREDEFINED_SHADER_TYPE_COUNT]{
  "",                                     // VisualFactoryCache::COLOR_SHADER
  "#define IS_REQUIRED_ROUNDED_CORNER\n", // VisualFactoryCache::COLOR_SHADER_ROUNDED_CORNER
};
constexpr VisualFactoryCache::ShaderType ShaderTypePredefines[PREDEFINED_SHADER_TYPE_COUNT]{
  VisualFactoryCache::ShaderType::COLOR_SHADER,
  VisualFactoryCache::ShaderType::COLOR_SHADER_ROUNDED_CORNER,
};
} // namespace

namespace ColorVisualShaderFeature
{
FeatureBuilder::FeatureBuilder()
: mColorRoundCorner(RoundedCorner::DISABLED),
  mColorBorderline(Borderline::DISABLED),
  mColorBlur(Blur::DISABLED),
  mColorCutout(Cutout::DISABLED)
{
}

FeatureBuilder& FeatureBuilder::EnableRoundCorner(bool enableRoundedCorner, bool enableSquircleCorner)
{
  mColorRoundCorner = (enableRoundedCorner ? (enableSquircleCorner ? RoundedCorner::SQUIRCLE_CORNER : RoundedCorner::ROUNDED_CORNER) : RoundedCorner::DISABLED);
  return *this;
}

FeatureBuilder& FeatureBuilder::EnableBorderLine(bool enableBorderLine)
{
  mColorBorderline = (enableBorderLine ? Borderline::ENABLED : Borderline::DISABLED);
  return *this;
}

FeatureBuilder& FeatureBuilder::EnableBlur(bool enableBlur)
{
  mColorBlur = (enableBlur ? Blur::ENABLED : Blur::DISABLED);
  return *this;
}

FeatureBuilder& FeatureBuilder::EnableCutout(bool enableCutout)
{
  mColorCutout = (enableCutout ? Cutout::ENABLED : Cutout::DISABLED);
  return *this;
}

VisualFactoryCache::ShaderType FeatureBuilder::GetShaderType() const
{
  VisualFactoryCache::ShaderType shaderType     = VisualFactoryCache::COLOR_SHADER;
  uint32_t                       shaderTypeFlag = ColorVisualRequireFlag::DEFAULT;

  if(mColorRoundCorner == RoundedCorner::SQUIRCLE_CORNER)
  {
    shaderTypeFlag += ColorVisualRequireFlag::SQUIRCLE_CORNER;
  }
  else if(mColorRoundCorner == RoundedCorner::ROUNDED_CORNER)
  {
    shaderTypeFlag += ColorVisualRequireFlag::ROUNDED_CORNER;
  }

  if(mColorBorderline)
  {
    shaderTypeFlag += ColorVisualRequireFlag::BORDERLINE;
  }

  if(mColorBlur)
  {
    shaderTypeFlag += ColorVisualRequireFlag::BLUR;
  }

  if(mColorCutout)
  {
    shaderTypeFlag += ColorVisualRequireFlag::CUTOUT;
  }

  DALI_ASSERT_DEBUG(shaderTypeFlag < SHADER_TYPE_TABLE_COUNT && "Invalid color shader type generated!");

  shaderType = SHADER_TYPE_TABLE[shaderTypeFlag];

  return shaderType;
}

void FeatureBuilder::GetVertexShaderPrefixList(std::string& vertexShaderPrefixList) const
{
  if(mColorRoundCorner != RoundedCorner::DISABLED)
  {
    vertexShaderPrefixList += "#define IS_REQUIRED_ROUNDED_CORNER\n";
  }
  if(mColorBlur == Blur::ENABLED)
  {
    vertexShaderPrefixList += "#define IS_REQUIRED_BLUR\n";
  }
  if(mColorBorderline == Borderline::ENABLED)
  {
    vertexShaderPrefixList += "#define IS_REQUIRED_BORDERLINE\n";
  }
  if(mColorCutout == Cutout::ENABLED)
  {
    vertexShaderPrefixList += "#define IS_REQUIRED_CUTOUT\n";
  }
}

void FeatureBuilder::GetFragmentShaderPrefixList(std::string& fragmentShaderPrefixList) const
{
  if(mColorRoundCorner != RoundedCorner::DISABLED)
  {
    fragmentShaderPrefixList += "#define IS_REQUIRED_ROUNDED_CORNER\n";
    if(mColorRoundCorner == RoundedCorner::SQUIRCLE_CORNER)
    {
      fragmentShaderPrefixList += "#define IS_REQUIRED_SQUIRCLE_CORNER\n";
    }
  }
  if(mColorBlur == Blur::ENABLED)
  {
    fragmentShaderPrefixList += "#define IS_REQUIRED_BLUR\n";
    // If shader version doesn't support latest blur with corner radius, Let we use legacy code.
    if(DALI_UNLIKELY(Dali::Shader::GetShaderLanguageVersion() < MINIMUM_SHADER_VERSION_SUPPORT_ROUNDED_BLUR))
    {
      fragmentShaderPrefixList += "#define SL_VERSION_LOW\n";
    }
  }
  if(mColorBorderline == Borderline::ENABLED)
  {
    fragmentShaderPrefixList += "#define IS_REQUIRED_BORDERLINE\n";
  }
  if(mColorCutout == Cutout::ENABLED)
  {
    fragmentShaderPrefixList += "#define IS_REQUIRED_CUTOUT\n";
  }
}

bool FeatureBuilder::IsCutoutEnabled() const
{
  return (mColorCutout == Cutout::ENABLED);
}

} // namespace ColorVisualShaderFeature

ColorVisualShaderFactory::ColorVisualShaderFactory()
{
}

ColorVisualShaderFactory::~ColorVisualShaderFactory()
{
}

Shader ColorVisualShaderFactory::GetShader(VisualFactoryCache& factoryCache, const ColorVisualShaderFeature::FeatureBuilder& featureBuilder)
{
  Shader                         shader;
  VisualFactoryCache::ShaderType shaderType = featureBuilder.GetShaderType();
  shader                                    = factoryCache.GetShader(shaderType);

  if(!shader)
  {
    std::string vertexShaderPrefixList;
    std::string fragmentShaderPrefixList;
    featureBuilder.GetVertexShaderPrefixList(vertexShaderPrefixList);
    featureBuilder.GetFragmentShaderPrefixList(fragmentShaderPrefixList);

    std::string vertexShader   = std::string(vertexShaderPrefixList + SHADER_COLOR_VISUAL_SHADER_VERT.data());
    std::string fragmentShader = std::string(fragmentShaderPrefixList + SHADER_COLOR_VISUAL_SHADER_FRAG.data());

    shader = factoryCache.GenerateAndSaveShader(shaderType, vertexShader, fragmentShader);

    if(featureBuilder.IsCutoutEnabled())
    {
      shader.RegisterUniqueProperty(CUTOUT_CORNER_RADIUS_UNIFORM_NAME, Vector4::ZERO);
    }
  }
  return shader;
}

std::string_view ColorVisualShaderFactory::GetVertexShaderSource()
{
  // static string variable to cache complete vertex shader
  static std::string gVertexShader;
  if(gVertexShader.empty())
  {
    gVertexShader = SHADER_COLOR_VISUAL_SHADER_VERT.data();
  }

  return gVertexShader;
}

std::string_view ColorVisualShaderFactory::GetFragmentShaderSource()
{
  // static string variable to cache complete fragment shader
  static std::string gFragmentShader;
  if(gFragmentShader.empty())
  {
    gFragmentShader = SHADER_COLOR_VISUAL_SHADER_FRAG.data();
  }
  return gFragmentShader;
}

bool ColorVisualShaderFactory::AddPrecompiledShader(PrecompileShaderOption& option)
{
  ShaderFlagList shaderOption = option.GetShaderOptions();

  auto        featureBuilder = ColorVisualShaderFeature::FeatureBuilder();
  std::string vertexPrefixList;
  std::string fragmentPrefixList;
  CreatePrecompileShader(featureBuilder, shaderOption);

  VisualFactoryCache::ShaderType type = featureBuilder.GetShaderType();
  featureBuilder.GetVertexShaderPrefixList(vertexPrefixList);
  featureBuilder.GetFragmentShaderPrefixList(fragmentPrefixList);
  return SavePrecompileShader(type, std::move(vertexPrefixList), std::move(fragmentPrefixList));
}

void ColorVisualShaderFactory::GetPreCompiledShader(ShaderPreCompiler::RawShaderData& shaders)
{
  std::vector<std::string> vertexPrefix;
  std::vector<std::string> fragmentPrefix;
  std::vector<std::string> shaderName;

  uint32_t shaderCount = 0;

  shaders.shaderCount = 0;

  // precompile requested shader first
  for(uint32_t i = 0u; i < mRequestedPrecompileShader.size(); i++)
  {
    vertexPrefix.push_back(std::move(mRequestedPrecompileShader[i].vertexPrefix));
    fragmentPrefix.push_back(std::move(mRequestedPrecompileShader[i].fragmentPrefix));
    shaderName.push_back(std::string(Scripting::GetLinearEnumerationName<VisualFactoryCache::ShaderType>(mRequestedPrecompileShader[i].type, VISUAL_SHADER_TYPE_TABLE, VISUAL_SHADER_TYPE_TABLE_COUNT)));
    shaderCount++;
  }

  // Clean up requested precompile shader list
  mRequestedPrecompileShader.clear();

  for(uint32_t i = 0u; i < PREDEFINED_SHADER_TYPE_COUNT; ++i)
  {
    vertexPrefix.push_back(std::string(VertexPredefines[i]));
    fragmentPrefix.push_back(std::string(FragmentPredefines[i]));
    shaderName.push_back(std::string(Scripting::GetLinearEnumerationName<VisualFactoryCache::ShaderType>(ShaderTypePredefines[i], VISUAL_SHADER_TYPE_TABLE, VISUAL_SHADER_TYPE_TABLE_COUNT)));
    shaderCount++;
  }

  shaders.vertexPrefix   = std::move(vertexPrefix);
  shaders.fragmentPrefix = std::move(fragmentPrefix);
  shaders.shaderName     = std::move(shaderName);
  shaders.vertexShader   = SHADER_COLOR_VISUAL_SHADER_VERT;
  shaders.fragmentShader = SHADER_COLOR_VISUAL_SHADER_FRAG;
  shaders.shaderCount    = shaderCount;
  shaders.custom         = false;
}

void ColorVisualShaderFactory::CreatePrecompileShader(ColorVisualShaderFeature::FeatureBuilder& builder, const ShaderFlagList& option)
{
  for(uint32_t i = 0; i < option.size(); ++i)
  {
    switch(option[i])
    {
      case PrecompileShaderOption::Flag::ROUNDED_CORNER:
      {
        builder.EnableRoundCorner(true, false);
        break;
      }
      case PrecompileShaderOption::Flag::SQUIRCLE_CORNER:
      {
        builder.EnableRoundCorner(true, true);
        break;
      }
      case PrecompileShaderOption::Flag::BORDERLINE:
      {
        builder.EnableBorderLine(true);
        break;
      }
      case PrecompileShaderOption::Flag::BLUR_EDGE:
      {
        builder.EnableBlur(true);
        break;
      }
      case PrecompileShaderOption::Flag::CUTOUT:
      {
        builder.EnableCutout(true);
        break;
      }
      default:
      {
        DALI_LOG_ERROR("Unknown option[%d]. maybe this type can't use this flag\n", static_cast<int>(option[i]));
        break;
      }
    }
  }
}

bool ColorVisualShaderFactory::SavePrecompileShader(VisualFactoryCache::ShaderType shader, std::string&& vertexPrefix, std::string&& fragmentPrefix)
{
  for(uint32_t i = 0u; i < PREDEFINED_SHADER_TYPE_COUNT; i++)
  {
    if(ShaderTypePredefines[i] == shader)
    {
      DALI_LOG_DEBUG_INFO("This shader already added list(%s).\n", Scripting::GetLinearEnumerationName<VisualFactoryCache::ShaderType>(ShaderTypePredefines[i], VISUAL_SHADER_TYPE_TABLE, VISUAL_SHADER_TYPE_TABLE_COUNT));
      return false;
    }
  }

  for(uint32_t i = 0u; i < mRequestedPrecompileShader.size(); i++)
  {
    if(mRequestedPrecompileShader[i].type == shader)
    {
      DALI_LOG_DEBUG_INFO("This shader already requsted(%s).\n", Scripting::GetLinearEnumerationName<VisualFactoryCache::ShaderType>(mRequestedPrecompileShader[i].type, VISUAL_SHADER_TYPE_TABLE, VISUAL_SHADER_TYPE_TABLE_COUNT));
      return false;
    }
  }

  RequestShaderInfo info;
  info.type           = shader;
  info.vertexPrefix   = std::move(vertexPrefix);
  info.fragmentPrefix = std::move(fragmentPrefix);
  mRequestedPrecompileShader.emplace_back(std::move(info));
  DALI_LOG_RELEASE_INFO("Add precompile shader success!!(%s)\n", Scripting::GetLinearEnumerationName<VisualFactoryCache::ShaderType>(shader, VISUAL_SHADER_TYPE_TABLE, VISUAL_SHADER_TYPE_TABLE_COUNT));
  return true;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

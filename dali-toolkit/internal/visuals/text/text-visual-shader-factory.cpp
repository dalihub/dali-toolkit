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
#include <dali-toolkit/internal/visuals/text/text-visual-shader-factory.h>

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
// enum of required list when we select shader
enum class TextVisualRequireFlag : uint32_t
{
  DEFAULT     = 0,
  STYLES      = 1 << 0,
  OVERLAY     = 1 << 1,
  EMOJI       = 1 << 2,
  MULTI_COLOR = 1 << 3,
};

const VisualFactoryCache::ShaderType SHADER_TYPE_TABLE[] =
  {
    VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT,
    VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_STYLE,
    VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_OVERLAY,
    VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_STYLE_AND_OVERLAY,
    VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_EMOJI,
    VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_STYLE_AND_EMOJI,
    VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_OVERLAY_AND_EMOJI,
    VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT_WITH_STYLE_AND_OVERLAY_AND_EMOJI,
    VisualFactoryCache::TEXT_SHADER_MULTI_COLOR_TEXT,
    VisualFactoryCache::TEXT_SHADER_MULTI_COLOR_TEXT_WITH_STYLE,
    VisualFactoryCache::TEXT_SHADER_MULTI_COLOR_TEXT_WITH_OVERLAY,
    VisualFactoryCache::TEXT_SHADER_MULTI_COLOR_TEXT_WITH_STYLE_AND_OVERLAY,
};

static constexpr auto      PREDEFINED_SHADER_TYPE_COUNT = 1u;
constexpr std::string_view VertexPredefines[PREDEFINED_SHADER_TYPE_COUNT]{
  "", // VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT
};
constexpr std::string_view FragmentPredefines[PREDEFINED_SHADER_TYPE_COUNT]{
  "", // VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT
};
constexpr VisualFactoryCache::ShaderType ShaderTypePredefines[PREDEFINED_SHADER_TYPE_COUNT]{
  VisualFactoryCache::ShaderType::TEXT_SHADER_SINGLE_COLOR_TEXT,
};

} // unnamed namespace

namespace TextVisualShaderFeature
{
FeatureBuilder::FeatureBuilder()
: mTextMultiColor(TextMultiColor::SINGLE_COLOR_TEXT),
  mTextEmoji(TextEmoji::NO_EMOJI),
  mTextStyle(TextStyle::NO_STYLES),
  mTextOverlay(TextOverlay::NO_OVERLAY)
{
}

FeatureBuilder& FeatureBuilder::EnableMultiColor(bool enableMultiColor)
{
  mTextMultiColor = enableMultiColor ? TextMultiColor::MULTI_COLOR_TEXT : TextMultiColor::SINGLE_COLOR_TEXT;
  return *this;
}
FeatureBuilder& FeatureBuilder::EnableEmoji(bool enableEmoji)
{
  mTextEmoji = enableEmoji ? TextEmoji::HAS_EMOJI : TextEmoji::NO_EMOJI;
  return *this;
}
FeatureBuilder& FeatureBuilder::EnableStyle(bool enableStyle)
{
  mTextStyle = enableStyle ? TextStyle::HAS_STYLES : TextStyle::NO_STYLES;
  return *this;
}
FeatureBuilder& FeatureBuilder::EnableOverlay(bool enableOverlay)
{
  mTextOverlay = enableOverlay ? TextOverlay::HAS_OVERLAY : TextOverlay::NO_OVERLAY;
  return *this;
}

VisualFactoryCache::ShaderType FeatureBuilder::GetShaderType() const
{
  uint32_t                       shaderTypeFlag = static_cast<uint32_t>(TextVisualRequireFlag::DEFAULT);
  VisualFactoryCache::ShaderType shaderType     = VisualFactoryCache::TEXT_SHADER_SINGLE_COLOR_TEXT;

  if(mTextStyle == TextVisualShaderFeature::TextStyle::HAS_STYLES)
  {
    shaderTypeFlag |= static_cast<uint32_t>(TextVisualRequireFlag::STYLES);
  }
  if(mTextOverlay == TextVisualShaderFeature::TextOverlay::HAS_OVERLAY)
  {
    shaderTypeFlag |= static_cast<uint32_t>(TextVisualRequireFlag::OVERLAY);
  }
  // multi color can also render emoji. If multi color text, dont consider emoji
  if(mTextMultiColor != TextVisualShaderFeature::TextMultiColor::MULTI_COLOR_TEXT && mTextEmoji == TextVisualShaderFeature::TextEmoji::HAS_EMOJI)
  {
    shaderTypeFlag |= static_cast<uint32_t>(TextVisualRequireFlag::EMOJI);
  }
  if(mTextMultiColor == TextVisualShaderFeature::TextMultiColor::MULTI_COLOR_TEXT)
  {
    shaderTypeFlag |= static_cast<uint32_t>(TextVisualRequireFlag::MULTI_COLOR);
  }

  shaderType = SHADER_TYPE_TABLE[shaderTypeFlag];
  return shaderType;
}

void FeatureBuilder::GetVertexShaderPrefixList(std::string& vertexShaderPrefixList) const
{
  // Do nothing
}

void FeatureBuilder::GetFragmentShaderPrefixList(std::string& fragmentShaderPrefixList) const
{
  if(mTextStyle == TextVisualShaderFeature::TextStyle::HAS_STYLES)
  {
    fragmentShaderPrefixList += "#define IS_REQUIRED_STYLE\n";
  }
  if(mTextOverlay == TextVisualShaderFeature::TextOverlay::HAS_OVERLAY)
  {
    fragmentShaderPrefixList += "#define IS_REQUIRED_OVERLAY\n";
  }
  // multi color can also render emoji. If multi color text, dont consider emoji
  if(mTextMultiColor != TextVisualShaderFeature::TextMultiColor::MULTI_COLOR_TEXT && mTextEmoji == TextVisualShaderFeature::TextEmoji::HAS_EMOJI)
  {
    fragmentShaderPrefixList += "#define IS_REQUIRED_EMOJI\n";
  }
  if(mTextMultiColor == TextVisualShaderFeature::TextMultiColor::MULTI_COLOR_TEXT)
  {
    fragmentShaderPrefixList += "#define IS_REQUIRED_MULTI_COLOR\n";
  }
}

} // namespace TextVisualShaderFeature

TextVisualShaderFactory::TextVisualShaderFactory()
{
}

TextVisualShaderFactory::~TextVisualShaderFactory()
{
}

Shader TextVisualShaderFactory::GetShader(VisualFactoryCache& factoryCache, const TextVisualShaderFeature::FeatureBuilder& featureBuilder)
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

    std::string vertexShader   = std::string(Dali::Shader::GetVertexShaderPrefix() + vertexShaderPrefixList + SHADER_TEXT_VISUAL_SHADER_VERT.data());
    std::string fragmentShader = std::string(Dali::Shader::GetFragmentShaderPrefix() + fragmentShaderPrefixList + SHADER_TEXT_VISUAL_SHADER_FRAG.data());

    shader = factoryCache.GenerateAndSaveShader(shaderType, vertexShader, fragmentShader);
  }
  return shader;
}

bool TextVisualShaderFactory::AddPrecompiledShader(PrecompileShaderOption& option)
{
  ShaderFlagList shaderOption = option.GetShaderOptions();

  auto        featureBuilder = TextVisualShaderFeature::FeatureBuilder();
  std::string vertexPrefixList;
  std::string fragmentPrefixList;
  CreatePrecompileShader(featureBuilder, shaderOption);

  VisualFactoryCache::ShaderType type = featureBuilder.GetShaderType();
  featureBuilder.GetVertexShaderPrefixList(vertexPrefixList);
  featureBuilder.GetFragmentShaderPrefixList(fragmentPrefixList);
  return SavePrecompileShader(type, std::move(vertexPrefixList), std::move(fragmentPrefixList));
}

void TextVisualShaderFactory::GetPreCompiledShader(ShaderPreCompiler::RawShaderData& shaders)
{
  std::vector<std::string> vertexPrefix;
  std::vector<std::string> fragmentPrefix;
  std::vector<std::string> shaderName;

  uint32_t shaderCount = 0;

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
  shaders.vertexShader   = SHADER_TEXT_VISUAL_SHADER_VERT;
  shaders.fragmentShader = SHADER_TEXT_VISUAL_SHADER_FRAG;
  shaders.shaderCount    = shaderCount;
  shaders.custom         = false;
}

void TextVisualShaderFactory::CreatePrecompileShader(TextVisualShaderFeature::FeatureBuilder& builder, const ShaderFlagList& option)
{
  for(uint32_t i = 0; i < option.size(); ++i)
  {
    switch(option[i])
    {
      case PrecompileShaderOption::Flag::STYLES:
      {
        builder.EnableStyle(true);
        break;
      }
      case PrecompileShaderOption::Flag::OVERLAY:
      {
        builder.EnableOverlay(true);
        break;
      }
      case PrecompileShaderOption::Flag::EMOJI:
      {
        builder.EnableEmoji(true);
        break;
      }
      case PrecompileShaderOption::Flag::MULTI_COLOR:
      {
        builder.EnableMultiColor(true);
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

bool TextVisualShaderFactory::SavePrecompileShader(VisualFactoryCache::ShaderType shader, std::string&& vertexPrefix, std::string&& fragmentPrefix)
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

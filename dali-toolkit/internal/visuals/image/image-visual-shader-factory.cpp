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
#include <dali-toolkit/internal/visuals/image/image-visual-shader-factory.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/rendering/texture-devel.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/visuals/image/image-visual-shader-debug.h>
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
const Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);

constexpr float ALPHA_PRE_MULTIPLIED(1.0f);

constexpr int CUSTOM_PROPERTY_COUNT(2); // PixelArea, pre-multiplied alpha

constexpr int              NATIVE_SHADER_TYPE_OFFSET = VisualFactoryCache::ShaderType::NATIVE_IMAGE_SHADER - VisualFactoryCache::ShaderType::IMAGE_SHADER;
constexpr std::string_view Y_FLIP_MASK_TEXTURE       = "uYFlipMaskTexture";
constexpr float            NOT_FLIP_MASK_TEXTURE     = 0.0f;

constexpr auto PREDEFINED_SHADER_TYPE_COUNT = 6u;

constexpr std::string_view VertexPredefines[PREDEFINED_SHADER_TYPE_COUNT]{
  "",                                     // VisualFactoryCache::IMAGE_SHADER,
  "#define IS_REQUIRED_ROUNDED_CORNER\n", // VisualFactoryCache::IMAGE_SHADER_ROUNDED_CORNER,
  "",                                     // VisualFactoryCache::IMAGE_SHADER_YUV_TO_RGB,
  "#define IS_REQUIRED_ROUNDED_CORNER\n", // VisualFactoryCache::IMAGE_SHADER_ROUNDED_CORNER_YUV_TO_RGB,
  "",                                     // VisualFactoryCache::IMAGE_SHADER_YUV_AND_RGB,
  "#define IS_REQUIRED_ROUNDED_CORNER\n", // VisualFactoryCache::IMAGE_SHADER_ROUNDED_CORNER_YUV_AND_RGB,
};
constexpr std::string_view FragmentPredefines[PREDEFINED_SHADER_TYPE_COUNT]{
  "",                                                                              // VisualFactoryCache::IMAGE_SHADER,
  "#define IS_REQUIRED_ROUNDED_CORNER\n",                                          // VisualFactoryCache::IMAGE_SHADER_ROUNDED_CORNER,
  "#define IS_REQUIRED_YUV_TO_RGB\n",                                              // VisualFactoryCache::IMAGE_SHADER_YUV_TO_RGB,
  "#define IS_REQUIRED_ROUNDED_CORNER\n#define IS_REQUIRED_YUV_TO_RGB\n",          // VisualFactoryCache::IMAGE_SHADER_ROUNDED_CORNER_YUV_TO_RGB,
  "#define IS_REQUIRED_UNIFIED_YUV_AND_RGB\n",                                     // VisualFactoryCache::IMAGE_SHADER_YUV_AND_RGB,
  "#define IS_REQUIRED_ROUNDED_CORNER\n#define IS_REQUIRED_UNIFIED_YUV_AND_RGB\n", // VisualFactoryCache::IMAGE_SHADER_ROUNDED_CORNER_YUV_AND_RGB,
};
constexpr VisualFactoryCache::ShaderType ShaderTypePredefines[PREDEFINED_SHADER_TYPE_COUNT]{
  VisualFactoryCache::ShaderType::IMAGE_SHADER,
  VisualFactoryCache::ShaderType::IMAGE_SHADER_ROUNDED_CORNER,
  VisualFactoryCache::ShaderType::IMAGE_SHADER_YUV_TO_RGB,
  VisualFactoryCache::ShaderType::IMAGE_SHADER_ROUNDED_CORNER_YUV_TO_RGB,
  VisualFactoryCache::ShaderType::IMAGE_SHADER_YUV_AND_RGB,
  VisualFactoryCache::ShaderType::IMAGE_SHADER_ROUNDED_CORNER_YUV_AND_RGB,
};
} // unnamed namespace

ImageVisualShaderFactory::ImageVisualShaderFactory()
: mFragmentShaderNeedChange(ImageVisualShaderFeature::ChangeFragmentShader::UNDECIDED)
{
}

ImageVisualShaderFactory::~ImageVisualShaderFactory()
{
}

Shader ImageVisualShaderFactory::GetShader(VisualFactoryCache& factoryCache, const ImageVisualShaderFeature::FeatureBuilder& featureBuilder)
{
  Shader                         shader;
  VisualFactoryCache::ShaderType shaderType = featureBuilder.GetShaderType();

  if(featureBuilder.NeedToChangeFragmentShader() == ImageVisualShaderFeature::ChangeFragmentShader::NEED_CHANGE &&
     (mFragmentShaderNeedChange == ImageVisualShaderFeature::ChangeFragmentShader::UNDECIDED ||
      mFragmentShaderNeedChange == ImageVisualShaderFeature::ChangeFragmentShader::NEED_CHANGE))
  {
    DALI_ASSERT_DEBUG((static_cast<int>(shaderType) >= static_cast<int>(VisualFactoryCache::IMAGE_SHADER)) &&
                      (static_cast<int>(shaderType) <= static_cast<int>(VisualFactoryCache::IMAGE_SHADER_ATLAS_CUSTOM_WRAP)) &&
                      "Do not support native image shader for given feature!!");
    shaderType = static_cast<VisualFactoryCache::ShaderType>(static_cast<int>(shaderType) + NATIVE_SHADER_TYPE_OFFSET);
  }

  shader = factoryCache.GetShader(shaderType);
  if(shader)
  {
    return shader;
  }

  std::string vertexShaderPrefixList;
  std::string fragmentShaderPrefixList;
  featureBuilder.GetVertexShaderPrefixList(vertexShaderPrefixList);
  featureBuilder.GetFragmentShaderPrefixList(fragmentShaderPrefixList);

  if(Dali::Toolkit::Internal::ImageVisualShaderDebug::DebugImageVisualShaderEnabled())
  {
    vertexShaderPrefixList += "#define IS_REQUIRED_DEBUG_VISUAL_SHADER\n";
    fragmentShaderPrefixList += "#define IS_REQUIRED_DEBUG_VISUAL_SHADER\n";
  }

  std::string vertexShader   = std::string(Dali::Shader::GetVertexShaderPrefix() + vertexShaderPrefixList + SHADER_IMAGE_VISUAL_SHADER_VERT.data());
  std::string fragmentShader = std::string(Dali::Shader::GetFragmentShaderPrefix() + fragmentShaderPrefixList + SHADER_IMAGE_VISUAL_SHADER_FRAG.data());

  if(Dali::Toolkit::Internal::ImageVisualShaderDebug::DebugImageVisualShaderEnabled())
  {
    Dali::Toolkit::Internal::ImageVisualShaderDebug::ApplyImageVisualShaderDebugScriptCode(vertexShader, fragmentShader);
  }

  if(featureBuilder.NeedToChangeFragmentShader() == ImageVisualShaderFeature::ChangeFragmentShader::NEED_CHANGE)
  {
    bool modified = DevelTexture::ApplyNativeFragmentShader(featureBuilder.GetTexture(), fragmentShader);
    if(mFragmentShaderNeedChange == ImageVisualShaderFeature::ChangeFragmentShader::NEED_CHANGE)
    {
      DALI_ASSERT_ALWAYS(modified && "NativeImageTexture need to change fragment shader. But DALI default image shader doesn't changed!");
    }
    else if(DALI_UNLIKELY(mFragmentShaderNeedChange == ImageVisualShaderFeature::ChangeFragmentShader::UNDECIDED))
    {
      mFragmentShaderNeedChange = (modified) ? ImageVisualShaderFeature::ChangeFragmentShader::NEED_CHANGE : ImageVisualShaderFeature::ChangeFragmentShader::DONT_CHANGE;

      if(mFragmentShaderNeedChange == ImageVisualShaderFeature::ChangeFragmentShader::DONT_CHANGE)
      {
        shaderType = static_cast<VisualFactoryCache::ShaderType>(static_cast<int>(shaderType) - NATIVE_SHADER_TYPE_OFFSET);
        shader     = factoryCache.GetShader(shaderType);
      }
    }
  }

  if(shader)
  {
    return shader;
  }

  shader = factoryCache.GenerateAndSaveShader(shaderType, vertexShader, fragmentShader);

  shader.ReserveCustomProperties(CUSTOM_PROPERTY_COUNT +
                                 (featureBuilder.IsEnabledAlphaMaskingOnRendering() ? 1 : 0));

  shader.RegisterProperty(PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT);

  // Most of image visual shader user (like svg, animated vector image visual) use pre-multiplied alpha.
  // If the visual dont want to using pre-multiplied alpha, it should be set as 0.0f as renderer side.
  shader.RegisterProperty(PREMULTIPLIED_ALPHA, ALPHA_PRE_MULTIPLIED);

  if(featureBuilder.IsEnabledAlphaMaskingOnRendering())
  {
    shader.RegisterProperty(Y_FLIP_MASK_TEXTURE, NOT_FLIP_MASK_TEXTURE);
  }

  return shader;
}

std::string_view ImageVisualShaderFactory::GetVertexShaderSource()
{
  // static string variable to cache complete vertex shader
  static std::string gVertexShader;
  if(gVertexShader.empty())
  {
    gVertexShader = Dali::Shader::GetVertexShaderPrefix() + SHADER_IMAGE_VISUAL_SHADER_VERT.data();
  }

  return gVertexShader;
}

std::string_view ImageVisualShaderFactory::GetFragmentShaderSource()
{
  // static string variable to cache complete fragment shader (no atlas)
  static std::string gFragmentShaderNoAtlas;
  if(gFragmentShaderNoAtlas.empty())
  {
    gFragmentShaderNoAtlas = Dali::Shader::GetFragmentShaderPrefix() + SHADER_IMAGE_VISUAL_SHADER_FRAG.data();
  }
  return gFragmentShaderNoAtlas;
}

bool ImageVisualShaderFactory::AddPrecompiledShader(PrecompileShaderOption& option)
{
  ShaderFlagList shaderOption = option.GetShaderOptions();

  auto        featureBuilder = ImageVisualShaderFeature::FeatureBuilder();
  std::string vertexPrefixList;
  std::string fragmentPrefixList;
  CreatePrecompileShader(featureBuilder, shaderOption);

  VisualFactoryCache::ShaderType type = featureBuilder.GetShaderType();
  featureBuilder.GetVertexShaderPrefixList(vertexPrefixList);
  featureBuilder.GetFragmentShaderPrefixList(fragmentPrefixList);
  return SavePrecompileShader(type, vertexPrefixList, fragmentPrefixList);
}

void ImageVisualShaderFactory::GetPreCompiledShader(RawShaderData& shaders)
{
  std::vector<std::string_view> vertexPrefix;
  std::vector<std::string_view> fragmentPrefix;
  std::vector<std::string_view> shaderName;
  shaders.shaderCount = 0;
  int shaderCount     = 0;

  for(uint32_t i = 0u; i < mRequestedPrecompileShader.size(); i++)
  {
    vertexPrefix.push_back(mRequestedPrecompileShader[i].vertexPrefix);
    fragmentPrefix.push_back(mRequestedPrecompileShader[i].fragmentPrefix);
    shaderName.push_back(Scripting::GetLinearEnumerationName<VisualFactoryCache::ShaderType>(mRequestedPrecompileShader[i].type, VISUAL_SHADER_TYPE_TABLE, VISUAL_SHADER_TYPE_TABLE_COUNT));
    shaderCount++;
  }

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
  shaders.vertexShader   = SHADER_IMAGE_VISUAL_SHADER_VERT;
  shaders.fragmentShader = SHADER_IMAGE_VISUAL_SHADER_FRAG;
  shaders.shaderCount    = shaderCount;
  shaders.custom         = false;
}

void ImageVisualShaderFactory::CreatePrecompileShader(ImageVisualShaderFeature::FeatureBuilder& builder, const ShaderFlagList& option)
{
  for(uint32_t i = 0; i < option.size(); ++i)
  {
    if(option[i] == PrecompileShaderOption::Flag::ATLAS_DEFAULT)
    {
      builder.EnableTextureAtlas(true);
      builder.ApplyDefaultTextureWrapMode(true);
    }
    else if(option[i] == PrecompileShaderOption::Flag::ATLAS_CUSTOM)
    {
      builder.EnableTextureAtlas(true);
      builder.ApplyDefaultTextureWrapMode(false);
    }
    else if(option[i] == PrecompileShaderOption::Flag::ROUNDED_CORNER)
    {
      builder.EnableRoundedCorner(true);
    }
    else if(option[i] == PrecompileShaderOption::Flag::BORDERLINE)
    {
      builder.EnableBorderline(true);
    }
    else if(option[i] == PrecompileShaderOption::Flag::MASKING)
    {
      builder.EnableAlphaMaskingOnRendering(true);
    }
    else if(option[i] == PrecompileShaderOption::Flag::YUV_TO_RGB)
    {
      builder.EnableYuvToRgb(true, false);
    }
    else if(option[i] == PrecompileShaderOption::Flag::YUV_AND_RGB)
    {
      builder.EnableYuvToRgb(false, true);
    }
  }
}

bool ImageVisualShaderFactory::SavePrecompileShader(VisualFactoryCache::ShaderType shader, std::string& vertexPrefix, std::string& fragmentPrefix)
{
  for(uint32_t i = 0u; i < PREDEFINED_SHADER_TYPE_COUNT; i++)
  {
    if(ShaderTypePredefines[i] == shader)
    {
      DALI_LOG_WARNING("This shader already added list(%s).", Scripting::GetLinearEnumerationName<VisualFactoryCache::ShaderType>(ShaderTypePredefines[i], VISUAL_SHADER_TYPE_TABLE, VISUAL_SHADER_TYPE_TABLE_COUNT));
      return false;
    }
  }

  for(uint32_t i = 0u; i < mRequestedPrecompileShader.size(); i++)
  {
    if(mRequestedPrecompileShader[i].type == shader)
    {
      DALI_LOG_WARNING("This shader already requsted(%s).", Scripting::GetLinearEnumerationName<VisualFactoryCache::ShaderType>(mRequestedPrecompileShader[i].type, VISUAL_SHADER_TYPE_TABLE, VISUAL_SHADER_TYPE_TABLE_COUNT));
      return false;
    }
  }

  RequestShaderInfo info;
  info.type           = shader;
  info.vertexPrefix   = vertexPrefix;
  info.fragmentPrefix = fragmentPrefix;
  mRequestedPrecompileShader.push_back(info);
  DALI_LOG_RELEASE_INFO("Add precompile shader success!!(%s)", Scripting::GetLinearEnumerationName<VisualFactoryCache::ShaderType>(shader, VISUAL_SHADER_TYPE_TABLE, VISUAL_SHADER_TYPE_TABLE_COUNT));
  return true;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

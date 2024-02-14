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
#include <dali-toolkit/internal/visuals/image-visual-shader-factory.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/rendering/texture-devel.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/visuals/image-visual-shader-debug.h>
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

constexpr int              NATIVE_SHADER_TYPE_OFFSET = VisualFactoryCache::ShaderType::NATIVE_IMAGE_SHADER - VisualFactoryCache::ShaderType::IMAGE_SHADER;
constexpr std::string_view Y_FLIP_MASK_TEXTURE       = "uYFlipMaskTexture";
constexpr float            NOT_FLIP_MASK_TEXTURE     = 0.0f;

constexpr auto SHADER_TYPE_COUNT = 6u;

constexpr std::string_view VertexPredefines[SHADER_TYPE_COUNT]{
  "",                                     // VisualFactoryCache::IMAGE_SHADER,
  "#define IS_REQUIRED_ROUNDED_CORNER\n", // VisualFactoryCache::IMAGE_SHADER_ROUNDED_CORNER,
  "",                                     // VisualFactoryCache::IMAGE_SHADER_YUV_TO_RGB,
  "#define IS_REQUIRED_ROUNDED_CORNER\n", // VisualFactoryCache::IMAGE_SHADER_ROUNDED_CORNER_YUV_TO_RGB,
  "",                                     // VisualFactoryCache::IMAGE_SHADER_YUV_AND_RGB,
  "#define IS_REQUIRED_ROUNDED_CORNER\n", // VisualFactoryCache::IMAGE_SHADER_ROUNDED_CORNER_YUV_AND_RGB,
};
constexpr std::string_view FragmentPredefines[SHADER_TYPE_COUNT]{
  "",                                                                              // VisualFactoryCache::IMAGE_SHADER,
  "#define IS_REQUIRED_ROUNDED_CORNER\n",                                          // VisualFactoryCache::IMAGE_SHADER_ROUNDED_CORNER,
  "#define IS_REQUIRED_YUV_TO_RGB\n",                                              // VisualFactoryCache::IMAGE_SHADER_YUV_TO_RGB,
  "#define IS_REQUIRED_ROUNDED_CORNER\n#define IS_REQUIRED_YUV_TO_RGB\n",          // VisualFactoryCache::IMAGE_SHADER_ROUNDED_CORNER_YUV_TO_RGB,
  "#define IS_REQUIRED_UNIFIED_YUV_AND_RGB\n",                                     // VisualFactoryCache::IMAGE_SHADER_YUV_AND_RGB,
  "#define IS_REQUIRED_ROUNDED_CORNER\n#define IS_REQUIRED_UNIFIED_YUV_AND_RGB\n", // VisualFactoryCache::IMAGE_SHADER_ROUNDED_CORNER_YUV_AND_RGB,
};
constexpr VisualFactoryCache::ShaderType ShaderTypePredefines[SHADER_TYPE_COUNT]{
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

Shader ImageVisualShaderFactory::GetShader(VisualFactoryCache& factoryCache, ImageVisualShaderFeatureBuilder& featureBuilder)
{
  Shader                         shader;
  VisualFactoryCache::ShaderType shaderType = featureBuilder.GetShaderType();

  if(featureBuilder.NeedToChangeFragmentShader() == ImageVisualShaderFeature::ChangeFragmentShader::NEED_CHANGE &&
     (mFragmentShaderNeedChange == ImageVisualShaderFeature::ChangeFragmentShader::UNDECIDED ||
      mFragmentShaderNeedChange == ImageVisualShaderFeature::ChangeFragmentShader::NEED_CHANGE))
  {
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
    Dali::Toolkit::Internal::ImageVisualShaderDebug::ApplyImageVisualShaderDebugScriptCode(fragmentShader);
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
  shader.RegisterProperty(PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT);
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

void ImageVisualShaderFactory::GetPreCompiledShader(RawShaderData& shaders)
{
  std::vector<std::string_view> vertexPrefix;
  std::vector<std::string_view> fragmentPrefix;
  std::vector<std::string_view> shaderName;
  shaders.shaderCount = 0;
  int shaderCount     = 0;
  for(uint32_t i = 0; i < SHADER_TYPE_COUNT; ++i)
  {
    vertexPrefix.push_back(VertexPredefines[i]);
    fragmentPrefix.push_back(FragmentPredefines[i]);
    shaderName.push_back(Scripting::GetLinearEnumerationName<VisualFactoryCache::ShaderType>(ShaderTypePredefines[i], VISUAL_SHADER_TYPE_TABLE, VISUAL_SHADER_TYPE_TABLE_COUNT));
    shaderCount++;
  }

  shaders.vertexPrefix   = vertexPrefix;
  shaders.fragmentPrefix = fragmentPrefix;
  shaders.shaderName     = shaderName;
  shaders.vertexShader   = SHADER_IMAGE_VISUAL_SHADER_VERT;
  shaders.fragmentShader = SHADER_IMAGE_VISUAL_SHADER_FRAG;
  shaders.shaderCount    = shaderCount;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

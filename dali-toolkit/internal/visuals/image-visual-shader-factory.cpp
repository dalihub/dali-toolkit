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

// CLASS HEADER
#include <dali-toolkit/internal/visuals/image-visual-shader-factory.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/rendering/texture-devel.h>

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
const Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);

// global string variable to caching complate vertex shader
static std::string gVertexShader;

// global string variable to caching complate fragment shader (no atlas)
static std::string gFragmentShaderNoAtlas;

const int NATIVE_SHADER_TYPE_OFFSET = VisualFactoryCache::ShaderType::NATIVE_IMAGE_SHADER - VisualFactoryCache::ShaderType::IMAGE_SHADER;

// enum of required list when we select shader
enum class ImageVisualRequireFlag : uint32_t
{
  DEFAULT         = 0,
  ROUNDED_CORNER  = 1 << 0,
  BORDERLINE      = 1 << 1,
  ALPHA_MASKING = 1 << 2,
};

static constexpr auto SHADER_TYPE_COUNT = 8u;
VisualFactoryCache::ShaderType SHADER_TYPE_TABLE[SHADER_TYPE_COUNT] =
{
  VisualFactoryCache::IMAGE_SHADER,
  VisualFactoryCache::IMAGE_SHADER_ROUNDED_CORNER,
  VisualFactoryCache::IMAGE_SHADER_BORDERLINE,
  VisualFactoryCache::IMAGE_SHADER_ROUNDED_BORDERLINE,
  VisualFactoryCache::IMAGE_SHADER_MASKING,
  VisualFactoryCache::IMAGE_SHADER_ROUNDED_CORNER_MASKING,
  VisualFactoryCache::IMAGE_SHADER_BORDERLINE_MASKING,
  VisualFactoryCache::IMAGE_SHADER_ROUNDED_BORDERLINE_MASKING
};

} // unnamed namespace

namespace ImageVisualShaderFeature
{
FeatureBuilder& FeatureBuilder::EnableTextureAtlas(bool enableAtlas)
{
  mTextureAtlas = (enableAtlas ? TextureAtlas::ENABLED : TextureAtlas::DISABLED);
  return *this;
}
FeatureBuilder& FeatureBuilder::ApplyDefaultTextureWrapMode(bool applyDefaultTextureWrapMode)
{
  mDefaultTextureWrapMode = (applyDefaultTextureWrapMode ? DefaultTextureWrapMode::APPLY : DefaultTextureWrapMode::DO_NOT_APPLY);
  return *this;
}
FeatureBuilder& FeatureBuilder::EnableRoundedCorner(bool enableRoundedCorner)
{
  mRoundedCorner = (enableRoundedCorner ? RoundedCorner::ENABLED : RoundedCorner::DISABLED);
  return *this;
}
FeatureBuilder& FeatureBuilder::EnableBorderline(bool enableBorderline)
{
  mBorderline = (enableBorderline ? Borderline::ENABLED : Borderline::DISABLED);
  return *this;
}
FeatureBuilder& FeatureBuilder::SetTextureForFragmentShaderCheck(const Dali::Texture& texture)
{
  mTexture = texture;
  return *this;
}
FeatureBuilder& FeatureBuilder::EnableAlphaMasking(bool enableAlphaMasking)
{
  mAlphaMasking = (enableAlphaMasking ? AlphaMasking::ENABLED : AlphaMasking::DISABLED);
  return *this;
}
} // namespace ImageVisualShaderFeature

ImageVisualShaderFactory::ImageVisualShaderFactory()
: mFragmentShaderNeedChange(ImageVisualShaderFeature::ChangeFragmentShader::UNDECIDED)
{
}

ImageVisualShaderFactory::~ImageVisualShaderFactory()
{
}

Shader ImageVisualShaderFactory::GetShader(VisualFactoryCache& factoryCache, const ImageVisualShaderFeature::FeatureBuilder& featureBuilder)
{
  Shader shader;
  VisualFactoryCache::ShaderType shaderType = VisualFactoryCache::IMAGE_SHADER;

  const auto& atlasing               = featureBuilder.mTextureAtlas;
  const auto& defaultTextureWrapping = featureBuilder.mDefaultTextureWrapMode;
  const auto& roundedCorner          = featureBuilder.mRoundedCorner;
  const auto& borderline             = featureBuilder.mBorderline;
  const auto& alphaMasking           = featureBuilder.mAlphaMasking;
  const auto& changeFragmentShader   = (featureBuilder.mTexture && DevelTexture::IsNative(featureBuilder.mTexture))
                                         ? ImageVisualShaderFeature::ChangeFragmentShader::NEED_CHANGE
                                         : ImageVisualShaderFeature::ChangeFragmentShader::DONT_CHANGE;

  if(atlasing == ImageVisualShaderFeature::TextureAtlas::ENABLED)
  {
    if(defaultTextureWrapping == ImageVisualShaderFeature::DefaultTextureWrapMode::APPLY)
    {
      shaderType = VisualFactoryCache::IMAGE_SHADER_ATLAS_DEFAULT_WRAP;
    }
    else
    {
      shaderType = VisualFactoryCache::IMAGE_SHADER_ATLAS_CUSTOM_WRAP;
    }
  }
  else
  {
    uint32_t shaderTypeFlag = static_cast<uint32_t>(ImageVisualRequireFlag::DEFAULT);
    if(roundedCorner == ImageVisualShaderFeature::RoundedCorner::ENABLED)
    {
      shaderTypeFlag |= static_cast<uint32_t>(ImageVisualRequireFlag::ROUNDED_CORNER);
    }
    if(borderline == ImageVisualShaderFeature::Borderline::ENABLED)
    {
      shaderTypeFlag |= static_cast<uint32_t>(ImageVisualRequireFlag::BORDERLINE);
    }
    if(alphaMasking == ImageVisualShaderFeature::AlphaMasking::ENABLED)
    {
      shaderTypeFlag |= static_cast<uint32_t>(ImageVisualRequireFlag::ALPHA_MASKING);
    }
    shaderType = SHADER_TYPE_TABLE[shaderTypeFlag];
  }

  if(changeFragmentShader == ImageVisualShaderFeature::ChangeFragmentShader::NEED_CHANGE &&
     (mFragmentShaderNeedChange == ImageVisualShaderFeature::ChangeFragmentShader::UNDECIDED ||
      mFragmentShaderNeedChange == ImageVisualShaderFeature::ChangeFragmentShader::NEED_CHANGE))
  {
    shaderType = static_cast<VisualFactoryCache::ShaderType>(static_cast<int>(shaderType) + NATIVE_SHADER_TYPE_OFFSET);
  }

  shader = factoryCache.GetShader(shaderType);
  if(!shader)
  {
    std::string vertexShaderPrefixList;
    std::string fragmentShaderPrefixList;
    if(atlasing == ImageVisualShaderFeature::TextureAtlas::ENABLED)
    {
      if(defaultTextureWrapping == ImageVisualShaderFeature::DefaultTextureWrapMode::APPLY)
      {
        fragmentShaderPrefixList += "#define ATLAS_DEFAULT_WARP 1\n";
      }
      else
      {
        fragmentShaderPrefixList += "#define ATLAS_CUSTOM_WARP 1\n";
      }
    }
    else
    {
      if(roundedCorner == ImageVisualShaderFeature::RoundedCorner::ENABLED)
      {
        vertexShaderPrefixList   += "#define IS_REQUIRED_ROUNDED_CORNER 1\n";
        fragmentShaderPrefixList += "#define IS_REQUIRED_ROUNDED_CORNER 1\n";
      }
      if(borderline == ImageVisualShaderFeature::Borderline::ENABLED)
      {
        vertexShaderPrefixList   += "#define IS_REQUIRED_BORDERLINE 1\n";
        fragmentShaderPrefixList += "#define IS_REQUIRED_BORDERLINE 1\n";
      }
      if(alphaMasking == ImageVisualShaderFeature::AlphaMasking::ENABLED)
      {
        vertexShaderPrefixList   += "#define IS_REQUIRED_ALPHA_MASKING 1\n";
        fragmentShaderPrefixList += "#define IS_REQUIRED_ALPHA_MASKING 1\n";
      }
    }

    std::string vertexShader   = std::string(Dali::Shader::GetVertexShaderPrefix()   + vertexShaderPrefixList   + SHADER_IMAGE_VISUAL_SHADER_VERT.data());
    std::string fragmentShader = std::string(Dali::Shader::GetFragmentShaderPrefix() + fragmentShaderPrefixList + SHADER_IMAGE_VISUAL_SHADER_FRAG.data());

    if(changeFragmentShader == ImageVisualShaderFeature::ChangeFragmentShader::NEED_CHANGE)
    {
      if(DALI_UNLIKELY(mFragmentShaderNeedChange == ImageVisualShaderFeature::ChangeFragmentShader::UNDECIDED))
      {
        // NOTE : This routine will run exist one times.
        //
        // First, we will run ApplyNativeFragmentShader
        //  - If fragment shader is modified, then current platform allow to change fragment shader.
        //    We cache this result mFragmentShaderNeedChange = ChangeFragmentShader::NEED_CHANGE.
        //  - If fragment shader is not modified, then current platform will always don't change fragment shader.
        //    We cache this result mFragmentShaderNeedChange = ChangeFragmentShader::DONT_CHANGE.
        //    And change current shaderType into normal image range.
        //    After cached the result, shaderType never become NATIVE_IMAGE_SHADER anymore.
        // Second, save shader result.

        // Try to apply fragmentShader
        bool modified = DevelTexture::ApplyNativeFragmentShader(featureBuilder.mTexture, fragmentShader);
        if(modified)
        {
          // Now we know that fragment shader need to change.
          mFragmentShaderNeedChange = ImageVisualShaderFeature::ChangeFragmentShader::NEED_CHANGE;
        }
        else
        {
          // Now we know that fragment shader even don't need to change.
          // We can skip ApplyNativeFragmentShader routine after now.
          mFragmentShaderNeedChange = ImageVisualShaderFeature::ChangeFragmentShader::DONT_CHANGE;

          // Now we need normal shader type
          // So decrease NATIVE_SHADER_TYPE_OFFSET.
          shaderType = static_cast<VisualFactoryCache::ShaderType>(static_cast<int>(shaderType) - NATIVE_SHADER_TYPE_OFFSET);

          // If we already compiled this type already, just use that cached shader.
          // Else, just go forward.
          shader = factoryCache.GetShader(shaderType);
          if(shader)
          {
            return shader;
          }
        }
      }
      else if(mFragmentShaderNeedChange == ImageVisualShaderFeature::ChangeFragmentShader::NEED_CHANGE)
      {
        // Always need to apply fragmentShader
        bool modified = DevelTexture::ApplyNativeFragmentShader(featureBuilder.mTexture, fragmentShader);
        DALI_ASSERT_ALWAYS(modified && "NativeImageTexture need to change fragment shader. But DALI default image shader doesn't changed!");
      }
    }
    shader = Shader::New(vertexShader, fragmentShader);
    shader.RegisterProperty(PIXEL_AREA_UNIFORM_NAME, FULL_TEXTURE_RECT);
    factoryCache.SaveShader(shaderType, shader);
  }

  return shader;
}

std::string_view ImageVisualShaderFactory::GetVertexShaderSource()
{
  if(gVertexShader.empty())
  {
    gVertexShader = Dali::Shader::GetVertexShaderPrefix() + SHADER_IMAGE_VISUAL_SHADER_VERT.data();
  }

  return gVertexShader;
}

std::string_view ImageVisualShaderFactory::GetFragmentShaderSource()
{
  if(gFragmentShaderNoAtlas.empty())
  {
    gFragmentShaderNoAtlas = Dali::Shader::GetFragmentShaderPrefix() + SHADER_IMAGE_VISUAL_SHADER_FRAG.data();
  }
  return gFragmentShaderNoAtlas;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

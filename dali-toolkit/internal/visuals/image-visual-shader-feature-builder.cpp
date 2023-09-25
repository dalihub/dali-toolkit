/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/visuals/image-visual-shader-feature-builder.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
// enum of required list when we select shader
enum class ImageVisualRequireFlag : uint32_t
{
  DEFAULT          = 0,
  ROUNDED_CORNER   = 1 << 0,
  BORDERLINE       = 1 << 1,
  ALPHA_MASKING    = 1 << 2,
  COLOR_CONVERSION = 1 << 3,

  UNIFIED_YUV_AND_RGB = 1 << 2, // Special enum to trick unified YUV and RGB.
};

static constexpr auto          SHADER_TYPE_COUNT = 16u;
VisualFactoryCache::ShaderType SHADER_TYPE_TABLE[SHADER_TYPE_COUNT] =
  {
    VisualFactoryCache::IMAGE_SHADER,
    VisualFactoryCache::IMAGE_SHADER_ROUNDED_CORNER,
    VisualFactoryCache::IMAGE_SHADER_BORDERLINE,
    VisualFactoryCache::IMAGE_SHADER_ROUNDED_BORDERLINE,
    VisualFactoryCache::IMAGE_SHADER_MASKING,
    VisualFactoryCache::IMAGE_SHADER_ROUNDED_CORNER_MASKING,
    VisualFactoryCache::IMAGE_SHADER_BORDERLINE_MASKING,
    VisualFactoryCache::IMAGE_SHADER_ROUNDED_BORDERLINE_MASKING,
    VisualFactoryCache::IMAGE_SHADER_YUV_TO_RGB,
    VisualFactoryCache::IMAGE_SHADER_ROUNDED_CORNER_YUV_TO_RGB,
    VisualFactoryCache::IMAGE_SHADER_BORDERLINE_YUV_TO_RGB,
    VisualFactoryCache::IMAGE_SHADER_ROUNDED_BORDERLINE_YUV_TO_RGB,
    VisualFactoryCache::IMAGE_SHADER_YUV_AND_RGB,
    VisualFactoryCache::IMAGE_SHADER_ROUNDED_CORNER_YUV_AND_RGB,
    VisualFactoryCache::IMAGE_SHADER_BORDERLINE_YUV_AND_RGB,
    VisualFactoryCache::IMAGE_SHADER_ROUNDED_BORDERLINE_YUV_AND_RGB};
} // unnamed namespace

ImageVisualShaderFeatureBuilder::ImageVisualShaderFeatureBuilder()
: mTextureAtlas(ImageVisualShaderFeature::TextureAtlas::DISABLED),
  mDefaultTextureWrapMode(ImageVisualShaderFeature::DefaultTextureWrapMode::APPLY),
  mRoundedCorner(ImageVisualShaderFeature::RoundedCorner::DISABLED),
  mBorderline(ImageVisualShaderFeature::Borderline::DISABLED),
  mAlphaMaskingOnRendering(ImageVisualShaderFeature::AlphaMaskingOnRendering::DISABLED),
  mColorConversion(ImageVisualShaderFeature::ColorConversion::DONT_NEED),
  mTexture()
{
}

ImageVisualShaderFeatureBuilder& ImageVisualShaderFeatureBuilder::EnableTextureAtlas(bool enableTextureAtlas)
{
  mTextureAtlas = (enableTextureAtlas ? ImageVisualShaderFeature::TextureAtlas::ENABLED : ImageVisualShaderFeature::TextureAtlas::DISABLED);
  return *this;
}

ImageVisualShaderFeatureBuilder& ImageVisualShaderFeatureBuilder::ApplyDefaultTextureWrapMode(bool applyDefaultTextureWrapMode)
{
  mDefaultTextureWrapMode = (applyDefaultTextureWrapMode ? ImageVisualShaderFeature::DefaultTextureWrapMode::APPLY : ImageVisualShaderFeature::DefaultTextureWrapMode::DO_NOT_APPLY);
  return *this;
}

ImageVisualShaderFeatureBuilder& ImageVisualShaderFeatureBuilder::EnableRoundedCorner(bool enableRoundedCorner)
{
  mRoundedCorner = (enableRoundedCorner ? ImageVisualShaderFeature::RoundedCorner::ENABLED : ImageVisualShaderFeature::RoundedCorner::DISABLED);
  return *this;
}

ImageVisualShaderFeatureBuilder& ImageVisualShaderFeatureBuilder::EnableBorderline(bool enableBorderline)
{
  mBorderline = (enableBorderline ? ImageVisualShaderFeature::Borderline::ENABLED : ImageVisualShaderFeature::Borderline::DISABLED);
  return *this;
}

ImageVisualShaderFeatureBuilder& ImageVisualShaderFeatureBuilder::SetTextureForFragmentShaderCheck(const Dali::Texture& texture)
{
  mTexture = texture;
  return *this;
}

ImageVisualShaderFeatureBuilder& ImageVisualShaderFeatureBuilder::EnableAlphaMaskingOnRendering(bool enableAlphaMaskingOnRendering)
{
  mAlphaMaskingOnRendering = (enableAlphaMaskingOnRendering ? ImageVisualShaderFeature::AlphaMaskingOnRendering::ENABLED : ImageVisualShaderFeature::AlphaMaskingOnRendering::DISABLED);
  return *this;
}

ImageVisualShaderFeatureBuilder& ImageVisualShaderFeatureBuilder::EnableYuvToRgb(bool enableYuvToRgb, bool enableUnifiedYuvAndRgb)
{
  mColorConversion = (enableUnifiedYuvAndRgb ? ImageVisualShaderFeature::ColorConversion::UNIFIED_YUV_AND_RGB : (enableYuvToRgb ? ImageVisualShaderFeature::ColorConversion::YUV_TO_RGB : ImageVisualShaderFeature::ColorConversion::DONT_NEED));
  return *this;
}

VisualFactoryCache::ShaderType ImageVisualShaderFeatureBuilder::GetShaderType()
{
  VisualFactoryCache::ShaderType shaderType = VisualFactoryCache::IMAGE_SHADER;
  if(mTextureAtlas == ImageVisualShaderFeature::TextureAtlas::ENABLED)
  {
    if(mDefaultTextureWrapMode == ImageVisualShaderFeature::DefaultTextureWrapMode::APPLY)
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
    if(mRoundedCorner == ImageVisualShaderFeature::RoundedCorner::ENABLED)
    {
      shaderTypeFlag |= static_cast<uint32_t>(ImageVisualRequireFlag::ROUNDED_CORNER);
    }
    if(mBorderline == ImageVisualShaderFeature::Borderline::ENABLED)
    {
      shaderTypeFlag |= static_cast<uint32_t>(ImageVisualRequireFlag::BORDERLINE);
    }
    if(mAlphaMaskingOnRendering == ImageVisualShaderFeature::AlphaMaskingOnRendering::ENABLED)
    {
      shaderTypeFlag |= static_cast<uint32_t>(ImageVisualRequireFlag::ALPHA_MASKING);
    }
    else if(mColorConversion == ImageVisualShaderFeature::ColorConversion::YUV_TO_RGB) // Not support gpu masking and color conversion at the same time now
    {
      shaderTypeFlag |= static_cast<uint32_t>(ImageVisualRequireFlag::COLOR_CONVERSION);
    }
    else if(mColorConversion == ImageVisualShaderFeature::ColorConversion::UNIFIED_YUV_AND_RGB)
    {
      shaderTypeFlag |= static_cast<uint32_t>(ImageVisualRequireFlag::COLOR_CONVERSION);
      shaderTypeFlag |= static_cast<uint32_t>(ImageVisualRequireFlag::UNIFIED_YUV_AND_RGB);
    }
    shaderType = SHADER_TYPE_TABLE[shaderTypeFlag];
  }

  return shaderType;
}

ImageVisualShaderFeature::ChangeFragmentShader::Type ImageVisualShaderFeatureBuilder::NeedToChangeFragmentShader()
{
  return (mTexture && DevelTexture::IsNative(mTexture))
           ? ImageVisualShaderFeature::ChangeFragmentShader::NEED_CHANGE
           : ImageVisualShaderFeature::ChangeFragmentShader::DONT_CHANGE;
}

void ImageVisualShaderFeatureBuilder::GetVertexShaderPrefixList(std::string& vertexShaderPrefixList)
{
  if(mTextureAtlas != ImageVisualShaderFeature::TextureAtlas::ENABLED)
  {
    if(mRoundedCorner == ImageVisualShaderFeature::RoundedCorner::ENABLED)
    {
      vertexShaderPrefixList += "#define IS_REQUIRED_ROUNDED_CORNER\n";
    }
    if(mBorderline == ImageVisualShaderFeature::Borderline::ENABLED)
    {
      vertexShaderPrefixList += "#define IS_REQUIRED_BORDERLINE\n";
    }
    if(mAlphaMaskingOnRendering == ImageVisualShaderFeature::AlphaMaskingOnRendering::ENABLED)
    {
      vertexShaderPrefixList += "#define IS_REQUIRED_ALPHA_MASKING\n";
    }
  }
}

void ImageVisualShaderFeatureBuilder::GetFragmentShaderPrefixList(std::string& fragmentShaderPrefixList)
{
  if(mTextureAtlas == ImageVisualShaderFeature::TextureAtlas::ENABLED)
  {
    if(mDefaultTextureWrapMode == ImageVisualShaderFeature::DefaultTextureWrapMode::APPLY)
    {
      fragmentShaderPrefixList += "#define ATLAS_DEFAULT_WARP\n";
    }
    else
    {
      fragmentShaderPrefixList += "#define ATLAS_CUSTOM_WARP\n";
    }
  }
  else
  {
    if(mRoundedCorner == ImageVisualShaderFeature::RoundedCorner::ENABLED)
    {
      fragmentShaderPrefixList += "#define IS_REQUIRED_ROUNDED_CORNER\n";
    }
    if(mBorderline == ImageVisualShaderFeature::Borderline::ENABLED)
    {
      fragmentShaderPrefixList += "#define IS_REQUIRED_BORDERLINE\n";
    }
    if(mAlphaMaskingOnRendering == ImageVisualShaderFeature::AlphaMaskingOnRendering::ENABLED)
    {
      fragmentShaderPrefixList += "#define IS_REQUIRED_ALPHA_MASKING\n";
    }
    else if(mColorConversion == ImageVisualShaderFeature::ColorConversion::YUV_TO_RGB)
    {
      fragmentShaderPrefixList += "#define IS_REQUIRED_YUV_TO_RGB\n";
    }
    else if(mColorConversion == ImageVisualShaderFeature::ColorConversion::UNIFIED_YUV_AND_RGB)
    {
      fragmentShaderPrefixList += "#define IS_REQUIRED_UNIFIED_YUV_AND_RGB\n";
    }
  }
}

Dali::Texture ImageVisualShaderFeatureBuilder::GetTexture()
{
  return mTexture;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

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
#include <dali-toolkit/internal/visuals/image/image-visual-shader-feature-builder.h>

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
  DEFAULT         = 0,
  ROUNDED_CORNER  = 1,
  SQUIRCLE_CORNER = 2,

  BORDERLINE       = (1 << 0) * 3,
  ALPHA_MASKING    = (1 << 1) * 3,
  COLOR_CONVERSION = (1 << 2) * 3,

  UNIFIED_YUV_AND_RGB = (1 << 1) * 3, // Special enum to trick unified YUV and RGB.
};

VisualFactoryCache::ShaderType SHADER_TYPE_TABLE[] = {
  VisualFactoryCache::IMAGE_SHADER,
  VisualFactoryCache::IMAGE_SHADER_ROUNDED_CORNER,
  VisualFactoryCache::IMAGE_SHADER_SQUIRCLE_CORNER,
  VisualFactoryCache::IMAGE_SHADER_BORDERLINE,
  VisualFactoryCache::IMAGE_SHADER_ROUNDED_BORDERLINE,
  VisualFactoryCache::IMAGE_SHADER_SQUIRCLE_BORDERLINE,
  VisualFactoryCache::IMAGE_SHADER_MASKING,
  VisualFactoryCache::IMAGE_SHADER_ROUNDED_CORNER_MASKING,
  VisualFactoryCache::IMAGE_SHADER_SQUIRCLE_CORNER_MASKING,
  VisualFactoryCache::IMAGE_SHADER_BORDERLINE_MASKING,
  VisualFactoryCache::IMAGE_SHADER_ROUNDED_BORDERLINE_MASKING,
  VisualFactoryCache::IMAGE_SHADER_SQUIRCLE_BORDERLINE_MASKING,
  VisualFactoryCache::IMAGE_SHADER_YUV_TO_RGB,
  VisualFactoryCache::IMAGE_SHADER_ROUNDED_CORNER_YUV_TO_RGB,
  VisualFactoryCache::IMAGE_SHADER_SQUIRCLE_CORNER_YUV_TO_RGB,
  VisualFactoryCache::IMAGE_SHADER_BORDERLINE_YUV_TO_RGB,
  VisualFactoryCache::IMAGE_SHADER_ROUNDED_BORDERLINE_YUV_TO_RGB,
  VisualFactoryCache::IMAGE_SHADER_SQUIRCLE_BORDERLINE_YUV_TO_RGB,
  VisualFactoryCache::IMAGE_SHADER_YUV_AND_RGB,
  VisualFactoryCache::IMAGE_SHADER_ROUNDED_CORNER_YUV_AND_RGB,
  VisualFactoryCache::IMAGE_SHADER_SQUIRCLE_CORNER_YUV_AND_RGB,
  VisualFactoryCache::IMAGE_SHADER_BORDERLINE_YUV_AND_RGB,
  VisualFactoryCache::IMAGE_SHADER_ROUNDED_BORDERLINE_YUV_AND_RGB,
  VisualFactoryCache::IMAGE_SHADER_SQUIRCLE_BORDERLINE_YUV_AND_RGB,
};
constexpr uint32_t SHADER_TYPE_TABLE_COUNT = sizeof(SHADER_TYPE_TABLE) / sizeof(SHADER_TYPE_TABLE[0]);
} // unnamed namespace

namespace ImageVisualShaderFeature
{
FeatureBuilder::FeatureBuilder()
: mTextureAtlas(TextureAtlas::DISABLED),
  mDefaultTextureWrapMode(DefaultTextureWrapMode::APPLY),
  mRoundedCorner(RoundedCorner::DISABLED),
  mBorderline(Borderline::DISABLED),
  mAlphaMaskingOnRendering(AlphaMaskingOnRendering::DISABLED),
  mColorConversion(ColorConversion::DONT_NEED),
  mUseDefaultTransform(true),
  mTexture()
{
}

FeatureBuilder& FeatureBuilder::EnableTextureAtlas(bool enableTextureAtlas)
{
  mTextureAtlas = (enableTextureAtlas ? TextureAtlas::ENABLED : TextureAtlas::DISABLED);
  return *this;
}

FeatureBuilder& FeatureBuilder::ApplyDefaultTextureWrapMode(bool applyDefaultTextureWrapMode)
{
  mDefaultTextureWrapMode = (applyDefaultTextureWrapMode ? DefaultTextureWrapMode::APPLY : DefaultTextureWrapMode::DO_NOT_APPLY);
  return *this;
}

FeatureBuilder& FeatureBuilder::EnableRoundedCorner(bool enableRoundedCorner, bool enableSquircleCorner)
{
  mRoundedCorner = (enableRoundedCorner ? (enableSquircleCorner ? RoundedCorner::SQUIRCLE_CORNER : RoundedCorner::ROUNDED_CORNER) : RoundedCorner::DISABLED);
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

FeatureBuilder& FeatureBuilder::EnableAlphaMaskingOnRendering(bool enableAlphaMaskingOnRendering)
{
  mAlphaMaskingOnRendering = (enableAlphaMaskingOnRendering ? AlphaMaskingOnRendering::ENABLED : AlphaMaskingOnRendering::DISABLED);
  return *this;
}

FeatureBuilder& FeatureBuilder::EnableYuvToRgb(bool enableYuvToRgb, bool enableUnifiedYuvAndRgb)
{
  mColorConversion = (enableUnifiedYuvAndRgb ? ColorConversion::UNIFIED_YUV_AND_RGB : (enableYuvToRgb ? ColorConversion::YUV_TO_RGB : ColorConversion::DONT_NEED));
  return *this;
}

FeatureBuilder& FeatureBuilder::UseDefaultTransform(bool useDefaultTransform)
{
  mUseDefaultTransform = useDefaultTransform;
  return *this;
}

VisualFactoryCache::ShaderType FeatureBuilder::GetShaderType() const
{
  VisualFactoryCache::ShaderType shaderType = VisualFactoryCache::IMAGE_SHADER;
  if(mTextureAtlas == TextureAtlas::ENABLED)
  {
    if(mDefaultTextureWrapMode == DefaultTextureWrapMode::APPLY)
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
    if(mRoundedCorner == RoundedCorner::SQUIRCLE_CORNER)
    {
      shaderTypeFlag += static_cast<uint32_t>(ImageVisualRequireFlag::SQUIRCLE_CORNER);
    }
    else if(mRoundedCorner == RoundedCorner::ROUNDED_CORNER)
    {
      shaderTypeFlag += static_cast<uint32_t>(ImageVisualRequireFlag::ROUNDED_CORNER);
    }

    if(mBorderline == Borderline::ENABLED)
    {
      shaderTypeFlag += static_cast<uint32_t>(ImageVisualRequireFlag::BORDERLINE);
    }

    if(mAlphaMaskingOnRendering == AlphaMaskingOnRendering::ENABLED)
    {
      shaderTypeFlag += static_cast<uint32_t>(ImageVisualRequireFlag::ALPHA_MASKING);
    }
    else if(mColorConversion == ColorConversion::YUV_TO_RGB) // Not support gpu masking and color conversion at the same time now
    {
      shaderTypeFlag += static_cast<uint32_t>(ImageVisualRequireFlag::COLOR_CONVERSION);
    }
    else if(mColorConversion == ColorConversion::UNIFIED_YUV_AND_RGB)
    {
      shaderTypeFlag += static_cast<uint32_t>(ImageVisualRequireFlag::COLOR_CONVERSION);
      shaderTypeFlag += static_cast<uint32_t>(ImageVisualRequireFlag::UNIFIED_YUV_AND_RGB);
    }

    DALI_ASSERT_DEBUG(shaderTypeFlag < SHADER_TYPE_TABLE_COUNT && "Invalid image shader type generated!");

    shaderType = SHADER_TYPE_TABLE[shaderTypeFlag];
  }

  return shaderType;
}

ChangeFragmentShader::Type FeatureBuilder::NeedToChangeFragmentShader() const
{
  return (mTexture && DevelTexture::IsNative(mTexture))
           ? ChangeFragmentShader::NEED_CHANGE
           : ChangeFragmentShader::DONT_CHANGE;
}

void FeatureBuilder::GetVertexShaderPrefixList(std::string& vertexShaderPrefixList) const
{
  if(mTextureAtlas != TextureAtlas::ENABLED)
  {
    if(mRoundedCorner != RoundedCorner::DISABLED)
    {
      vertexShaderPrefixList += "#define IS_REQUIRED_ROUNDED_CORNER\n";
    }
    if(mBorderline == Borderline::ENABLED)
    {
      vertexShaderPrefixList += "#define IS_REQUIRED_BORDERLINE\n";
    }
    if(mAlphaMaskingOnRendering == AlphaMaskingOnRendering::ENABLED)
    {
      vertexShaderPrefixList += "#define IS_REQUIRED_ALPHA_MASKING\n";
    }
  }
}

void FeatureBuilder::GetFragmentShaderPrefixList(std::string& fragmentShaderPrefixList) const
{
  if(mTextureAtlas == TextureAtlas::ENABLED)
  {
    if(mDefaultTextureWrapMode == DefaultTextureWrapMode::APPLY)
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
    if(mRoundedCorner != RoundedCorner::DISABLED)
    {
      fragmentShaderPrefixList += "#define IS_REQUIRED_ROUNDED_CORNER\n";
      if(mRoundedCorner == RoundedCorner::SQUIRCLE_CORNER)
      {
        fragmentShaderPrefixList += "#define IS_REQUIRED_SQUIRCLE_CORNER\n";
      }
    }
    if(mBorderline == Borderline::ENABLED)
    {
      fragmentShaderPrefixList += "#define IS_REQUIRED_BORDERLINE\n";
    }
    if(mAlphaMaskingOnRendering == AlphaMaskingOnRendering::ENABLED)
    {
      fragmentShaderPrefixList += "#define IS_REQUIRED_ALPHA_MASKING\n";
    }
    else if(mColorConversion == ColorConversion::YUV_TO_RGB)
    {
      fragmentShaderPrefixList += "#define IS_REQUIRED_YUV_TO_RGB\n";
    }
    else if(mColorConversion == ColorConversion::UNIFIED_YUV_AND_RGB)
    {
      fragmentShaderPrefixList += "#define IS_REQUIRED_UNIFIED_YUV_AND_RGB\n";
    }
  }
}

Dali::Texture FeatureBuilder::GetTexture() const
{
  return mTexture;
}

bool FeatureBuilder::IsEnabledAlphaMaskingOnRendering() const
{
  return mAlphaMaskingOnRendering == AlphaMaskingOnRendering::ENABLED;
}

bool FeatureBuilder::IsDefaultTransformUsed() const
{
  return mUseDefaultTransform;
}

} // namespace ImageVisualShaderFeature

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

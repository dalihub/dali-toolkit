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
 *
 */

// CLASS HEADER
#include <dali-scene3d/public-api/loader/material-definition.h>

// EXTERNAL INCLUDES
#include <dali-toolkit/devel-api/builder/base64-encoding.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/pixel-data-integ.h>
#include <dali/integration-api/trace.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/common/image-resource-loader.h>

namespace Dali
{
using namespace Toolkit;

namespace Scene3D
{
namespace Loader
{
const Matrix3 TextureDefinition::DEFAULT_TRANSFORM = Matrix3::IDENTITY;

namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_MODEL_PERFORMANCE_MARKER, false);

constexpr SamplerFlags::Type FILTER_MODES_FROM_DALI[]{
  SamplerFlags::FILTER_LINEAR | SamplerFlags::FILTER_MIPMAP_NEAREST,
  SamplerFlags::FILTER_LINEAR,
  SamplerFlags::FILTER_NEAREST,
  SamplerFlags::FILTER_LINEAR,
  SamplerFlags::FILTER_NEAREST | SamplerFlags::FILTER_MIPMAP_NEAREST,
  SamplerFlags::FILTER_LINEAR | SamplerFlags::FILTER_MIPMAP_NEAREST,
  SamplerFlags::FILTER_NEAREST | SamplerFlags::FILTER_MIPMAP_LINEAR,
  SamplerFlags::FILTER_LINEAR | SamplerFlags::FILTER_MIPMAP_LINEAR,
};

constexpr SamplerFlags::Type WRAP_MODES_FROM_DALI[]{
  SamplerFlags::WRAP_CLAMP,
  SamplerFlags::WRAP_CLAMP,
  SamplerFlags::WRAP_REPEAT,
  SamplerFlags::WRAP_MIRROR,
};

constexpr FilterMode::Type FILTER_MODES_TO_DALI[]{
  FilterMode::NEAREST,
  FilterMode::LINEAR,
  FilterMode::NEAREST_MIPMAP_NEAREST,
  FilterMode::LINEAR_MIPMAP_NEAREST,
  FilterMode::NEAREST_MIPMAP_LINEAR,
  FilterMode::LINEAR_MIPMAP_LINEAR,
};

constexpr WrapMode::Type WRAP_MODES_TO_DALI[]{
  WrapMode::REPEAT,
  WrapMode::CLAMP_TO_EDGE,
  WrapMode::MIRRORED_REPEAT};

const SamplerFlags::Type GetSingleValueSampler()
{
  static const SamplerFlags::Type SINGLE_VALUE_SAMPLER = SamplerFlags::Encode(FilterMode::NEAREST, FilterMode::NEAREST, WrapMode::CLAMP_TO_EDGE, WrapMode::CLAMP_TO_EDGE);
  return SINGLE_VALUE_SAMPLER;
}

static constexpr std::string_view EMBEDDED_DATA_PREFIX               = "data:";
static constexpr std::string_view EMBEDDED_DATA_IMAGE_MEDIA_TYPE     = "image/";
static constexpr std::string_view EMBEDDED_DATA_BASE64_ENCODING_TYPE = "base64,";

Dali::PixelData LoadImageResource(const std::string& resourcePath,
                                  TextureDefinition& textureDefinition,
                                  FittingMode::Type  fittingMode,
                                  bool               orientationCorrection)
{
  Dali::PixelData pixelData;
  if(!textureDefinition.mTextureBuffer.empty())
  {
    DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_MODEL_LOAD_IMAGE_FROM_BUFFER", [&](std::ostringstream& oss)
                                            { oss << "[s:" << textureDefinition.mTextureBuffer.size() << "]"; });
    Dali::Devel::PixelBuffer pixelBuffer = Dali::LoadImageFromBuffer(textureDefinition.mTextureBuffer.data(), textureDefinition.mTextureBuffer.size(), textureDefinition.mMinImageDimensions, fittingMode, textureDefinition.mSamplingMode, orientationCorrection);
    if(pixelBuffer)
    {
      pixelData = Devel::PixelBuffer::Convert(pixelBuffer);
    }
    DALI_TRACE_END_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_MODEL_LOAD_IMAGE_FROM_BUFFER", [&](std::ostringstream& oss)
                                          {
      oss << "[";
      if(pixelData)
      {
        oss << "d:" << pixelData.GetWidth() << "x" << pixelData.GetHeight() << " f:" << pixelData.GetPixelFormat() << " ";
      }
      oss << "s:" << textureDefinition.mTextureBuffer.size() << "]"; });
  }
  else if(textureDefinition.mImageUri.find(EMBEDDED_DATA_PREFIX.data()) == 0 && textureDefinition.mImageUri.find(EMBEDDED_DATA_IMAGE_MEDIA_TYPE.data(), EMBEDDED_DATA_PREFIX.length()) == EMBEDDED_DATA_PREFIX.length())
  {
    auto position = textureDefinition.mImageUri.find(EMBEDDED_DATA_BASE64_ENCODING_TYPE.data(), EMBEDDED_DATA_PREFIX.length() + EMBEDDED_DATA_IMAGE_MEDIA_TYPE.length());
    if(position != std::string::npos)
    {
      position += EMBEDDED_DATA_BASE64_ENCODING_TYPE.length();
      std::string_view     data = std::string_view(textureDefinition.mImageUri).substr(position);
      std::vector<uint8_t> buffer;
      Dali::Toolkit::DecodeBase64FromString(data, buffer);
      uint32_t bufferSize = buffer.size();

      DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_MODEL_LOAD_IMAGE_FROM_BUFFER", [&](std::ostringstream& oss)
                                              { oss << "[embedded s:" << bufferSize << "]"; });
      Dali::Devel::PixelBuffer pixelBuffer = Dali::LoadImageFromBuffer(reinterpret_cast<uint8_t*>(buffer.data()), bufferSize, textureDefinition.mMinImageDimensions, fittingMode, textureDefinition.mSamplingMode, orientationCorrection);
      if(pixelBuffer)
      {
        pixelData = Dali::Devel::PixelBuffer::Convert(pixelBuffer, true);
      }
      DALI_TRACE_END_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_MODEL_LOAD_IMAGE_FROM_BUFFER", [&](std::ostringstream& oss)
                                            {
        oss << "[";
        if(pixelData)
        {
          oss << "d:" << pixelData.GetWidth() << "x" << pixelData.GetHeight() << " f:" << pixelData.GetPixelFormat() << " ";
        }
        oss << "embedded s:" << bufferSize << "]"; });
    }
  }
  else
  {
    textureDefinition.mDirectoryPath = resourcePath;
    pixelData                        = Internal::ImageResourceLoader::GetCachedPixelData(resourcePath + textureDefinition.mImageUri, textureDefinition.mMinImageDimensions, fittingMode, textureDefinition.mSamplingMode, orientationCorrection);
  }
  return pixelData;
}
} // namespace

SamplerFlags::Type SamplerFlags::Encode(FilterMode::Type minFilter, FilterMode::Type magFilter, WrapMode::Type wrapS, WrapMode::Type wrapT)
{
  return FILTER_MODES_FROM_DALI[minFilter] | ((FILTER_MODES_FROM_DALI[magFilter] & FILTER_MAG_BITS) << FILTER_MAG_SHIFT) |
         (WRAP_MODES_FROM_DALI[wrapS] << WRAP_S_SHIFT) | (WRAP_MODES_FROM_DALI[wrapT] << WRAP_T_SHIFT);
}

FilterMode::Type SamplerFlags::GetMinFilter(Type flags)
{
  return FILTER_MODES_TO_DALI[flags & FILTER_MIN_MASK];
}

FilterMode::Type SamplerFlags::GetMagFilter(Type flags)
{
  return FILTER_MODES_TO_DALI[(flags >> FILTER_MAG_SHIFT) & FILTER_MAG_MASK];
}

WrapMode::Type SamplerFlags::GetWrapS(Type flags)
{
  return WRAP_MODES_TO_DALI[(flags >> WRAP_S_SHIFT) & WRAP_S_MASK];
}

WrapMode::Type SamplerFlags::GetWrapT(Type flags)
{
  return WRAP_MODES_TO_DALI[(flags >> WRAP_T_SHIFT) & WRAP_T_MASK];
}

Sampler SamplerFlags::MakeSampler(Type flags)
{
  auto sampler = Sampler::New();
  sampler.SetFilterMode(GetMinFilter(flags), GetMagFilter(flags));
  sampler.SetWrapMode(GetWrapS(flags), GetWrapT(flags));
  return sampler;
}

TextureDefinition::TextureDefinition(const std::string& imageUri, SamplerFlags::Type samplerFlags, ImageDimensions minImageDimensions, SamplingMode::Type samplingMode, Matrix3 transform)
: mImageUri(imageUri),
  mSamplerFlags(samplerFlags),
  mMinImageDimensions(minImageDimensions),
  mSamplingMode(samplingMode),
  mTransform(transform)
{
}

TextureDefinition::TextureDefinition(std::string&& imageUri, SamplerFlags::Type samplerFlags, ImageDimensions minImageDimensions, SamplingMode::Type samplingMode, Matrix3 transform)
: mImageUri(std::move(imageUri)),
  mSamplerFlags(samplerFlags),
  mMinImageDimensions(minImageDimensions),
  mSamplingMode(samplingMode),
  mTransform(transform)
{
}

TextureDefinition::TextureDefinition(std::vector<uint8_t>&& textureBuffer, SamplerFlags::Type samplerFlags, ImageDimensions minImageDimensions, SamplingMode::Type samplingMode, Matrix3 transform)
: mImageUri(),
  mSamplerFlags(samplerFlags),
  mMinImageDimensions(minImageDimensions),
  mSamplingMode(samplingMode),
  mTransform(transform),
  mTextureBuffer(std::move(textureBuffer))
{
}

MaterialDefinition::RawData
MaterialDefinition::LoadRaw(const std::string& imagesPath)
{
  RawData raw;

  const bool hasTransparency = MaskMatch(mFlags, TRANSPARENCY);
  // Why we add additional count here?
  uint32_t numBuffers = static_cast<uint32_t>(mTextureStages.size()) + (hasTransparency ? !CheckTextures(ALBEDO) + !CheckTextures(METALLIC | ROUGHNESS) + !CheckTextures(NORMAL)
                                                                                        : !CheckTextures(ALBEDO | METALLIC) + !CheckTextures(NORMAL | ROUGHNESS));
  if(numBuffers == 0)
  {
    return raw;
  }
  raw.mTextures.reserve(numBuffers);

  // Load textures
  auto iTexture   = mTextureStages.begin();
  auto checkStage = [&](uint32_t flags)
  {
    return iTexture != mTextureStages.end() && MaskMatch(iTexture->mSemantic, flags);
  };

  // Check for compulsory textures: Albedo, Metallic, Roughness, Normal
  if(checkStage(ALBEDO | METALLIC))
  {
    raw.mTextures.push_back({LoadImageResource(imagesPath, iTexture->mTexture, FittingMode::DEFAULT, true), iTexture->mTexture.mSamplerFlags});
    ++iTexture;

    if(checkStage(NORMAL | ROUGHNESS))
    {
      raw.mTextures.push_back({LoadImageResource(imagesPath, iTexture->mTexture, FittingMode::DEFAULT, true), iTexture->mTexture.mSamplerFlags});
      ++iTexture;
    }
    else // single value normal-roughness
    {
      raw.mTextures.push_back({Dali::Scene3D::Internal::ImageResourceLoader::GetEmptyPixelDataZAxisAndAlphaRGBA(), GetSingleValueSampler()});
    }
  }
  else
  {
    if(checkStage(ALBEDO))
    {
      raw.mTextures.push_back({LoadImageResource(imagesPath, iTexture->mTexture, FittingMode::DEFAULT, true), iTexture->mTexture.mSamplerFlags});
      ++iTexture;
    }
    else if(mNeedAlbedoTexture) // single value albedo, albedo-alpha or albedo-metallic
    {
      uint32_t bufferSize = 4;
      uint8_t* buffer     = nullptr;
      auto     format     = Pixel::Format::RGBA8888;
      if(hasTransparency) // albedo-alpha
      {
        buffer    = new uint8_t[bufferSize];
        buffer[3] = static_cast<uint8_t>(mColor.a * 255.f);
      }
      else if(!checkStage(METALLIC | ROUGHNESS)) // albedo-metallic
      {
        buffer    = new uint8_t[bufferSize];
        buffer[3] = 0xff; // metallic of 1.0
      }
      else // albedo
      {
        bufferSize = 3;
        buffer     = new uint8_t[bufferSize];
        format     = Pixel::Format::RGB888;
      }
      buffer[0] = static_cast<uint8_t>(mColor.r * 255.f);
      buffer[1] = static_cast<uint8_t>(mColor.g * 255.f);
      buffer[2] = static_cast<uint8_t>(mColor.b * 255.f);
      raw.mTextures.push_back({Dali::PixelData::New(buffer, bufferSize, 1, 1, format, PixelData::DELETE_ARRAY), GetSingleValueSampler()});
    }

    // If we have transparency, or an image based albedo map, we will have to continue with separate metallicRoughness + normal.
    const bool createMetallicRoughnessAndNormal = hasTransparency || std::distance(mTextureStages.begin(), iTexture) > 0;
    if(checkStage(METALLIC | ROUGHNESS))
    {
      raw.mTextures.push_back({LoadImageResource(imagesPath, iTexture->mTexture, FittingMode::DEFAULT, true), iTexture->mTexture.mSamplerFlags});
      ++iTexture;
    }
    else if(createMetallicRoughnessAndNormal && mNeedMetallicRoughnessTexture)
    {
      // NOTE: we want to set both metallic and roughness to 1.0; dli uses the R & A channels,
      // glTF2 uses B & G, so we might as well just set all components to 1.0.
      raw.mTextures.push_back({Dali::Scene3D::Internal::ImageResourceLoader::GetEmptyPixelDataWhiteRGBA(), GetSingleValueSampler()});
    }

    if(checkStage(NORMAL))
    {
      raw.mTextures.push_back({LoadImageResource(imagesPath, iTexture->mTexture, FittingMode::DEFAULT, true), iTexture->mTexture.mSamplerFlags});
      ++iTexture;
    }
    else if(mNeedNormalTexture)
    {
      if(createMetallicRoughnessAndNormal)
      {
        raw.mTextures.push_back({Dali::Scene3D::Internal::ImageResourceLoader::GetEmptyPixelDataZAxisRGB(), GetSingleValueSampler()});
      }
      else // single-value normal-roughness
      {
        raw.mTextures.push_back({Dali::Scene3D::Internal::ImageResourceLoader::GetEmptyPixelDataZAxisAndAlphaRGBA(), GetSingleValueSampler()});
      }
    }
  }

  // Extra textures.
  if(checkStage(SUBSURFACE))
  {
    raw.mTextures.push_back({LoadImageResource(imagesPath, iTexture->mTexture, FittingMode::DEFAULT, true), iTexture->mTexture.mSamplerFlags});
    ++iTexture;
  }

  if(checkStage(OCCLUSION))
  {
    raw.mTextures.push_back({LoadImageResource(imagesPath, iTexture->mTexture, FittingMode::DEFAULT, true), iTexture->mTexture.mSamplerFlags});
    ++iTexture;
  }

  if(checkStage(EMISSIVE))
  {
    raw.mTextures.push_back({LoadImageResource(imagesPath, iTexture->mTexture, FittingMode::DEFAULT, true), iTexture->mTexture.mSamplerFlags});
    ++iTexture;
  }

  if(checkStage(SPECULAR))
  {
    raw.mTextures.push_back({LoadImageResource(imagesPath, iTexture->mTexture, FittingMode::DEFAULT, true), iTexture->mTexture.mSamplerFlags});
    ++iTexture;
  }

  if(checkStage(SPECULAR_COLOR))
  {
    raw.mTextures.push_back({LoadImageResource(imagesPath, iTexture->mTexture, FittingMode::DEFAULT, true), iTexture->mTexture.mSamplerFlags});
    ++iTexture;
  }

  return raw;
}

TextureSet MaterialDefinition::Load(const EnvironmentDefinition::Vector& environments, RawData&& raw) const
{
  auto textureSet = TextureSet::New();

  uint32_t n = 0;
  for(auto& tData : raw.mTextures)
  {
    auto&   pixels = tData.mPixels;
    Texture texture;
    if(pixels)
    {
      texture = Dali::Scene3D::Internal::ImageResourceLoader::GetCachedTexture(pixels, tData.mSamplerFlags & SamplerFlags::MIPMAP_MASK);
    }

    textureSet.SetTexture(n, texture);
    textureSet.SetSampler(n, SamplerFlags::MakeSampler(tData.mSamplerFlags));

    ++n;
  }

  if(mShadowAvailable)
  {
    textureSet.SetTexture(n++, Dali::Scene3D::Internal::ImageResourceLoader::GetEmptyTextureWhiteRGB());
  }

  // Assign textures to slots -- starting with 2D ones, then cubemaps, if any.
  if(mEnvironmentIdx < static_cast<Index>(environments.size()))
  {
    auto& envTextures = environments[mEnvironmentIdx].second;
    // If pre-computed brdf texture is defined, set the texture.
    if(envTextures.mBrdf)
    {
      textureSet.SetTexture(n, envTextures.mBrdf);
      ++n;
    }

    if(envTextures.mDiffuse)
    {
      textureSet.SetTexture(n, envTextures.mDiffuse);
      ++n;
    }

    if(envTextures.mSpecular)
    {
      auto specularSampler = Sampler::New();
      specularSampler.SetWrapMode(WrapMode::CLAMP_TO_EDGE, WrapMode::CLAMP_TO_EDGE, WrapMode::CLAMP_TO_EDGE);
      specularSampler.SetFilterMode(FilterMode::LINEAR_MIPMAP_LINEAR, FilterMode::LINEAR);

      textureSet.SetTexture(n, envTextures.mSpecular);
      textureSet.SetSampler(n, specularSampler);
      ++n;
    }
  }
  else
  {
    ExceptionFlinger(ASSERT_LOCATION) << "Environment index (" << mEnvironmentIdx << ") out of bounds (" << environments.size() << ").";
  }

  return textureSet;
}

bool MaterialDefinition::CheckTextures(uint32_t flags) const
{
  return std::find_if(mTextureStages.begin(), mTextureStages.end(), [flags](const TextureStage& ts)
                      { return MaskMatch(ts.mSemantic, flags); }) != mTextureStages.end();
}

} // namespace Loader
} // namespace Scene3D
} // namespace Dali

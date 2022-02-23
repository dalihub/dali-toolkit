/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include "texture-cache-manager.h"

// EXTERNAL HEADERS
#include <dali/devel-api/common/hash.h>
#include <dali/integration-api/debug.h>

// INTERNAL HEADERS

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
#ifdef DEBUG_ENABLED
extern Debug::Filter* gTextureManagerLogFilter; ///< Define at texture-manager-impl.cpp

// clang-format off
#define GET_LOAD_STATE_STRING(loadState) \
  loadState == TextureManagerType::LoadState::NOT_STARTED      ? "NOT_STARTED"      : \
  loadState == TextureManagerType::LoadState::LOADING          ? "LOADING"          : \
  loadState == TextureManagerType::LoadState::LOAD_FINISHED    ? "LOAD_FINISHED"    : \
  loadState == TextureManagerType::LoadState::WAITING_FOR_MASK ? "WAITING_FOR_MASK" : \
  loadState == TextureManagerType::LoadState::MASK_APPLYING    ? "MASK_APPLYING"    : \
  loadState == TextureManagerType::LoadState::MASK_APPLIED     ? "MASK_APPLIED"     : \
  loadState == TextureManagerType::LoadState::UPLOADED         ? "UPLOADED"         : \
  loadState == TextureManagerType::LoadState::CANCELLED        ? "CANCELLED"        : \
  loadState == TextureManagerType::LoadState::LOAD_FAILED      ? "LOAD_FAILED"      : \
                                                                 "Unknown"
// clang-format on
#endif
namespace
{
} // Anonymous namespace

TextureCacheManager::TextureCacheManager()
: mCurrentTextureId(0)
{
}

TextureCacheManager::~TextureCacheManager()
{
}

VisualUrl TextureCacheManager::GetVisualUrl(const TextureCacheManager::TextureId& textureId)
{
  VisualUrl         visualUrl("");
  TextureCacheIndex cacheIndex = GetCacheIndexFromId(textureId);

  if(cacheIndex != INVALID_CACHE_INDEX)
  {
    DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureCacheManager::GetVisualUrl. Using cached texture id=%d, textureId=%d\n", cacheIndex, textureId);

    TextureInfo& cachedTextureInfo(mTextureInfoContainer[cacheIndex]);
    visualUrl = cachedTextureInfo.url;
  }
  return visualUrl;
}

TextureCacheManager::LoadState TextureCacheManager::GetTextureState(const TextureCacheManager::TextureId& textureId)
{
  LoadState loadState = TextureCacheManager::LoadState::NOT_STARTED;

  TextureCacheIndex cacheIndex = GetCacheIndexFromId(textureId);
  if(cacheIndex != INVALID_CACHE_INDEX)
  {
    TextureInfo& cachedTextureInfo(mTextureInfoContainer[cacheIndex]);
    loadState = cachedTextureInfo.loadState;
  }
  else
  {
    for(auto&& elem : mExternalTextures)
    {
      if(elem.textureId == textureId)
      {
        loadState = LoadState::UPLOADED;
        break;
      }
    }
  }
  return loadState;
}

TextureCacheManager::LoadState TextureCacheManager::GetTextureStateInternal(const TextureCacheManager::TextureId& textureId)
{
  LoadState loadState = TextureCacheManager::LoadState::NOT_STARTED;

  TextureCacheIndex cacheIndex = GetCacheIndexFromId(textureId);
  if(cacheIndex != INVALID_CACHE_INDEX)
  {
    TextureInfo& cachedTextureInfo(mTextureInfoContainer[cacheIndex]);
    loadState = cachedTextureInfo.loadState;
  }

  return loadState;
}

TextureSet TextureCacheManager::GetTextureSet(const TextureCacheManager::TextureId& textureId)
{
  TextureSet textureSet; // empty handle

  TextureCacheIndex cacheIndex = GetCacheIndexFromId(textureId);
  if(cacheIndex != INVALID_CACHE_INDEX)
  {
    TextureInfo& cachedTextureInfo(mTextureInfoContainer[cacheIndex]);
    textureSet = cachedTextureInfo.textureSet;
  }
  else
  {
    for(auto&& elem : mExternalTextures)
    {
      if(elem.textureId == textureId)
      {
        textureSet = elem.textureSet;
        break;
      }
    }
  }
  return textureSet;
}

TextureSet TextureCacheManager::GetExternalTextureSet(const TextureCacheManager::TextureId& textureId)
{
  TextureSet textureSet; // empty handle
  for(auto&& elem : mExternalTextures)
  {
    if(elem.textureId == textureId)
    {
      textureSet = elem.textureSet;
      break;
    }
  }
  return textureSet;
}

EncodedImageBuffer TextureCacheManager::GetEncodedImageBuffer(const TextureCacheManager::TextureId& textureId)
{
  EncodedImageBuffer encodedImageBuffer; // empty handle
  for(auto&& elem : mEncodedBufferTextures)
  {
    if(elem.textureId == textureId)
    {
      encodedImageBuffer = elem.encodedImageBuffer;
      break;
    }
  }
  return encodedImageBuffer;
}

EncodedImageBuffer TextureCacheManager::GetEncodedImageBuffer(const std::string& url)
{
  EncodedImageBuffer encodedImageBuffer; // empty handle
  if(url.size() > 0 && VisualUrl::BUFFER == VisualUrl::GetProtocolType(url))
  {
    std::string location = VisualUrl::GetLocation(url);
    if(location.size() > 0u)
    {
      TextureId targetId = std::stoi(location);
      return GetEncodedImageBuffer(targetId);
    }
  }
  return encodedImageBuffer;
}

std::string TextureCacheManager::AddExternalTexture(const TextureSet& textureSet)
{
  TextureCacheManager::ExternalTextureInfo info(GenerateUniqueTextureId(), textureSet);
  mExternalTextures.emplace_back(info);
  return VisualUrl::CreateTextureUrl(std::to_string(info.textureId));
}

std::string TextureCacheManager::AddExternalEncodedImageBuffer(const EncodedImageBuffer& encodedImageBuffer)
{
  // Duplication check
  for(auto&& elem : mEncodedBufferTextures)
  {
    if(elem.encodedImageBuffer == encodedImageBuffer)
    {
      // If same buffer added, increase reference count and return.
      elem.referenceCount++;
      return VisualUrl::CreateBufferUrl(std::to_string(elem.textureId));
    }
  }
  TextureCacheManager::EncodedBufferTextureInfo info(GenerateUniqueTextureId(), encodedImageBuffer);
  mEncodedBufferTextures.emplace_back(info);
  return VisualUrl::CreateBufferUrl(std::to_string(info.textureId));
}

TextureSet TextureCacheManager::RemoveExternalTexture(const std::string& url)
{
  if(url.size() > 0u)
  {
    if(VisualUrl::TEXTURE == VisualUrl::GetProtocolType(url))
    {
      // get the location from the Url
      std::string location = VisualUrl::GetLocation(url);
      if(location.size() > 0u)
      {
        TextureId  id  = std::stoi(location);
        const auto end = mExternalTextures.end();
        for(auto iter = mExternalTextures.begin(); iter != end; ++iter)
        {
          if(iter->textureId == id)
          {
            auto textureSet = iter->textureSet;
            if(--(iter->referenceCount) <= 0)
            {
              mExternalTextures.erase(iter);
            }
            return textureSet;
          }
        }
      }
    }
  }
  return TextureSet();
}

EncodedImageBuffer TextureCacheManager::RemoveExternalEncodedImageBuffer(const std::string& url)
{
  if(url.size() > 0u)
  {
    if(VisualUrl::BUFFER == VisualUrl::GetProtocolType(url))
    {
      // get the location from the Url
      std::string location = VisualUrl::GetLocation(url);
      if(location.size() > 0u)
      {
        TextureId  id  = std::stoi(location);
        const auto end = mEncodedBufferTextures.end();
        for(auto iter = mEncodedBufferTextures.begin(); iter != end; ++iter)
        {
          if(iter->textureId == id)
          {
            auto encodedImageBuffer = iter->encodedImageBuffer;
            if(--(iter->referenceCount) <= 0)
            {
              mEncodedBufferTextures.erase(iter);
            }
            return encodedImageBuffer;
          }
        }
      }
    }
  }
  return EncodedImageBuffer();
}

void TextureCacheManager::UseExternalResource(const VisualUrl& url)
{
  if(VisualUrl::TEXTURE == url.GetProtocolType())
  {
    std::string location = url.GetLocation();
    if(location.size() > 0u)
    {
      TextureId id = std::stoi(location);
      for(auto&& elem : mExternalTextures)
      {
        if(elem.textureId == id)
        {
          elem.referenceCount++;
          return;
        }
      }
    }
  }
  else if(VisualUrl::BUFFER == url.GetProtocolType())
  {
    std::string location = url.GetLocation();
    if(location.size() > 0u)
    {
      TextureId id = std::stoi(location);
      for(auto&& elem : mEncodedBufferTextures)
      {
        if(elem.textureId == id)
        {
          elem.referenceCount++;
          return;
        }
      }
    }
  }
}

TextureCacheManager::TextureId TextureCacheManager::GenerateUniqueTextureId()
{
  return mCurrentTextureId++;
}

TextureCacheManager::TextureCacheIndex TextureCacheManager::GetCacheIndexFromId(const TextureCacheManager::TextureId& textureId)
{
  const TextureCacheIndex size = static_cast<TextureCacheIndex>(mTextureInfoContainer.size());

  for(TextureCacheIndex i = 0; i < size; ++i)
  {
    if(mTextureInfoContainer[i].textureId == textureId)
    {
      return i;
    }
  }

  return INVALID_CACHE_INDEX;
}

TextureCacheManager::TextureHash TextureCacheManager::GenerateHash(
  const std::string&                    url,
  const Dali::ImageDimensions&          size,
  const Dali::FittingMode::Type&        fittingMode,
  const Dali::SamplingMode::Type&       samplingMode,
  const TextureCacheManager::UseAtlas&  useAtlas,
  const TextureCacheManager::TextureId& maskTextureId)
{
  std::vector<std::uint8_t> hashTarget(url.begin(), url.end());
  const size_t              urlLength = url.length();
  const uint16_t            width     = size.GetWidth();
  const uint16_t            height    = size.GetWidth();

  // If either the width or height has been specified, include the resizing options in the hash
  if(width != 0 || height != 0)
  {
    // We are appending 5 bytes to the URL to form the hash input.
    hashTarget.resize(urlLength + 5u);
    std::uint8_t* hashTargetPtr = &(hashTarget[urlLength]);

    // Pack the width and height (4 bytes total).
    *hashTargetPtr++ = size.GetWidth() & 0xff;
    *hashTargetPtr++ = (size.GetWidth() >> 8u) & 0xff;
    *hashTargetPtr++ = size.GetHeight() & 0xff;
    *hashTargetPtr++ = (size.GetHeight() >> 8u) & 0xff;

    // Bit-pack the FittingMode, SamplingMode and atlasing.
    // FittingMode=2bits, SamplingMode=3bits, useAtlas=1bit
    *hashTargetPtr = (fittingMode << 4u) | (samplingMode << 1) | (useAtlas == UseAtlas::USE_ATLAS ? 1 : 0);
  }
  else
  {
    // We are not including sizing information, but we still need an extra byte for atlasing.
    hashTarget.resize(urlLength + 1u);

    // Add the atlasing to the hash input.
    switch(useAtlas)
    {
      case UseAtlas::NO_ATLAS:
      {
        hashTarget[urlLength] = 'f';
        break;
      }
      case UseAtlas::USE_ATLAS:
      {
        hashTarget[urlLength] = 't';
        break;
      }
    }
  }

  if(maskTextureId != INVALID_TEXTURE_ID)
  {
    auto textureIdIndex = hashTarget.size();
    hashTarget.resize(hashTarget.size() + sizeof(TextureId));
    std::uint8_t* hashTargetPtr = reinterpret_cast<std::uint8_t*>(&(hashTarget[textureIdIndex]));

    // Append the texture id to the end of the URL byte by byte:
    // (to avoid SIGBUS / alignment issues)
    TextureId saltedMaskTextureId = maskTextureId;
    for(size_t byteIter = 0; byteIter < sizeof(TextureId); ++byteIter)
    {
      *hashTargetPtr++ = saltedMaskTextureId & 0xff;
      saltedMaskTextureId >>= 8u;
    }
  }

  return Dali::CalculateHash(hashTarget);
}

TextureCacheManager::TextureCacheIndex TextureCacheManager::FindCachedTexture(
  const TextureCacheManager::TextureHash&    hash,
  const std::string&                         url,
  const Dali::ImageDimensions&               size,
  const Dali::FittingMode::Type&             fittingMode,
  const Dali::SamplingMode::Type&            samplingMode,
  const TextureCacheManager::UseAtlas&       useAtlas,
  const TextureCacheManager::TextureId&      maskTextureId,
  const TextureCacheManager::MultiplyOnLoad& preMultiplyOnLoad)
{
  // Default to an invalid ID, in case we do not find a match.
  TextureCacheIndex cacheIndex = INVALID_CACHE_INDEX;

  // Iterate through our hashes to find a match.
  const TextureCacheIndex count = static_cast<TextureCacheIndex>(mTextureInfoContainer.size());
  for(TextureCacheIndex i = 0u; i < count; ++i)
  {
    if(mTextureInfoContainer[i].hash == hash)
    {
      // We have a match, now we check all the original parameters in case of a hash collision.
      TextureInfo& textureInfo(mTextureInfoContainer[i]);

      if((url == textureInfo.url.GetUrl()) &&
         (useAtlas == textureInfo.useAtlas) &&
         (maskTextureId == textureInfo.maskTextureId) &&
         (size == textureInfo.desiredSize) &&
         ((size.GetWidth() == 0 && size.GetHeight() == 0) ||
          (fittingMode == textureInfo.fittingMode &&
           samplingMode == textureInfo.samplingMode)))
      {
        // 1. If preMultiplyOnLoad is MULTIPLY_ON_LOAD, then textureInfo.preMultiplyOnLoad should be true. The premultiplication result can be different.
        // 2. If preMultiplyOnLoad is LOAD_WITHOUT_MULTIPLY, then textureInfo.preMultiplied should be false.
        if((preMultiplyOnLoad == MultiplyOnLoad::MULTIPLY_ON_LOAD && textureInfo.preMultiplyOnLoad) || (preMultiplyOnLoad == MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY && !textureInfo.preMultiplied))
        {
          // The found Texture is a match.
          cacheIndex = i;
          break;
        }
      }
    }
  }

  return cacheIndex;
}

TextureCacheManager::TextureCacheIndex TextureCacheManager::AppendCache(const TextureCacheManager::TextureInfo& textureInfo)
{
  TextureCacheIndex cacheIndex = static_cast<TextureCacheIndex>(mTextureInfoContainer.size());
  mTextureInfoContainer.emplace_back(textureInfo);
  return cacheIndex;
}

void TextureCacheManager::RemoveCache(const TextureCacheManager::TextureId& textureId)
{
  TextureCacheIndex textureInfoIndex = GetCacheIndexFromId(textureId);

  if(textureInfoIndex != INVALID_CACHE_INDEX)
  {
    TextureInfo& textureInfo(mTextureInfoContainer[textureInfoIndex]);

    DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureCacheManager::Remove(textureId:%d) url:%s\n  cacheIdx:%d loadState:%s reference count = %d\n", textureId, textureInfo.url.GetUrl().c_str(), textureInfoIndex, GET_LOAD_STATE_STRING(textureInfo.loadState), textureInfo.referenceCount);

    // Decrement the reference count and check if this is the last user of this Texture.
    if(--textureInfo.referenceCount <= 0)
    {
      // This is the last remove for this Texture.
      textureInfo.referenceCount = 0;
      bool removeTextureInfo     = false;

      // If loaded, we can remove the TextureInfo and the Atlas (if atlased).
      if(textureInfo.loadState == LoadState::UPLOADED)
      {
        if(textureInfo.atlas)
        {
          textureInfo.atlas.Remove(textureInfo.atlasRect);
        }
        removeTextureInfo = true;
      }
      else if(textureInfo.loadState == LoadState::LOADING)
      {
        // We mark the textureInfo for removal.
        // Once the load has completed, this method will be called again.
        textureInfo.loadState = LoadState::CANCELLED;
      }
      else
      {
        // In other states, we are not waiting for a load so we are safe to remove the TextureInfo data.
        removeTextureInfo = true;
      }

      // If the state allows us to remove the TextureInfo data, we do so.
      if(removeTextureInfo)
      {
        // If url location is BUFFER, decrease reference count of EncodedImageBuffer.
        if(textureInfo.url.IsBufferResource())
        {
          RemoveExternalEncodedImageBuffer(textureInfo.url.GetUrl());
        }
        // Permanently remove the textureInfo struct.
        mTextureInfoContainer.erase(mTextureInfoContainer.begin() + textureInfoIndex);
      }
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

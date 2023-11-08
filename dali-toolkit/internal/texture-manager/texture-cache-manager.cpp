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
 *
 */

// CLASS HEADER
#include <dali-toolkit/internal/texture-manager/texture-cache-manager.h>

// EXTERNAL HEADERS
#include <dali/devel-api/common/hash.h>
#include <dali/integration-api/debug.h>
#include <string_view>
#include <unordered_map>

// INTERNAL HEADERS

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
const std::string_view& GetEncodedImageBufferExtensions(Dali::EncodedImageBuffer::ImageType imageType)
{
  static const std::unordered_map<Dali::EncodedImageBuffer::ImageType, const std::string_view> gEncodedImageBufferExtensionMap =
    {
      {Dali::EncodedImageBuffer::ImageType::REGULAR_IMAGE, ""},
      {Dali::EncodedImageBuffer::ImageType::VECTOR_IMAGE, ".svg"},
      {Dali::EncodedImageBuffer::ImageType::ANIMATED_VECTOR_IMAGE, ".json"},
    };

  const auto iter = gEncodedImageBufferExtensionMap.find(imageType);

  DALI_ASSERT_DEBUG(iter != gEncodedImageBufferExtensionMap.end());

  return iter->second;
}
} // namespace
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
  loadState == TextureManagerType::LoadState::MASK_CANCELLED   ? "MASK_CANCELLED"   : \
  loadState == TextureManagerType::LoadState::LOAD_FAILED      ? "LOAD_FAILED"      : \
                                                                 "Unknown"
// clang-format on
#endif

// Due to the compile issue, this specialized template code must be defined top of this code.
template<>
void TextureCacheManager::RemoveTextureInfoByIndex<TextureCacheManager::EncodedImageBufferInfoContainerType>(TextureCacheManager::EncodedImageBufferInfoContainerType& cacheContainer, const TextureCacheManager::TextureCacheIndex& removeContainerIndex)
{
  // Swap last data of cacheContainer.
  if(static_cast<std::size_t>(removeContainerIndex.GetIndex() + 1) < cacheContainer.size())
  {
    // First, change the cache index infomations inside of converter
    mTextureIdConverter[cacheContainer.back().bufferId] = static_cast<std::uint32_t>(removeContainerIndex);

    // After change converter, swap the value between current data and last data.
    std::swap(cacheContainer[removeContainerIndex.GetIndex()], cacheContainer.back());
  }

  // Now we can assume that latest data should be removed. pop_back.
  cacheContainer.pop_back();
}

template<class ContainerType>
void TextureCacheManager::RemoveTextureInfoByIndex(ContainerType& cacheContainer, const TextureCacheManager::TextureCacheIndex& removeContainerIndex)
{
  // Swap last data of cacheContainer.
  if(static_cast<std::size_t>(removeContainerIndex.GetIndex() + 1) < cacheContainer.size())
  {
    // First, change the cache index infomations inside of converter
    mTextureIdConverter[cacheContainer.back().textureId] = static_cast<std::uint32_t>(removeContainerIndex);

    // After change converter, swap the value between current data and last data.
    std::swap(cacheContainer[removeContainerIndex.GetIndex()], cacheContainer.back());
  }

  // Now we can assume that latest data should be removed. pop_back.
  cacheContainer.pop_back();
}

TextureCacheManager::TextureCacheManager()
{
}

TextureCacheManager::~TextureCacheManager()
{
}

VisualUrl TextureCacheManager::GetVisualUrl(const TextureCacheManager::TextureId& textureId)
{
  VisualUrl         visualUrl("");
  TextureCacheIndex cacheIndex = static_cast<TextureCacheIndex>(mTextureIdConverter[static_cast<std::uint32_t>(textureId)]);

  switch(static_cast<TextureCacheIndexType>(cacheIndex.detailValue.type))
  {
    case TextureCacheIndexType::TEXTURE_CACHE_INDEX_TYPE_LOCAL:
    {
      DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureCacheManager::GetVisualUrl. Using cached texture index=%d, textureId=%d\n", cacheIndex.GetIndex(), textureId);

      TextureInfo& cachedTextureInfo(mTextureInfoContainer[cacheIndex.GetIndex()]);
      visualUrl = cachedTextureInfo.url;
      break;
    }
    case TextureCacheIndexType::TEXTURE_CACHE_INDEX_TYPE_TEXTURE:
    {
      DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureCacheManager::GetVisualUrl. Using cached external texture index=%d, textureId=%d\n", cacheIndex.GetIndex(), textureId);
      visualUrl = VisualUrl::CreateTextureUrl(std::to_string(textureId));
      break;
    }
    case TextureCacheIndexType::TEXTURE_CACHE_INDEX_TYPE_BUFFER:
    {
      DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureCacheManager::GetVisualUrl. Using cached buffer index=%d, bufferId=%d\n", cacheIndex.GetIndex(), textureId);

      EncodedImageBufferInfo& cachedEncodedImageBufferInfo(mEncodedImageBuffers[cacheIndex.GetIndex()]);
      const auto&             encodedImageBuffer = cachedEncodedImageBufferInfo.encodedImageBuffer;
      visualUrl                                  = VisualUrl::CreateBufferUrl(std::to_string(textureId), GetEncodedImageBufferExtensions(encodedImageBuffer.GetImageType()));
      break;
    }
    default:
    {
      break;
    }
  }

  return visualUrl;
}

TextureCacheManager::LoadState TextureCacheManager::GetTextureState(const TextureCacheManager::TextureId& textureId)
{
  LoadState         loadState  = TextureCacheManager::LoadState::NOT_STARTED;
  TextureCacheIndex cacheIndex = static_cast<TextureCacheIndex>(mTextureIdConverter[static_cast<std::uint32_t>(textureId)]);

  switch(static_cast<TextureCacheIndexType>(cacheIndex.detailValue.type))
  {
    case TextureCacheIndexType::TEXTURE_CACHE_INDEX_TYPE_LOCAL:
    {
      TextureInfo& cachedTextureInfo(mTextureInfoContainer[cacheIndex.GetIndex()]);
      loadState = cachedTextureInfo.loadState;
      break;
    }
    case TextureCacheIndexType::TEXTURE_CACHE_INDEX_TYPE_TEXTURE:
    {
      loadState = LoadState::UPLOADED;
      break;
    }
    default:
    {
      break;
    }
  }

  return loadState;
}

TextureCacheManager::LoadState TextureCacheManager::GetTextureStateInternal(const TextureCacheManager::TextureId& textureId)
{
  LoadState         loadState  = TextureCacheManager::LoadState::NOT_STARTED;
  TextureCacheIndex cacheIndex = GetCacheIndexFromId(textureId);
  if(cacheIndex != INVALID_CACHE_INDEX)
  {
    TextureInfo& cachedTextureInfo(mTextureInfoContainer[cacheIndex.GetIndex()]);
    loadState = cachedTextureInfo.loadState;
  }

  return loadState;
}

Texture TextureCacheManager::GetTexture(const TextureCacheManager::TextureId& textureId, uint32_t textureIndex)
{
  Texture           texture; // empty handle
  TextureCacheIndex cacheIndex = GetCacheIndexFromId(textureId);

  switch(static_cast<TextureCacheIndexType>(cacheIndex.detailValue.type))
  {
    case TextureCacheIndexType::TEXTURE_CACHE_INDEX_TYPE_LOCAL:
    {
      TextureInfo& cachedTextureInfo(mTextureInfoContainer[cacheIndex.GetIndex()]);
      if(textureIndex < static_cast<uint32_t>(cachedTextureInfo.textures.size()))
      {
        texture = cachedTextureInfo.textures[textureIndex];
      }
      break;
    }
    default:
    {
      break;
    }
  }

  return texture;
}

TextureCacheManager::ExternalTextureInfo& TextureCacheManager::GetExternalTextureInfo(const TextureCacheManager::TextureId& textureId)
{
  TextureCacheIndex cacheIndex = GetCacheIndexFromExternalTextureId(textureId);
  DALI_ASSERT_ALWAYS(cacheIndex != INVALID_CACHE_INDEX);

  return mExternalTextures[cacheIndex.GetIndex()];
}

EncodedImageBuffer TextureCacheManager::GetEncodedImageBuffer(const TextureCacheManager::TextureId& bufferId)
{
  EncodedImageBuffer encodedImageBuffer; // empty handle
  TextureCacheIndex  cacheIndex = GetCacheIndexFromEncodedImageBufferId(bufferId);
  if(cacheIndex != INVALID_CACHE_INDEX)
  {
    encodedImageBuffer = mEncodedImageBuffers[cacheIndex.GetIndex()].encodedImageBuffer;
  }
  return encodedImageBuffer;
}

EncodedImageBuffer TextureCacheManager::GetEncodedImageBuffer(const VisualUrl& url)
{
  EncodedImageBuffer encodedImageBuffer; // empty handle
  if(url.IsValid() && VisualUrl::BUFFER == url.GetProtocolType())
  {
    std::string location = url.GetLocationWithoutExtension();
    if(location.size() > 0u)
    {
      TextureId bufferId = std::stoi(location);
      return GetEncodedImageBuffer(bufferId);
    }
  }
  return encodedImageBuffer;
}

std::string TextureCacheManager::AddExternalTexture(const TextureSet& textureSet, bool preMultiplied)
{
  TextureId textureId = GenerateTextureId(TextureCacheIndex(TextureCacheIndexType::TEXTURE_CACHE_INDEX_TYPE_TEXTURE, mExternalTextures.size()));

  TextureCacheManager::ExternalTextureInfo textureInfo(textureId, textureSet, preMultiplied);
  mExternalTextures.emplace_back(textureInfo);

  DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureCacheManager::AddExternalTexture() : New texture registered. textureId:%d, preMultiplied:%d\n", textureInfo.textureId, preMultiplied);

  return VisualUrl::CreateTextureUrl(std::to_string(textureInfo.textureId));
}

std::string TextureCacheManager::AddEncodedImageBuffer(const EncodedImageBuffer& encodedImageBuffer)
{
  // Duplication check
  TextureHash       bufferHash       = static_cast<TextureHash>(encodedImageBuffer.GetHash());
  TextureCacheIndex bufferCacheIndex = FindCachedEncodedImageBuffer(bufferHash, encodedImageBuffer);
  if(bufferCacheIndex != INVALID_CACHE_INDEX)
  {
    EncodedImageBufferInfo& bufferInfo(mEncodedImageBuffers[bufferCacheIndex.GetIndex()]);

    DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureCacheManager::AddExternalEncodedImageBuffer() : Increase reference. bufferId:%d, cache index:%d, reference:%d\n", bufferInfo.bufferId, bufferCacheIndex.GetIndex(), static_cast<int>(bufferInfo.referenceCount));

    // If same buffer added, increase reference count and return.
    bufferInfo.referenceCount++;
    return VisualUrl::CreateBufferUrl(std::to_string(bufferInfo.bufferId), GetEncodedImageBufferExtensions(encodedImageBuffer.GetImageType()));
  }

  TextureId bufferId = GenerateTextureId(TextureCacheIndex(TextureCacheIndexType::TEXTURE_CACHE_INDEX_TYPE_BUFFER, mEncodedImageBuffers.size()));

  TextureCacheManager::EncodedImageBufferInfo info(bufferId, bufferHash, encodedImageBuffer);
  mEncodedImageBuffers.emplace_back(info);

  // Insert TextureHashContainer
  // Find exist list -or- Create new list.
  std::vector<TextureId>& idList = mTextureHashContainer[bufferHash];
  // We already assume that list doesn't contain id. just emplace back
  idList.emplace_back(bufferId);

  DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureCacheManager::AddExternalEncodedImageBuffer() : New buffer regested. bufferId:%d\n", info.bufferId);

  return VisualUrl::CreateBufferUrl(std::to_string(info.bufferId), GetEncodedImageBufferExtensions(encodedImageBuffer.GetImageType()));
}

TextureSet TextureCacheManager::RemoveExternalTexture(const VisualUrl& url)
{
  TextureSet        textureSet;
  bool              removeTextureInfo  = false;
  TextureCacheIndex removeTextureIndex = INVALID_CACHE_INDEX;
  if(url.IsValid())
  {
    if(VisualUrl::TEXTURE == url.GetProtocolType())
    {
      // get the location from the Url
      std::string location = url.GetLocation();
      if(location.size() > 0u)
      {
        TextureId textureId = std::stoi(location);
        removeTextureIndex  = GetCacheIndexFromExternalTextureId(textureId);
        if(removeTextureIndex != INVALID_CACHE_INDEX)
        {
          ExternalTextureInfo& textureInfo(mExternalTextures[removeTextureIndex.GetIndex()]);
          DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureCacheManager::RemoveExternalTexture(url:%s) textureId:%d reference:%d\n", url.GetUrl().c_str(), textureId, static_cast<int>(textureInfo.referenceCount));
          textureSet = textureInfo.textureSet;
          if(--(textureInfo.referenceCount) <= 0)
          {
            removeTextureInfo = true;
            // id life is finished. Remove it at converter
            mTextureIdConverter.Remove(textureId);
          }
        }
      }
    }
  }

  // Post removal process to avoid mExternalTextures reference problems.
  if(removeTextureInfo)
  {
    // Swap last data of mExternalTextures, and pop_back.
    RemoveTextureInfoByIndex(mExternalTextures, removeTextureIndex);
  }
  return textureSet;
}

EncodedImageBuffer TextureCacheManager::RemoveEncodedImageBuffer(const VisualUrl& url)
{
  EncodedImageBuffer encodedImageBuffer;
  bool               removeBufferInfo  = false;
  TextureCacheIndex  removeBufferIndex = INVALID_CACHE_INDEX;
  if(url.IsValid())
  {
    if(VisualUrl::BUFFER == url.GetProtocolType())
    {
      // get the location from the Url
      std::string location = url.GetLocationWithoutExtension();
      if(location.size() > 0u)
      {
        TextureId bufferId = std::stoi(location);
        removeBufferIndex  = GetCacheIndexFromEncodedImageBufferId(bufferId);

        if(removeBufferIndex != INVALID_CACHE_INDEX)
        {
          EncodedImageBufferInfo& bufferInfo(mEncodedImageBuffers[removeBufferIndex.GetIndex()]);
          DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureCacheManager::RemoveEncodedImageBuffer(url:%s) bufferId:%d reference:%d\n", url.GetUrl().c_str(), bufferId, static_cast<int>(bufferInfo.referenceCount));

          encodedImageBuffer = bufferInfo.encodedImageBuffer;
          if(--(bufferInfo.referenceCount) <= 0)
          {
            removeBufferInfo = true;
            // Step 1. remove current textureId information in mTextureHashContainer.
            RemoveHashId(bufferInfo.bufferHash, bufferId);
            // Step 2. id life is finished. Remove it at converter
            mTextureIdConverter.Remove(bufferId);
          }
        }
      }
    }
  }

  // Post removal process to avoid mEncodedImageBuffers reference problems.
  if(removeBufferInfo)
  {
    // Step 3. swap last data of mEncodedImageBuffers, and pop_back.
    RemoveTextureInfoByIndex(mEncodedImageBuffers, removeBufferIndex);
  }
  return encodedImageBuffer;
}

void TextureCacheManager::UseExternalResource(const VisualUrl& url)
{
  if(VisualUrl::TEXTURE == url.GetProtocolType())
  {
    std::string location = url.GetLocation();
    if(location.size() > 0u)
    {
      TextureId         id         = std::stoi(location);
      TextureCacheIndex cacheIndex = GetCacheIndexFromExternalTextureId(id);
      if(cacheIndex != INVALID_CACHE_INDEX)
      {
        ExternalTextureInfo& textureInfo(mExternalTextures[cacheIndex.GetIndex()]);
        DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureCacheManager::UseExternalResource(url:%s) type:TEXTURE, location:%s, reference:%d\n", url.GetUrl().c_str(), url.GetLocation().c_str(), static_cast<int>(textureInfo.referenceCount));

        textureInfo.referenceCount++;
        return;
      }
    }
  }
  else if(VisualUrl::BUFFER == url.GetProtocolType())
  {
    std::string location = url.GetLocationWithoutExtension();
    if(location.size() > 0u)
    {
      TextureId         id         = std::stoi(location);
      TextureCacheIndex cacheIndex = GetCacheIndexFromEncodedImageBufferId(id);
      if(cacheIndex != INVALID_CACHE_INDEX)
      {
        EncodedImageBufferInfo& bufferInfo(mEncodedImageBuffers[cacheIndex.GetIndex()]);
        DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureCacheManager::UseExternalResource(url:%s) type:BUFFER, location:%s, reference:%d\n", url.GetUrl().c_str(), url.GetLocation().c_str(), static_cast<int>(bufferInfo.referenceCount));

        bufferInfo.referenceCount++;
        return;
      }
    }
  }
}

TextureCacheManager::TextureId TextureCacheManager::GenerateTextureId(const TextureCacheIndex& textureCacheIndex)
{
  return mTextureIdConverter.Add(static_cast<std::uint32_t>(textureCacheIndex.indexValue));
}

TextureCacheManager::TextureCacheIndex TextureCacheManager::GetCacheIndexFromId(const TextureCacheManager::TextureId& textureId)
{
  if(textureId == INVALID_TEXTURE_ID) return INVALID_CACHE_INDEX;

  TextureCacheIndex cacheIndex = static_cast<TextureCacheIndex>(mTextureIdConverter[static_cast<std::uint32_t>(textureId)]);
  if(DALI_UNLIKELY(cacheIndex.detailValue.type != TextureCacheIndexType::TEXTURE_CACHE_INDEX_TYPE_LOCAL))
  {
    return INVALID_CACHE_INDEX;
  }

  DALI_ASSERT_DEBUG(static_cast<std::size_t>(cacheIndex.GetIndex()) < mTextureInfoContainer.size());

  return cacheIndex;
}

TextureCacheManager::TextureCacheIndex TextureCacheManager::GetCacheIndexFromExternalTextureId(const TextureCacheManager::TextureId& textureId)
{
  if(textureId == INVALID_TEXTURE_ID) return INVALID_CACHE_INDEX;

  TextureCacheIndex cacheIndex = static_cast<TextureCacheIndex>(mTextureIdConverter[static_cast<std::uint32_t>(textureId)]);
  if(DALI_UNLIKELY(cacheIndex.detailValue.type != TextureCacheIndexType::TEXTURE_CACHE_INDEX_TYPE_TEXTURE))
  {
    return INVALID_CACHE_INDEX;
  }

  DALI_ASSERT_DEBUG(static_cast<std::size_t>(cacheIndex.GetIndex()) < mExternalTextures.size());

  return cacheIndex;
}

TextureCacheManager::TextureCacheIndex TextureCacheManager::GetCacheIndexFromEncodedImageBufferId(const TextureCacheManager::TextureId& bufferId)
{
  if(bufferId == INVALID_TEXTURE_ID) return INVALID_CACHE_INDEX;

  TextureCacheIndex cacheIndex = static_cast<TextureCacheIndex>(mTextureIdConverter[static_cast<std::uint32_t>(bufferId)]);
  if(DALI_UNLIKELY(cacheIndex.detailValue.type != TextureCacheIndexType::TEXTURE_CACHE_INDEX_TYPE_BUFFER))
  {
    return INVALID_CACHE_INDEX;
  }

  DALI_ASSERT_DEBUG(static_cast<std::size_t>(cacheIndex.GetIndex()) < mEncodedImageBuffers.size());

  return cacheIndex;
}

TextureCacheManager::TextureCacheIndex TextureCacheManager::FindCachedEncodedImageBuffer(const TextureCacheManager::TextureHash& hash, const EncodedImageBuffer& encodedImageBuffer)
{
  // Iterate through our hashes to find a match.
  const auto& hashIterator = mTextureHashContainer.find(hash);
  if(hashIterator != mTextureHashContainer.cend())
  {
    for(const auto& id : hashIterator->second)
    {
      // We have a match, now we check all the original parameters in case of a hash collision.
      TextureCacheIndex cacheIndex = GetCacheIndexFromEncodedImageBufferId(id);
      if(cacheIndex != INVALID_CACHE_INDEX)
      {
        EncodedImageBufferInfo& bufferInfo(mEncodedImageBuffers[cacheIndex.GetIndex()]);

        if(bufferInfo.encodedImageBuffer == encodedImageBuffer)
        {
          // The found encoded image buffer.
          return cacheIndex;
        }
      }
    }
  }

  // Default to an invalid ID, in case we do not find a match.
  return INVALID_CACHE_INDEX;
}

TextureCacheManager::TextureHash TextureCacheManager::GenerateHash(
  const VisualUrl&                      url,
  const Dali::ImageDimensions&          size,
  const Dali::FittingMode::Type&        fittingMode,
  const Dali::SamplingMode::Type&       samplingMode,
  const TextureCacheManager::UseAtlas&  useAtlas,
  const TextureCacheManager::TextureId& maskTextureId,
  const bool&                           cropToMask,
  const std::uint32_t&                  frameIndex)
{
  std::vector<std::uint8_t> hashTarget;
  const uint16_t            width  = size.GetWidth();
  const uint16_t            height = size.GetWidth();

  // If either the width or height has been specified, include the resizing options in the hash
  if(width != 0 || height != 0)
  {
    // We are appending 5 bytes to the URL to form the hash input.
    hashTarget.resize(5u);
    std::uint8_t* hashTargetPtr = &(hashTarget[0u]);

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
    hashTarget.resize(1u);

    // Add the atlasing to the hash input.
    switch(useAtlas)
    {
      case UseAtlas::NO_ATLAS:
      {
        hashTarget[0u] = 'f';
        break;
      }
      case UseAtlas::USE_ATLAS:
      {
        hashTarget[0u] = 't';
        break;
      }
    }
  }

  if(maskTextureId != INVALID_TEXTURE_ID)
  {
    auto textureIdIndex = hashTarget.size();
    hashTarget.resize(hashTarget.size() + sizeof(TextureId) + 1u);
    std::uint8_t* hashTargetPtr = reinterpret_cast<std::uint8_t*>(&(hashTarget[textureIdIndex]));

    // Append the texture id to the end of the URL byte by byte:
    // (to avoid SIGBUS / alignment issues)
    TextureId saltedMaskTextureId = maskTextureId;
    for(size_t byteIter = 0; byteIter < sizeof(TextureId); ++byteIter)
    {
      *hashTargetPtr++ = saltedMaskTextureId & 0xff;
      saltedMaskTextureId >>= 8u;
    }
    *hashTargetPtr++ = (cropToMask ? 'C' : 'M');
  }

  // Append the frameIndex. We don't do additional job when frameIndex = 0u due to the non-animated image case.
  if(frameIndex > 0u)
  {
    auto textureIdIndex = hashTarget.size();
    hashTarget.resize(hashTarget.size() + sizeof(std::uint32_t));
    std::uint8_t* hashTargetPtr = reinterpret_cast<std::uint8_t*>(&(hashTarget[textureIdIndex]));

    // Append the frame index to the end of the URL byte by byte:
    std::uint32_t saltedFrameIndex = frameIndex;
    for(size_t byteIter = 0; byteIter < sizeof(std::uint8_t); ++byteIter)
    {
      *hashTargetPtr++ = saltedFrameIndex & 0xff;
      saltedFrameIndex >>= 8u;
    }
  }

  return url.GetUrlHash() ^ Dali::CalculateHash(hashTarget);
}

TextureCacheManager::TextureCacheIndex TextureCacheManager::FindCachedTexture(
  const TextureCacheManager::TextureHash&    hash,
  const VisualUrl&                           url,
  const Dali::ImageDimensions&               size,
  const Dali::FittingMode::Type&             fittingMode,
  const Dali::SamplingMode::Type&            samplingMode,
  const TextureCacheManager::UseAtlas&       useAtlas,
  const StorageType&                         storageType,
  const TextureCacheManager::TextureId&      maskTextureId,
  const bool&                                cropToMask,
  const TextureCacheManager::MultiplyOnLoad& preMultiplyOnLoad,
  const bool&                                isAnimatedImage,
  const std::uint32_t&                       frameIndex)
{
  // Iterate through our hashes to find a match.
  const auto& hashIterator = mTextureHashContainer.find(hash);
  if(hashIterator != mTextureHashContainer.cend())
  {
    for(const auto& textureId : hashIterator->second)
    {
      // We have a match, now we check all the original parameters in case of a hash collision.
      TextureCacheIndex cacheIndex = GetCacheIndexFromId(textureId);
      if(cacheIndex != INVALID_CACHE_INDEX)
      {
        TextureInfo& textureInfo(mTextureInfoContainer[cacheIndex.GetIndex()]);

        if((url.GetUrl() == textureInfo.url.GetUrl()) &&
           (useAtlas == textureInfo.useAtlas) &&
           (maskTextureId == textureInfo.maskTextureId) &&
           (cropToMask == textureInfo.cropToMask) &&
           (size == textureInfo.desiredSize) &&
           (isAnimatedImage == textureInfo.isAnimatedImageFormat) &&
           (storageType == textureInfo.storageType) &&
           (frameIndex == textureInfo.frameIndex) &&
           ((size.GetWidth() == 0 && size.GetHeight() == 0) ||
            (fittingMode == textureInfo.fittingMode &&
             samplingMode == textureInfo.samplingMode)))
        {
          // 1. If preMultiplyOnLoad is MULTIPLY_ON_LOAD, then textureInfo.preMultiplyOnLoad should be true. The premultiplication result can be different.
          // 2. If preMultiplyOnLoad is LOAD_WITHOUT_MULTIPLY, then textureInfo.preMultiplied should be false.
          if((preMultiplyOnLoad == MultiplyOnLoad::MULTIPLY_ON_LOAD && textureInfo.preMultiplyOnLoad) || (preMultiplyOnLoad == MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY && !textureInfo.preMultiplied))
          {
            // The found Texture is a match.
            return cacheIndex;
          }
        }
      }
    }
  }

  // Default to an invalid ID, in case we do not find a match.
  return INVALID_CACHE_INDEX;
}

TextureCacheManager::TextureCacheIndex TextureCacheManager::AppendCache(const TextureCacheManager::TextureInfo& textureInfo)
{
  // If we use EncodedImageBuffer, increase reference during it contains mTextureInfoContainer.
  // This reference will be decreased when we call RemoveCache
  if(textureInfo.url.GetProtocolType() == VisualUrl::BUFFER)
  {
    UseExternalResource(textureInfo.url);
  }

  TextureHash hash = textureInfo.hash;
  TextureId   id   = textureInfo.textureId;

  // Insert TextureHash container first
  // Find exist list -or- Create new list.
  std::vector<TextureId>& idList = mTextureHashContainer[hash];
  // We already assume that list doesn't contain id. just emplace back
  idList.emplace_back(id);

  // Insert TextureInfo back of mTextureInfoContainer.
  TextureCacheIndex cacheIndex = TextureCacheIndex(TextureCacheIndexType::TEXTURE_CACHE_INDEX_TYPE_LOCAL, mTextureInfoContainer.size());
  mTextureInfoContainer.emplace_back(textureInfo);

  // Add converter id --> cacheIndex
  // NOTE : We should assume that id already generated by GenerateTextureId function.
  mTextureIdConverter[id] = cacheIndex;

  return cacheIndex;
}

void TextureCacheManager::RemoveCache(TextureCacheManager::TextureInfo& textureInfo)
{
  TextureCacheIndex textureInfoIndex  = GetCacheIndexFromId(textureInfo.textureId);
  bool              removeTextureInfo = false;

  DALI_LOG_INFO(gTextureManagerLogFilter, Debug::Concise, "TextureCacheManager::Remove(textureId:%d) url:%s\n  cacheIdx:%d loadState:%s reference count = %d\n", textureInfo.textureId, textureInfo.url.GetUrl().c_str(), textureInfoIndex.GetIndex(), GET_LOAD_STATE_STRING(textureInfo.loadState), textureInfo.referenceCount);

  // Decrement the reference count and check if this is the last user of this Texture.
  if(--textureInfo.referenceCount <= 0)
  {
    // This is the last remove for this Texture.
    textureInfo.referenceCount = 0;

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
    else if(textureInfo.loadState == LoadState::MASK_APPLYING)
    {
      // We mark the textureInfo for removal.
      // Once the load has completed, this method will be called again.
      textureInfo.loadState = LoadState::MASK_CANCELLED;
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
        RemoveEncodedImageBuffer(textureInfo.url.GetUrl());
      }

      // Permanently remove the textureInfo struct.

      // Step 1. remove current textureId information in mTextureHashContainer.
      RemoveHashId(textureInfo.hash, textureInfo.textureId);
      // Step 2. make textureId is not using anymore. After this job, we can reuse textureId.
      mTextureIdConverter.Remove(textureInfo.textureId);
    }
  }

  // Post removal process to avoid mTextureInfoContainer reference problems.
  if(removeTextureInfo)
  {
    // Step 3. swap last data of TextureInfoContainer, and pop_back.
    RemoveTextureInfoByIndex(mTextureInfoContainer, textureInfoIndex);
  }
}

void TextureCacheManager::RemoveHashId(const TextureCacheManager::TextureHash& textureHash, const TextureCacheManager::TextureId& textureId)
{
  auto hashIterator = mTextureHashContainer.find(textureHash);
  if(hashIterator != mTextureHashContainer.end())
  {
    auto        hashIdList     = hashIterator->second;
    const auto& hashIdIterator = std::find(hashIdList.cbegin(), hashIdList.cend(), textureId);
    if(hashIdIterator != hashIdList.cend())
    {
      hashIdList.erase(hashIdIterator);
      if(hashIdList.size() == 0)
      {
        // If id list in current hash is empty, remove it self in the container.
        mTextureHashContainer.erase(hashIterator);
      }
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#ifndef DALI_TOOLKIT_TEXTURE_CACHE_MANAGER_H
#define DALI_TOOLKIT_TEXTURE_CACHE_MANAGER_H

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
 */

// EXTERNAL INCLUDES
#include <dali/devel-api/common/free-list.h>
#include <dali/public-api/adaptor-framework/encoded-image-buffer.h>
#include <unordered_map>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/texture-manager/texture-manager-type.h>
#include <dali-toolkit/internal/texture-manager/texture-upload-observer.h>
#include <dali-toolkit/internal/visuals/visual-url.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
/**
 * @brief The contain and managing cached textures.
 * Each Texture hold there TextureId. These TextureId can be used outside of TextureManager.
 * Internally, each cached texture can be accessed by TextureCacheIndex.
 * You can Convert TextureId into TextureCacheIndex by this class.
 *
 * Also, You can store external TextureSet or EncodedImageBuffer here.
 *
 * There are 3 type of CachedContainer in this manager
 *  - mTextureInfoContainer : Cache all kind of textures that need some load/upload jobs.
 *                            All kind of images that visual using (not vector image) will be stored here.
 *                            This container will use TEXTURE_CACHE_INDEX_TYPE_LOCAL
 *
 *  - mExternalTextures : External appended TextureSet cache container.
 *                        External TextureSet can be Something like NativeImageSource, FrameBuffer and PixelData.
 *                        This container will use TEXTURE_CACHE_INDEX_TYPE_TEXTURE
 *                        The textureId will be used for VisualUrl. ex) dali://1
 *
 *  - mEncodedImageBuffers : External appended EncodedImageBuffer cache container.
 *                           This container will use TEXTURE_CACHE_INDEX_TYPE_BUFFER
 *                           The bufferId will be used for VisualUrl. ex) enbuf://1
 *                           Note that this bufferId is not equal with textureId in mTextureInfoContainer.
 */
class TextureCacheManager
{
public:
  // Copy enum and types and const values that TextureCacheManager will use.
  using TextureCacheIndexType = TextureManagerType::TextureCacheIndexType;
  using TextureCacheIndexData = TextureManagerType::TextureCacheIndexData;

  using TextureId         = TextureManagerType::TextureId;
  using TextureCacheIndex = TextureManagerType::TextureCacheIndex;
  using TextureHash       = TextureManagerType::TextureHash;

  static constexpr TextureId         INVALID_TEXTURE_ID  = TextureManagerType::INVALID_TEXTURE_ID;
  static constexpr TextureCacheIndex INVALID_CACHE_INDEX = TextureManagerType::INVALID_CACHE_INDEX;

  using UseAtlas       = TextureManagerType::UseAtlas;
  using StorageType    = TextureManagerType::StorageType;
  using LoadType       = TextureManagerType::LoadType;
  using LoadState      = TextureManagerType::LoadState;
  using ReloadPolicy   = TextureManagerType::ReloadPolicy;
  using MultiplyOnLoad = TextureManagerType::MultiplyOnLoad;
  using TextureInfo    = TextureManagerType::TextureInfo;

public:
  /**
   * Constructor.
   */
  TextureCacheManager();

  /**
   * Destructor.
   */
  ~TextureCacheManager();

public:
  // TextureCacheManager Main API:

  /**
   * @brief Get the visualUrl associated with the texture id.
   * @param[in] textureId The texture Id to get
   * @return The visual Url associated with the texture id.
   */
  VisualUrl GetVisualUrl(const TextureCacheManager::TextureId& textureId);

  /**
   * @brief Get the current state of a texture
   * @note This API doesn't consider encodedimagebuffer.
   * @param[in] textureId The texture id to query
   * @return The loading state if the texture is valid, or NOT_STARTED if the textureId
   * is not valid.
   */
  TextureCacheManager::LoadState GetTextureState(const TextureCacheManager::TextureId& textureId);

  /**
   * @brief Get the current state of a texture
   * @note This API doesn't consider external & encodedimagebuffer.
   * @param[in] textureId The texture id to query
   * @return The loading state if the texture is valid, or NOT_STARTED if the textureId
   * is not valid.
   */
  TextureCacheManager::LoadState GetTextureStateInternal(const TextureCacheManager::TextureId& textureId);

  /**
   * @brief Get the associated texture set if the texture id is valid
   * @param[in] textureId The texture Id to look up
   * @return the associated texture set, or an empty handle if textureId is not valid
   */
  TextureSet GetTextureSet(const TextureCacheManager::TextureId& textureId);

  /**
   * @brief Get the external texture set if the texture id is valid
   * @param[in] textureId The texture Id to look up
   * @return the external texture set, or an empty handle if textureId is not valid
   */
  TextureSet GetExternalTextureSet(const TextureCacheManager::TextureId& textureId);

  /**
   * @brief Get the encoded image buffer
   * @param[in] bufferId The bufferId to look up
   * @return the encoded image buffer, or an empty handle if bufferId is not valid
   */
  EncodedImageBuffer GetEncodedImageBuffer(const TextureCacheManager::TextureId& bufferId);

  /**
   * @brief Get the encoded image buffer by VisualUrl
   * @param[in] url The url to look up
   * @return the encoded image buffer, or an empty handle if url is not buffer resource or buffer is not valid
   */
  EncodedImageBuffer GetEncodedImageBuffer(const VisualUrl& url);

  /**
   * Adds an external texture to the texture manager
   * @param[in] texture The texture to add
   * @return string containing the URL for the texture
   */
  std::string AddExternalTexture(const TextureSet& texture);

  /**
   * Adds an encoded image buffer to the texture manager
   * @param[in] encodedImageBuffer The image buffer to add
   * @return string containing the URL for the texture
   */
  std::string AddEncodedImageBuffer(const EncodedImageBuffer& encodedImageBuffer);

  /**
   * Removes an external texture from texture manager
   * @param[in] url The string containing the texture to remove
   * @return handle to the texture
   */
  TextureSet RemoveExternalTexture(const VisualUrl& url);

  /**
   * Removes an external encoded image buffer from texture manager
   * @param[in] url The string containing the encoded image buffer to remove
   * @return handle to the encoded image buffer
   */
  EncodedImageBuffer RemoveEncodedImageBuffer(const VisualUrl& url);

  /**
   * @brief Notify that external textures or encoded image buffers are used.
   * @param[in] url The URL of the texture to use.
   */
  void UseExternalResource(const VisualUrl& url);

public:
  // To Generate / Get / Remove TextureId.

  /**
   * @brief Generates a new valid TextureId.
   * @param[in] textureCacheIndex the index of the cache container. If we don't setup this value, default is INVALID_CACHE_INDEX
   * @return A TextureId
   */
  TextureCacheManager::TextureId GenerateTextureId(const TextureCacheIndex& textureCacheIndex = INVALID_CACHE_INDEX);

  /**
   * @brief Used to lookup an index into the TextureInfoContainer from a TextureId
   * @param[in] textureId The TextureId to look up
   * @return              The cache index
   */
  TextureCacheManager::TextureCacheIndex GetCacheIndexFromId(const TextureCacheManager::TextureId& textureId);

  /**
   * @brief Generates a hash for caching based on the input parameters.
   * Only applies size, fitting mode andsampling mode if the size is specified.
   * Only applies maskTextureId if it isn't INVALID_TEXTURE_ID
   * Always applies useAtlas.
   * @param[in] url              The URL of the image to load
   * @param[in] size             The image size
   * @param[in] fittingMode      The FittingMode to use
   * @param[in] samplingMode     The SamplingMode to use
   * @param[in] useAtlas         True if atlased
   * @param[in] maskTextureId    The masking texture id (or INVALID_TEXTURE_ID)
   * @param[in] cropToMask       True if crop to mask
   * @param[in] frameIndex       The frame index to use
   * @return                     A hash of the provided data for caching.
   */
  TextureCacheManager::TextureHash GenerateHash(
    const VisualUrl&                      url,
    const Dali::ImageDimensions&          size,
    const Dali::FittingMode::Type&        fittingMode,
    const Dali::SamplingMode::Type&       samplingMode,
    const TextureCacheManager::UseAtlas&  useAtlas,
    const TextureCacheManager::TextureId& maskTextureId,
    const bool&                           cropToMask,
    const std::uint32_t&                  frameIndex);

  /**
   * @brief Looks up a cached texture by its hash.
   * If found, the given parameters are used to check there is no hash-collision.
   * @param[in] hash              The hash to look up
   * @param[in] url               The URL of the image to load
   * @param[in] size              The image size
   * @param[in] fittingMode       The FittingMode to use
   * @param[in] samplingMode      The SamplingMode to use
   * @param[in] useAtlas          True if atlased
   * @param[in] storageType       Whether the pixel data is stored in the cache or uploaded to the GPU
   * @param[in] maskTextureId     Optional texture ID to use to mask this image
   * @param[in] cropToMask        True if crop to mask
   * @param[in] preMultiplyOnLoad if the image's color should be multiplied by it's alpha. Set to OFF if there is no alpha.
   * @param[in] isAnimatedImage   True if the texture is from animated image.
   * @param[in] frameIndex        The frame index to use
   * @return                      A TextureCacheIndex of a cached Texture if found. Or INVALID_CACHE_INDEX if not found.
   */
  TextureCacheManager::TextureCacheIndex FindCachedTexture(
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
    const std::uint32_t&                       frameIndex);

  /**
   * @brief Append a Texture to the TextureCacheManager.
   * @note This API doesn't check duplication of TextureId.
   * @note This API doesn't consider external & encodedimagebuffer.
   *
   * @param[in] textureInfo TextureInfo that want to cache in container.
   * @return Index of newly appended texture info.
   */
  TextureCacheManager::TextureCacheIndex AppendCache(const TextureCacheManager::TextureInfo& textureInfo);

  /**
   * @brief Remove a Texture from the TextureCacheManager.
   * @note This API doesn't consider external & encodedimagebuffer.
   *
   * Textures are cached and therefore only the removal of the last
   * occurrence of a Texture will cause its removal internally.
   *
   * @param[in] textureInfo TextureInfo that want to cache in container.
   */
  void RemoveCache(TextureCacheManager::TextureInfo& textureInfo);

public:
  /**
   * @brief Get TextureInfo as TextureCacheIndex.
   * @note This API doesn't consider external & encodedimagebuffer.
   * @param[in] textureCacheIndex Index of cahced texture.
   * @return TextureInfo as textureCacheIndex
   */
  inline TextureCacheManager::TextureInfo& operator[](const TextureCacheManager::TextureCacheIndex& textureCacheIndex) noexcept
  {
    return mTextureInfoContainer[textureCacheIndex.GetIndex()];
  }

  /**
   * @brief The number of associated cached image
   * @note This API doesn't consider external & encodedimagebuffer.
   * @return The number of associated cached image
   */
  inline std::size_t size() noexcept
  {
    return mTextureInfoContainer.size();
  }

private:
  // Private defined structs.

  /**
   * @brief This struct is used to manage the life-cycle of ExternalTexture url.
   */
  struct ExternalTextureInfo
  {
    ExternalTextureInfo(const TextureCacheManager::TextureId& textureId,
                        const TextureSet&                     textureSet)
    : textureId(textureId),
      textureSet(textureSet),
      referenceCount(1u)
    {
    }
    TextureCacheManager::TextureId textureId;
    TextureSet                     textureSet;
    std::int16_t                   referenceCount;
  };

  /**
   * @brief This struct is used to manage the life-cycle of EncodedImageBuffer url.
   */
  struct EncodedImageBufferInfo
  {
    EncodedImageBufferInfo(const TextureCacheManager::TextureId&   bufferId,
                           const TextureCacheManager::TextureHash& bufferHash,
                           const EncodedImageBuffer&               encodedImageBuffer)
    : bufferId(bufferId),
      bufferHash(bufferHash),
      encodedImageBuffer(encodedImageBuffer),
      referenceCount(1u)
    {
    }
    TextureCacheManager::TextureId   bufferId;
    TextureCacheManager::TextureHash bufferHash;
    EncodedImageBuffer               encodedImageBuffer;
    std::int16_t                     referenceCount;
  };

  typedef Dali::FreeList TextureIdConverterType; ///< The converter type from TextureId to index of TextureInfoContainer.

  typedef std::unordered_map<TextureCacheManager::TextureHash, std::vector<TextureCacheManager::TextureId>> TextureHashContainerType;            ///< The container type used to fast-find the TextureId by TextureHash.
  typedef std::vector<TextureCacheManager::TextureInfo>                                                     TextureInfoContainerType;            ///< The container type used to manage the life-cycle and caching of Textures
  typedef std::vector<TextureCacheManager::ExternalTextureInfo>                                             ExternalTextureInfoContainerType;    ///< The container type used to manage the life-cycle and caching of ExternalTexture url
  typedef std::vector<TextureCacheManager::EncodedImageBufferInfo>                                          EncodedImageBufferInfoContainerType; ///< The container type used to manage the life-cycle and caching of EncodedImageBuffer url

private:
  // Private API: only used internally

  /**
   * @brief Used to lookup an index into the ExternalTextureInfoContainer from a textureId
   * @param[in] textureId The TextureId to look up
   * @return              The cache index
   */
  TextureCacheManager::TextureCacheIndex GetCacheIndexFromExternalTextureId(const TextureCacheManager::TextureId& textureId);

  /**
   * @brief Used to lookup an index into the EncodedImageBufferInfoContainer from a bufferId
   * @param[in] bufferId The bufferId to look up
   * @return             The cache index
   */
  TextureCacheManager::TextureCacheIndex GetCacheIndexFromEncodedImageBufferId(const TextureCacheManager::TextureId& bufferId);

  /**
   * @brief Looks up a cached encoded image buffer cached by its hash.
   * If found, the given parameters are used to check there is no hash-collision.
   * @param[in] hash               The hash to look up
   * @param[in] encodedImageBuffer The image buffer to load
   * @return                       A TextureCacheIndex of a cached Texture if found. Or INVALID_CACHE_INDEX if not found.
   */
  TextureCacheManager::TextureCacheIndex FindCachedEncodedImageBuffer(const TextureCacheManager::TextureHash& hash, const EncodedImageBuffer& encodedImageBuffer);

  /**
   * @brief Remove id in HashContainer.
   * @param[in] hash The hash of the texture/buffer to be delete
   * @param[in] id   The texture/buffer id to be deleted.
   */
  void RemoveHashId(const TextureCacheManager::TextureHash& hash, const TextureCacheManager::TextureId& id);

  /**
   * @brief Remove data from container by the TextureCacheIndex.
   * It also valiate the TextureIdConverter internally.
   * We will assume that only valid TextureCacheIndex will come.
   *
   * @tparam ContainerType The type of container. It will automatically defined
   * @param[in] cacheContainer The container that will remove texture info.
   * @param[in] removeContainerIndex The index of texture info that will remove.
   */
  template<class ContainerType>
  void RemoveTextureInfoByIndex(ContainerType& cacheContainer, const TextureCacheManager::TextureCacheIndex& removeContainerIndex);

private:
  /**
   * Deleted copy constructor.
   */
  TextureCacheManager(const TextureCacheManager&) = delete;

  /**
   * Deleted assignment operator.
   */
  TextureCacheManager& operator=(const TextureCacheManager& rhs) = delete;

private:                                            // Member Variables:
  TextureIdConverterType   mTextureIdConverter{};   ///< Convert TextureId into various container's index.
  TextureHashContainerType mTextureHashContainer{}; ///< Used to manage the life-cycle and caching of Textures + EncodedImageBuffer by TextureHash

  TextureInfoContainerType            mTextureInfoContainer{}; ///< Used to manage the life-cycle and caching of Textures
  ExternalTextureInfoContainerType    mExternalTextures{};     ///< Externally provided textures
  EncodedImageBufferInfoContainerType mEncodedImageBuffers{};  ///< Externally encoded image buffer
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXTURE_CACHE_MANAGER_H

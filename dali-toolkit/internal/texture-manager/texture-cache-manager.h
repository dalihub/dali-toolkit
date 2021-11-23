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
#include <dali/public-api/adaptor-framework/encoded-image-buffer.h>

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
 */
class TextureCacheManager
{
public:
  // Copy enum and types and const values that TextureCacheManager will use.
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
   * @param[in] textureId The texture id to query
   * @return The loading state if the texture is valid, or NOT_STARTED if the textureId
   * is not valid.
   */
  TextureCacheManager::LoadState GetTextureState(const TextureCacheManager::TextureId& textureId);

  /**
   * @brief Get the current state of a texture
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
   * @param[in] textureId The textureId to look up
   * @return the encoded image buffer, or an empty handle if textureId is not valid
   */
  EncodedImageBuffer GetEncodedImageBuffer(const TextureCacheManager::TextureId& textureId);

  /**
   * @brief Get the encoded image buffer by VisualUrl
   * @param[in] url The url to look up
   * @return the encoded image buffer, or an empty handle if url is not buffer resource or buffer is not valid
   */
  EncodedImageBuffer GetEncodedImageBuffer(const std::string& url);

  /**
   * Adds an external texture to the texture manager
   * @param[in] texture The texture to add
   * @return string containing the URL for the texture
   */
  std::string AddExternalTexture(const TextureSet& texture);

  /**
   * Adds an external encoded image buffer to the texture manager
   * @param[in] encodedImageBuffer The image buffer to add
   * @return string containing the URL for the texture
   */
  std::string AddExternalEncodedImageBuffer(const EncodedImageBuffer& encodedImageBuffer);

  /**
   * Removes an external texture from texture manager
   * @param[in] url The string containing the texture to remove
   * @return handle to the texture
   */
  TextureSet RemoveExternalTexture(const std::string& url);

  /**
   * Removes an external encoded image buffer from texture manager
   * @param[in] url The string containing the encoded image buffer to remove
   * @return handle to the encoded image buffer
   */
  EncodedImageBuffer RemoveExternalEncodedImageBuffer(const std::string& url);

  /**
   * @brief Notify that external textures or external encoded image buffers are used.
   * @param[in] url The URL of the texture to use.
   */
  void UseExternalResource(const VisualUrl& url);

public:
  // To Generate / Get / Remove TextureId.

  /**
   * @brief Generates a new, unique TextureId
   * @return A unique TextureId
   */
  TextureCacheManager::TextureId GenerateUniqueTextureId();

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
   * @return                     A hash of the provided data for caching.
   */
  TextureCacheManager::TextureHash GenerateHash(
    const std::string&                    url,
    const Dali::ImageDimensions&          size,
    const Dali::FittingMode::Type&        fittingMode,
    const Dali::SamplingMode::Type&       samplingMode,
    const TextureCacheManager::UseAtlas&  useAtlas,
    const TextureCacheManager::TextureId& maskTextureId);

  /**
   * @brief Looks up a cached texture by its hash.
   * If found, the given parameters are used to check there is no hash-collision.
   * @param[in] hash              The hash to look up
   * @param[in] url               The URL of the image to load
   * @param[in] size              The image size
   * @param[in] fittingMode       The FittingMode to use
   * @param[in] samplingMode      The SamplingMode to use
   * @param[in] useAtlas          True if atlased
   * @param[in] maskTextureId     Optional texture ID to use to mask this image
   * @param[in] preMultiplyOnLoad If the image's color should be multiplied by it's alpha. Set to OFF if there is no alpha.
   * @param[in] isAnimatedImage   True if the texture is from animated image.
   * @return                      A TextureCacheId of a cached Texture if found. Or INVALID_CACHE_INDEX if not found.
   */
  TextureCacheManager::TextureCacheIndex FindCachedTexture(
    const TextureCacheManager::TextureHash&    hash,
    const std::string&                         url,
    const Dali::ImageDimensions&               size,
    const Dali::FittingMode::Type&             fittingMode,
    const Dali::SamplingMode::Type&            samplingMode,
    const TextureCacheManager::UseAtlas&       useAtlas,
    const TextureCacheManager::TextureId&      maskTextureId,
    const TextureCacheManager::MultiplyOnLoad& preMultiplyOnLoad,
    bool                                       isAnimatedImage);

  /**
   * @brief Append a Texture to the TextureCacheManager.
   * @note This API doesn't check duplication of TextureId.
   *
   * @param[in] textureInfo TextureInfo that want to cache in container.
   * @return Index of newly appended texture info.
   */
  TextureCacheManager::TextureCacheIndex AppendCache(const TextureCacheManager::TextureInfo& textureInfo);

  /**
   * @brief Remove a Texture from the TextureCacheManager.
   *
   * Textures are cached and therefore only the removal of the last
   * occurrence of a Texture will cause its removal internally.
   *
   * @param[in] textureId The Id of the Texture to remove at Cache.
   */
  void RemoveCache(const TextureCacheManager::TextureId& textureId);

public:
  /**
   * @brief Get TextureInfo as TextureCacheIndex.
   * @note This API doesn't consider external & encodedimagebuffer.
   * @param[in] textureCacheIndex Index of cahced texture.
   * @return TextureInfo as textureCacheIndex
   */
  inline TextureCacheManager::TextureInfo& operator[](const TextureCacheManager::TextureCacheIndex& textureCacheIndex) noexcept
  {
    return mTextureInfoContainer[textureCacheIndex];
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

  struct EncodedBufferTextureInfo
  {
    EncodedBufferTextureInfo(const TextureCacheManager::TextureId& textureId,
                             const EncodedImageBuffer&             encodedImageBuffer)
    : textureId(textureId),
      encodedImageBuffer(encodedImageBuffer),
      referenceCount(1u)
    {
    }
    TextureCacheManager::TextureId textureId;
    EncodedImageBuffer             encodedImageBuffer;
    std::int16_t                   referenceCount;
  };

  typedef std::vector<TextureCacheManager::TextureInfo>              TextureInfoContainerType;              ///< The container type used to manage the life-cycle and caching of Textures
  typedef std::vector<TextureCacheManager::ExternalTextureInfo>      ExternalTextureInfoContainerType;      ///< The container type used to manage the life-cycle and caching of Textures
  typedef std::vector<TextureCacheManager::EncodedBufferTextureInfo> EncodedBufferTextureInfoContainerType; ///< The container type used to manage the life-cycle and caching of Textures

private:
  /**
   * Deleted copy constructor.
   */
  TextureCacheManager(const TextureCacheManager&) = delete;

  /**
   * Deleted assignment operator.
   */
  TextureCacheManager& operator=(const TextureCacheManager& rhs) = delete;

private:                                                          // Member Variables:
  TextureInfoContainerType              mTextureInfoContainer{};  ///< Used to manage the life-cycle and caching of Textures
  ExternalTextureInfoContainerType      mExternalTextures{};      ///< Externally provided textures
  EncodedBufferTextureInfoContainerType mEncodedBufferTextures{}; ///< Externally encoded buffer textures
  TextureCacheManager::TextureId        mCurrentTextureId;        ///< The current value used for the unique Texture Id generation
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXTURE_CACHE_MANAGER_H

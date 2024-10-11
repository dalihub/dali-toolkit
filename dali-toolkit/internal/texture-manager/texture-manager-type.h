#ifndef DALI_TOOLKIT_TEXTURE_MANAGER_TYPE_H
#define DALI_TOOLKIT_TEXTURE_MANAGER_TYPE_H

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

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/animated-image-loading.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/math/vector4.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/image-loader/image-atlas.h>
#include <dali-toolkit/internal/texture-manager/texture-upload-observer.h>
#include <dali-toolkit/internal/visuals/visual-url.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
/**
 * @brief Define common type, enum, and struct that both TextureManager and TextureCacheManager will use.
 */
namespace TextureManagerType
{
// Enum!

enum TextureCacheIndexType
{
  TEXTURE_CACHE_INDEX_FREE_LIST  = 0, ///< Only for FreeList. We should not use this for TextureCacheIndex.
  TEXTURE_CACHE_INDEX_TYPE_LOCAL = 1,
  TEXTURE_CACHE_INDEX_TYPE_TEXTURE,
  TEXTURE_CACHE_INDEX_TYPE_BUFFER,
  TEXTURE_CACHE_INDEX_TYPE_MASKING, ///< Not implemented yet.
  TEXTURE_CACHE_INDEX_TYPE_MAX = 7, ///< Maximum number of cache type we can use.
};

// Union!

/**
 * @brief standard union type of texture index.
 * Due to the FreeList can only use for uint32_t and we need to seperate
 * each index per container type, we can only hold maximum 2^28 textures
 * at the same time.
 *      0 ~   2^28 - 1 : index of FreeList. TextureCacheManager will not use it.
 *   2^28 ~ 2*2^28 - 1 : index of mTextureInfoContainer, the main texture container.
 * 2*2^28 ~ 3*2^28 - 1 : index of mExternalTextures.
 * 3*2^28 ~ 4*2^28 - 1 : index of mEncodedBufferTextures.
 */
union TextureCacheIndexData
{
  TextureCacheIndexData() = default;
  constexpr TextureCacheIndexData(const uint32_t index)
  : indexValue(index)
  {
  }
  constexpr explicit TextureCacheIndexData(const int32_t index)
  : indexValue(static_cast<uint32_t>(index))
  {
  }
  constexpr TextureCacheIndexData(const TextureCacheIndexType& type, const uint32_t index)
  : detailValue{index, type}
  {
  }
  constexpr TextureCacheIndexData(const TextureCacheIndexData& indexData)
  : indexValue(indexData.indexValue)
  {
  }
  constexpr TextureCacheIndexData(TextureCacheIndexData&& indexData)
  : indexValue(indexData.indexValue)
  {
  }

  TextureCacheIndexData& operator=(const uint32_t index)
  {
    indexValue = index;
    return *this;
  }
  TextureCacheIndexData& operator=(const TextureCacheIndexData& rhs)
  {
    indexValue = rhs.indexValue;
    return *this;
  }
  TextureCacheIndexData& operator=(TextureCacheIndexData&& rhs)
  {
    indexValue = rhs.indexValue;
    return *this;
  }

  constexpr operator uint32_t() const
  {
    return indexValue;
  }
  constexpr operator uint32_t()
  {
    return indexValue;
  }
  constexpr explicit operator int32_t() const
  {
    return static_cast<int32_t>(indexValue);
  }
  constexpr explicit operator int32_t()
  {
    return static_cast<int32_t>(indexValue);
  }

  // Return detailValue.index. - the real index of datailValue.type container
  constexpr inline uint32_t GetIndex() const
  {
    return detailValue.index;
  }

  inline constexpr bool operator==(const TextureCacheIndexData& rhs)
  {
    return indexValue == rhs.indexValue;
  }
  inline constexpr bool operator<(const TextureCacheIndexData& rhs)
  {
    return indexValue < rhs.indexValue;
  }

  // Data area
  uint32_t indexValue;
  struct
  {
    unsigned int          index : 28;
    TextureCacheIndexType type : 4;
  } detailValue;
};

// Typedef:

typedef int32_t               TextureId;         ///< The TextureId type. This is used as a handle to refer to a particular Texture.
typedef TextureCacheIndexData TextureCacheIndex; ///< The TextureCacheIndex type. This is used as a handles to refer to a particular Texture in TextureCacheManager.
                                                 ///  Note : For the same Texture, TextureId will not be changed. But TextureCacheIndex can be chaged when TextureCacheManager
                                                 ///  Internal container informations changed by Append or Remove.
typedef size_t TextureHash;                      ///< The type used to store the hash used for Texture caching.

// Constant values:

static constexpr TextureId         INVALID_TEXTURE_ID  = -1; ///< Used to represent a null TextureId or error
static constexpr TextureCacheIndex INVALID_CACHE_INDEX = 0;  ///< Used to represent a null TextureCacheIndex or error

// Enum classes:

/**
 * Whether the pixel data should be kept in TextureManager, returned with pixelBuffer or uploaded for rendering
 */
enum class StorageType
{
  KEEP_PIXEL_BUFFER,   ///< Keep loaded pixel buffer inside of texture manager without making texture. This could be used for inside pixel process like mask image.
  RETURN_PIXEL_BUFFER, ///< Return loaded pixel buffer without making texture.
                       ///  Because a pixel buffer cannot be used multiple texture, this pixel buffer only cached during loading, and is removed after loading is finished.
  KEEP_TEXTURE,        ///< Keep loaded texture inside of texture manager. This could be used for pixel processing like GPU masking.
  UPLOAD_TO_TEXTURE    ///< Loaded image will be uploaded to texture and the texture will be returned.
};

/**
 * @brief The LoadState Enumeration represents the current state of a particular Texture's life-cycle.
 */
enum class LoadState
{
  NOT_STARTED,      ///< Default
  LOADING,          ///< Loading has been started, but not finished.
  LOAD_FINISHED,    ///< Loading has finished. (for CPU storage only)
  WAITING_FOR_MASK, ///< Loading has finished, but waiting for mask image
  MASK_APPLYING,    ///< Loading has finished, Mask is applying
  MASK_APPLIED,     ///< Loading has finished, Mask is applyied by GPU
  UPLOADED,         ///< Uploaded and ready. (For GPU upload only)
  CANCELLED,        ///< Removed before loading completed
  MASK_CANCELLED,   ///< Removed before mask applying completed
  LOAD_FAILED       ///< Async loading failed, e.g. connection problem
};

/**
 * @brief Types of reloading policies
 */
enum class ReloadPolicy
{
  CACHED = 0, ///< Loads cached texture if it exists.
  FORCED      ///< Forces reloading of texture.
};

/**
 * @brief Whether to multiply alpha into color channels on load
 */
enum class MultiplyOnLoad
{
  LOAD_WITHOUT_MULTIPLY = 0, ///< Don't modify the image
  MULTIPLY_ON_LOAD           ///< Multiply alpha into color channels on load
};

// Structs:

/**
 * @brief This struct is used to manage the life-cycle of Texture loading and caching.
 */
struct TextureInfo
{
  TextureInfo(const TextureId                   textureId,
              const TextureId                   maskTextureId,
              const VisualUrl&                  url,
              const Dali::ImageDimensions&      desiredSize,
              const float                       scaleFactor,
              const Dali::FittingMode::Type     fittingMode,
              const Dali::SamplingMode::Type    samplingMode,
              const bool                        loadSynchronously,
              const bool                        cropToMask,
              const TextureHash                 hash,
              const bool                        orientationCorrection,
              const bool                        preMultiplyOnLoad,
              const Dali::AnimatedImageLoading& animatedImageLoading,
              const uint32_t                    frameIndex,
              const bool                        loadYuvPlanes)
  : url(url),
    desiredSize(desiredSize),
    useSize(desiredSize),
    textureId(textureId),
    maskTextureId(maskTextureId),
    hash(hash),
    scaleFactor(scaleFactor),
    referenceCount(1),
    loadState(LoadState::NOT_STARTED),
    fittingMode(fittingMode),
    samplingMode(samplingMode),
    storageType(StorageType::UPLOAD_TO_TEXTURE),
    animatedImageLoading(animatedImageLoading),
    frameIndex(frameIndex),
    frameCount(0u),
    frameInterval(0u),
    loadSynchronously(loadSynchronously),
    cropToMask(cropToMask),
    orientationCorrection(true),
    preMultiplyOnLoad(preMultiplyOnLoad),
    preMultiplied(preMultiplyOnLoad),
    loadYuvPlanes(loadYuvPlanes)
  {
    isAnimatedImageFormat = (animatedImageLoading) ? true : false;
  }

  /**
   * Container type used to store all observer clients of this Texture
   */
  typedef Dali::Vector<TextureUploadObserver*> ObserverListType;

  ObserverListType           observerList;         ///< Container used to store all observer clients of this Texture
  Dali::Devel::PixelBuffer   pixelBuffer;          ///< The PixelBuffer holding the image data (May be empty after upload)
  std::vector<Dali::Texture> textures;             ///< The Textures
  VisualUrl                  url;                  ///< The URL of the image
  Dali::ImageDimensions      desiredSize;          ///< The size requested
  Dali::ImageDimensions      useSize;              ///< The size used
  TextureId                  textureId;            ///< The TextureId associated with this Texture
  TextureId                  maskTextureId;        ///< The mask TextureId to be applied on load
  TextureHash                hash;                 ///< The hash used to cache this Texture
  float                      scaleFactor;          ///< The scale factor to apply to the Texture when masking
  int32_t                    referenceCount;       ///< The reference count of clients using this Texture
  LoadState                  loadState;            ///< The load state showing the load progress of the Texture
  Dali::FittingMode::Type    fittingMode : 3;      ///< The requested FittingMode
  Dali::SamplingMode::Type   samplingMode : 3;     ///< The requested SamplingMode
  StorageType                storageType;          ///< CPU storage / GPU upload;
  Dali::AnimatedImageLoading animatedImageLoading; ///< AnimatedImageLoading that contains animated image information.
  uint32_t                   frameIndex;           ///< Frame index that be loaded, in case of animated image
  uint32_t                   frameCount;           ///< Total frame count of input animated image. If this variable is not 0, this textureInfo is for animated image file format.
  uint32_t                   frameInterval;        ///< Time interval between this frame and next frame of animated image.

  bool loadSynchronously : 1;     ///< True if synchronous loading was requested
  bool cropToMask : 1;            ///< True if the image should be cropped to the mask size.
  bool orientationCorrection : 1; ///< True if the image should be rotated to match exif orientation data
  bool preMultiplyOnLoad : 1;     ///< True if the image's color should be multiplied by it's alpha
  bool preMultiplied : 1;         ///< True if the image's color was multiplied by it's alpha
  bool isAnimatedImageFormat : 1; ///< true if the image is requested from animated image visual.
  bool loadYuvPlanes : 1;         ///< true if the image should be loaded as yuv planes
};

/**
 * @brief This struct is used to manage the life-cycle of ExternalTexture url.
 */
struct ExternalTextureInfo
{
  ExternalTextureInfo(const TextureId   textureId,
                      const TextureSet& textureSet,
                      const bool        preMultiplied)
  : textureId(textureId),
    textureSet(textureSet),
    referenceCount(1),
    preMultiplied(preMultiplied)
  {
  }

  TextureId  textureId;         ///< The TextureId associated with this ExternalTexture
  TextureSet textureSet;        ///< The external texture
  int32_t    referenceCount;    ///< The reference count of clients using this ExternalTexture
  bool       preMultiplied : 1; ///< True if the image's color was multiplied by it's alpha
};

} // namespace TextureManagerType

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXTURE_MANAGER_TYPE_H

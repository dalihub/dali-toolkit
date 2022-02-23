#ifndef DALI_TOOLKIT_TEXTURE_MANAGER_TYPE_H
#define DALI_TOOLKIT_TEXTURE_MANAGER_TYPE_H

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
// Typedef:

typedef std::int32_t TextureId;         ///< The TextureId type. This is used as a handle to refer to a particular Texture.
typedef std::int32_t TextureCacheIndex; ///< The TextureCacheIndex type. This is used as a handles to refer to a particular Texture in TextureCacheManager.
                                        ///  Note : For the same Texture, TextureId will not be changed. But TextureCacheIndex can be chaged when TextureCacheManager
                                        ///  Internal container informations changed by Append or Remove.
typedef std::size_t TextureHash;        ///< The type used to store the hash used for Texture caching.

// Constant values:

static constexpr TextureId         INVALID_TEXTURE_ID  = -1; ///< Used to represent a null TextureId or error
static constexpr TextureCacheIndex INVALID_CACHE_INDEX = -1; ///< Used to represent a null TextureCacheIndex or error

// Enum classes:

/**
 * Whether the texture should be atlased or uploaded into it's own GPU texture
 */
enum class UseAtlas
{
  NO_ATLAS,
  USE_ATLAS
};

/**
 * Whether the pixel data should be kept in TextureManager, returned with pixelBuffer or uploaded for rendering
 */
enum class StorageType
{
  KEEP_PIXEL_BUFFER,   ///< Keep loaded pixel buffer inside of texture manager without making texture. This could be used for inside pixel process like mask image.
  RETURN_PIXEL_BUFFER, ///< Return loaded pixel buffer without making texture.
                       ///  Because a pixel buffer cannot be used multiple texture, this pixel buffer only cached during loading, and is removed after loading is finished.
  UPLOAD_TO_TEXTURE    ///< Loaded image will be uploaded to texture and the texture will be returned.
};

/**
 * Whether the texture should be loaded synchronously or asynchronously.
 */
enum class LoadType
{
  LOAD_ASYNCHRONOUSLY,
  LOAD_SYNCHRONOUSLY
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
  TextureInfo(const TextureId&                  textureId,
              const TextureId&                  maskTextureId,
              const VisualUrl&                  url,
              const Dali::ImageDimensions&      desiredSize,
              const float&                      scaleFactor,
              const Dali::FittingMode::Type&    fittingMode,
              const Dali::SamplingMode::Type&   samplingMode,
              const bool&                       loadSynchronously,
              const bool&                       cropToMask,
              const UseAtlas&                   useAtlas,
              const TextureHash&                hash,
              const bool&                       orientationCorrection,
              const bool&                       preMultiplyOnLoad,
              const Dali::AnimatedImageLoading& animatedImageLoading,
              const std::uint32_t&              frameIndex)
  : url(url),
    desiredSize(desiredSize),
    useSize(desiredSize),
    atlasRect(0.0f, 0.0f, 1.0f, 1.0f), // Full atlas rectangle
    textureId(textureId),
    maskTextureId(maskTextureId),
    hash(hash),
    scaleFactor(scaleFactor),
    referenceCount(1u),
    loadState(LoadState::NOT_STARTED),
    fittingMode(fittingMode),
    samplingMode(samplingMode),
    storageType(StorageType::UPLOAD_TO_TEXTURE),
    animatedImageLoading(animatedImageLoading),
    frameIndex(frameIndex),
    useAtlas(useAtlas),
    loadSynchronously(loadSynchronously),
    cropToMask(cropToMask),
    orientationCorrection(true),
    preMultiplyOnLoad(preMultiplyOnLoad),
    preMultiplied(false)
  {
  }

  /**
   * Container type used to store all observer clients of this Texture
   */
  typedef Dali::Vector<TextureUploadObserver*> ObserverListType;

  ObserverListType           observerList;         ///< Container used to store all observer clients of this Texture
  Dali::Toolkit::ImageAtlas  atlas;                ///< The atlas this Texture lays within (if any)
  Dali::Devel::PixelBuffer   pixelBuffer;          ///< The PixelBuffer holding the image data (May be empty after upload)
  Dali::TextureSet           textureSet;           ///< The TextureSet holding the Texture
  VisualUrl                  url;                  ///< The URL of the image
  Dali::ImageDimensions      desiredSize;          ///< The size requested
  Dali::ImageDimensions      useSize;              ///< The size used
  Dali::Vector4              atlasRect;            ///< The atlas rect used if atlased
  TextureId                  textureId;            ///< The TextureId associated with this Texture
  TextureId                  maskTextureId;        ///< The mask TextureId to be applied on load
  TextureHash                hash;                 ///< The hash used to cache this Texture
  float                      scaleFactor;          ///< The scale factor to apply to the Texture when masking
  std::int16_t               referenceCount;       ///< The reference count of clients using this Texture
  LoadState                  loadState;            ///< The load state showing the load progress of the Texture
  Dali::FittingMode::Type    fittingMode : 3;      ///< The requested FittingMode
  Dali::SamplingMode::Type   samplingMode : 3;     ///< The requested SamplingMode
  StorageType                storageType;          ///< CPU storage / GPU upload;
  Dali::AnimatedImageLoading animatedImageLoading; ///< AnimatedImageLoading that contains animated image information.
  std::uint32_t              frameIndex;           ///< frame index that be loaded, in case of animated image
  UseAtlas                   useAtlas;             ///< USE_ATLAS if an atlas was requested.

  bool loadSynchronously : 1;     ///< True if synchronous loading was requested
  bool cropToMask : 1;            ///< True if the image should be cropped to the mask size.
  bool orientationCorrection : 1; ///< True if the image should be rotated to match exif orientation data
  bool preMultiplyOnLoad : 1;     ///< True if the image's color should be multiplied by it's alpha
  bool preMultiplied : 1;         ///< True if the image's color was multiplied by it's alpha
};

} // namespace TextureManagerType

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXTURE_MANAGER_TYPE_H

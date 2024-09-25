#ifndef DALI_SCENE3D_IMAGE_RESOURCE_LOADER_H
#define DALI_SCENE3D_IMAGE_RESOURCE_LOADER_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/rendering/texture.h>

// INTERNAL INCLUDES

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
/**
 * The namespace to load the images and cache raw PixelData.
 * The cached resources will be reused when the same input parameter is loaded multiple times.
 * @note This class can be called from worker threads.
 */
namespace ImageResourceLoader
{
// Called by main thread.
/**
 * @brief Get cached texture handle filled as white with RGB888 format.
 * @return A Texture object containing the white RGB888 color.
 */
Dali::Texture GetEmptyTextureWhiteRGB();

/**
 * @brief Get cached cube texture handle filled as white with RGB888 format at 6 faces.
 * @return A Texture object containing the white RGB888 color at 6 faces.
 */
Dali::Texture GetEmptyCubeTextureWhiteRGB();

/**
 * @brief Get default BRDF texture.
 * @return A Texture object containing the default BRDF texture.
 */
Dali::Texture GetDefaultBrdfTexture();

/**
 * @brief Get cached texture handle, or create new texture and upload.
 * @param[in] pixelData The PixelData of image to upload
 * @param[in] mipmapRequired True if this texture need to generate mipmap
 * @return A Texture object containing the pixelData, or an invalid object on failure
 */
Dali::Texture GetCachedTexture(Dali::PixelData pixelData, bool mipmapRequired);

/**
 * @brief Request to remove unused Texture and PixelData. We can choose the collect garbages incrementally or fully.
 *
 * @param[in] fullCollect True if we want to collect whole garbages. Default is false.
 */
void RequestGarbageCollect(bool fullCollect = false);

/**
 * @brief Let we ensure to create a ResourceLoader cache handler.
 */
void EnsureResourceLoaderCreated();

// Can be called by worker thread.
/**
 * @brief Get cached pixelData handle filled as white with RGB888 format.
 * @return A PixelData object containing the white RGB888 color.
 */
Dali::PixelData GetEmptyPixelDataWhiteRGB();

/**
 * @brief Get cached pixelData handle filled as white with RGBA8888 format.
 * @return A PixelData object containing the white RGBA8888 color.
 */
Dali::PixelData GetEmptyPixelDataWhiteRGBA();

/**
 * @brief Get cached pixelData handle that will be used for Z-Axis with RGB8888 format.
 * @return A PixelData object containing the Z-Axis RGB8888 color.
 */
Dali::PixelData GetEmptyPixelDataZAxisRGB();

/**
 * @brief Get cached pixelData handle that will be used for Z-Axis and 1.0 alpha with RGBA8888 format.
 * @return A PixelData object containing the Z-Axis and 1.0 alpha RGBA8888 color.
 */
Dali::PixelData GetEmptyPixelDataZAxisAndAlphaRGBA();

/**
 * @brief Get cached pixelData handle with brdf pixel data has.
 * @return A PixelData object containing brdf data.
 */
Dali::PixelData GetDefaultBrdfPixelData();

/**
 * @brief Get cached image, or loads an image synchronously.
 * @note If cache handler is not created yet, or destroyed due to app terminated, it will load image synchronously without cache.
 * @param[in] url The URL of the image file to load
 * @return A PixelData object containing the image, or an invalid object on failure
 */
Dali::PixelData GetCachedPixelData(const std::string& url);

/**
 * @brief Get cached image, or loads an image synchronously by specifying the target dimensions and options.
 * @note If cache handler is not created yet, or destroyed due to app terminated, it will load image synchronously without cache.
 * @param[in] url The URL of the image file to load
 * @param[in] dimensions The width and height to fit the loaded image to
 * @param[in] samplingMode The filtering method used when sampling pixels from the input image while fitting it to desired size
 * @return A PixelData object containing the image, or an invalid object on failure
 */
Dali::PixelData GetCachedPixelData(const std::string& url,
                                   ImageDimensions    dimensions,
                                   SamplingMode::Type samplingMode);
} // namespace ImageResourceLoader

} // namespace Internal

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_CACHE_MANAGER_H

#ifndef DALI_TOOLKIT_IMAGE_ATLAS_H
#define DALI_TOOLKIT_IMAGE_ATLAS_H
/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <string>
#include <stdint.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/images/pixel.h>
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/rendering/texture.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/image-loader/atlas-upload-observer.h>

namespace Dali
{

namespace Toolkit
{
namespace Internal DALI_INTERNAL
{
class ImageAtlas;
}

/**
 * @brief An ImageAtlas is a large texture containing multiple smaller images.
 *
 * Only images with url provided or pixel data are supported for uploading.
 * The images are loaded by a worker thread to avoid blocking the main event thread.
 */
class DALI_IMPORT_API ImageAtlas : public BaseHandle
{
public:

  typedef uint32_t SizeType;

public:

  /**
   * @brief Create a new ImageAtlas.
   *
   * @param [in] width          The atlas width in pixels.
   * @param [in] height         The atlas height in pixels.
   * @param [in] pixelFormat    The pixel format (rgba 32 bit by default).
   * @return A handle to a new ImageAtlas.
   */
  static ImageAtlas New( SizeType width, SizeType height,
                         Pixel::Format pixelFormat = Pixel::RGBA8888 );

  /**
   * @brief Create an empty handle.
   *
   * Calling member functions of an empty handle is not allowed.
   */
  ImageAtlas();

  /**
   * @brief Destructor.
   */
  ~ImageAtlas();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  ImageAtlas( const ImageAtlas& handle );

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] handle  A reference to the copied handle
   * @return A reference to this
   */
  ImageAtlas& operator=( const ImageAtlas& handle );

  /**
   * @brief Get the atlas image.
   *
   * This atlas texture is still valid after destroying the ImageAtlas object.
   *
   * @return The atlas texture
   */
  Texture GetAtlas();

  /*
   * @brief Query what percentage of space is been occupied in the atlas.
   *
   * @return The occupancy rate of the atlas.
   */
  float GetOccupancyRate() const;

  /**
   * @brief Set the broken image which is used to replace the image if loading fails.
   *
   * @param[in] brokenImageUrl The url of the broken image.
   */
  void SetBrokenImage( const std::string& brokenImageUrl );

  /**
   * @brief Upload a resource image to the atlas.
   *
   * @note To make the atlasing efficient, a valid size should be provided.
   *       If size is not provided, then the image file will be opened to read the actual size for loading.
   *       Do not set a size that is bigger than the actual image size, as the up-scaling is not available,
   *       the content of the area not covered by actual image is undefined, it will not be cleared.
   *
   * SamplingMode::BOX_THEN_LINEAR is used to sampling pixels from the input image while fitting it to desired size.
   *
   * @param [out] textureRect The texture area of the resource image in the atlas.
   * @param [in] url The URL of the resource image file to use.
   * @param [in] size The width and height to fit the loaded image to.
   * @param [in] fittingMode The method used to fit the shape of the image before loading to the shape defined by the size parameter.
   * @param [in] orientationCorrection Reorient the image to respect any orientation metadata in its header.
   * @return True if there is enough space to fit this image in,false otherwise.
   */
  bool Upload( Vector4& textureRect,
               const std::string& url,
               ImageDimensions size = ImageDimensions(),
               FittingMode::Type fittingMode = FittingMode::DEFAULT,
               bool orientationCorrection = true );

  /**
   * @brief Upload a resource image to the atlas.
   *
   * @note To make the atlasing efficient, a valid size should be provided.
   *       If size is not provided, then the image file will be opened to read the actual size for loading.
   *       Do not set a size that is bigger than the actual image size, as the up-scaling is not available,
   *       the content of the area not covered by actual image is undefined, it will not be cleared.
   *
   * SamplingMode::BOX_THEN_LINEAR is used to sampling pixels from the input image while fitting it to desired size.
   *
   * @param [out] textureRect The texture area of the resource image in the atlas.
   * @param [in] url The URL of the resource image file to use.
   * @param [in] size The width and height to fit the loaded image to.
   * @param [in] fittingMode The method used to fit the shape of the image before loading to the shape defined by the size parameter.
   * @param [in] orientationCorrection Reorient the image to respect any orientation metadata in its header.
   * @param[in] atlasUploadObserver The observer to observe the upload state inside the ImageAtlas.
   * @return True if there is enough space to fit this image in,false otherwise.
   * @note The valid callback function here is required to have the signature of void( void ).
   */
  bool Upload( Vector4& textureRect,
               const std::string& url,
               ImageDimensions size,
               FittingMode::Type fittingMode,
               bool orientationCorrection,
               AtlasUploadObserver* atlasUploadObserver );

  /**
   * @brief Upload a pixel buffer to atlas
   *
   * @param [out] textureRect The texture area of the resource image in the atlas.
   * @param [in] pixelData The pixel data.
   */
  bool Upload( Vector4& textureRect, PixelData pixelData );

  /**
   * @brief Remove the image at the given rectangle.
   *
   * The rectangular area is marked unoccupied, so new image can be added to this area.
   *
   * @param [in] textureRect The texture area to be removed.
   */
  void Remove( const Vector4& textureRect );

public: // Not intended for developer use

  explicit DALI_INTERNAL ImageAtlas( Internal::ImageAtlas* impl );
};

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_IMAGE_ATLAS_H

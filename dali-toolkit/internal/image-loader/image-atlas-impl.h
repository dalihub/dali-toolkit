#ifndef DALI_TOOLKIT_IMAGE_ATLAS_IMPL_H
#define DALI_TOOLKIT_IMAGE_ATLAS_IMPL_H

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
#include <dali/devel-api/common/owner-container.h>
#include <dali/public-api/adaptor-framework/encoded-image-buffer.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/connection-tracker.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/image-loader/image-atlas.h>
#include <dali-toolkit/internal/image-loader/atlas-packer.h>
#include <dali-toolkit/internal/visuals/visual-url.h>
#include <dali-toolkit/public-api/image-loader/async-image-loader.h>

namespace Dali
{
class EventThreadCallback;

namespace Toolkit
{
namespace Internal
{
class ImageAtlas : public BaseObject, public ConnectionTracker
{
public:
  typedef Toolkit::ImageAtlas::SizeType SizeType;

  /**
   * @copydoc ImageAtlas::PackToAtlas( const std::vector<PixelData>&, Dali::Vector<Vector4>& )
   */
  static Texture PackToAtlas(const std::vector<PixelData>& pixelData, Dali::Vector<Vector4>& textureRects);

  /**
   * Constructor
   * @param [in] width          The atlas width in pixels.
   * @param [in] height         The atlas height in pixels.
   * @param [in] pixelFormat    The pixel format.
   */
  ImageAtlas(SizeType width, SizeType height, Pixel::Format pixelFormat);

  /**
   * @copydoc Toolkit::ImageAtlas::New
   */
  static IntrusivePtr<ImageAtlas> New(SizeType width, SizeType height, Pixel::Format pixelFormat);

  /**
   * @copydoc Toolkit::ImageAtlas::GetAtlas
   */
  Texture GetAtlas();

  /**
   * @copydoc Toolkit::ImageAtlas::GetOccupancyRate
   */
  float GetOccupancyRate() const;

  /**
   * @copydoc Toolkit::ImageAtlas::SetBrokenImage
   */
  void SetBrokenImage(const std::string& brokenImageUrl);

  /**
   * @copydoc Toolkit::ImageAtlas::Upload( Vector4&, const std::string&, ImageDimensions,FittingMode::Type, bool )
   */
  bool Upload(Vector4&             textureRect,
              const VisualUrl&     url,
              ImageDimensions      size,
              FittingMode::Type    fittingMode,
              bool                 orientationCorrection,
              AtlasUploadObserver* atlasUploadObserver);

  /**
   * @brief Upload a resource image to the atlas by encoded buffer.
   *
   * @note To make the atlasing efficient, a valid size should be provided.
   *       If size is not provided, then SegFault occured.
   *       Do not set a size that is bigger than the actual image size, as the up-scaling is not available,
   *       the content of the area not covered by actual image is undefined, it will not be cleared.
   *
   * SamplingMode::BOX_THEN_LINEAR is used to sampling pixels from the input image while fitting it to desired size.
   *
   * @param [out] textureRect The texture area of the resource image in the atlas.
   * @param [in] encodedImageBuffer The encoded raw buffer of the resource image file to use.
   * @param [in] size The width and height to fit the loaded image to.
   * @param [in] fittingMode The method used to fit the shape of the image before loading to the shape defined by the size parameter.
   * @param [in] orientationCorrection Reorient the image to respect any orientation metadata in its header.
   * @param [in] atlasUploadObserver The observer to observe the upload state inside the ImageAtlas.
   * @return True if there is enough space to fit this image in,false otherwise.
   * @note The valid callback function here is required to have the signature of void( void ).
   */
  bool Upload(Vector4&                  textureRect,
              const EncodedImageBuffer& encodedImageBuffer,
              ImageDimensions           size,
              FittingMode::Type         fittingMode,
              bool                      orientationCorrection,
              AtlasUploadObserver*      atlasUploadObserver);

  /**
   * @copydoc Toolkit::ImageAtlas::Upload( Vector4&, PixelData )
   */
  bool Upload(Vector4& textureRect, PixelData pixelData);

  /**
   * @copydoc Toolkit::ImageAtlas::Remove
   */
  void Remove(const Vector4& textureRect);

  /**
   * Resets the destroying observer pointer so that we know not to call methods of this object any more.
   */
  void ObserverDestroyed(AtlasUploadObserver* observer);

protected:
  /**
   * Destructor
   */
  ~ImageAtlas();

private:
  /**
   * @copydoc PixelDataRequester::ProcessPixels
   */
  void UploadToAtlas(uint32_t id, PixelData pixelData);

  /**
   * Upload broken image
   *
   * @param[in] area The pixel area for uploading.
   */
  void UploadBrokenImage(const Rect<uint32_t>& area);

  // Undefined
  ImageAtlas(const ImageAtlas& imageAtlas);

  // Undefined
  ImageAtlas& operator=(const ImageAtlas& imageAtlas);

private:
  /**
   * Each loading task( identified with an ID ) is associated with a rect region for packing the loaded pixel data into the atlas,
   * and an AtlasUploadObserver whose UploadCompleted method should get executed once the sub texture is ready.
   */
  struct LoadingTaskInfo
  {
    LoadingTaskInfo(uint32_t             loadTaskId,
                    uint32_t             packPositionX,
                    uint32_t             packPositionY,
                    uint32_t             width,
                    uint32_t             height,
                    AtlasUploadObserver* observer)
    : loadTaskId(loadTaskId),
      packRect(packPositionX, packPositionY, width, height),
      observer(observer)
    {
    }

    uint32_t             loadTaskId;
    Rect<uint32_t>       packRect;
    AtlasUploadObserver* observer;
  };

  OwnerContainer<LoadingTaskInfo*> mLoadingTaskInfoContainer;

  Texture                   mAtlas;
  AtlasPacker               mPacker;
  Toolkit::AsyncImageLoader mAsyncLoader;
  std::string               mBrokenImageUrl;
  ImageDimensions           mBrokenImageSize;
  float                     mWidth;
  float                     mHeight;
  Pixel::Format             mPixelFormat;
};

} // namespace Internal

inline const Internal::ImageAtlas& GetImplementation(const Toolkit::ImageAtlas& imageAtlas)
{
  DALI_ASSERT_ALWAYS(imageAtlas && "ImageAtlas handle is empty");

  const BaseObject& handle = imageAtlas.GetBaseObject();

  return static_cast<const Internal::ImageAtlas&>(handle);
}

inline Internal::ImageAtlas& GetImplementation(Toolkit::ImageAtlas& imageAtlas)
{
  DALI_ASSERT_ALWAYS(imageAtlas && "ImageAtlas handle is empty");

  BaseObject& handle = imageAtlas.GetBaseObject();

  return static_cast<Internal::ImageAtlas&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_IMAGE_ATLAS_IMPL_H

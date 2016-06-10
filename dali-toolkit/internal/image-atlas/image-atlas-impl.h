#ifndef __DALI_TOOLKIT_IMAGE_ATLAS_IMPL_H__
#define __DALI_TOOLKIT_IMAGE_ATLAS_IMPL_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>
#include <dali/devel-api/images/atlas.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/image-atlas/image-atlas.h>
#include <dali-toolkit/internal/image-atlas/atlas-packer.h>
#include <dali-toolkit/internal/image-atlas/image-load-thread.h>

namespace Dali
{
class EventThreadCallback;

namespace Toolkit
{

namespace Internal
{

class ImageAtlas : public BaseObject
{
public:

  typedef Toolkit::ImageAtlas::SizeType SizeType;

  /**
   * Constructor
   * @param [in] width          The atlas width in pixels.
   * @param [in] height         The atlas height in pixels.
   * @param [in] pixelFormat    The pixel format.
   */
  ImageAtlas( SizeType width, SizeType height, Pixel::Format pixelFormat );

  /**
   * @copydoc Toolkit::ImageAtlas::New
   */
  static IntrusivePtr<ImageAtlas> New( SizeType width, SizeType height, Pixel::Format pixelFormat );

  /**
   * @copydoc Toolkit::ImageAtlas::GetAtlas
   */
  Image GetAtlas();

  /**
   * @copydoc Toolkit::ImageAtlas::SetBrokenImage
   */
  void SetBrokenImage( const std::string& brokenImageUrl );

  /**
   * @copydoc Toolkit::ImageAtlas::Upload( Vector4&, const std::string&, ImageDimensions,FittingMode::Type, bool )
   */
  bool Upload( Vector4& textureRect,
               const std::string& url,
               ImageDimensions size,
               FittingMode::Type fittingMode,
               bool orientationCorrection);

  /**
   * @copydoc Toolkit::ImageAtlas::Upload( Vector4&, PixelData )
   */
  bool Upload( Vector4& textureRect, PixelData pixelData );

  /**
   * @copydoc Toolkit::ImageAtlas::Remove
   */
  void Remove( const Vector4& textureRect );

protected:

  /**
   * Destructor
   */
  ~ImageAtlas();

private:

  /**
   * Upload the bitmap to atlas when the image is loaded in the worker thread.
   */
  void UploadToAtlas();

  /**
   * Upload broken image
   *
   * @param[in] area The pixel area for uploading.
   */
  void UploadBrokenImage( const Rect<SizeType>& area );

  // Undefined
  ImageAtlas( const ImageAtlas& imageAtlas);

  // Undefined
  ImageAtlas& operator=( const ImageAtlas& imageAtlas );

private:

  Atlas                mAtlas;
  AtlasPacker          mPacker;

  LoadQueue            mLoadQueue;
  CompleteQueue        mCompleteQueue;
  ImageLoadThread      mLoadingThread;

  std::string          mBrokenImageUrl;
  ImageDimensions      mBrokenImageSize;
  float                mWidth;
  float                mHeight;
  Pixel::Format        mPixelFormat;
  bool                 mLoadingThreadStarted;

};

} // namespace Internal

inline const Internal::ImageAtlas& GetImplementation( const Toolkit::ImageAtlas& imageAtlas )
{
  DALI_ASSERT_ALWAYS( imageAtlas && "ImageAtlas handle is empty" );

  const BaseObject& handle = imageAtlas.GetBaseObject();

  return static_cast<const Internal::ImageAtlas&>(handle);
}

inline Internal::ImageAtlas& GetImplementation( Toolkit::ImageAtlas& imageAtlas )
{
  DALI_ASSERT_ALWAYS( imageAtlas && "ImageAtlas handle is empty" );

  BaseObject& handle = imageAtlas.GetBaseObject();

  return static_cast<Internal::ImageAtlas&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_IMAGE_ATLAS_IMPL_H__ */

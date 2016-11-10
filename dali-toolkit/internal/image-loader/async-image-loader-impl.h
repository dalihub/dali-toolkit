#ifndef DALI_TOOLKIT_ASYNC_IMAGE_LOADER_IMPL_H
#define DALI_TOOLKIT_ASYNC_IMAGE_LOADER_IMPL_H

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
 */

// EXTERNAL INCLUDES
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/image-loader/async-image-loader.h>
#include <dali-toolkit/internal/image-loader/image-load-thread.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class AsyncImageLoader : public BaseObject
{
public:

  /**
   * Constructor
   */
  AsyncImageLoader();

  /**
   * @copydoc Toolkit::AsyncImageLoader::New()
   */
  static IntrusivePtr<AsyncImageLoader> New();

  /**
   * @copydoc Toolkit::AsyncImageLoader::Load( const std::string&, ImageDimensions, FittingMode::Type, SamplingMode::Type, bool )
   */
  uint32_t Load( const std::string& url,
                 ImageDimensions dimensions,
                 FittingMode::Type fittingMode,
                 SamplingMode::Type samplingMode,
                 bool orientationCorrection );

  /**
   * @copydoc Toolkit::AsyncImageLoader::ImageLoadedSignal
   */
  Toolkit::AsyncImageLoader::ImageLoadedSignalType& ImageLoadedSignal();

  /**
   * @copydoc Toolkit::AsyncImageLoader::Cancel
   */
  bool Cancel( uint32_t loadingTaskId );

  /**
   * @copydoc Toolkit::AsyncImageLoader::CancelAll
   */
  void CancelAll();

  /**
   * Process the completed loading task from the worker thread.
   */
  void ProcessLoadedImage();

protected:

  /**
   * Destructor
   */
  ~AsyncImageLoader();

private:

  Toolkit::AsyncImageLoader::ImageLoadedSignalType mLoadedSignal;

  ImageLoadThread mLoadThread;
  uint32_t        mLoadTaskId;
  bool            mIsLoadThreadStarted;


};

} // namespace Internal

inline const Internal::AsyncImageLoader& GetImplementation( const Toolkit::AsyncImageLoader& handle )
{
  DALI_ASSERT_ALWAYS( handle && "AsyncImageLoader handle is empty" );

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::AsyncImageLoader&>( object );
}

inline Internal::AsyncImageLoader& GetImplementation( Toolkit::AsyncImageLoader& handle )
{
  DALI_ASSERT_ALWAYS( handle && "AsyncImageLoader handle is empty" );

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::AsyncImageLoader&>( object );
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_ASYNC_IMAGE_LOADER_IMPL_H

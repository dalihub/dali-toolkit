#ifndef DALI_TOOLKIT_ASYNC_IMAGE_LOADER_H
#define DALI_TOOLKIT_ASYNC_IMAGE_LOADER_H

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
#include <string>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
class PixelData;

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class AsyncImageLoader;
}

/**
 * @brief The AsyncImageLoader is used to load pixel data from a URL asynchronously.
 *
 * The images are loaded in a worker thread to avoid blocking the main event thread.
 *
 * To keep track of the loading images, each load call is assigned an ID (which is returned by the Load() call).
 * To know when the Load has completed, connect to the ImageLoadedSignal.
 * This signal should be connected before Load is called (in case the signal is emitted immediately).
 *
 * Load errors can be detected by checking the PixelData object is valid from within the signal handler.

 * Note: The PixelData object will automatically be destroyed when it leaves its scope.
 *
 * Example:
 *
 * @code
 * class MyClass : public ConnectionTracker
 * {
 *   public:
 *
 *   MyCallback( uint32_t loadedTaskId, PixelData pixelData )
 *   {
 *     // First check if the image loaded correctly.
 *     if( pixelData )
 *     {
 *       if( loadedTaskId == mId1 )
 *       {
 *         // use the loaded pixel data from the first image
 *       }
 *       else if( loadedTaskId == mId2 )
 *       {
 *         // use the loaded pixel data from the second image
 *       }
 *     }
 *   }
 *
 *   uint32_t mId1;
 *   uint32_t mId2;
 * };
 *
 * MyClass myObject;
 * AsyncImageLoader imageLoader = AsyncImageLoader::New();
 *
 * // Connect the signal here.
 * imageLoader.ImageLoadedSignal().Connect( &myObject, &MyClass::MyCallback );
 *
 * // Invoke the load calls (must do this after connecting the signal to guarantee callbacks occur).
 * myObject.mId1 = imageLoader.Load( "first_image_url.jpg" );
 * myObject.mId2 = imageLoader.Load( "second_image_url.jpg" );
 *
 * @endcode
 */
class DALI_IMPORT_API AsyncImageLoader : public BaseHandle
{
public:

  typedef Signal< void( uint32_t, PixelData ) > ImageLoadedSignalType; ///< Image loaded signal type @SINCE_1_2_14

public:

  /**
   * @brief Constructor which creates an empty AsyncImageLoader handle.
   * @SINCE_1_2_14
   *
   * Use AsyncImageLoader::New() to create an initialised object.
   */
  AsyncImageLoader();

  /**
   * @brief Destructor
   * @SINCE_1_2_14
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~AsyncImageLoader();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   * @SINCE_1_2_14
   *
   * @param[in] handle A reference to the copied handle
   */
  AsyncImageLoader( const AsyncImageLoader& handle );

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   * @SINCE_1_2_14
   *
   * @param[in] handle  A reference to the copied handle
   * @return A reference to this
   */
  AsyncImageLoader& operator=( const AsyncImageLoader& handle );

  /**
   * @brief Create a new loader to load the image asynchronously in a worker thread.
   * @SINCE_1_2_14
   *
   * @return The image loader.
   */
  static AsyncImageLoader New();

  /**
   * @brief Downcast a handle to AsyncImageLoader handle.
   *
   * If the handle points to an AsyncImageLoader object the downcast produces
   * a valid handle. If not, the returned handle is left uninitialized.
   *
   * @SINCE_1_2_14
   * @param[in] handle A handle to an object
   * @return A handle to a AsyncImageLoader object or an uninitialized handle
   */
  static AsyncImageLoader DownCast( BaseHandle handle );

  /**
   * @brief Start an image loading task.
   * Note: When using this method, the following defaults will be used:
   * fittingMode = FittingMode::DEFAULT
   * samplingMode = SamplingMode::BOX_THEN_LINEAR
   * orientationCorrection = true
   *
   * @SINCE_1_2_14
   *
   * @param[in] url The URL of the image file to load.
   * @return The loading task id.
   */
  uint32_t Load( const std::string& url );

  /**
   * @brief Start an image loading task.
   * Note: When using this method, the following defaults will be used:
   * fittingMode = FittingMode::DEFAULT
   * samplingMode = SamplingMode::BOX_THEN_LINEAR
   * orientationCorrection = true
   *
   * @SINCE_1_2_14
   *
   * @param[in] url The URL of the image file to load.
   * @param[in] dimensions The width and height to fit the loaded image to.
   * @return The loading task id.
   */
  uint32_t Load( const std::string& url, ImageDimensions dimensions );

  /**
   * @brief Start an image loading task.
   * @SINCE_1_2_14
   *
   * @param[in] url The URL of the image file to load.
   * @param[in] dimensions The width and height to fit the loaded image to.
   * @param[in] fittingMode The method used to fit the shape of the image before loading to the shape defined by the size parameter.
   * @param[in] samplingMode The filtering method used when sampling pixels from the input image while fitting it to desired size.
   * @param[in] orientationCorrection Reorient the image to respect any orientation metadata in its header.
   * @return The loading task id.
   */
  uint32_t Load( const std::string& url,
                 ImageDimensions dimensions,
                 FittingMode::Type fittingMode,
                 SamplingMode::Type samplingMode,
                 bool orientationCorrection );

  /**
   * @brief Cancel a image loading task if it is still queueing in the work thread.
   * @SINCE_1_2_14
   *
   * @param[in] loadingTaskId The task id returned when invoking the load call.
   * @return If true, the loading task is removed from the queue, otherwise the loading is already implemented and unable to cancel anymore
   */
  bool Cancel( uint32_t loadingTaskId );

  /**
   * @brief Cancel all the loading tasks in the queue
   * @SINCE_1_2_14
   */
  void CancelAll();

  /**
   * @brief Signal emit for connected callback functions to get access to the loaded pixel data.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( uint32_t id, PixelData pixelData );
   * @endcode
   * @SINCE_1_2_14
   *
   * @return A reference to a signal object to Connect() with.
   */
  ImageLoadedSignalType& ImageLoadedSignal();

public: // Not intended for developer use

  /// @cond internal
  /**
   * @brief Allows the creation of a AsyncImageLoader handle from an internal pointer.
   *
   * @note Not intended for application developers
   * @SINCE_1_2_14
   *
   * @param[in] impl A pointer to the object.
   */
  explicit DALI_INTERNAL AsyncImageLoader( Internal::AsyncImageLoader* impl );
  /// @endcond

};

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_ASYNC_IMAGE_LOADER_H

#ifndef DALI_TOOLKIT_IMAGE_LOADING_TASK_H
#define DALI_TOOLKIT_IMAGE_LOADING_TASK_H

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
#include <dali-toolkit/devel-api/image-loader/async-image-loader-devel.h>
#include <dali-toolkit/internal/visuals/visual-url.h>
#include <dali/public-api/adaptor-framework/async-task-manager.h>
#include <dali/devel-api/adaptor-framework/event-thread-callback.h>
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali/devel-api/threading/conditional-wait.h>
#include <dali/devel-api/threading/mutex.h>
#include <dali/devel-api/threading/thread.h>
#include <dali/integration-api/adaptor-framework/log-factory-interface.h>
#include <dali/public-api/adaptor-framework/encoded-image-buffer.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/object/ref-object.h>
#include <dali-toolkit/internal/texture-manager/texture-manager-type.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

class LoadingTask;
using LoadingTaskPtr = IntrusivePtr<LoadingTask>;

/**
 * The task of loading and packing an image into the atlas
 */
class LoadingTask : public AsyncTask
{
public:
  /**
   * Constructor.
   * @param [in] id of the task
   * @param [in] animatedImageLoading The AnimatedImageLoading to load animated image
   * @param [in] frameIndex The frame index of a frame to be loaded frame
   * @param [in] preMultiplyOnLoad ON if the image's color should be multiplied by it's alpha. Set to OFF if there is no alpha or if the image need to be applied alpha mask.
   * @param [in] callback The callback that is called when the operation is completed.
   */
  LoadingTask(uint32_t                                 id,
              Dali::AnimatedImageLoading               animatedImageLoading,
              uint32_t                                 frameIndex,
              DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad,
              CallbackBase*                            callback);

  /**
   * Constructor.
   * @param [in] id of the task
   * @param [in] url The URL of the image file to load.
   * @param [in] size The width and height to fit the loaded image to, 0.0 means whole image
   * @param [in] fittingMode The method used to fit the shape of the image before loading to the shape defined by the size parameter.
   * @param [in] samplingMode The filtering method used when sampling pixels from the input image while fitting it to desired size.
   * @param [in] orientationCorrection Reorient the image to respect any orientation metadata in its header.
   * @param [in] preMultiplyOnLoad ON if the image's color should be multiplied by it's alpha. Set to OFF if there is no alpha or if the image need to be applied alpha mask.
   * @param [in] loadPlanes true to load image planes or false to load bitmap image.
   * @param [in] callback The callback that is called when the operation is completed.
   */
  LoadingTask(uint32_t                                 id,
              const VisualUrl&                         url,
              ImageDimensions                          dimensions,
              FittingMode::Type                        fittingMode,
              SamplingMode::Type                       samplingMode,
              bool                                     orientationCorrection,
              DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad,
              bool                                     loadPlanes,
              CallbackBase*                            callback);

  /**
   * Constructor.
   * @param [in] id of the task
   * @param [in] encodedImageBuffer The encoded buffer of the image to load.
   * @param [in] size The width and height to fit the loaded image to, 0.0 means whole image
   * @param [in] fittingMode The method used to fit the shape of the image before loading to the shape defined by the size parameter.
   * @param [in] samplingMode The filtering method used when sampling pixels from the input image while fitting it to desired size.
   * @param [in] orientationCorrection Reorient the image to respect any orientation metadata in its header.
   * @param [in] preMultiplyOnLoad ON if the image's color should be multiplied by it's alpha. Set to OFF if there is no alpha or if the image need to be applied alpha mask.
   * @param [in] callback The callback that is called when the operation is completed.
   */
  LoadingTask(uint32_t                                 id,
              const EncodedImageBuffer&                encodedImageBuffer,
              ImageDimensions                          dimensions,
              FittingMode::Type                        fittingMode,
              SamplingMode::Type                       samplingMode,
              bool                                     orientationCorrection,
              DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad,
              CallbackBase*                            callback);

  /**
   * Constructor.
   * @param [in] id of the task
   * @param [in] pixelBuffer of the to be masked image
   * @param [in] maskPixelBuffer of the mask image
   * @param [in] contentScale The factor to scale the content
   * @param [in] cropToMask Whether to crop the content to the mask size
   * @param [in] preMultiplyOnLoad ON if the image's color should be multiplied by it's alpha. Set to OFF if there is no alpha.
   * @param [in] callback The callback that is called when the operation is completed.
   */
  LoadingTask(uint32_t                                 id,
              Devel::PixelBuffer                       pixelBuffer,
              Devel::PixelBuffer                       maskPixelBuffer,
              float                                    contentScale,
              bool                                     cropToMask,
              DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad,
              CallbackBase*                            callback);

  /**
   * Destructor.
   */
  ~LoadingTask() override;

  /**
   * Process the task accodring to the type
   */
  void Process() override;

  /**
   * Whether the task is ready to process.
   * @return True if the task is ready to process.
   */
  bool IsReady() override;

  /**
   * @brief Set the Texture Id
   *
   */
  void SetTextureId(TextureManagerType::TextureId id);

private:
  // Undefined
  LoadingTask(const LoadingTask& queue);

  // Undefined
  LoadingTask& operator=(const LoadingTask& queue);

  /**
   * Load the image
   */
  void Load();

  /**
   * Apply mask
   */
  void ApplyMask();

  /**
   * Multiply alpha
   */
  void MultiplyAlpha();

public:
  std::vector<Devel::PixelBuffer> pixelBuffers{};              ///< pixelBuffer handle after successful load
                                                               ///< or pixelBuffer to be masked image in the mask task
  VisualUrl                                url;                ///< url of the image to load
  EncodedImageBuffer                       encodedImageBuffer; ///< encoded buffer of the image to load
  uint32_t                                 id;                 ///< The unique id associated with this task.
  TextureManagerType::TextureId            textureId;          ///< textureId for loading
  ImageDimensions                          dimensions;         ///< dimensions to load
  FittingMode::Type                        fittingMode;        ///< fitting options
  SamplingMode::Type                       samplingMode;       ///< sampling options
  DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad;  ///< if the image's color should be multiplied by it's alpha

  Devel::PixelBuffer         maskPixelBuffer; ///< pixelBuffer of mask image
  float                      contentScale;    ///< The factor to scale the content
  Dali::AnimatedImageLoading animatedImageLoading;
  uint32_t                   frameIndex;

  bool orientationCorrection : 1; ///< if orientation correction is needed
  bool isMaskTask : 1;            ///< whether this task is for mask or not
  bool cropToMask : 1;            ///< Whether to crop the content to the mask size
  bool loadPlanes : 1;            ///< Whether to load image planes
  bool isReady    : 1;            ///< Whether this task ready to run
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_IMAGE_LOAD_THREAD_H

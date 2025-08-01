#ifndef DALI_TOOLKIT_FAST_TRACK_IMAGE_LOADING_TASK_H
#define DALI_TOOLKIT_FAST_TRACK_IMAGE_LOADING_TASK_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali/devel-api/adaptor-framework/texture-upload-manager.h>
#include <dali/integration-api/adaptor-framework/log-factory-interface.h>
#include <dali/public-api/adaptor-framework/async-task-manager.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/rendering/texture.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/image-loader/async-image-loader-devel.h>
#include <dali-toolkit/internal/texture-manager/texture-manager-type.h>
#include <dali-toolkit/internal/visuals/visual-url.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class FastTrackLoadingTask;
using FastTrackLoadingTaskPtr = IntrusivePtr<FastTrackLoadingTask>;

/**
 * @brief The task of loading image and uploading texture on fast track.
 * It is available that upload the loaded image data on texture at image loader threads.
 */
class FastTrackLoadingTask : public AsyncTask
{
public:
  /**
   * @brief Constructor.
   */
  FastTrackLoadingTask(const VisualUrl&                         url,
                       ImageDimensions                          dimensions,
                       FittingMode::Type                        fittingMode,
                       SamplingMode::Type                       samplingMode,
                       bool                                     orientationCorrection,
                       DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad,
                       bool                                     loadPlanes,
                       CallbackBase*                            callback);

  /**
   * @brief Destructor.
   */
  ~FastTrackLoadingTask() override;

public: // Implementation of AsyncTask
  /**
   * @copydoc Dali::AsyncTask::Process()
   */
  void Process() override;

  /**
   * @copydoc Dali::AsyncTask::GetTaskName()
   */
  std::string_view GetTaskName() const override
  {
    return "FastTrackLoadingTask";
  }

private:
  // Undefined
  FastTrackLoadingTask(const FastTrackLoadingTask& queue)            = delete;
  FastTrackLoadingTask& operator=(const FastTrackLoadingTask& queue) = delete;

  /**
   * @brief Create textures for this task.
   * @note This should be called in construct timing.
   */
  void PrepareTexture();

  /**
   * @brief Load the image.
   */
  void Load();

  /**
   * @brief Multiply alpha if required.
   *
   * @param[in,out] pixelBuffer target pixel buffer that need to be multiplied alpha.
   */
  void MultiplyAlpha(Dali::Devel::PixelBuffer pixelBuffer);

  /**
   * @brief Upload loaded pixelBuffer into texture
   */
  void UploadToTexture();

private:
  /**
   * @brief Complete callback.
   *
   * @param[in] task The pointer of task who call this callback.
   */
  void OnComplete(AsyncTaskPtr task);

public:
  VisualUrl                  mUrl;      ///< url of the image to load.
  std::vector<Dali::Texture> mTextures; ///< textures for regular image.

private:
  ImageDimensions                          mDimensions;   ///< dimensions to load
  FittingMode::Type                        mFittingMode;  ///< fitting options
  SamplingMode::Type                       mSamplingMode; ///< sampling options
  DevelAsyncImageLoader::PreMultiplyOnLoad mPreMultiplyOnLoad;
  std::unique_ptr<CallbackBase>            mCallback;

  // Texture Upload relative API
  Dali::Devel::TextureUploadManager mTextureUploadManager;

  // Note : mPixelData is invalid after upload requested. We should keep image size informations.
  struct ImageInformation
  {
    uint32_t resourceId;

    uint32_t      width;
    uint32_t      height;
    Pixel::Format format;
  };
  std::vector<ImageInformation> mImageInformations;

  std::vector<Dali::PixelData> mPixelData;

  bool mOrientationCorrection : 1; ///< If orientation correction is needed

public:
  bool mLoadSuccess : 1;         ///< Whether image load successed or not.
  bool mLoadPlanesAvaliable : 1; ///< If image valid to load as planes or not.
  bool mPremultiplied : 1;       ///< True if the image's color was multiplied by it's alpha
  bool mPlanesLoaded : 1;        ///< True if the image load as planes.
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_FAST_TRACK_IMAGE_LOADING_TASK_H

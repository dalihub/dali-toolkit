#ifndef DALI_TOOLKIT_REMOTE_DECODE_TASK_H
#define DALI_TOOLKIT_REMOTE_DECODE_TASK_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/async-task-manager.h>
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali/public-api/adaptor-framework/image-options.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/texture-manager/texture-manager-type.h>

// STD INCLUDES
#include <string>
#include <vector>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class RemoteDecodeTask;
using RemoteDecodeTaskPtr = IntrusivePtr<RemoteDecodeTask>;

/**
 * @brief Decode task for files already downloaded by RemoteFileDownloadManager.
 */
class RemoteDecodeTask : public AsyncTask
{
public:
  RemoteDecodeTask(TextureManagerType::TextureId             textureId,
                   const std::string&                        localFilePath,
                   const Dali::ImageDimensions&              desiredSize,
                   Dali::SamplingMode::Type                  samplingMode,
                   bool                                      orientationCorrection,
                   DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad,
                   bool                                      loadYuvPlanes,
                   CallbackBase*                             callback);

  ~RemoteDecodeTask() override;

public: // Implementation of AsyncTask
  void Process() override;

  Dali::StringView GetTaskName() const override
  {
    return "RemoteDecodeTask";
  }

public:
  TextureManagerType::TextureId   textureId;
  std::vector<Devel::PixelBuffer> pixelBuffers;

private:
  std::string                              mLocalFilePath;
  Dali::ImageDimensions                    mDesiredSize;
  Dali::SamplingMode::Type                 mSamplingMode;
  DevelAsyncImageLoader::PreMultiplyOnLoad mPreMultiplyOnLoad;
  bool                                     mOrientationCorrection : 1;
  bool                                     mLoadYuvPlanes : 1;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_REMOTE_DECODE_TASK_H

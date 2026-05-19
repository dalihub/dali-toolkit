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
 *
 */

// CLASS HEADER
#include <dali-toolkit/internal/image-loader/remote-decode-task.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

RemoteDecodeTask::RemoteDecodeTask(TextureManagerType::TextureId             textureId,
                                   const std::string&                        localFilePath,
                                   const Dali::ImageDimensions&              desiredSize,
                                   Dali::SamplingMode::Type                  samplingMode,
                                   bool                                      orientationCorrection,
                                   DevelAsyncImageLoader::PreMultiplyOnLoad  preMultiplyOnLoad,
                                   bool                                      loadYuvPlanes,
                                   CallbackBase*                             callback)
: AsyncTask(callback),
  textureId(textureId),
  pixelBuffers(),
  mLocalFilePath(localFilePath),
  mDesiredSize(desiredSize),
  mSamplingMode(samplingMode),
  mPreMultiplyOnLoad(preMultiplyOnLoad),
  mOrientationCorrection(orientationCorrection),
  mLoadYuvPlanes(loadYuvPlanes)
{
}

RemoteDecodeTask::~RemoteDecodeTask() = default;

void RemoteDecodeTask::Process()
{
  Devel::PixelBuffer pixelBuffer;

  if(!mLocalFilePath.empty())
  {
    if(mLoadYuvPlanes)
    {
      Dali::LoadImagePlanesFromFile(mLocalFilePath, pixelBuffers, mDesiredSize, mSamplingMode, mOrientationCorrection);
    }
    else
    {
      pixelBuffer = Dali::LoadImageFromFile(mLocalFilePath, mDesiredSize, mSamplingMode, mOrientationCorrection);
    }
  }

  if(pixelBuffer)
  {
    pixelBuffers.push_back(pixelBuffer);
  }

  if(!pixelBuffers.empty() && mPreMultiplyOnLoad == DevelAsyncImageLoader::PreMultiplyOnLoad::ON)
  {
    pixelBuffers[0].MultiplyColorByAlpha();
  }

  if(pixelBuffers.empty())
  {
    DALI_LOG_ERROR("RemoteDecodeTask::Process: Loading is failed: %s\n", mLocalFilePath.c_str());
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

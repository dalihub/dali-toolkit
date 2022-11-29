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
 *
 */

// CLASS HEADER
#include "svg-task.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/file-loader.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/svg/svg-visual.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

SvgTask::SvgTask(VectorImageRenderer vectorRenderer, CallbackBase* callback)
: AsyncTask(callback),
  mVectorRenderer(vectorRenderer),
  mHasSucceeded(false)
{
}

PixelData SvgTask::GetPixelData() const
{
  return PixelData();
}

bool SvgTask::HasSucceeded() const
{
  return mHasSucceeded;
}

VectorImageRenderer SvgTask::GetRenderer()
{
  return mVectorRenderer;
}

SvgLoadingTask::SvgLoadingTask(VectorImageRenderer vectorRenderer, const VisualUrl& url, float dpi, CallbackBase* callback)
: SvgTask(vectorRenderer, callback),
  mUrl(url),
  mDpi(dpi)
{
}

SvgLoadingTask::~SvgLoadingTask()
{
}

void SvgLoadingTask::Process()
{
  if(mVectorRenderer.IsLoaded())
  {
    // Already loaded
    mHasSucceeded = true;
    return;
  }

  Dali::Vector<uint8_t> buffer;

  if(!mUrl.IsLocalResource())
  {
    if(!Dali::FileLoader::DownloadFileSynchronously(mUrl.GetUrl(), buffer))
    {
      DALI_LOG_ERROR("Failed to download file! [%s]\n", mUrl.GetUrl().c_str());
      return;
    }
  }
  else
  {
    if(!Dali::FileLoader::ReadFile(mUrl.GetUrl(), buffer))
    {
      DALI_LOG_ERROR("Failed to read file! [%s]\n", mUrl.GetUrl().c_str());
      return;
    }
  }

  buffer.PushBack('\0');

  if(!mVectorRenderer.Load(buffer, mDpi))
  {
    DALI_LOG_ERROR("Failed to load data! [%s]\n", mUrl.GetUrl().c_str());
    return;
  }

  mHasSucceeded = true;
}

bool SvgLoadingTask::IsReady()
{
  return true;
}

SvgRasterizingTask::SvgRasterizingTask(VectorImageRenderer vectorRenderer, uint32_t width, uint32_t height, CallbackBase* callback)
: SvgTask(vectorRenderer, callback),
  mWidth(width),
  mHeight(height)
{
}

SvgRasterizingTask::~SvgRasterizingTask()
{
}

void SvgRasterizingTask::Process()
{
  if(!mVectorRenderer.IsLoaded())
  {
    DALI_LOG_ERROR("File is not loaded!\n");
    return;
  }

  Devel::PixelBuffer pixelBuffer = mVectorRenderer.Rasterize(mWidth, mHeight);
  if(!pixelBuffer)
  {
    DALI_LOG_ERROR("Rasterize is failed!\n");
    return;
  }

  mPixelData    = Devel::PixelBuffer::Convert(pixelBuffer);
  mHasSucceeded = true;
}

bool SvgRasterizingTask::IsReady()
{
  return mVectorRenderer.IsLoaded();
}

PixelData SvgRasterizingTask::GetPixelData() const
{
  return mPixelData;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

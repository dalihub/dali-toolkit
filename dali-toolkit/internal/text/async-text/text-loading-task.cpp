/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/async-text/text-loading-task.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/thread-settings.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_TEXT_ASYNC, false);
} // namespace

TextLoadingTask::TextLoadingTask(const uint32_t id, const Text::AsyncTextParameters& parameters, CallbackBase* callback)
: AsyncTask(callback),
  mId(id),
  mParameters(parameters),
  mRenderInfo(),
  mIsReady(false),
  mMutex()
{
}

TextLoadingTask::TextLoadingTask(const uint32_t id, CallbackBase* callback)
: AsyncTask(callback),
  mId(id),
  mIsReady(true),
  mMutex()
{
  // Empty task for wake up the async task manger.
}

TextLoadingTask::~TextLoadingTask()
{
}

uint32_t TextLoadingTask::GetId()
{
  return mId;
}

void TextLoadingTask::SetLoader(Text::AsyncTextLoader& loader)
{
  Dali::Mutex::ScopedLock lock(mMutex);
  mLoader  = loader;
  mIsReady = true;
}

void TextLoadingTask::Process()
{
  if(mId == 0u)
  {
    return;
  }
  DALI_TRACE_SCOPE(gTraceFilter, "DALI_TEXT_ASYNC_LOADING_TASK_PROCESS");
  Load();
}

bool TextLoadingTask::IsReady()
{
  return mIsReady;
}

void TextLoadingTask::Load()
{
  switch(mParameters.requestType)
  {
    case Text::Async::RENDER_FIXED_SIZE:
    case Text::Async::RENDER_FIXED_WIDTH:
    case Text::Async::RENDER_CONSTRAINT:
    {
      if(mParameters.isAutoScrollEnabled && !mParameters.isMultiLine)
      {
#ifdef TRACE_ENABLED
        if(gTraceFilter && gTraceFilter->IsTraceEnabled())
        {
          DALI_LOG_RELEASE_INFO("RenderAutoScroll\n");
        }
#endif
        mRenderInfo = mLoader.RenderAutoScroll(mParameters);
      }
      else if(mParameters.isTextFitEnabled || mParameters.isTextFitArrayEnabled)
      {
#ifdef TRACE_ENABLED
        if(gTraceFilter && gTraceFilter->IsTraceEnabled())
        {
          DALI_LOG_RELEASE_INFO("RenderTextFit\n");
        }
#endif
        mRenderInfo = mLoader.RenderTextFit(mParameters);
      }
      else
      {
#ifdef TRACE_ENABLED
        if(gTraceFilter && gTraceFilter->IsTraceEnabled())
        {
          DALI_LOG_RELEASE_INFO("RenderText\n");
        }
#endif
        mRenderInfo = mLoader.RenderText(mParameters);
      }
      break;
    }
    case Text::Async::COMPUTE_NATURAL_SIZE:
    {
#ifdef TRACE_ENABLED
      if(gTraceFilter && gTraceFilter->IsTraceEnabled())
      {
        DALI_LOG_RELEASE_INFO("GetNaturalSize\n");
      }
#endif
      mRenderInfo = mLoader.GetNaturalSize(mParameters);
      break;
    }
    case Text::Async::COMPUTE_HEIGHT_FOR_WIDTH:
    {
#ifdef TRACE_ENABLED
      if(gTraceFilter && gTraceFilter->IsTraceEnabled())
      {
        DALI_LOG_RELEASE_INFO("GetHeightForWidth\n");
      }
#endif
      mRenderInfo = mLoader.GetHeightForWidth(mParameters);
      break;
    }
    default:
    {
      DALI_LOG_ERROR("Unexpected request type recieved : %d\n", mParameters.requestType);
      break;
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

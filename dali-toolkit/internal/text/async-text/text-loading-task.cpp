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
 *
 */

// CLASS HEADER
#include <dali-toolkit/internal/text/async-text/text-loading-task.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/thread-settings.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>
#include <dali/public-api/signals/callback.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/async-text/async-text-manager-impl.h> ///< To call AsyncTextManager::ReleaseLoader
#include <dali-toolkit/internal/text/async-text/async-text-manager.h>

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

TextLoadingTask::TextLoadingTask(const uint32_t id, const Text::AsyncTextParameters& parameters, Dali::AsyncTaskManager asyncTaskManager, CallbackBase* callback)
: AsyncTask(callback),
  mId(id),
  mParameters(parameters),
  mRenderInfo(),
  mAsyncTaskManager(asyncTaskManager),
  mAsyncTextManager(),
  mIsReady(false),
  mMutex()
{
}

TextLoadingTask::~TextLoadingTask()
{
  if(DALI_LIKELY(Dali::Adaptor::IsAvailable()))
  {
    // To avoid loader leaking. Never ever happend, but for safety.
    if(DALI_UNLIKELY(mAsyncTextManager && mLoader))
    {
      DALI_LOG_ERROR("Need to release loader!!");
      mAsyncTextManager->ReleaseLoader(nullptr, mLoader);
    }
  }
}

uint32_t TextLoadingTask::GetId()
{
  return mId;
}

void TextLoadingTask::SetLoader(Text::AsyncTextLoader& loader, TextLoadingTask::ReleaseCallbackReceiver releaseCallbackReceiver)
{
  {
    Dali::Mutex::ScopedLock lock(mMutex);
    mLoader = loader;
  }

  mAsyncTextManager = releaseCallbackReceiver;

  if(DALI_LIKELY(!mIsReady && mLoader))
  {
    mIsReady = true;
    NotifyToReady();
    mAsyncTaskManager.Reset();
  }
}

void TextLoadingTask::Process()
{
  if(mId == 0u)
  {
    return;
  }
  {
    DALI_TRACE_SCOPE(gTraceFilter, "DALI_TEXT_ASYNC_LOADING_TASK_PROCESS");
    Load();
  }
  if(DALI_LIKELY(mAsyncTextManager))
  {
    DALI_TRACE_SCOPE(gTraceFilter, "DALI_TEXT_ASYNC_LOADING_TASK_RELEASE");
    ReleaseLoader();
  }
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
    case Text::Async::RENDER_FIXED_HEIGHT:
    case Text::Async::RENDER_CONSTRAINT:
    {
      // To avoid duplicate calculation, we can skip Initialize and Update.
      Size naturalSize       = Size::ZERO;
      bool cachedNaturalSize = false;

      if(mParameters.renderScale > 1.0f)
      {
#ifdef TRACE_ENABLED
        if(gTraceFilter && gTraceFilter->IsTraceEnabled())
        {
          DALI_LOG_RELEASE_INFO("SetupRenderScale : %f\n", mParameters.renderScale);
        }
#endif
        naturalSize = mLoader.SetupRenderScale(mParameters, cachedNaturalSize);
      }

      if(mParameters.ellipsis && !mParameters.isMultiLine && mParameters.ellipsisMode == DevelText::Ellipsize::AUTO_SCROLL)
      {
        if(!cachedNaturalSize)
        {
          naturalSize       = mLoader.ComputeNaturalSize(mParameters);
          cachedNaturalSize = true;
        }
        if(mParameters.textWidth < naturalSize.width)
        {
#ifdef TRACE_ENABLED
          if(gTraceFilter && gTraceFilter->IsTraceEnabled())
          {
            DALI_LOG_RELEASE_INFO("RenderAutoScroll, Ellipsize::AUTO_SCROLL\n");
          }
#endif
          mParameters.isAutoScrollEnabled = true;
          mRenderInfo                     = mLoader.RenderAutoScroll(mParameters, cachedNaturalSize, naturalSize);
        }
        else
        {
#ifdef TRACE_ENABLED
          if(gTraceFilter && gTraceFilter->IsTraceEnabled())
          {
            DALI_LOG_RELEASE_INFO("RenderText, Ellipsize::AUTO_SCROLL\n");
          }
#endif
          mRenderInfo = mLoader.RenderText(mParameters, cachedNaturalSize, naturalSize);
        }
      }
      else if(mParameters.isAutoScrollEnabled && !mParameters.isMultiLine)
      {
#ifdef TRACE_ENABLED
        if(gTraceFilter && gTraceFilter->IsTraceEnabled())
        {
          DALI_LOG_RELEASE_INFO("RenderAutoScroll\n");
        }
#endif
        mRenderInfo = mLoader.RenderAutoScroll(mParameters, cachedNaturalSize, naturalSize);
      }
      else if(mParameters.isTextFitEnabled || mParameters.isTextFitArrayEnabled)
      {
#ifdef TRACE_ENABLED
        if(gTraceFilter && gTraceFilter->IsTraceEnabled())
        {
          DALI_LOG_RELEASE_INFO("RenderTextFit\n");
        }
#endif
        mRenderInfo = mLoader.RenderTextFit(mParameters, cachedNaturalSize, naturalSize);
      }
      else
      {
#ifdef TRACE_ENABLED
        if(gTraceFilter && gTraceFilter->IsTraceEnabled())
        {
          DALI_LOG_RELEASE_INFO("RenderText\n");
        }
#endif
        mRenderInfo = mLoader.RenderText(mParameters, cachedNaturalSize, naturalSize);
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

void TextLoadingTask::ReleaseLoader()
{
  if(DALI_LIKELY(mAsyncTextManager))
  {
    // Release all local varaibles before execute callback.
    if(DALI_LIKELY(mLoader))
    {
      auto loader = std::move(mLoader);
      mAsyncTextManager->ReleaseLoader(this, loader);
    }
    mAsyncTextManager.Reset();
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

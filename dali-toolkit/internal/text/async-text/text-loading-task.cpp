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
#include <dali-toolkit/internal/text/async-text/text-loading-task.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/thread-settings.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/signals/callback.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/async-text/async-text-manager-impl.h> ///< To call AsyncTextManager::ReleaseLoader
#include <dali-toolkit/internal/text/async-text/async-text-manager.h>
#include <dali-toolkit/internal/text/text-trace.h>

namespace DALI_NAMESPACE
{
namespace Toolkit
{
namespace Internal
{
namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_TEXT_ASYNC_PERFORMANCE_MARKER, false);
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

TextLoadingTask::~TextLoadingTask()
{
  if(DALI_LIKELY(Dali::Adaptor::IsAvailable()))
  {
    // To avoid loader leaking. Never ever happend, but for safety.
    if(DALI_UNLIKELY(mLoader))
    {
      DALI_LOG_ERROR("Need to release loader!!");
      Text::Internal::AsyncTextManager::ReleaseLoaderToManager(nullptr, mLoader);
    }
  }
}

uint32_t TextLoadingTask::GetId()
{
  return mId;
}

void TextLoadingTask::SetLoader(Text::AsyncTextLoader& loader)
{
  mLoader = loader;

  if(DALI_LIKELY(!mIsReady && mLoader))
  {
    {
      Dali::Mutex::ScopedLock lock(mMutex);
      mIsReady = true;
    }
    NotifyToReady();
  }
}

void TextLoadingTask::Process()
{
  if(DALI_LIKELY(mId != 0u))
  {
    DALI_TEXT_TRACE_SCOPE(gTraceFilter, "DALI_TEXT_ASYNC_PROCESS");
    Load();
  }
  ReleaseLoader();
}

bool TextLoadingTask::IsReady()
{
  Dali::Mutex::ScopedLock lock(mMutex);
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
        naturalSize = mLoader.SetupRenderScale(mParameters, cachedNaturalSize);
      }

      if(mParameters.ellipsis && mParameters.ellipsisMode == DevelText::Ellipsize::AUTO_SCROLL)
      {
        if(mParameters.autoScrollDirection == DevelText::AutoScroll::HORIZONTAL)
        {
          if(mParameters.isMultiLine)
          {
            DALI_LOG_DEBUG_INFO("Attempted ellipsize auto scroll horizontal on a non SINGLE_LINE_BOX, request ignored\n");
            mRenderInfo = mLoader.RenderText(mParameters, cachedNaturalSize, naturalSize);
          }
          else
          {
            if(!cachedNaturalSize)
            {
              naturalSize       = mLoader.ComputeNaturalSize(mParameters);
              cachedNaturalSize = true;
            }
            if(mParameters.textWidth < naturalSize.width)
            {
              mParameters.isAutoScrollEnabled = true;
              mRenderInfo                     = mLoader.RenderAutoScroll(mParameters, cachedNaturalSize, naturalSize);
            }
            else
            {
              mRenderInfo = mLoader.RenderText(mParameters, cachedNaturalSize, naturalSize);
            }
          }
        }
        else // AutoScroll::VERTICAL
        {
          const float textHeight = mLoader.ComputeHeightForWidth(mParameters, mParameters.textWidth, cachedNaturalSize);
          if(mParameters.textHeight < textHeight)
          {
            mParameters.isAutoScrollEnabled = true;
            mRenderInfo                     = mLoader.RenderAutoScroll(mParameters, true, naturalSize);
          }
          else
          {
            mRenderInfo = mLoader.RenderText(mParameters, cachedNaturalSize, naturalSize);
          }
        }
      }
      else if(mParameters.isAutoScrollEnabled &&
              ((!mParameters.isMultiLine && mParameters.autoScrollDirection == DevelText::AutoScroll::HORIZONTAL) ||
               (mParameters.isMultiLine && mParameters.autoScrollDirection == DevelText::AutoScroll::VERTICAL)))
      {
        mRenderInfo = mLoader.RenderAutoScroll(mParameters, cachedNaturalSize, naturalSize);
      }
      else if(mParameters.isTextFitEnabled || mParameters.isTextFitArrayEnabled)
      {
        mRenderInfo = mLoader.RenderTextFit(mParameters, cachedNaturalSize, naturalSize);
      }
      else
      {
        mRenderInfo = mLoader.RenderText(mParameters, cachedNaturalSize, naturalSize);
      }
      break;
    }
    case Text::Async::COMPUTE_NATURAL_SIZE:
    {
      mRenderInfo = mLoader.GetNaturalSize(mParameters);
      break;
    }
    case Text::Async::COMPUTE_HEIGHT_FOR_WIDTH:
    {
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
  // Release all local varaibles before execute callback.
  if(DALI_LIKELY(mLoader))
  {
    auto loader = std::move(mLoader);
    Text::Internal::AsyncTextManager::ReleaseLoaderToManager(this, loader);
  }
}

} // namespace Internal

} // namespace Toolkit

} //namespace DALI_NAMESPACE

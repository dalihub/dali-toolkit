/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/controls/gl-view/gl-view-render-thread.h>

//EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/thread-settings.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <chrono>
#include <thread>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
constexpr unsigned int NANOSECONDS_PER_SECOND(1e+9);

// The following values will get calculated at compile time
constexpr float    DEFAULT_FRAME_DURATION_IN_SECONDS(1.0f / 60.0f);
constexpr uint64_t DEFAULT_FRAME_DURATION_IN_NANOSECONDS(DEFAULT_FRAME_DURATION_IN_SECONDS* NANOSECONDS_PER_SECOND);

} // namespace

GlViewRenderThread::GlViewRenderThread(Dali::NativeImageSourceQueuePtr queue)
: mLogFactory(Dali::Adaptor::Get().GetLogFactory()),
  mSurfaceSize(1, 1),
  mNativeImageSurface(),
  mNativeImageQueue(queue),
  mSurfaceSemaphore(1),
  mGlInitCallback(nullptr),
  mGlRenderFrameCallback(nullptr),
  mGlTerminateCallback(nullptr),
  mResizeCallback(nullptr),
  mDepth(false),
  mStencil(false),
  mMSAA(0),
  mGraphicsApiVersion(20),
  mConditionalWait(),
  mIsThreadStarted(0),
  mIsThreadStopped(0),
  mIsThreadPaused(0),
  mIsRenderRequested(0),
  mRenderingMode(0),
  mIsSurfaceResized(0),
  mDefaultFrameDurationNanoseconds(DEFAULT_FRAME_DURATION_IN_NANOSECONDS)
{
  mNativeImageSurface = Dali::NativeImageSurface::New(mNativeImageQueue);

  if(!mNativeImageSurface)
  {
    DALI_LOG_ERROR("Creating NativeImageSurface Failed, Could not start GlView Render Thread");
  }
}

void GlViewRenderThread::RegisterGlCallbacks(CallbackBase* initCallback, CallbackBase* renderFrameCallback, CallbackBase* terminateCallback)
{
  if(!mGlInitCallback && !mGlRenderFrameCallback && !mGlTerminateCallback)
  {
    mGlInitCallback        = std::unique_ptr<CallbackBase>(initCallback);
    mGlRenderFrameCallback = std::unique_ptr<CallbackBase>(renderFrameCallback);
    mGlTerminateCallback   = std::unique_ptr<CallbackBase>(terminateCallback);
  }
}

void GlViewRenderThread::SetResizeCallback(CallbackBase* resizeCallback)
{
  if(!mResizeCallback)
  {
    mResizeCallback = std::unique_ptr<CallbackBase>(resizeCallback);
  }
}

bool GlViewRenderThread::SetGraphicsConfig(bool depth, bool stencil, int msaa, int version)
{
  mDepth              = depth;
  mStencil            = stencil;
  mMSAA               = msaa;
  mGraphicsApiVersion = version;

  if(mNativeImageSurface)
  {
    return mNativeImageSurface->SetGraphicsConfig(mDepth, mStencil, mMSAA, mGraphicsApiVersion);
  }

  return false;
}

void GlViewRenderThread::SetOnDemandRenderMode(bool onDemand)
{
  ConditionalWait::ScopedLock lock(mConditionalWait);
  mRenderingMode = static_cast<unsigned int>(onDemand);
  DALI_LOG_RELEASE_INFO("GlViewRenderThread::SetOnDemandRenderMode(): mRenderingMode: %d\n", mRenderingMode);
  if(!onDemand && !mIsThreadPaused)
  {
    mConditionalWait.Notify(lock);
  }
}

void GlViewRenderThread::SetSurfaceSize(Dali::Vector2 size)
{
  //GlViewRenderThread::Run was already blocked in Internal::GlView::OnSizeSet
  mSurfaceSize      = size;
  mIsSurfaceResized = 1;
}

void GlViewRenderThread::RenderOnce()
{
  //Notify GLRender thread.
  Dali::ConditionalWait::ScopedLock lock(mConditionalWait);
  mIsRenderRequested = 1;
  mConditionalWait.Notify(lock);
}

void GlViewRenderThread::GetNanoSeconds(uint64_t& timeInNanoseconds)
{
  // Get the time of a monotonic clock since its epoch.
  auto epoch        = std::chrono::steady_clock::now().time_since_epoch();
  auto duration     = std::chrono::duration_cast<std::chrono::nanoseconds>(epoch);
  timeInNanoseconds = static_cast<uint64_t>(duration.count());
}

void GlViewRenderThread::SleepUntil(uint64_t timeInNanoseconds)
{
  using Clock     = std::chrono::steady_clock;
  using TimePoint = std::chrono::time_point<Clock>;

  const Clock::duration duration = std::chrono::nanoseconds(timeInNanoseconds);
  const TimePoint       timePoint(duration);

  std::this_thread::sleep_until(timePoint);
}

void GlViewRenderThread::Run()
{
  Dali::SetThreadName("GlViewRenderer");
  mLogFactory.InstallLogFunction();

  int renderFrameResult = 0;

  if(!mNativeImageSurface)
  {
    DALI_LOG_ERROR("NativeImageSurface is null, Could not start GlView Render Thread");
    return;
  }

  AcquireSurface();
  mNativeImageSurface->InitializeGraphics();
  ReleaseSurface();

  mNativeImageSurface->PreRender();
  if(mGlInitCallback)
  {
    CallbackBase::Execute(*mGlInitCallback);
  }

  uint64_t timeToSleepUntil = 0;

  while(RenderReady(timeToSleepUntil))
  {
    uint64_t currentFrameStartTime = 0;
    GetNanoSeconds(currentFrameStartTime);

    AcquireSurface();
    mNativeImageSurface->PreRender();
    if(mIsSurfaceResized)
    {
      if(mResizeCallback)
      {
        CallbackBase::Execute(*mResizeCallback, static_cast<int>(mSurfaceSize.x), static_cast<int>(mSurfaceSize.y));
      }
      mIsSurfaceResized = 0;
    }

    if(mNativeImageSurface->CanRender())
    {
      if(mGlRenderFrameCallback)
      {
        renderFrameResult = CallbackBase::ExecuteReturn<int>(*mGlRenderFrameCallback);
        if(renderFrameResult)
        {
          mNativeImageSurface->PostRender();
        }
      }
    }

    ReleaseSurface();

    if(timeToSleepUntil == 0)
    {
      timeToSleepUntil = currentFrameStartTime + mDefaultFrameDurationNanoseconds;
    }
    else
    {
      timeToSleepUntil += mDefaultFrameDurationNanoseconds;
      uint64_t currentFrameEndTime = 0;
      GetNanoSeconds(currentFrameEndTime);
      while(currentFrameEndTime > timeToSleepUntil + mDefaultFrameDurationNanoseconds)
      {
        timeToSleepUntil += mDefaultFrameDurationNanoseconds;
      }
    }

    SleepUntil(timeToSleepUntil);
  }

  if(mGlTerminateCallback)
  {
    CallbackBase::Execute(*mGlTerminateCallback);
  }

  mNativeImageSurface->TerminateGraphics();

  return;
}

void GlViewRenderThread::Stop()
{
  // Set to come out Render Thread out of waiting condition.
  Dali::ConditionalWait::ScopedLock lock(mConditionalWait);
  mIsThreadStopped = 1;
  mIsThreadPaused  = 0;
  mConditionalWait.Notify(lock);
}

void GlViewRenderThread::Pause()
{
  //Notify GLRender thread, If actor visibility is change
  Dali::ConditionalWait::ScopedLock lock(mConditionalWait);
  mIsThreadPaused = 1;
  mConditionalWait.Notify(lock);
}

void GlViewRenderThread::Resume()
{
  Dali::ConditionalWait::ScopedLock lock(mConditionalWait);
  if(!mIsThreadStarted)
  {
    Start();
    mIsThreadStarted = 1;
  }
  mIsThreadPaused = 0;
  mConditionalWait.Notify(lock);
}

bool GlViewRenderThread::RenderReady(uint64_t& timeToSleepUntil)
{
  ConditionalWait::ScopedLock lock(mConditionalWait);
  while((!mIsThreadStopped && mRenderingMode && !mIsRenderRequested) || mIsThreadPaused)
  {
    timeToSleepUntil = 0;
    mConditionalWait.Wait(lock);
  }

  mIsRenderRequested = 0;
  // Keep the update-render thread alive if this thread is NOT to be destroyed
  return !mIsThreadStopped;
}

void GlViewRenderThread::AcquireSurface()
{
  mSurfaceSemaphore.Acquire();
}

void GlViewRenderThread::ReleaseSurface()
{
  mSurfaceSemaphore.Release(1);
}

GlViewRenderThread::~GlViewRenderThread()
{
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali

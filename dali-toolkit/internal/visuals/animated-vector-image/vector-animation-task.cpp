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
#include <dali-toolkit/internal/visuals/animated-vector-image/vector-animation-task.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/file-loader.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/object/property-array.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/animated-vector-image/vector-animation-manager.h>
#include <dali-toolkit/internal/visuals/animated-vector-image/vector-animation-thread.h>
#include <dali-toolkit/internal/visuals/image/image-visual-shader-factory.h>

#ifdef TRACE_ENABLED
#include <chrono>
#include <iomanip>
#include <sstream>
#include <thread>
#endif

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
constexpr auto LOOP_FOREVER = -1;
constexpr auto MICROSECONDS_PER_SECOND(1e+6);

#if defined(DEBUG_ENABLED)
Debug::Filter* gVectorAnimationLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_VECTOR_ANIMATION");
#endif

DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_VECTOR_ANIMATION_PERFORMANCE_MARKER, false);

#ifdef TRACE_ENABLED
uint64_t GetNanoseconds()
{
  // Get the time of a monotonic clock since its epoch.
  auto epoch    = std::chrono::steady_clock::now().time_since_epoch();
  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(epoch);
  return static_cast<uint64_t>(duration.count());
}
#endif

int64_t CalculateFrameDurationMicroSeconds(const float frameRate, const float frameSpeedFactor)
{
  return static_cast<int64_t>(MICROSECONDS_PER_SECOND / static_cast<double>(frameRate * frameSpeedFactor));
}

} // unnamed namespace

VectorAnimationTask::VectorAnimationTask(VisualFactoryCache& factoryCache)
: AsyncTask(MakeCallback(this, &VectorAnimationTask::TaskCompleted), AsyncTask::PriorityType::HIGH, AsyncTask::ThreadType::WORKER_THREAD),
  mImageUrl(),
  mEncodedImageBuffer(),
  mVectorRenderer(VectorAnimationRenderer::New()),
  mAnimationData(),
  mVectorAnimationThread(factoryCache.GetVectorAnimationManager().GetVectorAnimationThread()),
  mMutex(),
  mResourceReadySignal(),
  mLoadCompletedCallback(MakeCallback(this, &VectorAnimationTask::OnLoadCompleted)),
  mCachedLayerInfo(),
  mCachedMarkerInfo(),
  mPlayState(PlayState::STOPPED),
  mStopBehavior(DevelImageVisual::StopBehavior::CURRENT_FRAME),
  mLoopingMode(DevelImageVisual::LoopingMode::RESTART),
  mNextFrameStartTime(),
  mFrameDurationMicroSeconds(MICROSECONDS_PER_SECOND / 60.0f),
  mFrameRate(60.0f),
  mFrameSpeedFactor(1.0f),
  mCurrentFrame(0),
  mTotalFrame(0),
  mStartFrame(0),
  mEndFrame(0),
  mDroppedFrames(0),
  mWidth(0),
  mHeight(0),
  mAnimationDataIndex(0),
  mAppliedPlayStateId(0u),
  mLoopCount(LOOP_FOREVER),
  mCurrentLoop(0),
  mForward(true),
  mUpdateFrameNumber(false),
  mNeedAnimationFinishedTrigger(true),
  mNeedForceRenderOnceTrigger(false),
  mAnimationDataUpdated(false),
  mDestroyTask(false),
  mLoadRequest(false),
  mLoadFailed(false),
  mRasterized(false),
  mKeepAnimation(false),
  mLayerInfoCached(false),
  mMarkerInfoCached(false),
  mEnableFrameCache(false),
  mNotifyAfterRasterization(false),
  mSizeUpdated(false)
{
  mVectorRenderer.UploadCompletedSignal().Connect(this, &VectorAnimationTask::OnUploadCompleted);
}

VectorAnimationTask::~VectorAnimationTask()
{
  DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::~VectorAnimationTask: destructor [%p]\n", this);
}

void VectorAnimationTask::Process()
{
  mRasterized = Rasterize();
}

void VectorAnimationTask::Finalize()
{
  {
    Mutex::ScopedLock lock(mMutex);

    // Release some objects in the main thread
    if(mAnimationFinishedCallback)
    {
      mVectorAnimationThread.RemoveEventTriggerCallbacks(mAnimationFinishedCallback.get());
      mAnimationFinishedCallback.reset();
    }
    if(mLoadCompletedCallback)
    {
      mVectorAnimationThread.RemoveEventTriggerCallbacks(mLoadCompletedCallback.get());
      mLoadCompletedCallback.reset();
    }

    mDestroyTask = true;
  }

  mVectorRenderer.Finalize();
}

void VectorAnimationTask::TaskCompleted(VectorAnimationTaskPtr task)
{
  mVectorAnimationThread.OnTaskCompleted(task, task->IsRasterized(), task->IsAnimating());
}

bool VectorAnimationTask::IsRasterized()
{
  return mRasterized;
}

bool VectorAnimationTask::IsAnimating()
{
  return mKeepAnimation;
}

bool VectorAnimationTask::Load(bool synchronousLoading)
{
#ifdef TRACE_ENABLED
  uint64_t mStartTimeNanoSceonds = 0;
  uint64_t mEndTimeNanoSceonds   = 0;
#endif

  DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_LOTTIE_LOADING_TASK", [&](std::ostringstream& oss)
                                          {
    mStartTimeNanoSceonds = GetNanoseconds();
    oss << "[u:" << mImageUrl.GetEllipsedUrl() << "]"; });

  if(mEncodedImageBuffer)
  {
    if(!mVectorRenderer.Load(mEncodedImageBuffer.GetRawBuffer()))
    {
      mLoadFailed = true;
    }

    // We don't need to hold image buffer anymore.
    mEncodedImageBuffer.Reset();
  }
  else if(mImageUrl.IsLocalResource())
  {
    if(!mVectorRenderer.Load(mImageUrl.GetUrl()))
    {
      mLoadFailed = true;
    }
  }
  else
  {
    Dali::Vector<uint8_t> remoteData;
    if(!Dali::FileLoader::DownloadFileSynchronously(mImageUrl.GetUrl(), remoteData) || // Failed if we fail to download json file,
       !mVectorRenderer.Load(remoteData))                                              // or download data is not valid vector animation file.
    {
      mLoadFailed = true;
    }
  }

  if(mLoadFailed)
  {
    DALI_LOG_ERROR("VectorAnimationTask::Load: Load failed [%s]\n", mImageUrl.GetUrl().c_str());
    mLoadRequest = false;
    {
      Mutex::ScopedLock lock(mMutex);
      if(!synchronousLoading && mLoadCompletedCallback)
      {
        mVectorAnimationThread.AddEventTriggerCallback(mLoadCompletedCallback.get(), 0u);
      }
    }

    DALI_TRACE_END_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_LOTTIE_LOADING_TASK", [&](std::ostringstream& oss)
                                          {
      mEndTimeNanoSceonds = GetNanoseconds();
      oss << std::fixed << std::setprecision(3);
      oss << "[";
      oss << "d:" << static_cast<float>(mEndTimeNanoSceonds - mStartTimeNanoSceonds) / 1000000.0f << "ms ";
      oss << "u:" << mImageUrl.GetEllipsedUrl() << "]"; });
    return false;
  }

  mTotalFrame = mVectorRenderer.GetTotalFrameNumber();

  mEndFrame = mTotalFrame - 1;

  mFrameRate = mVectorRenderer.GetFrameRate();

  mFrameDurationMicroSeconds = CalculateFrameDurationMicroSeconds(mFrameRate, mFrameSpeedFactor);

  mLoadRequest = false;
  {
    Mutex::ScopedLock lock(mMutex);
    if(!synchronousLoading && mLoadCompletedCallback)
    {
      mVectorAnimationThread.AddEventTriggerCallback(mLoadCompletedCallback.get(), 0u);
    }
  }

  DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::Load: file = %s [%d frames, %f fps] [%p]\n", mImageUrl.GetUrl().c_str(), mTotalFrame, mFrameRate, this);

  DALI_TRACE_END_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_LOTTIE_LOADING_TASK", [&](std::ostringstream& oss)
                                        {
    mEndTimeNanoSceonds = GetNanoseconds();
    oss << std::fixed << std::setprecision(3);
    oss << "[";
    oss << "d:" << static_cast<float>(mEndTimeNanoSceonds - mStartTimeNanoSceonds) / 1000000.0f << "ms ";
    oss << "u:" << mImageUrl.GetEllipsedUrl() << "]"; });

  return true;
}

void VectorAnimationTask::SetRenderer(Renderer renderer)
{
  mVectorRenderer.SetRenderer(renderer);

  DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::SetRenderer [%p]\n", this);
}

void VectorAnimationTask::RequestLoad(const VisualUrl& url, EncodedImageBuffer encodedImageBuffer, bool synchronousLoading)
{
  mImageUrl           = url;
  mEncodedImageBuffer = encodedImageBuffer;

  if(!synchronousLoading)
  {
    mLoadRequest = true;

    mVectorAnimationThread.AddTask(this);
  }
  else
  {
    Load(true);

    OnLoadCompleted(0u);
  }
}

bool VectorAnimationTask::IsLoadRequested() const
{
  return mLoadRequest;
}

void VectorAnimationTask::SetAnimationData(const AnimationData& data)
{
  Mutex::ScopedLock lock(mMutex);

  DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::SetAnimationData [%p]\n", this);

  uint32_t index = mAnimationDataIndex == 0 ? 1 : 0; // Use the other buffer

  mAnimationData[index].push_back(data);
  mAnimationDataUpdated = true;

  if(data.resendFlag & VectorAnimationTask::RESEND_SIZE)
  {
    // The size should be changed in the main thread.
    SetSize(data.width, data.height);
  }

  mVectorAnimationThread.AddTask(this);
}

void VectorAnimationTask::SetSize(uint32_t width, uint32_t height)
{
  if(mWidth != width || mHeight != height)
  {
    mVectorRenderer.SetSize(width, height);

    mWidth  = width;
    mHeight = height;

    // If fixedCache is enabled, Call KeepRasterizedBuffer()
    if(mEnableFrameCache)
    {
      if(mTotalFrame > 0 && !mLoadFailed)
      {
        mVectorRenderer.KeepRasterizedBuffer();
      }
      else
      {
        // If Load is not yet, update the size later.
        mSizeUpdated = true;
      }
    }

    DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::SetSize: width = %d, height = %d [%p]\n", width, height, this);
  }
}

void VectorAnimationTask::PlayAnimation()
{
  if(mPlayState != PlayState::PLAYING)
  {
    mNeedAnimationFinishedTrigger = true;
    mUpdateFrameNumber            = false;
    mPlayState                    = PlayState::PLAYING;

    DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::PlayAnimation: Play [%p]\n", this);
  }
}

void VectorAnimationTask::StopAnimation()
{
  if(mPlayState != PlayState::STOPPING)
  {
    mNeedAnimationFinishedTrigger = false;
    mPlayState                    = PlayState::STOPPING;

    DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::StopAnimation: Stop [%p]\n", this);
  }
}

void VectorAnimationTask::PauseAnimation()
{
  if(mPlayState == PlayState::PLAYING)
  {
    mPlayState = PlayState::PAUSED;

    // Ensure to render paused frame.
    mNeedForceRenderOnceTrigger = true;

    DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::PauseAnimation: Pause [%p]\n", this);
  }
}

void VectorAnimationTask::SetAnimationFinishedCallback(CallbackBase* callback)
{
  Mutex::ScopedLock lock(mMutex);
  mAnimationFinishedCallback = std::unique_ptr<CallbackBase>(callback);
}

void VectorAnimationTask::SetLoopCount(int32_t count)
{
  if(mLoopCount != count)
  {
    mLoopCount   = count;
    mCurrentLoop = 0;

    DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::SetLoopCount: [%d] [%p]\n", count, this);
  }
}

void VectorAnimationTask::SetPlayRange(const Property::Array& playRange)
{
  bool     valid      = false;
  uint32_t startFrame = 0, endFrame = 0;
  size_t   count = playRange.Count();

  if(count >= 2)
  {
    int32_t start = 0, end = 0;
    if(playRange.GetElementAt(0).Get(start) && playRange.GetElementAt(1).Get(end))
    {
      startFrame = static_cast<uint32_t>(start);
      endFrame   = static_cast<uint32_t>(end);
      valid      = true;
    }
    else
    {
      std::string startMarker, endMarker;
      if(playRange.GetElementAt(0).Get(startMarker) && playRange.GetElementAt(1).Get(endMarker))
      {
        if(mVectorRenderer)
        {
          uint32_t frame; // We don't use this later
          if(mVectorRenderer.GetMarkerInfo(startMarker, startFrame, frame) && mVectorRenderer.GetMarkerInfo(endMarker, frame, endFrame))
          {
            valid = true;
          }
        }
      }
    }
  }
  else if(count == 1)
  {
    std::string marker;
    if(playRange.GetElementAt(0).Get(marker))
    {
      if(mVectorRenderer && mVectorRenderer.GetMarkerInfo(marker, startFrame, endFrame))
      {
        valid = true;
      }
    }
  }

  if(!valid)
  {
    DALI_LOG_ERROR("VectorAnimationTask::SetPlayRange: Invalid range [%p]\n", this);
    return;
  }

  // Make sure the range specified is between 0 and the total frame number
  startFrame = std::min(startFrame, mTotalFrame - 1);
  endFrame   = std::min(endFrame, mTotalFrame - 1);

  // If the range is not in order swap values
  if(startFrame > endFrame)
  {
    uint32_t temp = startFrame;
    startFrame    = endFrame;
    endFrame      = temp;
  }

  if(startFrame != mStartFrame || endFrame != mEndFrame)
  {
    mStartFrame = startFrame;
    mEndFrame   = endFrame;

    // If the current frame is out of the range, change the current frame also.
    if(mStartFrame > mCurrentFrame)
    {
      mCurrentFrame = mStartFrame;

      if(mPlayState != PlayState::PLAYING)
      {
        // Ensure to render current frame.
        mNeedForceRenderOnceTrigger = true;
      }
    }
    else if(mEndFrame < mCurrentFrame)
    {
      mCurrentFrame = mEndFrame;

      if(mPlayState != PlayState::PLAYING)
      {
        // Ensure to render current frame.
        mNeedForceRenderOnceTrigger = true;
      }
    }

    DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::SetPlayRange: [%d, %d] [%s] [%p]\n", mStartFrame, mEndFrame, mImageUrl.GetUrl().c_str(), this);
  }
}

void VectorAnimationTask::GetPlayRange(uint32_t& startFrame, uint32_t& endFrame)
{
  startFrame = mStartFrame;
  endFrame   = mEndFrame;
}

void VectorAnimationTask::SetCurrentFrameNumber(uint32_t frameNumber)
{
  if(mCurrentFrame == frameNumber)
  {
    DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::SetCurrentFrameNumber: Set same frame [%d] [%p]\n", frameNumber, this);
    return;
  }

  if(frameNumber >= mStartFrame && frameNumber <= mEndFrame)
  {
    mCurrentFrame      = frameNumber;
    mUpdateFrameNumber = false;

    if(mPlayState != PlayState::PLAYING)
    {
      // Ensure to render current frame.
      mNeedForceRenderOnceTrigger = true;
    }

    DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::SetCurrentFrameNumber: frame number = %d [%p]\n", mCurrentFrame, this);
  }
  else
  {
    DALI_LOG_ERROR("Invalid frame number [%d (%d, %d)] [%p]\n", frameNumber, mStartFrame, mEndFrame, this);
  }
}

uint32_t VectorAnimationTask::GetCurrentFrameNumber() const
{
  return mCurrentFrame;
}

uint32_t VectorAnimationTask::GetTotalFrameNumber() const
{
  return mTotalFrame;
}

void VectorAnimationTask::GetDefaultSize(uint32_t& width, uint32_t& height) const
{
  mVectorRenderer.GetDefaultSize(width, height);
}

void VectorAnimationTask::SetStopBehavior(DevelImageVisual::StopBehavior::Type stopBehavior)
{
  mStopBehavior = stopBehavior;

  DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::SetStopBehavior: stop behavor = %d [%p]\n", mStopBehavior, this);
}

void VectorAnimationTask::SetLoopingMode(DevelImageVisual::LoopingMode::Type loopingMode)
{
  mLoopingMode = loopingMode;

  DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::SetLoopingMode: looping mode = %d [%p]\n", mLoopingMode, this);
}

void VectorAnimationTask::GetLayerInfo(Property::Map& map) const
{
  // Fast-out if file is loading, or load failed.
  if(mLoadFailed || IsLoadRequested())
  {
    return;
  }

  if(DALI_UNLIKELY(!mLayerInfoCached))
  {
    // Update only 1 time.
    mLayerInfoCached = true;
    mVectorRenderer.GetLayerInfo(mCachedLayerInfo);
  }

  map = mCachedLayerInfo;
}

void VectorAnimationTask::GetMarkerInfo(Property::Map& map) const
{
  // Fast-out if file is loading, or load failed.
  if(mLoadFailed || IsLoadRequested())
  {
    return;
  }

  if(DALI_UNLIKELY(!mMarkerInfoCached))
  {
    // Update only 1 time.
    mMarkerInfoCached = true;
    mVectorRenderer.GetMarkerInfo(mCachedMarkerInfo);
  }

  map = mCachedMarkerInfo;
}

VectorAnimationTask::ResourceReadySignalType& VectorAnimationTask::ResourceReadySignal()
{
  return mResourceReadySignal;
}

bool VectorAnimationTask::Rasterize()
{
  bool     stopped = false;
  uint32_t currentFrame;
  mKeepAnimation = false;

  {
    Mutex::ScopedLock lock(mMutex);
    if(mDestroyTask)
    {
      // The task will be destroyed. We don't need rasterization.
      return false;
    }
  }

  if(mLoadRequest)
  {
    return Load(false);
  }

  if(mLoadFailed)
  {
    return false;
  }

#ifdef TRACE_ENABLED
  uint64_t mStartTimeNanoSceonds = 0;
  uint64_t mEndTimeNanoSceonds   = 0;
#endif
  DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_LOTTIE_RASTERIZE_TASK", [&](std::ostringstream& oss)
                                          {
    mStartTimeNanoSceonds = GetNanoseconds();
    oss << "[s:" << mWidth << "x" << mHeight << " ";
    oss << "u:" << mImageUrl.GetEllipsedUrl() << "]"; });

  ApplyAnimationData();

  if(mPlayState == PlayState::PLAYING && mUpdateFrameNumber)
  {
    mCurrentFrame = mForward ? mCurrentFrame + mDroppedFrames + 1 : (mCurrentFrame > mDroppedFrames ? mCurrentFrame - mDroppedFrames - 1 : 0);
    Dali::ClampInPlace(mCurrentFrame, mStartFrame, mEndFrame);
  }

  currentFrame = mCurrentFrame;

  mUpdateFrameNumber = true;

  if(mPlayState == PlayState::STOPPING)
  {
    mCurrentFrame = GetStoppedFrame(mStartFrame, mEndFrame, mCurrentFrame);
    currentFrame  = mCurrentFrame;
    stopped       = true;
  }
  else if(mPlayState == PlayState::PLAYING)
  {
    bool animationFinished = false;

    if(currentFrame >= mEndFrame) // last frame
    {
      if(mLoopingMode == DevelImageVisual::LoopingMode::AUTO_REVERSE)
      {
        mForward = false;
      }
      else
      {
        if(mLoopCount < 0 || ++mCurrentLoop < mLoopCount) // repeat forever or before the last loop
        {
          mCurrentFrame      = mStartFrame;
          mUpdateFrameNumber = false;
        }
        else
        {
          animationFinished = true; // end of animation
        }
      }
    }
    else if(currentFrame == mStartFrame && !mForward) // first frame
    {
      if(mLoopCount < 0 || ++mCurrentLoop < mLoopCount) // repeat forever or before the last loop
      {
        mForward = true;
      }
      else
      {
        animationFinished = true; // end of animation
      }
    }

    if(animationFinished)
    {
      if(mStopBehavior == DevelImageVisual::StopBehavior::CURRENT_FRAME)
      {
        stopped = true;
      }
      else
      {
        mPlayState = PlayState::STOPPING;
      }
    }
  }

  // Rasterize
  bool renderSuccess = false;
  if(mVectorRenderer)
  {
    renderSuccess = mVectorRenderer.Render(currentFrame);
    if(!renderSuccess)
    {
      DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::Rasterize: Rendering failed. Try again later.[%d] [%p]\n", currentFrame, this);
      mUpdateFrameNumber = false;
    }
  }

  if(stopped && renderSuccess)
  {
    mPlayState   = PlayState::STOPPED;
    mForward     = true;
    mCurrentLoop = 0;

    mNeedForceRenderOnceTrigger = true;

    if(mVectorRenderer)
    {
      // Notify the Renderer that rendering is stopped.
      mVectorRenderer.RenderStopped();
    }

    // Animation is finished
    {
      Mutex::ScopedLock lock(mMutex);
      if(mNeedAnimationFinishedTrigger && mAnimationFinishedCallback)
      {
        mVectorAnimationThread.AddEventTriggerCallback(mAnimationFinishedCallback.get(), mAppliedPlayStateId);
      }
    }

    DALI_LOG_INFO(gVectorAnimationLogFilter, Debug::Verbose, "VectorAnimationTask::Rasterize: Animation is finished [current = %d] [%p]\n", currentFrame, this);
  }

  // Forcely trigger render once if need.
  if(renderSuccess && (mNotifyAfterRasterization || mNeedForceRenderOnceTrigger))
  {
    mVectorAnimationThread.RequestForceRenderOnce();
    mNeedForceRenderOnceTrigger = false;
  }

  if(mPlayState != PlayState::PAUSED && mPlayState != PlayState::STOPPED)
  {
    mKeepAnimation = true;
  }

  DALI_TRACE_END_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_LOTTIE_RASTERIZE_TASK", [&](std::ostringstream& oss)
                                        {
    mEndTimeNanoSceonds = GetNanoseconds();
    oss << std::fixed << std::setprecision(3);
    oss << "[";
    oss << "d:" << static_cast<float>(mEndTimeNanoSceonds - mStartTimeNanoSceonds) / 1000000.0f << "ms ";
    oss << "s:" << mWidth << "x" << mHeight << " ";
    oss << "f:" << mCurrentFrame;
    if(mDroppedFrames > 0)
    {
      oss << "(+" << mDroppedFrames << ")";
    }
    oss << " ";
    oss << "l:" << mCurrentLoop << " ";
    oss << "p:" << mPlayState << " ";
    oss << "r:" << renderSuccess << " ";
    oss << "s:" << stopped << " ";
    oss << "k:" << mKeepAnimation << " ";
    oss << "u:" << mImageUrl.GetEllipsedUrl() << "]"; });

  return true;
}

uint32_t VectorAnimationTask::GetStoppedFrame(uint32_t startFrame, uint32_t endFrame, uint32_t currentFrame)
{
  uint32_t frame = currentFrame;

  switch(mStopBehavior)
  {
    case DevelImageVisual::StopBehavior::FIRST_FRAME:
    {
      frame = startFrame;
      break;
    }
    case DevelImageVisual::StopBehavior::LAST_FRAME:
    {
      if(mLoopingMode == DevelImageVisual::LoopingMode::AUTO_REVERSE)
      {
        frame = startFrame;
      }
      else
      {
        frame = endFrame;
      }
      break;
    }
    case DevelImageVisual::StopBehavior::CURRENT_FRAME:
    {
      frame = currentFrame;
      break;
    }
  }

  return frame;
}

/// Event & VectorAnimationThread called after Rasterize() finished.
VectorAnimationTask::TimePoint VectorAnimationTask::CalculateNextFrameTime(bool renderNow)
{
  // std::chrono::time_point template has second parameter duration which defaults to the std::chrono::steady_clock supported
  // duration. In some C++11 implementations it is a milliseconds duration, so it fails to compile unless mNextFrameStartTime
  // is casted to use the default duration.
  auto current = std::chrono::steady_clock::now();

  if(renderNow)
  {
    mNextFrameStartTime = current;
    mDroppedFrames      = 0;
  }
  else
  {
    uint32_t   droppedFrames        = 0;
    const auto durationMicroSeconds = std::chrono::microseconds(mFrameDurationMicroSeconds);

    mNextFrameStartTime = std::chrono::time_point_cast<TimePoint::duration>(mNextFrameStartTime + durationMicroSeconds);
    if(mNextFrameStartTime < current)
    {
      while(current > std::chrono::time_point_cast<TimePoint::duration>(mNextFrameStartTime + durationMicroSeconds) && droppedFrames < mTotalFrame)
      {
        droppedFrames++;
        mNextFrameStartTime = std::chrono::time_point_cast<TimePoint::duration>(mNextFrameStartTime + durationMicroSeconds);
      }

      mNextFrameStartTime = current;
    }
    mDroppedFrames = droppedFrames;
  }

  return mNextFrameStartTime;
}

VectorAnimationTask::TimePoint VectorAnimationTask::GetNextFrameTime()
{
  return mNextFrameStartTime;
}

void VectorAnimationTask::ApplyAnimationData()
{
  uint32_t index;

  {
    Mutex::ScopedLock lock(mMutex);

    if(!mAnimationDataUpdated || mAnimationData[mAnimationDataIndex].size() != 0)
    {
      // Data is not updated or the previous data is not applied yet.
      return;
    }

    mAnimationDataIndex   = mAnimationDataIndex == 0 ? 1 : 0; // Swap index
    mAnimationDataUpdated = false;

    index = mAnimationDataIndex;
  }

  for(const auto& animationData : mAnimationData[index])
  {
    if(animationData.resendFlag & VectorAnimationTask::RESEND_LOOP_COUNT)
    {
      SetLoopCount(animationData.loopCount);
    }

    if(animationData.resendFlag & VectorAnimationTask::RESEND_PLAY_RANGE)
    {
      SetPlayRange(animationData.playRange);
    }

    if(animationData.resendFlag & VectorAnimationTask::RESEND_STOP_BEHAVIOR)
    {
      SetStopBehavior(animationData.stopBehavior);
    }

    if(animationData.resendFlag & VectorAnimationTask::RESEND_LOOPING_MODE)
    {
      SetLoopingMode(animationData.loopingMode);
    }

    if(animationData.resendFlag & VectorAnimationTask::RESEND_CURRENT_FRAME)
    {
      SetCurrentFrameNumber(animationData.currentFrame);
    }

    if(animationData.resendFlag & VectorAnimationTask::RESEND_NOTIFY_AFTER_RASTERIZATION)
    {
      mNotifyAfterRasterization = animationData.notifyAfterRasterization;
    }

    if(animationData.resendFlag & VectorAnimationTask::RESEND_FRAME_SPEED_FACTOR)
    {
      mFrameSpeedFactor = animationData.frameSpeedFactor;

      // Recalculate frame duration with new frame speed factor.
      mFrameDurationMicroSeconds = CalculateFrameDurationMicroSeconds(mFrameRate, mFrameSpeedFactor);
    }

    if(animationData.resendFlag & VectorAnimationTask::RESEND_NEED_RESOURCE_READY)
    {
      mVectorRenderer.InvalidateBuffer();
    }

    if(animationData.resendFlag & VectorAnimationTask::RESEND_DYNAMIC_PROPERTY)
    {
      for(auto&& iter : animationData.dynamicProperties)
      {
        mVectorRenderer.AddPropertyValueCallback(iter.keyPath, static_cast<VectorAnimationRenderer::VectorProperty>(iter.property), iter.callback, iter.id);
      }
    }

    if(animationData.resendFlag & VectorAnimationTask::RESEND_PLAY_STATE)
    {
      mAppliedPlayStateId = animationData.playStateId;
      if(animationData.playState == DevelImageVisual::PlayState::PLAYING)
      {
        PlayAnimation();
      }
      else if(animationData.playState == DevelImageVisual::PlayState::PAUSED)
      {
        PauseAnimation();
      }
      else if(animationData.playState == DevelImageVisual::PlayState::STOPPED)
      {
        StopAnimation();
      }
    }
  }

  // reset data list
  mAnimationData[index].clear();
}

void VectorAnimationTask::OnUploadCompleted()
{
  mResourceReadySignal.Emit(ResourceStatus::READY);
}

void VectorAnimationTask::OnLoadCompleted(uint32_t /* not used */)
{
  if(!mLoadFailed)
  {
    if(mEnableFrameCache && mSizeUpdated)
    {
      mVectorRenderer.KeepRasterizedBuffer();
      mSizeUpdated = false;
    }
    mResourceReadySignal.Emit(ResourceStatus::LOADED);
  }
  else
  {
    mResourceReadySignal.Emit(ResourceStatus::FAILED);
  }
}
} // namespace Internal

} // namespace Toolkit

} // namespace Dali

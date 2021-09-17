#ifndef DALI_TOOLKIT_INTERNAL_GL_VIEW_THREAD_H
#define DALI_TOOLKIT_INTERNAL_GL_VIEW_THREAD_H

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

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/native-image-source-queue.h>
#include <dali/devel-api/threading/conditional-wait.h>
#include <dali/devel-api/threading/semaphore.h>
#include <dali/devel-api/threading/thread.h>
#include <dali/integration-api/adaptor-framework/log-factory-interface.h>
#include <dali/integration-api/adaptor-framework/native-image-surface.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/signals/callback.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
/**
 * @brief GlViewRenderThread is a render thread for GlView.
 * This invokes user's callbacks.
 */
class GlViewRenderThread : public Dali::Thread
{
public:
  /**
   * Constructor
   *
   * @param[in] queue The NativeImageSourceQueue that GL renders onto
   */
  GlViewRenderThread(Dali::NativeImageSourceQueuePtr queue);

  /**
   * destructor.
   */
  virtual ~GlViewRenderThread();

  /**
   * @copydoc Dali::Toolkit::GlView::RegisterGlCallback()
   */
  void RegisterGlCallback(CallbackBase* initCallback, CallbackBase* renderFrameCallback, CallbackBase* terminateCallback);

  /**
   * @copydoc Dali::Toolkit::GlView::SetResizeCallback()
   */
  void SetResizeCallback(CallbackBase* resizeCallback);

  /**
   * @copydoc Dali::Toolkit::GlView::SetGraphicsConfig()
   */
  bool SetGraphicsConfig(bool depth, bool stencil, int msaa, int version);

  /**
   * Enable OnDemand Rendering Mode
   *
   * @param[in] onDemand the flag of OnDemand Rendering Mode. If the flag is true, the rendering mode is set OnDemand,
   * otherwise the flag is false, rendering mode is set continuous mode.
   */
  void SetOnDemandRenderMode(bool onDemand);

  /**
   * Sets the surface size
   *
   * @param[in] size the size of the NaitveImageSurface
   */
  void SetSurfaceSize(Dali::Vector2 size);

  /**
   * @copydoc Dali::Toolkit::RenderOnce()
   */
  void RenderOnce();

  /**
   * Pauses the render thread.
   */
  void Pause();

  /**
   * Resumes the render thread.
   */
  void Resume();

  /**
   * Stops the render thread.
   * @note Should only be called in Stop as calling this will kill the render thread.
   */
  void Stop();

  /**
   * Acquires the surface resource
   */
  void AcquireSurface();

  /**
   * Releases the surface resource
   */
  void ReleaseSurface();

protected:
  /**
   * The routine that the thread will execute once it is started.
   */
  void Run() override;

private:
  GlViewRenderThread(const GlViewRenderThread& obj) = delete;
  GlViewRenderThread operator=(const GlViewRenderThread& obj) = delete;

  /**
   * Called by the Render Thread which ensures a wait if required.
   *
   * @param[out] timeToSleepUntil  The time remaining in nanoseconds to keep the thread sleeping before resuming.
   * @return false, if the thread should stop.
   */
  bool RenderReady(uint64_t& timeToSleepUntil);

  /**
   * @brief Get the monotonic time since the clock's epoch.
   * @param[out]  timeInNanoseconds  The time in nanoseconds since the reference point.
   */
  void GetNanoSeconds(uint64_t& timeInNanoseconds);

  /**
   * Blocks the execution of the current thread until specified sleep_time
   * @param[in] timeInNanoseconds  The time blocking for
   */
  void SleepUntil(uint64_t timeInNanoseconds);

private:
  const Dali::LogFactoryInterface& mLogFactory;
  Dali::Vector2                    mSurfaceSize; ///< The size of mNativeImageQueue
  Dali::NativeImageSurfacePtr      mNativeImageSurface;
  Dali::NativeImageSourceQueuePtr  mNativeImageQueue;
  Semaphore<>                      mSurfaceSemaphore; ///< The semaphore to avoid race condition to the render target

  std::unique_ptr<CallbackBase> mGlInitCallback;
  std::unique_ptr<CallbackBase> mGlRenderFrameCallback;
  std::unique_ptr<CallbackBase> mGlTerminateCallback;
  std::unique_ptr<CallbackBase> mResizeCallback;

  bool mDepth : 1;
  bool mStencil : 1;
  int  mMSAA;
  int  mGraphicsApiVersion;

  Dali::ConditionalWait mConditionalWait;
  volatile unsigned int mIsThreadStarted;   ///< Whether this thread has been started.
  volatile unsigned int mIsThreadStopped;   ///< Stop render thread. It means this render thread will be destroyed.
  volatile unsigned int mIsThreadPaused;    ///< Sleep render thread by pause.
  volatile unsigned int mIsRenderRequested; ///< Request rendering once
  volatile unsigned int mRenderingMode;     ///< Rendering Mode, 0: Continuous, 1:OnDemand
  volatile unsigned int mIsSurfaceResized;  ///< Invoke ResizeCallback when NativeImageSurface is resized.

  uint64_t mDefaultFrameDurationNanoseconds; ///< Default duration of a frame (used for sleeping if not enough time elapsed). Not protected by lock, but written to rarely so not worth adding a lock when reading.
};

} // namespace Internal
} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_GL_SURFACE_VIEW_THREAD_H

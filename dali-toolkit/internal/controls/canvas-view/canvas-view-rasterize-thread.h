#ifndef DALI_TOOLKIT_CANVAS_VIEW_RASTERIZE_THREAD_H
#define DALI_TOOLKIT_CANVAS_VIEW_RASTERIZE_THREAD_H

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
 */

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/canvas-renderer.h>
#include <dali/devel-api/adaptor-framework/event-thread-callback.h>
#include <dali/devel-api/threading/conditional-wait.h>
#include <dali/devel-api/threading/mutex.h>
#include <dali/devel-api/threading/thread.h>
#include <dali/integration-api/adaptor-framework/log-factory-interface.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/rendering/texture-set.h>
#include <memory>
#include <vector>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/canvas-view/canvas-view-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
using CanvasViewPtr = IntrusivePtr<CanvasView>;
class CanvasRendererRasterizingTask;
using CanvasRendererRasterizingTaskPtr = IntrusivePtr<CanvasRendererRasterizingTask>;

/**
 * The canvasview rasterizing tasks to be processed in the worker thread.
 *
 * Life cycle of a rasterizing task is as follows:
 * 1. Created by CanvasView in the main thread
 * 2. Queued in the worked thread waiting to be processed.
 * 3. If this task gets its turn to do the rasterization, it triggers main thread to apply the rasterized image to material then been deleted in main thread call back.
 *    Or if this task is been removed before its turn to be processed, it then been deleted in the worker thread.
 */
class CanvasRendererRasterizingTask : public RefObject
{
public:
  /**
   * Constructor
   * @param[in] canvasRenderer The renderer which the rasterized canvas to be applied.
   */
  CanvasRendererRasterizingTask(CanvasView* canvasView, CanvasRenderer canvasRenderer);

  /**
   * Destructor.
   */
  ~CanvasRendererRasterizingTask() = default;

  /**
   * Do the rasterization with the mRasterizer.
   * @return Returns True when it's successful. False otherwise.
   */
  bool Rasterize();

  /**
   * Get the CanvasView
   * @return The CanvasView pointer.
   */
  CanvasView* GetCanvasView() const;

  /**
   * Get the rasterization result.
   * @return The pixel data with the rasterized pixels.
   */
  PixelData GetPixelData() const;

  /**
   * Get size of rasterization result.
   * @return The size of the pixel data.
   */
  Vector2 GetBufferSize() const;

private:
  // Undefined
  CanvasRendererRasterizingTask(const CanvasRendererRasterizingTask& task);

  // Undefined
  CanvasRendererRasterizingTask& operator=(const CanvasRendererRasterizingTask& task);

private:
  CanvasViewPtr  mCanvasView;
  CanvasRenderer mCanvasRenderer;
  PixelData      mPixelData;
  Vector2        mBufferSize;
};

/**
 * The worker thread for CanvasView rasterization.
 */
class CanvasViewRasterizeThread : public Thread, Integration::Processor
{
public:
  /// @brief ApplyRasterizedImage Event signal type
  using RasterizationCompletedSignalType = Signal<void(PixelData)>;

public:
  /**
   * Constructor.
   *
   * @param[in] trigger The trigger to wake up the main thread.
   */
  CanvasViewRasterizeThread();

  /**
   * Terminate the CanvasView rasterize thread, join and delete.
   *
   * @param[in] thread The rasterize thread.
   */
  static void TerminateThread(CanvasViewRasterizeThread*& thread);

  /**
   * Add a rasterization task into the waiting queue, called by main thread.
   *
   * @param[in] task The task added to the queue.
   */
  void AddTask(CanvasRendererRasterizingTaskPtr task);

  /**
   * Remove the task with the given CanvasView from the waiting queue, called by main thread.
   *
   * Typically called when the actor is put off stage, so the renderer is not needed anymore.
   *
   * @param[in] canvasView The CanvasView pointer.
   */
  void RemoveTask(CanvasView* canvasView);

  /**
   * Applies the rasterized image to material
   */
  void ApplyRasterized();

  /**
   * @copydoc Dali::Integration::Processor::Process()
   */
  void Process(bool postProcessor) override;

  /**
   * @brief This signal is emitted when rasterized image is applied.
   *
   * @return The signal to connect to
   */
  RasterizationCompletedSignalType& RasterizationCompletedSignal();

private:
  /**
   * Pop the next task out from the queue.
   *
   * @return The next task to be processed.
   */
  CanvasRendererRasterizingTaskPtr NextTaskToProcess();

  /**
   * Pop the next task out from the completed queue, called by main thread.
   *
   * @return The next task in the completed queue.
   */
  CanvasRendererRasterizingTaskPtr NextCompletedTask();

  /**
   * Add a task in to the queue
   *
   * @param[in] task The task added to the queue.
   */
  void AddCompletedTask(CanvasRendererRasterizingTaskPtr task);

  /**
   * @brief Unregister a previously registered processor
   *
   */
  void UnregisterProcessor();

protected:
  /**
   * Destructor.
   */
  ~CanvasViewRasterizeThread() override;

  /**
   * The entry function of the worker thread.
   * It fetches task from the Queue, rasterizes the image and apply to the renderer.
   */
  void Run() override;

private:
  // Undefined
  CanvasViewRasterizeThread(const CanvasViewRasterizeThread& thread);

  // Undefined
  CanvasViewRasterizeThread& operator=(const CanvasViewRasterizeThread& thread);

private:
  std::vector<CanvasRendererRasterizingTaskPtr> mRasterizeTasks; //The queue of the tasks waiting to rasterize the CanvasView.
  std::vector<CanvasRendererRasterizingTaskPtr> mCompletedTasks; //The queue of the tasks with the SVG rasterization completed

  ConditionalWait                      mConditionalWait;
  Dali::Mutex                          mMutex;
  std::unique_ptr<EventThreadCallback> mTrigger;
  const Dali::LogFactoryInterface&     mLogFactory;
  bool                                 mProcessorRegistered;
  RasterizationCompletedSignalType     mRasterizationCompletedSignal;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_CANVAS_VIEW_RASTERIZE_THREAD_H

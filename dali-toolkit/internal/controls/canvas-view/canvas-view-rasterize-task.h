#ifndef DALI_TOOLKIT_CANVAS_VIEW_RASTERIZE_TASK_H
#define DALI_TOOLKIT_CANVAS_VIEW_RASTERIZE_TASK_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/canvas-renderer/canvas-renderer.h>
#include <dali/devel-api/adaptor-framework/event-thread-callback.h>
#include <dali/devel-api/threading/conditional-wait.h>
#include <dali/devel-api/threading/mutex.h>
#include <dali/devel-api/threading/thread.h>
#include <dali/integration-api/adaptor-framework/log-factory-interface.h>
#include <dali/public-api/adaptor-framework/async-task-manager.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/rendering/texture-set.h>
#include <memory>
#include <vector>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
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
class CanvasRendererRasterizingTask : public AsyncTask
{
public:
  /**
   * Constructor
   * @param[in] canvasRenderer The renderer which the rasterized canvas to be applied.
   * @param[in] callback The callback that is called when the operation is completed.
   */
  CanvasRendererRasterizingTask(CanvasRenderer canvasRenderer, CallbackBase* callback);

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
   * Whether the rasterize is completed
   * @return Returns True when it's completed
   */
  bool IsRasterized();

  /**
   * Get the rasterization result.
   * @return The texture with the rasterized pixels.
   */
  Texture GetRasterizedTexture();

public: // Implementation of AsyncTask
  /**
   * @copydoc Dali::AsyncTask::Process()
   */
  void Process() override;

  /**
   * @copydoc Dali::AsyncTask::IsReady()
   */
  bool IsReady() override;

  /**
   * @copydoc Dali::AsyncTask::GetTaskName()
   */
  std::string_view GetTaskName() const override
  {
    return "CanvasRendererRasterizingTask";
  }

private:
  // Undefined
  CanvasRendererRasterizingTask(const CanvasRendererRasterizingTask& task);

  // Undefined
  CanvasRendererRasterizingTask& operator=(const CanvasRendererRasterizingTask& task);

private:
  CanvasRenderer mCanvasRenderer;
  bool           mRasterizedSuccessed;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_CANVAS_VIEW_RASTERIZE_TASK_H

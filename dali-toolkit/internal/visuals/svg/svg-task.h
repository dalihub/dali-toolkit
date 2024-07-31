#ifndef DALI_TOOLKIT_SVG_TASK_H
#define DALI_TOOLKIT_SVG_TASK_H

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
 */

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/vector-image-renderer.h>
#include <dali/public-api/adaptor-framework/encoded-image-buffer.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/images/pixel-data.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/visual-url.h>
#include <dali/public-api/adaptor-framework/async-task-manager.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class SvgVisual;
typedef IntrusivePtr<SvgVisual> SvgVisualPtr;
class SvgTask;
typedef IntrusivePtr<SvgTask> SvgTaskPtr;

/**
 * The svg rasterizing tasks to be processed in the worker thread.
 *
 * Life cycle of a rasterizing task is as follows:
 * 1. Created by SvgVisual in the main thread
 * 2. Queued in the worked thread waiting to be processed.
 * 3. If this task gets its turn to do the rasterization, it triggers main thread to apply the rasterized image to material then been deleted in main thread call back
 *    Or if this task is been removed ( new image/size set to the visual or actor off stage) before its turn to be processed, it then been deleted in the worker thread.
 */
class SvgTask : public AsyncTask
{
public:
  /**
   * Constructor
   * @param[in] vectorRenderer The vector rasterizer.
   * @param[in] id The id of this task.
   * @param[in] callback The callback that is called when the operation is completed.
   * @param[in] priorityType The priority of this task.
   */
  SvgTask(VectorImageRenderer vectorRenderer, int32_t id, CallbackBase* callback, AsyncTask::PriorityType priorityType = AsyncTask::PriorityType::DEFAULT);

  /**
   * Destructor.
   */
  virtual ~SvgTask() = default;

  /**
   * Whether the task has succeeded.
   * @return True if the task has succeeded.
   */
  bool HasSucceeded() const;

  /**
   * Get the id of task.
   * @return The Id of task what we added from constructor.
   */
  int32_t GetId() const
  {
    return mId;
  }

  /**
   * @brief Get the task's imageRenderer
   * @return VectorImageRenderer
   */
  VectorImageRenderer GetRenderer();

  /**
   * Get the rasterization result.
   * @return The pixel data with the rasterized pixels.
   */
  virtual PixelData GetPixelData() const;

private:
  // Undefined
  SvgTask(const SvgTask& task) = delete;

  // Undefined
  SvgTask& operator=(const SvgTask& task) = delete;

protected:
  VectorImageRenderer mVectorRenderer;
  const int32_t       mId;
  bool                mHasSucceeded;
};

class SvgLoadingTask : public SvgTask
{
public:
  /**
   * Constructor
   * @param[in] vectorRenderer The vector rasterizer.
   * @param[in] id The id of this task.
   * @param[in] url The URL to svg resource to use.
   * @param[in] encodedImageBuffer The resource buffer if required.
   * @param[in] dpi The DPI of the screen.
   * @param[in] callback The callback that is called when the operation is completed.
   */
  SvgLoadingTask(VectorImageRenderer vectorRenderer, int32_t id, const VisualUrl& url, EncodedImageBuffer encodedImageBuffer, float dpi, CallbackBase* callback);

  /**
   * Destructor.
   */
  ~SvgLoadingTask() override;

public: // Implementation of AsyncTask
  /**
   * @copydoc Dali::AsyncTask::Process()
   */
  void Process();

  /**
   * @copydoc Dali::AsyncTask::IsReady()
   */
  bool IsReady();

  /**
   * @copydoc Dali::AsyncTask::GetTaskName()
   */
  std::string_view GetTaskName() const override
  {
    return "SvgLoadingTask";
  }

private:
  // Undefined
  SvgLoadingTask(const SvgLoadingTask& task) = delete;

  // Undefined
  SvgLoadingTask& operator=(const SvgLoadingTask& task) = delete;

private:
  VisualUrl          mImageUrl;
  EncodedImageBuffer mEncodedImageBuffer;
  float              mDpi;
};

class SvgRasterizingTask : public SvgTask
{
public:
  /**
   * Constructor
   * @param[in] vectorRenderer The vector rasterizer.
   * @param[in] id The id of this task.
   * @param[in] width The rasterization width.
   * @param[in] height The rasterization height.
   * @param[in] callback The callback that is called when the operation is completed.
   */
  SvgRasterizingTask(VectorImageRenderer vectorRenderer, int32_t id, uint32_t width, uint32_t height, CallbackBase* callback);

  /**
   * Destructor.
   */
  ~SvgRasterizingTask() override;

  /**
   * Get the rasterization result.
   * @return The pixel data with the rasterized pixels.
   */
  PixelData GetPixelData() const override;

#ifdef TRACE_ENABLED
  /**
   * Set the url of rasterizatoin visual. Only for tracing
   * @param[in] url The url of this visual
   */
  void SetUrl(VisualUrl url)
  {
    mImageUrl = std::move(url);
  }
#endif

public: // Implementation of AsyncTask
  /**
   * @copydoc Dali::AsyncTask::Process()
   */
  void Process();

  /**
   * @copydoc Dali::AsyncTask::IsReady()
   */
  bool IsReady();

  /**
   * @copydoc Dali::AsyncTask::GetTaskName()
   */
  std::string_view GetTaskName() const override
  {
    return "SvgRasterizingTask";
  }

private:
  // Undefined
  SvgRasterizingTask(const SvgRasterizingTask& task) = delete;

  // Undefined
  SvgRasterizingTask& operator=(const SvgRasterizingTask& task) = delete;

private:
#ifdef TRACE_ENABLED
  VisualUrl mImageUrl{};
#endif
  PixelData mPixelData;
  uint32_t  mWidth;
  uint32_t  mHeight;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_SVG_TASK_H

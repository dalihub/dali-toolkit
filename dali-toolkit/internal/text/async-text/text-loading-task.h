#ifndef DALI_TOOLKIT_TEXT_LOADING_TASK_H
#define DALI_TOOLKIT_TEXT_LOADING_TASK_H

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
#include <dali/devel-api/adaptor-framework/event-thread-callback.h>
#include <dali/devel-api/threading/mutex.h>
#include <dali/public-api/adaptor-framework/async-task-manager.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/async-text/async-text-loader.h>


namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class TextLoadingTask;
using TextLoadingTaskPtr = IntrusivePtr<TextLoadingTask>;

/**
 * The task of loading text(update, layout and render).
 */
class TextLoadingTask : public AsyncTask
{
public:
  /**
   * Constructor.
   * @param [in] id The task id assigned from async text manager.
   * @param [in] paramaters The async text parameters.
   * @param [in] callback The callback that is called when the operation is completed.
   */
  TextLoadingTask(const uint32_t            id,
                  Text::AsyncTextParameters paramaters,
                  CallbackBase*             callback);

  /**
   * Constructor, empty task for wake up the async task manger.
   * @param [in] id The task id assigned from async text manager.
   * @param [in] callback The callback that is called when the operation is completed.
   */
  TextLoadingTask(const uint32_t id,
                  CallbackBase*  callback);

  /**
   * Destructor.
   */
  ~TextLoadingTask() override;

  /**
   * Returns the id assigned when creating a task.
   * @return The task id.
   */
  uint32_t GetId();

  /**
   * Set async text loader to process.
   * The task becomes ready to process.
   * @param [in] loader The async text loader, a loader can only process one task at a time.
   * @return The task id.
   */
  void SetLoader(Text::AsyncTextLoader& loader);

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
    return "TextLoadingTask";
  }

private:
  // Undefined
  TextLoadingTask(const TextLoadingTask& queue);

  // Undefined
  TextLoadingTask& operator=(const TextLoadingTask& queue);

  /**
   * Load the text
   */
  void Load();

public:
  uint32_t                  mId;
  Text::AsyncTextLoader     mLoader;
  Text::AsyncTextParameters mParameters;
  Text::AsyncTextRenderInfo mRenderInfo;

private:
  bool            mIsReady : 1; ///< Whether this task ready to run
  Mutex           mMutex;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_LOADING_TASK_H

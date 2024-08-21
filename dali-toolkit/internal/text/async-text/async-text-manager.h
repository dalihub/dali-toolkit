#ifndef DALI_TOOLKIT_TEXT_ASYNC_TEXT_MANAGER_H
#define DALI_TOOLKIT_TEXT_ASYNC_TEXT_MANAGER_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/async-text/async-text-loader.h>
#include <dali-toolkit/internal/text/async-text/text-load-observer.h>
#include <dali-toolkit/internal/text/async-text/text-loading-task.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal DALI_INTERNAL
{
class AsyncTextManager;

} // namespace DALI_INTERNAL

/**
 * AsyncTextManager manages an async text loader and handles async text load requests for a text visual.
 */
class AsyncTextManager : public BaseHandle
{
public:
  /**
   * @brief Create an uninitialized AsyncTextManager handle.
   */
  AsyncTextManager();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~AsyncTextManager();

  /**
   * @brief This constructor is used by AsyncTextManager::Get().
   *
   * @param[in] implementation A pointer to the internal async text manager object.
   */
  explicit DALI_INTERNAL AsyncTextManager(Internal::AsyncTextManager* implementation);

  /**
   * @brief Retrieve a handle to the AsyncTextManager instance.
   *
   * @return A handle to the AsyncTextManager.
   */
  static AsyncTextManager Get();

  /**
   * @brief Make a request to asynchronously load text.
   *
   * TextLoadingTask is created and registered with the AsyncTaskManager.
   * The result is received through the AsyncTextManager::LoadComplete.
   *
   * @param[in] parameters A parameters to load text.
   * @param[in] observer An observer to notify load complete.
   *
   * @return A requested task id.
   */
  uint32_t RequestLoad(AsyncTextParameters& parameters, TextLoadObserver* observer);

  /**
   * @brief Request to cancel the task.
   *
   * Remove tasks from the waiting and running queues.
   * Observer's LoadComplete will not be called in the running queue.
   *
   * @param[in] taskId A task Id to be canceled.
   */
  void RequestCancel(uint32_t taskId);

public:
  // Default copy and move operator
  AsyncTextManager(const AsyncTextManager& rhs) = default;
  AsyncTextManager(AsyncTextManager&& rhs)      = default;
  AsyncTextManager& operator=(const AsyncTextManager& rhs) = default;
  AsyncTextManager& operator=(AsyncTextManager&& rhs) = default;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_ASYNC_TEXT_MANAGER_H

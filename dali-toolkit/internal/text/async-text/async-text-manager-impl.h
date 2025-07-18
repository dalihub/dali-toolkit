#ifndef DALI_TOOLKIT_TEXT_ASYNC_TEXT_MANAGER_IMPL_H
#define DALI_TOOLKIT_TEXT_ASYNC_TEXT_MANAGER_IMPL_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/base-object.h>
#include <map>
#include <queue>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/async-text/async-text-manager.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal
{
/**
 * @brief Async text manager implementation. @see Text::AsyncTextManager.
 */
class AsyncTextManager : public BaseObject, public ConnectionTracker
{
public:
  /**
   * Constructor
   */
  AsyncTextManager();

  /**
   * Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~AsyncTextManager();

  /**
   * @copydoc Dali::AsyncTextManager::Get()
   */
  static Text::AsyncTextManager Get();

  /**
   * @brief Callback function for when the locale is changed.
   * @param[in] locale The new system locale.
   */
  void OnLocaleChanged(std::string locale);

  /**
   * @copydoc Dali::AsyncTextManager::RequestLoad()
   */
  uint32_t RequestLoad(AsyncTextParameters& parameters, TextLoadObserver* observer);

  /**
   * @copydoc Dali::AsyncTextManager::RequestCancel()
   */
  void RequestCancel(uint32_t taskId);

  /**
   * @copydoc Dali::AsyncTextManager::LoadComplete()
   */
  void LoadComplete(Toolkit::Internal::TextLoadingTaskPtr task);

private:
  /**
   * Structure to hold info about a text load during NotifyObservers.
   */
  struct LoadElement
  {
    LoadElement()
    : mTask(),
      mObserver(nullptr),
      mParameters()
    {
    }

    LoadElement(Toolkit::Internal::TextLoadingTaskPtr task, TextLoadObserver* observer, const AsyncTextParameters& parameters)
    : mTask(task),
      mObserver(observer),
      mParameters(parameters)
    {
    }

    Toolkit::Internal::TextLoadingTaskPtr mTask;       ///< Task.
    TextLoadObserver*                     mObserver;   ///< Observer of text load.
    AsyncTextParameters                   mParameters; ///< Text parameters to load.
  };

  /**
   * Whether there is an available loader.
   */
  bool IsAvailableLoader();

  /**
   * Returns available loader.
   */
  Text::AsyncTextLoader GetAvailableLoader();

  /**
   * This is called by the AsyncTextManager when an observer is destroyed.
   * We use the callback to know when to remove an observer from our notify list.
   * @param[in] observer The observer that generated the callback.
   */
  void ObserverDestroyed(TextLoadObserver* observer);

private:
  std::string mLocale; ///< System locale.
  uint32_t    mTaskId; ///< Id for managing the requested task.

  std::vector<Text::AsyncTextLoader> mAvailableLoaders; ///< List of available async text loader.
  std::vector<Text::AsyncTextLoader> mRunningLoaders;   ///< List of running async text loader.
  std::map<uint32_t, LoadElement>    mWaitingTasks;     ///< Waiting tasks, key is task id.
  std::map<uint32_t, LoadElement>    mRunningTasks;     ///< Running tasks, key is task id.
};

} // namespace Internal

inline static Internal::AsyncTextManager& GetImplementation(AsyncTextManager& asyncTextManager)
{
  DALI_ASSERT_ALWAYS(asyncTextManager && "async text manager handle is empty");
  BaseObject& handle = asyncTextManager.GetBaseObject();
  return static_cast<Internal::AsyncTextManager&>(handle);
}

inline static const Internal::AsyncTextManager& GetImplementation(const AsyncTextManager& asyncTextManager)
{
  DALI_ASSERT_ALWAYS(asyncTextManager && "async text manager handle is empty");
  const BaseObject& handle = asyncTextManager.GetBaseObject();
  return static_cast<const Internal::AsyncTextManager&>(handle);
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_ASYNC_TEXT_MANAGER_IMPL_H

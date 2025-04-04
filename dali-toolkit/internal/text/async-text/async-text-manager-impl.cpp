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
#include <dali-toolkit/internal/text/async-text/async-text-manager-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/environment-variable.h>
#include <dali/devel-api/common/singleton-service.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>

// INTERNAL INCLUDES

namespace Dali
{
namespace Toolkit
{
namespace
{
const char*    DALI_TEXT_NUMBER_OF_ASYNC_TEXT_LOADER("DALI_TEXT_NUMBER_OF_ASYNC_TEXT_LOADER");
const int      DEFAULT_NUMBER_OF_LOADER = 4;
const int      MINIMUM_NUMBER_OF_LOADER = 1;
const int      MAXIMUM_NUMBER_OF_LOADER = 16;
const uint32_t EMPTY_TASK_ID            = 0u;

DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_TEXT_ASYNC, false);
} // namespace

namespace Text
{
namespace Internal
{
AsyncTextManager::AsyncTextManager()
: mLocale(),
  mTaskId(0u),
  mAvailableLoaders(),
  mRunningLoaders(),
  mWaitingTasks(),
  mRunningTasks()
{
  // Check environment variable for DALI_TEXT_NUMBER_OF_ASYNC_TEXT_LOADER
  auto numberOfLoaderString = Dali::EnvironmentVariable::GetEnvironmentVariable(DALI_TEXT_NUMBER_OF_ASYNC_TEXT_LOADER);
  int  numberOfLoader       = numberOfLoaderString ? std::atoi(numberOfLoaderString) : DEFAULT_NUMBER_OF_LOADER;

  numberOfLoader = std::clamp(numberOfLoader, MINIMUM_NUMBER_OF_LOADER, MAXIMUM_NUMBER_OF_LOADER);
  DALI_LOG_RELEASE_INFO("Number of async text loaders:%d\n", numberOfLoader);

  mLocale = TextAbstraction::GetLocaleFull();

  if(DALI_LIKELY(Dali::Adaptor::IsAvailable()))
  {
    const TextAbstraction::FontPathList& customFonts = TextAbstraction::FontClient::Get().GetCustomFontDirectories();
    for(int i = 0; i < numberOfLoader; i++)
    {
      Text::AsyncTextLoader loader = Text::AsyncTextLoader::New();
      loader.SetCustomFontDirectories(customFonts);
      mAvailableLoaders.push_back(loader);
    }

    Dali::Adaptor::Get().LocaleChangedSignal().Connect(this, &AsyncTextManager::OnLocaleChanged);
  }
}

AsyncTextManager::~AsyncTextManager()
{
}

Text::AsyncTextManager AsyncTextManager::Get()
{
  Text::AsyncTextManager asyncTextManagerHandle;

  SingletonService service(SingletonService::Get());
  if(service)
  {
    // Check whether the singleton is already created
    Dali::BaseHandle handle = service.GetSingleton(typeid(Text::AsyncTextManager));
    if(handle)
    {
      // If so, downcast the handle
      AsyncTextManager* impl = dynamic_cast<Internal::AsyncTextManager*>(handle.GetObjectPtr());
      asyncTextManagerHandle = Text::AsyncTextManager(impl);
    }
    else // create and register the object
    {
      asyncTextManagerHandle = Text::AsyncTextManager(new AsyncTextManager);
      service.Register(typeid(asyncTextManagerHandle), asyncTextManagerHandle);
    }
  }

  return asyncTextManagerHandle;
}

void AsyncTextManager::OnLocaleChanged(std::string locale)
{
  if(mLocale != locale)
  {
    mLocale = locale;

    for(auto& loader : mAvailableLoaders)
    {
      loader.ClearModule();
      loader.SetLocale(mLocale);
    }

    // When the Loader is in running state, just set the flag and clear it when it becomes available.
    for(auto& loader : mRunningLoaders)
    {
      loader.SetModuleClearNeeded(true);
      loader.SetLocaleUpdateNeeded(true);
    }
  }
}

bool AsyncTextManager::IsAvailableLoader()
{
  return mAvailableLoaders.size() > 0u;
}

Text::AsyncTextLoader AsyncTextManager::GetAvailableLoader()
{
  Text::AsyncTextLoader loader = mAvailableLoaders.back();
  if(loader.IsModuleClearNeeded())
  {
    loader.ClearModule();
    loader.SetModuleClearNeeded(false);
  }
  if(loader.IsLocaleUpdateNeeded())
  {
    loader.SetLocale(mLocale);
    loader.SetLocaleUpdateNeeded(false);
  }

  mAvailableLoaders.pop_back();
  mRunningLoaders.push_back(loader);
  return loader;
}

uint32_t AsyncTextManager::RequestLoad(AsyncTextParameters& parameters, TextLoadObserver* observer)
{
  // Each task must have its own unique id.
  mTaskId++;

  auto task = new Dali::Toolkit::Internal::TextLoadingTask(mTaskId, parameters, MakeCallback(this, &AsyncTextManager::LoadComplete));

  LoadElement element(task, observer, parameters);

  if(observer)
  {
#ifdef TRACE_ENABLED
    if(gTraceFilter && gTraceFilter->IsTraceEnabled())
    {
      DALI_LOG_RELEASE_INFO("RequestLoad -> connect DestructionSignal to observer : %p, task : %u\n", observer, mTaskId);
    }
#endif
    observer->ConnectDestructionSignal();
    observer->DestructionSignal().Connect(this, &AsyncTextManager::ObserverDestroyed);
  }

  if(IsAvailableLoader())
  {
    // Add element to running map.
    mRunningTasks[mTaskId] = element;

    // Loader move available list -> running list.
    Text::AsyncTextLoader loader = GetAvailableLoader();
    task->SetLoader(loader);
#ifdef TRACE_ENABLED
    if(gTraceFilter && gTraceFilter->IsTraceEnabled())
    {
      DALI_LOG_RELEASE_INFO("RequestLoad -> ob : %p, add task : %u\n", element.mObserver, mTaskId);
    }
#endif
  }
  else
  {
    // There is no available loader, add element to waiting queue.
    mWaitingTasks[mTaskId] = element;
#ifdef TRACE_ENABLED
    if(gTraceFilter && gTraceFilter->IsTraceEnabled())
    {
      DALI_LOG_RELEASE_INFO("RequestLoad -> ob : %p, add waiting task : %u\n", element.mObserver, mTaskId);
    }
#endif
  }

  Dali::AsyncTaskManager::Get().AddTask(task);

  return mTaskId;
}

void AsyncTextManager::RequestCancel(uint32_t taskId)
{
  auto it = mWaitingTasks.find(taskId);
  if(it != mWaitingTasks.end())
  {
    if(it->second.mObserver)
    {
#ifdef TRACE_ENABLED
      if(gTraceFilter && gTraceFilter->IsTraceEnabled())
      {
        DALI_LOG_RELEASE_INFO("RequestCancel -> ob : %p, remove wating task : %u\n", it->second.mObserver, taskId);
      }
#endif
      if(it->second.mObserver->DisconnectDestructionSignal())
      {
        it->second.mObserver->DestructionSignal().Disconnect(this, &AsyncTextManager::ObserverDestroyed);
      }
    }
    Dali::AsyncTaskManager::Get().RemoveTask(it->second.mTask);
    mWaitingTasks.erase(it);
    return;
  }
  else
  {
    auto it = mRunningTasks.find(taskId);
    if(it != mRunningTasks.end())
    {
      if(it->second.mObserver)
      {
#ifdef TRACE_ENABLED
        if(gTraceFilter && gTraceFilter->IsTraceEnabled())
        {
          DALI_LOG_RELEASE_INFO("RequestCancel -> ob : %p, remove running task : %u\n", it->second.mObserver, taskId);
        }
#endif
        if(it->second.mObserver->DisconnectDestructionSignal())
        {
          it->second.mObserver->DestructionSignal().Disconnect(this, &AsyncTextManager::ObserverDestroyed);
        }
      }
      mRunningTasks.erase(it);
      return;
    }
  }

  DALI_LOG_ERROR("There is no task in the Waiting queue and Running queue : %u\n", taskId);
}

void AsyncTextManager::LoadComplete(Toolkit::Internal::TextLoadingTaskPtr task)
{
  uint32_t taskId = task->GetId();

  if(taskId == EMPTY_TASK_ID)
  {
    // Ignore callback from empty task.
    return;
  }

  auto it = mRunningTasks.find(taskId);
  if(it != mRunningTasks.end())
  {
    // Find task, execute load complete.
    if(it->second.mObserver)
    {
#ifdef TRACE_ENABLED
      if(gTraceFilter && gTraceFilter->IsTraceEnabled())
      {
        DALI_LOG_RELEASE_INFO("LoadComplete -> ob : %p, remove task : %u\n", it->second.mObserver, taskId);
      }
#endif
      // TODO : If it fails for any reason, false should be sent.
      bool success = true;
      if(it->second.mObserver->DisconnectDestructionSignal())
      {
        it->second.mObserver->DestructionSignal().Disconnect(this, &AsyncTextManager::ObserverDestroyed);
      }
      it->second.mObserver->LoadComplete(success, TextLoadObserver::TextInformation(task->mRenderInfo, task->mParameters));
    }
    else
    {
#ifdef TRACE_ENABLED
      if(gTraceFilter && gTraceFilter->IsTraceEnabled())
      {
        DALI_LOG_RELEASE_INFO("LoadComplete -> observer destroyed -> remove task : %u\n", taskId);
      }
#endif
    }
    // If a task has completed, we souhld to remove the element from running map.
    mRunningTasks.erase(it);
  }
  else
  {
    DALI_LOG_DEBUG_INFO("LoadComplete -> Running task already removed! : %u\n", taskId);
  }

  for(auto iter = mRunningLoaders.begin(); iter != mRunningLoaders.end(); ++iter)
  {
    if((*iter) == task->mLoader)
    {
      // Since the task has completed, move the loader to the available list.
      mRunningLoaders.erase(iter);
      mAvailableLoaders.push_back(task->mLoader);
      break;
    }
  }

  if(!mWaitingTasks.empty() && IsAvailableLoader())
  {
    // Loader move available list -> running list.
    Text::AsyncTextLoader loader = GetAvailableLoader();

    // Takes out the oldest waiting queue.
    auto        item    = mWaitingTasks.begin();
    LoadElement element = item->second;
    mWaitingTasks.erase(item);

    if(element.mObserver)
    {
      // Puts it into the running map.
      uint32_t watingTaskId       = element.mTask->GetId();
      mRunningTasks[watingTaskId] = element;

      // Set loader and ready to process.
      element.mTask->SetLoader(loader);

      // TODO : AsyncTaskManager does not know that the IsReady has changed after the task is added.
      // Wake up by adding an empty task to AsyncTaskManager.
      WakeUpAsyncTaskManager();
#ifdef TRACE_ENABLED
      if(gTraceFilter && gTraceFilter->IsTraceEnabled())
      {
        DALI_LOG_RELEASE_INFO("LoadComplete task : %u -> new task -> ob : %p, add task : %u\n", taskId, element.mObserver, watingTaskId);
      }
#endif
    }
  }

#ifdef TRACE_ENABLED
  if(gTraceFilter && gTraceFilter->IsTraceEnabled())
  {
    DALI_LOG_RELEASE_INFO("LoadComplete -> available loaders : %lu, running loaders : %lu, waiting tasks : %lu\n", mAvailableLoaders.size(), mRunningLoaders.size(), mWaitingTasks.size());
  }
#endif
}

void AsyncTextManager::ObserverDestroyed(TextLoadObserver* observer)
{
#ifdef TRACE_ENABLED
  if(gTraceFilter && gTraceFilter->IsTraceEnabled())
  {
    DALI_LOG_RELEASE_INFO("ObserverDestroyed observer : %p\n", observer);
  }
#endif
  for(auto it = mRunningTasks.begin(); it != mRunningTasks.end();)
  {
    if(it->second.mObserver == observer)
    {
      it->second.mObserver = nullptr;
      it                   = mRunningTasks.erase(it);
    }
    else
    {
      ++it;
    }
  }

  for(auto it = mWaitingTasks.begin(); it != mWaitingTasks.end();)
  {
    if(it->second.mObserver == observer)
    {
      Dali::AsyncTaskManager::Get().RemoveTask(it->second.mTask);
      it->second.mObserver = nullptr;
      it                   = mWaitingTasks.erase(it);
    }
    else
    {
      ++it;
    }
  }
}

void AsyncTextManager::WakeUpAsyncTaskManager()
{
  auto emptyTask = new Dali::Toolkit::Internal::TextLoadingTask(EMPTY_TASK_ID, MakeCallback(this, &AsyncTextManager::LoadComplete));
  Dali::AsyncTaskManager::Get().AddTask(emptyTask);
}

} // namespace Internal

} // namespace Text

} // namespace Toolkit

} // namespace Dali

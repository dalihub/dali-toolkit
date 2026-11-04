/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/math/math-utils.h>

#include <mutex>

// INTERNAL INCLUDES

namespace DALI_NAMESPACE
{
namespace Toolkit
{
namespace
{
static constexpr Dali::TypeInfoId gSingletonTypeInfoId = DALI_TYPE_ID(Text::AsyncTextManager);

const char*    DALI_TEXT_NUMBER_OF_ASYNC_TEXT_LOADER("DALI_TEXT_NUMBER_OF_ASYNC_TEXT_LOADER");
const int      DEFAULT_NUMBER_OF_LOADER = 4;
const int      MINIMUM_NUMBER_OF_LOADER = 1;
const int      MAXIMUM_NUMBER_OF_LOADER = 16;
const uint32_t EMPTY_TASK_ID            = 0u;
} // namespace

namespace Text
{
namespace Internal
{
namespace
{
std::mutex                                       gStaticAsyncTextManagerMutex; ///< Mutex for AsyncTextManager
Dali::Toolkit::Text::Internal::AsyncTextManager* gAsyncTextManager = nullptr;  ///< Must be used under gStaticAsyncTextManagerMutex
} // namespace
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

  numberOfLoader = Dali::Clamp(numberOfLoader, MINIMUM_NUMBER_OF_LOADER, MAXIMUM_NUMBER_OF_LOADER);
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

    TextAbstraction::FontClient::Get().CustomFontAddedSignal().Connect(this, &AsyncTextManager::OnCustomFontAdded);
    Dali::Adaptor::Get().LocaleChangedSignal().Connect(this, &AsyncTextManager::OnLocaleChanged);
  }

  std::unique_lock<std::mutex> lock(gStaticAsyncTextManagerMutex);
  gAsyncTextManager = this;
}

AsyncTextManager::~AsyncTextManager()
{
  {
    std::unique_lock<std::mutex> lock(gStaticAsyncTextManagerMutex);
    if(gAsyncTextManager == this)
    {
      gAsyncTextManager = nullptr;
    }
  }
  {
    Mutex::ScopedLock lock(mTasksMutex);
    {
      Mutex::ScopedLock lock(mLoaderMutex);
      mAvailableLoaders.clear();
      mLocaleChangedLoaders.clear();
      mRunningLoaders.clear();
    }
    mWaitingTasks.clear();
    mRunningTasks.clear();
  }
}

Text::AsyncTextManager AsyncTextManager::Get()
{
  Text::AsyncTextManager asyncTextManagerHandle;

  SingletonService service(SingletonService::Get());
  if(service)
  {
    // Check whether the singleton is already created
    Dali::BaseHandle handle = service.GetSingleton(gSingletonTypeInfoId);
    if(handle)
    {
      // If so, downcast the handle
      AsyncTextManager* impl = static_cast<Internal::AsyncTextManager*>(handle.GetObjectPtr());
      asyncTextManagerHandle = Text::AsyncTextManager(impl);
    }
    else // create and register the object
    {
      asyncTextManagerHandle = Text::AsyncTextManager(new AsyncTextManager);
      service.Register(gSingletonTypeInfoId, asyncTextManagerHandle);
    }
  }

  return asyncTextManagerHandle;
}

void AsyncTextManager::OnCustomFontAdded(const std::string& path)
{
  for(auto& loader : mAvailableLoaders)
  {
    loader.RequestAddCustomFont(path);
  }
  for(auto& loader : mRunningLoaders)
  {
    loader.RequestAddCustomFont(path);
  }
}

void AsyncTextManager::OnLocaleChanged(std::string locale)
{
  if(mLocale != locale)
  {
    mLocale = locale;

    {
      Mutex::ScopedLock lock(mLoaderMutex);
      // Move available loaders to locale changed loaders first.
      while(!mAvailableLoaders.empty())
      {
        auto& loader = mAvailableLoaders.back();
        loader.SetModuleClearNeeded(true);
        loader.SetLocaleUpdateNeeded(true);
        mLocaleChangedLoaders.push_back(loader);
        mAvailableLoaders.pop_back();
      }

      // When the Loader is in running state, just set the flag and clear it when it becomes available.
      for(auto& loader : mRunningLoaders)
      {
        loader.SetModuleClearNeeded(true);
        loader.SetLocaleUpdateNeeded(true);
      }
    }

    ResolveLocaleChangedLoader();
  }
}

void AsyncTextManager::ResolveLocaleChangedLoader()
{
  while(true)
  {
    // Resolve locale changed loaders one my one, to minimize mutex lock.
    Text::AsyncTextLoader localeChangedLoader;
    {
      Mutex::ScopedLock lock(mLoaderMutex);
      if(!mLocaleChangedLoaders.empty())
      {
        localeChangedLoader = mLocaleChangedLoaders.back();
        mLocaleChangedLoaders.pop_back();
      }
    }
    if(DALI_LIKELY(!localeChangedLoader))
    {
      break;
    }
    if(localeChangedLoader.IsModuleClearNeeded())
    {
      localeChangedLoader.ClearModule();
      localeChangedLoader.SetModuleClearNeeded(false);
    }
    if(localeChangedLoader.IsLocaleUpdateNeeded())
    {
      localeChangedLoader.SetLocale(mLocale);
      localeChangedLoader.SetLocaleUpdateNeeded(false);
    }
    {
      Mutex::ScopedLock lock(mLoaderMutex);
      mAvailableLoaders.push_back(std::move(localeChangedLoader));
    }
    // Newly available task added now. Request to waiting task works.
    SetLoaderToWaitingTask();
  }
}

/// Main + Worker thread called. Might be called under mTasksMutex mutex
Text::AsyncTextLoader AsyncTextManager::GetAvailableLoader()
{
  Text::AsyncTextLoader avaiableLoader;
  {
    Mutex::ScopedLock lock(mLoaderMutex);
    if(!mAvailableLoaders.empty())
    {
      avaiableLoader = mAvailableLoaders.back();
      mAvailableLoaders.pop_back();
      mRunningLoaders.push_back(avaiableLoader);
    }
  }
  return avaiableLoader;
}

uint32_t AsyncTextManager::RequestLoad(AsyncTextParameters& parameters, TextLoadObserver* observer)
{
  // Each task must have its own unique id.
  mTaskId++;

  auto task = new Dali::Toolkit::Internal::TextLoadingTask(mTaskId, parameters, MakeCallback(this, &AsyncTextManager::LoadComplete));

  LoadElement element(task, observer, parameters);

  if(observer)
  {
    observer->ConnectDestructionSignal();
    observer->DestructionSignal().Connect(this, &AsyncTextManager::ObserverDestroyed);
  }

  {
    Mutex::ScopedLock lock(mTasksMutex);

    Text::AsyncTextLoader loader = GetAvailableLoader();
    if(loader)
    {
      // Add element to running map.
      mRunningTasks[mTaskId] = element;

      // Loader move available list -> running list.
      task->SetLoader(loader);
    }
    else
    {
      // There is no available loader, add element to waiting queue.
      mWaitingTasks[mTaskId] = element;
    }
  }

  // We need to add task now, due to task completed callback could trace this task.
  Dali::AsyncTaskManager::Get().AddTask(task);

  return mTaskId;
}

void AsyncTextManager::RequestCancel(uint32_t taskId)
{
  TextLoadObserver*                     cancelledObserver = nullptr;
  Toolkit::Internal::TextLoadingTaskPtr cancelledTask;

  {
    Mutex::ScopedLock lock(mTasksMutex);
    auto              it = mWaitingTasks.find(taskId);
    if(it != mWaitingTasks.end())
    {
      if(it->second.mObserver)
      {
        cancelledObserver = it->second.mObserver;
      }
      cancelledTask = std::move(it->second.mTask);
      mWaitingTasks.erase(it);
    }
    else
    {
      auto it = mRunningTasks.find(taskId);
      if(it != mRunningTasks.end())
      {
        if(it->second.mObserver)
        {
          cancelledObserver = it->second.mObserver;
        }
        mRunningTasks.erase(it);
      }
      return;
    }
  }

  if(cancelledObserver && cancelledObserver->DisconnectDestructionSignal())
  {
    cancelledObserver->DestructionSignal().Disconnect(this, &AsyncTextManager::ObserverDestroyed);
  }
  if(cancelledTask)
  {
    Dali::AsyncTaskManager::Get().RemoveTask(cancelledTask);
  }
}

void AsyncTextManager::LoadComplete(Toolkit::Internal::TextLoadingTaskPtr task)
{
  uint32_t taskId = task->GetId();

  if(taskId == EMPTY_TASK_ID)
  {
    // Ignore callback from empty task.
    return;
  }

  TextLoadObserver* completedObserver = nullptr;

  {
    Mutex::ScopedLock lock(mTasksMutex);

    auto it = mRunningTasks.find(taskId);
    if(it != mRunningTasks.end())
    {
      // Find task, execute load complete.
      if(it->second.mObserver)
      {
        completedObserver = it->second.mObserver;
      }
      // If a task has completed, we souhld to remove the element from running map.
      mRunningTasks.erase(it);
    }
    else
    {
      DALI_LOG_DEBUG_INFO("LoadComplete -> Running task already removed! : %u\n", taskId);
    }
  }

  if(completedObserver)
  {
    // TODO : If it fails for any reason, false should be sent.
    bool success = true;
    if(completedObserver->DisconnectDestructionSignal())
    {
      completedObserver->DestructionSignal().Disconnect(this, &AsyncTextManager::ObserverDestroyed);
    }
    completedObserver->LoadComplete(success, TextLoadObserver::TextInformation(task->mRenderInfo, task->mParameters));
  }

  // Resolve locale changed events here, and request loader again.
  ResolveLocaleChangedLoader();
}

/// Worker thread called
void AsyncTextManager::ReleaseLoaderToManager(Toolkit::Internal::TextLoadingTaskPtr task, Text::AsyncTextLoader loader)
{
  std::unique_lock<std::mutex> lock(gStaticAsyncTextManagerMutex);
  if(gAsyncTextManager)
  {
    gAsyncTextManager->ReleaseLoader(task, loader);
  }
  else
  {
    DALI_LOG_DEBUG_INFO("Skip ReleaseLoader\n");
  }
}

/// Worker thread called
void AsyncTextManager::ReleaseLoader(Toolkit::Internal::TextLoadingTaskPtr task, Text::AsyncTextLoader loader)
{
  bool setLoaderAvailable = false;
  {
    Mutex::ScopedLock lock(mLoaderMutex);
    for(auto iter = mRunningLoaders.begin(); iter != mRunningLoaders.end(); ++iter)
    {
      if(*iter == loader)
      {
        setLoaderAvailable = !(loader.IsModuleClearNeeded() || loader.IsLocaleUpdateNeeded());
        if(DALI_LIKELY(setLoaderAvailable))
        {
          mAvailableLoaders.push_back(loader);
        }
        else
        {
          mLocaleChangedLoaders.push_back(loader);
        }
        mRunningLoaders.erase(iter);
        break;
      }
    }
  }

  if(DALI_LIKELY(setLoaderAvailable))
  {
    SetLoaderToWaitingTask();
  }
}

/// Main + Worker thread called
void AsyncTextManager::SetLoaderToWaitingTask()
{
  Mutex::ScopedLock lock(mTasksMutex);
  if(!mWaitingTasks.empty())
  {
    Text::AsyncTextLoader loader = GetAvailableLoader();
    if(loader)
    {
      // Takes out the oldest waiting queue. Give loader directly.
      auto        item = mWaitingTasks.begin();
      LoadElement element(std::move(item->second));
      mWaitingTasks.erase(item);

      if(element.mObserver)
      {
        // Puts it into the running map.
        uint32_t watingTaskId       = element.mTask->GetId();
        mRunningTasks[watingTaskId] = element;

        // Set loader and ready to process.
        element.mTask->SetLoader(loader);
      }
    }
  }
}

void AsyncTextManager::ObserverDestroyed(TextLoadObserver* observer)
{
  // TODO : Optimize here if possible.
  static std::vector<Toolkit::Internal::TextLoadingTaskPtr> cancelledTasks;
  {
    Mutex::ScopedLock lock(mTasksMutex);
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
        cancelledTasks.emplace_back(std::move(it->second.mTask));
        it->second.mObserver = nullptr;
        it                   = mWaitingTasks.erase(it);
      }
      else
      {
        ++it;
      }
    }
  }
  while(!cancelledTasks.empty())
  {
    Dali::AsyncTaskManager::Get().RemoveTask(cancelledTasks.back());
    cancelledTasks.pop_back();
  }
}

} // namespace Internal

} // namespace Text

} // namespace Toolkit

} //namespace DALI_NAMESPACE

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
 *
 */

// CLASS HEADER
#include <dali-toolkit/internal/visuals/animated-vector-image/vector-animation-manager.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/animated-vector-image/vector-animation-thread.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gVectorAnimationLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_VECTOR_ANIMATION");
#endif

DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_IMAGE_PERFORMANCE_MARKER, false);

} // unnamed namespace

VectorAnimationManager::VectorAnimationManager()
: mEventCallbacks(),
  mLifecycleObservers(),
  mVectorAnimationThread(nullptr),
  mProcessorRegistered(false),
  mDestroyed(false)
{
}

VectorAnimationManager::~VectorAnimationManager()
{
  Finalize();

  for(auto observer : mLifecycleObservers)
  {
    observer->VectorAnimationManagerDestroyed();
  }
}

void VectorAnimationManager::AddObserver(VectorAnimationManager::LifecycleObserver& observer)
{
  DALI_ASSERT_DEBUG(mLifecycleObservers.end() == std::find(mLifecycleObservers.begin(), mLifecycleObservers.end(), &observer));
  mLifecycleObservers.push_back(&observer);
}

void VectorAnimationManager::RemoveObserver(VectorAnimationManager::LifecycleObserver& observer)
{
  auto iterator = std::find(mLifecycleObservers.begin(), mLifecycleObservers.end(), &observer);
  if(iterator != mLifecycleObservers.end())
  {
    mLifecycleObservers.erase(iterator);
  }
}

VectorAnimationThread& VectorAnimationManager::GetVectorAnimationThread()
{
  if(!mVectorAnimationThread)
  {
    mVectorAnimationThread = std::unique_ptr<VectorAnimationThread>(new VectorAnimationThread());
    mVectorAnimationThread->Start();
  }
  return *mVectorAnimationThread;
}

void VectorAnimationManager::RegisterEventCallback(CallbackBase* callback)
{
  if(DALI_LIKELY(!mDestroyed))
  {
    mEventCallbacks.emplace_back(std::unique_ptr<Dali::CallbackBase>(callback));

    if(!mProcessorRegistered && DALI_LIKELY(Adaptor::IsAvailable()))
    {
      Adaptor::Get().RegisterProcessor(*this, true); // Use post processor to trigger after layoutting
      mProcessorRegistered = true;
    }
  }
  else
  {
    delete callback; // No longer needed.
  }
}

void VectorAnimationManager::UnregisterEventCallback(CallbackBase* callback)
{
  auto iter = std::find_if(mEventCallbacks.begin(),
                           mEventCallbacks.end(),
                           [callback](const std::unique_ptr<CallbackBase>& element) {
                             return element.get() == callback;
                           });
  if(iter != mEventCallbacks.end())
  {
    mEventCallbacks.erase(iter);

    if(mEventCallbacks.empty())
    {
      if(Adaptor::IsAvailable())
      {
        Adaptor::Get().UnregisterProcessor(*this, true);
        mProcessorRegistered = false;
      }
    }
  }
}

void VectorAnimationManager::Finalize()
{
  if(DALI_LIKELY(!mDestroyed))
  {
    DALI_LOG_DEBUG_INFO("Finalizing Vector Animation Manager.\n");
    mDestroyed = true;

    mEventCallbacks.clear();

    if(mProcessorRegistered && Adaptor::IsAvailable())
    {
      Adaptor::Get().UnregisterProcessor(*this, true);
      mProcessorRegistered = false;
    }

    if(mVectorAnimationThread)
    {
      mVectorAnimationThread->Finalize();
    }
  }
}

void VectorAnimationManager::Process(bool postProcessor)
{
  if(DALI_LIKELY(!mDestroyed))
  {
#ifdef TRACE_ENABLED
    if(gTraceFilter && gTraceFilter->IsTraceEnabled())
    {
      if(mEventCallbacks.size() > 0u)
      {
        std::ostringstream oss;
        oss << "[" << mEventCallbacks.size() << "]";
        DALI_TRACE_BEGIN_WITH_MESSAGE(gTraceFilter, "DALI_VECTOR_ANIMATION_MANAGER_PROCESS", oss.str().c_str());
      }
    }
#endif

    for(auto&& iter : mEventCallbacks)
    {
      CallbackBase::Execute(*iter);
    }

#ifdef TRACE_ENABLED
    if(gTraceFilter && gTraceFilter->IsTraceEnabled())
    {
      if(mEventCallbacks.size() > 0u)
      {
        std::ostringstream oss;
        oss << "[" << mEventCallbacks.size() << "]";
        DALI_TRACE_END_WITH_MESSAGE(gTraceFilter, "DALI_VECTOR_ANIMATION_MANAGER_PROCESS", oss.str().c_str());
      }
    }
#endif
    mEventCallbacks.clear();
  }

  Adaptor::Get().UnregisterProcessor(*this, true);
  mProcessorRegistered = false;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

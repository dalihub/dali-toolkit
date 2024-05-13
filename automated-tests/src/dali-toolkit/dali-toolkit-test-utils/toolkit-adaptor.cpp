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

#include <algorithm>

#include <toolkit-window-impl.h>

// Don't want to include the actual window.h which otherwise will be indirectly included by adaptor.h.
#define DALI_WINDOW_H
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/adaptor-framework/scene-holder.h>

#include <dali/integration-api/debug.h>
#include <dali/integration-api/scene.h>
#include <test-application.h>
#include <toolkit-adaptor-impl.h>
#include <toolkit-application.h>
#include <toolkit-async-task-manager.h>
#include <toolkit-scene-holder-impl.h>
#include <toolkit-test-application.h>
#include "dali-test-suite-utils.h"

namespace Dali
{
namespace Internal
{
namespace Adaptor
{
///////////////////////////////////////////////////////////////////////////////
//
// Dali::Internal::Adaptor::Adaptor Stub
//
///////////////////////////////////////////////////////////////////////////////

Dali::Adaptor* gAdaptor = nullptr;

Dali::Adaptor& Adaptor::New()
{
  DALI_ASSERT_ALWAYS(!gAdaptor);
  gAdaptor = new Dali::Adaptor;
  return *gAdaptor;
}

Dali::Adaptor& Adaptor::Get()
{
  DALI_ASSERT_ALWAYS(gAdaptor);
  return *gAdaptor;
}

Adaptor::Adaptor()
{
}

Adaptor::~Adaptor()
{
  gAdaptor = nullptr;

  // Ensure all threads and not-excuted tasks are destroyed.
  // TODO : we'd better make some singletone service for toolkit UTC in future.
  Test::AsyncTaskManager::DestroyAsyncTaskManager();
}

void Adaptor::Start(Dali::Window window)
{
  AddWindow(&GetImplementation(window));
}

void Adaptor::Stop()
{
  if(mTestApplication)
  {
    Integration::Core& core = mTestApplication->GetCore();
    tet_printf("Adaptor::UnregisterProcessors\n");
    core.UnregisterProcessors();
  }

  mStopped = true;
}

Integration::Scene Adaptor::GetScene(Dali::Window window)
{
  return window.GetScene();
}

bool Adaptor::AddIdle(CallbackBase* callback, bool hasReturnValue)
{
  if(ToolkitApplication::ADD_IDLE_SUCCESS)
  {
    if(hasReturnValue)
    {
      mReturnCallbacks.PushBack(callback);
    }
    else
    {
      mCallbacks.PushBack(callback);
    }
  }
  return ToolkitApplication::ADD_IDLE_SUCCESS;
}

void Adaptor::RemoveIdle(CallbackBase* callback)
{
  mCallbacks.Erase(std::remove_if(mCallbacks.Begin(), mCallbacks.End(), [&callback](CallbackBase* current) { return callback == current; }), mCallbacks.End());
  mReturnCallbacks.Erase(std::remove_if(mReturnCallbacks.Begin(), mReturnCallbacks.End(), [&callback](CallbackBase* current) { return callback == current; }), mReturnCallbacks.End());
}

void Adaptor::RunIdles()
{
  Dali::Vector<CallbackBase*> reusedCallbacks;
  for(auto& callback : mReturnCallbacks)
  {
    bool retValue = CallbackBase::ExecuteReturn<bool>(*callback);
    if(retValue)
    {
      reusedCallbacks.PushBack(callback);
    }
  }
  for(auto& callback : mCallbacks)
  {
    CallbackBase::Execute(*callback);
  }

  mCallbacks.Clear();
  mReturnCallbacks.Clear();
  mReturnCallbacks.Swap(reusedCallbacks);
}

void Adaptor::RequestUpdateOnce()
{
  if(mTestApplication)
  {
    auto scene = mTestApplication->GetScene();
    if(scene)
    {
      tet_printf("Adaptor::RequestUpdateOnce()\n");
      scene.KeepRendering(0.0f);
    }
  }
}

Dali::RenderSurfaceInterface& Adaptor::GetSurface()
{
  DALI_ASSERT_ALWAYS(!mWindows.empty());

  return reinterpret_cast<Dali::RenderSurfaceInterface&>(mWindows.front()->GetRenderSurface());
}

Dali::WindowContainer Adaptor::GetWindows()
{
  Dali::WindowContainer windows;

  for(auto iter = mWindows.begin(); iter != mWindows.end(); ++iter)
  {
    // Downcast to Dali::Window
    Dali::Window window(dynamic_cast<Dali::Internal::Adaptor::Window*>(*iter));
    if(window)
    {
      windows.push_back(window);
    }
  }

  return windows;
}

Dali::SceneHolderList Adaptor::GetSceneHolders()
{
  Dali::SceneHolderList sceneHolderList;

  for(auto iter = mWindows.begin(); iter != mWindows.end(); ++iter)
  {
    sceneHolderList.push_back(Dali::Integration::SceneHolder(*iter));
  }

  return sceneHolderList;
}

Dali::Internal::Adaptor::SceneHolder* Adaptor::GetWindow(Dali::Actor& actor)
{
  Dali::Integration::Scene scene = Dali::Integration::Scene::Get(actor);

  for(auto window : mWindows)
  {
    if(scene == window->GetScene())
    {
      return window;
    }
  }

  return nullptr;
}

void Adaptor::AddWindow(Internal::Adaptor::SceneHolder* window)
{
  if(window)
  {
    mWindows.push_back(window);

    Dali::Integration::SceneHolder newWindow(window);
    mWindowCreatedSignal.Emit(newWindow);
  }
}

void Adaptor::RemoveWindow(Internal::Adaptor::SceneHolder* window)
{
  auto iter = std::find(mWindows.begin(), mWindows.end(), window);
  if(iter != mWindows.end())
  {
    mWindows.erase(iter);
  }
}

void Adaptor::RegisterProcessor(Integration::Processor& processor, bool postProcessor)
{
  Integration::Core& core = mTestApplication->GetCore();
  tet_printf("Adaptor::RegisterProcessor : %s\n", processor.GetProcessorName().data());
  core.RegisterProcessor(processor, postProcessor);
}

void Adaptor::UnregisterProcessor(Integration::Processor& processor, bool postProcessor)
{
  Integration::Core& core = mTestApplication->GetCore();
  tet_printf("Adaptor::UnregisterProcessor : %s\n", processor.GetProcessorName().data());
  core.UnregisterProcessor(processor, postProcessor);
}

void Adaptor::SetApplication(Dali::TestApplication& testApplication)
{
  mTestApplication = &testApplication;
}

Dali::Adaptor::AdaptorSignalType& Adaptor::ResizedSignal()
{
  return mResizedSignal;
}

Dali::Adaptor::AdaptorSignalType& Adaptor::LanguageChangedSignal()
{
  return mLanguageChangedSignal;
}

Dali::Adaptor::WindowCreatedSignalType& Adaptor::WindowCreatedSignal()
{
  return mWindowCreatedSignal;
}

} // namespace Adaptor
} // namespace Internal

///////////////////////////////////////////////////////////////////////////////
//
// Dali::Adaptor Stub
//
///////////////////////////////////////////////////////////////////////////////

Adaptor::Adaptor()
: mImpl(new Internal::Adaptor::Adaptor)
{
}

Adaptor::~Adaptor()
{
  Internal::Adaptor::gAdaptor = nullptr;
  delete mImpl;
}

void Adaptor::Start()
{
}

void Adaptor::Pause()
{
}

void Adaptor::Resume()
{
}

void Adaptor::Stop()
{
  mImpl->Stop();
}

bool Adaptor::AddIdle(CallbackBase* callback, bool hasReturnValue)
{
  return mImpl->AddIdle(callback, hasReturnValue);
}

void Adaptor::RemoveIdle(CallbackBase* callback)
{
  mImpl->RemoveIdle(callback);
}

void Adaptor::ReplaceSurface(Window window, Dali::RenderSurfaceInterface& surface)
{
}

void Adaptor::ReplaceSurface(Dali::Integration::SceneHolder window, Dali::RenderSurfaceInterface& surface)
{
}

Adaptor::AdaptorSignalType& Adaptor::ResizedSignal()
{
  return mImpl->ResizedSignal();
}

Adaptor::AdaptorSignalType& Adaptor::LanguageChangedSignal()
{
  return mImpl->LanguageChangedSignal();
}

Adaptor::WindowCreatedSignalType& Adaptor::WindowCreatedSignal()
{
  return mImpl->WindowCreatedSignal();
}

Dali::RenderSurfaceInterface& Adaptor::GetSurface()
{
  return mImpl->GetSurface();
}

Dali::WindowContainer Adaptor::GetWindows() const
{
  return mImpl->GetWindows();
}

Dali::SceneHolderList Adaptor::GetSceneHolders() const
{
  return mImpl->GetSceneHolders();
}

Any Adaptor::GetNativeWindowHandle()
{
  Any window;
  return window;
}

Any Adaptor::GetNativeWindowHandle(Actor actor)
{
  return GetNativeWindowHandle();
}

void Adaptor::ReleaseSurfaceLock()
{
}

void Adaptor::SetRenderRefreshRate(unsigned int numberOfVSyncsPerRender)
{
}

Adaptor& Adaptor::Get()
{
  return Internal::Adaptor::Adaptor::Get();
}

bool Adaptor::IsAvailable()
{
  return Internal::Adaptor::gAdaptor && (!Internal::Adaptor::Adaptor::GetImpl(*Internal::Adaptor::gAdaptor).IsStopped());
}

void Adaptor::NotifySceneCreated()
{
}

void Adaptor::NotifyLanguageChanged()
{
}

void Adaptor::FeedTouchPoint(TouchPoint& point, int timeStamp)
{
}

void Adaptor::FeedWheelEvent(WheelEvent& wheelEvent)
{
}

void Adaptor::FeedKeyEvent(KeyEvent& keyEvent)
{
}

void Adaptor::SceneCreated()
{
}

class LogFactory : public LogFactoryInterface
{
public:
  virtual void InstallLogFunction() const
  {
    Dali::Integration::Log::LogFunction logFunction(&ToolkitTestApplication::LogMessage);
    Dali::Integration::Log::InstallLogFunction(logFunction);
  }

  LogFactory()
  {
  }
  virtual ~LogFactory()
  {
  }
};

LogFactory*                gLogFactory = NULL;
const LogFactoryInterface& Adaptor::GetLogFactory()
{
  if(gLogFactory == NULL)
  {
    gLogFactory = new LogFactory;
  }
  return *gLogFactory;
}

class TraceFactory : public TraceFactoryInterface
{
public:
  virtual void InstallTraceFunction() const
  {
    Dali::Integration::Trace::LogContextFunction logContextFunction(&TestApplication::LogContext);
    Dali::Integration::Trace::InstallLogContextFunction(logContextFunction);
  }

  TraceFactory()
  {
  }
  virtual ~TraceFactory()
  {
  }
};

TraceFactory*                gTraceFactory = NULL;
const TraceFactoryInterface& Adaptor::GetTraceFactory()
{
  if(gTraceFactory == NULL)
  {
    gTraceFactory = new TraceFactory;
  }
  return *gTraceFactory;
}

void Adaptor::RegisterProcessor(Integration::Processor& processor, bool postProcessor)
{
  mImpl->RegisterProcessor(processor, postProcessor);
}

void Adaptor::UnregisterProcessor(Integration::Processor& processor, bool postProcessor)
{
  mImpl->UnregisterProcessor(processor, postProcessor);
}

} // namespace Dali

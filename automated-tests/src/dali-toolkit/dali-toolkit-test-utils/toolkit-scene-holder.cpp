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

#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/integration-api/events/touch-event-integ.h>

#include <toolkit-scene-holder-impl.h>

#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/object/base-object.h>

#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <toolkit-adaptor-impl.h>
#include "test-render-surface.h"

using AdaptorImpl = Dali::Internal::Adaptor::Adaptor;

namespace Dali
{
///////////////////////////////////////////////////////////////////////////////
//
// Dali::Internal::Adaptor::SceneHolder Stub
//
///////////////////////////////////////////////////////////////////////////////

namespace Internal
{
namespace Adaptor
{
class SceneHolder::SceneHolderLifeCycleObserver
{
public:
  SceneHolderLifeCycleObserver(Adaptor*& adaptor, bool& adaptorStarted)
  : mAdaptor(adaptor),
    mAdaptorStarted(adaptorStarted)
  {
  }

private: // Adaptor::LifeCycleObserver interface
  virtual void OnStart()
  {
    mAdaptorStarted = true;
  };
  virtual void OnPause(){};
  virtual void OnResume(){};
  virtual void OnStop()
  {
    // Mark adaptor as stopped;
    mAdaptorStarted = false;
  };
  virtual void OnDestroy()
  {
    mAdaptor = nullptr;
  };

private:
  Adaptor*& mAdaptor;
  bool&     mAdaptorStarted;
};

SceneHolder::SceneHolder(const Dali::Rect<int>& positionSize)
: mId(0),
  mScene(Dali::Integration::Scene::New(Dali::Size(static_cast<float>(positionSize.width), static_cast<float>(positionSize.height)))),
  mRenderSurface(new TestRenderSurface(positionSize))
{
}

SceneHolder::~SceneHolder()
{
  if(Dali::Adaptor::IsAvailable())
  {
    AdaptorImpl::GetImpl(AdaptorImpl::Get()).RemoveWindow(this);
  }
}

void SceneHolder::Add(Dali::Actor actor)
{
  mScene.Add(actor);
}

void SceneHolder::Remove(Dali::Actor actor)
{
  mScene.Remove(actor);
}

Dali::Layer SceneHolder::GetRootLayer() const
{
  return mScene.GetRootLayer();
}

void SceneHolder::SetBackgroundColor(Vector4 color)
{
  return mScene.SetBackgroundColor(color);
}

Vector4 SceneHolder::GetBackgroundColor() const
{
  return mScene.GetBackgroundColor();
}

void SceneHolder::FeedTouchPoint(Dali::TouchPoint& point, int timeStamp)
{
}

void SceneHolder::FeedWheelEvent(Dali::WheelEvent& wheelEvent)
{
}

void SceneHolder::FeedKeyEvent(Dali::KeyEvent& keyEvent)
{
}

Dali::Integration::SceneHolder::KeyEventSignalType& SceneHolder::KeyEventSignal()
{
  return mScene.KeyEventSignal();
}

Dali::Integration::SceneHolder::KeyEventGeneratedSignalType& SceneHolder::KeyEventGeneratedSignal()
{
  return mScene.KeyEventGeneratedSignal();
}

Dali::Integration::SceneHolder::TouchEventSignalType& SceneHolder::TouchedSignal()
{
  return mScene.TouchedSignal();
}

Dali::Integration::SceneHolder::WheelEventSignalType& SceneHolder::WheelEventSignal()
{
  return mScene.WheelEventSignal();
}

Dali::Integration::SceneHolder::WheelEventGeneratedSignalType& SceneHolder::WheelEventGeneratedSignal()
{
  return mScene.WheelEventGeneratedSignal();
}

Integration::Scene SceneHolder::GetScene()
{
  return mScene;
}

Dali::Integration::RenderSurfaceInterface& SceneHolder::GetRenderSurface()
{
  return *mRenderSurface;
}

Dali::RenderTaskList SceneHolder::GetRenderTaskList()
{
  return mScene.GetRenderTaskList();
}

} // namespace Adaptor

} // namespace Internal

///////////////////////////////////////////////////////////////////////////////
//
// Dali::Integration::SceneHolder Stub
//
///////////////////////////////////////////////////////////////////////////////

namespace Integration
{
SceneHolder::SceneHolder()
{
  // Dali::Internal::Adaptor::Adaptor::Get().WindowCreatedSignal().Emit( *this );
}

SceneHolder::~SceneHolder()
{
}

SceneHolder::SceneHolder(const SceneHolder& handle)
: BaseHandle(handle)
{
}

SceneHolder::SceneHolder(Internal::Adaptor::SceneHolder* internal)
: BaseHandle(internal)
{
}

SceneHolder& SceneHolder::operator=(const SceneHolder& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

Dali::Integration::SceneHolder SceneHolder::Get(Dali::Actor actor)
{
  Internal::Adaptor::SceneHolder* sceneHolderImpl = nullptr;

  if(Dali::Adaptor::IsAvailable())
  {
    sceneHolderImpl = AdaptorImpl::GetImpl(AdaptorImpl::Get()).GetWindow(actor);
  }

  return Dali::Integration::SceneHolder(sceneHolderImpl);
}

void SceneHolder::Add(Actor actor)
{
  GetImplementation(*this).Add(actor);
}

void SceneHolder::Remove(Actor actor)
{
  GetImplementation(*this).Remove(actor);
}

Dali::Layer SceneHolder::GetRootLayer() const
{
  return GetImplementation(*this).GetRootLayer();
}

void SceneHolder::SetBackgroundColor(Vector4 color)
{
  GetImplementation(*this).SetBackgroundColor(color);
}

Vector4 SceneHolder::GetBackgroundColor() const
{
  return GetImplementation(*this).GetBackgroundColor();
}

void SceneHolder::FeedTouchPoint(Dali::TouchPoint& point, int timeStamp)
{
  GetImplementation(*this).FeedTouchPoint(point, timeStamp);
}

void SceneHolder::FeedWheelEvent(Dali::WheelEvent& wheelEvent)
{
  GetImplementation(*this).FeedWheelEvent(wheelEvent);
}

void SceneHolder::FeedKeyEvent(Dali::KeyEvent& keyEvent)
{
  GetImplementation(*this).FeedKeyEvent(keyEvent);
}

RenderTaskList SceneHolder::GetRenderTaskList()
{
  return GetImplementation(*this).GetRenderTaskList();
}

SceneHolder::KeyEventSignalType& SceneHolder::KeyEventSignal()
{
  return GetImplementation(*this).KeyEventSignal();
}

SceneHolder::KeyEventGeneratedSignalType& SceneHolder::KeyEventGeneratedSignal()
{
  return GetImplementation(*this).KeyEventGeneratedSignal();
}

SceneHolder::TouchEventSignalType& SceneHolder::TouchedSignal()
{
  return GetImplementation(*this).TouchedSignal();
}

SceneHolder::WheelEventSignalType& SceneHolder::WheelEventSignal()
{
  return GetImplementation(*this).WheelEventSignal();
}

SceneHolder::WheelEventGeneratedSignalType& SceneHolder::WheelEventGeneratedSignal()
{
  return GetImplementation(*this).WheelEventGeneratedSignal();
}

} // namespace Integration

} // namespace Dali

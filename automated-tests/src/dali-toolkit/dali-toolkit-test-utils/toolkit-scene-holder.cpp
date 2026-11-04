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

#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/events/wheel-event.h>

#include <toolkit-scene-holder-impl.h>

#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/object/base-object.h>

#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <toolkit-adaptor-impl.h>
#include "test-render-surface.h"

using AdaptorImpl = Dali::Internal::Adaptor::Adaptor;

namespace DALI_NAMESPACE
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

  virtual ~SceneHolderLifeCycleObserver()
  {
  }

private: // Adaptor::LifeCycleObserver interface
  virtual void OnStart()
  {
    mAdaptorStarted = true;
  }
  virtual void OnPause()
  {
  }
  virtual void OnResume()
  {
  }
  virtual void OnStop()
  {
    // Mark adaptor as stopped;
    mAdaptorStarted = false;
  }
  virtual void OnDestroy()
  {
    mAdaptor = nullptr;
  }

private:
  Adaptor*& mAdaptor;
  bool&     mAdaptorStarted;
};

SceneHolder::SceneHolder(const Dali::BoundsInteger& positionSize)
: mId(0),
  mRenderSurface(new TestRenderSurface(positionSize)),
  mFocusChangedGeneratedSignal(),
  mSceneSignalBridgeSlot(this)
{
  Graphics::RenderTargetCreateInfo createInfo;
  createInfo.SetExtent({static_cast<uint16_t>(positionSize.width), static_cast<uint16_t>(positionSize.height)});
  createInfo.SetSurface(mRenderSurface.get());
  mScene = Dali::Integration::Scene::New(createInfo, Dali::Size(static_cast<float>(positionSize.width), static_cast<float>(positionSize.height)));

  mScene.KeyEventSignal().Connect(mSceneSignalBridgeSlot, &SceneHolder::OnSceneKeyEvent);
  mScene.KeyEventGeneratedSignal().Connect(mSceneSignalBridgeSlot, &SceneHolder::OnSceneKeyEventGenerated);
  mScene.TouchEventSignal().Connect(mSceneSignalBridgeSlot, &SceneHolder::OnSceneTouchEvent);
  mScene.WheelEventSignal().Connect(mSceneSignalBridgeSlot, &SceneHolder::OnSceneWheelEvent);
  mScene.WheelEventGeneratedSignal().Connect(mSceneSignalBridgeSlot, &SceneHolder::OnSceneWheelEventGenerated);
}

SceneHolder::~SceneHolder()
{
  if(Dali::Adaptor::IsAvailable())
  {
    if(mScene)
    {
      // The scene graph object should be removed first.
      mScene.RemoveSceneObject();
    }
    AdaptorImpl::GetImpl(AdaptorImpl::Get()).RemoveWindow(this);
    if(mScene)
    {
      // We should remove the surface from the Core last
      mScene.Discard();
    }
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
  return mSceneHolderKeyEventSignal;
}

Dali::Integration::SceneHolder::KeyEventGeneratedSignalType& SceneHolder::KeyEventGeneratedSignal()
{
  return mSceneHolderKeyEventGeneratedSignal;
}

Dali::Integration::SceneHolder::TouchEventSignalType& SceneHolder::TouchEventSignal()
{
  return mSceneHolderTouchEventSignal;
}

Dali::Integration::SceneHolder::WheelEventSignalType& SceneHolder::WheelEventSignal()
{
  return mSceneHolderWheelEventSignal;
}

Dali::Integration::SceneHolder::WheelEventGeneratedSignalType& SceneHolder::WheelEventGeneratedSignal()
{
  return mSceneHolderWheelEventGeneratedSignal;
}

void SceneHolder::OnSceneKeyEvent(Dali::KeyEvent event)
{
  Dali::Integration::SceneHolder handle(this);
  mSceneHolderKeyEventSignal.Emit(handle, event);
}

bool SceneHolder::OnSceneKeyEventGenerated(Dali::KeyEvent event)
{
  Dali::Integration::SceneHolder handle(this);
  return mSceneHolderKeyEventGeneratedSignal.Emit(handle, event);
}

void SceneHolder::OnSceneTouchEvent(Dali::TouchEvent event)
{
  Dali::Integration::SceneHolder handle(this);
  mSceneHolderTouchEventSignal.Emit(handle, event);
}

void SceneHolder::OnSceneWheelEvent(Dali::WheelEvent event)
{
  Dali::Integration::SceneHolder handle(this);
  mSceneHolderWheelEventSignal.Emit(handle, event);
}

bool SceneHolder::OnSceneWheelEventGenerated(Dali::WheelEvent event)
{
  Dali::Integration::SceneHolder handle(this);
  return mSceneHolderWheelEventGeneratedSignal.Emit(handle, event);
}

Dali::Integration::Scene SceneHolder::GetScene()
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

int32_t SceneHolder::GetNativeId() const
{
  return mScene.GetNativeId();
}

void SceneHolder::SetGeometryHittestEnabled(bool enabled)
{
  mScene.SetGeometryHittestEnabled(enabled);
}

bool SceneHolder::IsGeometryHittestEnabled()
{
  return mScene.IsGeometryHittestEnabled();
}

Dali::Integration::SceneHolder::KeyEventGeneratedSignalType& SceneHolder::InterceptKeyEventSignal()
{
  return mSceneHolderInterceptKeyEventSignal;
}

Dali::Integration::SceneHolder::KeyEventSignalType& SceneHolder::KeyEventDelayedSignal()
{
  return mSceneHolderKeyEventDelayedSignal;
}

Dali::Integration::SceneHolder::FocusChangedGeneratedSignalType& SceneHolder::FocusChangedGeneratedSignal()
{
  return mFocusChangedGeneratedSignal;
}

void SceneHolder::FocusChanged(bool focusIn)
{
  if(!mFocusChangedGeneratedSignal.Empty())
  {
    Dali::Integration::SceneHolder handle(this);
    mFocusChangedGeneratedSignal.Emit(handle, focusIn);
  }
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

Dali::Any SceneHolder::GetNativeHandle() const
{
  // Desktop UTCs have no native window. Falling through to the real adaptor
  // would access a mock with a different ABI (see GetNativeId below).
  return Dali::Any(0u);
}

Dali::Any SceneHolder::GetNativeRawHandle() const
{
  // Same ABI trap as GetNativeHandle above.
  return Dali::Any(0u);
}

int32_t SceneHolder::GetNativeId() const
{
  // ABI trap: the real adaptor's out-of-line implementation reads mScene at a
  // different offset of this mock object. Route to the internal stub, which
  // uses the mock's own layout.
  return GetImplementation(*this).GetNativeId();
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

void SceneHolder::FeedHoverEvent(Dali::TouchPoint& point)
{
  // Same ABI trap as GetNativeHandle above. The mock has no hover event
  // combiner and no current toolkit UTC injects hover this way, so the
  // event is dropped instead of falling through to the real adaptor.
  (void)point;
}

RenderTaskList SceneHolder::GetRenderTaskList()
{
  return GetImplementation(*this).GetRenderTaskList();
}

void SceneHolder::SetGeometryHittestEnabled(bool enabled)
{
  GetImplementation(*this).SetGeometryHittestEnabled(enabled);
}

bool SceneHolder::IsGeometryHittestEnabled()
{
  return GetImplementation(*this).IsGeometryHittestEnabled();
}

SceneHolder::KeyEventSignalType& SceneHolder::KeyEventSignal()
{
  return GetImplementation(*this).KeyEventSignal();
}

SceneHolder::KeyEventGeneratedSignalType& SceneHolder::KeyEventGeneratedSignal()
{
  return GetImplementation(*this).KeyEventGeneratedSignal();
}

SceneHolder::KeyEventGeneratedSignalType& SceneHolder::InterceptKeyEventSignal()
{
  // Same ABI trap as GetNativeHandle above: this signal lives at a different
  // offset in the real adaptor's SceneHolder layout.
  return GetImplementation(*this).InterceptKeyEventSignal();
}

SceneHolder::KeyEventSignalType& SceneHolder::KeyEventDelayedSignal()
{
  // Same ABI trap as GetNativeHandle above: this signal lives at a different
  // offset in the real adaptor's SceneHolder layout.
  return GetImplementation(*this).KeyEventDelayedSignal();
}

SceneHolder::TouchEventSignalType& SceneHolder::TouchEventSignal()
{
  return GetImplementation(*this).TouchEventSignal();
}

SceneHolder::WheelEventSignalType& SceneHolder::WheelEventSignal()
{
  return GetImplementation(*this).WheelEventSignal();
}

SceneHolder::WheelEventGeneratedSignalType& SceneHolder::WheelEventGeneratedSignal()
{
  return GetImplementation(*this).WheelEventGeneratedSignal();
}

SceneHolder::FocusChangedGeneratedSignalType& SceneHolder::FocusChangedGeneratedSignal()
{
  return GetImplementation(*this).FocusChangedGeneratedSignal();
}

} // namespace Integration

} //namespace DALI_NAMESPACE

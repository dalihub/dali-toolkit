/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/adaptors/scene-holder.h>

#include <toolkit-scene-holder-impl.h>

#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/object/base-object.h>

#include <dali/integration-api/adaptors/adaptor.h>
#include <toolkit-adaptor-impl.h>

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

SceneHolder::SceneHolder( const Dali::Rect<int>& positionSize )
: mRenderSurface( positionSize ),
  mScene( Dali::Integration::Scene::New( mRenderSurface ) )
{
}

SceneHolder::~SceneHolder()
{
}

void SceneHolder::SetBackgroundColor( Vector4 color )
{
  return mScene.SetBackgroundColor( color );
}

Vector4 SceneHolder::GetBackgroundColor() const
{
  return mScene.GetBackgroundColor();
}

void SceneHolder::FeedTouchPoint( Dali::TouchPoint& point, int timeStamp )
{
}

void SceneHolder::FeedWheelEvent( Dali::WheelEvent& wheelEvent )
{
}

void SceneHolder::FeedKeyEvent( Dali::KeyEvent& keyEvent )
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

Dali::Integration::SceneHolder::TouchSignalType& SceneHolder::TouchSignal()
{
  return mScene.TouchSignal();
}

Dali::Integration::SceneHolder::WheelEventSignalType& SceneHolder::WheelEventSignal()
{
  return mScene.WheelEventSignal();
}

Integration::Scene SceneHolder::GetScene()
{
  return mScene;
}

Integration::RenderSurface& SceneHolder::GetRenderSurface()
{
  return mRenderSurface;
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

SceneHolder::SceneHolder( const SceneHolder& handle )
: BaseHandle(handle)
{
}

SceneHolder::SceneHolder( Internal::Adaptor::SceneHolder* internal )
: BaseHandle(internal)
{
}

SceneHolder& SceneHolder::operator=( const SceneHolder& rhs )
{
  BaseHandle::operator=(rhs);
  return *this;
}

void SceneHolder::Add( Actor actor )
{
}

void SceneHolder::Remove( Actor actor )
{
}

Dali::Layer SceneHolder::GetRootLayer() const
{
  return Dali::Stage::GetCurrent().GetRootLayer();
}

void SceneHolder::SetBackgroundColor( Vector4 color )
{
  GetImplementation( *this ).SetBackgroundColor( color );
}

Vector4 SceneHolder::GetBackgroundColor() const
{
  return GetImplementation( *this ).GetBackgroundColor();
}

void SceneHolder::FeedTouchPoint( Dali::TouchPoint& point, int timeStamp )
{
  GetImplementation( *this ).FeedTouchPoint( point, timeStamp );
}

void SceneHolder::FeedWheelEvent( Dali::WheelEvent& wheelEvent )
{
  GetImplementation( *this ).FeedWheelEvent( wheelEvent );
}

void SceneHolder::FeedKeyEvent( Dali::KeyEvent& keyEvent )
{
  GetImplementation( *this ).FeedKeyEvent( keyEvent );
}

SceneHolder::KeyEventSignalType& SceneHolder::KeyEventSignal()
{
  return GetImplementation( *this ).KeyEventSignal();
}

SceneHolder::KeyEventGeneratedSignalType& SceneHolder::KeyEventGeneratedSignal()
{
  return GetImplementation( *this ).KeyEventGeneratedSignal();
}

SceneHolder::TouchSignalType& SceneHolder::TouchSignal()
{
  return GetImplementation( *this ).TouchSignal();
}

SceneHolder::WheelEventSignalType& SceneHolder::WheelEventSignal()
{
  return GetImplementation( *this ).WheelEventSignal();
}

SceneHolder SceneHolder::Get( Actor actor )
{
  return SceneHolder();
}

} // Integration

} // Dali

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

#include <dali/integration-api/adaptors/scene-holder.h>

#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/object/base-object.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

class SceneHolder: public Dali::BaseObject
{

};

} // Adaptor

} // Internal

namespace Integration
{

SceneHolder::SceneHolder()
{
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

Layer SceneHolder::GetRootLayer() const
{
  return Dali::Stage::GetCurrent().GetRootLayer();
}

void SceneHolder::SetBackgroundColor( Vector4 color )
{
}

Vector4 SceneHolder::GetBackgroundColor() const
{
  return Dali::Stage::GetCurrent().GetBackgroundColor();
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

} // Integration

} // Dali

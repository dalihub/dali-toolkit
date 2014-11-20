/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/control.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>

namespace Dali
{

namespace Toolkit
{

Control Control::New()
{
  return Internal::Control::New();
}

Control::Control()
{
}

Control::Control(const Control& uiControl)
: CustomActor( uiControl ? static_cast< const Internal::Control& >( uiControl.GetImplementation() ).GetOwner() : NULL)
{
}

Control::~Control()
{
}

Control& Control::operator=( const Control& handle )
{
  if( &handle != this )
  {
    CustomActor::operator=( handle );
  }
  return *this;
}

Control Control::DownCast( BaseHandle handle )
{
  return DownCast< Control, Internal::Control >(handle);
}

Internal::Control& Control::GetImplementation()
{
  return static_cast<Internal::Control&>(CustomActor::GetImplementation());
}

const Internal::Control& Control::GetImplementation() const
{
  return static_cast<const Internal::Control&>(CustomActor::GetImplementation());
}

void Control::SetKeyInputFocus()
{
  GetImplementation().SetKeyInputFocus();
}

bool Control::HasKeyInputFocus()
{
  return GetImplementation().HasKeyInputFocus();
}

void Control::ClearKeyInputFocus()
{
  GetImplementation().ClearKeyInputFocus();
}

PinchGestureDetector Control::GetPinchGestureDetector() const
{
  return GetImplementation().GetPinchGestureDetector();
}

PanGestureDetector Control::GetPanGestureDetector() const
{
  return GetImplementation().GetPanGestureDetector();
}

TapGestureDetector Control::GetTapGestureDetector() const
{
  return GetImplementation().GetTapGestureDetector();
}

LongPressGestureDetector Control::GetLongPressGestureDetector() const
{
  return GetImplementation().GetLongPressGestureDetector();
}

void Control::SetBackgroundColor( const Vector4& color )
{
  GetImplementation().SetBackgroundColor( color );
}

Vector4 Control::GetBackgroundColor() const
{
  return GetImplementation().GetBackgroundColor();
}

void Control::SetBackgroundImage( Image image )
{
  GetImplementation().SetBackgroundImage( image );
}

void Control::ClearBackground()
{
  GetImplementation().ClearBackground();
}

Actor Control::GetBackgroundActor() const
{
  return GetImplementation().GetBackgroundActor();
}

Control::KeyEventSignalType& Control::KeyEventSignal()
{
  return GetImplementation().KeyEventSignal();
}

Control::Control(Internal::Control& implementation)
: CustomActor(implementation)
{
}

Control::Control(Dali::Internal::CustomActor* internal)
: CustomActor(internal)
{
  VerifyCustomActorPointer<Internal::Control>(internal);
}

} // namespace Toolkit

} // namespace Dali

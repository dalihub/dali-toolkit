//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/control-impl.h>

namespace Dali
{

namespace Toolkit
{

const char* const Control::ACTION_CONTROL_ACTIVATED = "control-activated";
const char* const Control::SIGNAL_KEY_EVENT = "key-event";

Control Control::New()
{
  // Use TypeRegistry to create instance of control so that the type-info matches Control rather than ControlImpl
  TypeInfo typeInfo = TypeRegistry::Get().GetTypeInfo( typeid(Control) );
  DALI_ASSERT_ALWAYS( typeInfo && "TypeRegistry returning Invalid TypeInfo" );

  BaseHandle handle = typeInfo.CreateInstance();
  DALI_ASSERT_ALWAYS( handle && "Unable to Create Control" );

  Control control = DownCast( handle );
  DALI_ASSERT_ALWAYS( handle && "TypeRegistry did not create a Control" );

  return control;
}

Control::Control()
{
}

Control::Control(const Control& uiControl)
: CustomActor( uiControl ? static_cast<const ControlImpl&>( uiControl.GetImplementation() ).GetOwner() : NULL)
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
  return DownCast<Control, ControlImpl>(handle);
}

ControlImpl& Control::GetImplementation()
{
  return static_cast<ControlImpl&>(CustomActor::GetImplementation());
}

const ControlImpl& Control::GetImplementation() const
{
  return static_cast<const ControlImpl&>(CustomActor::GetImplementation());
}

void Control::SetSizePolicy( SizePolicy widthPolicy, SizePolicy heightPolicy )
{
  GetImplementation().SetSizePolicy( widthPolicy, heightPolicy );
}

void Control::GetSizePolicy( SizePolicy& widthPolicy, SizePolicy& heightPolicy ) const
{
  GetImplementation().GetSizePolicy( widthPolicy, heightPolicy );
}

void Control::SetMinimumSize( const Vector3& size )
{
  GetImplementation().SetMinimumSize( size );
}

const Vector3& Control::GetMinimumSize() const
{
  return GetImplementation().GetMinimumSize();
}

void Control::SetMaximumSize( const Vector3& size )
{
  GetImplementation().SetMaximumSize( size );
}

const Vector3& Control::GetMaximumSize() const
{
  return GetImplementation().GetMaximumSize();
}

Vector3 Control::GetNaturalSize()
{
  return GetImplementation().GetNaturalSize();
}

float Control::GetHeightForWidth( float width )
{
  return GetImplementation().GetHeightForWidth( width );
}

float Control::GetWidthForHeight( float height )
{
  return GetImplementation().GetWidthForHeight( height );
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

void Control::SetBackground( Image image )
{
  GetImplementation().SetBackground( image );
}

void Control::ClearBackground()
{
  GetImplementation().ClearBackground();
}

Actor Control::GetBackgroundActor() const
{
  return GetImplementation().GetBackgroundActor();
}

Control::KeyEventSignalV2& Control::KeyEventSignal()
{
  return GetImplementation().KeyEventSignal();
}

/**
 * @copydoc ConnectionTrackerInterface::SignalConnected
 */
void Control::SignalConnected( SlotObserver* slotObserver, CallbackBase* callback )
{
  GetImplementation().SignalConnected(slotObserver, callback );
}

/**
 * @copydoc ConnectionTrackerInterface::SignalDisconnected
 */
void Control::SignalDisconnected( SlotObserver* slotObserver, CallbackBase* callback )
{
  GetImplementation().SignalDisconnected(slotObserver, callback );
}

/**
 * @copydoc ConnectionTrackerInterface::GetConnectionCount
 */
std::size_t Control::GetConnectionCount() const
{
  return GetImplementation().GetConnectionCount( );
}


Control::Control(ControlImpl& implementation)
: CustomActor(implementation)
{
}

Control::Control(Dali::Internal::CustomActor* internal)
: CustomActor(internal)
{
  VerifyCustomActorPointer<ControlImpl>(internal);
}

} // namespace Toolkit

} // namespace Dali

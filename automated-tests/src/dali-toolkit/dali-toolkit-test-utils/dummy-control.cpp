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

#include "dummy-control.h"

namespace Dali
{

namespace Toolkit
{

DummyControl::DummyControl()
: mCustomSlot1Called(false)
{
}

DummyControl::DummyControl(const DummyControl& control)
: Control( control ),
  mCustomSlot1Called(false),
  mCustomSlot1Value(Vector3::ZERO)
{
}

DummyControl::~DummyControl()
{
}

DummyControl DummyControl::DownCast( BaseHandle handle )
{
  return Control::DownCast<DummyControl, DummyControlImpl>(handle);
}

DummyControl& DummyControl::operator=(const DummyControl& control)
{
  Control::operator=( control );
  return *this;
}

// Used to test signal connections
void DummyControl::CustomSlot1( Actor actor, const Vector3& value )
{
  mCustomSlot1Called = true;
  mCustomSlot1Value = value;
}

DummyControl DummyControlImpl::New()
{
  IntrusivePtr< DummyControlImpl > impl = new DummyControlImpl;
  DummyControl control( *impl );
  impl->Initialize();
  return control;
}

DummyControlImpl::DummyControlImpl()
: Control(true)
{
}

DummyControlImpl::~DummyControlImpl()
{
}

DummyControl DummyControlImplOverride::New()
{
  IntrusivePtr< DummyControlImplOverride > impl = new DummyControlImplOverride;
  DummyControl control( *impl );
  impl->Initialize();
  return control;
}


DummyControlImplOverride::DummyControlImplOverride()
: DummyControlImpl(),
  initializeCalled(false),
  styleChangeCalled(false),
  pinchCalled(false),
  panCalled(false),
  tapCalled(false),
  longPressCalled(false),
  stageConnectionCalled(false),
  stageDisconnectionCalled(false),
  childAddCalled(false),
  childRemoveCalled(false),
  sizeSetCalled(false),
  sizeAnimationCalled(false),
  touchEventCalled(false),
  mouseWheelEventCalled(false),
  keyEventCalled(false),
  keyInputFocusGained(false),
  keyInputFocusLost(false)
{
}

DummyControlImplOverride::~DummyControlImplOverride() { }


void DummyControlImplOverride::OnInitialize() { initializeCalled = true; }
void DummyControlImplOverride::OnStyleChange(StyleChange change) { styleChangeCalled = true;}
void DummyControlImplOverride::OnPinch(PinchGesture pinch) { pinchCalled = true; }
void DummyControlImplOverride::OnPan(PanGesture pan) { panCalled = true; }
void DummyControlImplOverride::OnTap(TapGesture tap) { tapCalled = true; }
void DummyControlImplOverride::OnLongPress(LongPressGesture longPress) { longPressCalled = true; }
void DummyControlImplOverride::OnStageConnection() { stageConnectionCalled = true; }
void DummyControlImplOverride::OnStageDisconnection() { stageDisconnectionCalled = true; }
void DummyControlImplOverride::OnChildAdd(Actor& child) { childAddCalled = true; }
void DummyControlImplOverride::OnChildRemove(Actor& child) { childRemoveCalled = true; }
void DummyControlImplOverride::OnSizeSet(const Vector3& targetSize) { sizeSetCalled = true; }
void DummyControlImplOverride::OnSizeAnimation(Animation& animation, const Vector3& targetSize) { sizeAnimationCalled = true; }
bool DummyControlImplOverride::OnTouchEvent(const TouchEvent& event) { touchEventCalled = true; return false; }
bool DummyControlImplOverride::OnMouseWheelEvent(const MouseWheelEvent& event) { mouseWheelEventCalled = true; return false; }
bool DummyControlImplOverride::OnKeyEvent(const KeyEvent& event) { keyEventCalled = true; return false;}
void DummyControlImplOverride::OnKeyInputFocusGained() { keyInputFocusGained = true; }
void DummyControlImplOverride::OnKeyInputFocusLost() { keyInputFocusLost = true; }

DummyControl DummyControl::New( bool override )
{
  DummyControl control;

  if (override)
  {
    control = DummyControlImplOverride::New();
  }
  else
  {
    control = DummyControlImpl::New();
  }

  return control;
}

DummyControl::DummyControl( DummyControlImpl& implementation )
: Control( implementation ),
  mCustomSlot1Called(false),
  mCustomSlot1Value(Vector3::ZERO)
{
}

DummyControl::DummyControl( Dali::Internal::CustomActor* internal )
: Control( internal ),
  mCustomSlot1Called(false),
  mCustomSlot1Value(Vector3::ZERO)
{
  VerifyCustomActorPointer<DummyControlImpl>(internal);
}

} // namespace Toolkit

} // namespace Dali

#ifndef __DALI_TOOLKIT_TEST_DUMMY_CONTROL_H__
#define __DALI_TOOLKIT_TEST_DUMMY_CONTROL_H__

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

// INTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>

namespace Dali
{

namespace Toolkit
{

class DummyControlImpl;

/**
 * Control does not have a New method so use this dummy class for the handle.
 */
class DummyControl : public Control
{
public:

  DummyControl()
  : mCustomSlot1Called(false)
  {
  }

  DummyControl(const DummyControl& control)
  : Control( control ),
    mCustomSlot1Called(false)
  {
  }

  virtual ~DummyControl()
  {
  }

  static DummyControl New( bool override = false );

  static DummyControl DownCast( BaseHandle handle )
  {
    return Control::DownCast<DummyControl, DummyControlImpl>(handle);
  }

  DummyControl& operator=(const DummyControl& control)
  {
    Control::operator=( control );
    return *this;
  }

  // Used to test signal connections
  void CustomSlot1( Actor actor, const Vector3& value )
  {
    mCustomSlot1Called = true;
    mCustomSlot1Value = value;
  }

public:

  bool mCustomSlot1Called;
  Vector3 mCustomSlot1Value;

public: // Not intended for application developers

  DummyControl( DummyControlImpl& implementation );
  DummyControl( Dali::Internal::CustomActor* internal );
};

/**
 * Cannot create an instance of ControlImpl, so use this dummy class for the implementation.
 * This class does not override any of ControlImpl's behaviour.
 */
class DummyControlImpl : public ControlImpl
{
public:

  static DummyControl New()
  {
    IntrusivePtr< DummyControlImpl > impl = new DummyControlImpl;
    DummyControl control( *impl );
    impl->Initialize();
    return control;
  }

public:
  void EnableGestureDetection(Gesture::Type type) { ControlImpl::EnableGestureDetection(type); }
  void DisableGestureDetection(Gesture::Type type) { ControlImpl::DisableGestureDetection(type); }
  PinchGestureDetector GetPinchGestureDetector() const { return ControlImpl::GetPinchGestureDetector(); }
  PanGestureDetector GetPanGestureDetector() const { return ControlImpl::GetPanGestureDetector(); }
  TapGestureDetector GetTapGestureDetector() const { return ControlImpl::GetTapGestureDetector(); }
  LongPressGestureDetector GetLongPressGestureDetector() const { return ControlImpl::GetLongPressGestureDetector(); }

protected:

  DummyControlImpl()
  : ControlImpl(true)
  {
  }

  virtual ~DummyControlImpl()
  {
  }
};

/**
 * Cannot create an instance of ControlImpl, so use this dummy class for the implementation.
 * This class DOES override ControlImpl's behaviour.
 */
class DummyControlImplOverride : public DummyControlImpl
{
public:

  static DummyControl New()
  {
    IntrusivePtr< DummyControlImplOverride > impl = new DummyControlImplOverride;
    DummyControl control( *impl );
    impl->Initialize();
    return control;
  }

private:

  DummyControlImplOverride()
  : DummyControlImpl(),
    initializeCalled(false),
    themeChangeCalled(false),
    fontChangeCalled( false ),
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

  virtual ~DummyControlImplOverride() { }

private: // From ControlImpl

  virtual void OnInitialize() { initializeCalled = true; }
  virtual void OnThemeChange( StyleManager styleManager ) { themeChangeCalled = true }
  virtual void OnFontChange( bool defaultFontChange, bool defaultFontSizeChange ) { fontChangeCalled = true;}
  virtual void OnPinch(PinchGesture pinch) { pinchCalled = true; }
  virtual void OnPan(PanGesture pan) { panCalled = true; }
  virtual void OnTap(TapGesture tap) { tapCalled = true; }
  virtual void OnLongPress(LongPressGesture longPress) { longPressCalled = true; }

private: // From CustomActorImpl

  virtual void OnStageConnection() { stageConnectionCalled = true; }
  virtual void OnStageDisconnection() { stageDisconnectionCalled = true; }
  virtual void OnChildAdd(Actor& child) { childAddCalled = true; }
  virtual void OnChildRemove(Actor& child) { childRemoveCalled = true; }
  virtual void OnSizeSet(const Vector3& targetSize) { sizeSetCalled = true; }
  virtual void OnSizeAnimation(Animation& animation, const Vector3& targetSize) { sizeAnimationCalled = true; }
  virtual bool OnTouchEvent(const TouchEvent& event) { touchEventCalled = true; return false; }
  virtual bool OnMouseWheelEvent(const MouseWheelEvent& event) { mouseWheelEventCalled = true; return false; }
  virtual bool OnKeyEvent(const KeyEvent& event) { keyEventCalled = true; return false;}
  virtual void OnKeyInputFocusGained() { keyInputFocusGained = true; }
  virtual void OnKeyInputFocusLost() { keyInputFocusLost = true; }

public:

  bool initializeCalled;
  bool themeChangeCalled;
  bool fontChangeCalled;
  bool pinchCalled;
  bool panCalled;
  bool tapCalled;
  bool longPressCalled;
  bool stageConnectionCalled;
  bool stageDisconnectionCalled;
  bool childAddCalled;
  bool childRemoveCalled;
  bool sizeSetCalled;
  bool sizeAnimationCalled;
  bool touchEventCalled;
  bool mouseWheelEventCalled;
  bool keyEventCalled;
  bool keyInputFocusGained;
  bool keyInputFocusLost;
};

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
: Control( implementation )
{
}

DummyControl::DummyControl( Dali::Internal::CustomActor* internal )
: Control( internal )
{
  VerifyCustomActorPointer<DummyControlImpl>(internal);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEST_DUMMY_CONTROL_H__

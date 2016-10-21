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
class ControlRenderer;
/**
 * Control does not have a New method so use this dummy class for the handle.
 */
class DummyControl : public Control
{
public:

  DummyControl();
  DummyControl(const DummyControl& control);
  ~DummyControl();

  static DummyControl New( bool override = false );

  static DummyControl DownCast( BaseHandle handle );


  DummyControl& operator=(const DummyControl& control);

public: // Not intended for application developers

  DummyControl( DummyControlImpl& implementation );
  DummyControl( Dali::Internal::CustomActor* internal );
};

/**
 * Cannot create an instance of Internal::Control, so use this dummy class for the implementation.
 * This class does not override any of Internal::Control's behaviour.
 */
class DummyControlImpl : public Internal::Control
{
public:

  static DummyControl New();

public:
  inline void EnableGestureDetection(Gesture::Type type) { Internal::Control::EnableGestureDetection(type); }
  inline void DisableGestureDetection(Gesture::Type type) { Internal::Control::DisableGestureDetection(type); }
  inline PinchGestureDetector GetPinchGestureDetector() const { return Internal::Control::GetPinchGestureDetector(); }
  inline PanGestureDetector GetPanGestureDetector() const { return Internal::Control::GetPanGestureDetector(); }
  inline TapGestureDetector GetTapGestureDetector() const { return Internal::Control::GetTapGestureDetector(); }
  inline LongPressGestureDetector GetLongPressGestureDetector() const { return Internal::Control::GetLongPressGestureDetector(); }

  void RegisterVisual( Property::Index index, Actor placementActor, Toolkit::Visual::Base visual);
  void RegisterVisual( Property::Index index, Actor placementActor, Toolkit::Visual::Base visual, bool enabled );
  void UnregisterVisual( Property::Index index );
  void EnableVisual( Property::Index index, bool enabled );
  bool IsVisualEnabled( Property::Index index );

  Toolkit::Visual::Base GetVisual( Property::Index index );
  Actor GetPlacementActor( Property::Index index );

  // Used to test signal connections
  void CustomSlot1( Actor actor );

  bool mCustomSlot1Called;

protected:

  DummyControlImpl();

  virtual ~DummyControlImpl();
};

/**
 * Cannot create an instance of Internal::Control, so use this dummy class for the implementation.
 * This class DOES override Internal::Control's behaviour.
 */
class DummyControlImplOverride : public DummyControlImpl
{
public:

  static DummyControl New();

private:

  DummyControlImplOverride();

  virtual ~DummyControlImplOverride();

private: // From Internal::Control

  virtual void OnInitialize();
  virtual bool OnAccessibilityActivated();
  virtual bool OnAccessibilityTouch( const TouchEvent& touchEvent );
  virtual bool OnAccessibilityValueChange( bool isIncrease );

  virtual void OnStyleChange( Toolkit::StyleManager styleManager, StyleChange::Type change );
  virtual void OnPinch(const PinchGesture& pinch);
  virtual void OnPan(const PanGesture& pan);
  virtual void OnTap(const TapGesture& tap);
  virtual void OnLongPress(const LongPressGesture& longPress);

private: // From CustomActorImpl

  virtual void OnStageConnection( int depth );
  virtual void OnStageDisconnection();
  virtual void OnChildAdd(Actor& child);
  virtual void OnChildRemove(Actor& child);
  virtual void OnSizeSet(const Vector3& targetSize);
  virtual void OnSizeAnimation(Animation& animation, const Vector3& targetSize);
  virtual bool OnTouchEvent(const TouchEvent& event);
  virtual bool OnHoverEvent(const HoverEvent& event);
  virtual bool OnWheelEvent(const WheelEvent& event);
  virtual bool OnKeyEvent(const KeyEvent& event);
  virtual void OnKeyInputFocusGained();
  virtual void OnKeyInputFocusLost();

public:

  bool initializeCalled;
  bool activatedCalled;
  bool onAccTouchedCalled;
  bool onAccValueChangeCalled;
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
  bool hoverEventCalled;
  bool wheelEventCalled;
  bool keyEventCalled;
  bool keyInputFocusGained;
  bool keyInputFocusLost;
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEST_DUMMY_CONTROL_H__

#ifndef DALI_TOOLKIT_TEST_DUMMY_CONTROL_H
#define DALI_TOOLKIT_TEST_DUMMY_CONTROL_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <functional>

namespace Dali
{

namespace Toolkit
{

class DummyControlImpl;
class TransitionData;

namespace Visual
{
class Base;
}

/**
 * Control does not have a New method so use this dummy class for the handle.
 */
class DummyControl : public Control
{
public:
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1, ///< @SINCE_1_0.0
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000              ///< Reserve property index
  };

  struct Property
  {
    enum Type
    {
      TEST_VISUAL = PROPERTY_START_INDEX,
      TEST_VISUAL2,
      FOREGROUND_VISUAL,
      FOCUS_VISUAL,
      LABEL_VISUAL
    };
  };

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
class DummyControlImpl : public Toolkit::Internal::Control
{
public:

  static DummyControl New();

public:
  inline void EnableGestureDetection(GestureType::Value type) { Internal::Control::EnableGestureDetection(type); }
  inline void DisableGestureDetection(GestureType::Value type) { Internal::Control::DisableGestureDetection(type); }
  inline PinchGestureDetector GetPinchGestureDetector() const { return Internal::Control::GetPinchGestureDetector(); }
  inline PanGestureDetector GetPanGestureDetector() const { return Internal::Control::GetPanGestureDetector(); }
  inline TapGestureDetector GetTapGestureDetector() const { return Internal::Control::GetTapGestureDetector(); }
  inline LongPressGestureDetector GetLongPressGestureDetector() const { return Internal::Control::GetLongPressGestureDetector(); }

  void RegisterVisual( Property::Index index, Toolkit::Visual::Base visual);
  void RegisterVisual( Property::Index index, Toolkit::Visual::Base visual, bool enabled );
  void UnregisterVisual( Property::Index index );
  void EnableVisual( Property::Index index, bool enabled );
  bool IsVisualEnabled( Property::Index index );
  int GetVisualCount();
  Toolkit::Visual::Base GetVisual( Property::Index index );
  Animation CreateTransition( const Toolkit::TransitionData& transition );
  void DoAction( Dali::Property::Index index, Dali::Property::Index action, const Dali::Property::Value attributes );

  static void SetProperty( BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value );

  static Property::Value GetProperty( BaseObject* object, Dali::Property::Index propertyIndex );

  // Used to test signal connections
  void CustomSlot1( Actor actor );

  bool mCustomSlot1Called;
  typedef std::vector<Property::Index> VisualIndices;
  VisualIndices mRegisteredVisualIndices;

protected:

  DummyControlImpl();

  virtual ~DummyControlImpl();
};

namespace Impl
{

/**
 * Cannot create an instance of Internal::Control, so use this dummy class for the implementation.
 * This class DOES override Internal::Control's behaviour.
 */
class DummyControl : public Toolkit::DummyControlImpl
{
public:

  typedef std::function<void( Size )> RelayoutCallbackFunc;

  static Toolkit::DummyControl New();

  void SetLayout( Property::Index visualIndex, Property::Map& map );

  void SetRelayoutCallback( RelayoutCallbackFunc callback );

private:

  DummyControl();

  virtual ~DummyControl();

private: // From Internal::Control

  void OnInitialize() override;
  bool OnAccessibilityActivated() override;
  bool OnAccessibilityValueChange( bool isIncrease ) override;

  void OnStyleChange( Toolkit::StyleManager styleManager, StyleChange::Type change ) override;
  void OnPinch(const PinchGesture& pinch) override;
  void OnPan(const PanGesture& pan) override;
  void OnTap(const TapGesture& tap) override;
  void OnLongPress(const LongPressGesture& longPress) override;
  Vector3 GetNaturalSize() override;
  bool OnKeyEvent(const KeyEvent& event) override;

private: // From CustomActorImpl

  void OnSceneConnection( int depth ) override;
  void OnSceneDisconnection() override;
  void OnChildAdd(Actor& child) override;
  void OnChildRemove(Actor& child) override;
  void OnSizeSet(const Vector3& targetSize) override;
  void OnSizeAnimation(Animation& animation, const Vector3& targetSize) override;
  void OnKeyInputFocusGained() override;
  void OnKeyInputFocusLost() override;
  void OnRelayout( const Vector2& size, RelayoutContainer& container ) override;

  DALI_INTERNAL DummyControl( const DummyControl& );
  DALI_INTERNAL DummyControl& operator=( const DummyControl& );

public:

  bool initializeCalled;
  bool activatedCalled;
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
  bool hoverEventCalled;
  bool wheelEventCalled;
  bool keyEventCalled;
  bool keyInputFocusGained;
  bool keyInputFocusLost;
  static int constructorCount;
  static int destructorCount;

  Property::Map mLayouts;
  RelayoutCallbackFunc mRelayoutCallback;

};

} // namespace Impl

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEST_DUMMY_CONTROL_H

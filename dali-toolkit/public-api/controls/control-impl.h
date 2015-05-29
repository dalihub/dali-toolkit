#ifndef __DALI_TOOLKIT_CONTROL_IMPL_H__
#define __DALI_TOOLKIT_CONTROL_IMPL_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/style-change.h>
#include <dali/public-api/events/long-press-gesture.h>
#include <dali/public-api/events/pan-gesture.h>
#include <dali/public-api/events/pinch-gesture.h>
#include <dali/public-api/events/tap-gesture.h>
#include <dali/public-api/object/property-index-ranges.h>
#include <dali/public-api/object/type-info.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{

namespace Toolkit
{
class StyleManager;

namespace Internal
{
class FocusManager;
class KeyboardFocusManager;
class KeyInputFocusManager;

/**
 * @brief This is the internal base class for all controls.
 *
 * It will provide some common functionality required by all controls.
 * Implements ConnectionTrackerInterface so that signals (typically connected to member functions) will
 * be disconnected automatically when the control is destroyed.
 */
class DALI_IMPORT_API Control : public CustomActorImpl, public ConnectionTrackerInterface
{
public:

  class Extension; ///< Forward declare future extension interface

  // Creation & Destruction

  /**
   * @brief Create a new ControlImpl instance that does not require touch by default.
   *
   * If touch is required then the user can connect to this class' touch signal.
   * @return A handle to the ControlImpl instance.
   */
  static Toolkit::Control New();

  /**
   * @brief Virtual destructor.
   */
  virtual ~Control();

  // Styling

  /**
   * @copydoc Dali::Toolkit::Control::SetStyleName
   */
  void SetStyleName( const std::string& styleName );

  /**
   * @copydoc Dali::Toolkit::Control::GetStyleName
   */
  const std::string& GetStyleName() const;

  // Background

  /**
   * @copydoc Dali::Toolkit::Control::SetBackgroundColor
   */
  void SetBackgroundColor( const Vector4& color );

  /**
   * @copydoc Dali::Toolkit::Control::GetBackgroundColor
   */
  Vector4 GetBackgroundColor() const;

  /**
   * @copydoc Dali::Toolkit::Control::SetBackgroundImage
   */
  void SetBackgroundImage( Image image );

  /**
   * @copydoc Dali::Toolkit::Control::ClearBackground
   */
  void ClearBackground();

  // Gesture Detection

  /**
   * @brief Allows deriving classes to enable any of the gesture detectors that are available.
   *
   * Gesture detection can be enabled one at a time or in bitwise format as shown:
   * @code
   * EnableGestureDetection(Gesture::Type(Gesture::Pinch | Gesture::Tap | Gesture::Pan));
   * @endcode
   * @param[in]  type  The gesture type(s) to enable.
   */
  void EnableGestureDetection( Gesture::Type type );

  /**
   * @brief Allows deriving classes to disable any of the gesture detectors.
   *
   * Like EnableGestureDetection, this can also be called using bitwise or.
   * @param[in]  type  The gesture type(s) to disable.
   * @see EnableGetureDetection
   */
  void DisableGestureDetection( Gesture::Type type );

  /**
   * @brief If deriving classes wish to fine tune pinch gesture
   * detection then they can access the gesture detector through this
   * API and modify the detection.
   *
   * @return The pinch gesture detector.
   * @pre Pinch detection should have been enabled via EnableGestureDetection().
   * @see EnableGestureDetection
   */
  PinchGestureDetector GetPinchGestureDetector() const;

  /**
   * @brief If deriving classes wish to fine tune pan gesture
   * detection then they can access the gesture detector through this
   * API and modify the detection.
   *
   * @return The pan gesture detector.
   * @pre Pan detection should have been enabled via EnableGestureDetection().
   * @see EnableGestureDetection
   */
  PanGestureDetector GetPanGestureDetector() const;

  /**
   * @brief If deriving classes wish to fine tune tap gesture
   * detection then they can access the gesture detector through this
   * API and modify the detection.
   *
   * @return The tap gesture detector.
   * @pre Tap detection should have been enabled via EnableGestureDetection().
   * @see EnableGestureDetection
   */
  TapGestureDetector GetTapGestureDetector() const;

  /**
   * @brief If deriving classes wish to fine tune long press gesture
   * detection then they can access the gesture detector through this
   * API and modify the detection.
   *
   * @return The long press gesture detector.
   * @pre Long press detection should have been enabled via EnableGestureDetection().
   * @see EnableGestureDetection
   */
  LongPressGestureDetector GetLongPressGestureDetector() const;

  // Keyboard Navigation

  /**
   * @brief Sets whether this control supports two dimensional
   * keyboard navigation (i.e. whether it knows how to handle the
   * keyboard focus movement between its child actors).
   *
   * The control doesn't support it by default.
   * @param[in] isSupported Whether this control supports two dimensional keyboard navigation.
   */
  void SetKeyboardNavigationSupport( bool isSupported );

  /**
   * @brief Gets whether this control supports two dimensional keyboard navigation.
   *
   * @return true if this control supports two dimensional keyboard navigation.
   */
  bool IsKeyboardNavigationSupported();

  // Key Input

  /**
   * @copydoc Toolkit::Control::SetKeyInputFocus()
   */
  void SetKeyInputFocus();

  /**
   * @copydoc Toolkit::Control::HasKeyInputFocus()
   */
  bool HasKeyInputFocus();

  /**
   * @copydoc Toolkit::Control::ClearKeyInputFocus()
   */
  void ClearKeyInputFocus();

  // Keyboard Focus

  /**
   * @brief Sets whether this control is a focus group for keyboard navigation.
   *
   * (i.e. the scope of keyboard focus movement
   * can be limitied to its child actors). The control is not a focus group by default.
   * @param[in] isFocusGroup Whether this control is set as a focus group for keyboard navigation.
   */
  void SetAsKeyboardFocusGroup( bool isFocusGroup );

  /**
   * @brief Gets whether this control is a focus group for keyboard navigation.
   *
   * @return true if this control is set as a focus group for keyboard navigation.
   */
  bool IsKeyboardFocusGroup();

  // Called by Focus Manager

  /**
   * @brief Called by the focus manager and keyboard focus manager to Activate the Control
   */
  DALI_INTERNAL void AccessibilityActivate();

  // Signals

  /**
   * @copydoc Dali::Toolkit::Control::KeyEventSignal()
   */
  Toolkit::Control::KeyEventSignalType& KeyEventSignal();

  /**
   * @copydoc Dali::Toolkit::Control::KeyInputFocusGainedSignal()
   */
  Toolkit::Control::KeyInputFocusSignalType& KeyInputFocusGainedSignal();

  /**
   * @copydoc Dali::Toolkit::Control::KeyInputFocusLostSignal()
   */
  Toolkit::Control::KeyInputFocusSignalType& KeyInputFocusLostSignal();

  /**
   * @brief Called by the KeyInputFocusManager to emit key event signals.
   *
   * @param[in] event The key event.
   * @return True if the event was consumed.
   */
  DALI_INTERNAL bool EmitKeyEventSignal( const KeyEvent& event );

protected: // For derived classes to call

  /**
   * @brief Emits KeyInputFocusGained signal if true else emits KeyInputFocusLost signal
   *
   * Should be called last by the control after it acts on the Input Focus change.
   *
   * @param[in] focusGained True if gained, False if lost
   */
  void EmitKeyInputFocusSignal( bool focusGained );

protected: // From CustomActorImpl, not to be used by application developers

  /**
   * @copydoc CustomActorImpl::OnStageConnection()
   */
  virtual void OnStageConnection();

  /**
   * @copydoc CustomActorImpl::OnStageDisconnection()
   */
  virtual void OnStageDisconnection();

  /**
   * @copydoc CustomActorImpl::OnChildAdd()
   */
  virtual void OnChildAdd( Actor& child );

  /**
   * @copydoc CustomActorImpl::OnChildRemove()
   */
  virtual void OnChildRemove( Actor& child );

  /**
   * @copydoc CustomActorImpl::OnSizeSet()
   */
  virtual void OnSizeSet( const Vector3& targetSize );

  /**
   * @copydoc CustomActorImpl::OnSizeAnimation()
   */
  virtual void OnSizeAnimation( Animation& animation, const Vector3& targetSize );

  /**
   * @copydoc CustomActorImpl::OnTouchEvent()
   */
  virtual bool OnTouchEvent( const TouchEvent& event );

  /**
   * @copydoc CustomActorImpl::OnHoverEvent()
   */
  virtual bool OnHoverEvent( const HoverEvent& event );

  /**
   * @copydoc CustomActorImpl::OnKeyEvent()
   */
  virtual bool OnKeyEvent( const KeyEvent& event );

  /**
   * @copydoc CustomActorImpl::OnWheelEvent()
   */
  virtual bool OnWheelEvent( const WheelEvent& event );

  /**
   * @copydoc CustomActorImpl::OnRelayout()
   */
  virtual void OnRelayout( const Vector2& size, RelayoutContainer& container );

  /**
   * @copydoc CustomActorImpl::OnSetResizePolicy()
   */
  virtual void OnSetResizePolicy( ResizePolicy::Type policy, Dimension::Type dimension );

  /**
   * @copydoc CustomActorImpl::GetNaturalSize()
   */
  virtual Vector3 GetNaturalSize();

  /**
   * @copydoc CustomActorImpl::CalculateChildSize()
   */
  virtual float CalculateChildSize( const Dali::Actor& child, Dimension::Type dimension );

  /**
   * @copydoc CustomActorImpl::GetHeightForWidth()
   */
  virtual float GetHeightForWidth( float width );

  /**
   * @copydoc CustomActorImpl::GetWidthForHeight()
   */
  virtual float GetWidthForHeight( float height );

  /**
   * @copydoc CustomActorImpl::RelayoutDependentOnChildren()
   */
  virtual bool RelayoutDependentOnChildren( Dimension::Type dimension = Dimension::ALL_DIMENSIONS );

  /**
   * @copydoc CustomActorImpl::OnCalculateRelayoutSize()
   */
  virtual void OnCalculateRelayoutSize( Dimension::Type dimension );

  /**
   * @copydoc CustomActorImpl::OnLayoutNegotiated()
   */
  virtual void OnLayoutNegotiated( float size, Dimension::Type dimension );

protected: // Helpers for deriving classes

  // Construction

  // Flags for the constructor
  enum ControlBehaviour
  {
    REQUIRES_STYLE_CHANGE_SIGNALS        = 1 << ( CustomActorImpl::ACTOR_FLAG_COUNT + 0 ),     ///< True if needs to monitor style change signals such as theme/font change
    REQUIRES_KEYBOARD_NAVIGATION_SUPPORT = 1 << ( CustomActorImpl::ACTOR_FLAG_COUNT + 1 ),     ///< True if needs to support keyboard navigation

    LAST_CONTROL_BEHAVIOUR_FLAG
  };

  static const int CONTROL_BEHAVIOUR_FLAG_COUNT = Log< LAST_CONTROL_BEHAVIOUR_FLAG - 1 >::value + 1;      ///< Total count of flags

  /**
   * @brief Control constructor
   *
   * @param[in] behaviourFlags Behavioural flags from ControlBehaviour enum
   */
  Control( ControlBehaviour behaviourFlags );

  /**
   * @brief Second phase initialization.
   */
  void Initialize();

public: // API for derived classes to override

  // Lifecycle

  /**
   * @brief This method is called after the Control has been initialized.
   *
   * Derived classes should do any second phase initialization by overriding this method.
   */
  virtual void OnInitialize();

  /**
   * @brief Called whenever an Actor is added to the control.
   *
   * Could be overridden by derived classes.
   *
   * @param[in] child The added actor.
   */
  virtual void OnControlChildAdd( Actor& child );

  /**
   * @brief Called whenever an Actor is removed from the control.
   *
   * Could be overridden by derived classes.
   *
   * @param[in] child The removed actor.
   */
  virtual void OnControlChildRemove( Actor& child );

  // Styling

  /**
   * @brief This method should be overridden by deriving classes requiring notifications when the style changes.
   *
   * @param[in] styleManager  The StyleManager object.
   * @param[in] change  Information denoting what has changed.
   */
  virtual void OnStyleChange( Toolkit::StyleManager styleManager, StyleChange::Type change );

  // Accessibility

  /**
   * @brief This method is called when the control is accessibility activated.
   *
   * Derived classes should override this to perform custom accessibility activation.
   * @return true if this control can perform accessibility activation.
   */
  virtual bool OnAccessibilityActivated();

  /**
   * @brief This method should be overridden by deriving classes when they wish to respond the accessibility
   * pan gesture.
   *
   * @param[in] gesture The pan gesture.
   * @return true if the pan gesture has been consumed by this control
   */
  virtual bool OnAccessibilityPan( PanGesture gesture );

  /**
   * @brief This method should be overridden by deriving classes when they wish to respond the accessibility
   * touch event.
   *
   * @param[in] touchEvent The touch event.
   * @return true if the touch event has been consumed by this control
   */
  virtual bool OnAccessibilityTouch( const TouchEvent& touchEvent );

  /**
   * @brief This method should be overridden by deriving classes when they wish to respond
   * the accessibility up and down action (i.e. value change of slider control).
   *
   * @param[in] isIncrease Whether the value should be increased or decreased
   * @return true if the value changed action has been consumed by this control
   */
  virtual bool OnAccessibilityValueChange( bool isIncrease );

  // Keyboard focus

  /**
   * @brief Called when the control gains key input focus.
   *
   * Should be overridden by derived classes if they need to customize what happens when focus is gained.
   */
  virtual void OnKeyInputFocusGained();

  /**
   * @brief Called when the control loses key input focus.
   *
   * Should be overridden by derived classes if they need to customize what happens when focus is lost.
   */
  virtual void OnKeyInputFocusLost();

  /**
   * @brief Gets the next keyboard focusable actor in this control towards the given direction.
   *
   * A control needs to override this function in order to support two dimensional keyboard navigation.
   * @param[in] currentFocusedActor The current focused actor.
   * @param[in] direction The direction to move the focus towards.
   * @param[in] loopEnabled Whether the focus movement should be looped within the control.
   * @return the next keyboard focusable actor in this control or an empty handle if no actor can be focused.
   */
  virtual Actor GetNextKeyboardFocusableActor( Actor currentFocusedActor, Toolkit::Control::KeyboardFocusNavigationDirection direction, bool loopEnabled );

  /**
   * @brief Informs this control that its chosen focusable actor will be focused.
   *
   * This allows the application to preform any actions if wishes
   * before the focus is actually moved to the chosen actor.
   *
   * @param[in] commitedFocusableActor The commited focusable actor.
   */
  virtual void OnKeyboardFocusChangeCommitted( Actor commitedFocusableActor );

  // Gestures

  /**
   * @brief Called whenever a pinch gesture is detected on this control.
   *
   * This can be overridden by deriving classes when pinch detection
   * is enabled.  The default behaviour is to scale the control by the
   * pinch scale.
   *
   * @note If overridden, then the default behaviour will not occur.
   * @note Pinch detection should be enabled via EnableGestureDetection().
   * @param[in]  pinch  The pinch gesture.
   * @see EnableGestureDetection
   */
  virtual void OnPinch( const PinchGesture& pinch );

  /**
   * @brief Called whenever a pan gesture is detected on this control.
   *
   * This should be overridden by deriving classes when pan detection
   * is enabled.
   *
   * @note There is no default behaviour with panning.
   * @note Pan detection should be enabled via EnableGestureDetection().
   * @param[in]  pan  The pan gesture.
   * @see EnableGestureDetection
   */
  virtual void OnPan( const PanGesture& pan );

  /**
   * @brief Called whenever a tap gesture is detected on this control.
   *
   * This should be overridden by deriving classes when tap detection
   * is enabled.
   *
   * @note There is no default behaviour with a tap.
   * @note Tap detection should be enabled via EnableGestureDetection().
   * @param[in]  tap  The tap gesture.
   * @see EnableGestureDetection
   */
  virtual void OnTap( const TapGesture& tap );

  /**
   * @brief Called whenever a long press gesture is detected on this control.
   *
   * This should be overridden by deriving classes when long press
   * detection is enabled.
   *
   * @note There is no default behaviour associated with a long press.
   * @note Long press detection should be enabled via EnableGestureDetection().
   * @param[in]  longPress  The long press gesture.
   * @see EnableGestureDetection
   */
  virtual void OnLongPress( const LongPressGesture& longPress );

  // From ConnectionTrackerInterface

  /**
   * @copydoc ConnectionTrackerInterface::SignalConnected
   */
  virtual void SignalConnected( SlotObserver* slotObserver, CallbackBase* callback );

  /**
   * @copydoc ConnectionTrackerInterface::SignalDisconnected
   */
  virtual void SignalDisconnected( SlotObserver* slotObserver, CallbackBase* callback );

  /**
   * Retrieve the extension for this control
   *
   * @return The extension if available, NULL otherwise
   */
  virtual Extension* GetControlExtension()
  {
    return NULL;
  }

private:

  // Undefined
  DALI_INTERNAL Control( const Control& );
  DALI_INTERNAL Control& operator=( const Control& );

  class Impl;
  Impl* mImpl;

};

/**
 * @brief Get implementation from the handle
 *
 * @pre handle is initialized and points to a control
 * @param handle
 * @return implementation
 */
Internal::Control& GetImplementation( Dali::Toolkit::Control& handle );

/**
 * @brief Get implementation from the handle
 *
 * @pre handle is initialized and points to a control
 * @param handle
 * @return implementation
 */
const Internal::Control& GetImplementation( const Dali::Toolkit::Control& handle );

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_CONTROL_IMPL_H__

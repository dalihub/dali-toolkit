#ifndef DALI_TOOLKIT_CONTROL_IMPL_H
#define DALI_TOOLKIT_CONTROL_IMPL_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
/**
 * @addtogroup dali_toolkit_controls
 * @{
 */

class StyleManager;

namespace Visual
{
class Base;
}
namespace Internal
{
/**
 * @brief This is the internal base class for all controls.
 *
 * It will provide some common functionality required by all controls.
 * Implements ConnectionTrackerInterface so that signals (typically connected to member functions) will
 * be disconnected automatically when the control is destroyed.
 * @SINCE_1_0.0
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
   * @SINCE_1_0.0
   * @return A handle to the ControlImpl instance.
   */
  static Toolkit::Control New();

  /**
   * @brief Virtual destructor.
   * @SINCE_1_0.0
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
   * @brief Set the background with a property map.
   *
   * @SINCE_1_0.0
   * @param[in] map The background property map.
   */
  void SetBackground(const Property::Map& map);

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
   * @SINCE_1_0.0
   * @param[in]  type  The gesture type(s) to enable.
   */
  void EnableGestureDetection( Gesture::Type type );

  /**
   * @brief Allows deriving classes to disable any of the gesture detectors.
   *
   * Like EnableGestureDetection, this can also be called using bitwise or.
   * @SINCE_1_0.0
   * @param[in]  type  The gesture type(s) to disable.
   * @see EnableGetureDetection
   */
  void DisableGestureDetection( Gesture::Type type );

  /**
   * @brief If deriving classes wish to fine tune pinch gesture
   * detection then they can access the gesture detector through this
   * API and modify the detection.
   *
   * @SINCE_1_0.0
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
   * @SINCE_1_0.0
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
   * @SINCE_1_0.0
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
   * @SINCE_1_0.0
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
   * @SINCE_1_0.0
   * @param[in] isSupported Whether this control supports two dimensional keyboard navigation.
   */
  void SetKeyboardNavigationSupport( bool isSupported );

  /**
   * @brief Gets whether this control supports two dimensional keyboard navigation.
   *
   * @SINCE_1_0.0
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
   * @SINCE_1_0.0
   * @param[in] isFocusGroup Whether this control is set as a focus group for keyboard navigation.
   */
  void SetAsKeyboardFocusGroup( bool isFocusGroup );

  /**
   * @brief Gets whether this control is a focus group for keyboard navigation.
   *
   * @SINCE_1_0.0
   * @return true if this control is set as a focus group for keyboard navigation.
   */
  bool IsKeyboardFocusGroup();

  /// @cond internal
  /**
   * @brief Called by the AccessibilityManager to activate the Control.
   * @SINCE_1_0.0
   */
  DALI_INTERNAL void AccessibilityActivate();

  /**
   * @brief Called by the KeyboardFocusManager.
   * @SINCE_1_0.0
   */
  DALI_INTERNAL void KeyboardEnter();
  /// @endcond

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

  /// @cond internal
  /**
   * @brief Called by the KeyInputFocusManager to emit key event signals.
   *
   * @SINCE_1_0.0
   * @param[in] event The key event.
   * @return True if the event was consumed.
   */
  DALI_INTERNAL bool EmitKeyEventSignal( const KeyEvent& event );
  /// @endcond

protected: // For derived classes to call

  /**
   * @brief Register a visual by Property Index, linking an Actor to controlRenderer when required.
   * In the case of the visual being an actor or control deeming controlRenderer not required then controlRenderer should be an empty handle.
   * No parenting is done during registration, this should be done by derived class.
   *
   * @SINCE_1_2.0
   *
   * @param[in] index The Property index of the visual, used to reference visual
   * @param[in] placementActor The actor used to by the visual.
   * @param[in] visual The visual to register
   * @note Derived class must NOT call visual.SetOnStage(placementActor). It is the responsibility of the base class to connect/disconnect registered visual to stage.
   */
   void RegisterVisual( Property::Index index, Actor& placementActor, Toolkit::Visual::Base& visual );

   /**
    * @brief Erase the entry matching the given index from the list of registered visuals
    * @param[in] index The Property index of the visual, used to reference visual
    *
    * @SINCE_1_2.0
    */
   void UnregisterVisual( Property::Index index );

   /**
    * @brief Retrieve the visual associated with the given property index.
    *
    * @SINCE_1_2.2
    *
    * @param[in] index The Property index of the visual.
    * @return The registered visual if exist, otherwise empty handle.
    * @note For managing object life-cycle, do not store the returned visual as a member which increments its reference count.
    */
   Toolkit::Visual::Base GetVisual( Property::Index index ) const;

   /**
    * @brief Retrieve the placement actor associated with the given index.
    *
    * @SINCE_1_2.2
    *
    * @@param[in] index The Property index of the visual.
    * @return Then placement actor if exist, otherwise empty handle.
    * @note For managing object life-cycle, do not store the returned placement actor as a member which increments its reference count.
    */
   Actor GetPlacementActor( Property::Index index ) const;

  /**
   * @brief Emits KeyInputFocusGained signal if true else emits KeyInputFocusLost signal
   *
   * Should be called last by the control after it acts on the Input Focus change.
   *
   * @SINCE_1_0.0
   * @param[in] focusGained True if gained, False if lost
   */
  void EmitKeyInputFocusSignal( bool focusGained );

protected: // From CustomActorImpl, not to be used by application developers

  /**
   * @copydoc CustomActorImpl::OnStageConnection()
   * @note If overridden, then an up-call to Control::OnStageConnection MUST be made at the start.
   */
  virtual void OnStageConnection( int depth );

  /**
   * @copydoc CustomActorImpl::OnStageDisconnection()
   * @note If overridden, then an up-call to Control::OnStageDisconnection MUST be made at the end.
   */
  virtual void OnStageDisconnection();

  /**
   * @copydoc CustomActorImpl::OnChildAdd()
   * @note If overridden, then an up-call to Control::OnChildAdd MUST be made at the start.
   */
  virtual void OnChildAdd( Actor& child );

  /**
   * @copydoc CustomActorImpl::OnChildRemove()
   * @note If overridden, then an up-call to Control::OnChildRemove MUST be made at the end.
   */
  virtual void OnChildRemove( Actor& child );

  /**
   * @copydoc CustomActorImpl::OnSizeSet()
   * @note If overridden, then an up-call to Control::OnSizeSet MUST be made at the start.
   */
  virtual void OnSizeSet( const Vector3& targetSize );

  /**
   * @copydoc CustomActorImpl::OnSizeAnimation()
   * @note If overridden, then an up-call to Control::OnSizeAnimation MUST be made at the start.
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

  /**
   * @brief Flags for the constructor
   * @SINCE_1_0.0
   */
  enum ControlBehaviour
  {
    REQUIRES_STYLE_CHANGE_SIGNALS        = 1 << ( CustomActorImpl::ACTOR_FLAG_COUNT + 0 ),     ///< True if needs to monitor style change signals such as theme/font change @SINCE_1_0.0
    REQUIRES_KEYBOARD_NAVIGATION_SUPPORT = 1 << ( CustomActorImpl::ACTOR_FLAG_COUNT + 1 ),     ///< True if needs to support keyboard navigation @SINCE_1_0.0

    LAST_CONTROL_BEHAVIOUR_FLAG
  };

  static const int CONTROL_BEHAVIOUR_FLAG_COUNT = Log< LAST_CONTROL_BEHAVIOUR_FLAG - 1 >::value + 1;      ///< Total count of flags

  /**
   * @brief Control constructor
   *
   * @SINCE_1_0.0
   * @param[in] behaviourFlags Behavioural flags from ControlBehaviour enum
   */
  Control( ControlBehaviour behaviourFlags );

  /**
   * @brief Second phase initialization.
   * @SINCE_1_0.0
   */
  void Initialize();

public: // API for derived classes to override

  // Lifecycle

  /**
   * @brief This method is called after the Control has been initialized.
   *
   * Derived classes should do any second phase initialization by overriding this method.
   * @SINCE_1_0.0
   */
  virtual void OnInitialize();

  /**
   * @DEPRECATED_1_1.30. Override OnChildAdd instead.
   *
   * @brief Called whenever an Actor is added to the control.
   *
   * Could be overridden by derived classes.
   *
   * @SINCE_1_0.0
   * @param[in] child The added actor.
   */
  virtual void OnControlChildAdd( Actor& child );

  /**
   * @DEPRECATED_1_1.30. Override OnChildRemove instead.
   *
   * @brief Called whenever an Actor is removed from the control.
   *
   * Could be overridden by derived classes.
   *
   * @SINCE_1_0.0
   * @param[in] child The removed actor.
   */
  virtual void OnControlChildRemove( Actor& child );

  // Styling

  /**
   * @brief This method should be overridden by deriving classes requiring notifications when the style changes.
   *
   * @SINCE_1_0.0
   * @param[in] styleManager  The StyleManager object.
   * @param[in] change  Information denoting what has changed.
   */
  virtual void OnStyleChange( Toolkit::StyleManager styleManager, StyleChange::Type change );

  // Accessibility

  /**
   * @brief This method is called when the control is accessibility activated.
   *
   * Derived classes should override this to perform custom accessibility activation.
   * @SINCE_1_0.0
   * @return true if this control can perform accessibility activation.
   */
  virtual bool OnAccessibilityActivated();

  /**
   * @brief This method should be overridden by deriving classes when they wish to respond the accessibility
   * pan gesture.
   *
   * @SINCE_1_0.0
   * @param[in] gesture The pan gesture.
   * @return true if the pan gesture has been consumed by this control
   */
  virtual bool OnAccessibilityPan( PanGesture gesture );

  /**
   * @brief This method should be overridden by deriving classes when they wish to respond the accessibility
   * touch event.
   *
   * @SINCE_1_0.0
   * @param[in] touchEvent The touch event.
   * @return true if the touch event has been consumed by this control
   */
  virtual bool OnAccessibilityTouch( const TouchEvent& touchEvent );

  /**
   * @brief This method should be overridden by deriving classes when they wish to respond
   * the accessibility up and down action (i.e. value change of slider control).
   *
   * @SINCE_1_0.0
   * @param[in] isIncrease Whether the value should be increased or decreased
   * @return true if the value changed action has been consumed by this control
   */
  virtual bool OnAccessibilityValueChange( bool isIncrease );

  /**
   * @brief This method should be overridden by deriving classes when they wish to respond
   * the accessibility zoom action.
   *
   * @SINCE_1_0.0
   * @return true if the zoom action has been consumed by this control
   */
  virtual bool OnAccessibilityZoom();

  // Keyboard focus

  /**
   * @brief Called when the control gains key input focus.
   *
   * Should be overridden by derived classes if they need to customize what happens when focus is gained.
   * @SINCE_1_0.0
   */
  virtual void OnKeyInputFocusGained();

  /**
   * @brief Called when the control loses key input focus.
   *
   * Should be overridden by derived classes if they need to customize what happens when focus is lost.
   * @SINCE_1_0.0
   */
  virtual void OnKeyInputFocusLost();

  /**
   * @brief Gets the next keyboard focusable actor in this control towards the given direction.
   *
   * A control needs to override this function in order to support two dimensional keyboard navigation.
   * @SINCE_1_0.0
   * @param[in] currentFocusedActor The current focused actor.
   * @param[in] direction The direction to move the focus towards.
   * @param[in] loopEnabled Whether the focus movement should be looped within the control.
   * @return the next keyboard focusable actor in this control or an empty handle if no actor can be focused.
   */
  virtual Actor GetNextKeyboardFocusableActor( Actor currentFocusedActor, Toolkit::Control::KeyboardFocus::Direction direction, bool loopEnabled );

  /**
   * @brief Informs this control that its chosen focusable actor will be focused.
   *
   * This allows the application to preform any actions if wishes
   * before the focus is actually moved to the chosen actor.
   *
   * @SINCE_1_0.0
   * @param[in] commitedFocusableActor The commited focusable actor.
   */
  virtual void OnKeyboardFocusChangeCommitted( Actor commitedFocusableActor );

  /**
   * @brief This method is called when the control has enter pressed on it.
   *
   * Derived classes should override this to perform custom actions.
   * @SINCE_1_0.0
   * @return true if this control supported this action.
   */
  virtual bool OnKeyboardEnter();

  // Gestures

  /**
   * @brief Called whenever a pinch gesture is detected on this control.
   *
   * This can be overridden by deriving classes when pinch detection
   * is enabled.  The default behaviour is to scale the control by the
   * pinch scale.
   *
   * @SINCE_1_0.0
   * @param[in]  pinch  The pinch gesture.
   * @note If overridden, then the default behaviour will not occur.
   * @note Pinch detection should be enabled via EnableGestureDetection().
   * @see EnableGestureDetection
   */
  virtual void OnPinch( const PinchGesture& pinch );

  /**
   * @brief Called whenever a pan gesture is detected on this control.
   *
   * This should be overridden by deriving classes when pan detection
   * is enabled.
   *
   * @SINCE_1_0.0
   * @param[in]  pan  The pan gesture.
   * @note There is no default behaviour with panning.
   * @note Pan detection should be enabled via EnableGestureDetection().
   * @see EnableGestureDetection
   */
  virtual void OnPan( const PanGesture& pan );

  /**
   * @brief Called whenever a tap gesture is detected on this control.
   *
   * This should be overridden by deriving classes when tap detection
   * is enabled.
   *
   * @SINCE_1_0.0
   * @param[in]  tap  The tap gesture.
   * @note There is no default behaviour with a tap.
   * @note Tap detection should be enabled via EnableGestureDetection().
   * @see EnableGestureDetection
   */
  virtual void OnTap( const TapGesture& tap );

  /**
   * @brief Called whenever a long press gesture is detected on this control.
   *
   * This should be overridden by deriving classes when long press
   * detection is enabled.
   *
   * @SINCE_1_0.0
   * @param[in]  longPress  The long press gesture.
   * @note There is no default behaviour associated with a long press.
   * @note Long press detection should be enabled via EnableGestureDetection().
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
   * @brief Retrieve the extension for this control
   *
   * @SINCE_1_0.0
   * @return The extension if available, NULL otherwise
   */
  virtual Extension* GetControlExtension()
  {
    return NULL;
  }

private:

  /// @cond internal
  // Undefined
  DALI_INTERNAL Control( const Control& );
  DALI_INTERNAL Control& operator=( const Control& );

  class Impl;
  Impl* mImpl;
  /// @endcond

};

/**
 * @brief Get implementation from the handle.
 *
 * @SINCE_1_0.0
 * @param handle
 * @return implementation
 * @pre handle is initialized and points to a control
 */
DALI_IMPORT_API Internal::Control& GetImplementation( Dali::Toolkit::Control& handle );

/**
 * @brief Get implementation from the handle.
 *
 * @SINCE_1_0.0
 * @param handle
 * @return implementation
 * @pre handle is initialized and points to a control
 */
DALI_IMPORT_API const Internal::Control& GetImplementation( const Dali::Toolkit::Control& handle );

} // namespace Internal

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_CONTROL_IMPL_H

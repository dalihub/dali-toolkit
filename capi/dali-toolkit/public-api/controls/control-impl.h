#ifndef __DALI_TOOLKIT_CONTROL_IMPL_H__
#define __DALI_TOOLKIT_CONTROL_IMPL_H__

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

/**
 * @addtogroup CAPI_DALI_TOOLKIT_CONTROLS_MODULE
 * @{
 */

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

class StyleManager;

namespace Internal DALI_INTERNAL
{
class RelayoutControllerImpl;
class KeyInputFocusManager;
}

typedef std::pair< Actor, Vector2 > ActorSizePair;       ///< Pair of actor and size
typedef std::vector< ActorSizePair > ActorSizeContainer; ///< Container of actors and their sizes

namespace Internal
{

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

  // Properties
  enum
  {
    CONTROL_PROPERTY_START_INDEX = PROPERTY_REGISTRATION_START_INDEX,
    CONTROL_PROPERTY_END_INDEX = CONTROL_PROPERTY_START_INDEX + 1000 ///< Reserving 1000 property indices
  };

  // Creation

  /**
   * @brief Create a new ControlImpl instance that does not require touch by default.
   *
   * If touch is required then the user can connect to this class' touch signal.
   * @return A handle to the ConntrolImpl instance.
   */
  static Toolkit::Control New();

  // Destruction

  /**
   * @brief Virtual destructor.
   */
  virtual ~Control();

  // Actions

  /**
   * @brief This method should be overridden by deriving classes when they wish to be notified when they
   * are activated.
   */
  virtual void OnActivated() { }

  /**
   * @brief This method should be overridden by deriving classes when they wish to respond the accessibility
   * pan gesture.
   *
   * @param[in] gesture The pan gesture.
   * @return true if the pan gesture has been consumed by this control
   */
  virtual bool OnAccessibilityPan(PanGesture gesture);

  /**
   * @brief This method should be overridden by deriving classes when they wish to respond the accessibility
   * touch event.
   *
   * @param[in] touchEvent The touch event.
   * @return true if the touch event has been consumed by this control
   */
  virtual bool OnAccessibilityTouch(const TouchEvent& touchEvent);

  /**
   * @brief This method should be overridden by deriving classes when they wish to respond
   * the accessibility up and down action (i.e. value change of slider control).
   *
   * @param[in] isIncrease Whether the value should be increased or decreased
   * @return true if the value changed action has been consumed by this control
   */
  virtual bool OnAccessibilityValueChange(bool isIncrease);

  /**
   * @brief Sets whether this control supports two dimensional
   * keyboard navigation (i.e. whether it knows how to handle the
   * keyboardn focus movement between its child actors).
   *
   * The control doesn't support it by default.
   * @param[in] isSupported Whether this control supports two dimensional keyboard navigation.
   */
  void SetKeyboardNavigationSupport(bool isSupported);

  /**
   * @brief Gets whether this control supports two dimensional keyboard navigation.
   *
   * @return true if this control supports two dimensional keyboard navigation.
   */
  bool IsKeyboardNavigationSupported();

  /**
   * @brief Sets whether this control is a focus group for keyboard navigation.
   *
   * (i.e. the scope of keyboard focus movement
   * can be limitied to its child actors). The control is not a focus group by default.
   * @param[in] isFocusGroup Whether this control is set as a focus group for keyboard navigation.
   */
  void SetAsKeyboardFocusGroup(bool isFocusGroup);

  /**
   * @brief Gets whether this control is a focus group for keyboard navigation.
   *
   * @return true if this control is set as a focus group for keyboard navigation.
   */
  bool IsKeyboardFocusGroup();

  /**
   * @brief Gets the next keyboard focusable actor in this control towards the given direction.
   *
   * A control needs to override this function in order to support two dimensional keyboard navigation.
   * @param[in] currentFocusedActor The current focused actor.
   * @param[in] direction The direction to move the focus towards.
   * @param[in] loopEnabled Whether the focus movement should be looped within the control.
   * @return the next keyboard focusable actor in this control or an empty handle if no actor can be focused.
   */
  virtual Actor GetNextKeyboardFocusableActor(Actor currentFocusedActor, Toolkit::Control::KeyboardFocusNavigationDirection direction, bool loopEnabled);

  /**
   * @brief Informs this control that its chosen focusable actor will be focused.
   *
   * This allows the application to preform any actions if wishes
   * before the focus is actually moved to the chosen actor.
   *
   * @param[in] commitedFocusableActor The commited focusable actor.
   */
  virtual void OnKeyboardFocusChangeCommitted(Actor commitedFocusableActor) { }

  /**
   * @brief Performs actions as requested using the action name.
   *
   * @param[in] object The object on which to perform the action.
   * @param[in] actionName The action to perform.
   * @param[in] attributes The attributes with which to perfrom this action.
   * @return true if action has been accepted by this control
   */
  static bool DoAction(BaseObject* object, const std::string& actionName, const std::vector<Property::Value>& attributes);

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
   * @copydoc Dali::Toolkit::Control::SetBackground
   */
  void SetBackground( Image image );

  /**
   * @copydoc Dali::Toolkit::Control::ClearBackground
   */
  void ClearBackground();

  /**
   * @copydoc Dali::Toolkit::Control::GetBackgroundActor
   */
  Actor GetBackgroundActor() const;

public:

  /**
   * @copydoc Dali::Toolkit::Control::KeyEventSignal()
   */
  Toolkit::Control::KeyEventSignalV2& KeyEventSignal();

protected:

  // Construction

  /**
   * @brief Second phase initialization.
   */
  void Initialize();

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
  void EnableGestureDetection(Gesture::Type type);

  /**
   * @brief Allows deriving classes to disable any of the gesture detectors.
   *
   * Like EnableGestureDetection, this can also be called using bitwise or.
   * @param[in]  type  The gesture type(s) to disable.
   * @see EnableGetureDetection
   */
  void DisableGestureDetection(Gesture::Type type);

private: // For derived classes to override

  /**
   * @brief This method is called after the Control has been initialized.
   *
   * Derived classes should do any second phase initialization by
   * overriding this method.
   */
  virtual void OnInitialize() { }

  /**
   * @brief This method should be overridden by deriving classes when
   * they wish to be notified when the style manager changes the theme.
   *
   * @param[in] styleManager  The StyleManager object.
   */
  virtual void OnThemeChange( Toolkit::StyleManager styleManager );

  /**
   * @brief This method should be overridden by deriving classes when
   * they wish to be notified when the style changes the default font.
   *
   * @param[in] defaultFontChange  Information denoting whether the default font has changed.
   * @param[in] defaultFontSizeChange Information denoting whether the default font size has changed.
   */
  virtual void OnFontChange( bool defaultFontChange, bool defaultFontSizeChange ){ }

  /**
   * @deprecated Use OnFontChange() instead.
   * Before the using of StyleManager, the StyleChange only deals with font change.
   *
   * @brief This method should be overridden by deriving classes when
   * they wish to be notified when the style changes.
   *
   * @param[in] change  Information denoting what has changed.
   */
  virtual void OnStyleChange( StyleChange change ) { }

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
  virtual void OnPinch(PinchGesture pinch);

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
  virtual void OnPan(PanGesture pan) { }

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
  virtual void OnTap(TapGesture tap) { }

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
  virtual void OnLongPress(LongPressGesture longPress) { }

  /**
   * @brief Called whenever the control is added to the stage.
   *
   * Could be overridden by derived classes.
   */
  virtual void OnControlStageConnection() { }

  /**
   * @brief Called whenever the control is removed from the stage.
   *
   * Could be overridden by derived classes.
   */
  virtual void OnControlStageDisconnection() { }

  /**
   * @brief Called whenever an Actor is added to the control.
   *
   * Could be overridden by derived classes.
   *
   * @param[in] child The added actor.
   */
  virtual void OnControlChildAdd( Actor& child ) { }

  /**
   * @brief Called whenever an Actor is removed from the control.
   *
   * Could be overridden by derived classes.
   *
   * @param[in] child The removed actor.
   */
  virtual void OnControlChildRemove( Actor& child ) { }

  /**
   * @brief Called whenever the Control's size is set.
   *
   * Could be overridden by derived classes.
   *
   * @param[in] size The new size.
   */
  virtual void OnControlSizeSet( const Vector3& size ) { }

  /**
   * @brief Called after the Dali::Stage::SignalMessageQueueFlushed()
   * signal is emitted if this control requested to be relaid-out.
   *
   * Should be overridden by derived classes if they need to layout
   * actors differently after certain operations like add or remove
   * actors, resize or after changing especific properties.
   *
   * @param[in]      size       The allocated size.
   * @param[in,out]  container  The control should add actors to this container that it is not able
   *                            to allocate a size for.
   */
  virtual void OnRelaidOut( Vector2 size, ActorSizeContainer& container );

private: // From CustomActorImpl, derived classes can override these.

  /**
   * @brief Sends a request to relayout this control.
   *
   * The control will be relaid out after the
   * Dali::Stage::SignalMessageQueueFlushed() signal is emitted.
   *
   * It calls OnControlStageConnection() to notify derived classes.
   *
   * @see Dali::CustomActorImpl::OnStageConnection()
   */
  virtual void OnStageConnection();

  /**
   * @brief Calls OnControlStageDisconnection() to notify derived classed.
   *
   * @see Dali::CustomActorImpl::OnStageDisconnection()
   */
  virtual void OnStageDisconnection();

  /**
   * @brief Sends a request to relayout this control.
   *
   * The control will be relaid out after the
   * Dali::Stage::SignalMessageQueueFlushed() signal is emitted.  It
   * calls OnControlChildAdd() to notify derived classes.
   *
   * @note This method shouldn't be overridden by derived classes.
   *
   * @param[in] child The added actor.
   *
   * @see Dali::CustomActorImpl::OnChildAdd(Actor&)
   */
  virtual void OnChildAdd(Actor& child);

  /**
   * @brief Sends a request to relayout this control.
   *
   * The control will be relaid out after the
   * Dali::Stage::SignalMessageQueueFlushed() signal is emitted.  It
   * calls OnControlChildRemove() to notify derived classes.
   *
   * @note This method shouldn't be overridden by derived classes.
   *
   * @param[in] child The removed actor.
   *
   * @see Dali::CustomActorImpl::OnChildRemove(Actor&)
   */
  virtual void OnChildRemove(Actor& child);

  /**
   * @brief It stores the size set by size negotiation and relayout.
   *
   * It also keeps a backup of the size set through the Actor's API used in the size negotiation.
   * It calls the OnControlSizeSet() to notify derived classes.
   *
   * @param[in] targetSize The new size.
   *
   * @see Dali::CustomActorImpl::OnSizeSet(const Vector3&)
   */
  virtual void OnSizeSet(const Vector3& targetSize);

  /**
   * @copydoc Dali::CustomActorImpl::OnSizeAnimation(Animation&, const Vector3&)
   */
  virtual void OnSizeAnimation(Animation& animation, const Vector3& targetSize);

  /**
   * @copydoc Dali::CustomActorImpl::OnTouchEvent(const TouchEvent&)
   */
  virtual bool OnTouchEvent(const TouchEvent& event);

  /**
   * @copydoc Dali::CustomActorImpl::OnKeyEvent(const KeyEvent&)
   */
  virtual bool OnKeyEvent(const KeyEvent& event);

  /**
   * @copydoc Dali::CustomActorImpl::OnMouseWheelEvent(const MouseWheelEvent&)
   */
  virtual bool OnMouseWheelEvent(const MouseWheelEvent& event);

  /**
   * @copydoc Dali::CustomActorImpl::OnKeyInputFocusGained()
   */
  virtual void OnKeyInputFocusGained();

  /**
   * @copydoc Dali::CustomActorImpl::OnKeyInputFocusLost()
   */
  virtual void OnKeyInputFocusLost();

  /**
   * @copydoc Dali::CustomActorImpl::GetChildByAlias(const std::string& actorAlias)
   */
  virtual Actor GetChildByAlias(const std::string& actorAlias);

private:

  /**
   * @brief Perform the activated action.
   *
   * @param[in] attributes The attributes to perfrom this action.
   */
  void DoActivatedAction(const PropertyValueContainer& attributes);

  /**
   * @brief This method is the callback for the StyleChangeSignal from StyleManager
   *
   * @param[in] styleManager The StyleManager Object
   * @param[in] change  Information denoting what has changed.
   */
  void DoStyleChange( Toolkit::StyleManager styleManager, StyleChange change );

protected: // Construction

  // Flags for the constructor
  enum ControlBehaviour
  {
    CONTROL_BEHAVIOUR_NONE         = 0x0,
    REQUIRES_TOUCH_EVENTS          = 0x1,     ///< True if the OnTouchEvent() callback is required.
    REQUIRES_STYLE_CHANGE_SIGNALS  = 0x2      ///< True if needs to monitor style change signals such as theme/font change
  };

  /**
   * @deprecated Use the constructor taking flags instead
   * @brief Create a Control.
   *
   * @param[in] requiresTouchEvents True if the OnTouchEvent() callback is required.
   */
  Control(bool requiresTouchEvents);

  /**
   * @brief Create a Control.
   *
   * @param[in] behaviourFlags Behavioural flags from ControlBehaviour enum
   */
  Control(ControlBehaviour behaviourFlags);

public:

  // Size negotiation

  /**
   * @copydoc Toolkit::Control::SetSizePolicy()
   */
  void SetSizePolicy( Toolkit::Control::SizePolicy widthPolicy, Toolkit::Control::SizePolicy heightPolicy );

  /**
   * @copydoc Toolkit::Control::GetSizePolicy()
   */
  void GetSizePolicy( Toolkit::Control::SizePolicy& widthPolicy, Toolkit::Control::SizePolicy& heightPolicy ) const;

  /**
   * @copydoc Toolkit::Control::SetMinimumSize()
   */
  void SetMinimumSize( const Vector3& size );

  /**
   * @copydoc Toolkit::Control::GetMinimumSize()
   */
  const Vector3& GetMinimumSize() const;

  /**
   * @copydoc Toolkit::Control::SetMaximumSize()
   */
  void SetMaximumSize( const Vector3& size );

  /**
   * @copydoc Toolkit::Control::GetMaximumSize()
   */
  const Vector3& GetMaximumSize() const;

  /**
   * @copydoc Toolkit::Control::GetNaturalSize()
   */
  virtual Vector3 GetNaturalSize();

  /**
   * @copydoc Toolkit::Control::GetHeightForWidth()
   */
  virtual float GetHeightForWidth( float width );

  /**
   * @copydoc Toolkit::Control::GetWidthForHeight()
   */
  virtual float GetWidthForHeight( float height );

  /**
   * @brief Retrieves the current Control's size.
   *
   * @return The control's size.
   */
  const Vector3& GetControlSize() const;

  /**
   * @brief Retrieves the Control's size set by the Application / Control.
   *
   * @return The control's size.
   */
  const Vector3& GetSizeSet() const;

  //KeyInput

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

  /**
   * @copydoc ConnectionTrackerInterface::SignalConnected
   */
  virtual void SignalConnected( SlotObserver* slotObserver, CallbackBase* callback );

  /**
   * @copydoc ConnectionTrackerInterface::SignalDisconnected
   */
  virtual void SignalDisconnected( SlotObserver* slotObserver, CallbackBase* callback );

  /**
   * @copydoc ConnectionTrackerInterface::GetConnectionCount
   */
  virtual std::size_t GetConnectionCount() const;

protected:

  /**
   * @brief Sends a request to be relaid-out.
   *
   * This method is called from OnStageConnection(), OnChildAdd(),
   * OnChildRemove(), SetSizePolicy(), SetMinimumSize() and
   * SetMaximumSize().
   *
   * This method could also be called from derived classes every time
   * a control's poperty change and it needs to be relaid-out.  After
   * the Dali::Stage::SignalMessageQueueFlushed() is emitted a
   * relayout process starts and all controls which called this method
   * will be relaid-out.
   *
   * @note RelayoutRequest() only sends a request per Control before
   * the Dali::Stage::SignalMessageQueueFlushed() signal is
   * emitted. That means a control will be relaid-out only once, even
   * if more than one request is sent between two consecutive signals.
   */
  void RelayoutRequest();

  /**
   * @brief Helper method for controls to Relayout their children if
   * they do not know whether that child is a control or not.
   *
   * @param[in]      actor      The actor to relayout.
   * @param[in]      size       The size to allocate to the actor.
   * @param[in,out]  container  The container that holds actors that have not been allocated a size yet.
   */
  static void Relayout( Actor actor, Vector2 size, ActorSizeContainer& container );

private: // Used by the RelayoutController

  /**
   * @brief Called by the RelayoutController to negotiate the size of a control.
   *
   * The size allocated by the the algorithm is passed in which the
   * control must adhere to.  A container is passed in as well which
   * the control should populate with actors it has not / or does not
   * need to handle in its size negotiation.
   *
   * @param[in]      size       The allocated size.
   * @param[in,out]  container  The container that holds actors that are fed back into the
   *                            RelayoutController algorithm.
   */
  void NegotiateSize( Vector2 size, ActorSizeContainer& container );

private:

  /**
   * @brief Called by NegotiateSize when the size to allocate to the control has been calculated.
   *
   * It calls the OnRelaidOut() method which can be overridden by derived classes.
   *
   * @param[in]      size       The allocated size.
   * @param[in,out]  container  The control should add actors to this container that it is not able
   *                            to allocate a size for.
   */
  void Relayout( Vector2 size, ActorSizeContainer& container );

  /**
   * @brief Used by the KeyInputFocusManager to emit key event signals.
   *
   * @param[in] event The key event.
   * @return True if the event was consumed.
   */
  bool EmitKeyEventSignal(const KeyEvent& event);



private:

  // Undefined
  Control(const Control&);
  Control& operator=(const Control&);

  class Impl;
  Impl *mImpl;

  friend class Internal::RelayoutControllerImpl;   ///< Relayout controller needs to call Relayout() which is private.
  friend class Internal::KeyInputFocusManager;     ///< KeyInputFocusManager needs to call which is private.
};

} // namespace Internal

typedef Internal::Control ControlImpl; ///< @deprecated, Use Internal::Control

} // namespace Toolkit

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TOOLKIT_CONTROL_IMPL_H__

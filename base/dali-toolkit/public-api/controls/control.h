#ifndef __DALI_TOOLKIT_CONTROL_H__
#define __DALI_TOOLKIT_CONTROL_H__

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
#include <dali/dali.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

//Forward declarations.

namespace Internal
{
class Control;
}

/**
 * @brief Control is the base class for all controls.
 *
 * The implementation of the control must be supplied; see Internal::Control for more details.
 * @see Internal::Control
 */
class Control : public CustomActor
{
public:

  /// @name Properties
  /** @{ */
  static const Property::Index PROPERTY_BACKGROUND_COLOR;    ///< name "background-color",        @see SetBackgroundColor,        type VECTOR4
  static const Property::Index PROPERTY_BACKGROUND;          ///< name "background",              @see SetBackground,             type MAP
  static const Property::Index PROPERTY_WIDTH_POLICY;        ///< name "width-policy",            @see SetSizePolicy,             type STRING
  static const Property::Index PROPERTY_HEIGHT_POLICY;       ///< name "height-policy",           @see SetSizePolicy,             type STRING
  static const Property::Index PROPERTY_MINIMUM_SIZE;        ///< name "minimum-size",            @see SetMinimumSize,            type VECTOR3
  static const Property::Index PROPERTY_MAXIMUM_SIZE;        ///< name "maximum-size",            @see SetMaximumSize,            type VECTOR3
  static const Property::Index PROPERTY_KEY_INPUT_FOCUS;     ///< name "key-input-focus",         @see SetKeyInputFocus,          type BOOLEAN
  /** @} */

  /// @name Signals
  /** @{ */
  static const char* const SIGNAL_KEY_EVENT;                 ///< name "key-event"
  static const char* const SIGNAL_TAPPED;                    ///< name "tapped"
  static const char* const SIGNAL_PANNED;                    ///< name "panned"
  static const char* const SIGNAL_PINCHED;                   ///< name "pinched"
  static const char* const SIGNAL_LONG_PRESSED;              ///< name "long-pressed"
  /** @} */

  /// @name Actions
  /** @{ */
  static const char* const ACTION_CONTROL_ACTIVATED;         ///< name "control-activated"
  /** @} */

  /**
   * @brief Describes how a control could be resized.
   */
  enum SizePolicy
  {
    Fixed,    ///< Size can't grow or shrink.
    Minimum,  ///< Size can grow but shrink up to a minimum level.
    Maximum,  ///< Size can shrink but grow up to a maximum value.
    Range,    ///< Size can grow or shrink between a minimum and a maximum values.
    Flexible, ///< Size can grow or shrink with no limits.
  };

  /**
   * @brief Describes what a control should do when a contained actor/control exceeds the boundary of the control.
   */
  enum ExceedPolicy
  {
    Crop,   ///< Control's contents will be cropped.
    Shrink, ///< Control's contents will be shrunk.
    Scroll  ///< Control's contents will be added to a scroll.
  };

  /**
   * @brief Describes the direction to move the keyboard focus towards.
   */
  enum KeyboardFocusNavigationDirection
  {
    Left,   ///< Move keyboard focus towards the left direction
    Right,  ///< Move keyboard focus towards the right direction
    Up,     ///< Move keyboard focus towards the up direction
    Down    ///< Move keyboard focus towards the down direction
  };

  // Typedefs

  /// @brief Key Event signal type;
  typedef SignalV2<bool ( Control, const KeyEvent& ) > KeyEventSignalV2;

public: // Creation & Destruction

  /**
   * @brief Create a new instance of a Control.
   *
   * @return A handle to a new Control.
   */
  static Control New();

  /**
   * @brief Create an uninitialized Control handle.
   *
   * Only derived versions can be instantiated.  Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   */
  Control();

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object
   * @param[in] uiControl Handle to copy
   */
  Control(const Control& uiControl);

  /**
   * @brief Dali::Control is intended as a base class
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Control();

public: // operators

  /**
   * @brief Assignment operator.
   *
   * Changes this handle to point to another real object
   * @param[in] handle Object to assign this to
   * @return reference to this
   */
  Control& operator=( const Control& handle );

public:

  /**
   * @brief Downcast an Object handle to Control.
   *
   * If handle points to a Control the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return handle to a Control or an uninitialized handle
   */
  static Control DownCast( BaseHandle handle );

  /**
   * @brief Retrieve the Control implementation.
   *
   * @return The implementation.
   */
  Internal::Control& GetImplementation();

  /**
   * @brief Retrieve the Control implementation.
   *
   * @return The implementation.
   */
  const Internal::Control& GetImplementation() const;

  // Size Negotiation

  /**
   * @brief Sets the size policies for the width and height dimensions.
   *
   * @param[in] widthPolicy Size policy for the width dimension.
   * @param[in] heightPolicy Size policy for the height dimension.
   */
  void SetSizePolicy( SizePolicy widthPolicy, SizePolicy heightPolicy );

  /**
   * @brief Retrieves the size policies for the width and height dimensions.
   *
   * @param[out] widthPolicy Width's size policy.
   * @param[out] heightPolicy Height's size policy.
   */
  void GetSizePolicy( SizePolicy& widthPolicy, SizePolicy& heightPolicy ) const;

  /**
   * @brief Sets the minimum size for the control.
   *
   * @param[in] size The minimum size.
   */
  void SetMinimumSize( const Vector3& size );

  /**
   * @brief Retrieves the minimum size.
   *
   * @return The minimum size.
   */
  const Vector3& GetMinimumSize() const;

  /**
   * @brief Sets the maximum size.
   *
   * @param[in] size The maximum size.
   */
  void SetMaximumSize( const Vector3& size );

  /**
   * @brief Retrieves the maximum size.
   *
   * @return The maximum size.
   */
  const Vector3& GetMaximumSize() const;

  /**
   * @brief Works out the natural size.
   *
   * Natural size is the control's size with any restriction.
   *
   * @return The natural size.
   */
  Vector3 GetNaturalSize();

  /**
   * @brief Works out the control's height for a given width.
   *
   * @param[in] width The control's width.
   *
   * @return The control's height for the given width.
   */
  float GetHeightForWidth( float width );

  /**
   * @brief Works out the control's width for a given height.
   *
   * @param[in] height The control's height.
   *
   * @return The control's width for the given height.
   */
  float GetWidthForHeight( float height );

  // Key Input

  /**
   * @brief This sets the control to receive key events.
   *
   * The key event can originate from a virtual or physical keyboard.
   * @pre The Control has been initialized.
   * @pre The Control should be on the stage before setting keyboard focus.
   * @return True if the control has foucs, False otherwise.
   */
  void SetKeyInputFocus();

  /**
   * @brief Quries whether the control has key input focus.
   *
   * Note: The control can be set to have the focus and still not receive all the key events if another control has over ridden it.
   * As the key input focus mechanism works like a stack, the top most control receives all the key events, and passes on the
   * unhandled events to the controls below in the stack. A control in the stack will regain key input focus when there are no more
   * controls above it in the focus stack.
   * To query for the conrol which is on top of the focus stack use Dali::Toolkit::KeyInputFocusManager::GetCurrentKeyboardFocusActor()
   * @pre The Control has been initialized.
   * @pre The Control should be on the stage before setting keyboard focus.
   * @return true if this control has keyboard input focus
   */
  bool HasKeyInputFocus();

  /**
   * @brief Once an actor is Set to receive key input focus this function is called to stop it receiving key events.
   *
   * A check is performed to ensure it was previously set, if this check fails then nothing is done.
   * @pre The Actor has been initialized.
   */
  void ClearKeyInputFocus();

  // Gesture Detection

  /**
   * @brief Retrieves the pinch gesture detector of the control.
   *
   * @return The pinch gesture detector.
   * @pre Pinch detection should have been enabled in the control.
   */
  PinchGestureDetector GetPinchGestureDetector() const;

  /**
   * @brief Retrieves the pan gesture detector of the control.
   *
   * @return The pan gesture detector.
   * @pre Pan detection should have been enabled in the control.
   */
  PanGestureDetector GetPanGestureDetector() const;

  /**
   * @brief Retrieves the tap gesture detector of the control.
   *
   * @return The tap gesture detector.
   * @pre Tap detection should have been enabled in the control.
   */
  TapGestureDetector GetTapGestureDetector() const;

  /**
   * @brief Retrieves the long press gesture detector of the control.
   *
   * @return The long press gesture detector.
   * @pre Long press detection should have been enabled in the control.
   */
  LongPressGestureDetector GetLongPressGestureDetector() const;

  // Background

  /**
   * @brief Sets the background color of the control.
   *
   * @param[in] color The required background color of the control
   *
   * @note The background color fully blends with the actor color.
   */
  void SetBackgroundColor( const Vector4& color );

  /**
   * @brief Retrieves the background color of the control.
   *
   * @return The background color of the control.
   */
  Vector4 GetBackgroundColor() const;

  /**
   * @brief Sets an image as the background of the control.
   *
   * The color of this image is blended with the background color @see SetBackgroundColor
   *
   * @param[in] image The image to set as the background.
   */
  void SetBackground( Image image );

  /**
   * @brief Clears the background.
   */
  void ClearBackground();

  /**
   * @brief Retrieves the actor used as the background for this control.
   *
   * @return The actor that used as the background for this control.
   */
  Actor GetBackgroundActor() const;

  // Signals

  /**
   * @brief This signal is emitted when key event is received.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName(Control control, const KeyEvent& event);
   * @endcode
   * The return value of True, indicates that the touch event should be consumed.
   * Otherwise the signal will be emitted on the next sensitive parent of the actor.
   * @pre The Control has been initialized.
   * @return The signal to connect to.
   */
  KeyEventSignalV2& KeyEventSignal();

public: // Not intended for application developers

  /**
   * @brief Create an initialised Control.
   *
   * @param[in] implementation The implementation for this control.
   * @return A handle to a newly allocated Dali resource.
   */
  Control(Internal::Control& implementation);

  /**
   * @brief This constructor is used by CustomActor within Dali core to create additional Control handles
   * using an Internal CustomActor pointer.
   *
   * @param [in] internal A pointer to a newly allocated Dali resource
   */
  Control(Dali::Internal::CustomActor* internal);

public: // Templates for Deriving Classes

  /**
   * @brief Template to allow deriving controls to DownCast handles to deriving handle classes.
   *
   * @tparam     T       The handle class
   * @tparam     I       The implementation class
   * @param[in]  handle  Handle to an object
   * @return Handle to a class T or an uninitialized handle.
   * @see DownCast(BaseHandle)
   */
  template<typename T, typename I>
  static T DownCast( BaseHandle handle )
  {
    T result;

    CustomActor custom = Dali::CustomActor::DownCast( handle );
    if ( custom )
    {
      CustomActorImpl& customImpl = custom.GetImplementation();

      I* impl = dynamic_cast<I*>(&customImpl);

      if (impl)
      {
        result = T(customImpl.GetOwner());
      }
    }

    return result;
  }

  /**
   * @brief Template to allow deriving controls to verify whether the Internal::CustomActor* is actually an
   * implementation of their class.
   *
   * @tparam     I         The implementation class
   * @param[in]  internal  Pointer to the Internal::CustomActor
   */
  template<typename I>
  void VerifyCustomActorPointer(Dali::Internal::CustomActor* internal)
  {
    // Can have a NULL pointer so we only need to check if the internal implementation is our class
    // when there is a value.
    if (internal)
    {
      DALI_ASSERT_DEBUG(dynamic_cast<I*>(&CustomActor(internal).GetImplementation()));
    }
  }

};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_CONTROL_H__

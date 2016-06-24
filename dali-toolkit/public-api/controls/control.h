#ifndef __DALI_TOOLKIT_CONTROL_H__
#define __DALI_TOOLKIT_CONTROL_H__

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
#include <dali/public-api/actors/custom-actor.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/events/long-press-gesture-detector.h>
#include <dali/public-api/events/pan-gesture-detector.h>
#include <dali/public-api/events/pinch-gesture-detector.h>
#include <dali/public-api/events/tap-gesture-detector.h>
#include <dali/public-api/events/tap-gesture-detector.h>
#include <dali/public-api/images/image.h>

namespace Dali
{

namespace Toolkit
{

//Forward declarations.

namespace Internal
{
class Control;
}
/**
 * @addtogroup dali_toolkit_controls
 * @{
 */

/**
 * @brief Control is the base class for all controls.
 *
 * The implementation of the control must be supplied; see Internal::Control for more details.
 * @SINCE_1_0.0
 * @see Internal::Control
 *
 * Signals
 * | %Signal Name           | Method                                              |
 * |------------------------|-----------------------------------------------------|
 * | keyEvent               | @ref KeyEventSignal()                               |
 * | keyInputFocusGained    | @ref KeyInputFocusGainedSignal()                    |
 * | keyInputFocusLost      | @ref KeyInputFocusLostSignal()                      |
 * | tapped                 | @ref GetTapGestureDetector().DetectedSignal()       |
 * | panned                 | @ref GetPanGestureDetector().DetectedSignal()       |
 * | pinched                | @ref GetPinchGestureDetector().DetectedSignal()     |
 * | longPressed            | @ref GetLongPressGestureDetector().DetectedSignal() |
 *
 * Actions
 * | %Action Name           | %Control method called                             |
 * |------------------------|----------------------------------------------------|
 * | accessibilityActivated | %OnAccessibilityActivated()                        |
 */
class DALI_IMPORT_API Control : public CustomActor
{
public:

  /**
   * @brief The start and end property ranges for control.
   * @SINCE_1_0.0
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = PROPERTY_REGISTRATION_START_INDEX,        ///< Start index is used by the property registration macro. @SINCE_1_0.0
    CONTROL_PROPERTY_START_INDEX = PROPERTY_START_INDEX,             ///< Start index of Control properties. @SINCE_1_0.0
    CONTROL_PROPERTY_END_INDEX = CONTROL_PROPERTY_START_INDEX + 1000 ///< Reserving 1000 property indices. @SINCE_1_0.0
  };

  /**
   * @brief An enumeration of properties belonging to the Control class.
   * @SINCE_1_0.0
   */
  struct Property
  {
    enum
    {
      /**
       * @brief name "styleName", type std::string
       * @SINCE_1_0.0
       * @see SetStyleName
       */
      STYLE_NAME = PROPERTY_START_INDEX,
      /**
       * @DEPRECATED_1_1.3
       * @brief name "background-color", mutually exclusive with BACKGROUND_IMAGE & BACKGROUND,  type Vector4
       * @SINCE_1_0.0
       * @see SetStyleName
       */
      BACKGROUND_COLOR,
      /**
       * @DEPRECATED_1_1.3
       * @brief name "background-image", mutually exclusive with BACKGROUND_COLOR & BACKGROUND,  type Map
       * @SINCE_1_0.0
       */
      BACKGROUND_IMAGE,
      /**
       * @brief name "keyInputFocus", type bool
       * @SINCE_1_0.0
       * @see SetKeyInputFocus
       */
      KEY_INPUT_FOCUS,
      /**
       * @brief name "background",       mutually exclusive with BACKGROUND_COLOR & BACKGROUND_IMAGE,                   type Map
       * @SINCE_1_1.3
       */
      BACKGROUND,
    };
  };

  /**
   * @brief Describes the direction to move the keyboard focus towards.
   * @SINCE_1_0.0
   */
  struct KeyboardFocus
  {
    enum Direction
    {
      LEFT,   ///< Move keyboard focus towards the left direction @SINCE_1_0.0
      RIGHT,  ///< Move keyboard focus towards the right direction @SINCE_1_0.0
      UP,     ///< Move keyboard focus towards the up direction @SINCE_1_0.0
      DOWN    ///< Move keyboard focus towards the down direction @SINCE_1_0.0
    };
  };

  // Typedefs

  /// @brief Key Event signal type;
  typedef Signal<bool ( Control, const KeyEvent& ) > KeyEventSignalType;

  /// @brief Key InputFocusType signal type;
  typedef Signal<void ( Control ) > KeyInputFocusSignalType;

public: // Creation & Destruction

  /**
   * @brief Create a new instance of a Control.
   *
   * @SINCE_1_0.0
   * @return A handle to a new Control.
   */
  static Control New();

  /**
   * @brief Create an uninitialized Control handle.
   *
   * Only derived versions can be instantiated.  Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   * @SINCE_1_0.0
   */
  Control();

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object.
   * @SINCE_1_0.0
   * @param[in] uiControl Handle to copy
   */
  Control(const Control& uiControl);

  /**
   * @brief Dali::Control is intended as a base class.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~Control();

public: // operators

  /**
   * @brief Assignment operator.
   *
   * Changes this handle to point to another real object.
   * @SINCE_1_0.0
   * @param[in] handle Object to assign this to
   * @return reference to this
   */
  Control& operator=( const Control& handle );

public:

  /**
   * @brief Downcast a handle to Control handle.
   *
   * If handle points to a Control the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return A handle to a Control or an uninitialized handle
   */
  static Control DownCast( BaseHandle handle );

  // Key Input

  /**
   * @brief This sets the control to receive key events.
   *
   * The key event can originate from a virtual or physical keyboard.
   * @SINCE_1_0.0
   * @pre The Control has been initialized.
   * @pre The Control should be on the stage before setting keyboard focus.
   */
  void SetKeyInputFocus();

  /**
   * @brief Quries whether the control has key input focus.
   *
   * @SINCE_1_0.0
   * @return true if this control has keyboard input focus
   * @pre The Control has been initialized.
   * @pre The Control should be on the stage before setting keyboard focus.
   * @note The control can be set to have the focus and still not receive all the key events if another control has over ridden it.
   * As the key input focus mechanism works like a stack, the top most control receives all the key events, and passes on the
   * unhandled events to the controls below in the stack. A control in the stack will regain key input focus when there are no more
   * controls above it in the focus stack.
   * To query for the conrol which is on top of the focus stack use Dali::Toolkit::KeyInputFocusManager::GetCurrentKeyboardFocusActor().
   */
  bool HasKeyInputFocus();

  /**
   * @brief Once an actor is Set to receive key input focus this function is called to stop it receiving key events.
   *
   * A check is performed to ensure it was previously set, if this check fails then nothing is done.
   * @SINCE_1_0.0
   * @pre The Actor has been initialized.
   */
  void ClearKeyInputFocus();

  // Gesture Detection

  /**
   * @brief Retrieves the pinch gesture detector of the control.
   *
   * @SINCE_1_0.0
   * @return The pinch gesture detector.
   * @note Will return an empty handle if the control does not handle the gesture itself.
   */
  PinchGestureDetector GetPinchGestureDetector() const;

  /**
   * @brief Retrieves the pan gesture detector of the control.
   *
   * @SINCE_1_0.0
   * @return The pan gesture detector.
   * @note Will return an empty handle if the control does not handle the gesture itself.
   */
  PanGestureDetector GetPanGestureDetector() const;

  /**
   * @brief Retrieves the tap gesture detector of the control.
   *
   * @SINCE_1_0.0
   * @return The tap gesture detector.
   * @note Will return an empty handle if the control does not handle the gesture itself.
   */
  TapGestureDetector GetTapGestureDetector() const;

  /**
   * @brief Retrieves the long press gesture detector of the control.
   *
   * @SINCE_1_0.0
   * @return The long press gesture detector.
   * @note Will return an empty handle if the control does not handle the gesture itself.
   */
  LongPressGestureDetector GetLongPressGestureDetector() const;

  // Styling

  /**
   * @brief Sets the name of the style to be applied to the control.
   *
   * @SINCE_1_0.0
   * @param[in] styleName A string matching a style described in a stylesheet.
   */
  void SetStyleName( const std::string& styleName );

  /**
   * @brief Retrieves the name of the style to be applied to the control (if any).
   * @SINCE_1_0.0
   * @return A string matching a style or an empty string.
   */
  const std::string& GetStyleName() const;

  // Background

  /**
   * @brief Sets the background color of the control.
   *
   * @SINCE_1_0.0
   * @param[in] color The required background color of the control
   *
   * @note if SetBackgroundImage is called later, this background color is removed.
   *
   * @note The background color fully blends with the actor color.
   */
  void SetBackgroundColor( const Vector4& color );

  /**
   * @DEPRECATED_1_1.3
   *
   * @brief Retrieves the background color of the control.
   *
   * @SINCE_1_0.0
   * @return The background color of the control.
   */
  Vector4 GetBackgroundColor() const;

  /**
   * @brief Sets an image as the background of the control.
   *
   * @SINCE_1_0.0
   * @param[in] image The image to set as the background.
   */
  void SetBackgroundImage( Image image );

  /**
   * @brief Clears the background.
   * @SINCE_1_0.0
   */
  void ClearBackground();

  // Signals

  /**
   * @brief This signal is emitted when key event is received.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName(Control control, const KeyEvent& event);
   * @endcode
   * The return value of True, indicates that the event should be consumed.
   * Otherwise the signal will be emitted on the next parent of the actor.
   * @SINCE_1_0.0
   * @return The signal to connect to.
   * @pre The Control has been initialized.
   */
  KeyEventSignalType& KeyEventSignal();

  /**
   * @brief This signal is emitted when the control gets Key Input Focus.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName( Control control );
   * @endcode
   * The return value of True, indicates that the event should be consumed.
   * Otherwise the signal will be emitted on the next parent of the actor.
   * @SINCE_1_0.0
   * @return The signal to connect to.
   * @pre The Control has been initialized.
   */
  KeyInputFocusSignalType& KeyInputFocusGainedSignal();

  /**
   * @brief This signal is emitted when the control loses Key Input Focus
   * which could be due to it being gained by another Control or Actor or just cleared from
   * this control as no longer required.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName( Control control );
   * @endcode
   * The return value of True, indicates that the event should be consumed.
   * Otherwise the signal will be emitted on the next parent of the actor.
   * @SINCE_1_0.0
   * @return The signal to connect to.
   * @pre The Control has been initialized.
   */
  KeyInputFocusSignalType& KeyInputFocusLostSignal();

public: // Intended for control developers

  /**
   * @brief Create an initialised Control.
   *
   * @SINCE_1_0.0
   * @param[in] implementation The implementation for this control.
   * @return A handle to a newly allocated Dali resource.
   *
   * @note Should NOT be called to create a handle from the implementation. As stated, this allocates a NEW Dali resource.
   */
  explicit Control(Internal::Control& implementation);

  /**
   * @brief This constructor is used by CustomActor within Dali core to create additional Control handles
   * using an Internal CustomActor pointer.
   *
   * @SINCE_1_0.0
   * @param [in] internal A pointer to a newly allocated Dali resource
   */
  explicit Control(Dali::Internal::CustomActor* internal);

public: // Templates for Deriving Classes

  /**
   * @internal
   * @brief Template to allow deriving controls to DownCast handles to deriving handle classes.
   *
   * @tparam     T       The handle class
   * @tparam     I       The implementation class
   * @SINCE_1_0.0
   * @param[in]  handle  Handle to an object
   * @return Handle to a class T or an uninitialized handle.
   * @see DownCast(BaseHandle)
   */
  template<typename T, typename I>
  DALI_INTERNAL static T DownCast( BaseHandle handle )
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
   * @internal
   * @brief Template to allow deriving controls to verify whether the Internal::CustomActor* is actually an
   * implementation of their class.
   *
   * @tparam     I         The implementation class
   * @SINCE_1_0.0
   * @param[in]  internal  Pointer to the Internal::CustomActor
   */
  template<typename I>
  DALI_INTERNAL void VerifyCustomActorPointer(Dali::Internal::CustomActor* internal)
  {
    // Can have a NULL pointer so we only need to check if the internal implementation is our class
    // when there is a value.
    if (internal)
    {
      DALI_ASSERT_DEBUG(dynamic_cast<I*>(&CustomActor(internal).GetImplementation()));
    }
  }

};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_CONTROL_H__

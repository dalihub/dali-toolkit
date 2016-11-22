#ifndef __DALI_TOOLKIT_POPUP_H__
#define __DALI_TOOLKIT_POPUP_H__

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class Popup;
}

/**
 * @brief The Popup widget provides a configurable pop-up dialog with built-in layout of three main fields.
 *
 * Fields:
 * - Background Image
 *   - Title
 *   - Content
 *   - Footer
 *
 * Please see the programming guide for a detailed description of the Popup including examples.
 *
 * Signals
 * | %Signal Name      | Method                       |
 * |-------------------|------------------------------|
 * | touchedOutside    | @ref OutsideTouchedSignal()  |
 * | showing           | @ref ShowingSignal()         |
 * | shown             | @ref ShownSignal()           |
 * | hiding            | @ref HidingSignal()          |
 * | hidden            | @ref HiddenSignal()          |
 */
class DALI_IMPORT_API Popup : public Control
{

public:

  /**
   * @brief The start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000              ///< Reserve property indices
  };

  /**
   * @brief An enumeration of properties belonging to the Popup class.
   */
  struct Property
  {
    enum
    {
      TITLE = PROPERTY_START_INDEX, ///< name "title",                  type Property::Map
      CONTENT,                      ///< name "content",                type Property::Map
      FOOTER,                       ///< name "footer",                 type Property::Map
      DISPLAY_STATE,                ///< name "displayState",           type std::string
      TOUCH_TRANSPARENT,            ///< name "touchTransparent",       type bool
      TAIL_VISIBILITY,              ///< name "tailVisibility",         type bool
      TAIL_POSITION,                ///< name "tailPosition",           type Vector3
      CONTEXTUAL_MODE,              ///< name "contextualMode",         type std::string
      ANIMATION_DURATION,           ///< name "animationDuration",      type float
      ANIMATION_MODE,               ///< name "animationMode",          type std::string
      ENTRY_ANIMATION,              ///< name "entryAnimation",         type Property::Map
      EXIT_ANIMATION,               ///< name "exitAnimation",          type Property::Map
      AUTO_HIDE_DELAY,              ///< name "autoHideDelay",          type int
      BACKING_ENABLED,              ///< name "backingEnabled",         type bool
      BACKING_COLOR,                ///< name "backingColor",           type Vector4
      POPUP_BACKGROUND_IMAGE,       ///< name "popupBackgroundImage",   type std::string
      POPUP_BACKGROUND_BORDER,      ///< name "popupBackgroundBorder",  type Rect< int >,      Values are in the order: left, right, bottom, top
      TAIL_UP_IMAGE,                ///< name "tailUpImage",            type std::string
      TAIL_DOWN_IMAGE,              ///< name "tailDownImage",          type std::string
      TAIL_LEFT_IMAGE,              ///< name "tailLeftImage",          type std::string
      TAIL_RIGHT_IMAGE,             ///< name "tailRightImage",         type std::string
    };
  };

  /**
   * The display states of the Popup.
   */
  enum DisplayState
  {
    SHOWING,           ///< The popup is transitioning in
    SHOWN,             ///< The popup is fully shown
    HIDING,            ///< The popup is transitioning out
    HIDDEN             ///< The popup is fully hidden
  };

  /**
   * The animation mode within popup.
   * Choose from a predefined mode or "CUSTOM" to use the ANIMATION_IN and ANIMATION_OUT properties.
   */
  enum AnimationMode
  {
    NONE,              ///< No animation.
    ZOOM,              ///< Popup zooms in and out animating the scale property.
    FADE,              ///< Popup fades in and out.
    CUSTOM             ///< Use the EntryAnimation and ExitAnimation animation properties.
  };

  /**
   * Types of contextual layout.
   * The Popup is positioned adjacent to it's parent in the direction specified by this mode.
   * NON_CONTEXTUAL disables any contextual positioning.
   */
  enum ContextualMode
  {
    NON_CONTEXTUAL,
    ABOVE,
    RIGHT,
    BELOW,
    LEFT
  };

public:

  /**
   * @brief Creates an empty Popup handle.
   */
  Popup();

  /**
   * @brief Create the Popup control.
   *
   * @return A handle to the Popup control.
   */
  static Popup New();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Popup();

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object
   * @param[in] handle Handle to the copied object
   */
  Popup( const Popup& handle );

  /**
   * @brief Assignment operator.
   *
   * Changes this handle to point to another real object
   * @param[in] handle Handle to the object
   * @return A reference to this
   */
  Popup& operator=( const Popup& handle );

  /**
   * @brief Downcast an Object handle to Popup.
   *
   * If handle points to a Popup the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a Popup or an uninitialized handle
   */
  static Popup DownCast( BaseHandle handle );

public:

  /**
   * @brief Sets a title for this Popup.
   *
   * @param[in] titleActor Any actor can be specified when using this method.
   */
  void SetTitle( Actor titleActor );

  /**
   * @brief Gets the title actor for this Popup.
   *
   * @return The actor representing the title is returned.
   */
  Actor GetTitle() const;

  /**
   * @brief Sets the content actor.
   * This can any actor type or heirarchy of actors.
   *
   * @param[in] content The actor to use.
   */
  void SetContent( Actor content );

  /**
   * @brief Gets the actor currently used for the content.
   *
   * @return The content actor.
   */
  Actor GetContent() const;

  /**
   * @brief Sets the actor to use for a footer in this Popup.
   *
   * @param[in] footer The footer actor to be added to this Popup
   */
  void SetFooter( Actor footer );

  /**
   * @brief Gets the footer actor.
   *
   * @return The footer actor.
   */
  Actor GetFooter() const;

  /**
   * @brief Sets the display state of Popup.
   *
   * There are 4 total display states.
   * Only 2 can be set, but all four can be read for better inspection of the current popup state.
   *
   * The other two states are getable, but not setable and are there for consistency.
   *
   * | Value    | Setting the state              | Getting the state              |
   * |----------|--------------------------------|--------------------------------|
   * | SHOWN    | Show the popup                 | The popup is fully shown       |
   * | HIDDEN   | Hide the popup                 | The popup is fully hidden      |
   * | SHOWING  |                                | The popup is transitioning in  |
   * | HIDING   |                                | The popup is transitioning out |
   *
   * All 4 state changes cause notifications via 4 respective signals that can be connected to.
   * @see GetDisplayState()
   *
   * @param[in] displayState The desired display state to change to.
   */
  void SetDisplayState( Toolkit::Popup::DisplayState displayState );

  /**
   * @brief Gets the current state of the popup.
   *
   * This will also show if the popup is in the process of showing or hiding.
   *
   * @return The current state of the popup.
   */
  Toolkit::Popup::DisplayState GetDisplayState() const;

public:

  typedef Signal< void () > TouchedOutsideSignalType;     ///< Touched outside signal type.
  typedef Signal< void () > DisplayStateChangeSignalType; ///< Used for signals emitted when the displayed state changes.

  /**
   * @brief Signal emitted when user has touched outside of the Dialog.
   */
  TouchedOutsideSignalType& OutsideTouchedSignal();

  /**
   * @brief Signal emitted when the Popup is starting to be shown.
   */
  DisplayStateChangeSignalType& ShowingSignal();

  /**
   * @brief Signal emitted when the Popup has been fully displayed.
   */
  DisplayStateChangeSignalType& ShownSignal();

  /**
   * @brief Signal emitted when the Popup is starting to be hidden.
   */
  DisplayStateChangeSignalType& HidingSignal();

  /**
   * @brief Signal emitted when the Popup has been completely hidden.
   */
  DisplayStateChangeSignalType& HiddenSignal();

public: // Not intended for application developers

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL Popup( Internal::Popup& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL Popup( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_POPUP_H__

#ifndef __DALI_TOOLKIT_POPUP_H__
#define __DALI_TOOLKIT_POPUP_H__

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
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/text-view/text-view.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class Popup;
}

class Button;

/**
 * Popup contains content that can come into focus when
 * activated, and out of focus when deactivated.
 *
 * Content:
 *
 * The content within a popup consists of:
 *
 * 1. Title
 * 2. Buttons
 * 3. Background/Frame (i.e. Scale-9 image)
 * 4. Custom Content (Actors)
 *
 * All of which are optional.
 *
 * States:
 *
 * A popup can be in a number of states:
 *
 * 1. HIDE (invisible)
 * 2. SHOW (visible at normal size)
 * 3. SHOW_MAXIMIZED (visible occupying full parent size)
 * 4. Or custom defined.
 *
 * Transition Effects:
 *
 * A popup can use various custom transition effects e.g.
 *
 * Alpha fade, Scaling transition, position/rotation, shader effects.
 *
 */
class Popup : public Control
{

public:

  //Signal Names
  static const char* const SIGNAL_TOUCHED_OUTSIDE;
  static const char* const SIGNAL_HIDDEN;

  /**
   * Current popup state
   */
  enum PopupState
  {
    POPUP_NONE,               ///< Init status
    POPUP_HIDE,               ///< Hidden (not visible)
    POPUP_SHOW,               ///< Shown (visible in default size)
  };

  typedef SignalV2< void () > TouchedOutsideSignalV2;
  typedef SignalV2< void () > HiddenSignalV2;

  /**
   * Signal emitted when user has touched outside of the Dialog.
   */
  TouchedOutsideSignalV2& OutsideTouchedSignal();

  /**
   * Signal emitted when popup has been hidden.
   */
  HiddenSignalV2& HiddenSignal();

public:

  /**
   * Creates an empty Popup handle
   */
  Popup();

  /**
   * Copy constructor. Creates another handle that points to the same real object
   */
  Popup( const Popup& handle );

  /**
   * Assignment operator. Changes this handle to point to another real object
   */
  Popup& operator=( const Popup& handle );

  /**
   * Virtual destructor.
   * Dali::Object derived classes typically do not contain member data.
   */
  virtual ~Popup();

  /**
   * Create the Poup control
   * @return A handle to the Popup control.
   */
  static Popup New();

  /**
   * Downcast an Object handle to Popup. If handle points to a Popup the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a Popup or an uninitialized handle
   */
  static Popup DownCast( BaseHandle handle );

public:

  /**
   * Sets the background image for this Popup.
   *
   * The background is resized (stretched according to scale settings),
   * to the size of the Popup.
   *
   * @param[in] image The Background ImageActor to cover background
   */
  void SetBackgroundImage( Actor image );

  /**
   * Sets a title for this Popup.
   *
   * By default a TextView is created with following settings: black color, split-by-word multi-line policy and split exceed policy.
   *
   * @param[in] text The text to appear as the heading for this Popup
   */
  void SetTitle( const std::string& text );

  /**
   * Sets a title for this Popup.
   *
   * @param[in] titleActor The TextView to appear as the heading for this Popup
   */
  void SetTitle( TextView titleActor );

  /**
   * Gets the text (TextView) for this Popup.
   *
   * @return The TextView representing this popup is returned.
   */
  TextView GetTitle() const;

  /**
   * Adds a button to this Popup.
   *
   * Buttons are added to the bottom of the Popup and Centered.
   *
   * By default the first button added will have the focus, and the focus will
   * shift to other buttons based on the sequence in which they are added to the popup.
   *
   * @param[in] button The button to be added to this Popup
   */
  void AddButton( Button button );

  /**
   * Sets state of Popup, such as HIDE, and SHOW.
   *
   * The Popup will instantaneously jump to this state.
   *
   * @param[in] state The state of the popup
   */
  void SetState( PopupState state );

  /**
   * Sets state of Popup, such as HIDE, and SHOW.
   *
   * The Popup will smoothly animate to this state.
   *
   * @param[in] state The state of the popup
   * @param[in] duration The time to animate to this new state.
   */
  void SetState( PopupState state, float duration );

  /**
   * Gets the state of the popup.
   * @return The state of the popup.
   */
  PopupState GetState() const;

  /**
   * Shows the popup.
   *
   * The Popup will animate to the SHOW state
   */
  void Show();

  /**
   * Hides the popup.
   *
   * The Popup will animate to the HIDE state
   */
  void Hide();

  /**
   * Shows the tail.
   *
   * The tail position is specified relative to it's Parent.
   * To display at top center for instace, pass:
   *
   * ParentOrigin::TOP_CENTER
   *
   * @note The tail images are defined inside PopupStyle as
   * tailUpImage, tailDownImage, tailLeftImage, and tailRightImage
   *
   * @param[in] position A position around the perimeter of the Parent.
   */
  void ShowTail(const Vector3& position);

  /**
   * Hides the tail.
   */
  void HideTail();

public: // Not intended for application developers

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  Popup(Internal::Popup& implementation);

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  Popup( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TOOLKIT_POPUP_H__

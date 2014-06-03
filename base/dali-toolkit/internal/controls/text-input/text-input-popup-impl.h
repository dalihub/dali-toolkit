#ifndef __DALI_TOOLKIT_INTERNAL_TEXT_INPUT_POPUP_H__
#define __DALI_TOOLKIT_INTERNAL_TEXT_INPUT_POPUP_H__

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

#include <dali-toolkit/public-api/controls/text-view/text-view.h>

namespace Dali
{

namespace Toolkit
{

class Button;

namespace Internal
{

class TextInputPopup : public ConnectionTracker
{

public:

  enum State
  {
    StateHidden,
    StateHiding,
    StateShowing,
    StateShown
  };

  // Signal names
  static const char* const SIGNAL_PRESSED;
  static const char* const SIGNAL_HIDE_FINISHED;
  static const char* const SIGNAL_SHOW_FINISHED;

  // Popup Button Pressed
  typedef SignalV2< bool( Toolkit::Button ) > PressedSignalV2;

  // Popup Hide Finished
  typedef SignalV2< void( TextInputPopup& ) > HideFinishedSignalV2;

  // Popup Show Finished
  typedef SignalV2< void( TextInputPopup& ) > ShowFinishedSignalV2;

  /**
   * Signal emitted when the button is touched.
   */
  PressedSignalV2& PressedSignal();

  /**
   * Signal emitted when popup is completely hidden
   * @note Only occurs after a Show() call with animation enabled.
   */
  HideFinishedSignalV2& HideFinishedSignal();

  /**
   * Signal emitted when popup is completely shown
   * @note Only occurs after a Hide() call with animation enabled.
   */
  ShowFinishedSignalV2& ShowFinishedSignal();

public:

  /**
   * Default constructor
   * Creates an empty popup base actor (no content i.e. invisible)
   */
  TextInputPopup();

  /**
   * @return The root actor of for this popup is returned.
   */
  Actor Self();

  /**
   * Clears popup options (popup no longer exists)
   */
  void Clear();

  /**
   * Create the label
   * @param[in] styledCaption The text to be displayed
   * @return the newly created label
   */
  Toolkit::TextView CreateOptionText( const MarkupProcessor::StyledTextArray& styledCaption );

  /**
   * Create the label
   * @param[in] iconImage the image to be used
   * @return the newly created Image actor to be used as the icon
   */
  ImageActor CreateOptionIcon( Image iconImage );

  /**
   * Creates and sets up the popup background
   */
  void CreatePopUpBackground();

  /**
   * Create divider if multiple options
   */
  void CreateDivider();

  /**
   * Create a background to be used when button pressed
   * @param[in] requiredSize size Image actor should be
   * @param[in] finalFlag flag to be set if option is the final one.
   * @return Returns an Image Actor to be used a pressed background
   */
  ImageActor CreatePressedBackground( const Vector3 requiredSize, const bool finalFlag );

  /**
   * Adds a popup option.
   * @note Creates popup frame if not already created.
   * @param[in] name The unique name for this option.
   * @param[in] caption The caption (label) for this option
   * @param[in] iconImage Image to displayed with text.
   * @param[in] finalOption Flag to indicate that this is the final option.
   * (set to true on the last option you add)
   */
  void AddOption(const std::string& name, const std::string& caption, const Image iconImage, bool finalOption);

  /**
   * Hides the popup
   * @param[in] animate (optional) whether to animate popup to hide state over time (i.e. tween).
   */
  void Hide(bool animate = true);

  /**
   * Shows the popup
   * @param[in] animate (optional) whether to animate popup to show state over time (i.e. tween).
   */
  void Show(bool animate = true);

  /**
   * Sets Alternative offset property.
   * The alternative offset property is how much to move in the horizontal and vertical
   * axes when the popup goes out of the screen on the left/right sides or top/bottom sides.
   * @param[in] offset Vector holding the left/right offset (x) and top/bottom offset (y)
   */
  void SetAlternativeOffset(Vector2 offset);

  /**
   * Returns the current state of the popup.
   * @return The state of the popup see enum State
   */
  State GetState(void) const;

  /**
   * Get the root actor which the buttons are added to.
   * @return the root actor
   */
  Actor GetRootActor() const;

private:

  /**
   * Adds popup to the stage (ideally on a separate top-most layer and as an overlay)
   */
  void AddToStage();

  /**
   * Applies constraint to keep Popup in view within the desired area.
   */
  void ApplyConfinementConstraint();

  /**
   * Removes popup from the stage.
   */
  void RemoveFromStage();

  /**
   * Called when a button is pressed in the popup
   * @param[in] button The button pressed.
   */
  bool OnButtonPressed( Toolkit::Button button );

  /**
   * Invoked upon popup Hide animation completing.
   * @note Only called for animating hide, not called for instantaneous (animate = false)
   * @param[in] source The animation which completed.
   */
  void OnHideFinished(Animation& source);

  /**
   * Invoked upon popup Show animation completing.
   * @note Only called for animating show, not called for instantaneous (animate = false)
   * @param[in] source The animation which completed.
   */
  void OnShowFinished(Animation& source);

private:

  State mState;                                       ///< Popup State.
  Actor mRootActor;                                   ///< The actor which all popup content is added to (i.e. panel and buttons)
  Property::Index mAlternativeOffsetProperty;         ///< Property [Vector3] how much to offset the popup if it goes out of the screen
  ImageActor mBackground;                             ///< The background popup panel
  ImageActor mTail;                                   ///< The tail for the popup
  Vector3 mContentSize;                               ///< Size of Content (i.e. Buttons)
  ActorContainer mButtonContainer;                    ///< List of buttons added to popup.
  ActorContainer mDividerContainer;                   ///< List of dividers added to popup.
  Animation mAnimation;                               ///< Popup Hide/Show animation.

  PressedSignalV2 mPressedSignal;              ///< Signal emitted when a button within the popup is pressed.
  HideFinishedSignalV2 mHideFinishedSignal;    ///< Signal emitted when popup is completely hidden
  ShowFinishedSignalV2 mShowFinishedSignal;    ///< Signal emitted when popup is completely shown

};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_ITEM_VIEW_H__

#ifndef __DALI_TOOLKIT_INTERNAL_TEXT_INPUT_POPUP_NEW_H__
#define __DALI_TOOLKIT_INTERNAL_TEXT_INPUT_POPUP_NEW_H__

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

#include <dali-toolkit/public-api/controls/text-view/text-view.h>
#include <dali-toolkit/public-api/controls/buttons/button.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * @brief Class to create and Popup bar made up of buttons.
 * It provides signals when a button is pressed.
 * The Popup must be positioned by it's owner.
 * The future plan is to reuse the Toolkit Popup control to house the buttons.
 */

class TextInputPopupNew : public ConnectionTracker
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
  typedef SignalV2< bool( Toolkit::Button ) > PopUpPressedSignal;

  // Popup Hide Finished
  typedef SignalV2< void( TextInputPopupNew& ) > PopUpHideFinishedSignal;

  // Popup Show Finished
  typedef SignalV2< void( TextInputPopupNew& ) > PopUpShowFinishedSignal;

  /**
   * Signal emitted when the button is touched.
   */
  PopUpPressedSignal& PressedSignal() { return mPressedSignal; }

  /**
   * Signal emitted when popup is completely hidden
   * @note Only occurs after a Show() call with animation enabled.
   */
  PopUpHideFinishedSignal& HideFinishedSignal() {return mHideFinishedSignal;}

  /**
   * Signal emitted when popup is completely shown
   * @note Only occurs after a Hide() call with animation enabled.
   */
  PopUpShowFinishedSignal& ShowFinishedSignal() {return mShowFinishedSignal;}

public:

  /**
   * Default constructor
   * Creates an empty popup base actor (no content i.e. invisible)
   */
  TextInputPopupNew()
  : mState(StateHidden)
  { };

  /**
   * Destructor
   */
  ~TextInputPopupNew(){};

  /**
   * @return The root actor of for this popup is returned.
   */
  Actor Self() { return mRootActor; };

  /**
   * Clears popup options (popup no longer exists)
   */
  void Clear(){};

  /**
   * Create the label
   * @param[in] styledCaption The text to be displayed
   * @return the newly created label
   */
  Toolkit::TextView CreateLabel( const MarkupProcessor::StyledTextArray& styledCaption ){return Toolkit::TextView();};

  /**
   * Create the label
   * @param[in] iconImage the image to be used
   * @return the newly created Image actor to be used as the icon
   */
  ImageActor CreateIcon( Image iconImage ) {return ImageActor();};

  /**
   * Creates and sets up the popup background
   */
  void CreatePopUpBackground(){};

  /**
   * Create divider if multiple options
   */
  void CreateDivider(){};

  /**
   * Create a background to be used when button pressed
   * @param[in] requiredSize size Image actor should be
   * @param[in] finalFlag flag to be set if option is the final one.
   * @return Returns an Image Actor to be used a pressed background
   */
  ImageActor CreatePressedBackground( const Vector3 requiredSize, const bool finalFlag ){ return ImageActor(); };

  /**
   * Adds a popup option button.
   * @note Creates popup frame if not already created.
   * @param[in] name The unique name for this option.
   * @param[in] caption The caption (label) for this option
   * @param[in] iconImage Image to displayed with text.
   * @param[in] finalOption Flag to indicate that this is the final option.
   * (set to true on the last option you add)
   */
  void AddButton(const std::string& name, const std::string& caption, const Image iconImage, bool finalOption ){};

  /**
   * Hides the popup
   * @param[in] animate (optional) whether to animate popup to hide state over time (i.e. tween).
   */
  void Hide(bool animate = true){};

  /**
   * Shows the popup
   * @param[in] animate (optional) whether to animate popup to show state over time (i.e. tween).
   * @param[in] target Actor to parent popup.
   */
  void Show( Actor target, bool animate = true ){};

  /**
   * @brief Get the calculated size of the PopUp
   * This can not be set directly as is calculated depending on the content added.
   *
   * @return Vector3 size of PopUp.
   */
  Vector3 GetSize() const { return Vector3::ZERO;};

  /**
   * Returns the current state of the popup.
   * @return The state of the popup see enum State
   */
  State GetState(void) const{ return StateHidden;};

  /**
   * Get the root actor which the buttons are added to.
   * @return the root actor
   */
  Actor GetRootActor() const { return Actor(); };

  /**
   * @brief Creates the PopUp with the required buttons for the provided states.
   * @param[in] isAllTextSelectedAlready Is all the text already selected
   * @param[in] isTextEmpty Contains some text
   * @param[in] hasClipboardGotContent  Something to paste from clipboard
   * @param[in] isSubsetOfTextAlreadySelected  Some but not all text is selected
   */
  void CreateCutCopyPastePopUp( bool isAllTextSelectedAlready, bool isTextEmpty, bool hasClipboardGotContent, bool isSubsetOfTextAlreadySelected ){};

  /**
   * @brief Applies constraint to keep Popup in view within the desired area.
   * @param[in] bounding box in which the PopUp must remain.
   *
   */
  void ApplyConfinementConstraint( Vector4 boundingBox ){};

private:

  /**
   * @brief Adds popup to the given parent
   * @paran[in] parent target to add Popup to
   */
  void AddToParent( Actor parent ){};

  /**
   * Removes popup from Parent.
   */
  void RemoveFromStage(){};

  /**
   * Called when a button is pressed in the popup
   * @param[in] button The button pressed.
   */
  bool OnButtonPressed( Toolkit::Button button ){return false;};

  /**
   * Invoked upon popup Hide animation completing.
   * @note Only called for animating hide, not called for instantaneous (animate = false)
   * @param[in] source The animation which completed.
   */
  void OnHideFinished(Animation& source){};

  /**
   * Invoked upon popup Show animation completing.
   * @note Only called for animating show, not called for instantaneous (animate = false)
   * @param[in] source The animation which completed.
   */
  void OnShowFinished(Animation& source);

private:

  /**
   * @brief Copy Constructor
   * @param[in] popup
   * Undefined/Hidden.
   */
  TextInputPopupNew(const TextInputPopupNew& popup );

  /**
   * @Assignment Constructor
   * @param[in] rhs
   * Undefined/Hidden.
   */
  TextInputPopupNew& operator=(const TextInputPopupNew& rhs);

private:

  State mState;                                       // Popup State.
  Actor mRootActor;                                   // The actor which all popup content is added to (i.e. panel and buttons)
  Vector3 mPopupSize;                                 // Size of the PopUp determined by it's content and max/min size constraints.
  ImageActor mBackground;                             // The background popup panel
  ImageActor mTail;                                   // The tail for the popup
  Vector3 mContentSize;                               // Size of Content (i.e. Buttons)
  ActorContainer mButtonContainer;                    // List of buttons added to popup.
  ActorContainer mDividerContainer;                   // List of dividers added to popup.
  Animation mAnimation;                               // Popup Hide/Show animation.

  PopUpPressedSignal mPressedSignal;                     // Signal emitted when a button within the popup is pressed.
  PopUpHideFinishedSignal mHideFinishedSignal;           // Signal emitted when popup is completely hidden
  PopUpShowFinishedSignal mShowFinishedSignal;           // Signal emitted when popup is completely shown

};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TEXT_INPUT_POPUP_NEW_H__

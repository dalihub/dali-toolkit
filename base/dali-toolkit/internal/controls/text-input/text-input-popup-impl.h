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
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view.h>

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

  enum Buttons
  {
    ButtonsCut,
    ButtonsCopy,
    ButtonsPaste,
    ButtonsSelect,
    ButtonsSelectAll,
    ButtonsClipboard,
    ButtonsEnumEnd
  };

  struct ButtonRequirement
  {
    TextInputPopup::Buttons buttonId;
    std::size_t orderOfPriority;
    std::string name;
    std::string caption;
    Image iconImage;
    bool enabled;
  };

  static const char* const OPTION_SELECT_WORD;
  static const char* const OPTION_SELECT_ALL;
  static const char* const OPTION_CUT;
  static const char* const OPTION_COPY;
  static const char* const OPTION_PASTE;
  static const char* const OPTION_CLIPBOARD;

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
   * @return Returns an Image Actor to be used a pressed background
   */
  ImageActor CreatePressedBackground( const Vector3 requiredSize );

  /**
   * Creates a ordered vector of button options
   */
  void CreateOrderedListOfOptions();

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

  /**
   * Set the Cut and Paste buttons color when in normal state
   * @param[in] color color to use
   */
  void SetCutPastePopUpColor( const Vector4& color );

  /**
   * Get the set color of the Copy and Paste PopUp buttons
   * @return color
   */
  const Vector4& GetCutPastePopUpColor() const;

  /**
   * Set the Cut and Paste button color when pressed.
   * @param[in] color color to use
   */
  void SetCutPastePopUpPressedColor( const Vector4& color );

  /**
   * Get the Cut and Paste pressed button color.
   * @return color
   */
  const Vector4& GetCutPastePopUpPressedColor() const;

  /**
   * Toggle if a popup button should be enabled (shown) or not
   * @param[in]  requiredButton Button Id to enable or disable
   * @param[in]  enable toggle to enable (true) or disable (false)
   */
  void TogglePopUpButtonOnOff( TextInputPopup::Buttons requiredButton, bool enable );

  /**
   * Set the Button Priority Position
   * @param[in] button Button id for priority to be set on
   * @param[in] priority Priority level, 1 is highest so will appear first.  0 priority will not show the button.
   */
  void SetButtonPriorityPosition( TextInputPopup::Buttons button, unsigned int priority );

  /**
   * Get the Button Priority Position
   * @param[in] button Button id to get priority of
   * @return the button priority, 1 is highest, 0 is not shown.
   */
  unsigned int GetButtonPriorityPosition( TextInputPopup::Buttons button ) const;

  /**
   * Adds Popup options which have been enabled.
   */
  void AddPopupOptions();

  /**
   * Set Boundary that PopUp should stay within
   * @param[in] boundingRectangle coordinates of bounding box from Top Left
   */
  void SetPopupBoundary( const Rect<float>& boundingRectangle );

private:

  /**
   * Creates a Button with the required parameters.
   * @param[in] buttonId enum representing the button
   * @param[in] orderOfPriority Position in toolbar button should be position, 1 is first from left to right.
   * @param[in] name Given name for Button actor
   * @param[in] caption Text to display in button
   * @param[in] iconImage Icon to display in button
   * @param[in] enabled Toggle if button should be used or not, this is decided by the current state/conditions.
   */
  TextInputPopup::ButtonRequirement CreateRequiredButton( TextInputPopup::Buttons buttonId, std::size_t orderOfPriority,
                                                                          const std::string& name, const std::string& caption, Image iconImage, bool enabled );

  /**
   * Adds Popup to the stage (ideally on a separate top-most layer and as an overlay)
   */
  void AddToStage();

  /**
   * Applies constraint to keep Popup in view within the desired area.
   */
  void ApplyConfinementConstraint();

  /**
   * Create a layer to hold the stencil
   * @param[in] size Size to of layer
   */
  void CreateLayer(  const Vector2& size );

  /**
   * Create a stencil to clip the scroll view content
   * @param[in] size Size to of stencil
   */
  void CreateStencil( const Vector2& size );

  /**
   * Popup has started to scroll
   * @param[in] position current scroll view position
   */
  void OnScrollStarted( const Vector3& position );

  /**
   * Popup has stopped scrolling
   * @param[in] position current scroll view position
   */
  void OnScrollCompleted( const Vector3& position );

  /**
   * Create a scroll view to hold the popup buttons and allow scrolling if too many buttons to fit within the visible boundary
   * @param[in] scrollViewSize size of content of the scroll view which can exceed its visible size
   * @param[in] visibleSize size of the visible scroll view
   */
  void CreateScrollView( const Vector2& scrollViewSize, const Vector2& visibleSize );

  /**
   * Removes Popup from the stage.
   */
  void RemoveFromStage();

  /**
   * Called when a button is pressed in the Popup
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

  Actor mStencil;                                     ///< Stencil to clip scrollview
  Toolkit::ScrollView mScrollView;                    ///< Scrollview to house the popup
  Layer mLayer;                                       ///< Layer to house the scroll view and stencil

  std::vector<ButtonRequirement> mOrderListOfButtons;        // List of buttons in the order to be displayed and a flag to indicate if needed.

  Vector4           mCutPasteButtonsColor;  // Color of the cut and paste popup
  Vector4           mCutPasteButtonsPressedColor;  // Color of the cut and paste buttons when pressed.
  Vector4           mBorderColor; // Color of the border around the Cut and Paste Popup

  Rect<float> mBoundingRect; // Boundary that Popup must stay within.

  // Priority of Options/Buttons in the Cut and Paste pop-up, higher priority buttons are displayed first, left to right.
  std::size_t mSelectOptionPriority;  // Position of Select Button
  std::size_t mSelectAllOptionPriority; // Position of Select All button
  std::size_t mCutOptionPriority; // Position of Cut button
  std::size_t mCopyOptionPriority; // Position of Copy button
  std::size_t mPasteOptionPriority;  // Position of Paste button
  std::size_t mClipboardOptionPriority;  // Position of Clipboard button

  PressedSignalV2 mPressedSignal;              ///< Signal emitted when a button within the popup is pressed.
  HideFinishedSignalV2 mHideFinishedSignal;    ///< Signal emitted when popup is completely hidden
  ShowFinishedSignalV2 mShowFinishedSignal;    ///< Signal emitted when popup is completely shown

};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_ITEM_VIEW_H__

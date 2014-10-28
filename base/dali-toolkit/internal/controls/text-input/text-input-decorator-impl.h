#ifndef __DALI_TOOLKIT_INTERNAL_TEXT_INPUT_DECORATOR_H__
#define __DALI_TOOLKIT_INTERNAL_TEXT_INPUT_DECORATOR_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/actors/mesh-actor.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/geometry/mesh.h>
#include <dali/public-api/signals/connection-tracker.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/text-input/textview-character-positions-impl.h>
#include <dali-toolkit/internal/controls/text-input/text-input-handles-impl.h>
#include <dali-toolkit/internal/controls/text-input/text-input-text-highlight-impl.h>
#include <dali-toolkit/internal/controls/text-input/text-input-popup-new-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class Decorator;

typedef IntrusivePtr<Decorator> DecoratorPtr;

/**
 *  @brief Decorator Class
 *
 *  Decorations are Selection Handles, cursor, grab handle, magnifier the "cut copy paste" PopUp and Selection highlight.
 *  The Decorator triggers creation of these decorations and positions them.
 *  Decoration positions can be dependent on other decorations like the PopUp on the Selection handles.
 *  The decorator maintains a Bounding Box which the decorations have to be positioned within, decorations can be flipped or hidden to obey this Bounding Box.
 *  Scrolling of Text can effect positioning of decorations, the decorator repositions decorations in this case.
 */

class Decorator : public ConnectionTracker
{

public:

  /**
   * @brief Constructor
   *
   * @param[in] textviewManager TextViewManager to be used
   */
  Decorator( TextViewCharacterPositioning& textviewManager, TextInputTextStyle& textStyle);

  /**
   * @brief Default destructor
   */
  ~Decorator();

  /**
   * @brief Set the dimensions of the bounding rectangle for decorations to obey.
   *
   * @param[in] boundingRectangle
   */
  void SetBoundingBox( const Rect<float>& boundingRectangle );

  /**
   * @brief Get the bounding dimensions of the bounding box
   *
   * @return dimensions of the bounding box from world origin. (x, y, w, z )
   *
   *  -----------------
   * |        ^        |
   * |        |        |
   * |        y        |
   * |        |        |
   * |        v        |
   * |<--x--> o <--z-->|
   * |        ^        |
   * |        |        |
   * |        w        |
   * |        |        |
   * |        v        |
   *  -----------------
   */
  Vector4 GetBoundingBox() const;

  /**
   * @brief Callback when a handle is panned/moved, either selection handles or grab handle
   *
   * @param actor Handle of the selection or grab handle.
   * @param gesture Data structure with the parameters of the gesture.
   */
  void OnHandlePan(Actor actor, PanGesture gesture);

  // Selection Handles

  /**
   * @brief Create a left and right selection handle and parent both to the provided actor
   * @param[in] parent actor in which the handles should be added to.
   */
  void CreateSelectionHandles( Actor parent );

  /**
   * @brief Remove selection handles from their parent
   */
  void RemoveSelectionHandles();

  /**
   * @brief Get size of Selection handles
   *
   * @return size of a selection handle
   */
  Vector3 GetSelectionHandleSize();

  /**
   * @brief Get position of Selection handle within text
   *
   * @return character position of a selection handle one
   */
  std::size_t GetHandleOnePosition() const;

  /**
   * @brief Get position of Selection handle within text
   *
   * @return character position of a selection handle two
   */
  std::size_t GetHandleTwoPosition() const;

  /**
   * @brief Position Selection a single handle at given positions within the text string.
   *
   * @param[in] selectionHandle handle to be positioned
   * @param[in] position where to place handle
   * @return Vector3 Position of handle as a coordinate.
   */
  Vector3 PositionSelectionHandle( Actor selectionHandle, std::size_t position );

  /**
   * @brief Position Selection a single handle at given coordinates
   *
   * @param[in] selectionHandle handle to be positioned
   * @param[in] actualPosition coordinates to position handle
   * @param[in] position where to place handle
   * @return Vector3 Position of handle as a coordinate.
   */
  Vector3 PositionSelectionHandle( Actor selectionHandle, Vector3& actualPosition, std::size_t position );

  /**
   * @brief Make both selection handle visible or invisible
   * @param[in] visible true to make visible, false to fine
   */
  void SetSelectionHandlesVisibility( bool visible );

  /**
   * @brief Position Selection handles at given positions within the text string.
   *
   * @param[in] start where to place first handle
   * @param[in] end  where to place second handle
   */
  void PositionSelectionHandles( std::size_t start, std::size_t end );

  /**
   * @brief Move selection handle by the given displacement.
   *
   * @param[in] selectionHandle Actor to move
   * @param[in] actualSelectionHandlePosition actual current position of the handle in x y z
   * @param[in] currentSelectionHandlePosition current position along the string
   * @param[in] displacement the x y displacement
   */
  Vector3 MoveSelectionHandle( Actor selectionHandle,
                               Vector3& actualSelectionHandlePosition,
                               std::size_t& currentSelectionHandlePosition,
                               const Vector2& displacement );

  /* Grab Handle */

  /**
   * @brief Position GrabHandlewith depending on the the character in the text it should be placed at
   * @param[in] positonInText the character position within the text the handle should be at
   */
  void PositionGrabHandle( std::size_t positionInText );

  /**
   * @brief Move grab handle to the required position within the text
   *
   * @param[in] displacement Displacement of the grab handle in actor coordinates.
   */
  void MoveGrabHandle( const Vector2& displacement );

  /**
   * @brief Show or hide the GrabHandle is visibility is true
   *
   * @param[in] visible flag to show or not show the grab handle
   */
  void ShowGrabHandle( bool visible );

  /**
   * @brief Create the GrabHandle used to position cursor
   * @param[in] targetParent the Actor to parent the GrabHandle
   */
  void CreateGrabHandle( Actor targetParent );

  /**
   * @brief Set the image to be used as the cursor grab hander
   * @pre The text input actor has been initialised.
   * @param[in] image The image to be used.
   */
  void SetGrabHandleImage( Image image );

  /**
   * @brief Toggle to enable the grab handle, used to position cursor when magnifier not being used.
   * Default behaviour is to use the magnifier to position the cursor, enabling this prevents the magnifier from being shown.
   * @param[in] toggle true to enable, false to disable grab handle
   */
  void EnableGrabHandle(bool toggle);

  /**
   * @brief Method to check if grab handle is enabled, if false then the magnifier will be used to position cursor.
   * @return bool returns true is grab handle enabled.
   */
  bool IsGrabHandleEnabled();

  /* Cursor */

  /**
   * @brief Get the current Cursor position
   * @return current cursor position
   */
  std::size_t GetCurrentCursorPosition() const;

  /**
   * @brief Set the Cursor position
   * @param[in] the position the cursor should be set to
   */
  void SetCurrentCursorPosition( std::size_t newCursorPosition );

  /**
   * @brief Set if the cursors are visible or not.
   * @param[in] visible flag true for visible
   */
  void SetCursorVisibility( bool visible );

  /**
   * @brief Display cursor
   * @param[in] nthChar position in text string to display cursor
   */
  void DrawCursor( const std::size_t nthChar = 0 );

  /**
   * Sets alternate cursor enable state
   * @see SetCursorVisibility
   * alternate cursor will only be visible if both SetCursorVisiblity
   * and cursor enabled have been set to true.
   */
  void SetAltCursorEnabled( bool enabled );

  /**
   * @brief Set the image to be used for the regular left to right cursor
   * @pre The text input actor has been initialised.
   * @param[in] image The image to be used.
   * @param[in] border The nine patch border for the image.
   */
  void SetCursorImage( Image image, const Vector4& border );

  /**
   * @brief Set the image to be used for the Right to Left cursor
   * @pre The text input actor has been initialised.
   * @param[in] image The image to be used.
   * @param[in] border The nine patch border for the image.
   */
  void SetRTLCursorImage( Image image, const Vector4& border );

  /**
   * @brief Creates a cursor from the supplied image and nine patch border.
   * @param[in] cursorImage the image to be used for the cursor.
   * @param[in] border the nine patch border corresponding to the supplied image.
   * @paran[in] cursorName actor name for cursor
   * @return the image actor to be used as the cursor.
   */
  ImageActor CreateCursor( Image cursorImage, const Vector4& border,  const std::string& cursorName );

  /**
   * @brief Creates a regular and Right-To-Left cursor and parents them to give target Actor
   * @param[in] targetParent target Actor
   */
  void CreateCursors( Actor targetParent );

  /**
   * @Brief Returns the cursor size at a given position in the text.
   * @return Size the size of the cursor
   */
  Size GetCursorSizeAt( std::size_t positionWithinTextToGetCursorSize );

  /**
   * @brief  Start a timer to signal cursor to blink.
   */
  void StartCursorBlinkTimer();

  /**
   * @brief  Stop the timer signalling the cursor to blink.
   */
  void StopCursorBlinkTimer();

  /**
   * @brief Callback when handle timer ticks.
   *
   * Cursor should become visible/invisible to simulate blinking.
   *
   * @return True if the timer should be keep running.
   */
  bool OnCursorBlinkTimerTick();

  /* Selection Highlight */

  /**
   * @brief Updates mesh data for selection highlight depending on handle positions and displays it.
   */
  void ShowUpdatedHighlight();

  /**
   * @brief Creates the Highlight used for selection
   *
   * @param[in] parent target actor in which the handles should be added to.
   */
  void CreateHighlight( Actor parent );

  /**
   * @brief Remove Highlight actor from it's parent
   */
  void RemoveHighlight();

  /**
   * @brief Set the visibility of the Highlight
   *
   * @param[in] visibility True to show and False to hide.
   */
  void HighlightVisibility( bool visiblility );

  /* Boundary Property Notifications when handle exceed bounding box*/

  /**
   * @brief PropertyNotification Callback when left boundary exceeded so handle can be flipped.
   *
   * @param[in] source PropertyNotification
   */
  void OnLeftBoundaryExceeded( PropertyNotification& source );
  /**
   * @brief PropertyNotification Callback when within left boundary so handle can be flipped back.
   *
   * @param[in] source PropertyNotification
   */
  void OnReturnToLeftBoundary( PropertyNotification& source );
  /**
   * @brief PropertyNotification Callback when right boundary exceeded so handle can be flipped.
   *
   * @param[in] source PropertyNotification
   */
  void OnRightBoundaryExceeded( PropertyNotification& source );
  /**
   * @brief PropertyNotification Callback when within right boundary so handle can be flipped back.
   *
   * @param[in] source PropertyNotification
   */
  void OnReturnToRightBoundary( PropertyNotification& source );

  /**
   * @brief PropertyNotification Callbacks for hiding handle one when it exceeds boundary.
   *
   * @param[in] source PropertyNotification
   */
  void OnHandleOneLeavesBoundary( PropertyNotification& source );
  /**
   * @brief PropertyNotification Callbacks for showing hidden handle one when returns within boundary
   *
   * @param[in] source PropertyNotification
   */
  void OnHandleOneWithinBoundary( PropertyNotification& source );
  /**
   * @brief PropertyNotification Callbacks for hiding handle two it  when exceeds boundary.
   *
   * @param[in] source PropertyNotification
   */
  void OnHandleTwoLeavesBoundary( PropertyNotification& source );
  /**
   * @brief PropertyNotification Callbacks for showing hidden handle two when returns within boundary
   *
   * @param[in] source PropertyNotification
   */
  void OnHandleTwoWithinBoundary( PropertyNotification& source );

  /**
   * @brief Set up property notifications on the position of the handles to facilitate flipping and hiding when at screen boundary.
   */
  void SetUpHandlePropertyNotifications();

  // Cut & Paste Pop-up

  /**
   * @brief Calculate positioning of PopUp relative to handles
   * @return Actual position of PopUp
   */
  Vector3 PositionOfPopUpRelativeToSelectionHandles( );

  /**
   * @brief Calculate alternative position of PopUp relative to handles  when can it not be displayed in the default upper position.
   * @return Actual position of PopUp
   */
  Vector3 AlternatePopUpPositionRelativeToSelectionHandles();

  /**
   * @brief Calculate positioning of PopUp relative to cursor
   * @return Actual position of PopUp
   */
  Vector3 PositionOfPopUpRelativeToCursor();

  /**
   * @brief Calculate alternative position of PopUp relative to cursor when can not be displayed in normal upper position.
   * @return Actual position of PopUp
   */
  Vector3 AlternatePopUpPositionRelativeToCursor();

  /**
   * @brief Calculate positioning of PopUp relative to GrabHandle
   * @return Actual position of PopUp
   */
  Vector3 PositionOfPopUpRelativeToGrabHandle();

 /**
  * @brief Show the PopUp in the provided target
  * @param[in] target target actor in which the PopUp should be added to.
  */
  void ShowPopUp( Actor target );

  /**
   * @brief Show PopUp in previously set Target.
   * @pre Must have previously called ShopPopUp( Actor target ) otherwise PopUp will not be shown.
   */
  void ShowPopUp();

  /**
   * @brief Create and Show Cut Copy Paste PopUp
   */
  void ShowPopupCutCopyPaste();

  /**
   * @brief Hide PopUp
   * @param[in] animate Animate or just hide instantly, default is true
   * @param[in] signalFinished Signal when finished, default is true
   */
  void HidePopUp( bool animate=true, bool signalFinished=true );

  /**
   * @brief Adds a popup option.
   * @brief Creates popup frame if not already created.
   * @param[in] name The unique name for this option.
   * @param[in] caption The caption (label) for this option
   * @param[in] icon the image icon to be displayed for this option
   * @param[in] finalOption Flag to indicate that this is the final option.
   * (set to true on the last option you add)
   */
  void AddPopupOption(const std::string& name, const std::string& caption, const Image icon, bool finalOption = false);

  /**
   * @brief Removes popup, and its options.
   */
  void ClearPopup();

  /**
   * @brief PropertyNotification Callbacks for flipping PopUp when exceeds boundary.
   * @param[in] source PropertyNotification
   */
  void PopUpLeavesVerticalBoundary( PropertyNotification& source );

  /**
   * @brief Setup position notifications when PopUp exceeds boundary
   */
  void SetUpPopUpPositionNotifications( );

  /**
   * @brief Callback for when a button is pressed in popup panel
   * @param[in] button handle to the button pressed.
   * @return bool consummation
   */
  bool OnPopupButtonPressed( Toolkit::Button button );

  // Decoration positioning during scrolling

  /**
   * @brief Updates the position of the decorations when Text is scrolled.
   *
   * @param[in] textView Handle of the text-view.
   * @param[in] scrollPosition The difference with the previous scroll position.
   */
  void TextViewScrolled( Toolkit::TextView textView, Vector2 scrollPosition );

  /**
   * @brief  Creates and starts a timer to scroll the text when handles are close to the edges of the text-input.
   *
   * It only starts the timer if it's already created.
   */
  void StartScrollTimer();

  /**
   * @brief  Stops the timer used to scroll the text.
   */
  void StopScrollTimer();

  /**
   * @brief Scroll Text according to handle position
   * @param[in out] handlePosition handle position within character string
   * @param[in] actual vector position of handle
   * @return updated actual vector position of handle
   */
  Vector3 ScrollRelativeToHandle( std::size_t& handlePosition, Vector3& actualHandlePosition );

  /**
   * @brief Callback called by the timer used to scroll the text.
   *
   * It calculates and sets a new scroll position.
   */
  bool OnScrollTimerTick();

  // Text Selection

  /**
   * @brief Function to get Text selected between the 2 selection handles.
   * @return StyledTextArray an array of
   */
  MarkupProcessor::StyledTextArray GetSelectedText();

private:

  /**
   * @brief Copy Constructor
   * @param[in] decorator
   * Undefined/Hidden.
   */
  Decorator(const Decorator& decorator);

  /**
   * @Assignment Constructor
   * @param[in] rhs
   * Undefined/Hidden.
   */
  Decorator& operator=(const Decorator& rhs);

public:

  typedef SignalV2< bool( Toolkit::Button ) > PressedSignal;
  typedef SignalV2< void () > CursorPositionedSignal;
  /**
   * @brief Signal emitted when the button is touched.
   * This is relayed from the PopUp class.  It enables the owner of the Decorator to act on the PopUp button press.
   */
  PressedSignal& PopUpButtonPressedSignal();

  /**
   * @brief Signal emitted when the cursor is repositioned
   * @param[in] cursor the new cursor position
   */
  CursorPositionedSignal& CursorRePositionedSignal();

private:

  Vector4 mBoundingRectangleWorldCoordinates;

  TextViewCharacterPositioning& mTextViewCharacterPositioning;

  TextInputHandles mTextInputHandles;

  TextInputTextStyle& mTextStyle;

  Vector3 mSelectionHandleOneActualPosition;    // Actual x y position of handle
  Vector3 mSelectionHandleTwoActualPosition;    // Actual x y position of handle
  std::size_t mSelectionHandleOnePosition;      // Position of handle along the string of text
  std::size_t mSelectionHandleTwoPosition;      // Position of handle along the string of text

  TextInputPopupNew mPopUpPanel;                // PopUp used for Cut Cpoy and Paste
  Actor mPopUpTarget;                           // Target Actor to parent PopUp

  Vector3 mActualGrabHandlePosition;            // Actual position of grab handle, this might not be snapped to a character
  std::size_t mGrabHandlePosition;              // Position of grab handle along the string of text
  Vector3 mCurrentHandlePosition;

  std::size_t mCursorPosition;                  // Current cursor position within the text string
  ImageActor mCursor;                           // Cursor overlayed on Text to show where new text will be inserted
  ImageActor mCursorRTL;                        // Right To Left Cursor overlayed on Text (where new RTL text would be inserted)
  Animation mCursorAnimation;                   // Animation for cursor blinking.
  Timer mCursorBlinkTimer;                      // Timer to signal cursor to blink

  Vector2 mScrollDisplacement;                  // How much to scroll by
  Timer mScrollTimer;                           // Timer to scroll text over a period of time not all in one update.

  TextHighlight mTextHighlight;                 // Holds data required to construct the highlight
  MeshActor mHighlightMeshActor;                // Mesh Actor to display highlight

  PanGestureDetector mPanGestureDetector;

  PressedSignal mPopUpButtonPressedSignal;           // Signal emitted when a button within the popup is pressed.
  CursorPositionedSignal mCursorRePositionedSignal;  // Signal emitted when a button when cursor position is changed.

  bool mCursorBlinkStatus:1;                    // \e true shows the cursor, \e false hides it.
  bool mCursorVisibility:1;                     // Should cursor be visible
  bool mCursorRTLEnabled:1;                     // Enable state of Alternate RTL Cursor (need to keep track of this as it's not always enabled)
  bool mIsGrabHandleInScrollArea:1;             // Whether the grab handle is inside the boundaries of the text-input.
  bool mIsCursorInScrollArea:1;                 // Whether the cursor is inside the boundaries of the text-input.
  bool mGrabHandleVisibility:1;                 // Should grab handle be visible
  bool mGrabHandleEnabled:1;                    // Flag to enable the grab handle instead of the default magnifier.
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TEXT_INPUT_DECORATOR_H__

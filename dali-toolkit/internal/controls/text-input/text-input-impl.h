#ifndef __DALI_TOOLKIT_INTERNAL_TEXT_INPUT_H__
#define __DALI_TOOLKIT_INTERNAL_TEXT_INPUT_H__

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/text-input/text-input.h>
#include <dali-toolkit/public-api/controls/text-view/text-view.h>
#include <dali-toolkit/public-api/controls/tool-bar/tool-bar.h>
#include <dali-toolkit/public-api/controls/buttons/push-button.h>
#include <dali-toolkit/internal/controls/text-input/text-input-popup-impl.h>

#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/adaptor-framework/common/imf-manager.h>
#include <dali/public-api/geometry/mesh.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class TextInput;
class TextView;

typedef IntrusivePtr<TextInput> TextInputPtr;

class TextInput : public ControlImpl
{
public:

  /**
   * Create a new TextInput
   * @return instrusive ptr to a TextInput
   */
  static Dali::Toolkit::TextInput New();

  /**
   * @copydoc Toolkit::TextInput::GetText
   */
  std::string GetText() const;

  /**
   * @copydoc Toolkit::TextInput::GetMarkupText()
   */
  std::string GetMarkupText() const;

  /**
   * @copydoc Toolkit::TextInput::SetPlaceholderText
   */
  void SetPlaceholderText( const std::string& placeHolderText );

  /**
   * @copydoc Toolkit::TextInput::SetPlaceholderText
   */
  std::string GetPlaceholderText();

  /**
   * @copydoc Toolkit::TextInput::SetInitialText
   */
  void SetInitialText(const std::string& initialText);

  /**
   *  set the text to be displayed in text-input, will overwrite any existing text.
   *  can be used to clear the text-input by passing an empty string.
   *  @param [in] initialText text to be initially displayed
   */
  void SetText(const std::string& initialText);

  /**
   * @copydoc Toolkit::TextInput::SetMaxCharacterLength
   */
  void SetMaxCharacterLength(std::size_t maxChars);

  /**
   * @copydoc Toolkit::TextInput::SetNumberOfLinesLimit
   */
  void SetNumberOfLinesLimit(std::size_t maxLines);

  /**
   * @copydoc Toolkit::TextInput::GetNumberOfLinesLimit
   */
  std::size_t GetNumberOfLinesLimit() const;

  /**
   * @copydoc Toolkit::TextInput::GetFont
   */
  Font GetFont() const;

  /**
   * @copydoc Toolkit::TextInput::SetFont
   */
  void SetFont(Font font);

  /**
   * @copydoc Toolkit::TextInput::InputStartedSignal()
   */
  Toolkit::TextInput::InputSignalV2& InputStartedSignal();

  /**
   * @copydoc Toolkit::TextInput::InputFinishedSignal()
   */
  Toolkit::TextInput::InputSignalV2& InputFinishedSignal();

  /**
   * @copydoc Toolkit::TextInput::CutAndPasteToolBarDisplayedSignal()
   */
  Toolkit::TextInput::InputSignalV2& CutAndPasteToolBarDisplayedSignal();

  /**
   * @copydoc Toolkit::TextInput::StyleChangedSignal()
   */
  Toolkit::TextInput::StyleChangedSignalV2& StyleChangedSignal();

  /**
   * @copydoc Toolkit::TextInput::MaxInputCharactersReachedSignal()
   */
  Toolkit::TextInput::MaxInputCharactersReachedSignalV2& MaxInputCharactersReachedSignal();

  /**
   * @copydoc Toolkit::TextInput::InputTextExceedBoundariesSignal()
   */
  Toolkit::TextInput::InputTextExceedBoundariesSignalV2& InputTextExceedBoundariesSignal();

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

  /**
   * @see Toolkit::TextInput::SetEditMode(bool editMode)
   * @see Toolkit::TextInput::SetEditMode(bool editMode, const Vector2& touchPoint)
   *
   * @param[in] editMode true or false to indicate editMode on or off.
   * @param[in] setCursorOnTouchPoint Whether to use the touch point to set the cursor position.
   * @param[in] touchPoint A position in actor coordinates within the text-input.
   */
  void SetEditable(bool editMode, bool setCursorOnTouchPoint, const Vector2& touchPoint = Vector2::ZERO);

  /**
   * @copydoc Toolkit::TextInput::GetEditMode
   */
  bool IsEditable() const;

  /**
   * @copydoc Toolkit::TextInput::SetTextSelectable
   */
  void SetTextSelectable(bool textSelectable = true);

  /**
   * @copydoc Toolkit::TextInput::IsTextSelectable
   */
  bool IsTextSelectable() const;

  /**
   * @copydoc Toolkit::TextInput::IsTextSelected
   */
  bool IsTextSelected() const;

  /**
   * @copydoc Toolkit::TextInput::DeSelectText
   */
  void DeSelectText();

  /**
   * @copydoc Toolkit::TextInput::SetEditOnTouch
   */
  void SetEditOnTouch(bool editOnTouch);

  /**
   * @copydoc Toolkit::TextInput::IsEditOnTouch
   */
  bool IsEditOnTouch() const;

  /**
   * @copydoc Toolkit::TextInput::SetGrabHandleImage
   */
  void SetGrabHandleImage(Dali::Image image);

  /**
   * @copydoc Toolkit::TextInput::SetCursorImage
   */
  void SetCursorImage(Dali::Image image, const Vector4& border );

  /**
   * @copydoc Toolkit::TextInput::GetSelectionHandleSize
   */
  Vector3 GetSelectionHandleSize();

  /**
   * @copydoc Toolkit::TextInput::SetRTLCursorImage
   */
  void SetRTLCursorImage(Dali::Image image, const Vector4& border );

  /**
   * @copydoc Toolkit::TextInput::EnableGrabHandle
   */
  void EnableGrabHandle(bool toggle);

  /**
   * @copydoc Toolkit::TextInput::IsGrabHandleEnabled
   */
  bool IsGrabHandleEnabled();

  /**
   * @copydoc Toolkit::TextInput::EnableSelectionHandleFlip
   */
  void EnableSelectionHandleFlip( bool toggle );

  /**
   * @copydoc Toolkit::TextInput::IsSelectionHandleFlipEnabled
   */
  bool IsSelectionHandleFlipEnabled();

  /**
   * @copydoc Toolkit::TextInput::SetSelectionHandleFlipMargin
   */
  void SetSelectionHandleFlipMargin( const Vector4& margin );

  /**
   * @copydoc Toolkit::TextInput::SetBoundingRectangle
   */
  void SetBoundingRectangle( const Rect<float>& boundingRectangle );

  /**
   * @copydoc Toolkit::TextInput::GetBoundingRectangle
   */
  const Rect<float> GetBoundingRectangle() const;

  /**
   * @copydoc Toolkit::TextInput::GetSelectionHandleFlipMargin
   */
  const Vector4& GetSelectionHandleFlipMargin();

  /**
   * @copydoc Toolkit::TextInput::SetTextColor
   */
  void SetTextColor( const Vector4& color );

  /**
   * @copydoc Toolkit::TextInput::SetActiveStyle
   */
  void SetActiveStyle( const TextStyle& style, const TextStyle::Mask mask );

  /**
   * @copydoc Toolkit::TextInput::ApplyStyle
   */
  void ApplyStyle( const TextStyle& style, const TextStyle::Mask mask );

  /**
   * @copydoc Toolkit::TextInput::ApplyStyleToAll
   */
  void ApplyStyleToAll( const TextStyle& style, const TextStyle::Mask mask );

  /**
   * @copydoc Toolkit::TextInput::GetStyleAtCursor
   */
  TextStyle GetStyleAtCursor() const;

  /**
   * Retrieves the character style for the given position.
   * @param[in] position The character position which style is required.
   * @return The style for the given position.
   */
  TextStyle GetStyleAt( std::size_t position ) const;

  /**
   * @copydoc Toolkit::TextInput::SetTextAlignment()
   */
  void SetTextAlignment( Toolkit::Alignment::Type align );

  /**
   * @copydoc Toolkit::TextInput::SetTextLineJustification()
   */
  void SetTextLineJustification( Toolkit::TextView::LineJustification justification );

  /**
   * @copydoc Toolkit::TextInput::SetFadeBoundary()
   */
  void SetFadeBoundary( const Toolkit::TextView::FadeBoundary& fadeBoundary );

  /**
   * @copydoc Toolkit::TextInput::GetFadeBoundary()
   */
  const Toolkit::TextView::FadeBoundary& GetFadeBoundary() const;

  /**
   * @copydoc Toolkit::TextInput::GetTextAlignment()
   */
  Toolkit::Alignment::Type GetTextAlignment() const;

  /**
   * @copydoc Toolkit::TextInput::SetMultilinePolicy()
   */
  void SetMultilinePolicy( Toolkit::TextView::MultilinePolicy policy );

  /**
   * @copydoc Toolkit::TextInput::GetMultilinePolicy()
   */
  Toolkit::TextView::MultilinePolicy GetMultilinePolicy() const;

  /**
   * @copydoc Toolkit::TextInput::SetWidthExceedPolicy()
   */
  void SetWidthExceedPolicy( Toolkit::TextView::ExceedPolicy policy );

  /**
   * @copydoc Toolkit::TextInput::GetWidthExceedPolicy()
   */
  Toolkit::TextView::ExceedPolicy GetWidthExceedPolicy() const;

  /**
   * @copydoc Toolkit::TextInput::SetHeightExceedPolicy()
   */
  void SetHeightExceedPolicy( Toolkit::TextView::ExceedPolicy policy );

  /**
   * @copydoc Toolkit::TextInput::GetHeightExceedPolicy()
   */
  Toolkit::TextView::ExceedPolicy GetHeightExceedPolicy() const;

  /**
   * @copydoc Toolkit::TextInput::SetExceedEnabled()
   */
  void SetExceedEnabled( bool enable );

  /**
   * @copydoc Toolkit::TextInput::GetExceedEnabled()
   */
  bool GetExceedEnabled() const;

  /**
   * @copydoc Toolkit::TextInput::SetBackground
   */
  void SetBackground(Dali::Image image );

  /**
   * @copydoc Toolkit::TextInput::SetNumberOfLines
   */
  void SetNumberOfLines(std::size_t lines);

  /**
   * @copydoc Toolkit::TextInput::GetNumberOfLines
   */
  std::size_t GetNumberOfLines();

  /**
   * @copydoc Toolkit::TextInput::GetNumberOfCharacters
   */
  std::size_t GetNumberOfCharacters() const;

private:

  /**
   * structure to hold each highlight box needed for text selection
   */
  struct HighlightBox
  {
    Vector3 size;                         ///< size of the highlight box
    Vector3 position;                     ///< position of highlight box
    ImageActor highlightBoxActor;         ///< as actor that is the highlight box
  };

  /**
   * structure to hold each character in displayed string and its position from the left
   */
  struct CharPositions
  {
    char character;   ///< todo change to UTF to aid multi-language support
    Vector3 position;
    Vector2 size;
  };

  /**
   * structure to hold coordinates of each quad, which will make up the mesh.
   */
  struct QuadCoordinates
  {
    /**
     * Default constructor
     */
    QuadCoordinates()
    {
    }

    /**
     * Constructor
     * @param[in] x1 left co-ordinate
     * @param[in] y1 top co-ordinate
     * @param[in] x2 right co-ordinate
     * @param[in] y2 bottom co-ordinate
     */
    QuadCoordinates(float x1, float y1, float x2, float y2)
    : min(x1, y1),
      max(x2, y2)
    {
    }

    Vector2 min;                          ///< top-left (minimum) position of quad
    Vector2 max;                          ///< bottom-right (maximum) position of quad
  };

  typedef std::vector<QuadCoordinates> QuadContainer;

  /**
   * structure for information required to build the highlight mesh
   */
  struct HighlightInfo
  {
    /**
     * Adds a Quad (2D rectangular sub-selection)
     * @param[in] x1 left co-ordinate
     * @param[in] y1 top co-ordinate
     * @param[in] x2 right co-ordinate
     * @param[in] y2 bottom co-ordinate
     */
    void AddQuad( float x1, float y1, float x2, float y2 );

    /**
     * Clamps all quads to fit within a min -> max 2D boundary.
     */
    void Clamp2D(const Vector2& min, const Vector2& max);

    QuadContainer mQuadList;                                 ///< List of quads (sub-selections that form to create complete selection)
  };

  /**
   * Holds requested selection start and end points for highlighted text.
   */
  struct SelectionParameters
  {
    SelectionParameters( size_t start, size_t end )
    : mStartOfSelection( start ), mEndOfSelection( end )
    {

    }

    size_t mStartOfSelection;
    size_t mEndOfSelection;
  };

  enum State
  {
    StateEdit,
    StateDraggingHandle
  };

  enum SelectionHandleId
  {
    HandleOne,   ///< Selection handle one which is on the left
    HandleTwo    ///< Selection handle two which is on the right
  };

  /**
   * Two different behaviours are needed to convert a touch point into a character index.
   * When a tap is received and the touch point doesn't hit any character, the final character selected might
   * be different than the one selected if the event is a pan.
   * i.e. If a tap is received and the touch point doesn't hit any character the expected position of the cursor
   * would be the end or the beginning of a line. However, this behaviour would be weird while panning.
   */
  enum TouchToIndex
  {
    TapMode,  ///< Touch point to character index conversion mode used for Tap events.
    DragMode  ///< Touch point to character index conversion mode used for Pan events.
  };

  /**
   * Used to set the direction when find the next visible character.
   */
  enum FindVisibleCharacterDirection
  {
    Left,  ///< Find visible characters on the left.
    Right, ///< Find visible characters on the right.
    ByEnd  ///< Start finding visible characters by the end.
  };

  /**
   *
   */
  virtual bool OnTouchEvent(const TouchEvent& event);

  /**
   * From CustomActorImpl; called after the Text Input actor is touched
   * @param[in] event The KeyEvent event.
   * @return True if the event should be consumed.
   */
  virtual bool OnKeyEvent(const KeyEvent& event);

  /**
   * From CustomActorImpl; called when this actor gains keyboard focus.
   */
  virtual void OnKeyInputFocusGained();

  /**
   * From CustomActorImpl; called when this actor loses keyboard focus.
   */
  virtual void OnKeyInputFocusLost();

  /**
   * From ControlImpl; called whenever the control is added to the stage.
   */
  virtual void OnControlStageConnection();

private: // From ControlImpl

  /**
   * Creation of the layer that is used by top level active parts of the TextInput like handles
   */
  void CreateActiveLayer();

  /**
   * @copydoc Toolkit::ControlImpl::OnInitialize()
   */
  virtual void OnInitialize();

  /**
   * @copydoc Toolkit::ControlImpl::OnControlSizeSet()
   */
  virtual void OnControlSizeSet(const Vector3& targetSize);

  /**
   * @copydoc Toolkit::ControlImpl::OnRelaidOut()
   */
  virtual void OnRelaidOut( Vector2 size, ActorSizeContainer& container );

  /**
   * Retrieves the text-input's natural size by calling TextView::GetNaturalSize().
   *
   * @return The natural size.
   */
  virtual Vector3 GetNaturalSize();

  /**
   * Retrieves the text-input's \e height for a given \e width by calling TextView::GetHeightForWidth().
   *
   * @param[in] width The given \e width.
   *
   * @return The \e height for the given \e width.
   */
  virtual float GetHeightForWidth( float width );

protected:

  /**
   * Construct a new TextInput.
   */
  TextInput();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~TextInput();

private:

  // Undefined
  TextInput(const TextInput&);

  // Undefined
  TextInput& operator=(const TextInput& rhs);

  /**
    * Callback when a handle is panned/moved, either selection handles or grab handle
    * @param actor Handle of the selection or grab handle.
    * @param gesture Data structure with the parameters of the gesture.
    */
   void OnHandlePan(Actor actor, PanGesture gesture);

   /**
    * Callback for touch down on Grab handle
    * @param[in] actor touched
    * @param[in] touch touch event
    */
   bool OnPressDown(Dali::Actor actor, const TouchEvent& touch);

   /**
    * Callback for touch down on Selection handle one
    * @param[in] actor touched
    * @param[in] touch touch event, used to determine if down or up event
    */
   bool OnHandleOneTouched(Dali::Actor actor, const TouchEvent& touch);

   /**
    * Callback for touch down on Selection handle two
    * @param[in] actor touched
    * @param[in] touch touch event, used to determine if down or up event
    */
   bool OnHandleTwoTouched(Dali::Actor actor, const TouchEvent& touch);

   /**
    * Callback for tap on TextInput
    * @param[in] actor
    * @param[in] tap touch event
    */
   void OnTextTap(Dali::Actor actor, Dali::TapGesture tap);

   /**
    * Callback for double tap on TextInput
    * @param[in] actor
    * @param[in] tap touch event
    */
   void OnDoubleTap(Dali::Actor actor, Dali::TapGesture tap);

    /**
     * Callback for long press on TextInput
     * @param[in] actor
     * @param[in] longPress long press event
     */
   void OnLongPress(Dali::Actor actor, Dali::LongPressGesture longPress);

   /**
    * Callback for the ClipboardEventNotifier when text is selected in the clipboard window.
    * @param[in] notifier The Clipboard Event Notifier.
    */
   void OnClipboardTextSelected( ClipboardEventNotifier& notifier );

   /**
    * Callback for when a button is pressed in popup panel
    * @param[in] button handle to the button pressed.
    */
   bool OnPopupButtonPressed( Toolkit::Button button );

   /**
    * Callback when handle timer ticks.
    *
    * Cursor should become visible/invisible to simulate blinking.
    *
    * @return True if the timer should be keep running.
    */
   bool OnCursorBlinkTimerTick();

   /**
    * Invoked upon popup Hide animation completing.
    * @note Only called for animating hide, not called for instantaneous (animate = false)
    * @param[in] popup The popup which was hidden.
    */
   void OnPopupHideFinished(TextInputPopup& popup);

   /**
    * Called in OnKeyEvent to handle key down events.
    * @param[in] event The KeyEvent event.
    * @return True if the event should be consumed.
    */
   bool OnKeyDownEvent(const KeyEvent& event);

   /**
    * Called in OnKeyEvent to handle key up events.
    * @param[in] event The KeyEvent event.
    * @return True if the event should be consumed.
    */
   bool OnKeyUpEvent(const KeyEvent& event);

   /**
    * Callback called when the text-view is scrolled.
    *
    * Updates the selection and grab handles, and the highlighted text.
    *
    * @param[in] textView Handle of the text-view.
    * @param[in] scrollPosition The difference with the previous scroll position.
    */
   void OnTextViewScrolled( Toolkit::TextView textView, Vector2 scrollPosition );

   /**
    * Scrolls the text-view to make the cursor visible.
    *
    * @param[in] cursorPosition The actual cursor position in actor coordinates.
    */
   void ScrollTextViewToMakeCursorVisible( const Vector3& cursorPosition );

   /**
    * Creates and starts a timer to scroll the text when handles are close to the edges of the text-input.
    *
    * It only starts the timer if it's already created.
    */
   void StartScrollTimer();

   /**
    * Stops the timer used to scroll the text.
    */
   void StopScrollTimer();

   /**
    * Callback called by the timer used to scroll the text.
    *
    * It calculates and sets a new scroll position.
    */
   bool OnScrollTimerTick();

public:  // Public to allow internal testing.

  /**
   * Register for touch events
   */
  void SetUpTouchEvents();

  /**
   * Sets up TextView Actor
   */
  void CreateTextViewActor();

  /**
   * Set Styled Text for text input.
   * @param[in] styleText The new styled text for the text input.
   */
  void SetText( const MarkupProcessor::StyledTextArray& styleText );

  /**
   * Start a timer to signal cursor to blink.
   */
  void StartCursorBlinkTimer();

  /**
   * Stop the timer signalling the cursor to blink.
   */
  void StopCursorBlinkTimer();

  /**
   * Starts input, setting focus and showing keyboard..
   */
  void StartEditMode();

  /**
   * Called when End of input and focus no longer required, keyboard is hidden.
   */
  void EndEditMode();

  /**
   * Applies a style to the current pre-edit / predicted word to show it is being edited.
   * @param[in] preEditStartPosition position in text array that the predicted word starts at
   * @param[in] preEditStringLength used to calculate how many characters need their style changed.
   */
  void ApplyPreEditStyle( std::size_t preEditStartPosition, std::size_t preEditStringLength );

  /**
   * Restores style to value before applying Pre-Edit style.
   */
  void RemovePreEditStyle();

  /**
   * Event received from IMF manager
   * @return ImfCallbackData data struture undicating if update is needed, cursor position and current text
   */
  ImfManager::ImfCallbackData ImfEventReceived( Dali::ImfManager& imfManager, const ImfManager::ImfEventData& test );

  /**
   * Called when the OnKey event is a Pre-edit string
   * @param[in] keyString String received in Pre-edit
   * @param[in] cursorOffset the cursor offset from where the pre-edit word starts
   * @return bool true if preedit reset is required.
   */
  bool PreEditReceived( const std::string& keyString, std::size_t cursorOffset  );

 /**
  * Called when the OnKey event is a Commit string
  * @param[in] keyString String received in Comment
  * @return update flag to trigger cursor update of TextInput only when needed.
  */
  bool CommitReceived( const std::string& keyString );

  /**
   * Deletes Pre-edit string
   * By default it doesn't update the character's size and position table, which is a costly
   * operation. As in many cases deletion and insertion (InsertAt) of text
   * occurs in the same action i.e. preedit/commit. It makes sense to
   * delete without updating, and then insert with updating.
   *
   * @return The number of characters to be deleted.
   */
  std::size_t DeletePreEdit();

  /**
   * Reset all pre-edit flag and signal IMF keyboard that the current pre-edit word has been comitted.
   * This may be due to the cursor being moved by user or reached the max character limit.
   * @param[in] preserveCursorPosition Set true to keep cursor in current position, eg. touch event caused cursor to move.
   */
  void PreEditReset( bool preserveCursorPosition );

  /**
   * Called after cursor position needs updating.
   * Redraws cursor and notifies VirtualKeyboard
   */
  void CursorUpdate();

  /**
   * Delete highlighted characters
   * @param[in] inheritStyle Whether style from previous character in the string should be inherited.
   */
  void DeleteHighlightedText( bool inheritStyle );

  /*
   * Delete range of characters
   * @param[in] start position of characters to delete
   * @param[in] ncharacters number of characters to delete
   */
  void DeleteRange(std::size_t start, std::size_t ncharacters);

  /**
   * Delete character at current cursor position and redisplay
   * @param[in] positionToDelete position of character to delete
   */
  void DeleteCharacter( std::size_t positionToDelete );

  /**
   * Add or replaces characters to currently displayed string at cursor position
   * By default it doesn't update the character's size and position table, which is a costly
   * operation. As in many cases deletion and insertion (InsertAt) of text
   * occurs in the same action i.e. preedit/commit. It makes sense to
   * delete without updating, and then insert with updating.
   * @param[in] newText string to add to TextInput display string.
   * @param[in] insertionPosition position to insert at.
   * @param[in] numberOfCharactersToReplace The number of characters to replace.
   * @return number of characters to offset the cursor by.
   */
  std::size_t InsertAt( const Text& newText, const std::size_t insertionPosition, const std::size_t numberOfCharactersToReplace );

  /**
   * Creates a cursor from the supplied image
   * @param[in] cursorImage the image to be used for the cursor.
   * @param[in] border the nine patch border corresponding to the supplied image.
   * @return the image actor to be used as the cursor.
   */
  ImageActor CreateCursor( Image cursorImage, const Vector4& border );

  /**
   * Moves cursor to the right
   * param[in] reverse if true then cursor moves in the reverse direction (to the left)
   * param[in] places number of character cursor should move.
   */
  void AdvanceCursor(bool reverse = false, std::size_t places = 1);

  /**
   * Draw a cursor / caret at position where new text should appear
   * @param[in] nthChar the position along the text string in which new text should appear.
   */
  void DrawCursor(const std::size_t nthChar = 0);

  /**
   * Sets cursor visibility
   * This sets visibility of the cursor. Which is comprised of 2
   * cursors. The conventional cursor, and the alternate (RTL) cursor,
   * which only appears when the cursor is at a character that can have
   * a character appended to different visual positions depending on whether that
   * character to be appended is RTL or LTR.
   * @param[in] visible true - enable visibility for cursor, false - disable visiblity for cursor
   */
  void SetCursorVisibility( bool visible );

  /**
   * Sets alternate cursor enable state
   * @see SetCursorVisibility
   * alternate cursor will only be visible if both SetCursorVisiblity
   * and cursor enabled have been set to true.
   */
  void SetAltCursorEnabled( bool enabled );

  /**
   * Create the grab handle that positions the cursor
   * @param[in] image to be used for grab handle
   *
   */
  void CreateGrabHandle(Image image=Image());

  /**
   * Create Grab area to be used by Grab Handle
   */
  void CreateGrabArea( Actor& parent);

  /**
   * Move grab handle to the required character position
   *
   * @param[in] displacement Displacement of the grab handle in actor coordinates.
   *
   * @return The new actual position the handle has been set to.
   */
  Vector3 MoveGrabHandle( const Vector2& displacement );

  /**
   * Show or hide the grab handle without baking the visibility flag.
   * Used when the Grab handle needs to be invisible due to text-view scrolling making it out of view
   *
   * @param[in] visible bool flag to set as true is grab handle should be shown, else false to hide.
   */
  void ShowGrabHandle( bool visible );

  /**
   * Show or hide the grab handle and bake the visibility flag.
   * Used when the state of text-input changes to a state which the grabhandle is not required. E.g. Selection mode starts or edit mode ends.
   * Calls ShowGrabHandle.
   *
   * @param[in] visible bool flag to set as true is grab handle should be shown, else false to hide.
   */
  void ShowGrabHandleAndSetVisibility( bool visible );

  /* Boundary Property Notifications when handle exceed bounding box*/

  /**
   * PropertyNotification Callback when left boundary exceeded so handle can be flipped.
   * @param[in] source PropertyNotification
   */
  void OnLeftBoundaryExceeded( PropertyNotification& source );
  /**
   * PropertyNotification Callback when within left boundary so handle can be flipped back.
   * @param[in] source PropertyNotification
   */
  void OnReturnToLeftBoundary( PropertyNotification& source );
  /**
   * PropertyNotification Callback when right boundary exceeded so handle can be flipped.
   * @param[in] source PropertyNotification
   */
  void OnRightBoundaryExceeded( PropertyNotification& source );
  /**
   * * PropertyNotification Callback when within right boundary so handle can be flipped back.
   * @param[in] source PropertyNotification
   */
  void OnReturnToRightBoundary( PropertyNotification& source );

  /**
   * PropertyNotification Callbacks for hiding handle one when it exceeds boundary.
   * @param[in] source PropertyNotification
   */
  void OnHandleOneLeavesBoundary( PropertyNotification& source );
  /**
   * PropertyNotification Callbacks for showing hidden handle one when returns within boundary
   * @param[in] source PropertyNotification
   */
  void OnHandleOneWithinBoundary( PropertyNotification& source );
  /**
   * PropertyNotification Callbacks for hiding handle two it  when exceeds boundary.
   * @param[in] source PropertyNotification
   */
  void OnHandleTwoLeavesBoundary( PropertyNotification& source );
  /**
   * PropertyNotification Callbacks for showing hidden handle two when returns within boundary
   * * @param[in] source PropertyNotification
   */
  void OnHandleTwoWithinBoundary( PropertyNotification& source );

  /**
   * Set up property notifications on the position of the handles to facilitate flipping and hiding when at screen boundary.
   */
  void SetUpHandlePropertyNotifications();

  /**
   * Create the selection handles that bound text to be selected for copy/cut.
   * @param[in] start initial position of start selection handle.
   * @param[in] end initial position of end selection handle.
   * @param[in] handleOneImage (optional) to be used for selection handle
   * @param[in] handleTwoImage (optional) to be used for selection handle
   */
  void CreateSelectionHandles( std::size_t start = 0, std::size_t end = std::numeric_limits<std::size_t>::max(), Dali::Image handleOneImage = Dali::Image(),  Dali::Image handleTwoImage = Dali::Image() );

  /**
   * Move the selection handles to required positions in text.
   *
   * @param[in] handleId the handle to position
   * @param[in] displacement Displacement of the selection handle in actor coordinates.
   *
   * @return The new actual position the handle has been set to.
   */
  Vector3 MoveSelectionHandle(SelectionHandleId handleId, const Vector2& displacement);

  /**
   * Calculate and position the specified selection handle the given index position
   *
   * @param[in] handleId the handle to position
   */
  void SetSelectionHandlePosition(SelectionHandleId handleId);

  /**
   * Gets the visual position of a logical position.
   * @note This is preferred over directly accessing the Map, as it resolves visual
   * positions outside of the character map range.
   * @param[in] logicalPosition The logical position
   * @return Visual position is returned.
   */
  std::size_t GetVisualPosition(std::size_t logicalPosition) const;

  /**
   * Gets a table of the visual text positions which has a flag
   * for each Character. The flag is either true (character selected)
   * or false (character deselected)
   * @note startSelection can be greater or less than endSelection
   *
   * @param[in,out] selectedVisualText The vector to be resized and populated with the selected flags
   * @param[in] startSelection The start selection point for the text
   * @param[in] endSelection The end selection point for the text
   */
  void GetVisualTextSelection(std::vector<bool>& selectedVisualText, std::size_t startSelection, std::size_t endSelection);

  /**
   *  Iterates between selection handles and computes the info required to build the highlight mesh
   */
  HighlightInfo CalculateHighlightInfo();

  /**
   * Calculates new Mesh data so highlight moves with selection handles.
   */
  void UpdateHighlight();

  /**
   * Removes popup, and its options.
   */
  void ClearPopup();

  /**
   * Adds a popup option.
   * @note Creates popup frame if not already created.
   * @param[in] name The unique name for this option.
   * @param[in] caption The caption (label) for this option
   * @param[in] icon the image icon to be displayed for this option
   * @param[in] finalOption Flag to indicate that this is the final option.
   * (set to true on the last option you add)
   */
  void AddPopupOption(const std::string& name, const std::string& caption, const Image icon, bool finalOption = false);

  /**
   * Sets popup position
   * @param[in] position The actual position for this popup.
   */
  void SetPopupPosition(const Vector3& position);

  /**
   * Hides the popup
   * @param[in] animate (optional) whether to animate popup to hide state over time (i.e. tween).
   * @param[in] signalFinished (optional) whether to perform an animation finish operation after the hide animation completes. Requires animate to be true.
   */
  void HidePopup( bool animate = true, bool signalFinished = true );

  /**
   * Shows the popup
   * @param[in] animate (optional) whether to animate popup to show state over time (i.e. tween).
   */
  void ShowPopup(bool animate = true);

  /**
   * Shows the cut-copy-paste popup
   */
  void ShowPopupCutCopyPaste();

  /**
   * Setup the selection popup and clipboard if relevant so the correct options are shown when ShowPopup is called.
   */
  void SetUpPopUpSelection();

  /**
   * Return the logical index containing the character position closest to the source.
   * Used for positioning the grab handle at characters when dragged along.
   * Two different behaviours are needed in case the source point doesn't actually touch a
   * character. @see TouchToIndex.
   * @param[in] source float to match against
   * @param[out] closestIndex index to the vector of character's size and position.
   * @return \e true if the source point is actually inside the geometry provided by TextView.
   */
  bool ReturnClosestIndex(const Vector2& source, std::size_t& closestIndex );

  /**
   * Returns the X-position of the current line justification
   * (relative to left of text-view container)
   * @return X position for line justification
   */
  float GetLineJustificationPosition() const;

  /**
   * Currently the cursor is positioned at the previous characters position plus it's width.
   * If the previous character is on a different line then this function returns the correct position.
   * @param[in] characterPosition the character position index that the cursor should be at
   * @return position of cursor/handle
   */
  Vector3 PositionCursorAfterWordWrap( std::size_t characterPosition ) const;


  /**
   * Return a vector which is the actual position for the given character position
   * The character position is where a cursor would be position for that character.
   * @param[in] characterPosition the logical (input) position in the 'string' of characters.
   *
   * @return Vector3 the actual x,y,z position
   */
  Vector3 GetActualPositionFromCharacterPosition(std::size_t characterPosition ) const;

  /**
   * Return a vector which is the actual position for the given character position
   * The character position is where a cursor would be positioned for that character to be inserted.
   * An additional alternatePosition is also set in circumstances where the possible writing
   * of characters would be in the opposite direction.
   * e.g. "HelloشقشلاهؤEnglish"
   *            |     |
   *            *     +
   * [*] - Primary actual position for cursor i.e. continuing writing LTR (English)
   * [+] - Alternate actual position for cursor i.e. writing RTL (Arabic)
   *
   * @param[in] characterPosition the logical (input) position in the 'string' of characters.
   * @param[out] directionRTL Whether the actual x,y,z position is after LTR (false) or RTL (true) text.
   * @param[out] alternatePosition the actual x,y,z position of the cursor if user types
   * in alternate direction to current flow of text.
   * @param[out] alternatePositionValid whether this alternate position is valid.
   * @return Vector3 the actual x,y,z position
   */
  Vector3 GetActualPositionFromCharacterPosition(std::size_t characterPosition, bool& directionRTL, Vector3& alternatePosition, bool& alternatePositionValid ) const;

  /**
   * Retrieve the character position of the first character on the row of text
   * that this character resides on.
   * @param[in] logicalPosition the position in the 'string' of characters.
   * @return logical character position of start of row.
   */
  std::size_t GetRowStartFromCharacterPosition(std::size_t logicalPosition) const;

  /**
   * Retrieve the dimensions of this row of text that the character resides on.
   * @param[in] characterPosition the position in the 'string' of characters.
   * @return The size of the rectangle representing this row
   */
  Size GetRowRectFromCharacterPosition(std::size_t characterPosition) const;

  /**
   * Retrieve the dimensions (and min-max) of this row of text that the character resides on.
   * @param[in] characterPosition the position in the 'string' of characters.
   * @param[out] min the top-left position of the rectangle representing this row
   * @param[out] max the bottom-right position of the rectangle representing this row
   * @return The size of the rectangle representing this row (max - min)
   */
  Size GetRowRectFromCharacterPosition(std::size_t characterPosition, Vector2& min, Vector2& max) const;

  /**
   * Checks if the provided touchedActor was this text-input
   * @param[in] touchedActor the touched actor that will be checked against this text-input
   * @return true if the touchActor was the text-input or one of its children
   */
  bool WasTouchedCheck( const Actor& touchedActor ) const;

  /**
   * Connect to the stage touch event
   */
  void StartMonitoringStageForTouch();

  /**
   * Disconnect from the stage touch event
   */
  void EndMonitoringStageForTouch();

  /**
   * Callback for when the screen is touched, this will be connected when selection starts and disconnected when it ends.
   * @param[in] event The touch event
   */
  void OnStageTouched(const TouchEvent& event);


  /**
   * Select the text between the given values
   * @param[in] start position within array to start selection
   * @param[in] end position within array to end selection
   */
  void SelectText(std::size_t start, std::size_t end);

  /**
   * Gets selected text and returns it as a StyleText vector
   * @return StyledText vector that is selected.
   */
  MarkupProcessor::StyledTextArray GetSelectedText();

  /**
   * Applies the given style to all text, selected or not selected.
   * By default all style settings are applied but a bit mask could be used to modify only certain style settings.
   */
  void ApplyStyleToRange(const TextStyle& style, const TextStyle::Mask mask, const std::size_t begin, const std::size_t end);

  /**
   * Callback for when the keyboard status changes
   * @param[in]  keyboardShown  Whether the keyboard has just been shown or not.
   */
  void KeyboardStatusChanged(bool keyboardShown);

  /**
   * Hide highlight shown between selection handles.
   */
  void RemoveHighlight();

  /**
   * Highlights text that has been selected
   */
  void CreateHighlight();

  /**
   * Copies selected text to clipboard
   * @return bool true if copy was successful.
   */
  bool CopySelectedTextToClipboard();

  /**
   * Pastes the given text to currently displayed string at the current cursor position
   * @param[in] text Text to be pasted
   */
  void PasteText( const Text& text );

  /**
   * Sets the direction of the text if it is empty.
   */
  void SetTextDirection();

  /**
   * Updates the line height accordingly with the current text input style.
   */
  void UpdateLineHeight();

  /**
   * Finds a visible character.
   *
   * The \e direction parameters specifies from where to start to look for a visible character. \e Left means start by characters in lower
   * positions than \e cursorPosition, \e Right means start by characters in greater positions than \e cursorPosition and \e ByEnd starts
   * by the last characters.
   *
   * If \e Left or \e Right is provided and any character is found, then looks in the other direction.
   *
   * @param[in] direction Direction used to find a visible character.
   * @param[in] cursorPosition Starting position.
   *
   * @return The found visible character.
   */
  std::size_t FindVisibleCharacter( const FindVisibleCharacterDirection direction , const std::size_t cursorPosition ) const;

  /**
   * @copydoc SetSortModifier()
   */
  void SetSortModifier( float depthOffset );

  /**
   * @copydoc SetSnapshotModeEnabled()
   */
  void SetSnapshotModeEnabled( bool enable );

  /**
   * @copydoc IsSnapshotModeEnabled()
   */
  bool IsSnapshotModeEnabled() const;

  /**
   * @copydoc SetScrollEnabled()
   */
  void SetScrollEnabled( bool enable );

  /**
   * @copydoc IsScrollEnabled()
   */
  bool IsScrollEnabled() const;

  /**
   * @copydoc SetScrollPosition()
   */
  void SetScrollPosition( const Vector2& position );

  /**
   * @copydoc GetScrollPosition()
   */
  Vector2 GetScrollPosition() const;

  /**
   * Insert or replaces the given text in the given position. It checks if the text exceeds the maximum allowed number of characters
   * and if it fits in the text-input's boundaries.
   *
   * @param[in] text Text to be inserted.
   * @param[in] position Position where the text is inserted.
   * @param[in] numberOfCharactersToReplace The number of characters to replace.
   * @param[out] textExceedsMaximunNumberOfCharacters The number of characters that have been inserted.
   * @param[out] textExceedsBoundary Whether the inserted text has exceeded the boundaries of the text-input.
   *
   * @return The number of characters that have been inserted.
   */
  std::size_t DoInsertAt( const Text& text, std::size_t position, std::size_t numberOfCharactersToReplace, bool& textExceedsMaximunNumberOfCharacters, bool& textExceedsBoundary );

  /**
   *  Retrieve Text-view's layout info.
   */
  void GetTextLayoutInfo();

  /**
   * Emits the style changed signal.
   */
  void EmitStyleChangedSignal();

  /**
   * Emits max input characters reached signal.
   */
  void EmitMaxInputCharactersReachedSignal();

  /**
   * Emits a signal when the input text exceeds the boundaries of the text-input.
   */
  void EmitInputTextExceedsBoundariesSignal();

private:

  State mState;                                 ///< Current State of Text input.
  MarkupProcessor::StyledTextArray mStyledText; ///< String currently displayed by TextView with style info.
  TextStyle mInputStyle;                        ///< Stores the current input style.
  float     mLineHeight;                        ///< Stores the current line height accordingly with the input style.
  Toolkit::TextView mDisplayedTextView;         ///< Actor which actually display text

  MarkupProcessor::StyledTextArray mStyledPlaceHolderText; ///< Text used as place holder with style info.

  std::size_t mMaxStringLength;                 ///< Max number of characters for string
  std::size_t mNumberOflinesLimit;              ///< Limit on the number of lines this TextInput will display.

  ImageActor mCursor;                           ///< Cursor overlayed on Text to show where new text will be inserted
  ImageActor mCursorRTL;                        ///< Right To Left Cursor overlayed on Text (where new RTL text would be inserted)
  Animation mCursorAnimation;                   ///< animator for cursor blinking.
  std::size_t mCursorPosition;                  ///< position along string cursor is at.
  Timer mCursorBlinkTimer;                      ///< Timer to signal cursor to blink

  Layer mActiveLayer;                           ///< Layer for active handles and alike that ensures they are above all else.

  Image mGrabHandleImage;                       ///< image to be used for grab handle
  ImageActor mGrabHandle;                       ///< Handle used to move cursor for editing

  Actor mGrabArea;                              ///< invisible actor that receives pans events for the grab handle.
  PanGestureDetector  mPanGestureDetector;      ///< Pan gesture for handles
  TapGestureDetector mTapDetector;              ///< used to start editing
  TapGestureDetector mDoubleTapDetector;        ///< double tap detector

  Vector3 mActualGrabHandlePosition;            ///< actual position of grab handle, this might not be snapped to a character

  LongPressGestureDetector mLongPressDetector;  ///< Gesture to start selection

  /*Selection handles*/
  Image mSelectionHandleOneImage;               ///< image used for selection handle one
  Image mSelectionHandleOneImagePressed;        ///< image used for selection handle one pressed state
  Image mSelectionHandleTwoImage;               ///< image used for selection handle two
  Image mSelectionHandleTwoImagePressed;        ///< image used for selection handle two pressed state

  bool mIsSelectionHandleOneFlipped:1;          ///< Flag to know whether the handle one is flipped or not.
  bool mIsSelectionHandleTwoFlipped:1;          ///< Flag to know whether the handle two is flipped or not.
  Vector3 mSelectionHandleOneOffset;            ///< Handle One's Offset
  Vector3 mSelectionHandleTwoOffset;            ///< Handle Two's Offset
  Vector3 mSelectionHandleOneActualPosition;    ///< Actual x y position of handle
  Vector3 mSelectionHandleTwoActualPosition;    ///< Actual x y position of handle
  std::size_t mSelectionHandleOnePosition;      ///< Position of handle along the string of text
  std::size_t mSelectionHandleTwoPosition;      ///< Position of handle along the string of text
  ImageActor mSelectionHandleOne;               ///< First selection handle used for selecting text to cut&paste
  ImageActor mSelectionHandleTwo;               ///< Second selection handle used for selecting text to cut&paste
  Actor mHandleOneGrabArea;                     ///< invisible actor that receives pans events for the selection handle.
  Actor mHandleTwoGrabArea;                     ///< invisible actor that receives pans events for the selection handle.

  Mesh              mHighlightMesh;             ///< Mesh Data for highlight
  MeshActor         mHighlightMeshActor;        ///< Mesh Actor to display highlight
  MeshData          mMeshData;                  ///< Container to hold meshData for highlight
  Material          mCustomMaterial;            ///< Custom material used for highlight
  HighlightInfo     mNewHighlightInfo;          ///< Geometry info to create highlight.

  Text mPreEditString;                          ///< Holds current input string prior to it being committed.
  std::size_t mPreEditStartPosition;            ///< Cursor position for start of pre-edit string
  std::size_t mPreEditLength;                   ///< Cursor position for length of current pre-edit string after purging characters that do not fit.
  std::size_t mNumberOfSurroundingCharactersDeleted; ///< Stores the number of surrounding characters deleted.
  unsigned long mTouchStartTime;                ///< Touch Start time (time in ms when down press began)

  Toolkit::TextView::TextLayoutInfo                                 mTextLayoutInfo; ///< It contains a table layout info per character sorted by the character's visual index (retrieved from TextView),
                                                                                     ///< a reorder map that stores each character's visual (output) index according to its logical (input) index,
                                                                                     ///< a reorder map that stores each character's logical (input) index according to its visual (output) index,
                                                                                     ///< the text size after layout and the scroll offset.

  MarkupProcessor::StyledTextArray mCurrentCopySelecton;                                              ///< Array to store copied text.
  TextInputPopup mPopUpPanel;                                                                         ///< Panel to house cut and paste, select all buttons.

  Timer mScrollTimer;
  Vector2 mScrollDisplacement;
  Vector3 mCurrentHandlePosition;
  SelectionHandleId mCurrentSelectionId;
  Vector3 mCurrentSelectionHandlePosition;
  SelectionParameters mRequestedSelection;
  Vector4 mSelectionHandleFlipMargin;
  Vector4 mBoundingRectangleWorldCoordinates;

  Clipboard mClipboard;                  ///< Handle to clipboard

  bool mOverrideAutomaticAlignment:1;    ///< Whether to override the alignment automatically set by the text content (e.g. european LTR or arabic RTL)
  bool mCursorRTLEnabled:1;              ///< Enable state of Alternate RTL Cursor (need to keep track of this as it's not always enabled)
  bool mClosestCursorPositionEOL:1;      ///< closest cursor position is end of line.
  bool mCursorBlinkStatus:1;             ///< \e true shows the cursor, \e false hiddes it.

  bool mCursorVisibility:1;              ///< Visibility status of Cursor
  bool mGrabHandleVisibility:1;          ///< Visibility status of the grab handle.

  bool mIsCursorInScrollArea:1;          ///< Whether the cursor is inside the boundaries of the text-input.
  bool mIsGrabHandleInScrollArea:1;      ///< Whether the grab handle is inside the boundaries of the text-input.

  bool mEditModeActive:1;                ///< true to indicate TextInput is in edit mode.
  bool mEditOnTouch:1;                   ///< true indicates edit mode starts by touching/tapping the TextInput
  bool mTextSelection:1;                 ///< true indicates text selection is enabled.
  bool mExceedEnabled:1;                 ///< Whether text-input editing text beyond its boundary is enabled or not. By default is enabled.
  bool mGrabHandleEnabled:1;             ///< Flag to enable the grab handle instead of the default magnifier.
  bool mIsSelectionHandleFlipEnabled:1;  ///< Flag to enable the selection handle flip

  bool mPreEditFlag:1;                   ///< Flag to indicate if key string received is still in pre-edit mode (Ecore IMF keyboard)
  bool mIgnoreCommitFlag:1;              ///< Flag to indicate if the commit string should be ignored, maybe due to a keyboard reset.
  bool mIgnoreFirstCommitFlag:1;         ///< Whether to ignore the first commit.
  bool mSelectingText:1;                 ///< Ignore surrounding text as selecting text

  bool mPreserveCursorPosition:1;        ///< Indicates that the commit message is from a reset and does not require the cursor to be moved

  bool mSelectTextOnCommit:1;

  bool mUnderlinedPriorToPreEdit:1;      ///< As predictive text adds underline style this flag enables us to revert back or keep underline.

  bool mCommitByKeyInput:1;              ///< Set if a commit is done by key input rather than automatically (usually when a space bar or enter is pressed).

  bool mPlaceHolderSet:1;                ///< Whether the place holder text is set.

  Toolkit::TextInput::InputSignalV2                      mInputStartedSignalV2;              ///< Signal emitted when input starts
  Toolkit::TextInput::InputSignalV2                      mInputFinishedSignalV2;             ///< Signal emitted when input ends
  Toolkit::TextInput::StyleChangedSignalV2               mStyleChangedSignalV2;              ///< Signal emitted when style changes.
  Toolkit::TextInput::MaxInputCharactersReachedSignalV2  mMaxInputCharactersReachedSignalV2; ///< Signal emitted when max input characters is reached.
  Toolkit::TextInput::InputSignalV2                      mCutAndPasteToolBarDisplayedV2;     ///< Signal emitted when toolbar displayed
  Toolkit::TextInput::InputTextExceedBoundariesSignalV2  mInputTextExceedBoundariesSignalV2; ///< Signal emitted when input text exceeds the boundaries of the text-input.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::TextInput& GetImpl(Toolkit::TextInput& textInput)
{
  DALI_ASSERT_ALWAYS(textInput);

  Dali::RefObject& handle = textInput.GetImplementation();

  return static_cast<Toolkit::Internal::TextInput&>(handle);
}

inline const Toolkit::Internal::TextInput& GetImpl(const Toolkit::TextInput& textInput)
{
  DALI_ASSERT_ALWAYS(textInput);

  const Dali::RefObject& handle = textInput.GetImplementation();

  return static_cast<const Toolkit::Internal::TextInput&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TEXT_INPUT_H__

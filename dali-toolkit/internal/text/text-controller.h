#ifndef __DALI_TOOLKIT_TEXT_CONTROLLER_H__
#define __DALI_TOOLKIT_TEXT_CONTROLLER_H__

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
#include <string>
#include <dali/devel-api/adaptor-framework/imf-manager.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/events/gesture.h>
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/object/ref-object.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/text-controls/text-selection-popup-callback-interface.h>
#include <dali-toolkit/internal/text/decorator/text-decorator.h>
#include <dali-toolkit/internal/text/font-run.h>
#include <dali-toolkit/internal/text/layouts/layout-engine.h>
#include <dali-toolkit/internal/text/text-control-interface.h>
#include <dali-toolkit/internal/text/text-view.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

class Controller;
class LayoutEngine;

typedef IntrusivePtr<Controller> ControllerPtr;
typedef Dali::Toolkit::Text::ControlInterface ControlInterface;

/**
 * @brief Different placeholder-text can be shown when the control is active/inactive.
 */
enum PlaceholderType
{
  PLACEHOLDER_TYPE_ACTIVE,
  PLACEHOLDER_TYPE_INACTIVE,
};

/**
 * @brief A Text Controller is used by UI Controls which display text.
 *
 * It manipulates the Logical & Visual text models on behalf of the UI Controls.
 * It provides a view of the text that can be used by rendering back-ends.
 *
 * For selectable/editable UI controls, the controller handles input events from the UI control
 * and decorations (grab handles etc) via the Decorator::ControllerInterface interface.
 *
 * The text selection popup button callbacks are as well handled via the TextSelectionPopupCallbackInterface interface.
 */
class Controller : public RefObject, public Decorator::ControllerInterface, public TextSelectionPopupCallbackInterface
{
public:

  /**
   * @brief Text related operations to be done in the relayout process.
   */
  enum OperationsMask
  {
    NO_OPERATION       = 0x0000,
    CONVERT_TO_UTF32   = 0x0001,
    GET_SCRIPTS        = 0x0002,
    VALIDATE_FONTS     = 0x0004,
    GET_LINE_BREAKS    = 0x0008,
    GET_WORD_BREAKS    = 0x0010,
    BIDI_INFO          = 0x0020,
    SHAPE_TEXT         = 0x0040,
    GET_GLYPH_METRICS  = 0x0080,
    LAYOUT             = 0x0100,
    UPDATE_ACTUAL_SIZE = 0x0200,
    REORDER            = 0x0400,
    ALIGN              = 0x0800,
    ALL_OPERATIONS     = 0xFFFF
  };

  /**
   * @brief Used to distinguish between regular key events and IMF events
   */
  enum InsertType
  {
    COMMIT,
    PRE_EDIT,
    CLIPBOARD
  };

  /**
   * @brief Create a new instance of a Controller.
   *
   * @param[in] controlInterface An interface used to request a text relayout.
   * @return A pointer to a new Controller.
   */
  static ControllerPtr New( ControlInterface& controlInterface );

  /**
   * @brief Called to enable text input.
   *
   * @note Selectable or editable controls should call this once after Controller::New().
   * @param[in] decorator Used to create cursor, selection handle decorations etc.
   */
  void EnableTextInput( DecoratorPtr decorator );

  /**
   * @brief Replaces any text previously set.
   *
   * @note This will be converted into UTF-32 when stored in the text model.
   * @param[in] text A string of UTF-8 characters.
   */
  void SetText( const std::string& text );

  /**
   * @brief Retrieve any text previously set.
   *
   * @return A string of UTF-8 characters.
   */
  void GetText( std::string& text ) const;

  /**
   * @brief Remove a given number of characters
   *
   * @param[in] cursorOffset Start position from the current cursor position to start deleting characters.
   * @param[in] numberOfChars The number of characters to delete from the cursorOffset.
   * @return True if the remove was successful.
   */
  bool RemoveText( int cursorOffset, int numberOfChars );

  /**
   * @brief Retrieve the current cursor position.
   *
   * @return The cursor position.
   */
  unsigned int GetLogicalCursorPosition() const;

  /**
   * @brief Replaces any placeholder text previously set.
   *
   * @param[in] type Different placeholder-text can be shown when the control is active/inactive.
   * @param[in] text A string of UTF-8 characters.
   */
  void SetPlaceholderText( PlaceholderType type, const std::string& text );

  /**
   * @brief Retrieve any placeholder text previously set.
   *
   * @param[in] type Different placeholder-text can be shown when the control is active/inactive.
   * @param[out] A string of UTF-8 characters.
   */
  void GetPlaceholderText( PlaceholderType type, std::string& text ) const;

  /**
   * @brief Sets the maximum number of characters that can be inserted into the TextModel
   *
   * @param[in] maxCharacters maximum number of characters to be accepted
   */
  void SetMaximumNumberOfCharacters( int maxCharacters );

  /**
   * @brief Sets the maximum number of characters that can be inserted into the TextModel
   *
   * @param[in] maxCharacters maximum number of characters to be accepted
   */
  int GetMaximumNumberOfCharacters();

  /**
   * @brief Set the default font family.
   *
   * @param[in] defaultFontFamily The default font family.
   */
  void SetDefaultFontFamily( const std::string& defaultFontFamily );

  /**
   * @brief Retrieve the default font family.
   *
   * @return The default font family.
   */
  const std::string& GetDefaultFontFamily() const;

  /**
   * @brief Set the default font style.
   *
   * @param[in] defaultFontStyle The default font style.
   */
  void SetDefaultFontStyle( const std::string& defaultFontStyle );

  /**
   * @brief Retrieve the default font style.
   *
   * @return The default font style.
   */
  const std::string& GetDefaultFontStyle() const;

  /**
   * @brief Set the default point size.
   *
   * @param[in] defaultFontStyle The default point size.
   */
  void SetDefaultPointSize( float pointSize );

  /**
   * @brief Retrieve the default point size.
   *
   * @return The default point size.
   */
  float GetDefaultPointSize() const;

  /**
   * @brief Set the text color
   *
   * @param textColor The text color
   */
  void SetTextColor( const Vector4& textColor );

  /**
   * @brief Retrieve the text color
   *
   * @return The text color
   */
  const Vector4& GetTextColor() const;

  /**
   * @brief Set the text color
   *
   * @param textColor The text color
   */
  void SetPlaceholderTextColor( const Vector4& textColor );

  /**
   * @brief Retrieve the text color
   *
   * @return The text color
   */
  const Vector4& GetPlaceholderTextColor() const;

  /**
   * @brief Set the shadow offset.
   *
   * @param[in] shadowOffset The shadow offset, 0,0 indicates no shadow.
   */
  void SetShadowOffset( const Vector2& shadowOffset );

  /**
   * @brief Retrieve the shadow offset.
   *
   * @return The shadow offset.
   */
  const Vector2& GetShadowOffset() const;

  /**
   * @brief Set the shadow color.
   *
   * @param[in] shadowColor The shadow color.
   */
  void SetShadowColor( const Vector4& shadowColor );

  /**
   * @brief Retrieve the shadow color.
   *
   * @return The shadow color.
   */
  const Vector4& GetShadowColor() const;

  /**
   * @brief Set the underline color.
   *
   * @param[in] color color of underline.
   */
  void SetUnderlineColor( const Vector4& color );

  /**
   * @brief Retrieve the underline color.
   *
   * @return The underline color.
   */
  const Vector4& GetUnderlineColor() const;

  /**
   * @brief Set the underline enabled flag.
   *
   * @param[in] enabled The underline enabled flag.
   */
  void SetUnderlineEnabled( bool enabled );

  /**
   * @brief Returns whether the text is underlined or not.
   *
   * @return The underline state.
   */
  bool IsUnderlineEnabled() const;

  /**
   * @brief Set the override used for underline height, 0 indicates height will be supplied by font metrics
   *
   * @param[in] height The height in pixels of the underline
   */
  void SetUnderlineHeight( float height );

  /**
   * @brief Retrieves the override height of an underline, 0 indicates height is supplied by font metrics
   *
   * @return The height of the underline, or 0 if height is not overrided.
   */
  float GetUnderlineHeight() const;

  /**
   * @brief Called to enable/disable cursor blink.
   *
   * @note Only editable controls should calls this.
   * @param[in] enabled Whether the cursor should blink or not.
   */
  void SetEnableCursorBlink( bool enable );

  /**
   * @brief Query whether cursor blink is enabled.
   *
   * @return Whether the cursor should blink or not.
   */
  bool GetEnableCursorBlink() const;

  /**
   * @brief Query the current scroll position; the UI control is responsible for moving actors to this position.
   *
   * @return The scroll position.
   */
  const Vector2& GetScrollPosition() const;

  /**
   * @brief Query the alignment offset.
   *
   * @return The alignmnet offset.
   */
  const Vector2& GetAlignmentOffset() const;

  /**
   * @copydoc Control::GetNaturalSize()
   */
  Vector3 GetNaturalSize();

  /**
   * @copydoc Control::GetHeightForWidth()
   */
  float GetHeightForWidth( float width );

  /**
   * @brief Triggers a relayout which updates View (if necessary).
   *
   * @note UI Controls are expected to minimize calls to this method e.g. call once after size negotiation.
   * @param[in] size A the size of a bounding box to layout text within.
   * @return True if the text model or decorations were updated.
   */
  bool Relayout( const Size& size );

  /**
   * @brief Process queued events which modify the model.
   */
  void ProcessModifyEvents();

  /**
   * @brief Used to remove placeholder text.
   */
  void ResetText();

  /**
   * @brief Used to reset the cursor position after setting a new text.
   *
   * @param[in] cursorIndex Where to place the cursor.
   */
  void ResetCursorPosition( CharacterIndex cursorIndex );

  /**
   * @brief Used to reset the scroll position after setting a new text.
   */
  void ResetScrollPosition();

  /**
   * @brief Used to process an event queued from SetText()
   */
  void TextReplacedEvent();

  /**
   * @brief Used to process an event queued from key events etc.
   */
  void TextInsertedEvent();

  /**
   * @brief Used to process an event queued from backspace key etc.
   */
  void TextDeletedEvent();

  /**
   * @brief Lays-out the text.
   *
   * GetNaturalSize(), GetHeightForWidth() and Relayout() calls this method.
   *
   * @param[in] size A the size of a bounding box to layout text within.
   * @param[in] operations The layout operations which need to be done.
   * @param[out] layoutSize The size of the laid-out text.
   */
  bool DoRelayout( const Size& size,
                   OperationsMask operations,
                   Size& layoutSize );

  /**
   * @brief Whether to enable the multi-line layout.
   *
   * @param[in] enable \e true enables the multi-line (by default)
   */
  void SetMultiLineEnabled( bool enable );

  /**
   * @return Whether the multi-line layout is enabled.
   */
  bool IsMultiLineEnabled() const;

  /**
   * @copydoc Dali::Toolkit::Text::LayoutEngine::SetHorizontalAlignment()
   */
  void SetHorizontalAlignment( LayoutEngine::HorizontalAlignment alignment );

  /**
   * @copydoc Dali::Toolkit::Text::LayoutEngine::GetHorizontalAlignment()
   */
  LayoutEngine::HorizontalAlignment GetHorizontalAlignment() const;

  /**
   * @copydoc Dali::Toolkit::Text::LayoutEngine::SetVerticalAlignment()
   */
  void SetVerticalAlignment( LayoutEngine::VerticalAlignment alignment );

  /**
   * @copydoc Dali::Toolkit::Text::LayoutEngine::GetVerticalAlignment()
   */
  LayoutEngine::VerticalAlignment GetVerticalAlignment() const;

  /**
   * @brief Calulates the alignment of the whole text inside the bounding box.
   *
   * @param[in] size The size of the bounding box.
   */
  void CalculateTextAlignment( const Size& size );

  /**
   * @brief Return the layout engine.
   *
   * @return A reference to the layout engine.
   */
  LayoutEngine& GetLayoutEngine();

  /**
   * @brief Return a view of the text.
   *
   * @return A reference to the view.
   */
  View& GetView();

  // Text-input Event Queuing

  /**
   * @brief Called by editable UI controls when keyboard focus is gained.
   */
  void KeyboardFocusGainEvent();

  /**
   * @brief Called by editable UI controls when focus is lost.
   */
  void KeyboardFocusLostEvent();

  /**
   * @brief Called by editable UI controls when key events are received.
   *
   * @param[in] event The key event.
   * @param[in] type Used to distinguish between regular key events and IMF events.
   */
  bool KeyEvent( const Dali::KeyEvent& event );

  /**
   * @brief Called by editable UI controls when key events are received.
   *
   * @param[in] text The text to insert.
   * @param[in] type Used to distinguish between regular key events and IMF events.
   */
  void InsertText( const std::string& text, InsertType type );

  /**
   * @brief Called by editable UI controls when a tap gesture occurs.
   * @param[in] tapCount The number of taps.
   * @param[in] x The x position relative to the top-left of the parent control.
   * @param[in] y The y position relative to the top-left of the parent control.
   */
  void TapEvent( unsigned int tapCount, float x, float y );

  /**
   * @brief Called by editable UI controls when a pan gesture occurs.
   *
   * @param[in] state The state of the gesture.
   * @param[in] displacement This distance panned since the last pan gesture.
   */
  void PanEvent( Gesture::State state, const Vector2& displacement );

  /**
   * @brief Creates a selection event.
   *
   * It could be called from the TapEvent (double tap) or when the text selection popup's sellect all button is pressed.
   *
   * @param[in] x The x position relative to the top-left of the parent control.
   * @param[in] y The y position relative to the top-left of the parent control.
   * @param[in] selectAll Whether the whole text is selected.
   */
  void SelectEvent( float x, float y, bool selectAll );

  /**
   * @brief Event received from IMF manager
   *
   * @param[in] imfManager The IMF manager.
   * @param[in] imfEvent The event received.
   * @return A data struture indicating if update is needed, cursor position and current text.
   */
  ImfManager::ImfCallbackData OnImfEvent( ImfManager& imfManager, const ImfManager::ImfEventData& imfEvent );

  /**
   * @copydoc Dali::Toolkit::Text::Decorator::ControllerInterface::GetTargetSize()
   */
  virtual void GetTargetSize( Vector2& targetSize );

  /**
   * @copydoc Dali::Toolkit::Text::Decorator::ControllerInterface::AddDecoration()
   */
  virtual void AddDecoration( Actor& actor, bool needsClipping );

  /**
   * @copydoc Dali::Toolkit::Text::Decorator::ControllerInterface::DecorationEvent()
   */
  virtual void DecorationEvent( HandleType handle, HandleState state, float x, float y );

  /**
   * @copydoc Dali::Toolkit::TextSelectionPopup::TextPopupButtonCallbackInterface::TextPopupButtonTouched()
   */
  virtual void TextPopupButtonTouched( Dali::Toolkit::TextSelectionPopup::Buttons button );

protected:

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~Controller();

private:

  /**
   * @brief Helper to clear font-specific data.
   */
  void ShowPlaceholderText();

  /**
   * @brief Helper to clear all the model data except for LogicalModel::mText.
   */
  void ClearModelData();

  /**
   * @brief Helper to clear font-specific data (only).
   */
  void ClearFontData();

  /**
   * @brief Private constructor.
   */
  Controller( ControlInterface& controlInterface );

  // Undefined
  Controller( const Controller& handle );

  // Undefined
  Controller& operator=( const Controller& handle );

private:

  struct Impl;
  Impl* mImpl;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_CONTROLLER_H__

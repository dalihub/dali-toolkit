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
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/events/gesture.h>
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/object/ref-object.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/decorator/text-decorator.h>
#include <dali-toolkit/internal/text/font-run.h>
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
 * @brief A Text Controller is used by UI Controls which display text.
 *
 * It manipulates the Logical & Visual text models on behalf of the UI Controls.
 * It provides a view of the text that can be used by rendering back-ends.
 *
 * For selectable/editable UI controls, the controller handles input events from the UI control
 * and decorations (grab handles etc) via an observer interface.
 */
class Controller : public RefObject, public Decorator::Observer
{
private:

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

public:

  /**
   * @brief Create a new instance of a Controller.
   *
   * @param[in] controlInterface An interface used to request a text relayout.
   * @return A pointer to a new Controller.
   */
  static ControllerPtr New( ControlInterface& controlInterface );

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
   * @brief Replaces any placeholder text previously set.
   *
   * @param[in] text A string of UTF-8 characters.
   */
  void SetPlaceholderText( const std::string& text );

  /**
   * @brief Retrieve any placeholder text previously set.
   *
   * @return A string of UTF-8 characters.
   */
  void GetPlaceholderText( std::string& text ) const;

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
   * @brief Retrieve the default fonts.
   *
   * @param[out] fonts The default font family, style and point sizes.
   * @param[in] numberOfCharacters The number of characters in the logical model.
   */
  void GetDefaultFonts( Dali::Vector<FontRun>& fonts, Length numberOfCharacters ) const;

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
   * @brief Called to enable text input.
   *
   * @note Only selectable or editable controls should calls this.
   * @param[in] decorator Used to create cursor, selection handle decorations etc.
   */
  void EnableTextInput( DecoratorPtr decorator );

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
   * @brief Used to process an event queued from SetText()
   *
   * @param[in] newText The new text to store in the logical model.
   */
  void ReplaceTextEvent( const std::string& newText );

  /**
   * @brief Used to process an event queued from key events etc.
   *
   * @param[in] text The text to insert into the logical model.
   */
  void InsertTextEvent( const std::string& text );

  /**
   * @brief Used to process an event queued from backspace key etc.
   */
  void DeleteTextEvent();

  /**
   * @brief Update the model following text replace/insert etc.
   *
   * @param[in] operationsRequired The layout operations which need to be done.
   */
  void UpdateModel( OperationsMask operationsRequired );

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
   * @brief Caller by editable UI controls when keyboard focus is gained.
   */
  void KeyboardFocusGainEvent();

  /**
   * @brief Caller by editable UI controls when focus is lost.
   */
  void KeyboardFocusLostEvent();

  /**
   * @brief Caller by editable UI controls when key events are received.
   *
   * @param[in] event The key event.
   */
  bool KeyEvent( const Dali::KeyEvent& event );

  /**
   * @brief Caller by editable UI controls when a tap gesture occurs.
   * @param[in] tapCount The number of taps.
   * @param[in] x The x position relative to the top-left of the parent control.
   * @param[in] y The y position relative to the top-left of the parent control.
   */
  void TapEvent( unsigned int tapCount, float x, float y );

  /**
   * @brief Caller by editable UI controls when a pan gesture occurs.
   *
   * @param[in] state The state of the gesture.
   * @param[in] displacement This distance panned since the last pan gesture.
   */
  void PanEvent( Gesture::State state, const Vector2& displacement );

  /**
   * @copydoc Dali::Toolkit::Text::Decorator::Observer::GrabHandleEvent()
   */
  virtual void GrabHandleEvent( GrabHandleState state, float x, float y );

protected:

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~Controller();

private:

  /**
   * @brief Request a relayout using the ControlInterface.
   */
  void RequestRelayout();

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

  // Avoid allocating this when the user does not specify a font
  struct FontDefaults;

  // Avoid allocating this for non-editable controls
  struct TextInput;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_CONTROLLER_H__

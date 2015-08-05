#ifndef __DALI_TOOLKIT_TEXT_DECORATOR_H__
#define __DALI_TOOLKIT_TEXT_DECORATOR_H__

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
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/math/rect.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/text-controls/text-selection-popup.h>

namespace Dali
{

struct Vector2;
struct Vector4;

namespace Toolkit
{

namespace Text
{

class Decorator;
typedef IntrusivePtr<Decorator> DecoratorPtr;

// Used to set the cursor positions etc.
enum Cursor
{
  PRIMARY_CURSOR,   ///< The primary cursor for bidirectional text (or the regular cursor for single-direction text)
  SECONDARY_CURSOR, ///< The secondary cursor for bidirectional text
  CURSOR_COUNT
};

// Determines which of the cursors are active (if any).
enum ActiveCursor
{
  ACTIVE_CURSOR_NONE,    ///< Neither primary nor secondary cursor are active
  ACTIVE_CURSOR_PRIMARY, ///< Primary cursor is active (only)
  ACTIVE_CURSOR_BOTH     ///< Both primary and secondary cursor are active
};

// The state information for handle events.
enum HandleState
{
  HANDLE_TAPPED,
  HANDLE_PRESSED,
  HANDLE_RELEASED,
  HANDLE_SCROLLING,
  HANDLE_STOP_SCROLLING
};

// Used to set different handle images
enum HandleImageType
{
  HANDLE_IMAGE_PRESSED,
  HANDLE_IMAGE_RELEASED,
  HANDLE_IMAGE_TYPE_COUNT
};

// Types of handles.
enum HandleType
{
  GRAB_HANDLE,
  LEFT_SELECTION_HANDLE,
  RIGHT_SELECTION_HANDLE,
  LEFT_SELECTION_HANDLE_MARKER,
  RIGHT_SELECTION_HANDLE_MARKER,
  HANDLE_TYPE_COUNT
};

/**
 * @brief A Text Decorator is used to display cursors, handles, selection highlights and pop-ups.
 *
 * The decorator is responsible for clipping decorations which are positioned outside of the parent area.
 *
 * The Popup decoration will be positioned either above the Grab handle or above the selection handles but if doing so
 * would cause the Popup to exceed the Decoration Bounding Box ( see SetBoundingBox API ) the the Popup will be repositioned below the handle(s).
 *
 * Selection handles will be flipped around to ensure they do not exceed the Decoration Bounding Box. ( Stay visible ).
 *
 * Decorator components forward input events to a controller class through an interface.
 * The controller is responsible for selecting which components are active.
 */
class Decorator : public RefObject
{
public:

  class ControllerInterface
  {
  public:

    /**
     * @brief Constructor.
     */
    ControllerInterface() {};

    /**
     * @brief Virtual destructor.
     */
    virtual ~ControllerInterface() {};

    /**
     * @brief Query the target size of the UI control.
     *
     * @param[out] targetSize The size of the UI control the decorator is adding it's decorations to.
     */
    virtual void GetTargetSize( Vector2& targetSize ) = 0;

    /**
     * @brief Add a decoration to the parent UI control.
     *
     * @param[in] decoration The actor displaying a decoration.
     */
    virtual void AddDecoration( Actor& actor, bool needsClipping ) = 0;

    /**
     * @brief An input event from one of the handles.
     *
     * @param[in] handleType The handle's type.
     * @param[in] state The handle's state.
     * @param[in] x The x position relative to the top-left of the parent control.
     * @param[in] y The y position relative to the top-left of the parent control.
     */
    virtual void DecorationEvent( HandleType handleType, HandleState state, float x, float y ) = 0;
  };

  /**
   * @brief Create a new instance of a Decorator.
   *
   * @param[in] controller The controller which receives input events from Decorator components.
   * @param[in] callbackInterface The text popup callback interface which receives the button click callbacks.
   *
   * @return A pointer to a new Decorator.
   */
  static DecoratorPtr New( ControllerInterface& controller,
                           TextSelectionPopupCallbackInterface& callbackInterface );

  /**
   * @brief Set the bounding box which handles, popup and similar decorations will not exceed.
   *
   * The default value is the width and height of the stage from the top left origin.
   * If a title bar for example is on the top of the screen then the y should be the title's height and
   * the boundary height the stage height minus the title's height.
   * Restrictions - The boundary box should be set up with a fixed z position for the text-input and the default camera.
   *
   * ------------------------------------------
   * |(x,y)                                   |
   * |o---------------------------------------|
   * ||                                      ||
   * ||            Bounding Box              || boundary height
   * ||                                      ||
   * |----------------------------------------|
   * ------------------------------------------
   *               boundary width
   *
   * @param[in] boundingBox Vector( x coordinate, y coordinate, width, height )
   */
  void SetBoundingBox( const Rect<int>& boundingBox );

  /**
   * @brief Retrieve the bounding box origin and dimensions.
   *
   * default is set once control is added to stage, before this the return vector will be Vector4:ZERO
   * @return Rect<int> the bounding box origin, width and height
   */
  const Rect<int>& GetBoundingBox() const;

  /**
   * @brief The decorator waits until a relayout before creating actors etc.
   *
   * @param[in] size The size of the parent control after size-negotiation.
   */
  void Relayout( const Dali::Vector2& size );

  /**
   * @brief Updates the decorator's actor positions after scrolling.
   *
   * @param[in] scrollOffset The scroll offset.
   */
  void UpdatePositions( const Vector2& scrollOffset );

  /**
   * @brief Sets which of the cursors are active.
   *
   * @note Cursor will only be visible if within the parent area.
   * @param[in] activeCursor Which of the cursors should be active (if any).
   */
  void SetActiveCursor( ActiveCursor activeCursor );

  /**
   * @brief Query which of the cursors are active.
   *
   * @return  Which of the cursors are active (if any).
   */
  unsigned int GetActiveCursor() const;

  /**
   * @brief Sets the position of a cursor.
   *
   * @param[in] cursor The cursor to set.
   * @param[in] x The x position relative to the top-left of the parent control.
   * @param[in] y The y position relative to the top-left of the parent control.
   * @param[in] cursorHeight The logical height of the cursor.
   * @param[in] lineHeight The logical height of the line.
   */
  void SetPosition( Cursor cursor, float x, float y, float cursorHeight, float lineHeight );

  /**
   * @brief Retrieves the position, height and lineHeight of a cursor.
   *
   * @param[in] cursor The cursor to get.
   * @param[out] x The x position relative to the top-left of the parent control.
   * @param[out] y The y position relative to the top-left of the parent control.
   * @param[out] cursorHeight The logical height of the cursor.
   * @param[out] lineHeight The logical height of the line.
   */
  void GetPosition( Cursor cursor, float& x, float& y, float& cursorHeight, float& lineHeight ) const;

  /**
   * @brief Retrieves the position of a cursor.
   *
   * @param[in] cursor The cursor to get.
   *
   * @return The position.
   */
  const Vector2& GetPosition( Cursor cursor ) const;

  /**
   * @brief Sets the color for a cursor.
   *
   * @param[in] cursor Whether this color is for the primary or secondary cursor.
   * @param[in] color The color to use.
   */
  void SetCursorColor( Cursor cursor, const Dali::Vector4& color );

  /**
   * @brief Retrieves the color for a cursor.
   *
   * @param[in] cursor Whether this color is for the primary or secondary cursor.
   * @return The cursor color.
   */
  const Dali::Vector4& GetColor( Cursor cursor ) const;

  /**
   * @brief Start blinking the cursor; see also SetCursorBlinkDuration().
   */
  void StartCursorBlink();

  /**
   * @brief Stop blinking the cursor.
   */
  void StopCursorBlink();

  /**
   * @brief Set the interval between cursor blinks.
   *
   * @param[in] seconds The interval in seconds.
   */
  void SetCursorBlinkInterval( float seconds );

  /**
   * @brief Retrieves the blink-interval for a cursor.
   *
   * @return The cursor blink-interval.
   */
  float GetCursorBlinkInterval() const;

  /**
   * @brief The cursor will stop blinking after this duration.
   *
   * @param[in] seconds The duration in seconds.
   */
  void SetCursorBlinkDuration( float seconds );

  /**
   * @brief Retrieves the blink-duration for a cursor.
   *
   * @return The cursor blink-duration.
   */
  float GetCursorBlinkDuration() const;

  /**
   * @brief Sets whether a handle is active.
   *
   * @param[in] handleType One of the handles.
   * @param[in] active True if the handle should be active.
   */
  void SetHandleActive( HandleType handleType,
                        bool active );

  /**
   * @brief Query whether a handle is active.
   *
   * @param[in] handleType One of the handles.
   *
   * @return True if the handle is active.
   */
  bool IsHandleActive( HandleType handleType ) const;

  /**
   * @brief Sets the image for one of the handles.
   *
   * @param[in] handleType One of the handles.
   * @param[in] handleImageType A different image can be set for the pressed/released states.
   * @param[in] image The image to use.
   */
  void SetHandleImage( HandleType handleType, HandleImageType handleImageType, Dali::Image image );

  /**
   * @brief Retrieves the image for one of the handles.
   *
   * @param[in] handleType One of the handles.
   * @param[in] handleImageType A different image can be set for the pressed/released states.
   *
   * @return The grab handle image.
   */
  Dali::Image GetHandleImage( HandleType handleType, HandleImageType handleImageType ) const;

  /**
   * @brief Sets the color of the handles
   *
   * @param[in] color The color to use.
   */
  void SetHandleColor( const Vector4& color );

  /**
   * @brief Retrieves the handles color.
   *
   * @return The color of the handles.
   */
  const Vector4& GetHandleColor() const;

  /**
   * @brief Sets the position of a selection handle.
   *
   * @param[in] handleType The handle to set.
   * @param[in] x The x position relative to the top-left of the parent control.
   * @param[in] y The y position relative to the top-left of the parent control.
   * @param[in] lineHeight The logical line height at this position.
   */
  void SetPosition( HandleType handleType, float x, float y, float lineHeight );

  /**
   * @brief Retrieves the position of a selection handle.
   *
   * @param[in] handleType The handle to get.
   * @param[out] x The x position relative to the top-left of the parent control.
   * @param[out] y The y position relative to the top-left of the parent control.
   * @param[out] lineHeight The logical line height at this position.
   */
  void GetPosition( HandleType handleType, float& x, float& y, float& lineHeight ) const;

  /**
   * @brief Retrieves the position of a selection handle.
   *
   * @param[in] handleType The handle to get.
   *
   * @return The position of the selection handle relative to the top-left of the parent control.
   */
  const Vector2& GetPosition( HandleType handleType ) const;

  /**
   * @brief Swaps the selection handle's images.
   *
   * This method is called by the text controller to swap the handles
   * when the start index is bigger than the end one.
   */
  void SwapSelectionHandlesEnabled( bool enable );

  /**
   * @brief Adds a quad to the existing selection highlights.
   *
   * @param[in] x1 The top-left x position.
   * @param[in] y1 The top-left y position.
   * @param[in] x2 The bottom-right x position.
   * @param[in] y3 The bottom-right y position.
   */
  void AddHighlight( float x1, float y1, float x2, float y2 );

  /**
   * @brief Removes all of the previously added highlights.
   */
  void ClearHighlights();

  /**
   * @brief Sets the selection highlight color.
   *
   * @param[in] color The color to use.
   */
  void SetHighlightColor( const Vector4& color );

  /**
   * @brief Retrieves the selection highlight color.
   *
   * @return The color of the highlight
   */
  const Vector4& GetHighlightColor() const;

  /**
   * @brief Sets into the decorator the depth used to render the text.
   *
   * @param[in] depth The text's depth.
   */
  void SetTextDepth( int textDepth );

  /**
   * @brief Set the Selection Popup to show or hide via the active flaf
   * @param[in] active true to show, false to hide
   */
  void SetPopupActive( bool active );

  /**
   * @brief Query whether the Selection Popup is active.
   *
   * @return True if the Selection Popup should be active.
   */
  bool IsPopupActive() const;

  /**
   * @brief Set a bit mask of the buttons to be shown by Popup
   * @param[in] enabledButtonsBitMask from TextSelectionPopup::Buttons enum
   */
  void SetEnabledPopupButtons( TextSelectionPopup::Buttons& enabledButtonsBitMask );

  /**
   * @brief Get the current bit mask of buttons to be shown by Popup
   * @return bitmask of TextSelectionPopup::Buttons
   */
  TextSelectionPopup::Buttons& GetEnabledPopupButtons();

  /**
   * @brief Sets the scroll threshold.
   *
   * It defines a square area inside the control, close to the edge.
   * When the cursor enters this area, the decorator starts to send scroll events.
   *
   * @param[in] threshold The scroll threshold.
   */
  void SetScrollThreshold( float threshold );

  /**
   * @brief Retrieves the scroll threshold.
   *
   * @retunr The scroll threshold.
   */
  float GetScrollThreshold() const;

  /**
   * @brief Sets the scroll speed.
   *
   * Is the distance the text is going to be scrolled during a scroll interval.
   *
   * @param[in] speed The scroll speed.
   */
  void SetScrollSpeed( float speed );

  /**
   * @brief Retrieves the scroll speed.
   *
   * @return The scroll speed.
   */
  float GetScrollSpeed() const;

  /**
   * @brief Notifies the decorator the whole text has been scrolled.
   */
  void NotifyEndOfScroll();

protected:

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~Decorator();

private:

  /**
   * @brief Private constructor.
   * @param[in] controller The controller which receives input events from Decorator components.
   * @param[in] callbackInterface The text popup callback interface which receives the button click callbacks.
   */
  Decorator( ControllerInterface& controller,
             TextSelectionPopupCallbackInterface& callbackInterface );

  // Undefined
  Decorator( const Decorator& handle );

  // Undefined
  Decorator& operator=( const Decorator& handle );

private:

  struct Impl;
  Impl* mImpl;
};
} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_DECORATOR_H__

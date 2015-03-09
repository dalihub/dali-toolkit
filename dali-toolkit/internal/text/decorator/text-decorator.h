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

namespace Dali
{

class Image;
class Vector2;
class Vector4;

namespace Toolkit
{

namespace Internal
{
class Control;
}

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

// The state information for grab handle events
enum GrabHandleState
{
  GRAB_HANDLE_TAPPED,
  GRAB_HANDLE_PRESSED,
  GRAB_HANDLE_RELEASED
};

// The set the selection-handle positions etc.
enum SelectionHandle
{
  PRIMARY_SELECTION_HANDLE,
  SECONDARY_SELECTION_HANDLE,
  SELECTION_HANDLE_COUNT
};

enum SelectionHandleState
{
  SELECTION_HANDLE_PRESSED,
  SELECTION_HANDLE_RELEASED
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
 * Decorator components forward input events to a controller class through an observer interface.
 * The controller is responsible for selecting which components are active.
 */
class Decorator : public RefObject
{
public:

  class Observer
  {
  public:

    /**
     * @brief Constructor.
     */
    Observer() {};

    /**
     * @brief Virtual destructor.
     */
    virtual ~Observer() {};

    /**
     * @brief An input event from the grab handle.
     *
     * @param[in] state The grab handle state.
     * @param[in] x The x position relative to the top-left of the parent control.
     * @param[in] y The y position relative to the top-left of the parent control.
     */
    virtual void GrabHandleEvent( GrabHandleState state, float x, float y ) = 0;
  };

  /**
   * @brief Create a new instance of a Decorator.
   *
   * @param[in] parent Decorations will be added to this parent control.
   * @param[in] observer A class which receives input events from Decorator components.
   * @return A pointer to a new Decorator.
   */
  static DecoratorPtr New( Dali::Toolkit::Internal::Control& parent, Observer& observer );

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
   * @param[in] height The logical height of the cursor.
   */
  void SetPosition( Cursor cursor, float x, float y, float height );

  /**
   * @brief Retrieves the position of a cursor.
   *
   * @param[in] cursor The cursor to get.
   * @param[out] x The x position relative to the top-left of the parent control.
   * @param[out] y The y position relative to the top-left of the parent control.
   * @param[out] height The logical height of the cursor.
   */
  void GetPosition( Cursor cursor, float& x, float& y, float& height ) const;

  /**
   * @brief Sets the image for a cursor.
   *
   * @param[in] image The image to use.
   */
  void SetCursorImage( Dali::Image image );

  /**
   * @brief Retrieves the image for a cursor.
   *
   * @return The cursor image.
   */
  Dali::Image GetCursorImage() const;

  /**
   * @brief Sets the color for a cursor.
   *
   * @param[in] cursor Whether this color is for the primary or secondary cursor.
   * @param[in] color The color to use.
   */
  void SetColor( Cursor cursor, const Dali::Vector4& color );

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
   * @brief Sets whether the grab handle is active.
   *
   * @note The grab handle follows the cursor position set with SetPosition(Cursor, ...)
   * @param[in] active True if the grab handle should be active.
   */
  void SetGrabHandleActive( bool active );

  /**
   * @brief Query whether the grab handle is active.
   *
   * @return True if the grab handle should be active.
   */
  bool IsGrabHandleActive() const;

  /**
   * @brief Sets the image for the grab handle.
   *
   * @param[in] image The image to use.
   */
  void SetGrabHandleImage( Dali::Image image );

  /**
   * @brief Retrieves the image for the grab handle.
   *
   * @return The grab handle image.
   */
  Dali::Image GetGrabHandleImage() const;

  /**
   * @brief Sets whether the selection handles and highlight are active.
   *
   * @param[in] active True if the selection handles and highlight are active.
   */
  void SetSelectionActive( bool active );

  /**
   * @brief Query whether the selection handles and highlight are active.
   *
   * @return True if the selection handles and highlight are active.
   */
  bool IsSelectionActive() const;

  /**
   * @brief Sets the position of a selection handle.
   *
   * @param[in] handle The handle to set.
   * @param[in] x The x position relative to the top-left of the parent control.
   * @param[in] y The y position relative to the top-left of the parent control.
   * @param[in] cursorHeight The logical cursor height at this position.
   */
  void SetPosition( SelectionHandle handle, float x, float y, float cursorHeight );

  /**
   * @brief Retrieves the position of a selection handle.
   *
   * @param[in] handle The handle to get.
   * @param[out] x The x position relative to the top-left of the parent control.
   * @param[out] y The y position relative to the top-left of the parent control.
   * @param[out] cursorHeight The logical cursor height at this position.
   */
  void GetPosition( SelectionHandle handle, float& x, float& y, float& cursorHeight ) const;

  /**
   * @brief Sets the image for one of the selection handles.
   *
   * @param[in] handle The selection handle.
   * @param[in] state A different image can be set for the pressed/released states.
   * @param[in] image The image to use.
   */
  void SetImage( SelectionHandle handle, SelectionHandleState state, Dali::Image image );

  /**
   * @brief Retrieves the image for a selection handle.
   *
   * @param[in] handle The selection handle.
   * @param[in] state A different image can be set for the pressed/released states.
   * @return The image.
   */
  Dali::Image GetImage( SelectionHandle handle, SelectionHandleState state ) const;

  /**
   * @brief Show the Copy and Paste Popup
   */
  void ShowPopup();

  /**
   * @brief Hide the Copy and Paste Popup
   */
  void HidePopup();

protected:

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~Decorator();

private:

  /**
   * @brief Private constructor.
   * @param[in] parent Decorations will be added to this parent control.
   * @param[in] observer A class which receives input events from Decorator components.
   */
  Decorator(Dali::Toolkit::Internal::Control& parent, Observer& observer );

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

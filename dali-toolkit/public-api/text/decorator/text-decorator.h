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

/**
 * @brief A Text Decorator is used to display cursors, handles, selection highlights and pop-ups.
 *
 * The decorator is responsible for clipping decorations which are positioned outside of the parent area.
 * In some cases the decorations will be moved or flipped around, to maintain visibility on-screen.
 */
class Decorator : public RefObject
{
public:

  /**
   * @brief Create a new instance of a Decorator.
   *
   * @param[in] parent Decorations will be added to this parent control.
   * @return A pointer to a new Decorator.
   */
  static DecoratorPtr New( Internal::Control& parent );

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
   * @brief Sets whether a cursor should be visible.
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

protected:

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~Decorator();

private:

  /**
   * @brief Private constructor.
   * @param[in] parent Decorations will be added to this parent control.
   */
  Decorator(Internal::Control& parent);

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

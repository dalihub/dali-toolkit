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

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/decorator/text-decorator.h>
#include <dali-toolkit/internal/text/text-control-interface.h>
#include <dali-toolkit/internal/text/text-view.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/object/ref-object.h>
#include <string>

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
    SHAPE_TEXT         = 0x0020,
    GET_GLYPH_METRICS  = 0x0040,
    LAYOUT             = 0x0080,
    UPDATE_ACTUAL_SIZE = 0x0100,
    UPDATE_POSITIONS   = 0x0200,
    REORDER            = 0x0400,
    ALIGNMENT          = 0x0800,
    RENDER             = 0x1000,
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
  void GetText( std::string& text );

  /**
   * @brief Called to enable text input.
   *
   * @note Only selectable or editable controls should calls this.
   * @param[in] decorator Used to create cursor, selection handle decorations etc.
   */
  void EnableTextInput( DecoratorPtr decorator );

  /**
   * @brief Triggers a relayout which updates View (if necessary).
   *
   * @note UI Controls are expected to minimize calls to this method e.g. call once after size negotiation.
   * @param[in] size A the size of a bounding box to layout text within.
   * @return True if the text model or decorations were updated.
   */
  bool Relayout( const Vector2& size );

  /**
   * @brief Lays-out the text.
   *
   * GetNaturalSize(), GetHeightForWidth() and Relayout() calls this method.
   *
   * @param[in] size A the size of a bounding box to layout text within.
   * @param[in] operations The layout operations which need to be done.
   */
  bool DoRelayout( const Vector2& size, OperationsMask operations );

  /**
   * @copydoc Control::GetNaturalSize()
   */
  Vector3 GetNaturalSize();

  /**
   * @copydoc Control::GetHeightForWidth()
   */
  float GetHeightForWidth( float width );

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

  /**
   * @brief Caller by editable UI controls when keyboard focus is gained.
   */
  void KeyboardFocusGainEvent();

  /**
   * @brief Caller by editable UI controls when focus is lost.
   */
  void KeyboardFocusLostEvent();

  /**
   * @brief Caller by editable UI controls when a tap gesture occurs.
   * @param[in] tapCount The number of taps.
   * @param[in] x The x position relative to the top-left of the parent control.
   * @param[in] y The y position relative to the top-left of the parent control.
   */
  void TapEvent( unsigned int tapCount, float x, float y );

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

  // Avoid allocating this for non-editable controls
  struct TextInput;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_CONTROLLER_H__

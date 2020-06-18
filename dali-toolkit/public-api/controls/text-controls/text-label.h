#ifndef DALI_TOOLKIT_TEXT_LABEL_H
#define DALI_TOOLKIT_TEXT_LABEL_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class TextLabel;
}
/**
 * @addtogroup dali_toolkit_controls_text_controls
 * @{
 */

/**
 * @brief A control which renders a short text string.
 *
 * Text labels are lightweight, non-editable and do not respond to user input.
 *
 * @section TextLabelProperties Properties
 * |%Property enum                    |String name          |Type            |Writable|Animatable|
 * |----------------------------------|---------------------|----------------|--------|----------|
 * | Property::RENDERING_BACKEND      | renderingBackend    |  INTEGER       | O      | X        |
 * | Property::TEXT                   | text                |  STRING        | O      | X        |
 * | Property::FONT_FAMILY            | fontFamily          |  STRING        | O      | X        |
 * | Property::FONT_STYLE             | fontStyle           |  STRING or MAP | O      | X        |
 * | Property::POINT_SIZE             | pointSize           |  FLOAT         | O      | X        |
 * | Property::MULTI_LINE             | multiLine           |  BOOLEAN       | O      | X        |
 * | Property::HORIZONTAL_ALIGNMENT   | horizontalAlignment |  STRING        | O      | X        |
 * | Property::VERTICAL_ALIGNMENT     | verticalAlignment   |  STRING        | O      | X        |
 * | Property::TEXT_COLOR             | textColor           |  VECTOR4       | O      | X        |
 * | Property::ENABLE_MARKUP          | enableMarkup        |  BOOLEAN       | O      | X        |
 * | Property::ENABLE_AUTO_SCROLL     | enableAutoScroll    |  BOOLEAN       | O      | X        |
 * | Property::AUTO_SCROLL_SPEED      | autoScrollSpeed     |  INTEGER       | O      | X        |
 * | Property::AUTO_SCROLL_LOOP_COUNT | autoScrollLoopCount |  INTEGER       | O      | X        |
 * | Property::AUTO_SCROLL_GAP        | autoScrollGap       |  INTEGER       | O      | X        |
 * | Property::SHADOW                 | shadow              |  STRING or MAP | O      | X        |
 * | Property::UNDERLINE              | underline           |  STRING or MAP | O      | X        |
 *
 * @SINCE_1_0.0
 */
class DALI_TOOLKIT_API TextLabel : public Control
{
public:

  /**
   * @brief Enumeration for the start and end property ranges for this control.
   * @SINCE_1_0.0
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1, ///< @SINCE_1_0.0
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000,             ///< Reserve property indices @SINCE_1_0.0

    ANIMATABLE_PROPERTY_START_INDEX = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX, ///< @SINCE_1_2.60
    ANIMATABLE_PROPERTY_END_INDEX =   ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 1000 ///< Reserve animatable property indices @SINCE_1_2.60
  };

  /**
   * @brief Enumeration for the instance of properties belonging to the TextLabel class.
   * @SINCE_1_0.0
   */
  struct Property
  {
    /**
     * @brief Enumeration for the instance of properties belonging to the TextLabel class.
     * @SINCE_1_0.0
     */
    enum
    {
      ///////////////////////////////////////////////////////////////////////////////
      // Event side (non-animatable) properties
      ///////////////////////////////////////////////////////////////////////////////

      /**
       * @DEPRECATED_1_2.53 No longer be supported and will be ignored.
       * @brief The type of rendering e.g. bitmap-based.
       * @details Name "renderingBackend", type Property::INT.
       * @SINCE_1_0.0
       */
      RENDERING_BACKEND = PROPERTY_START_INDEX,

      /**
       * @brief The text to display in UTF-8 format.
       * @details Name "text", type Property::STRING.
       * @SINCE_1_0.0
       */
      TEXT,

      /**
       * @brief The requested font family to use.
       * @details Name "fontFamily", type Property::STRING.
       * @SINCE_1_0.0
       */
      FONT_FAMILY,

      /**
       * @brief The requested font style to use.
       * @details Name "fontStyle", type Property::STRING or Property::MAP.
       * @SINCE_1_2.13
       */
      FONT_STYLE,

      /**
       * @brief The size of font in points.
       * @details Name "pointSize", type Property::FLOAT.
       *          Conversion from Pixel size to Point size:
       *           Point size = Pixel size * 72 / DPI
       * @SINCE_1_0.0
       */
      POINT_SIZE,

      /**
       * @brief The single-line or multi-line layout option.
       * @details Name "multiLine", type Property::BOOLEAN.
       * @SINCE_1_0.0
       * @note Default is false.
       */
      MULTI_LINE,

      /**
       * @brief The line horizontal alignment.
       * @details Name "horizontalAlignment", type Property::STRING or type HorizontalAlignment::Type (Property::INTEGER)
       *          Values "BEGIN", "CENTER", "END", default BEGIN.
       * @note Return type is Property::STRING
       * @SINCE_1_0.0
       */
      HORIZONTAL_ALIGNMENT,

      /**
       * @brief The line vertical alignment.
       * @details Name "verticalAlignment", type Property::STRING or type VerticalAlignment::Type (Property::INTEGER).
       *          Values "TOP",   "CENTER", "BOTTOM" @SINCE_1_0.0, default TOP.
       * @note Return type is Property::STRING
       * @SINCE_1_0.0
       */
      VERTICAL_ALIGNMENT,

      /**
       * @brief Whether the mark-up processing is enabled.
       * @details Name "enableMarkup", type Property::BOOLEAN.
       * @SINCE_1_0.0
       * @note Default is false.
       */
      ENABLE_MARKUP,

      /**
       * @brief Starts or stops auto scrolling.
       * @details Name "enableAutoScroll", type Property::BOOLEAN.
       * @SINCE_1_1.35
       * @note Default is false.
       */
      ENABLE_AUTO_SCROLL,

      /**
       * @brief Sets the speed of scrolling in pixels per second.
       * @details Name "autoScrollSpeed", type Property::INT.
       * @SINCE_1_1.35
       * @note Default in style sheet.
       */
      AUTO_SCROLL_SPEED,

      /**
       * @brief Number of complete loops when scrolling enabled.
       * @details Name "autoScrollLoopCount", type Property::INT.
       * @SINCE_1_1.35
       * @note Default in style sheet.
       */
      AUTO_SCROLL_LOOP_COUNT,

      /**
       * @brief Gap before scrolling wraps.
       * @details Name "autoScrollGap", type Property::INT.
       * @SINCE_1_1.35
       * @note Default in style sheet but can be overridden to prevent same text being shown at start and end.
       * @note Displayed gap size is not guaranteed if the text length plus gap exceeds the maximum texture size (i.e. GL_MAX_TEXTURE_SIZE).
       */
      AUTO_SCROLL_GAP,

      /**
       * @brief The default extra space between lines in points.
       * @details Name "lineSpacing", type Property::FLOAT.
       * @SINCE_1_1.37
       */
      LINE_SPACING,

      /**
       * @brief The default underline parameters.
       * @details Name "underline", type Property::MAP.
       *
       * The underline map contains the following keys:
       *
       * | %Property Name       | Type     | Required | Description                                                                                                        |
       * |----------------------|----------|----------|--------------------------------------------------------------------------------------------------------------------|
       * | enable               | BOOLEAN  | No       | True to enable the underline or false to disable (the default value is false)                                      |
       * | color                | VECTOR4  | No       | The color of the underline (the default value is Color::BLACK)                                                     |
       * | height               | FLOAT    | No       | The height of the underline (the default value is 0)                                                               |
       *
       * @SINCE_1_2.13
       */
      UNDERLINE,

      /**
       * @brief The default shadow parameters.
       * @details Name "shadow", type Property::MAP.
       *
       * The shadow map contains the following keys:
       *
       * | %Property Name       | Type     | Required | Description                                                                                                        |
       * |----------------------|----------|----------|--------------------------------------------------------------------------------------------------------------------|
       * | color                | VECTOR4  | No       | The color of the shadow (the default value is Color::BLACK)                                                        |
       * | offset               | VECTOR2  | No       | The offset from the text to draw the shadow in the X and Y axes (the default value is 0 which means no shadow)     |
       * | blurRadius           | FLOAT    | No       | The radius of blur to be applied to the shadow (the default value is 0 which means no blur)                        |
       *
       * @SINCE_1_2.13
       */
      SHADOW,

      /**
       * @brief The default emboss parameters.
       * @details Name "emboss", type Property::MAP.
       * @SINCE_1_2.13
       */
      EMBOSS,

      /**
       * @brief The default outline parameters.
       * @details Name "outline", type Property::MAP.
       *
       * The outline map contains the following keys:
       *
       * | %Property Name       | Type     | Required | Description                                                                                                        |
       * |----------------------|----------|----------|--------------------------------------------------------------------------------------------------------------------|
       * | color                | VECTOR4  | No       | The color of the outline (the default value is Color::WHITE)                                                       |
       * | width                | INTEGER  | No       | The width of the outline (the default value is 0 which means no outline)                                           |
       *
       * @SINCE_1_2.13
       */
      OUTLINE,

      /**
       * @brief The size of font in pixels.
       * @details Name "pixelSize", type Property::FLOAT.
       *          Conversion from Point size to Pixel size:
       *            Pixel size = Point size * DPI / 72
       * @SINCE_1_2.60
       */
      PIXEL_SIZE,

      /**
       * @brief Whether we should show the ellipsis if required.
       * @details Name "ellipsis", type Property::BOOLEAN.
       * @SINCE_1_2.60
       */
      ELLIPSIS,

      /**
       * @brief The amount of time to delay the starting time of auto scrolling and further loops.
       * @details Name "autoScrollLoopDelay", type Property::FLOAT.
       * @SINCE_1_2.60
       */
      AUTO_SCROLL_LOOP_DELAY,

      /**
       * @brief The auto scrolling stop behaviour.
       * @details Name "autoScrollStopMode", type AutoScrollStopMode::Type (Property::INTEGER) or Property::STRING.
       * @SINCE_1_2.60
       * @note Default is AutoScrollStopMode::FINISH_LOOP.
       * @see AutoScrollStopMode::Type
       */
      AUTO_SCROLL_STOP_MODE,

      /**
       * @brief The line count of text.
       * @details name "lineCount", type Property::INTEGER.
       * @SINCE_1_2.60
       * @note This property is read-only.
       */
      LINE_COUNT,

      /**
       * @brief Line wrap mode when text lines are greater than the layout width.
       * @details Name "lineWrapMode", type Text::LineWrap::Mode (Text::Property::INTEGER) or Property::STRING.
       * @SINCE_1_2.60
       * @note Default is Text::LineWrap::WORD.
       * @note Return type is Text::LineWrap::Mode (Text::Property::INTEGER).
       * @see Text::LineWrap
       */
      LINE_WRAP_MODE,

      ///////////////////////////////////////////////////////////////////////////////
      // Animatable Properties
      ///////////////////////////////////////////////////////////////////////////////

      /**
       * @brief The color of the text.
       * @details Name "textColor", type Property::VECTOR4.
       * @SINCE_1_2.60
       */
      TEXT_COLOR = ANIMATABLE_PROPERTY_START_INDEX,

      /**
       * @brief The red component of the text color.
       * @details Name "textColorRed", type Property::FLOAT.
       * @SINCE_1_2.60
       * @see TEXT_COLOR
       */
      TEXT_COLOR_RED,

      /**
       * @brief The green component of the text color.
       * @details Name "textColorGreen", type Property::FLOAT.
       * @SINCE_1_2.60
       * @see TEXT_COLOR
       */
      TEXT_COLOR_GREEN,

      /**
       * @brief The blue component of the text color.
       * @details Name "textColorBlue", type Property::FLOAT.
       * @SINCE_1_2.60
       * @see TEXT_COLOR
       */
      TEXT_COLOR_BLUE,

      /**
       * @brief The alpha component of the text color.
       * @details Name "textColorAlpha", type Property::FLOAT.
       * @SINCE_1_2.60
       * @see TEXT_COLOR
       */
      TEXT_COLOR_ALPHA,
    };
  };

  /**
   * @brief The enumerations used for auto scroll stop mode.
   * @SINCE_1_2.60
   * @see Property::AUTO_SCROLL_STOP_MODE.
   */
  struct AutoScrollStopMode
  {
    /**
     * @brief The enumerations used for auto scroll stop mode.
     * @SINCE_1_2.60
     * @see Property::AUTO_SCROLL_STOP_MODE.
     */
    enum Type
    {
      FINISH_LOOP = 0,  ///< Stop animation after current loop finishes. @SINCE_1_2.60
      IMMEDIATE         ///< Stop animation immediately and reset position. @SINCE_1_2.60
    };
  };

  /**
   * @brief Creates the TextLabel control.
   *
   * @SINCE_1_0.0
   * @return A handle to the TextLabel control
   */
  static TextLabel New();

  /**
   * @brief Creates the TextLabel control.
   *
   * @SINCE_1_0.0
   * @param[in] text The text to display
   * @return A handle to the TextLabel control
   */
  static TextLabel New( const std::string& text );

  /**
   * @brief Creates an empty handle.
   * @SINCE_1_0.0
   */
  TextLabel();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_1_0.0
   * @param[in] handle The handle to copy from
   */
  TextLabel( const TextLabel& handle );

  /**
   * @brief Assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] handle The handle to copy from
   * @return A reference to this
   */
  TextLabel& operator=( const TextLabel& handle );

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~TextLabel();

  /**
   * @brief Downcasts a handle to TextLabel.
   *
   * If the BaseHandle points is a TextLabel, the downcast returns a valid handle.
   * If not, the returned handle is left empty.
   *
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return Handle to a TextLabel or an empty handle
   */
  static TextLabel DownCast( BaseHandle handle );

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @SINCE_1_0.0
   * @param[in] implementation The Control implementation
   */
  DALI_INTERNAL TextLabel( Internal::TextLabel& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @SINCE_1_0.0
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_INTERNAL TextLabel( Dali::Internal::CustomActor* internal );
  /// @endcond
};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_LABEL_H

#ifndef __DALI_TOOLKIT_TEXT_LABEL_H__
#define __DALI_TOOLKIT_TEXT_LABEL_H__

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
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API TextLabel : public Control
{
public:

  /**
   * @brief The start and end property ranges for this control.
   * @SINCE_1_0.0
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1, ///< @SINCE_1_0.0
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000              ///< Reserve property indices @SINCE_1_0.0
  };

  /**
   * @brief An enumeration of properties belonging to the TextLabel class.
   * @SINCE_1_0.0
   */
  struct Property
  {
    enum
    {
      RENDERING_BACKEND = PROPERTY_START_INDEX, ///< name "renderingBackend",     The type or rendering e.g. bitmap-based,          type INT @SINCE_1_0.0
      TEXT,                                     ///< name "text",                 The text to display in UTF-8 format,              type STRING @SINCE_1_0.0
      FONT_FAMILY,                              ///< name "fontFamily",           The requested font family,                        type STRING @SINCE_1_0.0
      FONT_STYLE,                               ///< name "fontStyle",            The requested font style,                         type STRING @SINCE_1_0.0
      POINT_SIZE,                               ///< name "pointSize",            The size of font in points,                       type FLOAT @SINCE_1_0.0
      MULTI_LINE,                               ///< name "multiLine",            The single-line or multi-line layout option,      type BOOLEAN @SINCE_1_0.0
      HORIZONTAL_ALIGNMENT,                     ///< name "horizontalAlignment",  The line horizontal alignment,                    type STRING,  values "BEGIN", "CENTER", "END" @SINCE_1_0.0
      VERTICAL_ALIGNMENT,                       ///< name "verticalAlignment",    The line vertical alignment,                      type STRING,  values "TOP",   "CENTER", "BOTTOM" @SINCE_1_0.0
      TEXT_COLOR,                               ///< name "textColor",            The text color,                                   type VECTOR4 @SINCE_1_0.0
      SHADOW_OFFSET,                            ///< name "shadowOffset",         The drop shadow offset 0 indicates no shadow,     type VECTOR2 @SINCE_1_0.0
      SHADOW_COLOR,                             ///< name "shadowColor",          The color of a drop shadow,                       type VECTOR4 @SINCE_1_0.0
      UNDERLINE_ENABLED,                        ///< name "underlineEnabled",     The underline enabled flag,                       type BOOLEAN @SINCE_1_0.0
      UNDERLINE_COLOR,                          ///< name "underlineColor",       The color of the underline,                       type VECTOR4 @SINCE_1_0.0
      UNDERLINE_HEIGHT,                         ///< name "underlineHeight",      Overrides the underline height from font metrics, type FLOAT @SINCE_1_0.0
      ENABLE_MARKUP,                            ///< name "enableMarkup",         Whether the mark-up processing is enabled.        type BOOLEAN @SINCE_1_0.0
      ENABLE_AUTO_SCROLL,                       ///< name "enableAutoScroll",     Whether to start auto text scrolling              type BOOLEAN @SINCE_1_1.35
      AUTO_SCROLL_SPEED,                        ///< name "autoScrollSpeed",      Speed at which text should auto scroll            type INT @SINCE_1_1.35
      AUTO_SCROLL_LOOP_COUNT,                   ///< name "autoScrollLoopCount",  Number of times the text should scroll (loop)     type INT @SINCE_1_1.35
      AUTO_SCROLL_GAP                           ///< name "autoScrollGap",        Gap before text wraps around when scrolling       type FLOAT @SINCE_1_1.35
    };
  };

  /**
   * @brief Create the TextLabel control.
   *
   * @SINCE_1_0.0
   * @return A handle to the TextLabel control.
   */
  static TextLabel New();

  /**
   * @brief Create the TextLabel control.
   *
   * @SINCE_1_0.0
   * @param[in] text The text to display.
   * @return A handle to the TextLabel control.
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
   * @param[in] handle The handle to copy from.
   */
  TextLabel( const TextLabel& handle );

  /**
   * @brief Assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] handle The handle to copy from.
   * @return A reference to this.
   */
  TextLabel& operator=( const TextLabel& handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~TextLabel();

  /**
   * @brief Downcast a handle to TextLabel.
   *
   * If the BaseHandle points is a TextLabel the downcast returns a valid handle.
   * If not the returned handle is left empty.
   *
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return handle to a TextLabel or an empty handle
   */
  static TextLabel DownCast( BaseHandle handle );

public: // Not intended for application developers

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @SINCE_1_0.0
   * @param[in] implementation The Control implementation.
   */
  DALI_INTERNAL TextLabel( Internal::TextLabel& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @SINCE_1_0.0
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL TextLabel( Dali::Internal::CustomActor* internal );
};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_LABEL_H__

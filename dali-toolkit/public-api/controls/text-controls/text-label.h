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
 * @brief A control which renders a short text string.
 *
 * Text labels are lightweight, non-editable and do not respond to user input.
 */
class DALI_IMPORT_API TextLabel : public Control
{
public:

  /**
   * @brief The start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000              ///< Reserve property indices
  };

  /**
   * @brief An enumeration of properties belonging to the TextLabel class.
   */
  struct Property
  {
    enum
    {
      RENDERING_BACKEND = PROPERTY_START_INDEX, ///< name "rendering-backend",    The type or rendering e.g. bitmap-based,          type INT
      TEXT,                                     ///< name "text",                 The text to display in UTF-8 format,              type STRING
      FONT_FAMILY,                              ///< name "font-family",          The requested font family,                        type STRING
      FONT_STYLE,                               ///< name "font-style",           The requested font style e.g. Regular/Italic,     type STRING
      POINT_SIZE,                               ///< name "point-size",           The size of font in points,                       type FLOAT
      MULTI_LINE,                               ///< name "multi-line",           The single-line or multi-line layout option,      type BOOLEAN
      HORIZONTAL_ALIGNMENT,                     ///< name "horizontal-alignment", The line horizontal alignment,                    type STRING,  values "BEGIN", "CENTER", "END"
      VERTICAL_ALIGNMENT,                       ///< name "vertical-alignment",   The line vertical alignment,                      type STRING,  values   "TOP",   "CENTER",   "BOTTOM"
      TEXT_COLOR,                               ///< name "text-color",           The text color,                                   type VECTOR4
      SHADOW_OFFSET,                            ///< name "shadow-offset",        The drop shadow offset 0 indicates no shadow,     type VECTOR2
      SHADOW_COLOR,                             ///< name "shadow-color",         The color of a drop shadow,                       type VECTOR4
      UNDERLINE_ENABLED,                        ///< name "underline-enabled",    The underline enabled flag,                       type BOOLEAN
      UNDERLINE_COLOR,                          ///< name "underline-color",      The color of the underline,                       type VECTOR4
      UNDERLINE_HEIGHT                          ///< name "underline-height",     Overrides the underline height from font metrics, type FLOAT
    };
  };

  /**
   * @brief Create the TextLabel control.
   *
   * @return A handle to the TextLabel control.
   */
  static TextLabel New();

  /**
   * @brief Create the TextLabel control.
   *
   * @param[in] text The text to display.
   * @return A handle to the TextLabel control.
   */
  static TextLabel New( const std::string& text );

  /**
   * @brief Creates an empty handle.
   */
  TextLabel();

  /**
   * @brief Copy constructor.
   *
   * @param[in] handle The handle to copy from.
   */
  TextLabel( const TextLabel& handle );

  /**
   * @brief Assignment operator.
   *
   * @param[in] handle The handle to copy from.
   * @return A reference to this.
   */
  TextLabel& operator=( const TextLabel& handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~TextLabel();

  /**
   * @brief Downcast a handle to TextLabel.
   *
   * If the BaseHandle points is a TextLabel the downcast returns a valid handle.
   * If not the returned handle is left empty.
   *
   * @param[in] handle Handle to an object
   * @return handle to a TextLabel or an empty handle
   */
  static TextLabel DownCast( BaseHandle handle );

public: // Not intended for application developers

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in] implementation The Control implementation.
   */
  DALI_INTERNAL TextLabel( Internal::TextLabel& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL TextLabel( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_LABEL_H__

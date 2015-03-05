#ifndef __DALI_TOOLKIT_TEXT_FIELD_H__
#define __DALI_TOOLKIT_TEXT_FIELD_H__

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
class TextField;
}

/**
 * @brief A control which provides a single-line editable text field.
 */
class DALI_IMPORT_API TextField : public Control
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
      PROPERTY_RENDERING_BACKEND = PROPERTY_START_INDEX, ///< name "rendering-backend",       The type or rendering e.g. bitmap-based,                                  type INT
      PROPERTY_PLACEHOLDER_TEXT,                         ///< name "placeholder-text",        The text to display when the TextField is empty,                          type STRING
      PROPERTY_TEXT,                                     ///< name "text",                    The text to display in UTF-8 format,                                      type STRING
      PROPERTY_FONT_FAMILY,                              ///< name "font-family",             The requested font family,                                                type STRING
      PROPERTY_FONT_STYLE,                               ///< name "font-style",              The requested font style e.g. Regular/Italic,                             type STRING
      PROPERTY_POINT_SIZE,                               ///< name "point-size",              The size of font in points,                                               type FLOAT
      PROPERTY_CURSOR_IMAGE,                             ///< name "cursor-image",            The image to display for cursors,                                         type STRING
      PROPERTY_PRIMARY_CURSOR_COLOR,                     ///< name "primary-cursor-color",    The color to apply to the primary cursor,                                 type VECTOR4
      PROPERTY_SECONDARY_CURSOR_COLOR,                   ///< name "secondary-cursor-color",  The color to apply to the secondary cursor,                               type VECTOR4
      PROPERTY_ENABLE_CURSOR_BLINK,                      ///< name "enable-cursor-blink",     Whether the cursor should blink or not,                                   type BOOLEAN
      PROPERTY_CURSOR_BLINK_INTERVAL,                    ///< name "cursor-blink-interval",   The time interval between cursor on/off states,                           type FLOAT
      PROPERTY_CURSOR_BLINK_DURATION,                    ///< name "cursor-blink-duration",   The cursor will stop blinking after this duration (if non-zero),          type FLOAT
      PROPERTY_GRAB_HANDLE_IMAGE,                        ///< name "grab-handle-image",       The image to display for grab handle,                                     type STRING
      PROPERTY_DECORATION_BOUNDING_BOX                   ///< name "decoration-bounding-box", The decorations (handles etc) will positioned within this area on-screen, type RECTANGLE
    };
  };

  /**
   * Create the TextField control.
   * @return A handle to the TextField control.
   */
  static TextField New();

  /**
   * @brief Creates an empty handle.
   */
  TextField();

  /**
   * @brief Copy constructor.
   *
   * @param[in] handle The handle to copy from.
   */
  TextField( const TextField& handle );

  /**
   * @brief Assignment operator.
   *
   * @param[in] handle The handle to copy from.
   * @return A reference to this.
   */
  TextField& operator=( const TextField& handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~TextField();

  /**
   * @brief Downcast a handle to TextField.
   *
   * If the BaseHandle points is a TextField the downcast returns a valid handle.
   * If not the returned handle is left empty.
   *
   * @param[in] handle Handle to an object
   * @return handle to a TextField or an empty handle
   */
  static TextField DownCast( BaseHandle handle );

public: // Not intended for application developers

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in] implementation The Control implementation.
   */
  DALI_INTERNAL TextField( Internal::TextField& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL TextField( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_FIELD_H__

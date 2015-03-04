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
#include <dali-toolkit/public-api/text/rendering/text-renderer.h>

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

  // Property indices
  static const Dali::Property::Index PROPERTY_RENDERING_BACKEND;       ///< name "rendering-backend",       type INT
  static const Dali::Property::Index PROPERTY_PLACEHOLDER_TEXT;        ///< name "placeholder-text",        type STRING
  static const Dali::Property::Index PROPERTY_TEXT;                    ///< name "text",                    type STRING
  static const Dali::Property::Index PROPERTY_CURSOR_IMAGE;            ///< name "cursor-image",            type STRING
  static const Dali::Property::Index PROPERTY_PRIMARY_CURSOR_COLOR;    ///< name "primary-cursor-color",    type VECTOR4
  static const Dali::Property::Index PROPERTY_SECONDARY_CURSOR_COLOR;  ///< name "secondary-cursor-color",  type VECTOR4
  static const Dali::Property::Index PROPERTY_ENABLE_CURSOR_BLINK;     ///< name "enable-cursor-blink",     type BOOLEAN
  static const Dali::Property::Index PROPERTY_CURSOR_BLINK_INTERVAL;   ///< name "cursor-blink-interval",   type FLOAT
  static const Dali::Property::Index PROPERTY_CURSOR_BLINK_DURATION;   ///< name "cursor-blink-duration",   type FLOAT
  static const Dali::Property::Index PROPERTY_GRAB_HANDLE_IMAGE;       ///< name "grab-handle-image",       type STRING
  static const Dali::Property::Index PROPERTY_DECORATION_BOUNDING_BOX; ///< name "decoration-bounding-box", type RECTANGLE

  // Property names
  static const std::string RENDERING_BACKEND_PROPERTY_NAME;      ///< Property, name "rendering-backend",       type INT
  static const std::string PLACEHOLDER_TEXT_PROPERTY_NAME;       ///< Property, name "placeholder-text",        type STRING
  static const std::string TEXT_PROPERTY_NAME;                   ///< Property, name "text",                    type STRING
  static const std::string CURSOR_IMAGE_PROPERTY_NAME;           ///< Property, name "cursor-image",            type STRING
  static const std::string PRIMARY_CURSOR_COLOR_PROPERTY_NAME;   ///< Property, name "primary-cursor-color",    type VECTOR4
  static const std::string SECONDARY_CURSOR_COLOR_PROPERTY_NAME; ///< Property, name "secondary-cursor-color",  type VECTOR4
  static const std::string ENABLE_CURSOR_BLINK_PROPERTY_NAME;    ///< Property, name "enable-cursor-blink",     type BOOLEAN
  static const std::string CURSOR_BLINK_INTERVAL_PROPERTY_NAME;  ///< Property, name "cursor-blink-interval",   type FLOAT
  static const std::string CURSOR_BLINK_DURATION_PROPERTY_NAME;  ///< Property, name "cursor-blink-duration",   type FLOAT
  static const std::string GRAB_HANDLE_IMAGE_PROPERTY_NAME;      ///< Property, name "grab-handle-image",       type STRING
  static const std::string DECORATION_BOUNDING_BOX;              ///< Property, name "decoration-bounding-box", type RECTANGLE

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

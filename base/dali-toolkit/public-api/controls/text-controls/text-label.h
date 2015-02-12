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
#include <dali-toolkit/public-api/text/rendering/text-renderer.h>

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
 */
class DALI_IMPORT_API TextLabel : public Control
{
public:

  // Property indices
  static const Property::Index PROPERTY_TEXT;       ///< name "text", type STRING
  static const Property::Index PROPERTY_MULTI_LINE; ///< name "multi-line", type BOOLEAN

  // Property names
  static const std::string TEXT_PROPERTY_NAME;       ///< Property, name "text", type STRING
  static const std::string MULTI_LINE_PROPERTY_NAME; ///< Property, name "multi-line", type BOOLEAN

  /**
   * Create the TextLabel control.
   * @return A handle to the TextLabel control.
   */
  static TextLabel New();

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

  /**
   * @brief Set the rendering back-end used by the TextLabel.
   *
   * @param[in] renderer The text renderer to use.
   */
  void SetRenderer( Text::RendererPtr renderer );

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

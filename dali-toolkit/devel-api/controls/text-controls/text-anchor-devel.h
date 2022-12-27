#ifndef DALI_TOOLKIT_TEXT_ANCHOR_DEVEL_H
#define DALI_TOOLKIT_TEXT_ANCHOR_DEVEL_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
class TextAnchor;
}

/**
 * @brief A control which renders anchor (hyperlink) in hypertext.
 */
class DALI_TOOLKIT_API TextAnchor : public Control
{
public:
  /**
   * @brief The start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX   = PROPERTY_START_INDEX + 1000 ///< Reserve property indices
  };

  /**
   * @brief An enumeration of properties belonging to the TextAnchor class.
   */
  struct Property
  {
    enum
    {
      /**
       * @brief The index of a character in text at which an anchor starts.
       * @details Name "startCharacterIndex", type INTEGER.
       */
      START_CHARACTER_INDEX = PROPERTY_START_INDEX,

      /**
       * @brief The index of a character in text that stands one position after the anchor's last character.
       * @details Name "endCharacterIndex", type INTEGER.
       */
      END_CHARACTER_INDEX,

      /**
       * @brief The URI associated with an anchor.
       * @details Name "uri", type STRING.
       */
      URI
    };
  };

  /**
   * @brief Creates the TextAnchor control.
   * @return A handle to the TextAnchor control.
   */
  static TextAnchor New();

  /**
   * @brief Creates an empty handle.
   */
  TextAnchor();

  /**
   * @brief Copy constructor.
   *
   * @param[in] handle The handle to copy from.
   */
  TextAnchor(const TextAnchor& handle);

  /**
   * @brief Assignment operator.
   *
   * @param[in] handle The handle to copy from.
   * @return A reference to this.
   */
  TextAnchor& operator=(const TextAnchor& handle);

  /**
   * @brief Move constructor.
   *
   * @param[in] handle The handle to move from.
   */
  TextAnchor(TextAnchor&& handle);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] handle The handle to move from.
   * @return A reference to this.
   */
  TextAnchor& operator=(TextAnchor&& handle);

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~TextAnchor();

  /**
   * @brief Downcast a handle to TextAnchor.
   *
   * If the BaseHandle points is a TextAnchor the downcast returns a valid handle.
   * If not the returned handle is left empty.
   *
   * @param[in] handle Handle to an object
   * @return handle to a TextAnchor or an empty handle
   */
  static TextAnchor DownCast(BaseHandle handle);

public: // Not intended for application developers
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in] implementation The Control implementation.
   */
  DALI_INTERNAL TextAnchor(Internal::TextAnchor& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL TextAnchor(Dali::Internal::CustomActor* internal);

}; // Class TextAnchor

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_ANCHOR_DEVEL_H

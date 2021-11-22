#ifndef DALI_TOOLKIT_TEXT_CONTROLLER_IMPL_DATA_CLEARER_H
#define DALI_TOOLKIT_TEXT_CONTROLLER_IMPL_DATA_CLEARER_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/text-controller.h>

namespace Dali::Toolkit::Text
{

/// Provides methods to clear some of the model data in the Text::Controller::Impl
struct ControllerImplDataClearer
{

  /**
   * @brief Helper to clear completely the parts of the model specified by the given @p operations.
   *
   * @note It never clears the text stored in utf32.
   *
   * @param[in] impl The text controller impl.
   * @param[in] operations The operations required.
   */
  static void ClearFullModelData(Controller::Impl& impl, Controller::OperationsMask operations);

  /**
   * @brief Helper to clear completely the parts of the model related with the characters specified by the given @p operations.
   *
   * @note It never clears the text stored in utf32.
   *
   * @param[in] impl The text controller impl.
   * @param[in] startIndex Index to the first character to be cleared.
   * @param[in] endIndex Index to the last character to be cleared.
   * @param[in] operations The operations required.
   */
  static void ClearCharacterModelData(Controller::Impl& impl, CharacterIndex startIndex, CharacterIndex endIndex, Controller::OperationsMask operations);

  /**
   * @brief Helper to clear completely the parts of the model related with the glyphs specified by the given @p operations.
   *
   * @note It never clears the text stored in utf32.
   * @note Character indices are transformed to glyph indices.
   *
   * @param[in] impl The text controller impl.
   * @param[in] startIndex Index to the first character to be cleared.
   * @param[in] endIndex Index to the last character to be cleared.
   * @param[in] operations The operations required.
   */
  static void ClearGlyphModelData(Controller::Impl& impl, CharacterIndex startIndex, CharacterIndex endIndex, Controller::OperationsMask operations);

  /**
   * @brief Helper to clear the parts of the model specified by the given @p operations and from @p startIndex to @p endIndex.
   *
   * @note It never clears the text stored in utf32.
   *
   * @param[in] impl The text controller impl.
   * @param[in] startIndex Index to the first character to be cleared.
   * @param[in] endIndex Index to the last character to be cleared.
   * @param[in] operations The operations required.
   */
  static void ClearModelData(Controller::Impl& impl, CharacterIndex startIndex, CharacterIndex endIndex, Controller::OperationsMask operations);
};

} // namespace Dali::Toolkit::Text

#endif // DALI_TOOLKIT_TEXT_CONTROLLER_IMPL_DATA_CLEARER_H

#ifndef DALI_TOOLKIT_TEXT_ANCHOR_H
#define DALI_TOOLKIT_TEXT_ANCHOR_H

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
#include <dali-toolkit/internal/text/text-definitions.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * @brief An anchor within the text.
 */
struct Anchor
{
  CharacterIndex startIndex; ///< The character's start index of the anchor within the string.
  CharacterIndex endIndex;   ///< The character's end index of the anchor within the string.
  char*          href;       ///< The url path

  uint32_t colorRunIndex;               ///< RunIndex of color run, used to change color when property updated or clicked.
  uint32_t underlinedCharacterRunIndex; ///< RunIndex of underline run, used to change color when property updated or clicked.

  Vector4 markupClickedColor; ///< The markup color of the anchor when clicked. if there is no markup attribute, the default color stored in the controller is used.

  bool     isClicked               = false; ///< Whether the anchor is clicked or not.
  bool     isMarkupColorSet        = false; ///< Whether the markup color has been set or not.
  bool     isMarkupClickedColorSet = false; ///< Whether the markup clicked color has been set or not.
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_ANCHOR_H
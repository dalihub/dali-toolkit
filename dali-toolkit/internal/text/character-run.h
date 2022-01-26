#ifndef DALI_TOOLKIT_TEXT_CHARACTER_RUN_H
#define DALI_TOOLKIT_TEXT_CHARACTER_RUN_H

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
 * @brief A run of consecutive characters.
 */
struct CharacterRun
{
  CharacterRun()
  : characterIndex{0u},
    numberOfCharacters{0u}
  {
  }

  CharacterRun(CharacterIndex characterIndex, Length numberOfCharacters)
  : characterIndex{characterIndex},
    numberOfCharacters{numberOfCharacters}
  {
  }

  CharacterIndex characterIndex;     ///< Index to the first character.
  Length         numberOfCharacters; ///< Number of characters in the run.

  //Methods

  /**
  * @brief Calculate the end index in run.
  * @return the end character index in run.
  */
  CharacterIndex GetEndCharacterIndex() const
  {
    DALI_ASSERT_DEBUG(!((0u == numberOfCharacters) && (characterIndex > 0u)) &&
                      "Toolkit::Text::CharacterRun. NumberOfCharacters should be greater than zero");

    // Note: Length is uint32. Extra validation to avoid a potential defects.
    return (numberOfCharacters == 0u ? 0u : (characterIndex + numberOfCharacters - 1u));
  }
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_CHARACTER_RUN_H

#ifndef __DALI_TOOLKIT_TEXT_MULTI_LANGUAGE_HELPER_FUNCTIONS_H__
#define __DALI_TOOLKIT_TEXT_MULTI_LANGUAGE_HELPER_FUNCTIONS_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/font-description-run.h>
#include <dali-toolkit/internal/text/script-run.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

/**
 * @brief Merges the font descriptions to retrieve the font Id for each character.
 *
 * @param[in] fontDescriptions The font descriptions.
 * @param[out] fontIds The font id for each character.
 * @param[in] defaultFontDescription The default font description.
 * @param[in] defaultPointSize The default font size.
 * @param[in] startIndex The character from where the fonts are merged.
 * @param[in] numberOfCharacters The number of characters to set the font.
 */
void MergeFontDescriptions( const Vector<FontDescriptionRun>& fontDescriptions,
                            Vector<FontId>& fontIds,
                            const TextAbstraction::FontDescription& defaultFontDescription,
                            TextAbstraction::PointSize26Dot6 defaultPointSize,
                            CharacterIndex startIndex,
                            Length numberOfCharacters );

/**
 * @brief Retrieves the script Id from the script run for a given character's @p index.
 *
 * If the character's index exceeds the current script run it increases the iterator to get the next one.
 *
 * @param[in] index The character's index.
 * @param[in,out] scriptRunIt Iterator to the current script run.
 * @param[in] scriptRunEndIt Iterator to one after the last script run.
 *
 * @return The script.
 */
Script GetScript( Length index,
                  Vector<ScriptRun>::ConstIterator& scriptRunIt,
                  const Vector<ScriptRun>::ConstIterator& scriptRunEndIt );

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_MULTI_LANGUAGE_HELPER_FUNCTIONS_H__

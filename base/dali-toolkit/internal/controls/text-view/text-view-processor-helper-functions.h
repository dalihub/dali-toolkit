#ifndef __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_PROCESSOR_HELPER_FUNCTIONS_H__
#define __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_PROCESSOR_HELPER_FUNCTIONS_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/text-actor.h>
#include <dali-toolkit/internal/controls/text-view/text-view-processor-types.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace TextViewProcessor
{

/**
 * Enum type used to update a size adding other one.
 * If the grow type is \e GrowWidth, size's widths are added and size's height is the max of both.
 * Alternativelly if grow type is \e GrowHeight heights are added and width is the max of both.
 */
enum SizeGrowType
{
  GrowWidth,
  GrowHeight,
};

/**
 * Updates the size of a character, word, line or the whole text with a given size.
 *
 * For characters, words, and lines the new height will be the maximum of both sizes
 * and the new width will be the sum of both.
 *
 * For the whole text, the new height will be the sum of both and the new width the max of both.
 *
 * The \e type parameter speficies which type of update is required.
 *
 * @param[in,out] size1 The current size to be updated.
 * @param[in] size2 The size of a character, word, or a line.
 * @param[in] type Type of update.
 */
void UpdateSize( Size& size1, const Size& size2, SizeGrowType type = GrowWidth );

/**
 * Return the type of separator (white space, new line or no separator) for the given character.
 *
 * @param[in] character The given character.
 *
 * @return The type of separator.
 */
TextSeparatorType GetTextSeparatorType( const Character& character );

/**
 * Choose a suitable font family name for the given styled text.
 *
 * It may modify the text-style of the given text by setting a suitable font-family.
 *
 * @param[in,out] text Text with style.
 */
void ChooseFontFamilyName( MarkupProcessor::StyledText& text );

/**
 * Retrieves the line, word and character indices for the given global character's index.
 *
 * i.e. The retrieved indices for the character 18 (j) for the following text would be: 2, 0, 2, 0
 *
 *  a b c d
 *  e f g h
 *  i j k l
 *  m n o p
 *
 * @param[in] index The global character index.
 * @param[in] textLayoutInfo Contains info about the number of characters per word, and line.
 * @param[out] indices Index to the line, word and character within the word where the given character is located.
 */
void GetIndicesFromGlobalCharacterIndex( std::size_t index,
                                         const TextLayoutInfo& textLayoutInfo,
                                         TextInfoIndices& indices );

/**
 * Clear the text of the given text-actors.
 *
 * @param[in] textActors The given text-actors.
 */
void ClearText( std::vector<TextActor>& textActors );

} //namespace TextViewProcessor

} //namespace Internal

} //namespace Toolkit

} //namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_PROCESSOR_HELPER_FUNCTIONS_H__

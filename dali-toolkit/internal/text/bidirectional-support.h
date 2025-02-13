#ifndef DALI_TOOLKIT_TEXT_BIDIRECTIONAL_SUPPORT_H
#define DALI_TOOLKIT_TEXT_BIDIRECTIONAL_SUPPORT_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/text-abstraction/bidirectional-support.h>
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali/public-api/common/dali-vector.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/bidirectional-line-info-run.h>
#include <dali-toolkit/internal/text/bidirectional-paragraph-info-run.h>
#include <dali-toolkit/internal/text/line-run.h>
#include <dali-toolkit/internal/text/script-run.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * Sets the bidirectional info into the logical model.
 *
 * @param[in] bidirectionalSupport BidirectionalSupport to use in this function.
 * @param[in] text Vector of UTF-32 characters.
 * @param[in] scripts Vector containing the script runs for the whole text.
 * @param[in] lineBreakInfo The line break info.
 * @param[in] startIndex The character from where the bidirectional info is set.
 * @param[in] numberOfCharacters The number of characters.
 * @param[out] bidirectionalInfo Vector with the bidirectional infor for each paragraph.
 * @param[out] lineInfoRuns Line runs with the visual to logical conversion maps.
 * @param[in] matchLayoutDirection Whether match for layout direction or not.
 * @param[in] layoutDirection The direction of the layout.
 */
void SetBidirectionalInfo(TextAbstraction::BidirectionalSupport& bidirectionalSupport,
                          const Vector<Character>&               text,
                          const Vector<ScriptRun>&               scripts,
                          const Vector<LineBreakInfo>&           lineBreakInfo,
                          CharacterIndex                         startIndex,
                          Length                                 numberOfCharacters,
                          Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo,
                          Vector<BidirectionalLineInfoRun>&      lineInfoRuns,
                          bool                                   matchLayoutDirection = false,
                          Dali::LayoutDirection::Type            layoutDirection      = LayoutDirection::LEFT_TO_RIGHT);

/**
 * @brief Sets the visual to logical map table for a given line.
 *
 * @param[in] bidirectionalSupport BidirectionalSupport to use in this function.
 * @param[in] bidirectionalParagraphInfo The paragraph's bidirectional info.
 * @param[out] lineInfoRuns Line runs with the visual to logical conversion maps.
 * @param[in] bidiLineIndex Index to the line's bidirectional info.
 * @param[in] startIndex The character from where the bidirectional info is set.
 * @param[in] numberOfCharacters The number of characters.
 * @param[in] startIndexInSecondHalfLine The character from where the bidirectional info is set for the second half of line..
 * @param[in] numberOfCharactersInSecondHalfLine The number of characters for the second half of line..
 * @param[in] direction The direction of the line.
 */
void ReorderLine(TextAbstraction::BidirectionalSupport& bidirectionalSupport,
                 const BidirectionalParagraphInfoRun&   bidirectionalParagraphInfo,
                 Vector<BidirectionalLineInfoRun>&      lineInfoRuns,
                 BidirectionalLineRunIndex              bidiLineIndex,
                 CharacterIndex                         startIndex,
                 Length                                 numberOfCharacters,
                 CharacterIndex                         startIndexInSecondHalfLine,
                 Length                                 numberOfCharactersInSecondHalfLine,
                 CharacterDirection                     direction);

/**
 * @brief Replaces any character in the right to left paragraphs which could be mirrored.
 *
 * @param[in] bidirectionalSupport BidirectionalSupport to use in this function.
 * @param[in] text The text.
 * @param[in] directions Vector with the direction of each paragraph.
 * @param[in] bidirectionalInfo Vector with the bidirectional infor for each paragraph.
 * @param[in] startIndex The character from where the text is mirrored.
 * @param[in] numberOfCharacters The number of characters.
 * @param[out] mirroredText The mirroredText.
 *
 * @return @e true if a character has been replaced.
 */
bool GetMirroredText(TextAbstraction::BidirectionalSupport&       bidirectionalSupport,
                     const Vector<Character>&                     text,
                     const Vector<CharacterDirection>&            directions,
                     const Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo,
                     CharacterIndex                               startIndex,
                     Length                                       numberOfCharacters,
                     Vector<Character>&                           mirroredText);

/**
 * @brief Retrieves the characters' directions.
 *
 * @pre The @p logicalModel needs to have a text set.
 * @pre The @p logicalModel needs to have the bidirectional info indices for each paragraph set.
 *
 * @param[in] bidirectionalSupport BidirectionalSupport to use in this function.
 * @param[in] bidirectionalInfo Vector with the bidirectional infor for each paragraph.
 * @param[in] totalNumberOfCharacters The number of characters of the whole text.
 * @param[in] startIndex The character from where the direction info is set.
 * @param[in] numberOfCharacters The number of characters.
 * @param[out] directions The direction, @e false is left to right and @e true is right to left, of each character of the paragraph.
 */
void GetCharactersDirection(TextAbstraction::BidirectionalSupport&       bidirectionalSupport,
                            const Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo,
                            Length                                       totalNumberOfCharacters,
                            CharacterIndex                               startIndex,
                            Length                                       numberOfCharacters,
                            Vector<CharacterDirection>&                  directions);

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_BIDIRECTIONAL_SUPPORT_H

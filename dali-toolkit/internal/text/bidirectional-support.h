#ifndef __DALI_TOOLKIT_TEXT_BIDIRECTIONAL_SUPPORT_H__
#define __DALI_TOOLKIT_TEXT_BIDIRECTIONAL_SUPPORT_H__

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

// EXTERNAL INCLUDES
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

class LogicalModel;
class VisualModel;

/**
 * Sets the bidirectional info into the logical model.
 *
 * @param[in] text Vector of UTF-32 characters.
 * @param[in] scripts Vector containing the script runs for the whole text.
 * @param[in] lineBreakInfo The line break info.
 * @param[out] bidirectionalInfo Vector with the bidirectional infor for each paragraph.
 */
void SetBidirectionalInfo( const Vector<Character>& text,
                           const Vector<ScriptRun>& scripts,
                           const Vector<LineBreakInfo>& lineBreakInfo,
                           Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo );

/**
 * Replaces the bidirectional info from the logical model.
 *
 * @pre The @p model needs to have a text set.
 * @pre The @p model needs to have the line break info set.
 *
 * If the @p numberOfCharactersToRemove is zero, this operation is like an insert.
 * If the @p numberOfCharactersToInsert is zero, this operation is like a remove.
 *
 * @param[in,out] model The text's logical model.
 * @param[in] characterIndex Index to the first character.
 * @param[in] numberOfCharactersToRemove The number of characters removed from the text.
 * @param[in] numberOfCharactersToInsert The number of characters inserted in the text.
 */
void ReplaceBidirectionalInfo( LogicalModel& model,
                               CharacterIndex characterIndex,
                               Length numberOfCharactersToRemove,
                               Length numberOfCharactersToInsert );

/**
 * Sets the visual to logical map tables.
 *
 * Any map tables previously set are removed.
 *
 * @pre The @p logicalModel needs to have a text set.
 * @pre The @p logicalModel needs to have the bidirectional info indices for each paragraph set.
 * @pre The @p visualModel needs to have the laid-out lines info set.
 *
 * @param[in] bidirectionalInfo Vector with the bidirectional infor for each paragraph.
 * @param[in] lineRuns The line runs converted to characters.
 * @param[out] lineInfoRuns line runs with the visual to logical conversion maps.
 */
void ReorderLines( const Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo,
                   const Vector<LineRun>& lineRuns,
                   Vector<BidirectionalLineInfoRun>& lineInfoRuns );

/**
 * Replaces the visual to logical and logical to visual map tables.
 *
 * @pre The @p logicalModel needs to have a text set.
 * @pre The @p logicalModel needs to have the line break info set.
 * @pre The @p visualModel needs to have the laid-out lines info set.
 *
 * If the @p numberOfCharactersToRemove is zero, this operation is like an insert.
 * If the @p numberOfCharactersToInsert is zero, this operation is like a remove.
 *
 * @param[in,out] logicalModel The text's logical model.
 * @param[in] visualModel The text's visual model.
 * @param[in] characterIndex Index to the first character.
 * @param[in] numberOfCharactersToRemove The number of characters removed from the text.
 * @param[in] numberOfCharactersToInsert The number of characters inserted in the text.
 */
void ReorderLines( LogicalModel& logicalModel,
                   const VisualModel& visualModel,
                   CharacterIndex characterIndex,
                   Length numberOfCharactersToRemove,
                   Length numberOfCharactersToInsert );

/**
 * @brief Replaces any character which could be mirrored.
 *
 * @param[in] text The text.
 * @param[in] mirroredText The mirroredText.
 *
 * @return @e true if a character has been replaced.
 */
bool GetMirroredText( const Vector<Character>& text,
                      Vector<Character>& mirroredText );

/**
 * @brief Retrieves the character's directions.
 *
 * @pre The @p logicalModel needs to have a text set.
 * @pre The @p logicalModel needs to have the bidirectional info indices for each paragraph set.
 *
 * @param[in] bidirectionalInfo Vector with the bidirectional infor for each paragraph.
 * @param[out] directions The direction, @e false is left to right and @e true is right to left, of each character of the paragraph.
 */
void GetCharactersDirection( const Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo,
                             Vector<CharacterDirection>& directions );

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_BIDIRECTIONAL_SUPPORT_H__

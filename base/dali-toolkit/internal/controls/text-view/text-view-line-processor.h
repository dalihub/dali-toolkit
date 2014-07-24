#ifndef __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_LINE_PROCESSOR_H__
#define __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_LINE_PROCESSOR_H__

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
#include "text-view-impl.h"
#include "text-view-processor-types.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace TextViewProcessor
{

/**
 * Updates the line layout size info.
 *
 * @param[in,out] lineLayoutInfo The line layout info.
 * @param[in] lineHeightOffset The line height offset.
 */
void UpdateLineLayoutInfo( TextViewProcessor::LineLayoutInfo& lineLayoutInfo, float lineHeightOffset );

/**
 * Creates a data structure with info to layout the line, and data structures with useful info to modify the layout data structure if characters are added or removed.
 *
 * @param[in] line The styled line.
 * @param[in,out] relayoutData Natural size (metrics), layout, text-actor info and conversion from visual to logical order and vice versa (for RTL text).
 * @param[out] lineLayoutInfo Layout info for the whole line.
 */
void CreateLineInfo( const MarkupProcessor::StyledTextArray& line,
                     TextView::RelayoutData& relayoutData,
                     TextViewProcessor::LineLayoutInfo& lineLayoutInfo );

/**
 * Removes a given number of groups of words from the given line.
 *
 * @pre \e groupIndex and \e groupIndex + \e numberOfGroups can't exceed the bounds of the line.
 *
 * @param[in] groupIndex Index to the group of words within the line with the starting position to be deleted.
 * @param[in] numberOfGroups The number of group of words to be deleted.
 * @param[in] lineHeightOffset Additional space between lines. Needed to update layout info.
 * @param[in,out] lineLayout The input is the layout info of the line. The output is the layout info of the line without the removed group of words.
 */
void RemoveWordGroupsFromLine( std::size_t groupIndex,
                               std::size_t numberOfGroups,
                               const PointSize& lineHeightOffset,
                               LineLayoutInfo& lineLayout );

/**
 * Splits a given line in two.
 *
 * @note It deletes whatever there is in the last part of the line.
 *
 * @param[in] indices Index to the group of words within the line, index to the word and index to the character within the word to split the line.
 * @param[in] lineHeightOffset Additional space between lines. Needed to update layout info.
 * @param[in,out] firstLineLayoutInfo The input is the layout info of the given line. The output is the first part of the input line (from the group of words \e 0 to the group of words \e groupPosition).
 * @param[in,out] lastLineLayoutInfo Layout info of the last part of the given line ( from the group of words \e groupPosition + \e 1 to the end of the line).
 */
void SplitLine( const TextInfoIndices& indices,
                const PointSize& lineHeightOffset,
                LineLayoutInfo& firstLineLayoutInfo,
                LineLayoutInfo& lastLineLayoutInfo );

/**
 * Merges the two given lines by adding groups of words of the last line to the firs one.
 *
 * @note Does nothing if last part of the line is empty.
 * @note If the first part of the line is empty it just copy the last part to it.
 * @note it asserts if the last word of the first group is a line separator (new line character)
 *
 * @param[in,out] firstLineLineLayoutInfo The input is the layout info of the first line. The output is the layout info of the merged line.
 * @param[in] lastLineLayoutInfo Layout info of the last line.
 *
 */
void MergeLine( LineLayoutInfo& firstLineLineLayoutInfo,
                const LineLayoutInfo& lastLineLayoutInfo );

/**
 * Retrieves the layout information of the last word of the given line.
 *
 * @param[in] lineLayoutInfo The line layout.
 *
 * @return Layout information of the last word of the line.
 */
WordLayoutInfo GetLastWordLayoutInfo( const LineLayoutInfo& lineLayoutInfo );

/**
 * Retrieves the layout information of the first character of the given line.
 *
 * @param[in] lineLayoutInfo The line layout.
 *
 * @return Layout information of the first character of the line.
 */
CharacterLayoutInfo GetFirstCharacterLayoutInfo( const LineLayoutInfo& lineLayoutInfo );

/**
 * Retrieves the layout information of the last character of the given line.
 *
 * @param[in] lineLayoutInfo The line layout.
 *
 * @return Layout information of the last character of the line.
 */
CharacterLayoutInfo GetLastCharacterLayoutInfo( const LineLayoutInfo& lineLayoutInfo );

/**
 * Collects text-actors from the given lines and stores them into the text-actor vector.
 *
 * @param[out] textActors Stores the text-actors of the given lines.
 * @param[in] textLayoutInfo Whole text with the given lines.
 * @param[in] lineIndexBegin The first line.
 * @param[in] lineIndexEnd The last line.
 */
void CollectTextActorsFromLines( std::vector<TextActor>& textActors, const TextLayoutInfo& textLayoutInfo, std::size_t lineIndexBegin, std::size_t lineIndexEnd );

} //namespace TextViewProcessor

} //namespace Internal

} //namespace Toolkit

} //namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_LINE_PROCESSOR_H__

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
#include <dali-toolkit/internal/controls/text-view/text-view-impl.h>

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
void UpdateLineLayoutInfo( LineLayoutInfo& lineLayoutInfo, float lineHeightOffset );

/**
 * Creates a data structure with info to layout the line, and data structures with useful info to modify the layout data structure if characters are added or removed.
 *
 * @param[in] line The styled line.
 * @param[in,out] relayoutData Natural size (metrics), layout, text-actor info and conversion from visual to logical order and vice versa (for RTL text).
 * @param[out] lineLayoutInfo Layout info for the whole line.
 */
void CreateLineInfo( const MarkupProcessor::StyledTextArray& line,
                     TextView::RelayoutData& relayoutData,
                     LineLayoutInfo& lineLayoutInfo );

/**
 * Removes a given number of words from the given line.
 *
 * @pre \e wordIndex and \e wordIndex + \e numberOfWords can't exceed the bounds of the line.
 *
 * @param[in] wordIndex Index to the word within the line with the starting position to be deleted.
 * @param[in] numberOfWords The number of words to be deleted.
 * @param[in] lineHeightOffset Additional space between lines. Needed to update layout info.
 * @param[in,out] lineLayout The input is the layout info of the line. The output is the layout info of the line without the removed words.
 */
void RemoveWordsFromLine( std::size_t wordIndex,
                          std::size_t numberOfWords,
                          float lineHeightOffset,
                          LineLayoutInfo& lineLayout );

/**
 * @param[in,out] relayoutData Natural size (metrics), layout, text-actor info.
 * @param[in] numberOfCharacters The number of characters to be deleted.
 * @param[out] mergeWords Whether words need to be merged after removing characters.
 * @param[out] mergeLines Whether current line need to be merged with the next one.
 * @param[in,out] textInfoIndicesBegin Indices to the line, word and characters from where to delete characters. It returns from where words need to be removed.
 * @param[out] textInfoIndicesEnd If lines or words need to be merged it returns info to delete them (If a word is merged, it has to be removed. Equal for lines).
 * @param[out] textInfoMergeIndicesBegin The indices to the first part of the line and word to be merged.
 * @param[out] textInfoMergeIndicesEnd The indices to the last part of the line and word to be merged.
 * @param[in,out] lineLayout Layout info of the line where the word is located.
 * @param[out] removedTextActorsFromFirstWord Stores removed text-actors of the word pointed by the 'begin' index.
 * @param[out] removedTextActorsFromLastWord Stores removed text-actors of the word pointed by the 'end' index.
 */
void RemoveCharactersFromLineInfo( TextView::RelayoutData& relayoutData,
                                   std::size_t numberOfCharacters,
                                   bool& mergeWords,
                                   bool& mergeLines,
                                   TextInfoIndices& textInfoIndicesBegin,
                                   TextInfoIndices& textInfoIndicesEnd,
                                   TextInfoIndices& textInfoMergeIndicesBegin,
                                   TextInfoIndices& textInfoMergeIndicesEnd,
                                   LineLayoutInfo& lineLayout,
                                   std::vector<TextActor>& removedTextActorsFromFirstWord,
                                   std::vector<TextActor>& removedTextActorsFromLastWord );

/**
 * Splits a given line in two.
 *
 * @note It deletes whatever there is in the last part of the line.
 *
 * @param[in] indices Index to the word within the line and index to the character within the word to split the line.
 * @param[in] lineHeightOffset Additional space between lines. Needed to update layout info.
 * @param[in,out] firstLineLayoutInfo The input is the layout info of the given line. The output is the first part of the input line.
 * @param[in,out] lastLineLayoutInfo Layout info of the last part of the given line.
 */
void SplitLine( const TextInfoIndices& indices,
                const PointSize& lineHeightOffset,
                LineLayoutInfo& firstLineLayoutInfo,
                LineLayoutInfo& lastLineLayoutInfo );

/**
 * Merges the two given lines by adding words of the last line to the firs one.
 *
 * @note Does nothing if last part of the line is empty.
 * @note If the first part of the line is empty it just copy the last part to it.
 * @note it asserts if the last word of the first line is a line separator (new line character)
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

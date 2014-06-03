#ifndef __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_WORD_GROUP_PROCESSOR_H__
#define __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_WORD_GROUP_PROCESSOR_H__

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
 * Updates the word group layout size info.
 *
 * @param[in,out] wordGroupLayoutInfo The word group layout info.
 */
void UpdateGroupLayoutInfo( TextViewProcessor::WordGroupLayoutInfo& wordGroupLayoutInfo );

/**
 * Creates a data structure with info to layout the group of words, and data structures with useful info to modify the layout data structure if characters are added or removed.
 *
 * @param[in] wordGroup The styled group of words.
 * @param[in,out] textLayoutInfo Layout info for the whole text. This function uses it to check if the words should be split into individual characters. It also modifies which is the maximum word width.
 * @param[out] wordGroupLayoutInfo Layout info for the whole group of words.
 */
void CreateWordGroupInfo( const MarkupProcessor::StyledTextArray& wordGroup,
                          TextViewProcessor::TextLayoutInfo& textLayoutInfo,
                          TextViewProcessor::WordGroupLayoutInfo& wordGroupLayoutInfo );

/**
 * Removes a given number of words from the given group of words.
 *
 * @pre \e wordIndex and \e wordIndex + \e numberOfWords can't exceed the bounds of the group.
 *
 * @param[in] wordIndex Index to the word within the group of words with the starting position to be deleted.
 * @param[in] numberOfWords The number of words to be deleted.
 * @param[in,out] wordGroupLayoutInfo The input is the layout info of the group of words. The output is the layout info of the group of words without the removed words.
 */
void RemoveWordsFromWordGroup( std::size_t wordIndex,
                               std::size_t numberOfWords,
                               WordGroupLayoutInfo& wordGroupLayoutInfo );

/**
 * @param[in,out] relayoutData Natural size (metrics), layout, text-actor info.
 * @param[in] numberOfCharacters The number of characters to be deleted.
 * @param[out] mergeWords Whether words need to be merged after removing characters.
 * @param[out] mergeLines Whether current line need to be merged with the next one.
 * @param[in,out] textInfoIndicesBegin Indices to the line, word and characters from where to delete characters. It returns from where words need to be removed.
 * @param[out] textInfoIndicesEnd If lines or words need to be merged it returns info to delete them (If a word is merged, it has to be removed. Equal for lines).
 * @param[out] textInfoMergeIndicesBegin The indices to the first part of the line, group and word to be merged.
 * @param[out] textInfoMergeIndicesEnd The indices to the last part of the line, group and word to be merged.
 * @param[in,out] groupLayout Layout info of the group of words where the word is located.
 * @param[out] removedTextActorsFromFirstWord Stores removed text-actors of the word pointed by the 'begin' index.
 * @param[out] removedTextActorsFromLastWord Stores removed text-actors of the word pointed by the 'end' index.
 */
void RemoveCharactersFromWordGroupInfo( TextView::RelayoutData& relayoutData,
                                        std::size_t numberOfCharacters,
                                        bool& mergeWords,
                                        bool& mergeLines,
                                        TextViewProcessor::TextInfoIndices& textInfoIndicesBegin,
                                        TextViewProcessor::TextInfoIndices& textInfoIndicesEnd,
                                        TextViewProcessor::TextInfoIndices& textInfoMergeIndicesBegin,
                                        TextViewProcessor::TextInfoIndices& textInfoMergeIndicesEnd,
                                        TextViewProcessor::WordGroupLayoutInfo& groupLayout,
                                        std::vector<TextActor>& removedTextActorsFromFirstWord,
                                        std::vector<TextActor>& removedTextActorsFromLastWord );

/**
 * Splits a group of words in two.
 *
 * @note It deletes whatever there is in the last part of the group of words.
 *
 * @param[in] indices Index to the word within the group of words and index to the character within the word where to split the word.
 * @param[in,out] firstWordGroupLayoutInfo The input is the layout info of the given group of words. The output is the first part of the input group of words (from the word \e 0 to the word \e wordPosition).
 * @param[in,out] lastWordGroupLayoutInfo Layout info of the last part of the given group of words ( from the word \e wordPosition + \e 1 to the end of the group of words).
 */
void SplitWordGroup( const TextInfoIndices& indices,
                     WordGroupLayoutInfo& firstWordGroupLayoutInfo,
                     WordGroupLayoutInfo& lastWordGroupLayoutInfo );

/**
 * Merges the two given groups of words by adding words of the last group of words to the firs one.
 *
 * @note Does nothing if last part of the group of words is empty.
 * @note If the first part of the group of words is empty it just copy the last part to it.
 * @note It assets if groups of words contain text with different direction. (Left to Right and Right to Left text)
 * @note it asserts if the last word of the first group is a line separator (new line character)
 *
 * @param[in,out] firstWordGroupLayoutInfo The input is the layout info of the first group of words. The output is the layout info of the merged group of words.
 * @param[in] lastWordGroupLayoutInfo Layout info of the last group of words.
 *
 */
void MergeWordGroup( WordGroupLayoutInfo& firstWordGroupLayoutInfo,
                     const WordGroupLayoutInfo& lastWordGroupLayoutInfo );

/**
 * Collects text-actors from the given line, within the given indices, and stores them into the text-actor vector.
 *
 * @param[out] textActors Stores the text-actors of the given line.
 * @param[in] line The line with groups of words.
 * @param[in] groupIndexBegin Index to the first group of words.
 * @param[in] groupIndexEnd Index to the last group of words.
 */
void CollectTextActorsFromGroups( std::vector<TextActor>& textActors, const LineLayoutInfo& line, std::size_t groupIndexBegin, std::size_t groupIndexEnd );

} //namespace TextViewProcessor

} //namespace Internal

} //namespace Toolkit

} //namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_WORD_GROUP_PROCESSOR_H__

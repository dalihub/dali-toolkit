#ifndef __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_PARAGRAPH_PROCESSOR_H__
#define __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_PARAGRAPH_PROCESSOR_H__

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
 * Creates a data structure with info to layout the paragraph, and data structures with useful info to modify the layout data structure if characters are added or removed.
 *
 * @param[in] paragraph The styled paragraph.
 * @param[in,out] relayoutData Natural size (metrics), layout, text-actor info and conversion from visual to logical order and vice versa (for RTL text).
 * @param[out] paragraphLayoutInfo Layout info for the whole paragraph.
 */
void CreateParagraphInfo( const MarkupProcessor::StyledTextArray& line,
                          TextView::RelayoutData& relayoutData,
                          ParagraphLayoutInfo& paragraphLayoutInfo );

/**
 * Updates the paragraph layout info.
 *
 * @param[in,out] paragraphLayoutInfo The paragraph layout info.
 * @param[in] lineHeightOffset The offset added to the line height.
 */
void UpdateLayoutInfo( ParagraphLayoutInfo& paragraphLayoutInfo, float lineHeightOffset );

/**
 * Removes a given number of words from the given paragraph.
 *
 * @pre \e wordIndex and \e wordIndex + \e numberOfWords can't exceed the bounds of the paragraph.
 *
 * @param[in] wordIndex Index to the word within the paragraph with the starting position to be deleted.
 * @param[in] numberOfWords The number of words to be deleted.
 * @param[in] lineHeightOffset The offset added to the line height. Needed to update the layout info.
 * @param[in,out] paragraphLayout The input is the layout info of the paragraph. The output is the layout info of the paragraph without the removed words.
 */
void RemoveWordsFromParagraph( std::size_t wordIndex,
                               std::size_t numberOfWords,
                               float lineHeightOffset,
                               ParagraphLayoutInfo& paragraphLayout );

/**
 * @param[in,out] relayoutData Natural size (metrics), layout, text-actor info.
 * @param[in] numberOfCharacters The number of characters to be deleted.
 * @param[out] mergeWords Whether words need to be merged after removing characters.
 * @param[out] mergeParagraphs Whether current paragraph need to be merged with the next one.
 * @param[in,out] textInfoIndicesBegin Indices to the paragraph, word and characters from where to delete characters. It returns from where words need to be removed.
 * @param[out] textInfoIndicesEnd If paragraphs or words need to be merged it returns info to delete them (If a word is merged, it has to be removed. Equal for paragraphs).
 * @param[out] textInfoMergeIndicesBegin The indices to the first part of the paragraph and word to be merged.
 * @param[out] textInfoMergeIndicesEnd The indices to the last part of the paragraph and word to be merged.
 * @param[in,out] paragraphLayout Layout info of the paragraph where the word is located.
 * @param[out] removedTextActorsFromFirstWord Stores removed text-actors of the word pointed by the 'begin' index.
 * @param[out] removedTextActorsFromLastWord Stores removed text-actors of the word pointed by the 'end' index.
 */
void RemoveCharactersFromParagraphInfo( TextView::RelayoutData& relayoutData,
                                        std::size_t numberOfCharacters,
                                        bool& mergeWords,
                                        bool& mergeParagraphs,
                                        TextInfoIndices& textInfoIndicesBegin,
                                        TextInfoIndices& textInfoIndicesEnd,
                                        TextInfoIndices& textInfoMergeIndicesBegin,
                                        TextInfoIndices& textInfoMergeIndicesEnd,
                                        ParagraphLayoutInfo& paragraphLayout,
                                        std::vector<TextActor>& removedTextActorsFromFirstWord,
                                        std::vector<TextActor>& removedTextActorsFromLastWord );

/**
 * Splits a given paragraph in two.
 *
 * @note It deletes whatever there is in the last part of the paragraph.
 *
 * @param[in] indices Index to the word within the paragraph and index to the character within the word to split the paragraph.
 * @param[in] lineHeightOffset Additional space between lines. Needed to update layout info.
 * @param[in,out] firstParagraphLayoutInfo The input is the layout info of the given paragraph. The output is the first part of the input paragraph.
 * @param[in,out] lastParagraphLayoutInfo Layout info of the last part of the given paragraph.
 */
void SplitParagraph( const TextInfoIndices& indices,
                     float lineHeightOffset,
                     ParagraphLayoutInfo& firstParagraphLayoutInfo,
                     ParagraphLayoutInfo& lastParagraphLayoutInfo );

/**
 * Merges the two given paragraphs by adding words of the last paragraph to the firs one.
 *
 * @note Does nothing if last part of the paragraph is empty.
 * @note If the first part of the paragraph is empty it just copy the last part to it.
 * @note it asserts if the last word of the first paragraph is a paragraph separator (new paragraph character)
 *
 * @param[in,out] firstParagraphLayoutInfo The input is the layout info of the first paragraph. The output is the layout info of the merged paragraph.
 * @param[in] lastParagraphLayoutInfo Layout info of the last paragraph.
 *
 */
void MergeParagraph( ParagraphLayoutInfo& firstParagraphLayoutInfo,
                     const ParagraphLayoutInfo& lastParagraphLayoutInfo );

/**
 * Retrieves the layout information of the last word of the given paragraph.
 *
 * @param[in] paragraphLayoutInfo The paragraph layout.
 *
 * @return Layout information of the last word of the paragraph.
 */
WordLayoutInfo GetLastWordLayoutInfo( const ParagraphLayoutInfo& paragraphLayoutInfo );

/**
 * Retrieves the layout information of the first character of the given paragraph.
 *
 * @param[in] paragraphLayoutInfo The paragraph layout.
 *
 * @return Layout information of the first character of the paragraph.
 */
CharacterLayoutInfo GetFirstCharacterLayoutInfo( const ParagraphLayoutInfo& paragraphLayoutInfo );

/**
 * Retrieves the layout information of the last character of the given paragraph.
 *
 * @param[in] paragraphLayoutInfo The paragraph layout.
 *
 * @return Layout information of the last character of the paragraph.
 */
CharacterLayoutInfo GetLastCharacterLayoutInfo( const ParagraphLayoutInfo& paragraphLayoutInfo );

/**
 * Collects text-actors from the given paragraphs and stores them into the text-actor vector.
 *
 * @param[out] textActors Stores the text-actors of the given paragraphs.
 * @param[in] textLayoutInfo Whole text with the given paragraphs.
 * @param[in] paragraphIndexBegin The first paragraph.
 * @param[in] paragraphIndexEnd The last paragraph.
 */
void CollectTextActorsFromParagraphs( std::vector<TextActor>& textActors, const TextLayoutInfo& textLayoutInfo, std::size_t paragraphIndexBegin, std::size_t paragraphIndexEnd );

} //namespace TextViewProcessor

} //namespace Internal

} //namespace Toolkit

} //namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_PARAGRAPH_PROCESSOR_H__

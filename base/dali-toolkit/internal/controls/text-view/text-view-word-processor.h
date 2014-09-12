#ifndef __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_WORD_PROCESSOR_H__
#define __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_WORD_PROCESSOR_H__

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
 * Creates a data structure with info to layout the word, and data structures with useful info to modify the layout data structure if characters are added or removed.
 *
 * @param[in] word The styled word.
 * @param[out] wordLayoutInfo Layout info for all characters of the word.
 */
void CreateWordTextInfo( const MarkupProcessor::StyledTextArray& word,
                         WordLayoutInfo& wordLayoutInfo );

/**
 * Removes a given number of characters from the given word.
 *
 * It calls the RemoveCharactersFromWord() function to remove characters from the word.
 *
 * If the word is a white space \e mergeWords will return \e true and \e textInfoMergeIndicesBegin and \e textInfoMergeIndicesEnd will be set to merge the two adjacent words.
 * If the word is a new paragraph character \e mergeParagraphs will return \e true and \e textInfoMergeIndicesBegin and \e textInfoMergeIndicesEnd will be set to merge the two paragraphs.
 *
 * @param[in,out] relayoutData Natural size (metrics), layout, text-actor info.
 * @param[in] numberOfCharacters The number of characters to be deleted.
 * @param[out] mergeWords Whether adjacent words need to be merged.
 * @param[out] mergeParagraphs Whether current paragraph need to be merged with the next one.
 * @param[in,out] textInfoIndicesBegin Indices to the paragraph, word and characters from where to delete characters. It returns from where words need to be removed.
 * @param[out] textInfoIndicesEnd If paragraphs or words need to be merged it returns info to delete them (If a word is merged, it has to be removed. Equal for paragraphs).
 * @param[out] textInfoMergeIndicesBegin The indices to the first part of the paragraph and word to be merged.
 * @param[out] textInfoMergeIndicesEnd The indices to the last part of the paragraph and word to be merged.
 * @param[in,out] paragraphLayout Layout info of the paragraph where the word is located.
 * @param[out] removedTextActors Stores handles of temoved text-actors.
 */
void RemoveCharactersFromWordInfo( TextView::RelayoutData& relayoutData,
                                   std::size_t numberOfCharacters,
                                   bool& mergeWords,
                                   bool& mergeParagraphs,
                                   TextInfoIndices& textInfoIndicesBegin,
                                   TextInfoIndices& textInfoIndicesEnd,
                                   TextInfoIndices& textInfoMergeIndicesBegin,
                                   TextInfoIndices& textInfoMergeIndicesEnd,
                                   ParagraphLayoutInfo& paragraphLayout,
                                   std::vector<TextActor>& removedTextActors );
/**
 * Removes a given number of characters from the given word.
 *
 * @pre \e positon and \e position + \e numberOfCharacters can't exceed the bounds of the word.
 *
 * @param[in] position Character index within the word with the starting position to be deleted.
 * @param[in] numberOfCharacters The number of characters to be deleted.
 * @param[in,out] wordLayout The input is the layout info of the word. The output is the layout info of the word without the removed characters.
 */
void RemoveCharactersFromWord( std::size_t position,
                               std::size_t numberOfCharacters,
                               WordLayoutInfo& wordLayout );

/**
 * Splits a word in two.
 *
 * Removes part of the text from the input word and creates a new word with the removed text.
 *
 * i.e. The result of split 'word' by the position 3 would be 'wor' and 'd'.
 *
 * @note It deletes whatever there is in the last part of the word.
 *
 * @param[in] position Character index where to split the word.
 * @param[in,out] firstWordLayoutInfo The input is the layout info of the given word. The output is the first part of the input word (from the character \e 0 to the character \e position - \e 1).
 * @param[out] lastWordLayoutInfo Layout info of the last part of the given word ( from the character \e position to the end of the word).
 */
void SplitWord( std::size_t position,
                WordLayoutInfo& firstWordLayoutInfo,
                WordLayoutInfo& lastWordLayoutInfo );

/**
 * Merges the two given words by adding characters of the last word to the firs one.
 *
 * @note Does nothing if last part of the word is empty.
 * @note If the first part of the word is empty it just copy the last part to it.
 * @note It asserts if the first or the last word is a word separator (white space) or a paragraph separator (new paragraph character)
 *
 * @param[in,out] firstWordLayoutInfo The input is the layout info of the first word. The output is the layout info of the merged word.
 * @param[in] lastWordLayoutInfo Layout info of the last word.
 */
void MergeWord( WordLayoutInfo& firstWordLayoutInfo,
                const WordLayoutInfo& lastWordLayoutInfo );

/**
 * Retrieves the layout information of the first character of the given word.
 *
 * @param[in] wordLayoutInfo The word layout.
 *
 * @return Layout information of the first character of the word.
 */
CharacterLayoutInfo GetFirstCharacterLayoutInfo( const WordLayoutInfo& wordLayoutInfo );

/**
 * Retrieves the layout information of the last character of the given word.
 *
 * @param[in] wordLayoutInfo The word layout.
 *
 * @return Layout information of the last character of the word.
 */
CharacterLayoutInfo GetLastCharacterLayoutInfo( const WordLayoutInfo& wordLayoutInfo );

/**
 * Collects text-actors from the given word, within the given indices, and stores them into the text-actor vector.
 *
 * @param[out] textActors Stores the text-actors of the given word.
 * @param[in] characterIndexBegin The first text-actor to be stored.
 * @param[in] characterIndexEnd The last text-actor to be stored.
 */
void CollectTextActors( std::vector<TextActor>& textActors, const WordLayoutInfo& word, std::size_t characterIndexBegin, std::size_t characterIndexEnd );

/**
 * Collects text-actors from the given paragraph, within the given indices, and stores them into the text-actor vector.
 *
 * @param[out] textActors Stores the text-actors of the given paragraph.
 * @param[in] paragraph The paragraph with the words.
 * @param[in] wordIndexBegin Index to the first word.
 * @param[in] wordIndexEnd Index to the last word.
 */
void CollectTextActorsFromWords( std::vector<TextActor>& textActors, const ParagraphLayoutInfo& paragraph, std::size_t wordIndexBegin, std::size_t wordIndexEnd );

} //namespace TextViewProcessor

} //namespace Internal

} //namespace Toolkit

} //namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_WORD_PROCESSOR_H__

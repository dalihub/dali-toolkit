#ifndef __DALI_TOOLKIT_TEXT_PROCESSOR_H__
#define __DALI_TOOLKIT_TEXT_PROCESSOR_H__

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
#include <dali-toolkit/public-api/markup-processor/markup-processor.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace TextProcessor
{
// Forward declarations.
struct BidirectionalParagraphInfo;

/**
 * Splits the given text in paragraphs.
 *
 * @note Assumes the StyledTextArray has 1 Character per Text element. (which is the case for text in TextInput, but
 * not necessarily the case for text in TextView)
 *
 * @param [in] styledTextArray The given text.
 * @param [out] paragraphs The text split in paragraphs.
 * @param [out] styles The styles of each character of each paragraph.
 */
void SplitInParagraphs( const MarkupProcessor::StyledTextArray& styledTextArray,
                        std::vector<Text>& paragraphs,
                        std::vector< Vector<TextStyle*> >& styles );

/**
 * Finds the position of all word separators (currently white spaces and new paragraph characters '\n') in the given text.
 *
 * @param [in] paragraph The given paragraph.
 * @param [out] positions Positions within the paragraph of all word sepatators.
 */
void SplitInWords( const Text& paragraph,
                   Vector<std::size_t>& positions );

/**
 * Finds the nearest word in a string to a specified
 * offset (in Characters).
 *
 * @note Assumes the StyledTextArray has 1 Character per Text element. (which is the case for text in TextInput, but
 * not necessarily the case for text in TextView)
 *
 * @param[in] text The text to search through.
 * @param[in] offset The current offset position to begin search from.
 * @param[out] start The start position of nearest word
 * @param[out] end The end position of nearest word
 */
void FindNearestWord( const MarkupProcessor::StyledTextArray& text, size_t offset, size_t& start, size_t& end);

} // namespace TextProcessor

} // namespace Internal

} // namespace DaliToolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_PROCESSOR_H__

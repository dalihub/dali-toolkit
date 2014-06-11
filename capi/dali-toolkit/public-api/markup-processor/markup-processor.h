#ifndef __DALI_TOOLKIT_MARKUP_PROCESSOR_H__
#define __DALI_TOOLKIT_MARKUP_PROCESSOR_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/**
 * @addtogroup CAPI_DALI_TOOLKIT_MARKUP_PROCESSOR_MODULE
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>

#include <dali/public-api/text/text.h>
#include <dali/public-api/text/text-style.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

/**
 * @brief Markup Processor enumerations, structures and functions.
 *
 * See the \link markup-processor Markup Processor \endlink page in the Programming Guide.
 */
namespace MarkupProcessor
{

/**
 * @brief A pair of Dali::Text and Dali::TextStyle.
 *
 * mText is a Dali::Text object which can store text in different
 * languages. mStyle is a Dali::TextStyle object which can store all
 * text styling features provided by Dali::TextActor.
 */
struct StyledText
{
  /**
   * @brief Constructor
   */
  StyledText()
  : mText(),
    mStyle()
  {
  }

  /**
   * @brief Constructor
   *
   * @param[in] text  A Text object
   * @param[in] style A Style object
   */
  StyledText( const Text& text, const TextStyle& style )
  : mText( text ),
    mStyle( style )
  {
  }

  Text      mText;  ///< Store text. Could be a mix of different languages.
  TextStyle mStyle; ///< Store the style for the text.
};

/**
 * @brief This type defines a vector of StyledText.
 *
 * It's used to store a whole text with its style and set it to a
 * Dali::Toolkit::TextView. It could be used in other UI
 * Dali::Toolkit::Control classes which need text with style.
 */
typedef std::vector<StyledText> StyledTextArray;

/**
 * @brief Creates a text array with its style from a markup string.
 *
 * Refer to the \link markup-processor Markup Processor \endlink page in the Programming Guide
 * to see the html-ish sintax and some examples.
 *
 * @param [in] markupString A string with style.
 * @param [out] styledTextArray A text array split in characters, each one with its style.
 * @param [in] scanForMarkup If true will check to see string contains markup, else assume not
 */
void GetStyledTextArray( const std::string& markupString, StyledTextArray& styledTextArray, bool scanForMarkup );

/**
 * @brief Creates a plain string from a text array (thus stripping the style meta).
 *
 * @param [in] styledTextArray A text array split in characters, each one with its style.
 * @param [out] plainString A string without style.
 */
void GetPlainString( const StyledTextArray& styledTextArray, std::string& plainString );

/**
 * @brief Creates a markup string from a text array with its style.
 *
 * @param [in] styledTextArray A text array split in characters, each one with its style.
 * @param [out] markupString A string with style.
 */
void GetMarkupString( const StyledTextArray& styledTextArray, std::string& markupString );

/**
 * @brief Sets a text style to the given text.
 *
 * By default all style settings are applied but a bit mask could be used to modify only certain style settings.
 * @param[in,out] styledTextArray The given text
 * @param[in] style The given style
 * @param[in] mask The bit mask.
 */
void SetTextStyle( StyledTextArray& styledTextArray, const TextStyle& style, TextStyle::Mask mask = TextStyle::ALL );

/**
 * @brief Sets a text style to the given text.
 *
 * @see SetTextStyle( StyledTextArray& styledTextArray, const TextStyle& style, TextStyle::Mask mask )
 *
 * @param[in] text The input text.
 * @param[out] styledTextArray The input text with the given style.
 * @param[in] style The given style.
 * @param[in] mask The bit mask.
 */
void SetTextStyle( const Text& text, StyledTextArray& styledTextArray, const TextStyle& style, TextStyle::Mask mask = TextStyle::ALL );

/**
 * @brief Sets a text style to a range of characters of the given text.
 *
 * By default all style settings are applied but a bit mask could be used to modify only certain style settings.
 * @param[in,out] styledTextArray The given text
 * @param[in] style The given style
 * @param[in] mask The bit mask.
 * @param[in] begin The first character of the range.
 * @param[in] end The last character of the range.
 * @note It will assert if begin or end are out of range, or if begin > end.
 */
void SetTextStyleToRange( StyledTextArray& styledTextArray, const TextStyle& style, TextStyle::Mask mask, std::size_t begin, std::size_t end );

} // namespace MarkupProcessor

} // namespace Toolkit

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TOOLKIT_MARKUP_PROCESSOR_H__

#ifndef __DALI_TOOLKIT_INTERNAL_TEXT_FONT_STYLE_H__
#define __DALI_TOOLKIT_INTERNAL_TEXT_FONT_STYLE_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/scripting/scripting.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/text-controller.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

const Scripting::StringEnum FONT_WEIGHT_STRING_TABLE[] =
{
  { "thin", TextAbstraction::FontWeight::THIN },
  { "ultraLight", TextAbstraction::FontWeight::ULTRA_LIGHT },
  { "extraLight", TextAbstraction::FontWeight::EXTRA_LIGHT },
  { "light", TextAbstraction::FontWeight::LIGHT },
  { "demiLight", TextAbstraction::FontWeight::DEMI_LIGHT },
  { "semiLight", TextAbstraction::FontWeight::SEMI_LIGHT },
  { "book", TextAbstraction::FontWeight::BOOK },
  { "normal", TextAbstraction::FontWeight::NORMAL },
  { "regular", TextAbstraction::FontWeight::REGULAR },
  { "medium", TextAbstraction::FontWeight::MEDIUM },
  { "demiBold", TextAbstraction::FontWeight::DEMI_BOLD },
  { "semiBold", TextAbstraction::FontWeight::SEMI_BOLD },
  { "bold", TextAbstraction::FontWeight::BOLD },
  { "ultraBold", TextAbstraction::FontWeight::ULTRA_BOLD },
  { "extraBold", TextAbstraction::FontWeight::EXTRA_BOLD },
  { "black", TextAbstraction::FontWeight::BLACK },
  { "heavy", TextAbstraction::FontWeight::HEAVY },
  { "extraBlack", TextAbstraction::FontWeight::EXTRA_BLACK }
};
const unsigned int FONT_WEIGHT_STRING_TABLE_COUNT = sizeof( FONT_WEIGHT_STRING_TABLE ) / sizeof( FONT_WEIGHT_STRING_TABLE[0] );

const Scripting::StringEnum FONT_WIDTH_STRING_TABLE[] =
{
  { "ultraCondensed", TextAbstraction::FontWidth::ULTRA_CONDENSED },
  { "extraCondensed", TextAbstraction::FontWidth::EXTRA_CONDENSED },
  { "condensed", TextAbstraction::FontWidth::CONDENSED },
  { "semiCondensed", TextAbstraction::FontWidth::SEMI_CONDENSED },
  { "normal", TextAbstraction::FontWidth::NORMAL },
  { "semiExpanded", TextAbstraction::FontWidth::SEMI_EXPANDED },
  { "expanded", TextAbstraction::FontWidth::EXPANDED },
  { "extraExpanded", TextAbstraction::FontWidth::EXTRA_EXPANDED },
  { "ultraExpanded", TextAbstraction::FontWidth::ULTRA_EXPANDED },
};
const unsigned int FONT_WIDTH_STRING_TABLE_COUNT = sizeof( FONT_WIDTH_STRING_TABLE ) / sizeof( FONT_WIDTH_STRING_TABLE[0] );

const Scripting::StringEnum FONT_SLANT_STRING_TABLE[] =
{
  { "normal", TextAbstraction::FontSlant::NORMAL },
  { "roman", TextAbstraction::FontSlant::ROMAN },
  { "italic", TextAbstraction::FontSlant::ITALIC },
  { "oblique", TextAbstraction::FontSlant::OBLIQUE }
};
const unsigned int FONT_SLANT_STRING_TABLE_COUNT = sizeof( FONT_SLANT_STRING_TABLE ) / sizeof( FONT_SLANT_STRING_TABLE[0] );

namespace FontStyle
{
  enum Type
  {
    DEFAULT, ///< The default font's style.
    INPUT    ///< The input font's style.
  };
};

/**
 * @brief Sets the font family property.
 *
 * @param[in] controller The text's controller.
 * @param[in] value The value of the font's family.
 */
void SetFontFamilyProperty( ControllerPtr controller, const Property::Value& value );

/**
 * @brief Sets the font's style property.
 *
 * @param[in] controller The text's controller.
 * @param[in] value The value of the font's style.
 * @param[in] type Whether the property is for the default font's style or the input font's style.
 *
 */
void SetFontStyleProperty( ControllerPtr controller, const Property::Value& value, FontStyle::Type type );

/**
 * @brief Retrieves the font's style property.
 *
 * @param[in] controller The text's controller.
 * @param[out] value The value of the font's style.
 * @param[in] type Whether the property is for the default font's style or the input font's style.
 */
void GetFontStyleProperty( ControllerPtr controller, Property::Value& value, FontStyle::Type type );

/**
 * @brief Converts a weight string into @e FontWeight.
 *
 * @param[in] weightStr The weight string. Must end with '\0'.
 *
 * @return The @e FontWeight value corresponding to the string.
 */
FontWeight StringToWeight( const char* const weightStr );

/**
 * @brief Converts a width string into @e FontWidth.
 *
 * @param[in] widthStr The width string. Must end with '\0'.
 *
 * @return The @e FontWidth value corresponding to the string.
 */
FontWidth StringToWidth( const char* const widthStr );

/**
 * @brief Converts a slant string into @e FontSlant.
 *
 * @param[in] slantStr The slant string. Must end with '\0'.
 *
 * @return The @e FontSlant value corresponding to the string.
 */
FontSlant StringToSlant( const char* const slantStr );
} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TEXT_FONT_STYLE_H__

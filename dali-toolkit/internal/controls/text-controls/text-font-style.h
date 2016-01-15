#ifndef __DALI_TOOLKIT_INTERNAL_TEXT_FONT_STYLE_H__
#define __DALI_TOOLKIT_INTERNAL_TEXT_FONT_STYLE_H__

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
#include <dali/devel-api/scripting/scripting.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/text-controller.h>
#include <dali-toolkit/internal/text/text-definitions.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

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

const Scripting::StringEnum FONT_SLANT_STRING_TABLE[] =
{
  { "normal", TextAbstraction::FontSlant::NORMAL },
  { "roman", TextAbstraction::FontSlant::ROMAN },
  { "italic", TextAbstraction::FontSlant::ITALIC },
  { "oblique", TextAbstraction::FontSlant::OBLIQUE }
};
const unsigned int FONT_SLANT_STRING_TABLE_COUNT = sizeof( FONT_SLANT_STRING_TABLE ) / sizeof( FONT_SLANT_STRING_TABLE[0] );

/**
 * @brief Sets the font's style property.
 *
 * @param[in] controller The text's controller.
 * @param[in] value The value of the font's style.
 *
 */
void SetFontStyleProperty( ControllerPtr controller, const Property::Value& value );

/**
 * @brief Retrieves the font's style property.
 *
 * @param[in] controller The text's controller.
 * @param[out] value The value of the font's style.
 */
void GetFontStyleProperty( ControllerPtr controller, Property::Value& value );

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TEXT_FONT_STYLE_H__

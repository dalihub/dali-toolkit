#ifndef DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_FONT_H
#define DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_FONT_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

namespace Dali
{
namespace Toolkit
{
namespace Text
{
struct Tag;
struct Attribute;
struct FontDescriptionRun;

/**
 * @brief Retrieves the font attributes from the tag and sets it to the font run.
 *
 * @param[in] tag The font tag and its attributes.
 * @param[in,out] fontRun The font description run.
 */
void ProcessFontTag(const Tag& tag, FontDescriptionRun& fontRun);

/**
 * @brief Fill the font run with the font slant attribute value.
 *
 * @param[in] attribute the font slant attribute.
 * @param[out] fontRun The font description run.
 */
void ProcessFontSlant(const Attribute& attribute, FontDescriptionRun& fontRun);

/**
 * @brief Fill the font run with the font width attribute value.
 *
 * @param[in] attribute the font width attribute.
 * @param[out] fontRun The font description run.
 */
void ProcessFontWidth(const Attribute& attribute, FontDescriptionRun& fontRun);

/**
 * @brief Fill the font run with the font weight attribute value.
 *
 * @param[in] attribute the font weight attribute.
 * @param[out] fontRun The font description run.
 */
void ProcessFontWeight(const Attribute& attribute, FontDescriptionRun& fontRun);

/**
 * @brief Fill the font run with the font size attribute value.
 *
 * @param[in] attribute the font size attribute.
 * @param[out] fontRun The font description run.
 */
void ProcessFontSize(const Attribute& attribute, FontDescriptionRun& fontRun);

/**
 * @brief Fill the font run with the font family attribute value.
 *
 * @param[in] attribute the font family attribute.
 * @param[out] fontRun The font description run.
 */
void ProcessFontFamily(const Attribute& attribute, FontDescriptionRun& fontRun);

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_FONT_H

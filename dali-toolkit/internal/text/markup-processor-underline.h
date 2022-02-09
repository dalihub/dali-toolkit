#ifndef DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_UNDERLINE_H
#define DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_UNDERLINE_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
struct UnderlinedCharacterRun;

/**
 * @brief Fill the underlined character run with the type attribute value.
 *
 * @param[in] attribute the type attribute.
 * @param[out] underlinedCharacterRun The underlined character run
 */
void ProcessTypeAttribute(const Attribute& attribute, UnderlinedCharacterRun& underlinedCharacterRun);

/**
 * @brief Fill the underlined character run with the dash-gap attribute value.
 *
 * @param[in] attribute the dash-gap attribute.
 * @param[out] underlinedCharacterRun The underlined character run
 */
void ProcessDashGapAttribute(const Attribute& attribute, UnderlinedCharacterRun& underlinedCharacterRun);

/**
 * @brief Fill the underlined character run with the dash-width attribute value.
 *
 * @param[in] attribute the dash-width attribute.
 * @param[out] underlinedCharacterRun The underlined character run
 */
void ProcessDashWidthAttribute(const Attribute& attribute, UnderlinedCharacterRun& underlinedCharacterRun);

/**
 * @brief Fill the underlined character run with the height attribute value.
 *
 * @param[in] attribute the height attribute.
 * @param[out] underlinedCharacterRun The underlined character run
 */
void ProcessHeightAttribute(const Attribute& attribute, UnderlinedCharacterRun& underlinedCharacterRun);

/**
 * @brief Fill the underlined character run with the color attribute value.
 *
 * @param[in] attribute the color attribute.
 * @param[out] underlinedCharacterRun The underlined character run
 */
void ProcessColorAttribute(const Attribute& attribute, UnderlinedCharacterRun& underlinedCharacterRun);

/**
 * @brief Retrieves the underline run info from the tag and sets it to the underline run.
 *
 * @param[in] tag The underline tag and its attributes.
 * @param[in,out] underlinedCharacterRun The underlined character run
 */
void ProcessUnderlineTag(const Tag& tag, UnderlinedCharacterRun& underlinedCharacterRun);

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_UNDERLINE_H

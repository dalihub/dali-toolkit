#ifndef DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_CHARACTER_SPACING_H
#define DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_CHARACTER_SPACING_H

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
struct CharacterSpacingCharacterRun;

/**
 * @brief Fill the character-spacing character run with the value (space or advance) attribute.
 *
 * @param[in] attribute the value attribute.
 * @param[out] characterSpacingCharacterRun The underlined character run
 */
void ProcessValueAttribute(const Attribute& attribute, CharacterSpacingCharacterRun& characterSpacingCharacterRun);

/**
 * @brief Retrieves the character-spacing run info from the tag and sets it to the character-spacing run.
 *
 * @param[in] tag The character-spacing tag and its attributes.
 * @param[in,out] characterSpacingCharacterRun The character-spacing character run
 */
void ProcessCharacterSpacingTag(const Tag& tag, CharacterSpacingCharacterRun& characterSpacingCharacterRun);

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_CHARACTER_SPACING_H

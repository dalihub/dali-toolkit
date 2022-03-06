#ifndef DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_STRIKETHROUGH_H
#define DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_STRIKETHROUGH_H

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
struct StrikethroughCharacterRun;

/**
 * @brief Fill the strikethrough character run with the color attribute value.
 *
 * @param[in] attribute the color attribute.
 * @param[out] strikethroughCharacterRun The strikethrough character run
 */
void ProcessColorAttribute(const Attribute& attribute, StrikethroughCharacterRun& strikethroughCharacterRun);

/**
 * @brief Retrieves the strikethrough run info from the tag and sets it to the strikethrough run.
 *
 * @param[in] tag The strikethrough tag and its attributes.
 * @param[in,out] strikethroughRun The strikethrough run.
 */
void ProcessStrikethroughTag(const Tag& tag, StrikethroughCharacterRun& strikethroughRun);

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_STRIKETHROUGH_H

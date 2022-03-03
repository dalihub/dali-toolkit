#ifndef DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_PARAGRAPH_H
#define DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_PARAGRAPH_H

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
struct BoundedParagraphRun;

/**
 * @brief Retrieves the horizontal alignment value from the tag and sets it to the bounded paragraph run.
 *
 * @param[in] attribute the horizontal alignment attribute.
 * @param[in,out] boundedParagraphRun The bounded paragraph run.
 */
void ProcessHorizontalAlignment(const Attribute& attribute, BoundedParagraphRun& boundedParagraphRun);

/**
 * @brief Retrieves the paragraph value from the tag and sets it to the bounded paragraph run.
 *
 * @param[in] tag The paragraph tag and its attributes.
 * @param[in,out] boundedParagraphRun The bounded paragraph run.
 */
void ProcessAttributesOfParagraphTag(const Tag& tag, BoundedParagraphRun& boundedParagraphRun);

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_PARAGRAPH_H

#ifndef DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_SPAN_H
#define DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_SPAN_H

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
struct MarkupProcessData;

/**
 * @brief process the span from the tag and process all styles in it.
 *
 * @param[in] tag The span tag and its attributes.
 * @param[out] colorRun the color run to be filled.
 * @param[out] fontRun the font run to be filled.
 * @param[out] underlinedCharacterRun the underlined character run to be filled.
 * @param[out] backgroundColorRun the background color run to be filled.
 * @param[out] strikethroughRun the strikethrough run to be filled.
 * @param[out] isColorDefined if the span has color defined.
 * @param[out] isFontDefined if the span has font defined.
 * @param[out] isUnderlinedCharacterDefined if the span has underlined-character defined.
 * @param[out] isBackgroundColorDefined if the span has background color defined.
 * @param[out] isStrikethroughDefined if the span has strikethrough defined.
 */
void ProcessSpanTag(const Tag&                 tag,
                    ColorRun&                  colorRun,
                    FontDescriptionRun&        fontRun,
                    UnderlinedCharacterRun&    underlinedCharacterRun,
                    ColorRun&                  backgroundColorRun,
                    StrikethroughCharacterRun& strikethroughRun,
                    bool&                      isColorDefined,
                    bool&                      isFontDefined,
                    bool&                      isUnderlinedCharacterDefined,
                    bool&                      isBackgroundColorDefined,
                    bool&                      isStrikethroughDefined);

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_SPAN_H

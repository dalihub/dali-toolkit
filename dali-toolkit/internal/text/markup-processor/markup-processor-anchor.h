#ifndef DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_ANCHOR_H
#define DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_ANCHOR_H

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

#include <dali-toolkit/internal/text/color-run.h>
#include <dali-toolkit/internal/text/underlined-character-run.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

struct Tag;
struct Anchor;

/**
 * @brief Retrieves the @e anchor from the @p tag.
 *
 * @param[in] tag The anchor tag and its attributes.
 * @param[in,out] anchor The anchor.
 * @param[out] colorRun the color run to be filled.
 * @param[out] underlinedCharacterRun the underlined character run to be filled.
 */
void ProcessAnchorTag(const Tag&              tag,
                      Anchor&                 anchor,
                      ColorRun&               colorRun,
                      UnderlinedCharacterRun& underlinedCharacterRun);

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_MARKUP_PROCESSOR_ANCHOR_H
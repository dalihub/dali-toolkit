#ifndef DALI_TOOLKIT_TEXT_BOUNDED_PARAGRAPH_RUN_H
#define DALI_TOOLKIT_TEXT_BOUNDED_PARAGRAPH_RUN_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/math/vector2.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/character-run.h>
#include <dali-toolkit/public-api/text/text-enumerations.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * @brief BoundedParagraphRun
 *
 * In terms of the bidirectional algorithm, a 'paragraph' is understood as a run of characters between Paragraph Separators or appropriate Newline Functions.
 * A 'paragraph' may also be determined by higher-level protocols like a mark-up tag.
 * Bounded-paragraph is a pragraph that have been bounded by explicit tags like a mark-up tag.
 * Bounded-paragraph could contain multi paragraphs that have been breaked by Paragraph Separators or appropriate Newline Functions.
 * This will be used to handle information for the attributes of markup tag. Like TextAlign, TextDirection, TextIndent, LineHeight, etc.
 */
struct BoundedParagraphRun
{
  /**
   * Default constructor to set the default values of bitfields
   */
  BoundedParagraphRun()
  : characterRun{},
    horizontalAlignment(Text::HorizontalAlignment::BEGIN),
    relativeLineSize(1),
    horizontalAlignmentDefined{false},
    relativeLineSizeDefined(false)
  {
  }

  CharacterRun                    characterRun;                   ///< The initial character index within the whole text and the number of characters of the run.
  Text::HorizontalAlignment::Type horizontalAlignment;            ///< The paragraph horizontal alignment. Values "BEGIN" "CENTER" "END".
  float                           relativeLineSize;               ///< The relative line height to be used for this paragaraph.
  bool                            horizontalAlignmentDefined : 1; ///< Whether the horizontal alignment is defined.
  bool                            relativeLineSizeDefined : 1;    ///< Whether the relative line height is defined for this paragraph.
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_BOUNDED_PARAGRAPH_RUN_H

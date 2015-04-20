#ifndef __DALI_TOOLKIT_TEXT_BIDIRECTIONAL_PARAGRAPH_INFO_RUN_H__
#define __DALI_TOOLKIT_TEXT_BIDIRECTIONAL_PARAGRAPH_INFO_RUN_H__

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

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/character-run.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

/**
 * @brief BidirectionalParagraphInfoRun
 *
 * In terms of the bidirectional algorithm, a 'paragraph' is understood as a run of characters between Paragraph Separators or appropriate Newline Functions.
 * A 'paragraph' may also be determined by higher-level protocols like a mark-up tag.
 */
struct BidirectionalParagraphInfoRun
{
  CharacterRun  characterRun;           ///< The initial character index within the whole text and the number of characters of the run.
  BidiInfoIndex bidirectionalInfoIndex; ///< Index to the table with the bidirectional info per paragraph.
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_BIDIRECTIONAL_PARAGRAPH_INFO_RUN_H__

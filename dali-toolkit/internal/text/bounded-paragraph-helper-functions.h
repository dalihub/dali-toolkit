#ifndef DALI_TOOLKIT_TEXT_BOUNDED_PARAGRAPH_HELPER_FUNCTIONS_H
#define DALI_TOOLKIT_TEXT_BOUNDED_PARAGRAPH_HELPER_FUNCTIONS_H

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
#include <dali/public-api/common/dali-vector.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/bounded-paragraph-run.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * @brief Merge BoundedParagraphRun when the paragraph separator has been removed.
 * Adding the next characters until arrived to a paragraph separator.
 * Or merging with the next BoundedParagraphRun and removing the next BoundedParagraphRun.
 *
 * @param[in] text Vector of UTF-32 characters.
 * @param[in] index Index to the first character updated.
 * @param[in] numberOfCharacters The number of the removed characters.
 * @param[inout] boundedParagraphRuns The bounded paragraph runs.
 *
 */
void MergeBoundedParagraphRunsWhenRemoveCharacters(const Vector<Character>&     text,
                                                   const CharacterIndex&        index,
                                                   const int&                   numberOfCharacters,
                                                   Vector<BoundedParagraphRun>& boundedParagraphRuns);
} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_BOUNDED_PARAGRAPH_HELPER_FUNCTIONS_H

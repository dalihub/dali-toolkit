#ifndef DALI_TOOLKIT_TEXT_HYPHENATOR_H
#define DALI_TOOLKIT_TEXT_HYPHENATOR_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/font-run.h>
#include <dali-toolkit/internal/text/script-run.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * Gets a vector booleans that indicates possible hyphen locations.
 *
 * @param[in] word the word to get possible hyphens for.
 * @param[in] wordSize the word size in bytes.
 * @param[in] lang the language for the word
 *
 * @return vector of boolean, true if possible to hyphenate at this character position.
 */
Vector<bool> GetWordHyphens(const Character* word,
                            Length           wordSize,
                            const char*      lang);

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_HYPHENATOR_H

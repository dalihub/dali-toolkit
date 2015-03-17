#ifndef __DALI_TEXT_ABSTRACTION_TEXT_IO_H__
#define __DALI_TEXT_ABSTRACTION_TEXT_IO_H__

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

// EXTERNAL INCLUDES
#include <iosfwd>
#include <dali/public-api/common/dali-vector.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/font-run.h>
#include <dali-toolkit/internal/text/line-run.h>
#include <dali-toolkit/internal/text/script-run.h>
#include <dali-toolkit/internal/text/text-definitions.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

/**
 * @brief Print a vector of characters.
 *
 * @param [in] o The output stream operator.
 * @param [in] text The text to print.
 * @return The output stream operator.
 */
std::ostream& operator<< (std::ostream& o, const Vector<Character>& text);

/**
 * @brief Print a vector of script runs.
 *
 * @param [in] o The output stream operator.
 * @param [in] scriptRuns The script runs to print.
 * @return The output stream operator.
 */
std::ostream& operator<< (std::ostream& o, const Vector<ScriptRun>& scriptRuns);

/**
 * @brief Print a vector of font runs.
 *
 * @param [in] o The output stream operator.
 * @param [in] fontRuns The font runs to print.
 * @return The output stream operator.
 */
std::ostream& operator<< (std::ostream& o, const Vector<FontRun>& fontRuns);

/**
 * @brief Print a vector of line runs.
 *
 * @param [in] o The output stream operator.
 * @param [in] lineRuns The line runs to print.
 * @return The output stream operator.
 */
std::ostream& operator<< (std::ostream& o, const Vector<LineRun>& lineRuns);

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TEXT_ABSTRACTION_TEXT_IO_H__

#ifndef DALI_TOOLKIT_TEXT_LINE_HELPER_FUNCTIONS_H
#define DALI_TOOLKIT_TEXT_LINE_HELPER_FUNCTIONS_H

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

// INTERNAL INCLUDES

#include <dali-toolkit/devel-api/text/text-enumerations-devel.h>
#include <dali-toolkit/internal/text/line-run.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
  * @brief Retrieves the vertical offset to shift text inside line to up by negative value and to down by positive value.
  * The shifting depends on the vertical line alignment @p verLineAlign and lineSpacing when the lineSpacing is poistive.
  * When the lineSpacing is negative then ignore @p verLineAlign
  * @param[in] line the line.
  * @param[in] line the line.
  *
  * @return The vertical offset before text.
  */
float GetPreOffsetVerticalLineAlignment(const LineRun& line, const Dali::Toolkit::DevelText::VerticalLineAlignment::Type& verLineAlign);

/**
  * @brief Retrieves the vertical offset to shift the next line to up by negative value and to down by positive value.
  * The shifting depends on the vertical line alignment @p verLineAlign and lineSpacing when the lineSpacing is poistive.
  * When the lineSpacing is negative then ignore @p verLineAlign
  * @param[in] line the line.
  * @param[in] line the line.
  *
  * @return The vertical offset after text.
  */
float GetPostOffsetVerticalLineAlignment(const LineRun& line, const Dali::Toolkit::DevelText::VerticalLineAlignment::Type& verLineAlign);
} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_LINE_HELPER_FUNCTIONS_H

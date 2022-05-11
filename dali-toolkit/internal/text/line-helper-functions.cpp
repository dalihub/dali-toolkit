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

// FILE HEADER
#include <dali-toolkit/internal/text/line-helper-functions.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
float GetPreOffsetVerticalLineAlignment(const LineRun& line, const DevelText::VerticalLineAlignment::Type& verLineAlign)
{
  // Calculate vertical line alignment
  float offset = 0.0f;

  switch(verLineAlign)
  {
    case DevelText::VerticalLineAlignment::TOP:
    {
      break;
    }
    case DevelText::VerticalLineAlignment::MIDDLE:
    {
      offset = line.lineSpacing * 0.5f;
      break;
    }
    case DevelText::VerticalLineAlignment::BOTTOM:
    {
      offset = line.lineSpacing;
      break;
    }
  }

  // Apply TOP case when the lineSpacing is less than zero.
  offset = line.lineSpacing < 0.0f ? 0.0f : offset;

  return offset;
}

float GetPostOffsetVerticalLineAlignment(const LineRun& line, const DevelText::VerticalLineAlignment::Type& verLineAlign)
{
  // Calculate vertical line alignment
  float offset = 0.0f;

  switch(verLineAlign)
  {
    case DevelText::VerticalLineAlignment::TOP:
    {
      offset = line.lineSpacing;
      break;
    }
    case DevelText::VerticalLineAlignment::MIDDLE:
    {
      offset = line.lineSpacing * 0.5f;
      break;
    }
    case DevelText::VerticalLineAlignment::BOTTOM:
    {
      break;
    }
  }

  // Apply TOP case when the lineSpacing is less than zero.
  offset = line.lineSpacing < 0.0f ? line.lineSpacing : offset;

  return offset;
}
} // namespace Text

} // namespace Toolkit

} // namespace Dali

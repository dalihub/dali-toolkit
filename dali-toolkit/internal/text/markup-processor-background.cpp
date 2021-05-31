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
#include <dali-toolkit/internal/text/markup-processor-background.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>
#include <memory.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/color-run.h>
#include <dali-toolkit/internal/text/markup-processor-helper-functions.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace
{
const std::string XHTML_COLOR_ATTRIBUTE("color");
} // namespace

void ProcessBackground(const Tag& tag, ColorRun& colorRun)
{
  for(auto&& attribute : tag.attributes)
  {
    if(TokenComparison(XHTML_COLOR_ATTRIBUTE, attribute.nameBuffer, attribute.nameLength))
    {
      ColorStringToVector4(attribute.valueBuffer, attribute.valueLength, colorRun.color);
    }
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
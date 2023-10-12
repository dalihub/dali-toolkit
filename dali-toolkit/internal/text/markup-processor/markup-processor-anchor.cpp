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

// FILE HEADER
#include <dali-toolkit/internal/text/markup-processor/markup-processor-anchor.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>
#include <memory.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/anchor.h>
#include <dali-toolkit/internal/text/markup-processor/markup-processor-color.h>
#include <dali-toolkit/internal/text/markup-processor/markup-processor-underline.h>
#include <dali-toolkit/internal/text/markup-processor/markup-processor-helper-functions.h>
#include <dali-toolkit/internal/text/markup-tags-and-attributes.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{

void ProcessAnchorTag(const Tag&              tag,
                      Anchor&                 anchor,
                      ColorRun&               colorRun,
                      UnderlinedCharacterRun& underlinedCharacterRun)
{
  for(auto&& attribute : tag.attributes)
  {
    if(TokenComparison(MARKUP::ANCHOR_ATTRIBUTES::HREF, attribute.nameBuffer, attribute.nameLength))
    {
      Length hrefLength = attribute.valueLength + 1;
      anchor.href       = new char[hrefLength];
      memcpy(anchor.href, attribute.valueBuffer, hrefLength);
      anchor.href[hrefLength - 1] = '\0';
      // The memory is freed when the font run is removed from the logical model.
    }
    else if(TokenComparison(MARKUP::ANCHOR_ATTRIBUTES::COLOR, attribute.nameBuffer, attribute.nameLength))
    {
      ProcessColor(attribute, colorRun);
      ProcessColorAttribute(attribute, underlinedCharacterRun);
    }
    else if(TokenComparison(MARKUP::ANCHOR_ATTRIBUTES::CLICKED_COLOR, attribute.nameBuffer, attribute.nameLength))
    {
      ColorStringToVector4(attribute.valueBuffer, attribute.valueLength, anchor.clickedColor);
    }
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
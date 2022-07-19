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
#include <dali-toolkit/internal/text/markup-processor/markup-processor-embedded-item.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>
#include <memory.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/embedded-item.h>
#include <dali-toolkit/internal/text/markup-processor/markup-processor-helper-functions.h>
#include <dali-toolkit/internal/text/markup-tags-and-attributes.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace
{
const std::string NONE("none");
const std::string MULTIPLY("multiply");
} // namespace

void ProcessEmbeddedItem(const Tag& tag, EmbeddedItem& embeddedItem)
{
  embeddedItem.url               = nullptr;
  embeddedItem.urlLength         = 0u;
  embeddedItem.width             = 0u;
  embeddedItem.height            = 0u;
  embeddedItem.colorBlendingMode = ColorBlendingMode::NONE;

  for(Vector<Attribute>::ConstIterator it    = tag.attributes.Begin(),
                                       endIt = tag.attributes.End();
      it != endIt;
      ++it)
  {
    const Attribute& attribute(*it);
    if(TokenComparison(MARKUP::EMBEDDED_ITEM_ATTRIBUTES::URL, attribute.nameBuffer, attribute.nameLength))
    {
      embeddedItem.urlLength = attribute.valueLength;
      embeddedItem.url       = new char[embeddedItem.urlLength];
      memcpy(embeddedItem.url, attribute.valueBuffer, embeddedItem.urlLength);
      // The memory is freed when the font run is removed from the logical model.
    }
    else if(TokenComparison(MARKUP::EMBEDDED_ITEM_ATTRIBUTES::WIDTH, attribute.nameBuffer, attribute.nameLength))
    {
      embeddedItem.width = StringToUint(attribute.valueBuffer);
    }
    else if(TokenComparison(MARKUP::EMBEDDED_ITEM_ATTRIBUTES::HEIGHT, attribute.nameBuffer, attribute.nameLength))
    {
      embeddedItem.height = StringToUint(attribute.valueBuffer);
    }
    else if(TokenComparison(MARKUP::EMBEDDED_ITEM_ATTRIBUTES::COLOR_BLENDING, attribute.nameBuffer, attribute.nameLength))
    {
      if(TokenComparison(MULTIPLY, attribute.valueBuffer, attribute.valueLength))
      {
        embeddedItem.colorBlendingMode = ColorBlendingMode::MULTIPLY;
      }
    }
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

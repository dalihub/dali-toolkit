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
#include <dali-toolkit/internal/text/markup-processor/markup-processor-font.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>
#include <memory.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/font-description-run.h>
#include <dali-toolkit/internal/text/markup-processor/markup-processor-attribute-helper-functions.h>
#include <dali-toolkit/internal/text/markup-processor/markup-processor-helper-functions.h>
#include <dali-toolkit/internal/text/markup-tags-and-attributes.h>
#include <dali-toolkit/internal/text/text-font-style.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace
{
const std::string  FONT_PREFIX("font-");
const unsigned int FONT_PREFIX_LENGTH      = 5u;
const unsigned int MIN_FONT_ATTRIBUTE_SIZE = 4u;   ///< The minimum length of any of the possible 'weight', 'width' , 'slant' or 'size' values.
const unsigned int MAX_FONT_ATTRIBUTE_SIZE = 15u;  ///< The maximum length of any of the possible 'weight', 'width' or 'slant' values.
const float        PIXEL_FORMAT_64_FACTOR  = 64.f; ///< 64.f is used to convert from point size to 26.6 pixel format.
} // namespace

void processFontAttributeValue(char value[], const Attribute& attribute)
{
  // The StringToWeight() uses the Scripting::GetEnumeration() function which requires the input string to end with a '\0' char.
  const Length length = attribute.valueLength > MAX_FONT_ATTRIBUTE_SIZE ? MAX_FONT_ATTRIBUTE_SIZE : attribute.valueLength;
  memcpy(value, attribute.valueBuffer, length);
  value[length] = 0;
}

void ProcessFontFamily(const Attribute& attribute, FontDescriptionRun& fontRun)
{
  fontRun.familyDefined = true;
  fontRun.familyLength  = attribute.valueLength;
  fontRun.familyName    = new char[fontRun.familyLength];
  memcpy(fontRun.familyName, attribute.valueBuffer, fontRun.familyLength);
  // The memory is freed when the font run is removed from the logical model.
}

void ProcessFontSize(const Attribute& attribute, FontDescriptionRun& fontRun)
{
  // 64.f is used to convert from point size to 26.6 pixel format.
  fontRun.size        = static_cast<PointSize26Dot6>(ProcessFloatAttribute(attribute) * PIXEL_FORMAT_64_FACTOR);
  fontRun.sizeDefined = true;
}

void ProcessFontWeight(const Attribute& attribute, FontDescriptionRun& fontRun)
{
  fontRun.weightDefined = ProcessEnumerationAttribute<FontWeight>(attribute, MAX_FONT_ATTRIBUTE_SIZE, &StringToWeight, fontRun.weight);
}

void ProcessFontWidth(const Attribute& attribute, FontDescriptionRun& fontRun)
{
  fontRun.widthDefined = ProcessEnumerationAttribute<FontWidth>(attribute, MAX_FONT_ATTRIBUTE_SIZE, &StringToWidth, fontRun.width);
}

void ProcessFontSlant(const Attribute& attribute, FontDescriptionRun& fontRun)
{
  fontRun.slantDefined = ProcessEnumerationAttribute<FontSlant>(attribute, MAX_FONT_ATTRIBUTE_SIZE, &StringToSlant, fontRun.slant);
}

void ProcessFontTag(const Tag& tag, FontDescriptionRun& fontRun)
{
  for(Vector<Attribute>::ConstIterator it    = tag.attributes.Begin(),
                                       endIt = tag.attributes.End();
      it != endIt;
      ++it)
  {
    const Attribute& attribute(*it);

    if(TokenComparison(MARKUP::FONT_ATTRIBUTES::FAMILY, attribute.nameBuffer, attribute.nameLength))
    {
      ProcessFontFamily(attribute, fontRun);
    }
    else if(TokenComparison(MARKUP::FONT_ATTRIBUTES::SIZE, attribute.nameBuffer, attribute.nameLength))
    {
      ProcessFontSize(attribute, fontRun);
    }
    else if(TokenComparison(MARKUP::FONT_ATTRIBUTES::WEIGHT, attribute.nameBuffer, attribute.nameLength))
    {
      ProcessFontWeight(attribute, fontRun);
    }
    else if(TokenComparison(MARKUP::FONT_ATTRIBUTES::WIDTH, attribute.nameBuffer, attribute.nameLength))
    {
      ProcessFontWidth(attribute, fontRun);
    }
    else if(TokenComparison(MARKUP::FONT_ATTRIBUTES::SLANT, attribute.nameBuffer, attribute.nameLength))
    {
      ProcessFontSlant(attribute, fontRun);
    }
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

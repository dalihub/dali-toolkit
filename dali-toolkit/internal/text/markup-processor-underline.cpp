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
#include <dali-toolkit/internal/text/markup-processor-underline.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/markup-processor-attribute-helper-functions.h>
#include <dali-toolkit/internal/text/markup-processor-helper-functions.h>
#include <dali-toolkit/internal/text/text-effects-style.h>
#include <dali-toolkit/internal/text/underlined-character-run.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace
{
const std::string XHTML_COLOR_ATTRIBUTE("color");
const std::string XHTML_HEIGHT_ATTRIBUTE("height");
const std::string XHTML_TYPE_ATTRIBUTE("type");
const std::string XHTML_DASH_GAP_ATTRIBUTE("dash-gap");
const std::string XHTML_DASH_WIDTH_ATTRIBUTE("dash-width");

const unsigned int MAX_TYPE_ATTRIBUTE_SIZE = 7u; ///< The maximum length of any of the possible 'type' values.

} // namespace

void ProcessTypeAttribute(const Attribute& attribute, UnderlinedCharacterRun& underlinedCharacterRun)
{
  underlinedCharacterRun.properties.typeDefined = ProcessEnumerationAttribute<Text::Underline::Type>(attribute,
                                                                                                     MAX_TYPE_ATTRIBUTE_SIZE,
                                                                                                     &StringToUnderlineType,
                                                                                                     underlinedCharacterRun.properties.type);
}

void ProcessDashGapAttribute(const Attribute& attribute, UnderlinedCharacterRun& underlinedCharacterRun)
{
  underlinedCharacterRun.properties.dashGap        = ProcessFloatAttribute(attribute);
  underlinedCharacterRun.properties.dashGapDefined = true;
}

void ProcessDashWidthAttribute(const Attribute& attribute, UnderlinedCharacterRun& underlinedCharacterRun)
{
  underlinedCharacterRun.properties.dashWidth        = ProcessFloatAttribute(attribute);
  underlinedCharacterRun.properties.dashWidthDefined = true;
}
void ProcessHeightAttribute(const Attribute& attribute, UnderlinedCharacterRun& underlinedCharacterRun)
{
  underlinedCharacterRun.properties.height        = ProcessFloatAttribute(attribute);
  underlinedCharacterRun.properties.heightDefined = true;
}

void ProcessColorAttribute(const Attribute& attribute, UnderlinedCharacterRun& underlinedCharacterRun)
{
  ColorStringToVector4(attribute.valueBuffer, attribute.valueLength, underlinedCharacterRun.properties.color);
  underlinedCharacterRun.properties.colorDefined = true;
}

void ProcessUnderlineTag(const Tag& tag, UnderlinedCharacterRun& underlinedCharacterRun)
{
  for(Vector<Attribute>::ConstIterator it    = tag.attributes.Begin(),
                                       endIt = tag.attributes.End();
      it != endIt;
      ++it)
  {
    const Attribute& attribute(*it);

    if(TokenComparison(XHTML_COLOR_ATTRIBUTE, attribute.nameBuffer, attribute.nameLength))
    {
      ProcessColorAttribute(attribute, underlinedCharacterRun);
    }
    else if(TokenComparison(XHTML_HEIGHT_ATTRIBUTE, attribute.nameBuffer, attribute.nameLength))
    {
      ProcessHeightAttribute(attribute, underlinedCharacterRun);
    }
    else if(TokenComparison(XHTML_TYPE_ATTRIBUTE, attribute.nameBuffer, attribute.nameLength))
    {
      ProcessTypeAttribute(attribute, underlinedCharacterRun);
    }
    else if(TokenComparison(XHTML_DASH_GAP_ATTRIBUTE, attribute.nameBuffer, attribute.nameLength))
    {
      ProcessDashGapAttribute(attribute, underlinedCharacterRun);
    }
    else if(TokenComparison(XHTML_DASH_WIDTH_ATTRIBUTE, attribute.nameBuffer, attribute.nameLength))
    {
      ProcessDashWidthAttribute(attribute, underlinedCharacterRun);
    }
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

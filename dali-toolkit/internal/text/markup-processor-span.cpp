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

// FILE HEADER
#include <dali-toolkit/internal/text/markup-processor-color.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/color-run.h>
#include <dali-toolkit/internal/text/font-description-run.h>
#include <dali-toolkit/internal/text/markup-processor-font.h>
#include <dali-toolkit/internal/text/markup-processor-helper-functions.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace
{
const std::string XHTML_FAMILY_ATTRIBUTE("font-family");
const std::string XHTML_SIZE_ATTRIBUTE("font-size");
const std::string XHTML_WEIGHT_ATTRIBUTE("font-weight");
const std::string XHTML_WIDTH_ATTRIBUTE("font-width");
const std::string XHTML_SLANT_ATTRIBUTE("font-slant");

const std::string XHTML_COLOR_ATTRIBUTE("text-color");
} // namespace

void ProcessSpanTag(const Tag& tag, ColorRun& colorRun, FontDescriptionRun& fontRun, bool& isColorDefined, bool& isFontDefined)
{
  for(Vector<Attribute>::ConstIterator it    = tag.attributes.Begin(),
                                       endIt = tag.attributes.End();
      it != endIt;
      ++it)
  {
    const Attribute& attribute(*it);

    if(TokenComparison(XHTML_COLOR_ATTRIBUTE, attribute.nameBuffer, attribute.nameLength))
    {
      isColorDefined = true;
      ProcessColor(attribute, colorRun);
    }
    else if(TokenComparison(XHTML_FAMILY_ATTRIBUTE, attribute.nameBuffer, attribute.nameLength))
    {
      isFontDefined = true;
      ProcessFontFamily(attribute, fontRun);
    }
    else if(TokenComparison(XHTML_SIZE_ATTRIBUTE, attribute.nameBuffer, attribute.nameLength))
    {
      isFontDefined = true;
      ProcessFontSize(attribute, fontRun);
    }
    else if(TokenComparison(XHTML_WEIGHT_ATTRIBUTE, attribute.nameBuffer, attribute.nameLength))
    {
      isFontDefined = true;
      ProcessFontWeight(attribute, fontRun);
    }
    else if(TokenComparison(XHTML_WIDTH_ATTRIBUTE, attribute.nameBuffer, attribute.nameLength))
    {
      isFontDefined = true;
      ProcessFontWidth(attribute, fontRun);
    }
    else if(TokenComparison(XHTML_SLANT_ATTRIBUTE, attribute.nameBuffer, attribute.nameLength))
    {
      isFontDefined = true;
      ProcessFontSlant(attribute, fontRun);
    }
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

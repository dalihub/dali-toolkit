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
#include <dali-toolkit/internal/text/markup-processor-paragraph.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/bounded-paragraph-run.h>
#include <dali-toolkit/internal/text/markup-processor-helper-functions.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace
{
const std::string XHTML_ALIGN_ATTRIBUTE("align");
const std::string XHTML_RELATIVE_LINE_HEIGHT_ATTRIBUTE("rel-line-height");
} // namespace

void ProcessHorizontalAlignment(const Attribute& attribute, BoundedParagraphRun& boundedParagraphRun)
{
  boundedParagraphRun.horizontalAlignmentDefined = HorizontalAlignmentTypeStringToTypeValue(attribute.valueBuffer,
                                                                                            attribute.valueLength,
                                                                                            boundedParagraphRun.horizontalAlignment);
}

void ProcessRelativeLineHeight(const Attribute& attribute, BoundedParagraphRun& boundedParagraphRun)
{
  boundedParagraphRun.relativeLineSize        = StringToFloat(attribute.valueBuffer);
  boundedParagraphRun.relativeLineSizeDefined = true;
}

void ProcessAttributesOfParagraphTag(const Tag& tag, BoundedParagraphRun& boundedParagraphRun)
{
  // By default the align attribute is not defined until it's parsed.
  boundedParagraphRun.horizontalAlignmentDefined = false;

  for(Vector<Attribute>::ConstIterator it    = tag.attributes.Begin(),
                                       endIt = tag.attributes.End();
      it != endIt;
      ++it)
  {
    const Attribute& attribute(*it);
    if(TokenComparison(XHTML_ALIGN_ATTRIBUTE, attribute.nameBuffer, attribute.nameLength))
    {
      ProcessHorizontalAlignment(attribute, boundedParagraphRun);
    }
    else if(TokenComparison(XHTML_RELATIVE_LINE_HEIGHT_ATTRIBUTE, attribute.nameBuffer, attribute.nameLength))
    {
      ProcessRelativeLineHeight(attribute, boundedParagraphRun);
    }
  }
}
} // namespace Text

} // namespace Toolkit

} // namespace Dali

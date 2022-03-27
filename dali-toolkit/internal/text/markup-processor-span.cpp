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
#include <dali-toolkit/internal/text/markup-processor-character-spacing.h>
#include <dali-toolkit/internal/text/markup-processor-font.h>
#include <dali-toolkit/internal/text/markup-processor-helper-functions.h>
#include <dali-toolkit/internal/text/markup-processor-strikethrough.h>
#include <dali-toolkit/internal/text/markup-processor-underline.h>
#include <dali-toolkit/internal/text/markup-tags-and-attributes.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
void ProcessSpanTag(const Tag&                    tag,
                    ColorRun&                     colorRun,
                    FontDescriptionRun&           fontRun,
                    UnderlinedCharacterRun&       underlinedCharacterRun,
                    ColorRun&                     backgroundColorRun,
                    StrikethroughCharacterRun&    strikethroughRun,
                    CharacterSpacingCharacterRun& characterSpacingCharacterRun,
                    bool&                         isColorDefined,
                    bool&                         isFontDefined,
                    bool&                         isUnderlinedCharacterDefined,
                    bool&                         isBackgroundColorDefined,
                    bool&                         isStrikethroughDefined,
                    bool&                         isCharacterSpacingDefined)
{
  for(Vector<Attribute>::ConstIterator it    = tag.attributes.Begin(),
                                       endIt = tag.attributes.End();
      it != endIt;
      ++it)
  {
    const Attribute& attribute(*it);

    if(TokenComparison(MARKUP::SPAN_ATTRIBUTES::TEXT_COLOR, attribute.nameBuffer, attribute.nameLength))
    {
      isColorDefined = true;
      ProcessColor(attribute, colorRun);
    }
    else if(TokenComparison(MARKUP::SPAN_ATTRIBUTES::BACKGROUND_COLOR, attribute.nameBuffer, attribute.nameLength))
    {
      isBackgroundColorDefined = true;
      ProcessColor(attribute, backgroundColorRun);
    }
    else if(TokenComparison(MARKUP::SPAN_ATTRIBUTES::FONT_FAMILY, attribute.nameBuffer, attribute.nameLength))
    {
      isFontDefined = true;
      ProcessFontFamily(attribute, fontRun);
    }
    else if(TokenComparison(MARKUP::SPAN_ATTRIBUTES::FONT_SIZE, attribute.nameBuffer, attribute.nameLength))
    {
      isFontDefined = true;
      ProcessFontSize(attribute, fontRun);
    }
    else if(TokenComparison(MARKUP::SPAN_ATTRIBUTES::FONT_WEIGHT, attribute.nameBuffer, attribute.nameLength))
    {
      isFontDefined = true;
      ProcessFontWeight(attribute, fontRun);
    }
    else if(TokenComparison(MARKUP::SPAN_ATTRIBUTES::FONT_WIDTH, attribute.nameBuffer, attribute.nameLength))
    {
      isFontDefined = true;
      ProcessFontWidth(attribute, fontRun);
    }
    else if(TokenComparison(MARKUP::SPAN_ATTRIBUTES::FONT_SLANT, attribute.nameBuffer, attribute.nameLength))
    {
      isFontDefined = true;
      ProcessFontSlant(attribute, fontRun);
    }
    else if(TokenComparison(MARKUP::SPAN_ATTRIBUTES::UNDERLINE_COLOR, attribute.nameBuffer, attribute.nameLength))
    {
      isUnderlinedCharacterDefined = true;
      ProcessColorAttribute(attribute, underlinedCharacterRun);
    }
    else if(TokenComparison(MARKUP::SPAN_ATTRIBUTES::UNDERLINE_HEIGHT, attribute.nameBuffer, attribute.nameLength))
    {
      isUnderlinedCharacterDefined = true;
      ProcessHeightAttribute(attribute, underlinedCharacterRun);
    }
    else if(TokenComparison(MARKUP::SPAN_ATTRIBUTES::UNDERLINE_TYPE, attribute.nameBuffer, attribute.nameLength))
    {
      isUnderlinedCharacterDefined = true;
      ProcessTypeAttribute(attribute, underlinedCharacterRun);
    }
    else if(TokenComparison(MARKUP::SPAN_ATTRIBUTES::UNDERLINE_DASH_GAP, attribute.nameBuffer, attribute.nameLength))
    {
      isUnderlinedCharacterDefined = true;
      ProcessDashGapAttribute(attribute, underlinedCharacterRun);
    }
    else if(TokenComparison(MARKUP::SPAN_ATTRIBUTES::UNDERLINE_DASH_WIDTH, attribute.nameBuffer, attribute.nameLength))
    {
      isUnderlinedCharacterDefined = true;
      ProcessDashWidthAttribute(attribute, underlinedCharacterRun);
    }
    else if(TokenComparison(MARKUP::SPAN_ATTRIBUTES::STRIKETHROUGH_COLOR, attribute.nameBuffer, attribute.nameLength))
    {
      isStrikethroughDefined = true;
      ProcessColorAttribute(attribute, strikethroughRun);
    }
    else if(TokenComparison(MARKUP::SPAN_ATTRIBUTES::STRIKETHROUGH_HEIGHT, attribute.nameBuffer, attribute.nameLength))
    {
      isStrikethroughDefined = true;
      ProcessHeightAttribute(attribute, strikethroughRun);
    }
    else if(TokenComparison(MARKUP::SPAN_ATTRIBUTES::CHARACTER_SPACING_VALUE, attribute.nameBuffer, attribute.nameLength))
    {
      isCharacterSpacingDefined = true;
      ProcessValueAttribute(attribute, characterSpacingCharacterRun);
    }
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

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

// CLASS HEADER
#include <dali-toolkit/internal/text/rendering/styles/underline-helper-functions.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
bool IsGlyphUnderlined(GlyphIndex                                 index,
                       const Vector<UnderlinedGlyphRun>&          underlineRuns,
                       Vector<UnderlinedGlyphRun>::ConstIterator& currentUnderlinedGlyphRunIt)
{
  for(Vector<UnderlinedGlyphRun>::ConstIterator it    = underlineRuns.Begin(),
                                                endIt = underlineRuns.End();
      it != endIt;
      ++it)
  {
    const UnderlinedGlyphRun& run = *it;

    if((run.glyphRun.glyphIndex <= index) && (index < run.glyphRun.glyphIndex + run.glyphRun.numberOfGlyphs))
    {
      currentUnderlinedGlyphRunIt = it;
      return true;
    }
  }

  return false;
}

float GetCurrentUnderlineHeight(const Vector<UnderlinedGlyphRun>&         underlineRuns,
                                Vector<UnderlinedGlyphRun>::ConstIterator currentUnderlinedGlyphRunIt,
                                const float                               underlineHeight)
{
  if(currentUnderlinedGlyphRunIt == underlineRuns.End())
  {
    return underlineHeight;
  }

  const UnderlinedGlyphRun& underlinedGlyphRun = *currentUnderlinedGlyphRunIt;
  return (underlinedGlyphRun.properties.heightDefined ? underlinedGlyphRun.properties.height : underlineHeight);
}

UnderlineStyleProperties GetCurrentUnderlineProperties(const bool&                               isGlyphUnderlined,
                                                       const Vector<UnderlinedGlyphRun>&         underlineRuns,
                                                       Vector<UnderlinedGlyphRun>::ConstIterator currentUnderlinedGlyphRunIt,
                                                       const UnderlineStyleProperties&           commonUnderlineProperties)
{
  return (isGlyphUnderlined && (currentUnderlinedGlyphRunIt != underlineRuns.End()))
           ? currentUnderlinedGlyphRunIt->properties
           : commonUnderlineProperties;
}

float FetchUnderlinePositionFromFontMetrics(const FontMetrics& fontMetrics)
{
  //Helper method to fetch the underline metrics for the specified font glyph
  const float descender         = ceil(fabsf(fontMetrics.descender));
  float       underlinePosition = ceil(fabsf(fontMetrics.underlinePosition));

  // Clamp the underline position at the font descender and check for ( as EFL describes it ) a broken font
  if(underlinePosition > descender)
  {
    underlinePosition = descender;
  }

  if(fabsf(underlinePosition) < Math::MACHINE_EPSILON_1000)
  {
    // Move offset down by one ( EFL behavior )
    underlinePosition = 1.0f;
  }

  return underlinePosition;
}

void CalcualteUnderlineHeight(const FontMetrics& fontMetrics, float& currentUnderlineHeight, float& maxUnderlineHeight)
{
  //Helper method to fetch the underline metrics for the specified font glyph
  //Height of underline represents the thickness of line.
  if(fabsf(currentUnderlineHeight) < Math::MACHINE_EPSILON_1000)
  {
    currentUnderlineHeight = fontMetrics.underlineThickness;

    // Ensure underline will be at least a pixel high
    if(currentUnderlineHeight < 1.0f)
    {
      currentUnderlineHeight = 1.0f;
    }
    else
    {
      currentUnderlineHeight = ceil(currentUnderlineHeight);
    }
  }

  // The underline height should be the max underline height of all glyphs of the line.
  if(currentUnderlineHeight > maxUnderlineHeight)
  {
    maxUnderlineHeight = currentUnderlineHeight;
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
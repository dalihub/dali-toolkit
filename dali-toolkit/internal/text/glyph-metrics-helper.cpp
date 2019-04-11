
/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/glyph-metrics-helper.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

Length GetNumberOfGlyphsOfGroup( GlyphIndex glyphIndex,
                                 GlyphIndex lastGlyphPlusOne,
                                 const Length* const charactersPerGlyphBuffer )
{
  Length numberOfGLyphsInGroup = 1u;

  for( GlyphIndex index = glyphIndex; index < lastGlyphPlusOne; ++index )
  {
    if( 0u == *( charactersPerGlyphBuffer + index ) )
    {
      ++numberOfGLyphsInGroup;
    }
    else
    {
      break;
    }
  }

  return numberOfGLyphsInGroup;
}

void GetGlyphsMetrics( GlyphIndex glyphIndex,
                       Length numberOfGlyphs,
                       GlyphMetrics& glyphMetrics,
                       const GlyphInfo* const glyphsBuffer,
                       MetricsPtr& metrics )
{
  const GlyphInfo& firstGlyph = *( glyphsBuffer + glyphIndex );

  Text::FontMetrics fontMetrics;
  if( 0u != firstGlyph.fontId )
  {
    metrics->GetFontMetrics( firstGlyph.fontId, fontMetrics );
  }
  else if( 0u != firstGlyph.index )
  {
    // It may be an embedded image.
    fontMetrics.ascender = firstGlyph.height;
    fontMetrics.descender = 0.f;
    fontMetrics.height = fontMetrics.ascender;
  }

  const bool isItalicFont = metrics->HasItalicStyle( firstGlyph.fontId );

  glyphMetrics.fontId = firstGlyph.fontId;
  glyphMetrics.fontHeight = fontMetrics.height;
  glyphMetrics.width = firstGlyph.width + ( ( firstGlyph.isItalicRequired && !isItalicFont ) ? static_cast<unsigned int>( TextAbstraction::FontClient::DEFAULT_ITALIC_ANGLE * static_cast<float>( firstGlyph.height ) ) : 0u );
  glyphMetrics.advance = firstGlyph.advance;
  glyphMetrics.ascender = fontMetrics.ascender;
  glyphMetrics.xBearing = firstGlyph.xBearing;

  if( 1u < numberOfGlyphs )
  {
    const float widthInit = firstGlyph.xBearing;

    for( unsigned int i = 1u; i < numberOfGlyphs; ++i )
    {
      const GlyphInfo& glyphInfo = *( glyphsBuffer + glyphIndex + i );

      glyphMetrics.width = glyphMetrics.advance + glyphInfo.xBearing + glyphInfo.width + ( ( firstGlyph.isItalicRequired && !isItalicFont ) ? static_cast<unsigned int>( TextAbstraction::FontClient::DEFAULT_ITALIC_ANGLE * static_cast<float>( firstGlyph.height ) ) : 0u );
      glyphMetrics.advance += glyphInfo.advance;

    }

    glyphMetrics.width -= widthInit;
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

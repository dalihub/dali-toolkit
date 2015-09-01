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

// CLASS HEADER
#include <dali-toolkit/internal/text/text-view.h>

// EXTERNAL INCLUDES
#include <dali/public-api/math/vector2.h>
#include <dali/devel-api/text-abstraction/font-client.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

struct View::Impl
{
  VisualModelPtr mVisualModel;
  TextAbstraction::FontClient mFontClient; ///< Handle to the font client.
};

View::View()
: mImpl( NULL )
{
  mImpl = new View::Impl();

  mImpl->mFontClient = TextAbstraction::FontClient::Get();
}

View::~View()
{
  delete mImpl;
}

void View::SetVisualModel( VisualModelPtr visualModel )
{
  mImpl->mVisualModel = visualModel;
}

const Vector2& View::GetControlSize() const
{
  if ( mImpl->mVisualModel )
  {
    return mImpl->mVisualModel->mControlSize;
  }

  return Vector2::ZERO;
}

Length View::GetNumberOfGlyphs() const
{
  if( mImpl->mVisualModel )
  {
    const VisualModel& model = *mImpl->mVisualModel;

    const Length glyphCount = model.mGlyphs.Count();
    const Length positionCount = model.mGlyphPositions.Count();

    DALI_ASSERT_DEBUG( positionCount <= glyphCount && "Invalid glyph positions in Model" );

    return (positionCount < glyphCount) ? positionCount : glyphCount;
  }

  return 0;
}

Length View::GetGlyphs( GlyphInfo* glyphs,
                        Vector2* glyphPositions,
                        GlyphIndex glyphIndex,
                        Length numberOfGlyphs ) const
{
  Length numberOfLaidOutGlyphs = 0u;

  if( mImpl->mVisualModel )
  {
    // If ellipsis is enabled, the number of glyphs the layout engine has laid out may be less than 'numberOfGlyphs'.
    // Check the last laid out line to know if the layout engine elided some text.

    const Length numberOfLines = mImpl->mVisualModel->mLines.Count();
    if( numberOfLines > 0u )
    {
      const LineRun& lastLine = *( mImpl->mVisualModel->mLines.Begin() + ( numberOfLines - 1u ) );

      // If ellipsis is enabled, calculate the number of laid out glyphs.
      // Otherwise use the given number of glyphs.
      if( lastLine.ellipsis )
      {
        numberOfLaidOutGlyphs = lastLine.glyphRun.glyphIndex + lastLine.glyphRun.numberOfGlyphs;
      }
      else
      {
        numberOfLaidOutGlyphs = numberOfGlyphs;
      }

      // Retrieve from the visual model the glyphs and positions.
      mImpl->mVisualModel->GetGlyphs( glyphs,
                                      glyphIndex,
                                      numberOfLaidOutGlyphs );

      mImpl->mVisualModel->GetGlyphPositions( glyphPositions,
                                              glyphIndex,
                                              numberOfLaidOutGlyphs );

      // Get the lines for the given range of glyphs.
      // The lines contain the alignment offset which needs to be added to the glyph's position.
      LineIndex firstLine = 0u;
      Length numberOfLines = 0u;
      mImpl->mVisualModel->GetNumberOfLines( glyphIndex,
                                             numberOfLaidOutGlyphs,
                                             firstLine,
                                             numberOfLines );

      Vector<LineRun> lines;
      lines.Resize( numberOfLines );
      LineRun* lineBuffer = lines.Begin();

      mImpl->mVisualModel->GetLinesOfGlyphRange( lineBuffer,
                                                 glyphIndex,
                                                 numberOfLaidOutGlyphs );

      // Get the first line for the given glyph range.
      LineIndex lineIndex = firstLine;
      LineRun* line = lineBuffer + lineIndex;

      // Index of the last glyph of the line.
      GlyphIndex lastGlyphIndexOfLine = line->glyphRun.glyphIndex + line->glyphRun.numberOfGlyphs - 1u;

      // Add the alignment offset to the glyph's position.
      for( Length index = 0u; index < numberOfLaidOutGlyphs; ++index )
      {
        ( *( glyphPositions + index ) ).x += line->alignmentOffset;

        if( lastGlyphIndexOfLine == index )
        {
          // Get the next line.
          ++lineIndex;

          if( lineIndex < numberOfLines )
          {
            line = lineBuffer + lineIndex;
            lastGlyphIndexOfLine = line->glyphRun.glyphIndex + line->glyphRun.numberOfGlyphs - 1u;
          }
        }
      }

      if( 1u == numberOfLaidOutGlyphs )
      {
        // not a point try to do ellipsis with only one laid out character.
        return numberOfLaidOutGlyphs;
      }

      if( lastLine.ellipsis )
      {
        // firstPenX, penY and firstPenSet are used to position the ellipsis glyph if needed.
        float firstPenX = 0.f; // Used if rtl text is elided.
        float penY = 0.f;
        bool firstPenSet = false;

        // Add the ellipsis glyph.
        bool inserted = false;
        float removedGlypsWidth = 0.f;
        Length numberOfRemovedGlyphs = 0u;
        GlyphIndex index = numberOfLaidOutGlyphs - 1u;

        // The ellipsis glyph has to fit in the place where the last glyph(s) is(are) removed.
        while( !inserted )
        {
          const GlyphInfo& glyphToRemove = *( glyphs + index );

          if( 0u != glyphToRemove.fontId )
          {
            // i.e. The font id of the glyph shaped from the '\n' character is zero.

            // Need to reshape the glyph as the font may be different in size.
            const GlyphInfo& ellipsisGlyph = mImpl->mFontClient.GetEllipsisGlyph( mImpl->mFontClient.GetPointSize( glyphToRemove.fontId ) );

            if( !firstPenSet )
            {
              const Vector2& position = *( glyphPositions + index );

              // Calculates the penY of the current line. It will be used to position the ellipsis glyph.
              penY = position.y + glyphToRemove.yBearing;

              // Calculates the first penX which will be used if rtl text is elided.
              firstPenX = position.x - glyphToRemove.xBearing;
              if( firstPenX < -ellipsisGlyph.xBearing )
              {
                // Avoids to exceed the bounding box when rtl text is elided.
                firstPenX = -ellipsisGlyph.xBearing;
              }

              removedGlypsWidth = -ellipsisGlyph.xBearing;

              firstPenSet = true;
            }

            removedGlypsWidth += std::min( glyphToRemove.advance, ( glyphToRemove.xBearing + glyphToRemove.width ) );

            // Calculate the width of the ellipsis glyph and check if it fits.
            const float ellipsisGlyphWidth = ellipsisGlyph.width + ellipsisGlyph.xBearing;
            if( ellipsisGlyphWidth < removedGlypsWidth )
            {
              GlyphInfo& glyphInfo = *( glyphs + index );
              Vector2& position = *( glyphPositions + index );
              position.x -= ( 0.f > glyphInfo.xBearing ) ? glyphInfo.xBearing : 0.f;

              // Replace the glyph by the ellipsis glyph.
              glyphInfo = ellipsisGlyph;

              // Change the 'x' and 'y' position of the ellipsis glyph.

              if( position.x > firstPenX )
              {
                position.x = firstPenX + removedGlypsWidth - ellipsisGlyphWidth;
              }

              position.x += ellipsisGlyph.xBearing;
              position.y = penY - ellipsisGlyph.yBearing;

              inserted = true;
            }
          }

          if( !inserted )
          {
            if( index > 0u )
            {
              --index;
            }
            else
            {
              // No space for the ellipsis.
              inserted = true;
            }
            ++numberOfRemovedGlyphs;
          }
        }

        // 'Removes' all the glyphs after the ellipsis glyph.
        numberOfLaidOutGlyphs -= numberOfRemovedGlyphs;
      }
    }
  }

  return numberOfLaidOutGlyphs;
}

const Vector4& View::GetTextColor() const
{
  if( mImpl->mVisualModel )
  {
    return mImpl->mVisualModel->GetTextColor();
  }
  return Vector4::ZERO;
}

const Vector2& View::GetShadowOffset() const
{
  if( mImpl->mVisualModel )
  {
    return mImpl->mVisualModel->GetShadowOffset();
  }
  return Vector2::ZERO;
}

const Vector4& View::GetShadowColor() const
{
  if( mImpl->mVisualModel )
  {
    return mImpl->mVisualModel->GetShadowColor();
  }
  return Vector4::ZERO;
}

const Vector4& View::GetUnderlineColor() const
{
  if( mImpl->mVisualModel )
  {
    return mImpl->mVisualModel->GetUnderlineColor();
  }
  return Vector4::ZERO;
}

bool View::IsUnderlineEnabled() const
{
  if( mImpl->mVisualModel )
  {
    return mImpl->mVisualModel->IsUnderlineEnabled();
  }
  return false;
}

float View::GetUnderlineHeight() const
{
  if( mImpl->mVisualModel )
  {
    return mImpl->mVisualModel->GetUnderlineHeight();
  }
  return 0.0f;
}

Length View::GetNumberOfUnderlineRuns() const
{
  if( mImpl->mVisualModel )
  {
    return mImpl->mVisualModel->mUnderlineRuns.Count();
  }

  return 0u;
}

void View::GetUnderlineRuns( GlyphRun* underlineRuns,
                             UnderlineRunIndex index,
                             Length numberOfRuns ) const
{
  if( mImpl->mVisualModel )
  {
    mImpl->mVisualModel->GetUnderlineRuns( underlineRuns,
                                           index,
                                           numberOfRuns );
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

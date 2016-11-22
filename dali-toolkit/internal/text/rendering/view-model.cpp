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

// CLASS HEADER
#include <dali-toolkit/internal/text/rendering/view-model.h>

// EXTERNAL INCLUDES
#include <memory.h>
#include <dali/devel-api/text-abstraction/font-client.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/line-run.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

ViewModel::ViewModel( const ModelInterface* const model )
: mModel( model ),
  mElidedGlyphs(),
  mElidedLayout(),
  mIsTextElided( false )
{
}

ViewModel::~ViewModel()
{
}

const Size& ViewModel::GetControlSize() const
{
  return mModel->GetControlSize();
}

const Size& ViewModel::GetLayoutSize() const
{
  return mModel->GetLayoutSize();
}

const Vector2& ViewModel::GetScrollPosition() const
{
  return mModel->GetScrollPosition();
}

Layout::HorizontalAlignment ViewModel::GetHorizontalAlignment() const
{
  return mModel->GetHorizontalAlignment();
}

Layout::VerticalAlignment ViewModel::GetVerticalAlignment() const
{
  return mModel->GetVerticalAlignment();
}

bool ViewModel::IsTextElideEnabled() const
{
  return mModel->IsTextElideEnabled();
}

Length ViewModel::GetNumberOfLines() const
{
  return mModel->GetNumberOfLines();
}

const LineRun* const ViewModel::GetLines() const
{
  return mModel->GetLines();
}

Length ViewModel::GetNumberOfGlyphs() const
{
  if( mIsTextElided && mModel->IsTextElideEnabled() )
  {
     return mElidedGlyphs.Count();
  }
  else
  {
    return mModel->GetNumberOfGlyphs();
  }

  return 0u;
}

const GlyphInfo* const ViewModel::GetGlyphs() const
{
  if( mIsTextElided && mModel->IsTextElideEnabled() )
  {
    return mElidedGlyphs.Begin();
  }
  else
  {
    return mModel->GetGlyphs();
  }

  return NULL;
}

const Vector2* const ViewModel::GetLayout() const
{
  if( mIsTextElided && mModel->IsTextElideEnabled() )
  {
    return mElidedLayout.Begin();
  }
  else
  {
    return mModel->GetLayout();
  }

  return NULL;
}

const Vector4* const ViewModel::GetColors() const
{
  return mModel->GetColors();
}

const ColorIndex* const ViewModel::GetColorIndices() const
{
  return mModel->GetColorIndices();
}

const Vector4& ViewModel::GetDefaultColor() const
{
  return mModel->GetDefaultColor();
}

void ViewModel::ElideGlyphs()
{
  mIsTextElided = false;

  if( mModel->IsTextElideEnabled() )
  {
    const Length numberOfLines = mModel->GetNumberOfLines();
    if( 0u != numberOfLines )
    {
      const LineRun* const lines = mModel->GetLines();

      const LineRun& lastLine = *( lines + ( numberOfLines - 1u ) );
      const Length numberOfLaidOutGlyphs = lastLine.glyphRun.glyphIndex + lastLine.glyphRun.numberOfGlyphs;

      if( lastLine.ellipsis && ( 0u != numberOfLaidOutGlyphs ) )
      {
        mIsTextElided = true;
        TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();

        const GlyphInfo* const glyphs = mModel->GetGlyphs();
        const Vector2* const positions = mModel->GetLayout();

        // Copy the glyphs to be elided.
        mElidedGlyphs.Resize( numberOfLaidOutGlyphs );
        mElidedLayout.Resize( numberOfLaidOutGlyphs );

        GlyphInfo* elidedGlyphsBuffer = mElidedGlyphs.Begin();
        Vector2* elidedPositionsBuffer = mElidedLayout.Begin();

        memcpy( elidedGlyphsBuffer, glyphs, numberOfLaidOutGlyphs * sizeof( GlyphInfo ) );
        memcpy( elidedPositionsBuffer, positions, numberOfLaidOutGlyphs * sizeof( Vector2 ) );

        const Size& controlSize = mModel->GetControlSize();

        if( ( 1u == numberOfLines ) &&
            ( lastLine.ascender - lastLine.descender > controlSize.height ) )
        {
          // Get the first glyph which is going to be replaced and the ellipsis glyph.
          GlyphInfo& glyphToRemove = *elidedGlyphsBuffer;
          const GlyphInfo& ellipsisGlyph = fontClient.GetEllipsisGlyph( fontClient.GetPointSize( glyphToRemove.fontId ) );

          // Change the 'x' and 'y' position of the ellipsis glyph.
          Vector2& position = *elidedPositionsBuffer;

          position.x = ellipsisGlyph.xBearing;
          position.y = -lastLine.ascender + controlSize.height - ellipsisGlyph.yBearing;

          // Replace the glyph by the ellipsis glyph and resize the buffers.
          glyphToRemove = ellipsisGlyph;

          mElidedGlyphs.Resize( 1u );
          mElidedLayout.Resize( 1u );

          return;
        }

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
          const GlyphInfo& glyphToRemove = *( elidedGlyphsBuffer + index );

          if( 0u != glyphToRemove.fontId )
          {
            // i.e. The font id of the glyph shaped from the '\n' character is zero.

            // Need to reshape the glyph as the font may be different in size.
            const GlyphInfo& ellipsisGlyph = fontClient.GetEllipsisGlyph( fontClient.GetPointSize( glyphToRemove.fontId ) );

            if( !firstPenSet )
            {
              const Vector2& position = *( elidedPositionsBuffer + index );

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
              GlyphInfo& glyphInfo = *( elidedGlyphsBuffer + index );
              Vector2& position = *( elidedPositionsBuffer + index );
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
        } // while( !inserted )

          // 'Removes' all the glyphs after the ellipsis glyph.
        const Length numberOfGlyphs = numberOfLaidOutGlyphs - numberOfRemovedGlyphs;
        mElidedGlyphs.Resize( numberOfGlyphs );
        mElidedLayout.Resize( numberOfGlyphs );
      }
    }
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/rendering/text-typesetter.h>

// EXTERNAL INCLUDES
#include <memory.h>
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/public-api/common/constants.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/rendering/view-model.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-label-devel.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

namespace
{

/**
 * @brief Data struct used to set the buffer of the glyph's bitmap into the final bitmap's buffer.
 */
struct GlyphData
{
  Devel::PixelBuffer                           bitmapBuffer;     ///< The buffer of the whole bitmap. The format is RGBA8888.
  Vector2*                                     position;         ///< The position of the glyph.
  TextAbstraction::FontClient::GlyphBufferData glyphBitmap;      ///< The glyph's bitmap.
  unsigned int                                 width;            ///< The bitmap's width.
  unsigned int                                 height;           ///< The bitmap's height.
  int                                          horizontalOffset; ///< The horizontal offset to be added to the 'x' glyph's position.
  int                                          verticalOffset;   ///< The vertical offset to be added to the 'y' glyph's position.
};

/**
 * @brief Sets the glyph's buffer into the bitmap's buffer.
 *
 * @param[in] data Struct which contains the glyph's data and the bitmap's data.
 * @param[in] position The position of the glyph.
 * @param[in] color The color of the glyph.
 * @param[in] style The style of the text.
 * @param[in] pixelFormat The format of the pixel in the image that the text is rendered as (i.e. either Pixel::BGRA8888 or Pixel::L8).
 */
void TypesetGlyph( GlyphData& data,
                   const Vector2* const position,
                   const Vector4* const color,
                   Typesetter::Style style,
                   Pixel::Format pixelFormat )
{
  if( ( 0u == data.glyphBitmap.width ) || ( 0u == data.glyphBitmap.height ) )
  {
    // Nothing to do if the width or height of the buffer is zero.
    return;
  }

  const int widthMinusOne = static_cast<int>( data.width - 1u );
  const int heightMinusOne = static_cast<int>( data.height - 1u );

  if ( Pixel::RGBA8888 == pixelFormat )
  {
    // Whether the given glyph is a color one.
    const bool isColorGlyph = Pixel::BGRA8888 == data.glyphBitmap.format;

    // Pointer to the color glyph if there is one.
    const uint32_t* const colorGlyphBuffer = isColorGlyph ? reinterpret_cast<uint32_t*>( data.glyphBitmap.buffer ) : NULL;

    // Pack the given color into a 32bit buffer. The alpha channel will be updated later for each pixel.
    // The format is RGBA8888.
    uint32_t packedColor = 0u;
    uint8_t* packedColorBuffer = reinterpret_cast<uint8_t*>( &packedColor );
    *( packedColorBuffer + 2 ) = static_cast<uint8_t>( color->b * 255.f );
    *( packedColorBuffer + 1 ) = static_cast<uint8_t>( color->g * 255.f );
      *packedColorBuffer       = static_cast<uint8_t>( color->r * 255.f );

    // Initial vertical offset.
    const int yOffset = data.verticalOffset + position->y;

    // Traverse the pixels of the glyph line per line.
    for( int lineIndex = 0, glyphHeight = static_cast<int>( data.glyphBitmap.height ); lineIndex < glyphHeight; ++lineIndex )
    {
      const int yOffsetIndex = yOffset + lineIndex;
      if( ( 0 > yOffsetIndex ) || ( yOffsetIndex > heightMinusOne ) )
      {
        // Do not write out of bounds.
        continue;
      }

      const int verticalOffset = yOffsetIndex * data.width;
      const int xOffset = data.horizontalOffset + position->x;
      const int glyphBufferOffset = lineIndex * static_cast<int>( data.glyphBitmap.width );
      for( int index = 0, glyphWidth = static_cast<int>( data.glyphBitmap.width ); index < glyphWidth; ++index )
      {
        const int xOffsetIndex = xOffset + index;
        if( ( 0 > xOffsetIndex ) || ( xOffsetIndex > widthMinusOne ) )
        {
          // Don't write out of bounds.
          continue;
        }

        uint32_t* bitmapBuffer = reinterpret_cast< uint32_t* >( data.bitmapBuffer.GetBuffer() );

        if( isColorGlyph )
        {
          // Retrieves the color from the color glyph. The format is BGRA8888.
          uint32_t packedColorGlyph = *( colorGlyphBuffer + glyphBufferOffset + index );
          uint8_t* packedColorGlyphBuffer = reinterpret_cast<uint8_t*>( &packedColorGlyph );

          if( Typesetter::STYLE_SHADOW == style )
          {
            // The shadow of color glyph needs to have the shadow color.
            *( packedColorGlyphBuffer + 2 ) = static_cast<uint8_t>( color->b * 255.f );
            *( packedColorGlyphBuffer + 1 ) = static_cast<uint8_t>( color->g * 255.f );
              *packedColorGlyphBuffer       = static_cast<uint8_t>( color->r * 255.f );
          }
          else
          {
            std::swap( *packedColorGlyphBuffer, *( packedColorGlyphBuffer + 2u ) ); // Swap B and R.
          }

          // Update the alpha channel.
          if( Typesetter::STYLE_MASK == style || Typesetter::STYLE_OUTLINE == style ) // Outline not shown for color glyph
          {
            // Create an alpha mask for color glyph.
            *( packedColorGlyphBuffer + 3u ) = 0u;
          }
          else
          {
            *( packedColorGlyphBuffer + 3u ) = static_cast<uint8_t>( color->a * static_cast<float>( *( packedColorGlyphBuffer + 3u ) ) );
          }

          // Set the color into the final pixel buffer.
          *( bitmapBuffer + verticalOffset + xOffsetIndex ) = packedColorGlyph;
        }
        else
        {
          // Update the alpha channel.
          const uint8_t alpha = *( data.glyphBitmap.buffer + glyphBufferOffset + index );

          // Copy non-transparent pixels only
          if ( alpha > 0u )
          {
            // Check alpha of overlapped pixels
            uint32_t& currentColor = *( bitmapBuffer + verticalOffset + xOffsetIndex );
            uint8_t* packedCurrentColorBuffer = reinterpret_cast<uint8_t*>( &currentColor );

            uint8_t currentAlpha = *( packedCurrentColorBuffer + 3u );
            uint8_t newAlpha = static_cast<uint8_t>( color->a * static_cast<float>( alpha ) );

            // For any pixel overlapped with the pixel in previous glyphs, make sure we don't
            // overwrite a previous bigger alpha with a smaller alpha (in order to avoid
            // semi-transparent gaps between joint glyphs with overlapped pixels, which could
            // happen, for example, in the RTL text when we copy glyphs from right to left).
            *( packedColorBuffer + 3u ) = std::max( currentAlpha, newAlpha );

            // Set the color into the final pixel buffer.
            currentColor = packedColor;
          }
        }
      }
    }
  }
  else
  {
    // Whether the given glyph is a color one.
    const bool isColorGlyph = Pixel::BGRA8888 == data.glyphBitmap.format;

    // Initial vertical offset.
    const int yOffset = data.verticalOffset + position->y;

    // Traverse the pixels of the glyph line per line.
    for( int lineIndex = 0, glyphHeight = static_cast<int>( data.glyphBitmap.height ); lineIndex < glyphHeight; ++lineIndex )
    {
      const int yOffsetIndex = yOffset + lineIndex;
      if( ( 0 > yOffsetIndex ) || ( yOffsetIndex > heightMinusOne ) )
      {
        // Do not write out of bounds.
        continue;
      }

      const int verticalOffset = yOffsetIndex * data.width;
      const int xOffset = data.horizontalOffset + position->x;
      const int glyphBufferOffset = lineIndex * static_cast<int>( data.glyphBitmap.width );
      for( int index = 0, glyphWidth = static_cast<int>( data.glyphBitmap.width ); index < glyphWidth; ++index )
      {
        const int xOffsetIndex = xOffset + index;
        if( ( 0 > xOffsetIndex ) || ( xOffsetIndex > widthMinusOne ) )
        {
          // Don't write out of bounds.
          continue;
        }

        uint8_t* bitmapBuffer = reinterpret_cast< uint8_t* >( data.bitmapBuffer.GetBuffer() );

        if ( !isColorGlyph )
        {
          // Update the alpha channel.
          const uint8_t alpha = *( data.glyphBitmap.buffer + glyphBufferOffset + index );

          // Copy non-transparent pixels only
          if ( alpha > 0u )
          {
            // Check alpha of overlapped pixels
            uint8_t& currentAlpha = *( bitmapBuffer + verticalOffset + xOffsetIndex );
            uint8_t newAlpha = static_cast<uint8_t>( color->a * static_cast<float>( alpha ) );

            // For any pixel overlapped with the pixel in previous glyphs, make sure we don't
            // overwrite a previous bigger alpha with a smaller alpha (in order to avoid
            // semi-transparent gaps between joint glyphs with overlapped pixels, which could
            // happen, for example, in the RTL text when we copy glyphs from right to left).
            *( bitmapBuffer + verticalOffset + xOffsetIndex ) = std::max( currentAlpha, newAlpha );
          }
        }
      }
    }
  }
}

bool IsGlyphUnderlined( GlyphIndex index,
                         const Vector<GlyphRun>& underlineRuns )
{
  for( Vector<GlyphRun>::ConstIterator it = underlineRuns.Begin(),
         endIt = underlineRuns.End();
         it != endIt;
       ++it )
  {
    const GlyphRun& run = *it;

    if( ( run.glyphIndex <= index ) && ( index < run.glyphIndex + run.numberOfGlyphs ) )
    {
      return true;
    }
  }

  return false;
}

} // namespace

TypesetterPtr Typesetter::New( const ModelInterface* const model )
{
  return TypesetterPtr( new Typesetter( model ) );
}

ViewModel* Typesetter::GetViewModel()
{
  return mModel;
}

PixelData Typesetter::Render( const Vector2& size, RenderBehaviour behaviour, bool ignoreHorizontalAlignment, Pixel::Format pixelFormat )
{
  // @todo. This initial implementation for a TextLabel has only one visible page.

  // Elides the text if needed.
  mModel->ElideGlyphs();

  // Retrieves the layout size.
  const Size& layoutSize = mModel->GetLayoutSize();

  // Set the offset for the vertical alignment.
  int penY = 0u;

  switch( mModel->GetVerticalAlignment() )
  {
    case VerticalAlignment::TOP:
    {
      // No offset to add.
      break;
    }
    case VerticalAlignment::CENTER:
    {
      penY = static_cast<int>( 0.5f * ( size.height - layoutSize.height ) );
      break;
    }
    case VerticalAlignment::BOTTOM:
    {
      penY = static_cast<int>( size.height - layoutSize.height );
      break;
    }
  }

  // Calculate vertical line alignment
  switch( mModel->GetVerticalLineAlignment() )
  {
    case DevelText::VerticalLineAlignment::TOP:
    {
      break;
    }
    case DevelText::VerticalLineAlignment::MIDDLE:
    {
      const auto& line = *mModel->GetLines();
      penY -= line.descender;
      penY += static_cast<int>(line.lineSpacing*0.5f + line.descender);
      break;
    }
    case DevelText::VerticalLineAlignment::BOTTOM:
    {
      const auto& line = *mModel->GetLines();
      const auto lineHeight = line.ascender + (-line.descender) + line.lineSpacing;
      penY += static_cast<int>(lineHeight - (line.ascender - line.descender));
      break;
    }
  }

  // Generate the image buffers of the text for each different style first,
  // then combine all of them together as one final image buffer. We try to
  // do all of these in CPU only, so that once the final texture is generated,
  // no calculation is needed in GPU during each frame.

  const unsigned int bufferWidth = static_cast<unsigned int>( size.width );
  const unsigned int bufferHeight = static_cast<unsigned int>( size.height );

  const unsigned int bufferSizeInt = bufferWidth * bufferHeight;
  const unsigned int bufferSizeChar = 4u * bufferSizeInt;

  Length numberOfGlyphs = mModel->GetNumberOfGlyphs();

  Devel::PixelBuffer imageBuffer;

  if( RENDER_MASK == behaviour )
  {
    // Generate the image buffer as an alpha mask for color glyphs.
    imageBuffer = CreateImageBuffer( bufferWidth, bufferHeight, Typesetter::STYLE_MASK, ignoreHorizontalAlignment, pixelFormat, penY, 0u, numberOfGlyphs - 1 );
  }
  else if( RENDER_NO_TEXT == behaviour )
  {
    // Generate an empty image buffer so that it can been combined with the image buffers for styles
    imageBuffer = Devel::PixelBuffer::New( bufferWidth, bufferHeight, Pixel::RGBA8888 );
    memset( imageBuffer.GetBuffer(), 0u, bufferSizeChar );
  }
  else
  {
    // Generate the image buffer for the text with no style.
    imageBuffer = CreateImageBuffer( bufferWidth, bufferHeight, Typesetter::STYLE_NONE, ignoreHorizontalAlignment, pixelFormat, penY, 0u, numberOfGlyphs -1 );
  }

  if ( ( RENDER_NO_STYLES != behaviour ) && ( RENDER_MASK != behaviour ) )
  {

    // Generate the outline if enabled
    const float outlineWidth = mModel->GetOutlineWidth();
    if ( outlineWidth > Math::MACHINE_EPSILON_1 )
    {
      // Create the image buffer for outline
      Devel::PixelBuffer outlineImageBuffer = CreateImageBuffer( bufferWidth, bufferHeight, Typesetter::STYLE_OUTLINE, ignoreHorizontalAlignment, pixelFormat, penY, 0u, numberOfGlyphs -1 );

      // Combine the two buffers
      imageBuffer = CombineImageBuffer( imageBuffer, outlineImageBuffer, bufferWidth, bufferHeight );
    }

    // @todo. Support shadow and underline for partial text later on.

    // Generate the shadow if enabled
    const Vector2& shadowOffset = mModel->GetShadowOffset();
    if ( fabsf( shadowOffset.x ) > Math::MACHINE_EPSILON_1 || fabsf( shadowOffset.y ) > Math::MACHINE_EPSILON_1 )
    {
      // Create the image buffer for shadow
      Devel::PixelBuffer shadowImageBuffer = CreateImageBuffer( bufferWidth, bufferHeight, Typesetter::STYLE_SHADOW, ignoreHorizontalAlignment, pixelFormat, penY, 0u, numberOfGlyphs - 1 );

      // Check whether it will be a soft shadow
      const float& blurRadius = mModel->GetShadowBlurRadius();

      if ( blurRadius > Math::MACHINE_EPSILON_1 )
      {
        shadowImageBuffer.ApplyGaussianBlur( blurRadius );
      }

      // Combine the two buffers
      imageBuffer = CombineImageBuffer( imageBuffer, shadowImageBuffer, bufferWidth, bufferHeight );
    }

    // Generate the underline if enabled
    const bool underlineEnabled = mModel->IsUnderlineEnabled();
    if ( underlineEnabled )
    {
      // Create the image buffer for underline
      Devel::PixelBuffer underlineImageBuffer = CreateImageBuffer( bufferWidth, bufferHeight, Typesetter::STYLE_UNDERLINE, ignoreHorizontalAlignment, pixelFormat, penY, 0u, numberOfGlyphs - 1 );

      // Combine the two buffers
      imageBuffer = CombineImageBuffer( imageBuffer, underlineImageBuffer, bufferWidth, bufferHeight );
    }
  }

  // Create the final PixelData for the combined image buffer
  PixelData pixelData = Devel::PixelBuffer::Convert( imageBuffer );

  return pixelData;
}

Devel::PixelBuffer Typesetter::CreateImageBuffer( const unsigned int bufferWidth, const unsigned int bufferHeight, Typesetter::Style style, bool ignoreHorizontalAlignment, Pixel::Format pixelFormat, int verticalOffset, GlyphIndex fromGlyphIndex, GlyphIndex toGlyphIndex )
{
  // Retrieve lines, glyphs, positions and colors from the view model.
  const Length modelNumberOfLines = mModel->GetNumberOfLines();
  const LineRun* const modelLinesBuffer = mModel->GetLines();
  const Length numberOfGlyphs = mModel->GetNumberOfGlyphs();
  const GlyphInfo* const glyphsBuffer = mModel->GetGlyphs();
  const Vector2* const positionBuffer = mModel->GetLayout();
  const Vector4* const colorsBuffer = mModel->GetColors();
  const ColorIndex* const colorIndexBuffer = mModel->GetColorIndices();

  // Whether to use the default color.
  const bool useDefaultColor = ( NULL == colorsBuffer );
  const Vector4& defaultColor = mModel->GetDefaultColor();

  // Create and initialize the pixel buffer.
  GlyphData glyphData;
  glyphData.verticalOffset = verticalOffset;
  glyphData.width = bufferWidth;
  glyphData.height = bufferHeight;
  glyphData.bitmapBuffer = Devel::PixelBuffer::New( bufferWidth, bufferHeight, pixelFormat );
  glyphData.horizontalOffset = 0;

  if ( Pixel::RGBA8888 == pixelFormat )
  {
    const unsigned int bufferSizeInt = bufferWidth * bufferHeight;
    const unsigned int bufferSizeChar = 4u * bufferSizeInt;
    memset( glyphData.bitmapBuffer.GetBuffer(), 0u, bufferSizeChar );
  }
  else
  {
    memset( glyphData.bitmapBuffer.GetBuffer(), 0, bufferWidth * bufferHeight );
  }

  // Get a handle of the font client. Used to retrieve the bitmaps of the glyphs.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();

  // Traverses the lines of the text.
  for( LineIndex lineIndex = 0u; lineIndex < modelNumberOfLines; ++lineIndex )
  {
    const LineRun& line = *( modelLinesBuffer + lineIndex );

    // Sets the horizontal offset of the line.
    glyphData.horizontalOffset = ignoreHorizontalAlignment ? 0 : static_cast<int>( line.alignmentOffset );

    // Increases the vertical offset with the line's ascender.
    glyphData.verticalOffset += static_cast<int>( line.ascender );

    // Include line spacing after first line
    if( lineIndex > 0u )
    {
      glyphData.verticalOffset += static_cast<int>( line.lineSpacing );
    }

    // Retrieves the glyph's outline width
    float outlineWidth = mModel->GetOutlineWidth();

    if( style == Typesetter::STYLE_OUTLINE )
    {
      glyphData.horizontalOffset -= outlineWidth;
      if( lineIndex == 0u )
      {
        // Only need to add the vertical outline offset for the first line
        glyphData.verticalOffset -= outlineWidth;
      }
    }
    else if ( style == Typesetter::STYLE_SHADOW )
    {
      const Vector2& shadowOffset = mModel->GetShadowOffset();
      glyphData.horizontalOffset += shadowOffset.x - outlineWidth; // if outline enabled then shadow should offset from outline

      if ( lineIndex == 0u )
      {
        // Only need to add the vertical shadow offset for first line
        glyphData.verticalOffset += shadowOffset.y - outlineWidth;
      }
    }

    const bool underlineEnabled = mModel->IsUnderlineEnabled();
    const Vector4& underlineColor = mModel->GetUnderlineColor();
    const float underlineHeight = mModel->GetUnderlineHeight();

    // Get the underline runs.
    const Length numberOfUnderlineRuns = mModel->GetNumberOfUnderlineRuns();
    Vector<GlyphRun> underlineRuns;
    underlineRuns.Resize( numberOfUnderlineRuns );
    mModel->GetUnderlineRuns( underlineRuns.Begin(), 0u, numberOfUnderlineRuns );

    bool thereAreUnderlinedGlyphs = false;

    float currentUnderlinePosition = 0.0f;
    float currentUnderlineThickness = underlineHeight;
    float maxUnderlineThickness = currentUnderlineThickness;

    FontId lastUnderlinedFontId = 0;

    float lineExtentLeft = bufferWidth;
    float lineExtentRight = 0.0f;
    float baseline = 0.0f;

    // Traverses the glyphs of the line.
    const GlyphIndex endGlyphIndex = std::min( numberOfGlyphs, line.glyphRun.glyphIndex + line.glyphRun.numberOfGlyphs );
    for( GlyphIndex glyphIndex = line.glyphRun.glyphIndex; glyphIndex < endGlyphIndex; ++glyphIndex )
    {
      if ( glyphIndex < fromGlyphIndex || glyphIndex > toGlyphIndex )
      {
        // Ignore any glyph that out of the specified range
        continue;
      }

      // Retrieve the glyph's info.
      const GlyphInfo* const glyphInfo = glyphsBuffer + glyphIndex;

      if( ( glyphInfo->width < Math::MACHINE_EPSILON_1000 ) ||
          ( glyphInfo->height < Math::MACHINE_EPSILON_1000 ) )
      {
        // Nothing to do if the glyph's width or height is zero.
        continue;
      }

      const bool underlineGlyph = underlineEnabled || IsGlyphUnderlined( glyphIndex, underlineRuns );
      thereAreUnderlinedGlyphs = thereAreUnderlinedGlyphs || underlineGlyph;

      // Are we still using the same fontId as previous
      if( underlineGlyph && ( glyphInfo->fontId != lastUnderlinedFontId ) )
      {
        // We need to fetch fresh font underline metrics
        FontMetrics fontMetrics;
        fontClient.GetFontMetrics( glyphInfo->fontId, fontMetrics );
        currentUnderlinePosition = ceil( fabsf( fontMetrics.underlinePosition ) );
        const float descender = ceil( fabsf( fontMetrics.descender ) );

        if( fabsf( underlineHeight ) < Math::MACHINE_EPSILON_1000 )
        {
          currentUnderlineThickness = fontMetrics.underlineThickness;

          // Ensure underline will be at least a pixel high
          if ( currentUnderlineThickness < 1.0f )
          {
            currentUnderlineThickness = 1.0f;
          }
          else
          {
            currentUnderlineThickness = ceil( currentUnderlineThickness );
          }
        }

        // The underline thickness should be the max underline thickness of all glyphs of the line.
        if ( currentUnderlineThickness > maxUnderlineThickness )
        {
          maxUnderlineThickness = currentUnderlineThickness;
        }

        // Clamp the underline position at the font descender and check for ( as EFL describes it ) a broken font
        if( currentUnderlinePosition > descender )
        {
          currentUnderlinePosition = descender;
        }

        if( fabsf( currentUnderlinePosition ) < Math::MACHINE_EPSILON_1000 )
        {
          // Move offset down by one ( EFL behavior )
          currentUnderlinePosition = 1.0f;
        }

        lastUnderlinedFontId = glyphInfo->fontId;
      } // underline

      // Retrieves the glyph's position.
      const Vector2* const position = positionBuffer + glyphIndex;
      if ( baseline < position->y + glyphInfo->yBearing )
      {
        baseline = position->y + glyphInfo->yBearing;
      }

      // Calculate the positions of leftmost and rightmost glyphs in the current line
      if ( position->x < lineExtentLeft)
      {
        lineExtentLeft = position->x;
      }

      if ( position->x + glyphInfo->width > lineExtentRight)
      {
        lineExtentRight = position->x + glyphInfo->width;
      }

      // Retrieves the glyph's color.
      const ColorIndex colorIndex = *( colorIndexBuffer + glyphIndex );

      const Vector4* color;
      if ( style == Typesetter::STYLE_SHADOW )
      {
        color = &( mModel->GetShadowColor() );
      }
      else if ( style == Typesetter::STYLE_OUTLINE )
      {
        color = &( mModel->GetOutlineColor() );
      }
      else
      {
        color = ( useDefaultColor || ( 0u == colorIndex ) ) ? &defaultColor : colorsBuffer + ( colorIndex - 1u );
      }

      // Retrieves the glyph's bitmap.
      glyphData.glyphBitmap.buffer = NULL;
      glyphData.glyphBitmap.width = glyphInfo->width;   // Desired width and height.
      glyphData.glyphBitmap.height = glyphInfo->height;

      if( style != Typesetter::STYLE_OUTLINE && style != Typesetter::STYLE_SHADOW )
      {
        // Don't render outline for other styles
        outlineWidth = 0.0f;
      }
      if( style != Typesetter::STYLE_UNDERLINE )
      {
        fontClient.CreateBitmap( glyphInfo->fontId,
                                 glyphInfo->index,
                                 glyphData.glyphBitmap,
                                 outlineWidth );
      }


      // Sets the glyph's bitmap into the bitmap of the whole text.
      if( NULL != glyphData.glyphBitmap.buffer )
      {
        TypesetGlyph( glyphData,
                      position,
                      color,
                      style,
                      pixelFormat);
        // delete the glyphBitmap.buffer as it is now copied into glyphData.bitmapBuffer
        delete []glyphData.glyphBitmap.buffer;
        glyphData.glyphBitmap.buffer = NULL;
      }
    }

    // Draw the underline from the leftmost glyph to the rightmost glyph
    if ( thereAreUnderlinedGlyphs && style == Typesetter::STYLE_UNDERLINE )
    {
      int underlineYOffset = glyphData.verticalOffset + baseline + currentUnderlinePosition;

      for( unsigned int y = underlineYOffset; y < underlineYOffset + maxUnderlineThickness; y++ )
      {
        if( y > bufferHeight - 1 )
        {
          // Do not write out of bounds.
          break;
        }

        for( unsigned int x = glyphData.horizontalOffset + lineExtentLeft; x <= glyphData.horizontalOffset + lineExtentRight; x++ )
        {
          if( x > bufferWidth - 1 )
          {
            // Do not write out of bounds.
            break;
          }

          // Always RGBA image for text with styles
          uint32_t* bitmapBuffer = reinterpret_cast< uint32_t* >( glyphData.bitmapBuffer.GetBuffer() );
          uint32_t underlinePixel = *( bitmapBuffer + y * glyphData.width + x );
          uint8_t* underlinePixelBuffer = reinterpret_cast<uint8_t*>( &underlinePixel );

          // Write the underline color to the pixel buffer
          *( underlinePixelBuffer ) = static_cast<uint8_t>( underlineColor.r * 255.f );
          *( underlinePixelBuffer + 1u ) = static_cast<uint8_t>( underlineColor.g * 255.f );
          *( underlinePixelBuffer + 2u ) = static_cast<uint8_t>( underlineColor.b * 255.f );
          *( underlinePixelBuffer + 3u ) = static_cast<uint8_t>( underlineColor.a * 255.f );

          *( bitmapBuffer + y * glyphData.width + x ) = underlinePixel;
        }
      }
    }

    // Increases the vertical offset with the line's descender.
    glyphData.verticalOffset += static_cast<int>( -line.descender );
  }

  return glyphData.bitmapBuffer;
}

Devel::PixelBuffer Typesetter::CombineImageBuffer( Devel::PixelBuffer topPixelBuffer, Devel::PixelBuffer bottomPixelBuffer, const unsigned int bufferWidth, const unsigned int bufferHeight )
{
  unsigned char* topBuffer = topPixelBuffer.GetBuffer();
  unsigned char* bottomBuffer = bottomPixelBuffer.GetBuffer();

  Devel::PixelBuffer combinedPixelBuffer;

  if ( topBuffer == NULL && bottomBuffer == NULL )
  {
    // Nothing to do if both buffers are empty.
    return combinedPixelBuffer;
  }

  if ( topBuffer == NULL )
  {
    // Nothing to do if topBuffer is empty.
    return bottomPixelBuffer;
  }

  if ( bottomBuffer == NULL )
  {
    // Nothing to do if bottomBuffer is empty.
    return topPixelBuffer;
  }

  // Always combine two RGBA images
  const unsigned int bufferSizeInt = bufferWidth * bufferHeight;
  const unsigned int bufferSizeChar = 4u * bufferSizeInt;

  combinedPixelBuffer = Devel::PixelBuffer::New( bufferWidth, bufferHeight, Pixel::RGBA8888 );
  uint8_t* combinedBuffer = reinterpret_cast< uint8_t* >( combinedPixelBuffer.GetBuffer() );
  memset( combinedBuffer, 0u, bufferSizeChar );

  for (unsigned int pixelIndex = 0; pixelIndex < bufferSizeInt; pixelIndex++)
  {
    // If the alpha of the pixel in either buffer is not fully opaque, blend the two pixels.
    // Otherwise, copy pixel from topBuffer to combinedBuffer.

    unsigned int alphaBuffer1 = topBuffer[pixelIndex*4+3];
    unsigned int alphaBuffer2 = bottomBuffer[pixelIndex*4+3];

    if ( alphaBuffer1 != 255 || alphaBuffer2 != 255 )
    {
      // At least one pixel is not fully opaque
      // "Over" blend the the pixel from topBuffer with the pixel in bottomBuffer
      combinedBuffer[pixelIndex*4] = ( topBuffer[pixelIndex*4] * topBuffer[pixelIndex*4+3] / 255 ) + ( bottomBuffer[pixelIndex*4] * bottomBuffer[pixelIndex*4+3] * ( 255 - topBuffer[pixelIndex*4+3] ) / ( 255*255 ) );
      combinedBuffer[pixelIndex*4+1] = ( topBuffer[pixelIndex*4+1] * topBuffer[pixelIndex*4+3] / 255 ) + ( bottomBuffer[pixelIndex*4+1] * bottomBuffer[pixelIndex*4+3] * ( 255 - topBuffer[pixelIndex*4+3] ) / ( 255*255 ) );
      combinedBuffer[pixelIndex*4+2] = ( topBuffer[pixelIndex*4+2] * topBuffer[pixelIndex*4+3] / 255 ) + ( bottomBuffer[pixelIndex*4+2] * bottomBuffer[pixelIndex*4+3] * ( 255 - topBuffer[pixelIndex*4+3] ) / ( 255*255 ) );
      combinedBuffer[pixelIndex*4+3] = topBuffer[pixelIndex*4+3] + ( bottomBuffer[pixelIndex*4+3] * ( 255 - topBuffer[pixelIndex*4+3] ) / 255 );
    }
    else
    {
      // Copy the pixel from topBuffer to combinedBuffer
      combinedBuffer[pixelIndex*4] = topBuffer[pixelIndex*4];
      combinedBuffer[pixelIndex*4+1] = topBuffer[pixelIndex*4+1];
      combinedBuffer[pixelIndex*4+2] = topBuffer[pixelIndex*4+2];
      combinedBuffer[pixelIndex*4+3] = topBuffer[pixelIndex*4+3];
    }
  }

  return combinedPixelBuffer;
}

Typesetter::Typesetter( const ModelInterface* const model )
: mModel( new ViewModel( model ) )
{
}

Typesetter::~Typesetter()
{
  delete mModel;
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

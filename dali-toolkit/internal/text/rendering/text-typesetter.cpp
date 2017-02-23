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
#include <dali-toolkit/internal/text/rendering/text-typesetter.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/text-abstraction/font-client.h>
#include <memory.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/rendering/view-model.h>

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
  uint32_t*                                    bitmapBuffer;     ///< The buffer of the whole bitmap. The format is RGBA8888.
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
 */
void TypesetGlyph( const GlyphData& data,
                   const Vector2* const position,
                   const Vector4* const color )
{
  if( ( 0u == data.glyphBitmap.width ) || ( 0u == data.glyphBitmap.height ) )
  {
    // Nothing to do if the width or height of the buffer is zero.
    return;
  }

  const int widthMinusOne = static_cast<int>( data.width - 1u );
  const int heightMinusOne = static_cast<int>( data.height - 1u );

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
      break;
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
        break;
      }

      if( isColorGlyph )
      {
        // Retrieves the color from the glyph. The format is BGRA8888.
        uint32_t packedColorGlyph = *( colorGlyphBuffer + glyphBufferOffset + index );

        // Update the alpha channel.
        uint8_t* packedColorGlyphBuffer = reinterpret_cast<uint8_t*>( &packedColorGlyph );
        std::swap( *packedColorGlyphBuffer, *( packedColorGlyphBuffer + 2u ) ); // Swap B and R.
        *( packedColorGlyphBuffer + 3u ) = static_cast<uint8_t>( color->a * static_cast<float>( *( packedColorGlyphBuffer + 3u ) ) );

        // Set the color into the final pixel buffer.
        *( data.bitmapBuffer + verticalOffset + xOffsetIndex ) = packedColorGlyph;
      }
      else
      {
        // Update the alpha channel.
        const uint8_t alpha = *( data.glyphBitmap.buffer + glyphBufferOffset + index );
        *( packedColorBuffer + 3u ) = static_cast<uint8_t>( color->a * static_cast<float>( alpha ) );

        // Set the color into the final pixel buffer.
        *( data.bitmapBuffer + verticalOffset + xOffsetIndex ) = packedColor;
      }
    }
  }
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

PixelData Typesetter::Render( const Vector2& size )
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
    case Layout::VERTICAL_ALIGN_TOP:
    {
      // No offset to add.
      break;
    }
    case Layout::VERTICAL_ALIGN_CENTER:
    {
      penY = static_cast<int>( 0.5f * ( size.height - layoutSize.height ) );
      break;
    }
    case Layout::VERTICAL_ALIGN_BOTTOM:
    {
      penY = static_cast<int>( size.height - layoutSize.height );
      break;
    }
  }

  // Retrieve lines, glyphs, positions and colors from the view model.
  const Length modelNumberOfLines = mModel->GetNumberOfLines();
  const LineRun* const modelLinesBuffer = mModel->GetLines();
  const Length numberOfGlyphs = mModel->GetNumberOfGlyphs();
  const GlyphInfo* const glyphsBuffer = mModel->GetGlyphs();
  const Vector2* const positionBuffer = mModel->GetLayout();
  const Vector4* const colorsBuffer = mModel->GetColors();
  const ColorIndex* const colorIndexBuffer = mModel->GetColorIndices();

  // Whether to use the default color.
  const bool useDefaultColor = NULL == colorsBuffer;
  const Vector4& defaultColor = mModel->GetDefaultColor();

  // Create and initialize the pixel buffer.
  GlyphData glyphData;
  glyphData.verticalOffset = penY;

  glyphData.width = static_cast<unsigned int>( size.width );
  glyphData.height = static_cast<unsigned int>( size.height );
  const unsigned int bufferSizeInt = glyphData.width * glyphData.height;
  const unsigned int bufferSizeChar = 4u * bufferSizeInt;
  glyphData.bitmapBuffer = new uint32_t[ bufferSizeInt ]; // This array will get deleted by PixelData because of the DELETE_ARRAY parameter.
  memset( glyphData.bitmapBuffer, 0u, bufferSizeChar );

  PixelData pixelData = PixelData::New( reinterpret_cast<uint8_t*>( glyphData.bitmapBuffer ),
                                        bufferSizeChar,
                                        glyphData.width,
                                        glyphData.height,
                                        Pixel::RGBA8888, // The format is RGBA8888 because is the format accepted by the image atlas manager.
                                        PixelData::DELETE_ARRAY );

  // Get a handle of the font client. Used to retrieve the bitmaps of the glyphs.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();

  // Traverses the lines of the text.
  for( LineIndex lineIndex = 0u; lineIndex < modelNumberOfLines; ++lineIndex )
  {
    const LineRun& line = *( modelLinesBuffer + lineIndex );

    // Sets the horizontal offset of the line.
    glyphData.horizontalOffset = static_cast<int>( line.alignmentOffset );

    // Increases the vertical offset with the line's ascender.
    glyphData.verticalOffset += static_cast<int>( line.ascender );

    // Traverses the glyphs of the line.
    const GlyphIndex endGlyphIndex = std::min( numberOfGlyphs, line.glyphRun.glyphIndex + line.glyphRun.numberOfGlyphs );
    for( GlyphIndex glyphIndex = line.glyphRun.glyphIndex; glyphIndex < endGlyphIndex; ++glyphIndex )
    {
      // Retrieve the glyph's info.
      const GlyphInfo* const glyphInfo = glyphsBuffer + glyphIndex;

      if( ( glyphInfo->width < Math::MACHINE_EPSILON_1000 ) ||
          ( glyphInfo->height < Math::MACHINE_EPSILON_1000 ) )
      {
        // Nothing to do if the glyph's width or height is zero.
        continue;
      }

      // Retrieves the glyph's position.
      const Vector2* const position = positionBuffer + glyphIndex;

      // Retrieves the glyph's color.
      const ColorIndex colorIndex = *( colorIndexBuffer + glyphIndex );
      const Vector4* const color = ( useDefaultColor || ( 0u == colorIndex ) ) ? &defaultColor : colorsBuffer + ( colorIndex - 1u );

      // Retrieves the glyph's bitmap.
      glyphData.glyphBitmap.buffer = NULL;
      glyphData.glyphBitmap.width = glyphInfo->width;   // Desired width and height.
      glyphData.glyphBitmap.height = glyphInfo->height;
      fontClient.CreateBitmap( glyphInfo->fontId,
                               glyphInfo->index,
                               glyphData.glyphBitmap );

      // Sets the glyph's bitmap into the bitmap of the whole text.
      if( NULL != glyphData.glyphBitmap.buffer )
      {
        TypesetGlyph( glyphData,
                      position,
                      color );
      }
    }

    // Increases the vertical offset with the line's descender.
    glyphData.verticalOffset += static_cast<int>( -line.descender );
  }

  return pixelData;
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

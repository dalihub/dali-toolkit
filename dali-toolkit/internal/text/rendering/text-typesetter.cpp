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
#include <dali-toolkit/internal/text/rendering/text-typesetter.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/public-api/common/constants.h>
#include <memory.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/text-controls/text-label-devel.h>
#include <dali-toolkit/internal/text/glyph-metrics-helper.h>
#include <dali-toolkit/internal/text/line-helper-functions.h>
#include <dali-toolkit/internal/text/rendering/styles/character-spacing-helper-functions.h>
#include <dali-toolkit/internal/text/rendering/styles/strikethrough-helper-functions.h>
#include <dali-toolkit/internal/text/rendering/styles/underline-helper-functions.h>
#include <dali-toolkit/internal/text/rendering/view-model.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace
{
const float HALF(0.5f);
const float ONE_AND_A_HALF(1.5f);

/**
 * @brief Fast multiply & divide by 255. It wiil be useful when we applying alpha value in color
 *
 * @param x The value between [0..255]
 * @param y The value between [0..255]
 * @return (x*y)/255
 */
inline uint8_t MultiplyAndNormalizeColor(const uint8_t x, const uint8_t y) noexcept
{
  const uint32_t xy = static_cast<const uint32_t>(x) * y;
  return ((xy << 15) + (xy << 7) + xy) >> 23;
}

/**
 * @brief Data struct used to set the buffer of the glyph's bitmap into the final bitmap's buffer.
 */
struct GlyphData
{
  Devel::PixelBuffer                           bitmapBuffer;     ///< The buffer of the whole bitmap. The format is RGBA8888.
  Vector2*                                     position;         ///< The position of the glyph.
  TextAbstraction::FontClient::GlyphBufferData glyphBitmap;      ///< The glyph's bitmap.
  uint32_t                                     width;            ///< The bitmap's width.
  uint32_t                                     height;           ///< The bitmap's height.
  int32_t                                      horizontalOffset; ///< The horizontal offset to be added to the 'x' glyph's position.
  int32_t                                      verticalOffset;   ///< The vertical offset to be added to the 'y' glyph's position.
};

/**
 * @brief Sets the glyph's buffer into the bitmap's buffer.
 *
 * @param[in, out] data Struct which contains the glyph's data and the bitmap's data.
 * @param[in] position The position of the glyph.
 * @param[in] color The color of the glyph.
 * @param[in] style The style of the text.
 * @param[in] pixelFormat The format of the pixel in the image that the text is rendered as (i.e. either Pixel::BGRA8888 or Pixel::L8).
 */
void TypesetGlyph(GlyphData& __restrict__ data,
                  const Vector2* const __restrict__ position,
                  const Vector4* const __restrict__ color,
                  const Typesetter::Style style,
                  const Pixel::Format     pixelFormat)
{
  if((0u == data.glyphBitmap.width) || (0u == data.glyphBitmap.height))
  {
    // Nothing to do if the width or height of the buffer is zero.
    return;
  }

  // Initial vertical / horizontal offset.
  const int32_t yOffset = data.verticalOffset + position->y;
  const int32_t xOffset = data.horizontalOffset + position->x;

  // Whether the given glyph is a color one.
  const bool     isColorGlyph    = data.glyphBitmap.isColorEmoji || data.glyphBitmap.isColorBitmap;
  const uint32_t glyphPixelSize  = Pixel::GetBytesPerPixel(data.glyphBitmap.format);
  const uint32_t glyphAlphaIndex = glyphPixelSize - 1u;

  // Determinate iterator range.
  const int32_t lineIndexRangeMin = std::max(0, -yOffset);
  const int32_t lineIndexRangeMax = std::min(static_cast<int32_t>(data.glyphBitmap.height), static_cast<int32_t>(data.height) - yOffset);
  const int32_t indexRangeMin     = std::max(0, -xOffset);
  const int32_t indexRangeMax     = std::min(static_cast<int32_t>(data.glyphBitmap.width), static_cast<int32_t>(data.width) - xOffset);

  // If current glyph don't need to be rendered, just ignore.
  if(lineIndexRangeMax <= lineIndexRangeMin || indexRangeMax <= indexRangeMin)
  {
    return;
  }

  if(Pixel::RGBA8888 == pixelFormat)
  {
    uint32_t* __restrict__ bitmapBuffer = reinterpret_cast<uint32_t*>(data.bitmapBuffer.GetBuffer());
    // Skip basic line.
    bitmapBuffer += (lineIndexRangeMin + yOffset) * static_cast<int32_t>(data.width);

    // Fast-cut if style is MASK or OUTLINE. Outline not shown for color glyph.
    // Just overwrite transparent color and return.
    if(isColorGlyph && (Typesetter::STYLE_MASK == style || Typesetter::STYLE_OUTLINE == style))
    {
      for(int32_t lineIndex = lineIndexRangeMin; lineIndex < lineIndexRangeMax; ++lineIndex)
      {
        // We can use memset here.
        memset(bitmapBuffer + xOffset + indexRangeMin, 0, (indexRangeMax - indexRangeMin) * sizeof(uint32_t));
        bitmapBuffer += data.width;
      }
      return;
    }

    const bool swapChannelsBR = Pixel::BGRA8888 == data.glyphBitmap.format;

    // Offset byte value of glyph bitmap.
    uint32_t glyphOffet = 0u;

    // Allocate scanline memory for glyph bitmap if we need.
    const bool useLocalScanline         = data.glyphBitmap.compressionType != TextAbstraction::FontClient::GlyphBufferData::CompressionType::NO_COMPRESSION;
    uint8_t* __restrict__ glyphScanline = useLocalScanline ? (uint8_t*)malloc(data.glyphBitmap.width * glyphPixelSize) : data.glyphBitmap.buffer;

    // Precalculate input color's packed result.
    uint32_t packedInputColor                    = 0u;
    uint8_t* __restrict__ packedInputColorBuffer = reinterpret_cast<uint8_t*>(&packedInputColor);

    *(packedInputColorBuffer + 3u) = static_cast<uint8_t>(color->a * 255);
    *(packedInputColorBuffer + 2u) = static_cast<uint8_t>(color->b * 255);
    *(packedInputColorBuffer + 1u) = static_cast<uint8_t>(color->g * 255);
    *(packedInputColorBuffer)      = static_cast<uint8_t>(color->r * 255);

    // Skip basic line of glyph.
    if(useLocalScanline)
    {
      for(int32_t lineIndex = 0; lineIndex < lineIndexRangeMin; ++lineIndex)
      {
        TextAbstraction::FontClient::GlyphBufferData::DecompressScanline(data.glyphBitmap, glyphScanline, glyphOffet);
      }
    }
    else
    {
      glyphScanline += lineIndexRangeMin * static_cast<int32_t>(data.glyphBitmap.width * glyphPixelSize);
    }

    // Traverse the pixels of the glyph line per line.
    if(isColorGlyph)
    {
      for(int32_t lineIndex = lineIndexRangeMin; lineIndex < lineIndexRangeMax; ++lineIndex)
      {
        if(useLocalScanline)
        {
          TextAbstraction::FontClient::GlyphBufferData::DecompressScanline(data.glyphBitmap, glyphScanline, glyphOffet);
        }

        for(int32_t index = indexRangeMin; index < indexRangeMax; ++index)
        {
          const int32_t xOffsetIndex = xOffset + index;

          // Retrieves the color from the color glyph.
          uint32_t packedColorGlyph                    = *(reinterpret_cast<const uint32_t*>(glyphScanline + (index << 2)));
          uint8_t* __restrict__ packedColorGlyphBuffer = reinterpret_cast<uint8_t*>(&packedColorGlyph);

          // Update the alpha channel.
          const uint8_t colorAlpha       = MultiplyAndNormalizeColor(*(packedInputColorBuffer + 3u), *(packedColorGlyphBuffer + 3u));
          *(packedColorGlyphBuffer + 3u) = colorAlpha;

          if(Typesetter::STYLE_SHADOW == style)
          {
            // The shadow of color glyph needs to have the shadow color.
            *(packedColorGlyphBuffer + 2u) = MultiplyAndNormalizeColor(*(packedInputColorBuffer + 2u), colorAlpha);
            *(packedColorGlyphBuffer + 1u) = MultiplyAndNormalizeColor(*(packedInputColorBuffer + 1u), colorAlpha);
            *packedColorGlyphBuffer        = MultiplyAndNormalizeColor(*packedInputColorBuffer, colorAlpha);
          }
          else
          {
            if(swapChannelsBR)
            {
              std::swap(*packedColorGlyphBuffer, *(packedColorGlyphBuffer + 2u)); // Swap B and R.
            }

            *(packedColorGlyphBuffer + 2u) = MultiplyAndNormalizeColor(*(packedColorGlyphBuffer + 2u), colorAlpha);
            *(packedColorGlyphBuffer + 1u) = MultiplyAndNormalizeColor(*(packedColorGlyphBuffer + 1u), colorAlpha);
            *packedColorGlyphBuffer        = MultiplyAndNormalizeColor(*packedColorGlyphBuffer, colorAlpha);

            if(data.glyphBitmap.isColorBitmap)
            {
              *(packedColorGlyphBuffer + 2u) = MultiplyAndNormalizeColor(*(packedInputColorBuffer + 2u), *(packedColorGlyphBuffer + 2u));
              *(packedColorGlyphBuffer + 1u) = MultiplyAndNormalizeColor(*(packedInputColorBuffer + 1u), *(packedColorGlyphBuffer + 1u));
              *packedColorGlyphBuffer        = MultiplyAndNormalizeColor(*packedInputColorBuffer, *packedColorGlyphBuffer);
            }
          }

          // Set the color into the final pixel buffer.
          *(bitmapBuffer + xOffsetIndex) = packedColorGlyph;
        }

        bitmapBuffer += data.width;
        if(!useLocalScanline)
        {
          glyphScanline += data.glyphBitmap.width * glyphPixelSize;
        }
      }
    }
    else
    {
      for(int32_t lineIndex = lineIndexRangeMin; lineIndex < lineIndexRangeMax; ++lineIndex)
      {
        if(useLocalScanline)
        {
          TextAbstraction::FontClient::GlyphBufferData::DecompressScanline(data.glyphBitmap, glyphScanline, glyphOffet);
        }

        for(int32_t index = indexRangeMin; index < indexRangeMax; ++index)
        {
          // Update the alpha channel.
          const uint8_t alpha = *(glyphScanline + index * glyphPixelSize + glyphAlphaIndex);

          // Copy non-transparent pixels only
          if(alpha > 0u)
          {
            const int32_t xOffsetIndex = xOffset + index;

            // Check alpha of overlapped pixels
            uint32_t& currentColor             = *(bitmapBuffer + xOffsetIndex);
            uint8_t*  packedCurrentColorBuffer = reinterpret_cast<uint8_t*>(&currentColor);

            // For any pixel overlapped with the pixel in previous glyphs, make sure we don't
            // overwrite a previous bigger alpha with a smaller alpha (in order to avoid
            // semi-transparent gaps between joint glyphs with overlapped pixels, which could
            // happen, for example, in the RTL text when we copy glyphs from right to left).
            uint8_t currentAlpha = *(packedCurrentColorBuffer + 3u);
            currentAlpha         = std::max(currentAlpha, alpha);
            if(currentAlpha == 255)
            {
              // Fast-cut to avoid float type operation.
              currentColor = packedInputColor;
            }
            else
            {
              // Pack the given color into a 32bit buffer. The alpha channel will be updated later for each pixel.
              // The format is RGBA8888.
              uint32_t packedColor                    = 0u;
              uint8_t* __restrict__ packedColorBuffer = reinterpret_cast<uint8_t*>(&packedColor);

              // Color is pre-muliplied with its alpha.
              *(packedColorBuffer + 3u) = MultiplyAndNormalizeColor(*(packedInputColorBuffer + 3u), currentAlpha);
              *(packedColorBuffer + 2u) = MultiplyAndNormalizeColor(*(packedInputColorBuffer + 2u), currentAlpha);
              *(packedColorBuffer + 1u) = MultiplyAndNormalizeColor(*(packedInputColorBuffer + 1u), currentAlpha);
              *(packedColorBuffer)      = MultiplyAndNormalizeColor(*packedInputColorBuffer, currentAlpha);

              // Set the color into the final pixel buffer.
              currentColor = packedColor;
            }
          }
        }

        bitmapBuffer += data.width;
        if(!useLocalScanline)
        {
          glyphScanline += data.glyphBitmap.width * glyphPixelSize;
        }
      }
    }

    if(useLocalScanline)
    {
      free(glyphScanline);
    }
  }
  else // Pixel::L8
  {
    // Below codes required only if not color glyph.
    if(!isColorGlyph)
    {
      uint8_t* __restrict__ bitmapBuffer = data.bitmapBuffer.GetBuffer();

      // Offset byte value of glyph bitmap.
      uint32_t glyphOffet = 0u;

      // Allocate scanline memory for glyph bitmap if we need.
      const bool useLocalScanline         = data.glyphBitmap.compressionType != TextAbstraction::FontClient::GlyphBufferData::CompressionType::NO_COMPRESSION;
      uint8_t* __restrict__ glyphScanline = useLocalScanline ? (uint8_t*)malloc(data.glyphBitmap.width * glyphPixelSize) : data.glyphBitmap.buffer;

      // Skip basic line.
      bitmapBuffer += (lineIndexRangeMin + yOffset) * static_cast<int32_t>(data.width);

      // Skip basic line of glyph.
      if(useLocalScanline)
      {
        for(int32_t lineIndex = 0; lineIndex < lineIndexRangeMin; ++lineIndex)
        {
          TextAbstraction::FontClient::GlyphBufferData::DecompressScanline(data.glyphBitmap, glyphScanline, glyphOffet);
        }
      }
      else
      {
        glyphScanline += lineIndexRangeMin * static_cast<int32_t>(data.glyphBitmap.width * glyphPixelSize);
      }

      // Traverse the pixels of the glyph line per line.
      for(int32_t lineIndex = lineIndexRangeMin; lineIndex < lineIndexRangeMax; ++lineIndex)
      {
        if(useLocalScanline)
        {
          TextAbstraction::FontClient::GlyphBufferData::DecompressScanline(data.glyphBitmap, glyphScanline, glyphOffet);
        }

        for(int32_t index = indexRangeMin; index < indexRangeMax; ++index)
        {
          const int32_t xOffsetIndex = xOffset + index;

          // Update the alpha channel.
          const uint8_t alpha = *(glyphScanline + index * glyphPixelSize + glyphAlphaIndex);

          // Copy non-transparent pixels only
          if(alpha > 0u)
          {
            // Check alpha of overlapped pixels
            uint8_t& currentAlpha = *(bitmapBuffer + xOffsetIndex);

            // For any pixel overlapped with the pixel in previous glyphs, make sure we don't
            // overwrite a previous bigger alpha with a smaller alpha (in order to avoid
            // semi-transparent gaps between joint glyphs with overlapped pixels, which could
            // happen, for example, in the RTL text when we copy glyphs from right to left).
            currentAlpha = std::max(currentAlpha, alpha);
          }
        }

        bitmapBuffer += data.width;
        if(!useLocalScanline)
        {
          glyphScanline += data.glyphBitmap.width * glyphPixelSize;
        }
      }

      if(useLocalScanline)
      {
        free(glyphScanline);
      }
    }
  }
}

/// Draws the specified underline color to the buffer
void DrawUnderline(
  const uint32_t                  bufferWidth,
  const uint32_t                  bufferHeight,
  GlyphData&                      glyphData,
  const float                     baseline,
  const float                     currentUnderlinePosition,
  const float                     maxUnderlineHeight,
  const float                     lineExtentLeft,
  const float                     lineExtentRight,
  const UnderlineStyleProperties& commonUnderlineProperties,
  const UnderlineStyleProperties& currentUnderlineProperties,
  const LineRun&                  line)
{
  const Vector4&              underlineColor       = currentUnderlineProperties.colorDefined ? currentUnderlineProperties.color : commonUnderlineProperties.color;
  const Text::Underline::Type underlineType        = currentUnderlineProperties.typeDefined ? currentUnderlineProperties.type : commonUnderlineProperties.type;
  const float                 dashedUnderlineWidth = currentUnderlineProperties.dashWidthDefined ? currentUnderlineProperties.dashWidth : commonUnderlineProperties.dashWidth;
  const float                 dashedUnderlineGap   = currentUnderlineProperties.dashGapDefined ? currentUnderlineProperties.dashGap : commonUnderlineProperties.dashGap;

  int32_t underlineYOffset = glyphData.verticalOffset + baseline + currentUnderlinePosition;

  const uint32_t yRangeMin = underlineYOffset;
  const uint32_t yRangeMax = std::min(bufferHeight, underlineYOffset + static_cast<uint32_t>(maxUnderlineHeight));
  const uint32_t xRangeMin = static_cast<uint32_t>(glyphData.horizontalOffset + lineExtentLeft);
  const uint32_t xRangeMax = std::min(bufferWidth, static_cast<uint32_t>(glyphData.horizontalOffset + lineExtentRight + 1)); // Due to include last point, we add 1 here

  // If current glyph don't need to be rendered, just ignore.
  if((underlineType != Text::Underline::DOUBLE && yRangeMax <= yRangeMin) || xRangeMax <= xRangeMin)
  {
    return;
  }

  // We can optimize by memset when underlineColor.a is near zero
  uint8_t underlineColorAlpha = static_cast<uint8_t>(underlineColor.a * 255.f);

  uint32_t* bitmapBuffer = reinterpret_cast<uint32_t*>(glyphData.bitmapBuffer.GetBuffer());

  // Skip yRangeMin line.
  bitmapBuffer += yRangeMin * glyphData.width;

  // Note if underlineType is DASHED, we cannot setup color by memset.
  if(underlineType != Text::Underline::DASHED && underlineColorAlpha == 0)
  {
    for(uint32_t y = yRangeMin; y < yRangeMax; y++)
    {
      // We can use memset.
      memset(bitmapBuffer + xRangeMin, 0, (xRangeMax - xRangeMin) * sizeof(uint32_t));
      bitmapBuffer += glyphData.width;
    }
    if(underlineType == Text::Underline::DOUBLE)
    {
      int32_t        secondUnderlineYOffset = underlineYOffset - ONE_AND_A_HALF * maxUnderlineHeight;
      const uint32_t secondYRangeMin        = static_cast<uint32_t>(std::max(0, secondUnderlineYOffset));
      const uint32_t secondYRangeMax        = static_cast<uint32_t>(std::max(0, std::min(static_cast<int32_t>(bufferHeight), secondUnderlineYOffset + static_cast<int32_t>(maxUnderlineHeight))));

      // Rewind bitmapBuffer pointer, and skip secondYRangeMin line.
      bitmapBuffer = reinterpret_cast<uint32_t*>(glyphData.bitmapBuffer.GetBuffer()) + yRangeMin * glyphData.width;

      for(uint32_t y = secondYRangeMin; y < secondYRangeMax; y++)
      {
        // We can use memset.
        memset(bitmapBuffer + xRangeMin, 0, (xRangeMax - xRangeMin) * sizeof(uint32_t));
        bitmapBuffer += glyphData.width;
      }
    }
  }
  else
  {
    uint32_t packedUnderlineColor       = 0u;
    uint8_t* packedUnderlineColorBuffer = reinterpret_cast<uint8_t*>(&packedUnderlineColor);

    // Write the color to the pixel buffer
    *(packedUnderlineColorBuffer + 3u) = underlineColorAlpha;
    *(packedUnderlineColorBuffer + 2u) = static_cast<uint8_t>(underlineColor.b * underlineColorAlpha);
    *(packedUnderlineColorBuffer + 1u) = static_cast<uint8_t>(underlineColor.g * underlineColorAlpha);
    *(packedUnderlineColorBuffer)      = static_cast<uint8_t>(underlineColor.r * underlineColorAlpha);

    for(uint32_t y = yRangeMin; y < yRangeMax; y++)
    {
      if(underlineType == Text::Underline::DASHED)
      {
        float dashWidth = dashedUnderlineWidth;
        float dashGap   = 0;

        for(uint32_t x = xRangeMin; x < xRangeMax; x++)
        {
          if(dashGap == 0 && dashWidth > 0)
          {
            // Note : this is same logic as bitmap[y][x] = underlineColor;
            *(bitmapBuffer + x) = packedUnderlineColor;
            dashWidth--;
          }
          else if(dashGap < dashedUnderlineGap)
          {
            dashGap++;
          }
          else
          {
            //reset
            dashWidth = dashedUnderlineWidth;
            dashGap   = 0;
          }
        }
      }
      else
      {
        for(uint32_t x = xRangeMin; x < xRangeMax; x++)
        {
          // Note : this is same logic as bitmap[y][x] = underlineColor;
          *(bitmapBuffer + x) = packedUnderlineColor;
        }
      }
      bitmapBuffer += glyphData.width;
    }
    if(underlineType == Text::Underline::DOUBLE)
    {
      int32_t        secondUnderlineYOffset = underlineYOffset - ONE_AND_A_HALF * maxUnderlineHeight;
      const uint32_t secondYRangeMin        = static_cast<uint32_t>(std::max(0, secondUnderlineYOffset));
      const uint32_t secondYRangeMax        = static_cast<uint32_t>(std::max(0, std::min(static_cast<int32_t>(bufferHeight), secondUnderlineYOffset + static_cast<int32_t>(maxUnderlineHeight))));

      // Rewind bitmapBuffer pointer, and skip secondYRangeMin line.
      bitmapBuffer = reinterpret_cast<uint32_t*>(glyphData.bitmapBuffer.GetBuffer()) + yRangeMin * glyphData.width;

      for(uint32_t y = secondYRangeMin; y < secondYRangeMax; y++)
      {
        for(uint32_t x = xRangeMin; x < xRangeMax; x++)
        {
          // Note : this is same logic as bitmap[y][x] = underlineColor;
          *(bitmapBuffer + x) = packedUnderlineColor;
        }
        bitmapBuffer += glyphData.width;
      }
    }
  }
}

/// Draws the background color to the buffer
void DrawBackgroundColor(
  Vector4        backgroundColor,
  const uint32_t bufferWidth,
  const uint32_t bufferHeight,
  GlyphData&     glyphData,
  const float    baseline,
  const LineRun& line,
  const float    lineExtentLeft,
  const float    lineExtentRight)
{
  const int32_t yRangeMin = std::max(0, static_cast<int32_t>(glyphData.verticalOffset + baseline - line.ascender));
  const int32_t yRangeMax = std::min(static_cast<int32_t>(bufferHeight), static_cast<int32_t>(glyphData.verticalOffset + baseline - line.descender));
  const int32_t xRangeMin = std::max(0, static_cast<int32_t>(glyphData.horizontalOffset + lineExtentLeft));
  const int32_t xRangeMax = std::min(static_cast<int32_t>(bufferWidth), static_cast<int32_t>(glyphData.horizontalOffset + lineExtentRight + 1)); // Due to include last point, we add 1 here

  // If current glyph don't need to be rendered, just ignore.
  if(yRangeMax <= yRangeMin || xRangeMax <= xRangeMin)
  {
    return;
  }

  // We can optimize by memset when backgroundColor.a is near zero
  uint8_t backgroundColorAlpha = static_cast<uint8_t>(backgroundColor.a * 255.f);

  uint32_t* bitmapBuffer = reinterpret_cast<uint32_t*>(glyphData.bitmapBuffer.GetBuffer());

  // Skip yRangeMin line.
  bitmapBuffer += yRangeMin * glyphData.width;

  if(backgroundColorAlpha == 0)
  {
    for(int32_t y = yRangeMin; y < yRangeMax; y++)
    {
      // We can use memset.
      memset(bitmapBuffer + xRangeMin, 0, (xRangeMax - xRangeMin) * sizeof(uint32_t));
      bitmapBuffer += glyphData.width;
    }
  }
  else
  {
    uint32_t packedBackgroundColor       = 0u;
    uint8_t* packedBackgroundColorBuffer = reinterpret_cast<uint8_t*>(&packedBackgroundColor);

    // Write the color to the pixel buffer
    *(packedBackgroundColorBuffer + 3u) = backgroundColorAlpha;
    *(packedBackgroundColorBuffer + 2u) = static_cast<uint8_t>(backgroundColor.b * backgroundColorAlpha);
    *(packedBackgroundColorBuffer + 1u) = static_cast<uint8_t>(backgroundColor.g * backgroundColorAlpha);
    *(packedBackgroundColorBuffer)      = static_cast<uint8_t>(backgroundColor.r * backgroundColorAlpha);

    for(int32_t y = yRangeMin; y < yRangeMax; y++)
    {
      for(int32_t x = xRangeMin; x < xRangeMax; x++)
      {
        // Note : this is same logic as bitmap[y][x] = backgroundColor;
        *(bitmapBuffer + x) = packedBackgroundColor;
      }
      bitmapBuffer += glyphData.width;
    }
  }
}

Devel::PixelBuffer DrawGlyphsBackground(const ViewModel* model, Devel::PixelBuffer& buffer, const uint32_t bufferWidth, const uint32_t bufferHeight, const bool ignoreHorizontalAlignment, const int32_t horizontalOffset, const int32_t verticalOffset)
{
  // Retrieve lines, glyphs, positions and colors from the view model.
  const Length            modelNumberOfLines           = model->GetNumberOfLines();
  const LineRun* const    modelLinesBuffer             = model->GetLines();
  const Length            numberOfGlyphs               = model->GetNumberOfGlyphs();
  const GlyphInfo* const  glyphsBuffer                 = model->GetGlyphs();
  const Vector2* const    positionBuffer               = model->GetLayout();
  const Vector4* const    backgroundColorsBuffer       = model->GetBackgroundColors();
  const ColorIndex* const backgroundColorIndicesBuffer = model->GetBackgroundColorIndices();

  const DevelText::VerticalLineAlignment::Type verLineAlign = model->GetVerticalLineAlignment();

  // Create and initialize the pixel buffer.
  GlyphData glyphData;
  glyphData.verticalOffset   = verticalOffset;
  glyphData.width            = bufferWidth;
  glyphData.height           = bufferHeight;
  glyphData.bitmapBuffer     = buffer;
  glyphData.horizontalOffset = 0;

  ColorIndex prevBackgroundColorIndex = 0;
  ColorIndex backgroundColorIndex     = 0;

  // Traverses the lines of the text.
  for(LineIndex lineIndex = 0u; lineIndex < modelNumberOfLines; ++lineIndex)
  {
    const LineRun& line = *(modelLinesBuffer + lineIndex);

    // Sets the horizontal offset of the line.
    glyphData.horizontalOffset = ignoreHorizontalAlignment ? 0 : static_cast<int32_t>(line.alignmentOffset);
    glyphData.horizontalOffset += horizontalOffset;

    // Increases the vertical offset with the line's ascender.
    glyphData.verticalOffset += static_cast<int32_t>(line.ascender + GetPreOffsetVerticalLineAlignment(line, verLineAlign));

    float left     = bufferWidth;
    float right    = 0.0f;
    float baseline = 0.0f;

    // Traverses the glyphs of the line.
    const GlyphIndex endGlyphIndex = std::min(numberOfGlyphs, line.glyphRun.glyphIndex + line.glyphRun.numberOfGlyphs);
    for(GlyphIndex glyphIndex = line.glyphRun.glyphIndex; glyphIndex < endGlyphIndex; ++glyphIndex)
    {
      // Retrieve the glyph's info.
      const GlyphInfo* const glyphInfo = glyphsBuffer + glyphIndex;

      if((glyphInfo->width < Math::MACHINE_EPSILON_1000) ||
         (glyphInfo->height < Math::MACHINE_EPSILON_1000))
      {
        // Nothing to do if default background color, the glyph's width or height is zero.
        continue;
      }

      backgroundColorIndex = (nullptr == backgroundColorsBuffer) ? 0u : *(backgroundColorIndicesBuffer + glyphIndex);

      if((backgroundColorIndex != prevBackgroundColorIndex) &&
         (prevBackgroundColorIndex != 0u))
      {
        const Vector4& backgroundColor = *(backgroundColorsBuffer + prevBackgroundColorIndex - 1u);
        DrawBackgroundColor(backgroundColor, bufferWidth, bufferHeight, glyphData, baseline, line, left, right);
      }

      if(backgroundColorIndex == 0u)
      {
        prevBackgroundColorIndex = backgroundColorIndex;
        //if background color is the default do nothing
        continue;
      }

      // Retrieves the glyph's position.
      const Vector2* const position = positionBuffer + glyphIndex;

      if(baseline < position->y + glyphInfo->yBearing)
      {
        baseline = position->y + glyphInfo->yBearing;
      }

      // Calculate the positions of leftmost and rightmost glyphs in the current line
      if((position->x < left) || (backgroundColorIndex != prevBackgroundColorIndex))
      {
        left = position->x - glyphInfo->xBearing;
      }

      if(position->x + glyphInfo->width > right)
      {
        right = position->x - glyphInfo->xBearing + glyphInfo->advance;
      }

      prevBackgroundColorIndex = backgroundColorIndex;
    }

    //draw last background at line end if not default
    if(backgroundColorIndex != 0u)
    {
      const Vector4& backgroundColor = *(backgroundColorsBuffer + backgroundColorIndex - 1u);
      DrawBackgroundColor(backgroundColor, bufferWidth, bufferHeight, glyphData, baseline, line, left, right);
    }

    // Increases the vertical offset with the line's descender.
    glyphData.verticalOffset += static_cast<int32_t>(-line.descender + GetPostOffsetVerticalLineAlignment(line, verLineAlign));
  }

  return glyphData.bitmapBuffer;
}

/// Draws the specified strikethrough color to the buffer
void DrawStrikethrough(const uint32_t                      bufferWidth,
                       const uint32_t                      bufferHeight,
                       GlyphData&                          glyphData,
                       const float                         baseline,
                       const float                         strikethroughStartingYPosition,
                       const float                         maxStrikethroughHeight,
                       const float                         lineExtentLeft,
                       const float                         lineExtentRight,
                       const StrikethroughStyleProperties& commonStrikethroughProperties,
                       const StrikethroughStyleProperties& currentStrikethroughProperties,
                       const LineRun&                      line)
{
  const Vector4& strikethroughColor = currentStrikethroughProperties.colorDefined ? currentStrikethroughProperties.color : commonStrikethroughProperties.color;

  const uint32_t yRangeMin = static_cast<uint32_t>(strikethroughStartingYPosition);
  const uint32_t yRangeMax = std::min(bufferHeight, static_cast<uint32_t>(strikethroughStartingYPosition + maxStrikethroughHeight));
  const uint32_t xRangeMin = static_cast<uint32_t>(glyphData.horizontalOffset + lineExtentLeft);
  const uint32_t xRangeMax = std::min(bufferWidth, static_cast<uint32_t>(glyphData.horizontalOffset + lineExtentRight + 1)); // Due to include last point, we add 1 here

  // If current glyph don't need to be rendered, just ignore.
  if(yRangeMax <= yRangeMin || xRangeMax <= xRangeMin)
  {
    return;
  }

  // We can optimize by memset when strikethroughColor.a is near zero
  uint8_t strikethroughColorAlpha = static_cast<uint8_t>(strikethroughColor.a * 255.f);

  uint32_t* bitmapBuffer = reinterpret_cast<uint32_t*>(glyphData.bitmapBuffer.GetBuffer());

  // Skip yRangeMin line.
  bitmapBuffer += yRangeMin * glyphData.width;

  if(strikethroughColorAlpha == 0)
  {
    for(uint32_t y = yRangeMin; y < yRangeMax; y++)
    {
      // We can use memset.
      memset(bitmapBuffer + xRangeMin, 0, (xRangeMax - xRangeMin) * sizeof(uint32_t));
      bitmapBuffer += glyphData.width;
    }
  }
  else
  {
    uint32_t packedStrikethroughColor       = 0u;
    uint8_t* packedStrikethroughColorBuffer = reinterpret_cast<uint8_t*>(&packedStrikethroughColor);

    // Write the color to the pixel buffer
    *(packedStrikethroughColorBuffer + 3u) = strikethroughColorAlpha;
    *(packedStrikethroughColorBuffer + 2u) = static_cast<uint8_t>(strikethroughColor.b * strikethroughColorAlpha);
    *(packedStrikethroughColorBuffer + 1u) = static_cast<uint8_t>(strikethroughColor.g * strikethroughColorAlpha);
    *(packedStrikethroughColorBuffer)      = static_cast<uint8_t>(strikethroughColor.r * strikethroughColorAlpha);

    for(uint32_t y = yRangeMin; y < yRangeMax; y++)
    {
      for(uint32_t x = xRangeMin; x < xRangeMax; x++)
      {
        // Note : this is same logic as bitmap[y][x] = strikethroughColor;
        *(bitmapBuffer + x) = packedStrikethroughColor;
      }
      bitmapBuffer += glyphData.width;
    }
  }
}

/**
 * @brief Create an initialized image buffer filled with transparent color.
 *
 * Creates the pixel data used to generate the final image with the given size.
 *
 * @param[in] bufferWidth The width of the image buffer.
 * @param[in] bufferHeight The height of the image buffer.
 * @param[in] pixelFormat The format of the pixel in the image that the text is rendered as (i.e. either Pixel::BGRA8888 or Pixel::L8).
 *
 * @return An image buffer.
 */
inline Devel::PixelBuffer CreateTransparentImageBuffer(const uint32_t bufferWidth, const uint32_t bufferHeight, const Pixel::Format pixelFormat)
{
  Devel::PixelBuffer imageBuffer = Devel::PixelBuffer::New(bufferWidth, bufferHeight, pixelFormat);

  if(Pixel::RGBA8888 == pixelFormat)
  {
    const uint32_t bufferSizeInt  = bufferWidth * bufferHeight;
    const uint32_t bufferSizeChar = sizeof(uint32_t) * bufferSizeInt;
    memset(imageBuffer.GetBuffer(), 0, bufferSizeChar);
  }
  else
  {
    memset(imageBuffer.GetBuffer(), 0, bufferWidth * bufferHeight);
  }

  return imageBuffer;
}

/**
 * @brief Combine the two RGBA image buffers together.
 *
 * The top layer buffer will blend over the bottom layer buffer:
 * - If the pixel is not fully opaque from either buffer, it will be blended with
 *   the pixel from the other buffer and copied to the combined buffer.
 * - If the pixels from both buffers are fully opaque, the pixels from the top layer
 *   buffer will be copied to the combined buffer.
 *
 * Due to the performance issue, We need to re-use input'ed pixelBuffer memory.
 * We can determine which pixelBuffer's memory is destination
 *
 * @param[in, out] topPixelBuffer The top layer buffer.
 * @param[in, out] bottomPixelBuffer The bottom layer buffer.
 * @param[in] bufferWidth The width of the image buffer.
 * @param[in] bufferHeight The height of the image buffer.
 * @param[in] storeResultIntoTop True if we store the combined image buffer result into topPixelBuffer.
 * False if we store the combined image buffer result into bottomPixelBuffer.
 *
 */
void CombineImageBuffer(Devel::PixelBuffer& __restrict__ topPixelBuffer, Devel::PixelBuffer& __restrict__ bottomPixelBuffer, const uint32_t bufferWidth, const uint32_t bufferHeight, bool storeResultIntoTop)
{
  // Assume that we always combine two RGBA images
  // Jump with 4bytes for optimize runtime.
  uint32_t* topBuffer    = reinterpret_cast<uint32_t*>(topPixelBuffer.GetBuffer());
  uint32_t* bottomBuffer = reinterpret_cast<uint32_t*>(bottomPixelBuffer.GetBuffer());

  if(topBuffer == NULL && bottomBuffer == NULL)
  {
    // Nothing to do if both buffers are empty.
    return;
  }

  if(topBuffer == NULL)
  {
    // Nothing to do if topBuffer is empty.
    // If we need to store the result into top, change topPixelBuffer as bottomPixelBuffer.
    if(storeResultIntoTop)
    {
      topPixelBuffer = bottomPixelBuffer;
    }
    return;
  }

  if(bottomBuffer == NULL)
  {
    // Nothing to do if bottomBuffer is empty.
    // If we need to store the result into bottom, change bottomPixelBuffer as topPixelBuffer.
    if(!storeResultIntoTop)
    {
      bottomPixelBuffer = topPixelBuffer;
    }
    return;
  }

  const uint32_t bufferSizeInt = bufferWidth * bufferHeight;

  uint32_t* __restrict__ combinedBuffer       = storeResultIntoTop ? topBuffer : bottomBuffer;
  uint8_t* __restrict__ topAlphaBufferPointer = reinterpret_cast<uint8_t*>(topBuffer) + 3;

  for(uint32_t pixelIndex = 0; pixelIndex < bufferSizeInt; ++pixelIndex)
  {
    // If the alpha of the pixel in either buffer is not fully opaque, blend the two pixels.
    // Otherwise, copy pixel from topBuffer to combinedBuffer.
    // Note : Be careful when we read & write into combinedBuffer. It can be write into same pointer.

    uint8_t topAlpha = *topAlphaBufferPointer;

    if(topAlpha == 0)
    {
      // Copy the pixel from bottomBuffer to combinedBuffer
      if(storeResultIntoTop)
      {
        *(combinedBuffer) = *(bottomBuffer);
      }
    }
    else if(topAlpha == 255)
    {
      // Copy the pixel from topBuffer to combinedBuffer
      if(!storeResultIntoTop)
      {
        *(combinedBuffer) = *(topBuffer);
      }
    }
    else
    {
      // At least one pixel is not fully opaque
      // "Over" blend the the pixel from topBuffer with the pixel in bottomBuffer
      uint32_t blendedBottomBufferColor                    = *(bottomBuffer);
      uint8_t* __restrict__ blendedBottomBufferColorBuffer = reinterpret_cast<uint8_t*>(&blendedBottomBufferColor);

      blendedBottomBufferColorBuffer[0] = MultiplyAndNormalizeColor(blendedBottomBufferColorBuffer[0], 255 - topAlpha);
      blendedBottomBufferColorBuffer[1] = MultiplyAndNormalizeColor(blendedBottomBufferColorBuffer[1], 255 - topAlpha);
      blendedBottomBufferColorBuffer[2] = MultiplyAndNormalizeColor(blendedBottomBufferColorBuffer[2], 255 - topAlpha);
      blendedBottomBufferColorBuffer[3] = MultiplyAndNormalizeColor(blendedBottomBufferColorBuffer[3], 255 - topAlpha);

      *(combinedBuffer) = *(topBuffer) + blendedBottomBufferColor;
    }

    // Increase each buffer's pointer.
    ++combinedBuffer;
    ++topBuffer;
    ++bottomBuffer;
    topAlphaBufferPointer += sizeof(uint32_t) / sizeof(uint8_t);
  }
}

} // namespace

TypesetterPtr Typesetter::New(const ModelInterface* const model)
{
  return TypesetterPtr(new Typesetter(model));
}

ViewModel* Typesetter::GetViewModel()
{
  return mModel;
}

PixelData Typesetter::Render(const Vector2& size, Toolkit::DevelText::TextDirection::Type textDirection, RenderBehaviour behaviour, bool ignoreHorizontalAlignment, Pixel::Format pixelFormat)
{
  // @todo. This initial implementation for a TextLabel has only one visible page.

  // Elides the text if needed.
  mModel->ElideGlyphs();

  // Retrieves the layout size.
  const Size& layoutSize = mModel->GetLayoutSize();

  const int32_t outlineWidth = static_cast<int32_t>(mModel->GetOutlineWidth());

  // Set the offset for the horizontal alignment according to the text direction and outline width.
  int32_t penX = 0;

  switch(mModel->GetHorizontalAlignment())
  {
    case HorizontalAlignment::BEGIN:
    {
      // No offset to add.
      break;
    }
    case HorizontalAlignment::CENTER:
    {
      penX += (textDirection == Toolkit::DevelText::TextDirection::LEFT_TO_RIGHT) ? -outlineWidth : outlineWidth;
      break;
    }
    case HorizontalAlignment::END:
    {
      penX += (textDirection == Toolkit::DevelText::TextDirection::LEFT_TO_RIGHT) ? -outlineWidth * 2 : outlineWidth * 2;
      break;
    }
  }

  // Set the offset for the vertical alignment.
  int32_t penY = 0u;

  switch(mModel->GetVerticalAlignment())
  {
    case VerticalAlignment::TOP:
    {
      // No offset to add.
      break;
    }
    case VerticalAlignment::CENTER:
    {
      penY = static_cast<int32_t>(0.5f * (size.height - layoutSize.height));
      penY = penY < 0.f ? 0.f : penY;
      break;
    }
    case VerticalAlignment::BOTTOM:
    {
      penY = static_cast<int32_t>(size.height - layoutSize.height);
      break;
    }
  }

  // Generate the image buffers of the text for each different style first,
  // then combine all of them together as one final image buffer. We try to
  // do all of these in CPU only, so that once the final texture is generated,
  // no calculation is needed in GPU during each frame.

  const uint32_t bufferWidth  = static_cast<uint32_t>(size.width);
  const uint32_t bufferHeight = static_cast<uint32_t>(size.height);

  const uint32_t bufferSizeInt  = bufferWidth * bufferHeight;
  const uint32_t bufferSizeChar = sizeof(uint32_t) * bufferSizeInt;

  //Elided text in ellipsis at START could start on index greater than 0
  auto startIndexOfGlyphs = mModel->GetStartIndexOfElidedGlyphs();
  auto endIndexOfGlyphs   = mModel->GetEndIndexOfElidedGlyphs();

  Devel::PixelBuffer imageBuffer;

  if(RENDER_MASK == behaviour)
  {
    // Generate the image buffer as an alpha mask for color glyphs.
    imageBuffer = CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_MASK, ignoreHorizontalAlignment, pixelFormat, penX, penY, startIndexOfGlyphs, endIndexOfGlyphs);
  }
  else if(RENDER_NO_TEXT == behaviour || RENDER_OVERLAY_STYLE == behaviour)
  {
    // Generate an empty image buffer so that it can been combined with the image buffers for styles
    imageBuffer = Devel::PixelBuffer::New(bufferWidth, bufferHeight, Pixel::RGBA8888);
    memset(imageBuffer.GetBuffer(), 0u, bufferSizeChar);
  }
  else
  {
    // Generate the image buffer for the text with no style.
    imageBuffer = CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_NONE, ignoreHorizontalAlignment, pixelFormat, penX, penY, startIndexOfGlyphs, endIndexOfGlyphs);
  }

  if((RENDER_NO_STYLES != behaviour) && (RENDER_MASK != behaviour))
  {
    // Generate the outline if enabled
    const uint16_t outlineWidth = mModel->GetOutlineWidth();
    const float    outlineAlpha = mModel->GetOutlineColor().a;
    if(outlineWidth != 0u && fabsf(outlineAlpha) > Math::MACHINE_EPSILON_1 && RENDER_OVERLAY_STYLE != behaviour)
    {
      // Create the image buffer for outline
      Devel::PixelBuffer outlineImageBuffer = CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_OUTLINE, ignoreHorizontalAlignment, pixelFormat, penX, penY, startIndexOfGlyphs, endIndexOfGlyphs);

      // Combine the two buffers
      CombineImageBuffer(imageBuffer, outlineImageBuffer, bufferWidth, bufferHeight, true);
    }

    // @todo. Support shadow for partial text later on.

    // Generate the shadow if enabled
    const Vector2& shadowOffset = mModel->GetShadowOffset();
    const float    shadowAlpha  = mModel->GetShadowColor().a;
    if(RENDER_OVERLAY_STYLE != behaviour && fabsf(shadowAlpha) > Math::MACHINE_EPSILON_1 && (fabsf(shadowOffset.x) > Math::MACHINE_EPSILON_1 || fabsf(shadowOffset.y) > Math::MACHINE_EPSILON_1))
    {
      // Create the image buffer for shadow
      Devel::PixelBuffer shadowImageBuffer = CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_SHADOW, ignoreHorizontalAlignment, pixelFormat, penX, penY, startIndexOfGlyphs, endIndexOfGlyphs);

      // Check whether it will be a soft shadow
      const float& blurRadius = mModel->GetShadowBlurRadius();

      if(blurRadius > Math::MACHINE_EPSILON_1)
      {
        shadowImageBuffer.ApplyGaussianBlur(blurRadius);
      }

      // Combine the two buffers
      CombineImageBuffer(imageBuffer, shadowImageBuffer, bufferWidth, bufferHeight, true);
    }

    // Generate the background if enabled
    const bool backgroundEnabled   = mModel->IsBackgroundEnabled();
    const bool backgroundMarkupSet = mModel->IsMarkupBackgroundColorSet();
    if((backgroundEnabled || backgroundMarkupSet) && RENDER_OVERLAY_STYLE != behaviour)
    {
      Devel::PixelBuffer backgroundImageBuffer;

      if(backgroundEnabled)
      {
        backgroundImageBuffer = CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_BACKGROUND, ignoreHorizontalAlignment, pixelFormat, penX, penY, startIndexOfGlyphs, endIndexOfGlyphs);
      }
      else
      {
        backgroundImageBuffer = CreateTransparentImageBuffer(bufferWidth, bufferHeight, pixelFormat);
      }

      if(backgroundMarkupSet)
      {
        DrawGlyphsBackground(mModel, backgroundImageBuffer, bufferWidth, bufferHeight, ignoreHorizontalAlignment, penX, penY);
      }

      // Combine the two buffers
      CombineImageBuffer(imageBuffer, backgroundImageBuffer, bufferWidth, bufferHeight, true);
    }

    if(RENDER_OVERLAY_STYLE == behaviour)
    {
      if(mModel->IsUnderlineEnabled())
      {
        // Create the image buffer for underline
        Devel::PixelBuffer underlineImageBuffer = CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_UNDERLINE, ignoreHorizontalAlignment, pixelFormat, penX, penY, startIndexOfGlyphs, endIndexOfGlyphs);

        // Combine the two buffers
        CombineImageBuffer(imageBuffer, underlineImageBuffer, bufferWidth, bufferHeight, true);
      }

      if(mModel->IsStrikethroughEnabled())
      {
        // Create the image buffer for strikethrough
        Devel::PixelBuffer strikethroughImageBuffer = CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_STRIKETHROUGH, ignoreHorizontalAlignment, pixelFormat, penX, penY, 0u, endIndexOfGlyphs);

        // Combine the two buffers
        CombineImageBuffer(imageBuffer, strikethroughImageBuffer, bufferWidth, bufferHeight, true);
      }

      // Markup-Processor for overlay styles
      if(mModel->IsMarkupProcessorEnabled())
      {
        if(mModel->IsMarkupUnderlineSet())
        {
          imageBuffer = ApplyUnderlineMarkupImageBuffer(imageBuffer, bufferWidth, bufferHeight, ignoreHorizontalAlignment, pixelFormat, penX, penY);
        }

        if(mModel->IsMarkupStrikethroughSet())
        {
          imageBuffer = ApplyStrikethroughMarkupImageBuffer(imageBuffer, bufferWidth, bufferHeight, ignoreHorizontalAlignment, pixelFormat, penX, penY);
        }
      }
    }
  }

  // Create the final PixelData for the combined image buffer
  PixelData pixelData = Devel::PixelBuffer::Convert(imageBuffer);

  return pixelData;
}

Devel::PixelBuffer Typesetter::CreateImageBuffer(const uint32_t bufferWidth, const uint32_t bufferHeight, const Typesetter::Style style, const bool ignoreHorizontalAlignment, const Pixel::Format pixelFormat, const int32_t horizontalOffset, const int32_t verticalOffset, const GlyphIndex fromGlyphIndex, const GlyphIndex toGlyphIndex)
{
  // Retrieve lines, glyphs, positions and colors from the view model.
  const Length modelNumberOfLines                       = mModel->GetNumberOfLines();
  const LineRun* const __restrict__ modelLinesBuffer    = mModel->GetLines();
  const GlyphInfo* const __restrict__ glyphsBuffer      = mModel->GetGlyphs();
  const Vector2* const __restrict__ positionBuffer      = mModel->GetLayout();
  const Vector4* const __restrict__ colorsBuffer        = mModel->GetColors();
  const ColorIndex* const __restrict__ colorIndexBuffer = mModel->GetColorIndices();
  const GlyphInfo* __restrict__ hyphens                 = mModel->GetHyphens();
  const Length* __restrict__ hyphenIndices              = mModel->GetHyphenIndices();
  const Length hyphensCount                             = mModel->GetHyphensCount();

  // Elided text info. Indices according to elided text and Ellipsis position.
  const auto startIndexOfGlyphs              = mModel->GetStartIndexOfElidedGlyphs();
  const auto endIndexOfGlyphs                = mModel->GetEndIndexOfElidedGlyphs();
  const auto firstMiddleIndexOfElidedGlyphs  = mModel->GetFirstMiddleIndexOfElidedGlyphs();
  const auto secondMiddleIndexOfElidedGlyphs = mModel->GetSecondMiddleIndexOfElidedGlyphs();
  const auto ellipsisPosition                = mModel->GetEllipsisPosition();

  // Whether to use the default color.
  const bool     useDefaultColor = (NULL == colorsBuffer);
  const Vector4& defaultColor    = mModel->GetDefaultColor();

  // Create and initialize the pixel buffer.
  GlyphData glyphData;
  glyphData.verticalOffset   = verticalOffset;
  glyphData.width            = bufferWidth;
  glyphData.height           = bufferHeight;
  glyphData.bitmapBuffer     = CreateTransparentImageBuffer(bufferWidth, bufferHeight, pixelFormat);
  glyphData.horizontalOffset = 0;

  // Get a handle of the font client. Used to retrieve the bitmaps of the glyphs.
  TextAbstraction::FontClient fontClient  = TextAbstraction::FontClient::Get();
  Length                      hyphenIndex = 0;

  const Character* __restrict__ textBuffer                       = mModel->GetTextBuffer();
  float calculatedAdvance                                        = 0.f;
  const Vector<CharacterIndex>& __restrict__ glyphToCharacterMap = mModel->GetGlyphsToCharacters();
  const CharacterIndex* __restrict__ glyphToCharacterMapBuffer   = glyphToCharacterMap.Begin();

  const DevelText::VerticalLineAlignment::Type verLineAlign = mModel->GetVerticalLineAlignment();

  // Traverses the lines of the text.
  for(LineIndex lineIndex = 0u; lineIndex < modelNumberOfLines; ++lineIndex)
  {
    const LineRun& line = *(modelLinesBuffer + lineIndex);

    // Sets the horizontal offset of the line.
    glyphData.horizontalOffset = ignoreHorizontalAlignment ? 0 : static_cast<int32_t>(line.alignmentOffset);
    glyphData.horizontalOffset += horizontalOffset;

    // Increases the vertical offset with the line's ascender.
    glyphData.verticalOffset += static_cast<int32_t>(line.ascender + GetPreOffsetVerticalLineAlignment(line, verLineAlign));

    // Retrieves the glyph's outline width
    float outlineWidth = static_cast<float>(mModel->GetOutlineWidth());

    if(style == Typesetter::STYLE_OUTLINE)
    {
      glyphData.horizontalOffset -= outlineWidth;
      if(lineIndex == 0u)
      {
        // Only need to add the vertical outline offset for the first line
        glyphData.verticalOffset -= outlineWidth;
      }
    }
    else if(style == Typesetter::STYLE_SHADOW)
    {
      const Vector2& shadowOffset = mModel->GetShadowOffset();
      glyphData.horizontalOffset += shadowOffset.x - outlineWidth; // if outline enabled then shadow should offset from outline

      if(lineIndex == 0u)
      {
        // Only need to add the vertical shadow offset for first line
        glyphData.verticalOffset += shadowOffset.y - outlineWidth;
      }
    }

    const bool  underlineEnabled      = mModel->IsUnderlineEnabled();
    const bool  strikethroughEnabled  = mModel->IsStrikethroughEnabled();
    const float modelCharacterSpacing = mModel->GetCharacterSpacing();

    // Get the character-spacing runs.
    const Vector<CharacterSpacingGlyphRun>& __restrict__ characterSpacingGlyphRuns = mModel->GetCharacterSpacingGlyphRuns();

    // Aggregate underline-style-properties from mModel
    const UnderlineStyleProperties modelUnderlineProperties{mModel->GetUnderlineType(),
                                                            mModel->GetUnderlineColor(),
                                                            mModel->GetUnderlineHeight(),
                                                            mModel->GetDashedUnderlineGap(),
                                                            mModel->GetDashedUnderlineWidth(),
                                                            true,
                                                            true,
                                                            true,
                                                            true,
                                                            true};

    // Aggregate strikethrough-style-properties from mModel
    const StrikethroughStyleProperties modelStrikethroughProperties{mModel->GetStrikethroughColor(),
                                                                    mModel->GetStrikethroughHeight(),
                                                                    true,
                                                                    true};

    // Get the underline runs.
    const Length               numberOfUnderlineRuns = mModel->GetNumberOfUnderlineRuns();
    Vector<UnderlinedGlyphRun> underlineRuns;
    underlineRuns.Resize(numberOfUnderlineRuns);
    mModel->GetUnderlineRuns(underlineRuns.Begin(), 0u, numberOfUnderlineRuns);

    // Get the strikethrough runs.
    const Length                  numberOfStrikethroughRuns = mModel->GetNumberOfStrikethroughRuns();
    Vector<StrikethroughGlyphRun> strikethroughRuns;
    strikethroughRuns.Resize(numberOfStrikethroughRuns);
    mModel->GetStrikethroughRuns(strikethroughRuns.Begin(), 0u, numberOfStrikethroughRuns);

    bool thereAreUnderlinedGlyphs    = false;
    bool thereAreStrikethroughGlyphs = false;

    float currentUnderlinePosition   = 0.0f;
    float currentUnderlineHeight     = modelUnderlineProperties.height;
    float maxUnderlineHeight         = currentUnderlineHeight;
    auto  currentUnderlineProperties = modelUnderlineProperties;

    float currentStrikethroughHeight     = modelStrikethroughProperties.height;
    float maxStrikethroughHeight         = currentStrikethroughHeight;
    auto  currentStrikethroughProperties = modelStrikethroughProperties;
    float strikethroughStartingYPosition = 0.0f;

    FontId lastFontId = 0;

    float lineExtentLeft  = bufferWidth;
    float lineExtentRight = 0.0f;
    float baseline        = 0.0f;
    bool  addHyphen       = false;

    // Traverses the glyphs of the line.
    const GlyphIndex startGlyphIndex = std::max(std::max(line.glyphRun.glyphIndex, startIndexOfGlyphs), fromGlyphIndex);
    GlyphIndex       endGlyphIndex   = (line.isSplitToTwoHalves ? line.glyphRunSecondHalf.glyphIndex + line.glyphRunSecondHalf.numberOfGlyphs : line.glyphRun.glyphIndex + line.glyphRun.numberOfGlyphs) - 1u;
    endGlyphIndex                    = std::min(std::min(endGlyphIndex, endIndexOfGlyphs), toGlyphIndex);

    for(GlyphIndex glyphIndex = startGlyphIndex; glyphIndex <= endGlyphIndex; ++glyphIndex)
    {
      //To handle START case of ellipsis, the first glyph has been shifted
      //glyphIndex represent indices in whole glyphs but elidedGlyphIndex represents indices in elided Glyphs
      GlyphIndex elidedGlyphIndex = glyphIndex - startIndexOfGlyphs;

      //To handle MIDDLE case of ellipsis, the first glyph in the second half of line has been shifted and skip the removed glyph from middle.
      if(ellipsisPosition == DevelText::EllipsisPosition::MIDDLE)
      {
        if(glyphIndex > firstMiddleIndexOfElidedGlyphs &&
           glyphIndex < secondMiddleIndexOfElidedGlyphs)
        {
          // Ignore any glyph that removed for MIDDLE ellipsis
          continue;
        }
        if(glyphIndex >= secondMiddleIndexOfElidedGlyphs)
        {
          elidedGlyphIndex -= (secondMiddleIndexOfElidedGlyphs - firstMiddleIndexOfElidedGlyphs - 1u);
        }
      }

      // Retrieve the glyph's info.
      const GlyphInfo* glyphInfo;

      if(addHyphen && hyphens)
      {
        glyphInfo = hyphens + hyphenIndex;
        hyphenIndex++;
      }
      else
      {
        glyphInfo = glyphsBuffer + elidedGlyphIndex;
      }

      if((glyphInfo->width < Math::MACHINE_EPSILON_1000) ||
         (glyphInfo->height < Math::MACHINE_EPSILON_1000))
      {
        // Nothing to do if the glyph's width or height is zero.
        continue;
      }

      Vector<UnderlinedGlyphRun>::ConstIterator currentUnderlinedGlyphRunIt = underlineRuns.End();
      const bool                                underlineGlyph              = underlineEnabled || IsGlyphUnderlined(glyphIndex, underlineRuns, currentUnderlinedGlyphRunIt);
      currentUnderlineProperties                                            = GetCurrentUnderlineProperties(glyphIndex, underlineGlyph, underlineRuns, currentUnderlinedGlyphRunIt, modelUnderlineProperties);
      currentUnderlineHeight                                                = currentUnderlineProperties.height;
      thereAreUnderlinedGlyphs                                              = thereAreUnderlinedGlyphs || underlineGlyph;

      Vector<StrikethroughGlyphRun>::ConstIterator currentStrikethroughGlyphRunIt = strikethroughRuns.End();
      const bool                                   strikethroughGlyph             = strikethroughEnabled || IsGlyphStrikethrough(glyphIndex, strikethroughRuns, currentStrikethroughGlyphRunIt);
      currentStrikethroughProperties                                              = GetCurrentStrikethroughProperties(glyphIndex, strikethroughGlyph, strikethroughRuns, currentStrikethroughGlyphRunIt, modelStrikethroughProperties);
      currentStrikethroughHeight                                                  = currentStrikethroughProperties.height;
      thereAreStrikethroughGlyphs                                                 = thereAreStrikethroughGlyphs || strikethroughGlyph;

      // Are we still using the same fontId as previous
      if((glyphInfo->fontId != lastFontId) && (strikethroughGlyph || underlineGlyph))
      {
        // We need to fetch fresh font underline metrics
        FontMetrics fontMetrics;
        fontClient.GetFontMetrics(glyphInfo->fontId, fontMetrics);

        //The currentUnderlinePosition will be used for both Underline and/or Strikethrough
        currentUnderlinePosition = FetchUnderlinePositionFromFontMetrics(fontMetrics);

        if(underlineGlyph)
        {
          CalcualteUnderlineHeight(fontMetrics, currentUnderlineHeight, maxUnderlineHeight);
        }

        if(strikethroughGlyph)
        {
          CalcualteStrikethroughHeight(currentStrikethroughHeight, maxStrikethroughHeight);
        }

        // Update lastFontId because fontId is changed
        lastFontId = glyphInfo->fontId; // Prevents searching for existing blocksizes when string of the same fontId.
      }

      // Retrieves the glyph's position.
      Vector2 position = *(positionBuffer + elidedGlyphIndex);

      if(addHyphen)
      {
        GlyphInfo   tempInfo         = *(glyphsBuffer + elidedGlyphIndex);
        const float characterSpacing = GetGlyphCharacterSpacing(glyphIndex, characterSpacingGlyphRuns, modelCharacterSpacing);
        calculatedAdvance            = GetCalculatedAdvance(*(textBuffer + (*(glyphToCharacterMapBuffer + elidedGlyphIndex))), characterSpacing, tempInfo.advance);
        position.x                   = position.x + calculatedAdvance - tempInfo.xBearing + glyphInfo->xBearing;
        position.y                   = -glyphInfo->yBearing;
      }

      if(baseline < position.y + glyphInfo->yBearing)
      {
        baseline = position.y + glyphInfo->yBearing;
      }

      // Calculate the positions of leftmost and rightmost glyphs in the current line
      if(position.x < lineExtentLeft)
      {
        lineExtentLeft = position.x;
      }

      if(position.x + glyphInfo->width > lineExtentRight)
      {
        lineExtentRight = position.x + glyphInfo->width;
      }

      // Retrieves the glyph's color.
      const ColorIndex colorIndex = useDefaultColor ? 0u : *(colorIndexBuffer + glyphIndex);

      Vector4 color;
      if(style == Typesetter::STYLE_SHADOW)
      {
        color = mModel->GetShadowColor();
      }
      else if(style == Typesetter::STYLE_OUTLINE)
      {
        color = mModel->GetOutlineColor();
      }
      else
      {
        color = (useDefaultColor || (0u == colorIndex)) ? defaultColor : *(colorsBuffer + (colorIndex - 1u));
      }

      // Premultiply alpha
      color.r *= color.a;
      color.g *= color.a;
      color.b *= color.a;

      // Retrieves the glyph's bitmap.
      glyphData.glyphBitmap.buffer = NULL;
      glyphData.glyphBitmap.width  = glyphInfo->width; // Desired width and height.
      glyphData.glyphBitmap.height = glyphInfo->height;

      if(style != Typesetter::STYLE_OUTLINE && style != Typesetter::STYLE_SHADOW)
      {
        // Don't render outline for other styles
        outlineWidth = 0.0f;
      }

      if(style != Typesetter::STYLE_UNDERLINE && style != Typesetter::STYLE_STRIKETHROUGH)
      {
        fontClient.CreateBitmap(glyphInfo->fontId,
                                glyphInfo->index,
                                glyphInfo->isItalicRequired,
                                glyphInfo->isBoldRequired,
                                glyphData.glyphBitmap,
                                static_cast<int32_t>(outlineWidth));
      }

      // Sets the glyph's bitmap into the bitmap of the whole text.
      if(NULL != glyphData.glyphBitmap.buffer)
      {
        if(style == Typesetter::STYLE_OUTLINE)
        {
          // Set the position offset for the current glyph
          glyphData.horizontalOffset -= glyphData.glyphBitmap.outlineOffsetX;
          glyphData.verticalOffset -= glyphData.glyphBitmap.outlineOffsetY;
        }

        // Set the buffer of the glyph's bitmap into the final bitmap's buffer
        TypesetGlyph(glyphData,
                     &position,
                     &color,
                     style,
                     pixelFormat);

        if(style == Typesetter::STYLE_OUTLINE)
        {
          // Reset the position offset for the next glyph
          glyphData.horizontalOffset += glyphData.glyphBitmap.outlineOffsetX;
          glyphData.verticalOffset += glyphData.glyphBitmap.outlineOffsetY;
        }

        // free the glyphBitmap.buffer if it is owner of buffer
        if(glyphData.glyphBitmap.isBufferOwned)
        {
          free(glyphData.glyphBitmap.buffer);
          glyphData.glyphBitmap.isBufferOwned = false;
        }
        glyphData.glyphBitmap.buffer = NULL;
      }

      if(hyphenIndices)
      {
        while((hyphenIndex < hyphensCount) && (glyphIndex > hyphenIndices[hyphenIndex]))
        {
          hyphenIndex++;
        }

        addHyphen = ((hyphenIndex < hyphensCount) && ((glyphIndex + 1) == hyphenIndices[hyphenIndex]));
        if(addHyphen)
        {
          glyphIndex--;
        }
      }
    }

    // Draw the underline from the leftmost glyph to the rightmost glyph
    if(thereAreUnderlinedGlyphs && style == Typesetter::STYLE_UNDERLINE)
    {
      DrawUnderline(bufferWidth, bufferHeight, glyphData, baseline, currentUnderlinePosition, maxUnderlineHeight, lineExtentLeft, lineExtentRight, modelUnderlineProperties, currentUnderlineProperties, line);
    }

    // Draw the background color from the leftmost glyph to the rightmost glyph
    if(style == Typesetter::STYLE_BACKGROUND)
    {
      DrawBackgroundColor(mModel->GetBackgroundColor(), bufferWidth, bufferHeight, glyphData, baseline, line, lineExtentLeft, lineExtentRight);
    }

    // Draw the strikethrough from the leftmost glyph to the rightmost glyph
    if(thereAreStrikethroughGlyphs && style == Typesetter::STYLE_STRIKETHROUGH)
    {
      //TODO : The currently implemented strikethrough creates a strikethrough on the line level. We need to create different strikethroughs the case of glyphs with different sizes.
      strikethroughStartingYPosition = (glyphData.verticalOffset + baseline + currentUnderlinePosition) - ((line.ascender) * HALF); // Since Free Type font doesn't contain the strikethrough-position property, strikethrough position will be calculated by moving the underline position upwards by half the value of the line height.
      DrawStrikethrough(bufferWidth, bufferHeight, glyphData, baseline, strikethroughStartingYPosition, maxStrikethroughHeight, lineExtentLeft, lineExtentRight, modelStrikethroughProperties, currentStrikethroughProperties, line);
    }

    // Increases the vertical offset with the line's descender & line spacing.
    glyphData.verticalOffset += static_cast<int32_t>(-line.descender + GetPostOffsetVerticalLineAlignment(line, verLineAlign));
  }

  return glyphData.bitmapBuffer;
}

Devel::PixelBuffer Typesetter::ApplyUnderlineMarkupImageBuffer(Devel::PixelBuffer topPixelBuffer, const uint32_t bufferWidth, const uint32_t bufferHeight, const bool ignoreHorizontalAlignment, const Pixel::Format pixelFormat, const int32_t horizontalOffset, const int32_t verticalOffset)
{
  // Underline-tags (this is for Markup case)
  // Get the underline runs.
  const Length               numberOfUnderlineRuns = mModel->GetNumberOfUnderlineRuns();
  Vector<UnderlinedGlyphRun> underlineRuns;
  underlineRuns.Resize(numberOfUnderlineRuns);
  mModel->GetUnderlineRuns(underlineRuns.Begin(), 0u, numberOfUnderlineRuns);

  // Iterate on the consecutive underlined glyph run and connect them into one chunk of underlined characters.
  Vector<UnderlinedGlyphRun>::ConstIterator itGlyphRun    = underlineRuns.Begin();
  Vector<UnderlinedGlyphRun>::ConstIterator endItGlyphRun = underlineRuns.End();
  GlyphIndex                                startGlyphIndex, endGlyphIndex;

  //The outer loop to iterate on the separated chunks of underlined glyph runs
  while(itGlyphRun != endItGlyphRun)
  {
    startGlyphIndex = itGlyphRun->glyphRun.glyphIndex;
    endGlyphIndex   = startGlyphIndex + itGlyphRun->glyphRun.numberOfGlyphs - 1;

    // Create the image buffer for underline
    Devel::PixelBuffer underlineImageBuffer = CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_UNDERLINE, ignoreHorizontalAlignment, pixelFormat, horizontalOffset, verticalOffset, startGlyphIndex, endGlyphIndex);
    // Combine the two buffers
    // Result pixel buffer will be stored into topPixelBuffer.
    CombineImageBuffer(underlineImageBuffer, topPixelBuffer, bufferWidth, bufferHeight, false);

    itGlyphRun++;
  }

  return topPixelBuffer;
}

Devel::PixelBuffer Typesetter::ApplyStrikethroughMarkupImageBuffer(Devel::PixelBuffer topPixelBuffer, const uint32_t bufferWidth, const uint32_t bufferHeight, const bool ignoreHorizontalAlignment, const Pixel::Format pixelFormat, const int32_t horizontalOffset, const int32_t verticalOffset)
{
  // strikethrough-tags (this is for Markup case)
  // Get the strikethrough runs.
  const Length                  numberOfStrikethroughRuns = mModel->GetNumberOfStrikethroughRuns();
  Vector<StrikethroughGlyphRun> strikethroughRuns;
  strikethroughRuns.Resize(numberOfStrikethroughRuns);
  mModel->GetStrikethroughRuns(strikethroughRuns.Begin(), 0u, numberOfStrikethroughRuns);

  // Iterate on the consecutive strikethrough glyph run and connect them into one chunk of strikethrough characters.
  Vector<StrikethroughGlyphRun>::ConstIterator itGlyphRun    = strikethroughRuns.Begin();
  Vector<StrikethroughGlyphRun>::ConstIterator endItGlyphRun = strikethroughRuns.End();
  GlyphIndex                                   startGlyphIndex, endGlyphIndex;

  //The outer loop to iterate on the separated chunks of strikethrough glyph runs
  while(itGlyphRun != endItGlyphRun)
  {
    startGlyphIndex = itGlyphRun->glyphRun.glyphIndex;
    endGlyphIndex   = startGlyphIndex + itGlyphRun->glyphRun.numberOfGlyphs - 1;

    // Create the image buffer for strikethrough
    Devel::PixelBuffer strikethroughImageBuffer = CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_STRIKETHROUGH, ignoreHorizontalAlignment, pixelFormat, horizontalOffset, verticalOffset, startGlyphIndex, endGlyphIndex);
    // Combine the two buffers
    // Result pixel buffer will be stored into topPixelBuffer.
    CombineImageBuffer(strikethroughImageBuffer, topPixelBuffer, bufferWidth, bufferHeight, false);

    itGlyphRun++;
  }

  return topPixelBuffer;
}

Typesetter::Typesetter(const ModelInterface* const model)
: mModel(new ViewModel(model))
{
}

Typesetter::~Typesetter()
{
  delete mModel;
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

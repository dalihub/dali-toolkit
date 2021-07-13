/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
void TypesetGlyph(GlyphData&           data,
                  const Vector2* const position,
                  const Vector4* const color,
                  Typesetter::Style    style,
                  Pixel::Format        pixelFormat)
{
  if((0u == data.glyphBitmap.width) || (0u == data.glyphBitmap.height))
  {
    // Nothing to do if the width or height of the buffer is zero.
    return;
  }

  const int widthMinusOne  = static_cast<int>(data.width - 1u);
  const int heightMinusOne = static_cast<int>(data.height - 1u);

  if(Pixel::RGBA8888 == pixelFormat)
  {
    // Whether the given glyph is a color one.
    const bool     isColorGlyph   = data.glyphBitmap.isColorEmoji || data.glyphBitmap.isColorBitmap;
    const uint32_t glyphPixelSize = Pixel::GetBytesPerPixel(data.glyphBitmap.format);
    const uint32_t alphaIndex     = glyphPixelSize - 1u;
    const bool     swapChannelsBR = Pixel::BGRA8888 == data.glyphBitmap.format;

    // Pointer to the color glyph if there is one.
    const uint32_t* const colorGlyphBuffer = isColorGlyph ? reinterpret_cast<uint32_t*>(data.glyphBitmap.buffer) : NULL;

    // Initial vertical offset.
    const int yOffset = data.verticalOffset + position->y;

    uint32_t* bitmapBuffer = reinterpret_cast<uint32_t*>(data.bitmapBuffer.GetBuffer());

    // Traverse the pixels of the glyph line per line.
    for(int lineIndex = 0, glyphHeight = static_cast<int>(data.glyphBitmap.height); lineIndex < glyphHeight; ++lineIndex)
    {
      const int yOffsetIndex = yOffset + lineIndex;
      if((0 > yOffsetIndex) || (yOffsetIndex > heightMinusOne))
      {
        // Do not write out of bounds.
        continue;
      }

      const int verticalOffset    = yOffsetIndex * data.width;
      const int xOffset           = data.horizontalOffset + position->x;
      const int glyphBufferOffset = lineIndex * static_cast<int>(data.glyphBitmap.width);
      for(int index = 0, glyphWidth = static_cast<int>(data.glyphBitmap.width); index < glyphWidth; ++index)
      {
        const int xOffsetIndex = xOffset + index;
        if((0 > xOffsetIndex) || (xOffsetIndex > widthMinusOne))
        {
          // Don't write out of bounds.
          continue;
        }

        if(isColorGlyph)
        {
          // Retrieves the color from the color glyph.
          uint32_t packedColorGlyph       = *(colorGlyphBuffer + glyphBufferOffset + index);
          uint8_t* packedColorGlyphBuffer = reinterpret_cast<uint8_t*>(&packedColorGlyph);

          // Update the alpha channel.
          if(Typesetter::STYLE_MASK == style || Typesetter::STYLE_OUTLINE == style) // Outline not shown for color glyph
          {
            // Create an alpha mask for color glyph.
            *(packedColorGlyphBuffer + 3u) = 0u;
            *(packedColorGlyphBuffer + 2u) = 0u;
            *(packedColorGlyphBuffer + 1u) = 0u;
            *packedColorGlyphBuffer        = 0u;
          }
          else
          {
            const uint8_t colorAlpha       = static_cast<uint8_t>(color->a * static_cast<float>(*(packedColorGlyphBuffer + 3u)));
            *(packedColorGlyphBuffer + 3u) = colorAlpha;

            if(Typesetter::STYLE_SHADOW == style)
            {
              // The shadow of color glyph needs to have the shadow color.
              *(packedColorGlyphBuffer + 2u) = static_cast<uint8_t>(color->b * colorAlpha);
              *(packedColorGlyphBuffer + 1u) = static_cast<uint8_t>(color->g * colorAlpha);
              *packedColorGlyphBuffer        = static_cast<uint8_t>(color->r * colorAlpha);
            }
            else
            {
              if(swapChannelsBR)
              {
                std::swap(*packedColorGlyphBuffer, *(packedColorGlyphBuffer + 2u)); // Swap B and R.
              }

              *(packedColorGlyphBuffer + 2u) = (*(packedColorGlyphBuffer + 2u) * colorAlpha / 255);
              *(packedColorGlyphBuffer + 1u) = (*(packedColorGlyphBuffer + 1u) * colorAlpha / 255);
              *packedColorGlyphBuffer        = (*(packedColorGlyphBuffer)*colorAlpha / 255);

              if(data.glyphBitmap.isColorBitmap)
              {
                *(packedColorGlyphBuffer + 2u) = static_cast<uint8_t>(*(packedColorGlyphBuffer + 2u) * color->b);
                *(packedColorGlyphBuffer + 1u) = static_cast<uint8_t>(*(packedColorGlyphBuffer + 1u) * color->g);
                *packedColorGlyphBuffer        = static_cast<uint8_t>(*packedColorGlyphBuffer * color->r);
              }
            }
          }

          // Set the color into the final pixel buffer.
          *(bitmapBuffer + verticalOffset + xOffsetIndex) = packedColorGlyph;
        }
        else
        {
          // Pack the given color into a 32bit buffer. The alpha channel will be updated later for each pixel.
          // The format is RGBA8888.
          uint32_t packedColor       = 0u;
          uint8_t* packedColorBuffer = reinterpret_cast<uint8_t*>(&packedColor);

          // Update the alpha channel.
          const uint8_t alpha = *(data.glyphBitmap.buffer + glyphPixelSize * (glyphBufferOffset + index) + alphaIndex);

          // Copy non-transparent pixels only
          if(alpha > 0u)
          {
            // Check alpha of overlapped pixels
            uint32_t& currentColor             = *(bitmapBuffer + verticalOffset + xOffsetIndex);
            uint8_t*  packedCurrentColorBuffer = reinterpret_cast<uint8_t*>(&currentColor);

            // For any pixel overlapped with the pixel in previous glyphs, make sure we don't
            // overwrite a previous bigger alpha with a smaller alpha (in order to avoid
            // semi-transparent gaps between joint glyphs with overlapped pixels, which could
            // happen, for example, in the RTL text when we copy glyphs from right to left).
            uint8_t currentAlpha = *(packedCurrentColorBuffer + 3u);
            currentAlpha         = std::max(currentAlpha, alpha);

            // Color is pre-muliplied with its alpha.
            *(packedColorBuffer + 3u) = static_cast<uint8_t>(color->a * currentAlpha);
            *(packedColorBuffer + 2u) = static_cast<uint8_t>(color->b * currentAlpha);
            *(packedColorBuffer + 1u) = static_cast<uint8_t>(color->g * currentAlpha);
            *(packedColorBuffer)      = static_cast<uint8_t>(color->r * currentAlpha);

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
    const bool     isColorGlyph   = data.glyphBitmap.isColorEmoji || data.glyphBitmap.isColorBitmap;
    const uint32_t glyphPixelSize = Pixel::GetBytesPerPixel(data.glyphBitmap.format);
    const uint32_t alphaIndex     = glyphPixelSize - 1u;

    // Initial vertical offset.
    const int yOffset = data.verticalOffset + position->y;

    uint8_t* bitmapBuffer = reinterpret_cast<uint8_t*>(data.bitmapBuffer.GetBuffer());

    // Traverse the pixels of the glyph line per line.
    for(int lineIndex = 0, glyphHeight = static_cast<int>(data.glyphBitmap.height); lineIndex < glyphHeight; ++lineIndex)
    {
      const int yOffsetIndex = yOffset + lineIndex;
      if((0 > yOffsetIndex) || (yOffsetIndex > heightMinusOne))
      {
        // Do not write out of bounds.
        continue;
      }

      const int verticalOffset    = yOffsetIndex * data.width;
      const int xOffset           = data.horizontalOffset + position->x;
      const int glyphBufferOffset = lineIndex * static_cast<int>(data.glyphBitmap.width);
      for(int index = 0, glyphWidth = static_cast<int>(data.glyphBitmap.width); index < glyphWidth; ++index)
      {
        const int xOffsetIndex = xOffset + index;
        if((0 > xOffsetIndex) || (xOffsetIndex > widthMinusOne))
        {
          // Don't write out of bounds.
          continue;
        }

        if(!isColorGlyph)
        {
          // Update the alpha channel.
          const uint8_t alpha = *(data.glyphBitmap.buffer + glyphPixelSize * (glyphBufferOffset + index) + alphaIndex);

          // Copy non-transparent pixels only
          if(alpha > 0u)
          {
            // Check alpha of overlapped pixels
            uint8_t& currentAlpha = *(bitmapBuffer + verticalOffset + xOffsetIndex);

            // For any pixel overlapped with the pixel in previous glyphs, make sure we don't
            // overwrite a previous bigger alpha with a smaller alpha (in order to avoid
            // semi-transparent gaps between joint glyphs with overlapped pixels, which could
            // happen, for example, in the RTL text when we copy glyphs from right to left).
            currentAlpha = std::max(currentAlpha, alpha);
          }
        }
      }
    }
  }
}

bool IsGlyphUnderlined(GlyphIndex              index,
                       const Vector<GlyphRun>& underlineRuns)
{
  for(Vector<GlyphRun>::ConstIterator it    = underlineRuns.Begin(),
                                      endIt = underlineRuns.End();
      it != endIt;
      ++it)
  {
    const GlyphRun& run = *it;

    if((run.glyphIndex <= index) && (index < run.glyphIndex + run.numberOfGlyphs))
    {
      return true;
    }
  }

  return false;
}

/// Helper method to fetch the underline metrics for the specified font glyph
void FetchFontUnderlineMetrics(
  TextAbstraction::FontClient& fontClient,
  const GlyphInfo* const       glyphInfo,
  float&                       currentUnderlinePosition,
  const float                  underlineHeight,
  float&                       currentUnderlineThickness,
  float&                       maxUnderlineThickness,
  FontId&                      lastUnderlinedFontId)
{
  FontMetrics fontMetrics;
  fontClient.GetFontMetrics(glyphInfo->fontId, fontMetrics);
  currentUnderlinePosition = ceil(fabsf(fontMetrics.underlinePosition));
  const float descender    = ceil(fabsf(fontMetrics.descender));

  if(fabsf(underlineHeight) < Math::MACHINE_EPSILON_1000)
  {
    currentUnderlineThickness = fontMetrics.underlineThickness;

    // Ensure underline will be at least a pixel high
    if(currentUnderlineThickness < 1.0f)
    {
      currentUnderlineThickness = 1.0f;
    }
    else
    {
      currentUnderlineThickness = ceil(currentUnderlineThickness);
    }
  }

  // The underline thickness should be the max underline thickness of all glyphs of the line.
  if(currentUnderlineThickness > maxUnderlineThickness)
  {
    maxUnderlineThickness = currentUnderlineThickness;
  }

  // Clamp the underline position at the font descender and check for ( as EFL describes it ) a broken font
  if(currentUnderlinePosition > descender)
  {
    currentUnderlinePosition = descender;
  }

  if(fabsf(currentUnderlinePosition) < Math::MACHINE_EPSILON_1000)
  {
    // Move offset down by one ( EFL behavior )
    currentUnderlinePosition = 1.0f;
  }

  lastUnderlinedFontId = glyphInfo->fontId;
}

/// Draws the specified color to the pixel buffer
void WriteColorToPixelBuffer(
  GlyphData&         glyphData,
  uint32_t*          bitmapBuffer,
  const Vector4&     color,
  const unsigned int x,
  const unsigned int y)
{
  // Always RGBA image for text with styles
  uint32_t pixel       = *(bitmapBuffer + y * glyphData.width + x);
  uint8_t* pixelBuffer = reinterpret_cast<uint8_t*>(&pixel);

  // Write the color to the pixel buffer
  uint8_t colorAlpha  = static_cast<uint8_t>(color.a * 255.f);
  *(pixelBuffer + 3u) = colorAlpha;
  *(pixelBuffer + 2u) = static_cast<uint8_t>(color.b * colorAlpha);
  *(pixelBuffer + 1u) = static_cast<uint8_t>(color.g * colorAlpha);
  *(pixelBuffer)      = static_cast<uint8_t>(color.r * colorAlpha);

  *(bitmapBuffer + y * glyphData.width + x) = pixel;
}

/// Draws the specified underline color to the buffer
void DrawUnderline(
  const Vector4&     underlineColor,
  const unsigned int bufferWidth,
  const unsigned int bufferHeight,
  GlyphData&         glyphData,
  const float        baseline,
  const float        currentUnderlinePosition,
  const float        maxUnderlineThickness,
  const float        lineExtentLeft,
  const float        lineExtentRight)
{
  int       underlineYOffset = glyphData.verticalOffset + baseline + currentUnderlinePosition;
  uint32_t* bitmapBuffer     = reinterpret_cast<uint32_t*>(glyphData.bitmapBuffer.GetBuffer());

  for(unsigned int y = underlineYOffset; y < underlineYOffset + maxUnderlineThickness; y++)
  {
    if(y > bufferHeight - 1)
    {
      // Do not write out of bounds.
      break;
    }

    for(unsigned int x = glyphData.horizontalOffset + lineExtentLeft; x <= glyphData.horizontalOffset + lineExtentRight; x++)
    {
      if(x > bufferWidth - 1)
      {
        // Do not write out of bounds.
        break;
      }

      WriteColorToPixelBuffer(glyphData, bitmapBuffer, underlineColor, x, y);
    }
  }
}

/// Draws the background color to the buffer
void DrawBackgroundColor(
  Vector4            backgroundColor,
  const unsigned int bufferWidth,
  const unsigned int bufferHeight,
  GlyphData&         glyphData,
  const float        baseline,
  const LineRun&     line,
  const float        lineExtentLeft,
  const float        lineExtentRight)
{
  uint32_t* bitmapBuffer = reinterpret_cast<uint32_t*>(glyphData.bitmapBuffer.GetBuffer());

  for(int y = glyphData.verticalOffset + baseline - line.ascender; y < glyphData.verticalOffset + baseline - line.descender; y++)
  {
    if((y < 0) || (y > static_cast<int>(bufferHeight - 1)))
    {
      // Do not write out of bounds.
      continue;
    }

    for(int x = glyphData.horizontalOffset + lineExtentLeft; x <= glyphData.horizontalOffset + lineExtentRight; x++)
    {
      if((x < 0) || (x > static_cast<int>(bufferWidth - 1)))
      {
        // Do not write out of bounds.
        continue;
      }

      WriteColorToPixelBuffer(glyphData, bitmapBuffer, backgroundColor, x, y);
    }
  }
}

Devel::PixelBuffer DrawGlyphsBackground(const ViewModel* model, Devel::PixelBuffer& buffer, const unsigned int bufferWidth, const unsigned int bufferHeight, bool ignoreHorizontalAlignment, int horizontalOffset, int verticalOffset)
{
  // Retrieve lines, glyphs, positions and colors from the view model.
  const Length            modelNumberOfLines           = model->GetNumberOfLines();
  const LineRun* const    modelLinesBuffer             = model->GetLines();
  const Length            numberOfGlyphs               = model->GetNumberOfGlyphs();
  const GlyphInfo* const  glyphsBuffer                 = model->GetGlyphs();
  const Vector2* const    positionBuffer               = model->GetLayout();
  const Vector4* const    backgroundColorsBuffer       = model->GetBackgroundColors();
  const ColorIndex* const backgroundColorIndicesBuffer = model->GetBackgroundColorIndices();

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
    glyphData.horizontalOffset = ignoreHorizontalAlignment ? 0 : static_cast<int>(line.alignmentOffset);
    glyphData.horizontalOffset += horizontalOffset;

    // Increases the vertical offset with the line's ascender.
    glyphData.verticalOffset += static_cast<int>(line.ascender);

    // Include line spacing after first line
    if(lineIndex > 0u)
    {
      glyphData.verticalOffset += static_cast<int>(line.lineSpacing);
    }

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
    glyphData.verticalOffset += static_cast<int>(-line.descender);
  }

  return glyphData.bitmapBuffer;
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

Devel::PixelBuffer Typesetter::CreateImageBuffer(const unsigned int bufferWidth, const unsigned int bufferHeight, Pixel::Format pixelFormat)
{
  Devel::PixelBuffer imageBuffer = Devel::PixelBuffer::New(bufferWidth, bufferHeight, pixelFormat);

  if(Pixel::RGBA8888 == pixelFormat)
  {
    const unsigned int bufferSizeInt  = bufferWidth * bufferHeight;
    const unsigned int bufferSizeChar = 4u * bufferSizeInt;
    memset(imageBuffer.GetBuffer(), 0u, bufferSizeChar);
  }
  else
  {
    memset(imageBuffer.GetBuffer(), 0, bufferWidth * bufferHeight);
  }

  return imageBuffer;
}

PixelData Typesetter::Render(const Vector2& size, Toolkit::DevelText::TextDirection::Type textDirection, RenderBehaviour behaviour, bool ignoreHorizontalAlignment, Pixel::Format pixelFormat)
{
  // @todo. This initial implementation for a TextLabel has only one visible page.

  // Elides the text if needed.
  mModel->ElideGlyphs();

  // Retrieves the layout size.
  const Size& layoutSize = mModel->GetLayoutSize();

  const int outlineWidth = static_cast<int>(mModel->GetOutlineWidth());

  // Set the offset for the horizontal alignment according to the text direction and outline width.
  int penX = 0;

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
  int penY = 0u;

  switch(mModel->GetVerticalAlignment())
  {
    case VerticalAlignment::TOP:
    {
      // No offset to add.
      break;
    }
    case VerticalAlignment::CENTER:
    {
      penY = static_cast<int>(0.5f * (size.height - layoutSize.height));
      penY = penY < 0.f ? 0.f : penY;
      break;
    }
    case VerticalAlignment::BOTTOM:
    {
      penY = static_cast<int>(size.height - layoutSize.height);
      break;
    }
  }

  // Calculate vertical line alignment
  switch(mModel->GetVerticalLineAlignment())
  {
    case DevelText::VerticalLineAlignment::TOP:
    {
      break;
    }
    case DevelText::VerticalLineAlignment::MIDDLE:
    {
      const auto& line = *mModel->GetLines();
      penY -= line.descender;
      penY += static_cast<int>(line.lineSpacing * 0.5f + line.descender);
      break;
    }
    case DevelText::VerticalLineAlignment::BOTTOM:
    {
      const auto& line       = *mModel->GetLines();
      const auto  lineHeight = line.ascender + (-line.descender) + line.lineSpacing;
      penY += static_cast<int>(lineHeight - (line.ascender - line.descender));
      break;
    }
  }

  // Generate the image buffers of the text for each different style first,
  // then combine all of them together as one final image buffer. We try to
  // do all of these in CPU only, so that once the final texture is generated,
  // no calculation is needed in GPU during each frame.

  const unsigned int bufferWidth  = static_cast<unsigned int>(size.width);
  const unsigned int bufferHeight = static_cast<unsigned int>(size.height);

  const unsigned int bufferSizeInt  = bufferWidth * bufferHeight;
  const unsigned int bufferSizeChar = 4u * bufferSizeInt;

  Length numberOfGlyphs = mModel->GetNumberOfGlyphs();

  Devel::PixelBuffer imageBuffer;

  if(RENDER_MASK == behaviour)
  {
    // Generate the image buffer as an alpha mask for color glyphs.
    imageBuffer = CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_MASK, ignoreHorizontalAlignment, pixelFormat, penX, penY, 0u, numberOfGlyphs - 1);
  }
  else if(RENDER_NO_TEXT == behaviour)
  {
    // Generate an empty image buffer so that it can been combined with the image buffers for styles
    imageBuffer = Devel::PixelBuffer::New(bufferWidth, bufferHeight, Pixel::RGBA8888);
    memset(imageBuffer.GetBuffer(), 0u, bufferSizeChar);
  }
  else
  {
    // Generate the image buffer for the text with no style.
    imageBuffer = CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_NONE, ignoreHorizontalAlignment, pixelFormat, penX, penY, 0u, numberOfGlyphs - 1);
  }

  if((RENDER_NO_STYLES != behaviour) && (RENDER_MASK != behaviour))
  {
    // Generate the outline if enabled
    const uint16_t outlineWidth = mModel->GetOutlineWidth();
    if(outlineWidth != 0u)
    {
      // Create the image buffer for outline
      Devel::PixelBuffer outlineImageBuffer = CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_OUTLINE, ignoreHorizontalAlignment, pixelFormat, penX, penY, 0u, numberOfGlyphs - 1);

      // Combine the two buffers
      imageBuffer = CombineImageBuffer(imageBuffer, outlineImageBuffer, bufferWidth, bufferHeight);
    }

    // @todo. Support shadow and underline for partial text later on.

    // Generate the shadow if enabled
    const Vector2& shadowOffset = mModel->GetShadowOffset();
    if(fabsf(shadowOffset.x) > Math::MACHINE_EPSILON_1 || fabsf(shadowOffset.y) > Math::MACHINE_EPSILON_1)
    {
      // Create the image buffer for shadow
      Devel::PixelBuffer shadowImageBuffer = CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_SHADOW, ignoreHorizontalAlignment, pixelFormat, penX, penY, 0u, numberOfGlyphs - 1);

      // Check whether it will be a soft shadow
      const float& blurRadius = mModel->GetShadowBlurRadius();

      if(blurRadius > Math::MACHINE_EPSILON_1)
      {
        shadowImageBuffer.ApplyGaussianBlur(blurRadius);
      }

      // Combine the two buffers
      imageBuffer = CombineImageBuffer(imageBuffer, shadowImageBuffer, bufferWidth, bufferHeight);
    }

    // Generate the underline if enabled
    const bool underlineEnabled = mModel->IsUnderlineEnabled();
    if(underlineEnabled)
    {
      // Create the image buffer for underline
      Devel::PixelBuffer underlineImageBuffer = CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_UNDERLINE, ignoreHorizontalAlignment, pixelFormat, penX, penY, 0u, numberOfGlyphs - 1);

      // Combine the two buffers
      imageBuffer = CombineImageBuffer(imageBuffer, underlineImageBuffer, bufferWidth, bufferHeight);
    }

    // Generate the background if enabled
    const bool backgroundEnabled   = mModel->IsBackgroundEnabled();
    const bool backgroundMarkupSet = mModel->IsMarkupBackgroundColorSet();
    if(backgroundEnabled || backgroundMarkupSet)
    {
      Devel::PixelBuffer backgroundImageBuffer;

      if(backgroundEnabled)
      {
        backgroundImageBuffer = CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_BACKGROUND, ignoreHorizontalAlignment, pixelFormat, penX, penY, 0u, numberOfGlyphs - 1);
      }
      else
      {
        backgroundImageBuffer = CreateImageBuffer(bufferWidth, bufferHeight, pixelFormat);
      }

      if(backgroundMarkupSet)
      {
        DrawGlyphsBackground(mModel, backgroundImageBuffer, bufferWidth, bufferHeight, ignoreHorizontalAlignment, penX, penY);
      }

      // Combine the two buffers
      imageBuffer = CombineImageBuffer(imageBuffer, backgroundImageBuffer, bufferWidth, bufferHeight);
    }

    // Markup-Processor

    imageBuffer = ApplyMarkupProcessorOnPixelBuffer(imageBuffer, bufferWidth, bufferHeight, ignoreHorizontalAlignment, pixelFormat, penX, penY);
  }

  // Create the final PixelData for the combined image buffer
  PixelData pixelData = Devel::PixelBuffer::Convert(imageBuffer);

  return pixelData;
}

Devel::PixelBuffer Typesetter::CreateImageBuffer(const unsigned int bufferWidth, const unsigned int bufferHeight, Typesetter::Style style, bool ignoreHorizontalAlignment, Pixel::Format pixelFormat, int horizontalOffset, int verticalOffset, GlyphIndex fromGlyphIndex, GlyphIndex toGlyphIndex)
{
  // Retrieve lines, glyphs, positions and colors from the view model.
  const Length            modelNumberOfLines = mModel->GetNumberOfLines();
  const LineRun* const    modelLinesBuffer   = mModel->GetLines();
  const Length            numberOfGlyphs     = mModel->GetNumberOfGlyphs();
  const GlyphInfo* const  glyphsBuffer       = mModel->GetGlyphs();
  const Vector2* const    positionBuffer     = mModel->GetLayout();
  const Vector4* const    colorsBuffer       = mModel->GetColors();
  const ColorIndex* const colorIndexBuffer   = mModel->GetColorIndices();
  const GlyphInfo*        hyphens            = mModel->GetHyphens();
  const Length*           hyphenIndices      = mModel->GetHyphenIndices();
  const Length            hyphensCount       = mModel->GetHyphensCount();

  // Whether to use the default color.
  const bool     useDefaultColor = (NULL == colorsBuffer);
  const Vector4& defaultColor    = mModel->GetDefaultColor();

  // Create and initialize the pixel buffer.
  GlyphData glyphData;
  glyphData.verticalOffset   = verticalOffset;
  glyphData.width            = bufferWidth;
  glyphData.height           = bufferHeight;
  glyphData.bitmapBuffer     = CreateImageBuffer(bufferWidth, bufferHeight, pixelFormat);
  glyphData.horizontalOffset = 0;

  // Get a handle of the font client. Used to retrieve the bitmaps of the glyphs.
  TextAbstraction::FontClient fontClient  = TextAbstraction::FontClient::Get();
  Length                      hyphenIndex = 0;

  // Traverses the lines of the text.
  for(LineIndex lineIndex = 0u; lineIndex < modelNumberOfLines; ++lineIndex)
  {
    const LineRun& line = *(modelLinesBuffer + lineIndex);

    // Sets the horizontal offset of the line.
    glyphData.horizontalOffset = ignoreHorizontalAlignment ? 0 : static_cast<int>(line.alignmentOffset);
    glyphData.horizontalOffset += horizontalOffset;

    // Increases the vertical offset with the line's ascender.
    glyphData.verticalOffset += static_cast<int>(line.ascender);

    // Include line spacing after first line
    if(lineIndex > 0u)
    {
      glyphData.verticalOffset += static_cast<int>(line.lineSpacing);
    }

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

    const bool     underlineEnabled = mModel->IsUnderlineEnabled();
    const Vector4& underlineColor   = mModel->GetUnderlineColor();
    const float    underlineHeight  = mModel->GetUnderlineHeight();

    // Get the underline runs.
    const Length     numberOfUnderlineRuns = mModel->GetNumberOfUnderlineRuns();
    Vector<GlyphRun> underlineRuns;
    underlineRuns.Resize(numberOfUnderlineRuns);
    mModel->GetUnderlineRuns(underlineRuns.Begin(), 0u, numberOfUnderlineRuns);

    bool thereAreUnderlinedGlyphs = false;

    float currentUnderlinePosition  = 0.0f;
    float currentUnderlineThickness = underlineHeight;
    float maxUnderlineThickness     = currentUnderlineThickness;

    FontId lastUnderlinedFontId = 0;

    float lineExtentLeft  = bufferWidth;
    float lineExtentRight = 0.0f;
    float baseline        = 0.0f;
    bool  addHyphen       = false;

    // Traverses the glyphs of the line.
    const GlyphIndex endGlyphIndex = std::min(numberOfGlyphs, line.glyphRun.glyphIndex + line.glyphRun.numberOfGlyphs);
    for(GlyphIndex glyphIndex = line.glyphRun.glyphIndex; glyphIndex < endGlyphIndex; ++glyphIndex)
    {
      if(glyphIndex < fromGlyphIndex || glyphIndex > toGlyphIndex)
      {
        // Ignore any glyph that out of the specified range
        continue;
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
        glyphInfo = glyphsBuffer + glyphIndex;
      }

      if((glyphInfo->width < Math::MACHINE_EPSILON_1000) ||
         (glyphInfo->height < Math::MACHINE_EPSILON_1000))
      {
        // Nothing to do if the glyph's width or height is zero.
        continue;
      }

      const bool underlineGlyph = underlineEnabled || IsGlyphUnderlined(glyphIndex, underlineRuns);
      thereAreUnderlinedGlyphs  = thereAreUnderlinedGlyphs || underlineGlyph;

      // Are we still using the same fontId as previous
      if(underlineGlyph && (glyphInfo->fontId != lastUnderlinedFontId))
      {
        // We need to fetch fresh font underline metrics
        FetchFontUnderlineMetrics(fontClient, glyphInfo, currentUnderlinePosition, underlineHeight, currentUnderlineThickness, maxUnderlineThickness, lastUnderlinedFontId);
      } // underline

      // Retrieves the glyph's position.
      Vector2 position = *(positionBuffer + glyphIndex);

      if(addHyphen)
      {
        GlyphInfo tempInfo = *(glyphsBuffer + glyphIndex);
        position.x         = position.x + tempInfo.advance - tempInfo.xBearing + glyphInfo->xBearing;
        position.y         = -glyphInfo->yBearing;
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

      if(style != Typesetter::STYLE_UNDERLINE)
      {
        fontClient.CreateBitmap(glyphInfo->fontId,
                                glyphInfo->index,
                                glyphInfo->isItalicRequired,
                                glyphInfo->isBoldRequired,
                                glyphData.glyphBitmap,
                                static_cast<int>(outlineWidth));
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

        // delete the glyphBitmap.buffer as it is now copied into glyphData.bitmapBuffer
        delete[] glyphData.glyphBitmap.buffer;
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
      DrawUnderline(underlineColor, bufferWidth, bufferHeight, glyphData, baseline, currentUnderlinePosition, maxUnderlineThickness, lineExtentLeft, lineExtentRight);
    }

    // Draw the background color from the leftmost glyph to the rightmost glyph
    if(style == Typesetter::STYLE_BACKGROUND)
    {
      DrawBackgroundColor(mModel->GetBackgroundColor(), bufferWidth, bufferHeight, glyphData, baseline, line, lineExtentLeft, lineExtentRight);
    }

    // Increases the vertical offset with the line's descender.
    glyphData.verticalOffset += static_cast<int>(-line.descender);
  }

  return glyphData.bitmapBuffer;
}

Devel::PixelBuffer Typesetter::CombineImageBuffer(Devel::PixelBuffer topPixelBuffer, Devel::PixelBuffer bottomPixelBuffer, const unsigned int bufferWidth, const unsigned int bufferHeight)
{
  unsigned char* topBuffer    = topPixelBuffer.GetBuffer();
  unsigned char* bottomBuffer = bottomPixelBuffer.GetBuffer();

  Devel::PixelBuffer combinedPixelBuffer;

  if(topBuffer == NULL && bottomBuffer == NULL)
  {
    // Nothing to do if both buffers are empty.
    return combinedPixelBuffer;
  }

  if(topBuffer == NULL)
  {
    // Nothing to do if topBuffer is empty.
    return bottomPixelBuffer;
  }

  if(bottomBuffer == NULL)
  {
    // Nothing to do if bottomBuffer is empty.
    return topPixelBuffer;
  }

  // Always combine two RGBA images
  const unsigned int bufferSizeInt  = bufferWidth * bufferHeight;
  const unsigned int bufferSizeChar = 4u * bufferSizeInt;

  combinedPixelBuffer     = Devel::PixelBuffer::New(bufferWidth, bufferHeight, Pixel::RGBA8888);
  uint8_t* combinedBuffer = reinterpret_cast<uint8_t*>(combinedPixelBuffer.GetBuffer());
  memset(combinedBuffer, 0u, bufferSizeChar);

  for(unsigned int pixelIndex = 0; pixelIndex < bufferSizeInt; pixelIndex++)
  {
    // If the alpha of the pixel in either buffer is not fully opaque, blend the two pixels.
    // Otherwise, copy pixel from topBuffer to combinedBuffer.

    unsigned int alphaBuffer1 = topBuffer[pixelIndex * 4 + 3];

    if(alphaBuffer1 != 255)
    {
      // At least one pixel is not fully opaque
      // "Over" blend the the pixel from topBuffer with the pixel in bottomBuffer
      combinedBuffer[pixelIndex * 4]     = topBuffer[pixelIndex * 4] + (bottomBuffer[pixelIndex * 4] * (255 - topBuffer[pixelIndex * 4 + 3]) / 255);
      combinedBuffer[pixelIndex * 4 + 1] = topBuffer[pixelIndex * 4 + 1] + (bottomBuffer[pixelIndex * 4 + 1] * (255 - topBuffer[pixelIndex * 4 + 3]) / 255);
      combinedBuffer[pixelIndex * 4 + 2] = topBuffer[pixelIndex * 4 + 2] + (bottomBuffer[pixelIndex * 4 + 2] * (255 - topBuffer[pixelIndex * 4 + 3]) / 255);
      combinedBuffer[pixelIndex * 4 + 3] = topBuffer[pixelIndex * 4 + 3] + (bottomBuffer[pixelIndex * 4 + 3] * (255 - topBuffer[pixelIndex * 4 + 3]) / 255);
    }
    else
    {
      // Copy the pixel from topBuffer to combinedBuffer
      combinedBuffer[pixelIndex * 4]     = topBuffer[pixelIndex * 4];
      combinedBuffer[pixelIndex * 4 + 1] = topBuffer[pixelIndex * 4 + 1];
      combinedBuffer[pixelIndex * 4 + 2] = topBuffer[pixelIndex * 4 + 2];
      combinedBuffer[pixelIndex * 4 + 3] = topBuffer[pixelIndex * 4 + 3];
    }
  }

  return combinedPixelBuffer;
}

Devel::PixelBuffer Typesetter::ApplyMarkupProcessorOnPixelBuffer(Devel::PixelBuffer topPixelBuffer, const unsigned int bufferWidth, const unsigned int bufferHeight, bool ignoreHorizontalAlignment, Pixel::Format pixelFormat, int horizontalOffset, int verticalOffset)
{
  // Apply the markup-Processor if enabled
  const bool markupProcessorEnabled = mModel->IsMarkupProcessorEnabled();
  if(markupProcessorEnabled)
  {
    // Underline-tags (this is for Markup case)
    // Get the underline runs.
    const Length     numberOfUnderlineRuns = mModel->GetNumberOfUnderlineRuns();
    Vector<GlyphRun> underlineRuns;
    underlineRuns.Resize(numberOfUnderlineRuns);
    mModel->GetUnderlineRuns(underlineRuns.Begin(), 0u, numberOfUnderlineRuns);

    // Iterate on the consecutive underlined glyph run and connect them into one chunk of underlined characters.
    Vector<GlyphRun>::ConstIterator itGlyphRun    = underlineRuns.Begin();
    Vector<GlyphRun>::ConstIterator endItGlyphRun = underlineRuns.End();
    GlyphIndex                      startGlyphIndex, endGlyphIndex;

    //The outer loop to iterate on the separated chunks of underlined glyph runs
    while(itGlyphRun != endItGlyphRun)
    {
      startGlyphIndex = itGlyphRun->glyphIndex;
      endGlyphIndex   = startGlyphIndex;
      //The inner loop to make a connected underline for the consecutive characters
      do
      {
        endGlyphIndex += itGlyphRun->numberOfGlyphs;
        itGlyphRun++;
      } while(itGlyphRun != endItGlyphRun && itGlyphRun->glyphIndex == endGlyphIndex);

      endGlyphIndex--;

      // Create the image buffer for underline
      Devel::PixelBuffer underlineImageBuffer = CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_UNDERLINE, ignoreHorizontalAlignment, pixelFormat, horizontalOffset, verticalOffset, startGlyphIndex, endGlyphIndex);
      // Combine the two buffers
      topPixelBuffer = CombineImageBuffer(topPixelBuffer, underlineImageBuffer, bufferWidth, bufferHeight);
    }
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

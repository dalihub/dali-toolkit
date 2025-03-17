/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/math-utils.h>
#include <memory.h>
#include <cmath>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/text-controls/text-label-devel.h>
#include <dali-toolkit/internal/text/glyph-metrics-helper.h>
#include <dali-toolkit/internal/text/line-helper-functions.h>
#include <dali-toolkit/internal/text/rendering/styles/character-spacing-helper-functions.h>
#include <dali-toolkit/internal/text/rendering/styles/strikethrough-helper-functions.h>
#include <dali-toolkit/internal/text/rendering/styles/underline-helper-functions.h>
#include <dali-toolkit/internal/text/rendering/text-typesetter-impl.h>
#include <dali-toolkit/internal/text/rendering/view-model.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_TEXT_PERFORMANCE_MARKER, false);

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
 * @brief Fast multiply & Summation & divide by 255.
 *
 * @param x1 The value between [0..255]
 * @param y1 The value between [0..255]
 * @param x2 The value between [0..255]
 * @param y2 The value between [0..255]
 * @return min(255, (x1*y1)/255 + (x2*y2)/255)
 */
inline uint8_t MultiplyAndSummationAndNormalizeColor(const uint8_t x1, const uint8_t y1, const uint8_t x2, const uint8_t y2) noexcept
{
  const uint32_t xy1 = static_cast<const uint32_t>(x1) * y1;
  const uint32_t xy2 = static_cast<const uint32_t>(x2) * y2;
  const uint32_t res = std::min(65025u, xy1 + xy2); // 65025 is 255 * 255.
  return ((res + ((res + 257) >> 8)) >> 8);         // fast divide by 255.
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

  if(topBuffer == nullptr && bottomBuffer == nullptr)
  {
    // Nothing to do if both buffers are empty.
    return;
  }

  if(topBuffer == nullptr)
  {
    // Nothing to do if topBuffer is empty.
    // If we need to store the result into top, change topPixelBuffer as bottomPixelBuffer.
    if(storeResultIntoTop)
    {
      topPixelBuffer = bottomPixelBuffer;
    }
    return;
  }

  if(bottomBuffer == nullptr)
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
  return mImpl->GetViewModel();
}

void Typesetter::SetFontClient(TextAbstraction::FontClient& fontClient)
{
  mImpl->SetFontClient(fontClient);
}

PixelData Typesetter::Render(const Vector2& size, Toolkit::DevelText::TextDirection::Type textDirection, RenderBehaviour behaviour, bool ignoreHorizontalAlignment, Pixel::Format pixelFormat)
{
  Devel::PixelBuffer result    = RenderWithPixelBuffer(size, textDirection, behaviour, ignoreHorizontalAlignment, pixelFormat);
  PixelData          pixelData = Devel::PixelBuffer::Convert(result);

  return pixelData;
}

PixelData Typesetter::RenderWithCutout(const Vector2& size, Toolkit::DevelText::TextDirection::Type textDirection, Devel::PixelBuffer mask, RenderBehaviour behaviour, bool ignoreHorizontalAlignment, Pixel::Format pixelFormat, float originAlpha)
{
  Devel::PixelBuffer result = RenderWithPixelBuffer(size, textDirection, behaviour, ignoreHorizontalAlignment, pixelFormat);
  SetMaskForImageBuffer(mask, result, size.width, size.height, originAlpha);

  PixelData pixelData = Devel::PixelBuffer::Convert(result);

  return pixelData;
}

Devel::PixelBuffer Typesetter::RenderWithPixelBuffer(const Vector2& size, Toolkit::DevelText::TextDirection::Type textDirection, RenderBehaviour behaviour, bool ignoreHorizontalAlignment, Pixel::Format pixelFormat)
{
  DALI_TRACE_SCOPE(gTraceFilter, "DALI_TEXT_RENDERING_TYPESETTER");
  // @todo. This initial implementation for a TextLabel has only one visible page.

  // Use l-value to make ensure it is not nullptr, so compiler happy.
  auto& viewModel = *(mImpl->GetViewModel());

  // Elides the text if needed.
  viewModel.ElideGlyphs(mImpl->GetFontClient());

  // Retrieves the layout size.
  const Size&   layoutSize   = viewModel.GetLayoutSize();
  const int32_t outlineWidth = static_cast<int32_t>(viewModel.GetOutlineWidth());

  // Set the offset for the horizontal alignment according to the text direction and outline width.
  int32_t penX = 0;
  switch(viewModel.GetHorizontalAlignment())
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
  switch(viewModel.GetVerticalAlignment())
  {
    case VerticalAlignment::TOP:
    {
      // No offset to add.
      break;
    }
    case VerticalAlignment::CENTER:
    {
      penY = static_cast<int32_t>(std::round(0.5f * (size.height - layoutSize.height)));
      penY = penY < 0.f ? 0.f : penY;

      break;
    }
    case VerticalAlignment::BOTTOM:
    {
      penY = static_cast<int32_t>(size.height - layoutSize.height);
      break;
    }
  }

  const bool isCutoutEnabled = viewModel.IsCutoutEnabled();
  if(isCutoutEnabled)
  {
    Vector2 offset = viewModel.GetOffsetWithCutout();
    penX           = offset.x;
    penY           = offset.y;
  }

  // Generate the image buffers of the text for each different style first,
  // then combine all of them together as one final image buffer. We try to
  // do all of these in CPU only, so that once the final texture is generated,
  // no calculation is needed in GPU during each frame.

  const uint32_t bufferWidth  = static_cast<uint32_t>(size.width);
  const uint32_t bufferHeight = static_cast<uint32_t>(size.height);

  const uint32_t bufferSizeInt  = bufferWidth * bufferHeight;
  const size_t   bufferSizeChar = sizeof(uint32_t) * static_cast<std::size_t>(bufferSizeInt);

  //Elided text in ellipsis at START could start on index greater than 0
  auto startIndexOfGlyphs = viewModel.GetStartIndexOfElidedGlyphs();
  auto endIndexOfGlyphs   = viewModel.GetEndIndexOfElidedGlyphs();

  Devel::PixelBuffer imageBuffer;

  if(RENDER_MASK == behaviour)
  {
    // Generate the image buffer as an alpha mask for color glyphs.
    imageBuffer = mImpl->CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_MASK, ignoreHorizontalAlignment, pixelFormat, penX, penY, startIndexOfGlyphs, endIndexOfGlyphs);
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
    imageBuffer = mImpl->CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_NONE, ignoreHorizontalAlignment, pixelFormat, penX, penY, startIndexOfGlyphs, endIndexOfGlyphs);
  }

  if((RENDER_NO_STYLES != behaviour) && (RENDER_MASK != behaviour))
  {
    // Generate the outline if enabled
    const uint16_t outlineWidth = viewModel.GetOutlineWidth();
    const float    outlineAlpha = viewModel.GetOutlineColor().a;
    if(outlineWidth != 0u && fabsf(outlineAlpha) > Math::MACHINE_EPSILON_1 && RENDER_OVERLAY_STYLE != behaviour)
    {
      // Create the image buffer for outline
      Devel::PixelBuffer outlineImageBuffer = mImpl->CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_OUTLINE, ignoreHorizontalAlignment, pixelFormat, penX, penY, startIndexOfGlyphs, endIndexOfGlyphs);

      const float& blurRadius = viewModel.GetOutlineBlurRadius();

      if(blurRadius > Math::MACHINE_EPSILON_1)
      {
        outlineImageBuffer.ApplyGaussianBlur(blurRadius);
      }

      // Combine the two buffers
      CombineImageBuffer(imageBuffer, outlineImageBuffer, bufferWidth, bufferHeight, true);
    }

    // @todo. Support shadow for partial text later on.

    // Generate the shadow if enabled
    const Vector2& shadowOffset = viewModel.GetShadowOffset();
    const float    shadowAlpha  = viewModel.GetShadowColor().a;
    if(RENDER_OVERLAY_STYLE != behaviour && fabsf(shadowAlpha) > Math::MACHINE_EPSILON_1 && (fabsf(shadowOffset.x) > Math::MACHINE_EPSILON_1 || fabsf(shadowOffset.y) > Math::MACHINE_EPSILON_1))
    {
      // Create the image buffer for shadow
      Devel::PixelBuffer shadowImageBuffer = mImpl->CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_SHADOW, ignoreHorizontalAlignment, pixelFormat, penX, penY, startIndexOfGlyphs, endIndexOfGlyphs);

      // Check whether it will be a soft shadow
      const float& blurRadius = viewModel.GetShadowBlurRadius();

      if(blurRadius > Math::MACHINE_EPSILON_1)
      {
        shadowImageBuffer.ApplyGaussianBlur(blurRadius);
      }

      // Combine the two buffers
      CombineImageBuffer(imageBuffer, shadowImageBuffer, bufferWidth, bufferHeight, true);
    }

    // Generate the background if enabled
    const bool backgroundEnabled   = viewModel.IsBackgroundEnabled();
    const bool backgroundMarkupSet = viewModel.IsMarkupBackgroundColorSet();
    if((backgroundEnabled || backgroundMarkupSet) && RENDER_OVERLAY_STYLE != behaviour)
    {
      Devel::PixelBuffer backgroundImageBuffer;

      if(backgroundEnabled)
      {
        backgroundImageBuffer = mImpl->CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_BACKGROUND, ignoreHorizontalAlignment, pixelFormat, penX, penY, startIndexOfGlyphs, endIndexOfGlyphs);
      }
      else
      {
        backgroundImageBuffer = Impl::CreateTransparentImageBuffer(bufferWidth, bufferHeight, pixelFormat);
      }

      if(backgroundMarkupSet)
      {
        mImpl->DrawGlyphsBackground(backgroundImageBuffer, bufferWidth, bufferHeight, ignoreHorizontalAlignment, penX, penY);
      }

      // Combine the two buffers
      CombineImageBuffer(imageBuffer, backgroundImageBuffer, bufferWidth, bufferHeight, true);
    }

    // Generate the background_with_mask if enabled
    const bool backgroundWithCutoutEnabled = viewModel.IsBackgroundWithCutoutEnabled();
    if((backgroundWithCutoutEnabled) && RENDER_OVERLAY_STYLE != behaviour)
    {
      Devel::PixelBuffer backgroundImageBuffer;

      backgroundImageBuffer = CreateFullBackgroundBuffer(bufferWidth, bufferHeight, viewModel.GetBackgroundColorWithCutout());

      // Combine the two buffers
      CombineImageBuffer(imageBuffer, backgroundImageBuffer, bufferWidth, bufferHeight, true);
    }

    if(RENDER_OVERLAY_STYLE == behaviour)
    {
      if(viewModel.IsUnderlineEnabled())
      {
        // Create the image buffer for underline
        Devel::PixelBuffer underlineImageBuffer = mImpl->CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_UNDERLINE, ignoreHorizontalAlignment, pixelFormat, penX, penY, startIndexOfGlyphs, endIndexOfGlyphs);

        // Combine the two buffers
        CombineImageBuffer(imageBuffer, underlineImageBuffer, bufferWidth, bufferHeight, true);
      }

      if(viewModel.IsStrikethroughEnabled())
      {
        // Create the image buffer for strikethrough
        Devel::PixelBuffer strikethroughImageBuffer = mImpl->CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_STRIKETHROUGH, ignoreHorizontalAlignment, pixelFormat, penX, penY, 0u, endIndexOfGlyphs);

        // Combine the two buffers
        CombineImageBuffer(imageBuffer, strikethroughImageBuffer, bufferWidth, bufferHeight, true);
      }

      // Markup-Processor for overlay styles
      if(viewModel.IsMarkupProcessorEnabled() || viewModel.IsSpannedTextPlaced())
      {
        if(viewModel.IsMarkupUnderlineSet())
        {
          imageBuffer = ApplyUnderlineMarkupImageBuffer(imageBuffer, bufferWidth, bufferHeight, ignoreHorizontalAlignment, pixelFormat, penX, penY);
        }

        if(viewModel.IsMarkupStrikethroughSet())
        {
          imageBuffer = ApplyStrikethroughMarkupImageBuffer(imageBuffer, bufferWidth, bufferHeight, ignoreHorizontalAlignment, pixelFormat, penX, penY);
        }
      }
    }
  }

  return imageBuffer;
}

Devel::PixelBuffer Typesetter::CreateFullBackgroundBuffer(const uint32_t bufferWidth, const uint32_t bufferHeight, const Vector4& backgroundColor)
{
  const uint32_t bufferSizeInt        = bufferWidth * bufferHeight;
  uint8_t        backgroundColorAlpha = static_cast<uint8_t>(backgroundColor.a * 255.f);

  Devel::PixelBuffer buffer = Devel::PixelBuffer::New(bufferWidth, bufferHeight, Pixel::RGBA8888);

  uint32_t* bitmapBuffer = reinterpret_cast<uint32_t*>(buffer.GetBuffer());

  uint32_t packedBackgroundColor       = 0u;
  uint8_t* packedBackgroundColorBuffer = reinterpret_cast<uint8_t*>(&packedBackgroundColor);

  // Write the color to the pixel buffer
  *(packedBackgroundColorBuffer + 3u) = backgroundColorAlpha;
  *(packedBackgroundColorBuffer + 2u) = static_cast<uint8_t>(backgroundColor.b * backgroundColorAlpha);
  *(packedBackgroundColorBuffer + 1u) = static_cast<uint8_t>(backgroundColor.g * backgroundColorAlpha);
  *(packedBackgroundColorBuffer)      = static_cast<uint8_t>(backgroundColor.r * backgroundColorAlpha);

  std::fill(bitmapBuffer, bitmapBuffer + bufferSizeInt, packedBackgroundColor);

  return buffer;
}

Devel::PixelBuffer Typesetter::ApplyUnderlineMarkupImageBuffer(Devel::PixelBuffer topPixelBuffer, const uint32_t bufferWidth, const uint32_t bufferHeight, const bool ignoreHorizontalAlignment, const Pixel::Format pixelFormat, const int32_t horizontalOffset, const int32_t verticalOffset)
{
  // Use l-value to make ensure it is not nullptr, so compiler happy.
  auto& viewModel = *(mImpl->GetViewModel());

  // Underline-tags (this is for Markup case)
  // Get the underline runs.
  const Length               numberOfUnderlineRuns = viewModel.GetNumberOfUnderlineRuns();
  Vector<UnderlinedGlyphRun> underlineRuns;
  underlineRuns.Resize(numberOfUnderlineRuns);
  viewModel.GetUnderlineRuns(underlineRuns.Begin(), 0u, numberOfUnderlineRuns);

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
    Devel::PixelBuffer underlineImageBuffer = mImpl->CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_UNDERLINE, ignoreHorizontalAlignment, pixelFormat, horizontalOffset, verticalOffset, startGlyphIndex, endGlyphIndex);
    // Combine the two buffers
    // Result pixel buffer will be stored into topPixelBuffer.
    CombineImageBuffer(underlineImageBuffer, topPixelBuffer, bufferWidth, bufferHeight, false);

    itGlyphRun++;
  }

  return topPixelBuffer;
}

Devel::PixelBuffer Typesetter::ApplyStrikethroughMarkupImageBuffer(Devel::PixelBuffer topPixelBuffer, const uint32_t bufferWidth, const uint32_t bufferHeight, const bool ignoreHorizontalAlignment, const Pixel::Format pixelFormat, const int32_t horizontalOffset, const int32_t verticalOffset)
{
  // Use l-value to make ensure it is not nullptr, so compiler happy.
  auto& viewModel = *(mImpl->GetViewModel());

  // strikethrough-tags (this is for Markup case)
  // Get the strikethrough runs.
  const Length                  numberOfStrikethroughRuns = viewModel.GetNumberOfStrikethroughRuns();
  Vector<StrikethroughGlyphRun> strikethroughRuns;
  strikethroughRuns.Resize(numberOfStrikethroughRuns);
  viewModel.GetStrikethroughRuns(strikethroughRuns.Begin(), 0u, numberOfStrikethroughRuns);

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
    Devel::PixelBuffer strikethroughImageBuffer = mImpl->CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_STRIKETHROUGH, ignoreHorizontalAlignment, pixelFormat, horizontalOffset, verticalOffset, startGlyphIndex, endGlyphIndex);
    // Combine the two buffers
    // Result pixel buffer will be stored into topPixelBuffer.
    CombineImageBuffer(strikethroughImageBuffer, topPixelBuffer, bufferWidth, bufferHeight, false);

    itGlyphRun++;
  }

  return topPixelBuffer;
}

void Typesetter::SetMaskForImageBuffer(Devel::PixelBuffer& __restrict__ topPixelBuffer, Devel::PixelBuffer& __restrict__ bottomPixelBuffer, const uint32_t bufferWidth, const uint32_t bufferHeight, float originAlpha)
{
  // Assume that we always combine two RGBA images
  // Jump with 4bytes for optimize runtime.
  uint32_t* topBuffer    = reinterpret_cast<uint32_t*>(topPixelBuffer.GetBuffer());
  uint32_t* bottomBuffer = reinterpret_cast<uint32_t*>(bottomPixelBuffer.GetBuffer());

  if(topBuffer == nullptr || bottomBuffer == nullptr)
  {
    // Nothing to do if one of both buffers are empty.
    return;
  }

  const uint32_t bufferSizeInt = bufferWidth * bufferHeight;

  for(uint32_t pixelIndex = 0; pixelIndex < bufferSizeInt; ++pixelIndex)
  {
    uint32_t topBufferColor                       = *(topBuffer);
    uint32_t bottomBufferColor                    = *(bottomBuffer);
    uint8_t* __restrict__ topBufferColorBuffer    = reinterpret_cast<uint8_t*>(&topBufferColor);
    uint8_t* __restrict__ bottomBufferColorBuffer = reinterpret_cast<uint8_t*>(&bottomBufferColor);

    // Return the transparency of the text to original.
    uint8_t originAlphaInt = originAlpha * 255;

    uint8_t topAlpha    = topBufferColorBuffer[3];
    uint8_t bottomAlpha = 255 - topAlpha;

    // Manual blending.
    bottomBufferColorBuffer[0] = MultiplyAndSummationAndNormalizeColor(topBufferColorBuffer[0], originAlphaInt, bottomBufferColorBuffer[0], bottomAlpha);
    bottomBufferColorBuffer[1] = MultiplyAndSummationAndNormalizeColor(topBufferColorBuffer[1], originAlphaInt, bottomBufferColorBuffer[1], bottomAlpha);
    bottomBufferColorBuffer[2] = MultiplyAndSummationAndNormalizeColor(topBufferColorBuffer[2], originAlphaInt, bottomBufferColorBuffer[2], bottomAlpha);
    bottomBufferColorBuffer[3] = MultiplyAndSummationAndNormalizeColor(topBufferColorBuffer[3], originAlphaInt, bottomBufferColorBuffer[3], bottomAlpha);

    *(bottomBuffer) = bottomBufferColor;

    // Increase each buffer's pointer.
    ++topBuffer;
    ++bottomBuffer;
  }
}

Typesetter::Typesetter(const ModelInterface* const model)
: mImpl{std::make_unique<Impl>(model)}
{
}

Typesetter::~Typesetter() = default;

} // namespace Text

} // namespace Toolkit

} // namespace Dali

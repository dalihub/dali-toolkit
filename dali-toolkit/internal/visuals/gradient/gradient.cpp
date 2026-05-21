/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include "gradient.h"

#include <dali/integration-api/texture-integ.h>
#include <dali/public-api/math/vector4.h>
#include <algorithm> // std::sort

namespace
{
// The maximum width of the lookup texture ( it is a 1-dimension texture with the height as 1 )
const uint32_t MAXIMUM_TEXTURE_RESOLUTION(512u);
} // namespace

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
Gradient::Gradient(IntrusivePtr<Gradient> oldGradient)
: mGradientUnits(Toolkit::GradientVisual::Units::OBJECT_BOUNDING_BOX),
  mSpreadMethod(Toolkit::GradientVisual::SpreadMethod::PAD),
  mStartOffset(0.f)
{
  if(oldGradient)
  {
    mGradientStops = oldGradient->GetStops();
    mGradientUnits = oldGradient->GetGradientUnits();
    mSpreadMethod  = oldGradient->GetSpreadMethod();
    mStartOffset   = oldGradient->GetStartOffset();
  }
}

Gradient::~Gradient()
{
}

const Vector<Gradient::GradientStop>& Gradient::GetStops() const
{
  return mGradientStops;
}

void Gradient::ClearStops()
{
  mGradientStops.Clear();
}

void Gradient::ApplyStops(const Vector<float>& offsets, const Vector<Vector4>& colors)
{
  uint32_t numStops = std::max(mGradientStops.Count(), std::max(offsets.Count(), colors.Count()));
  mGradientStops.Resize(numStops);

  for(uint32_t i = 0; i < offsets.Count(); ++i)
  {
    // the offset is clamped to the range [0.0, 1.0]
    mGradientStops[i].mOffset = Clamp(offsets[i], 0.f, 1.f);
  }
  for(uint32_t i = 0; i < colors.Count(); ++i)
  {
    mGradientStops[i].mStopColor = colors[i];
  }
}

void Gradient::SetGradientUnits(Toolkit::GradientVisual::Units::Type gradientUnits)
{
  mGradientUnits = gradientUnits;
}

Toolkit::GradientVisual::Units::Type Gradient::GetGradientUnits() const
{
  return mGradientUnits;
}

void Gradient::SetSpreadMethod(Toolkit::GradientVisual::SpreadMethod::Type spread)
{
  mSpreadMethod = spread;
}

Toolkit::GradientVisual::SpreadMethod::Type Gradient::GetSpreadMethod() const
{
  return mSpreadMethod;
}

const Matrix3& Gradient::GetAlignmentTransform() const
{
  return mAlignmentTransform;
}

/**
 * Following the SVG gradient.
 *
 * Not only the spread method decides the texture wrap mode:
 *    PAD-->GL_CLAMP_TO_EDGE; REPEAT-->GL_REPEAT; REFLECT-->GL_MIRROR_REPEAT
 *
 *  If the stops have not covered the whole zero to one range,
 *  the REPEAT spread behaves different from the two others in the lookup texture generation.
 */
Dali::Texture Gradient::GenerateLookupTexture()
{
  uint32_t numStops = mGradientStops.Count();
  if(DALI_UNLIKELY(numStops <= 0))
  {
    return Dali::Texture();
  }

  // Fill -1 offsets automatically.
  if(mGradientStops[0].mOffset < 0.0f)
  {
    mGradientStops[0].mOffset = 0.0f;
  }
  if(mGradientStops[numStops - 1].mOffset < 0.0f)
  {
    mGradientStops[numStops - 1].mOffset = 1.0f;
  }
  for(uint32_t i = 0u; i < numStops - 1;)
  {
    uint32_t j = i + 1;
    for(; j < numStops; ++j)
    {
      if(mGradientStops[j].mOffset >= 0.0f)
      {
        break;
      }
    }
    if(j == i + 1)
    {
      ++i;
      continue;
    }
    const float offsetBegin = mGradientStops[i].mOffset;
    const float offsetEnd   = mGradientStops[j].mOffset;
    for(uint32_t k = i + 1; k < j; ++k)
    {
      mGradientStops[k].mOffset = offsetBegin + (offsetEnd - offsetBegin) * static_cast<float>(k - i) / (j - i);
    }
    i = j;
  }

  std::sort(mGradientStops.Begin(), mGradientStops.End());

  // Copy the stops to use pre-multiplied colors during texture generation. Also allow we can use additional stops if we need.
  Vector<float>   offsets;
  Vector<Vector4> preMulipliedColors;
  bool            hasAlpha = false;

  offsets.Resize(numStops);
  preMulipliedColors.Resize(numStops);
  for(uint32_t i = 0u; i < numStops; ++i)
  {
    offsets[i]            = mGradientStops[i].mOffset;
    preMulipliedColors[i] = mGradientStops[i].mStopColor;

    if(!Equals(preMulipliedColors[i].a, 1.0f, Math::MACHINE_EPSILON_1))
    {
      hasAlpha = true;
      preMulipliedColors[i].r *= preMulipliedColors[i].a;
      preMulipliedColors[i].g *= preMulipliedColors[i].a;
      preMulipliedColors[i].b *= preMulipliedColors[i].a;
    }
  }

  /**
   * If the stops have not covered the whole zero to one range,
   * for PAD and REFLECT, use the color of the first stop to fill the range  [0.0, first stop offset)
   *                  and use the color of the last stop to fill the range (last stop offset, 1.0]
   * for REPEAT, mix the two color of the first and last stop to fill the remainder
   */
  if(offsets[0] > 0.f)
  {
    Vector4 firstStopColor(preMulipliedColors[0]); // If spread method is PAD or REFLECT
    if(mSpreadMethod == Toolkit::GradientVisual::SpreadMethod::REPEAT)
    {
      Vector4 preMulipliedFirstColor = preMulipliedColors[0];
      Vector4 preMulipliedLastColor  = preMulipliedColors[numStops - 1];

      firstStopColor = (preMulipliedFirstColor * (1.f - offsets[numStops - 1]) +
                        preMulipliedLastColor * offsets[0]) /
                       (offsets[0] + 1.f - offsets[numStops - 1]);
    }

    offsets.Insert(offsets.Begin(), 0.0f);
    preMulipliedColors.Insert(preMulipliedColors.Begin(), firstStopColor);
    numStops++;
  }

  if(offsets[numStops - 1] < 1.f)
  {
    Vector4 lastStopColor(preMulipliedColors[numStops - 1]); // If spread method is PAD or REFLECT
    if(mSpreadMethod == Toolkit::GradientVisual::SpreadMethod::REPEAT)
    {
      lastStopColor = preMulipliedColors[0];
    }
    offsets.PushBack(1.0f);
    preMulipliedColors.PushBack(lastStopColor);
    numStops++;
  }

  /**
   * Generate the pixels with the color transit from one stop to next.
   */
  unsigned int resolution = EstimateTextureResolution();

  const Pixel::Format pixelFormat = hasAlpha ? Pixel::RGBA8888 : Pixel::RGB888;
  const uint32_t      bpp         = Pixel::GetBytesPerPixel(pixelFormat);

  unsigned int   bufferSize = resolution * bpp;
  unsigned char* pixels     = new unsigned char[bufferSize];
  PixelData      pixelData  = PixelData::New(pixels, bufferSize, resolution, 1u, pixelFormat, PixelData::DELETE_ARRAY);

  int   segmentStart = 0;
  int   segmentEnd   = 0;
  int   k            = 0;
  float length       = static_cast<float>(resolution);
  for(unsigned int i = 0; i < numStops - 1u; i++)
  {
    const float offsetEnd = std::min(1.0f, std::max(0.0f, offsets[i + 1]));

    segmentEnd = floorf(offsetEnd * length + 0.5f);
    if(segmentEnd == segmentStart)
    {
      continue;
    }
    float segmentWidth = static_cast<float>(segmentEnd - segmentStart);

    Vector4 preMulipliedStartColor = preMulipliedColors[i];
    Vector4 preMulipliedEndColor   = preMulipliedColors[i + 1];

    for(int j = segmentStart; j < segmentEnd; j++)
    {
      float   ratio        = static_cast<float>(j - segmentStart) / (segmentWidth - 1);
      Vector4 currentColor = preMulipliedStartColor * (1.f - ratio) + preMulipliedEndColor * ratio;
      pixels[k * bpp]      = static_cast<unsigned char>(255.f * Clamp(currentColor.r, 0.f, 1.f));
      pixels[k * bpp + 1]  = static_cast<unsigned char>(255.f * Clamp(currentColor.g, 0.f, 1.f));
      pixels[k * bpp + 2]  = static_cast<unsigned char>(255.f * Clamp(currentColor.b, 0.f, 1.f));
      if(hasAlpha)
      {
        pixels[k * bpp + 3] = static_cast<unsigned char>(255.f * Clamp(currentColor.a, 0.f, 1.f));
      }
      k++;
    }
    segmentStart = segmentEnd;
  }

  Texture texture = Texture::New(TextureType::TEXTURE_2D, pixelFormat, resolution, 1u);
#if defined(GPU_MEMORY_PROFILE_ENABLED)
  Dali::Integration::TextureUploadWithContent(texture, pixelData, "gradient", Dali::Integration::TextureContextTypeHint::GRADIENT_TEXTURE);
#else
  texture.Upload(pixelData);
#endif

  return texture;
}

unsigned int Gradient::EstimateTextureResolution()
{
  return MAXIMUM_TEXTURE_RESOLUTION;
}

void Gradient::SetStartOffset(float startOffset)
{
  mStartOffset = startOffset;
}

float Gradient::GetStartOffset() const
{
  return mStartOffset;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

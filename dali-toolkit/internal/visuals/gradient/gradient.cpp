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

// CLASS HEADER
#include <dali-toolkit/internal/visuals/gradient/gradient.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/texture-integ.h>
#include <dali/public-api/common/dali-utility.h>
#include <dali/public-api/math/vector4.h>
#include <algorithm> // std::sort

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/gradient/gradient-texture-cache.h>

namespace DALI_NAMESPACE
{
namespace Toolkit
{
namespace Internal
{
Gradient::Gradient(IntrusivePtr<Gradient> oldGradient)
: mGradientUnits(Toolkit::GradientVisual::Units::OBJECT_BOUNDING_BOX),
  mSpreadMethod(Toolkit::GradientVisual::SpreadMethod::PAD),
  mStartOffset(0.f),
  mStopsNormalized(false)
{
  if(oldGradient)
  {
    mGradientStops   = oldGradient->GetStops();
    mStopsNormalized = oldGradient->mStopsNormalized;
    mGradientUnits   = oldGradient->GetGradientUnits();
    mSpreadMethod    = oldGradient->GetSpreadMethod();
    mStartOffset     = oldGradient->GetStartOffset();
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
  mStopsNormalized = false;
}

void Gradient::ApplyStops(const Vector<float>& offsets, const Vector<Vector4>& colors)
{
  mStopsNormalized  = false;
  uint32_t numStops = Max(mGradientStops.Count(), Max(offsets.Count(), colors.Count()));
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

void Gradient::NormalizeStops()
{
  if(mStopsNormalized)
  {
    return;
  }
  mStopsNormalized  = true;
  uint32_t numStops = static_cast<uint32_t>(mGradientStops.Count());
  if(numStops == 0u)
  {
    return;
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
}

Dali::Texture Gradient::GenerateLookupTexture()
{
  NormalizeStops();
  return GradientTextureCache::GenerateTexture(*this);
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

} //namespace DALI_NAMESPACE

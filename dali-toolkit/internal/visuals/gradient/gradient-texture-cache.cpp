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
#include <dali-toolkit/internal/visuals/gradient/gradient-texture-cache.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/common/free-list.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/processor-interface.h>
#include <dali/integration-api/string-utils.h>
#include <dali/integration-api/texture-integ.h>

#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <utility>
#include <vector>

#include <iomanip>
#include <locale>
#include <sstream>
#include <string>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/gradient/gradient.h>

namespace DALI_NAMESPACE
{
namespace Toolkit
{
namespace Internal
{
namespace
{
constexpr uint32_t GRADIENT_TEXTURE_SIZE = 512;

/**
 * @brief Formats gradient stops for the GPU memory profile using locale-independent values.
 * @param[in] stops The gradient stops containing offsets and unpremultiplied colors
 * @return The offset and RGBA color pairs followed by the stop count
 */
std::string FormatStops(const Vector<Gradient::GradientStop>& stops)
{
  std::ostringstream content;
  content.imbue(std::locale::classic());
  for(uint32_t index = 0u; index < stops.Count(); ++index)
  {
    const auto&        stop = stops[index];
    std::ostringstream offsetStream;
    offsetStream.imbue(std::locale::classic());
    offsetStream << std::fixed << std::setprecision(6) << stop.mOffset;
    std::string offset          = offsetStream.str();
    const auto  decimalPosition = offset.find('.');
    if(decimalPosition != std::string::npos)
    {
      while(offset.size() > decimalPosition + 2u && offset.back() == '0')
      {
        offset.pop_back();
      }
    }
    if(offset == "-0.0")
    {
      offset = "0.0";
    }
    if(index != 0u)
    {
      content << '-';
    }
    content << '(' << offset << ",#" << std::hex << std::uppercase << std::setfill('0');
    for(uint32_t component = 0u; component < 4u; ++component)
    {
      content << std::setw(2) << static_cast<uint32_t>(255.f * Clamp(stop.mStopColor[component], 0.f, 1.f));
    }
    content << ')';
  }
  content << '/' << std::dec << stops.Count();
  return content.str();
}

// Hash quantized integers, then compare original values on hash collisions.
size_t HashStops(const Vector<Gradient::GradientStop>& stops, bool repeat)
{
  size_t hash    = stops.Count();
  auto   Combine = [&hash](uint32_t value)
  {
    hash ^= value + 0x9e3779b9u + (hash << 6u) + (hash >> 2u);
  };
  Combine(repeat);
  for(const auto& stop : stops)
  {
    Combine(static_cast<uint32_t>(Clamp(stop.mOffset, 0.f, 1.f) * 65535.f));
    for(uint32_t component = 0; component < 4u; ++component)
    {
      Combine(static_cast<uint32_t>(Clamp(stop.mStopColor[component], 0.f, 1.f) * 255.f));
    }
  }
  return hash;
}

bool EqualStops(const Vector<Gradient::GradientStop>& lhs, const Vector<Gradient::GradientStop>& rhs)
{
  if(lhs.Count() != rhs.Count())
  {
    return false;
  }
  for(uint32_t i = 0u; i < lhs.Count(); ++i)
  {
    if(lhs[i].mOffset != rhs[i].mOffset)
    {
      return false;
    }
    for(uint32_t component = 0u; component < 4u; ++component)
    {
      if(lhs[i].mStopColor[component] != rhs[i].mStopColor[component])
      {
        return false;
      }
    }
  }
  return true;
}
} // namespace

/**
 * @brief Owns gradient cache entries, ID generation and deferred release processing.
 */
struct GradientTextureCache::Impl : public Dali::Integration::Processor
{
  /**
   * @brief Stores the lookup key, texture and reference count for one cache entry.
   */
  struct Entry
  {
    Vector<Gradient::GradientStop> stops;
    Texture                        texture;
    size_t                         hash{0u};
    Id                             id{INVALID_ID};
    uint32_t                       references{0u};
    bool                           repeat{false};
  };

  ~Impl() override
  {
    if(mReleaseProcessorRegistered && DALI_LIKELY(Dali::Adaptor::IsAvailable()))
    {
      Dali::Adaptor::Get().UnregisterProcessorOnce(*this, true);
    }
  }

  /**
   * @brief Resolves an active ID to its index in the contiguous entry container.
   * @param[in] id The lookup texture ID
   * @return The entry index, or INVALID_INDEX for an invalid or released ID
   */
  uint32_t GetEntryIndex(Id id) const
  {
    const uint32_t key = static_cast<uint32_t>(id);
    if(id == INVALID_ID || key >= mAllocatedIdCount)
    {
      return INVALID_INDEX;
    }
    const uint32_t index = mIds[key];
    if(index >= mEntries.size() || mEntries[index].id != id)
    {
      return INVALID_INDEX;
    }
    return index;
  }

  /**
   * @brief Queues a reference release and schedules post-processing if needed.
   * @param[in] id The cached lookup texture ID
   */
  void RequestRelease(Id id)
  {
    if(GetEntryIndex(id) == INVALID_INDEX)
    {
      return;
    }
    mReleaseQueue.push_back(id);
    if(!mReleaseProcessorRegistered && DALI_LIKELY(Dali::Adaptor::IsAvailable()))
    {
      mReleaseProcessorRegistered = true;
      Dali::Adaptor::Get().RegisterProcessorOnce(*this, true);
    }
  }

private:
  /**
   * @copydoc Dali::Integration::Processor::Process()
   */
  void Process(bool postProcessor) override
  {
    mReleaseProcessorRegistered = false;
    if(DALI_UNLIKELY(!Dali::Adaptor::IsAvailable()))
    {
      return;
    }
    for(Id id : mReleaseQueue)
    {
      Release(id);
    }
    mReleaseQueue.clear();
  }

  /**
   * @copydoc Dali::Integration::Processor::GetProcessorName()
   */
  std::string_view GetProcessorName() const override
  {
    return "GradientTextureCache";
  }

  /**
   * @brief Releases one reference and removes an entry when its reference count reaches zero.
   * @param[in] id The lookup texture ID queued for release
   */
  void Release(Id id)
  {
    const uint32_t index = GetEntryIndex(id);
    if(index == INVALID_INDEX)
    {
      return;
    }
    auto& entry = mEntries[index];
    if(--entry.references != 0u)
    {
      return;
    }

    auto& ids = mHashIds[entry.hash];
    ids.erase(std::remove(ids.begin(), ids.end(), id), ids.end());
    if(ids.empty())
    {
      mHashIds.erase(entry.hash);
    }

    // Update the moved entry's ID mapping before swapping it into the hole.
    if(index + 1u < mEntries.size())
    {
      mIds[static_cast<uint32_t>(mEntries.back().id)] = index;
      std::swap(mEntries[index], mEntries.back());
    }
    mEntries.pop_back();
    mIds.Remove(static_cast<uint32_t>(id));
  }

  friend class GradientTextureCache;

  static constexpr uint32_t INVALID_INDEX = static_cast<uint32_t>(INVALID_ID);

  std::unordered_map<size_t, std::vector<Id>> mHashIds;                           ///< Hash buckets of stable lookup IDs.
  std::vector<Entry>                          mEntries;                           ///< Dense entries; removal swaps with the final element.
  std::vector<Id>                             mReleaseQueue;                      ///< References awaiting post-processing.
  Dali::FreeList                              mIds;                               ///< Converts an active ID to its current entry index.
  uint32_t                                    mAllocatedIdCount{0u};              ///< Upper bound for IDs ever allocated by the free list.
  bool                                        mReleaseProcessorRegistered{false}; ///< Whether a post-processor is registered.
};

GradientTextureCache::GradientTextureCache()
: mImpl(std::make_unique<Impl>())
{
}

GradientTextureCache::~GradientTextureCache() = default;

GradientTextureCache::Id GradientTextureCache::Acquire(Gradient& gradient)
{
  if(DALI_UNLIKELY(!Dali::Adaptor::IsAvailable()))
  {
    return INVALID_ID;
  }
  gradient.NormalizeStops();
  const auto& stops = gradient.GetStops();
  if(stops.Empty())
  {
    return INVALID_ID;
  }
  const bool repeat = gradient.GetSpreadMethod() == Toolkit::GradientVisual::SpreadMethod::REPEAT &&
                      (stops[0].mOffset > 0.f || stops[stops.Count() - 1u].mOffset < 1.f);
  const size_t hash  = HashStops(stops, repeat);
  auto         found = mImpl->mHashIds.find(hash);
  if(found != mImpl->mHashIds.end())
  {
    for(Id id : found->second)
    {
      auto& entry = mImpl->mEntries[mImpl->mIds[static_cast<uint32_t>(id)]];
      if(entry.repeat == repeat && EqualStops(entry.stops, stops))
      {
        ++entry.references;
        return entry.id;
      }
    }
  }
  const auto index = static_cast<uint32_t>(mImpl->mEntries.size());
  const auto key   = mImpl->mIds.Add(index);
  const Id   id    = static_cast<Id>(key);
  DALI_ASSERT_ALWAYS(id != INVALID_ID && "Gradient texture cache ID capacity exceeded");
  mImpl->mAllocatedIdCount = std::max(mImpl->mAllocatedIdCount, key + 1u);
  mImpl->mEntries.push_back(Impl::Entry{stops, GenerateTexture(gradient), hash, id, 1u, repeat});
  mImpl->mHashIds[hash].push_back(id);
  return id;
}

Texture GradientTextureCache::GetTexture(Id id) const
{
  if(DALI_UNLIKELY(!Dali::Adaptor::IsAvailable()))
  {
    return Texture();
  }
  const uint32_t index = mImpl->GetEntryIndex(id);
  if(index != Impl::INVALID_INDEX)
  {
    return mImpl->mEntries[index].texture;
  }
  return Texture();
}

void GradientTextureCache::RequestRelease(Id id)
{
  if(DALI_UNLIKELY(!Dali::Adaptor::IsAvailable()))
  {
    return;
  }
  mImpl->RequestRelease(id);
}

void GradientTextureCache::Reference(Id id)
{
  if(DALI_UNLIKELY(!Dali::Adaptor::IsAvailable()))
  {
    return;
  }
  const uint32_t index = mImpl->GetEntryIndex(id);
  if(index != Impl::INVALID_INDEX)
  {
    ++mImpl->mEntries[index].references;
  }
}

Texture GradientTextureCache::GenerateTexture(Gradient& gradient)
{
  if(DALI_UNLIKELY(!Dali::Adaptor::IsAvailable()))
  {
    return Texture();
  }
  const auto& stops        = gradient.GetStops();
  const auto  spreadMethod = gradient.GetSpreadMethod();
  uint32_t    numStops     = static_cast<uint32_t>(stops.Count());
  if(numStops == 0u)
  {
    return Texture();
  }
  // Copy the stops to use pre-multiplied colors during texture generation. Also allow we can use additional stops if we need.
  Vector<float>   offsets;
  Vector<Vector4> preMulipliedColors;
  bool            hasAlpha = false;

  offsets.Resize(numStops);
  preMulipliedColors.Resize(numStops);
  for(uint32_t i = 0u; i < numStops; ++i)
  {
    offsets[i]            = stops[i].mOffset;
    preMulipliedColors[i] = stops[i].mStopColor;

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
    if(spreadMethod == Toolkit::GradientVisual::SpreadMethod::REPEAT)
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
    if(spreadMethod == Toolkit::GradientVisual::SpreadMethod::REPEAT)
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
  const Pixel::Format pixelFormat = hasAlpha ? Pixel::RGBA8888 : Pixel::RGB888;
  const uint32_t      bpp         = Pixel::GetBytesPerPixel(pixelFormat);

  uint32_t  bufferSize = GRADIENT_TEXTURE_SIZE * bpp;
  uint8_t*  pixels     = new uint8_t[bufferSize];
  PixelData pixelData  = PixelData::New(pixels, bufferSize, GRADIENT_TEXTURE_SIZE, 1u, pixelFormat, PixelData::DELETE_ARRAY);

  int   segmentStart = 0;
  int   segmentEnd   = 0;
  int   k            = 0;
  float length       = static_cast<float>(GRADIENT_TEXTURE_SIZE);
  for(uint32_t i = 0; i < numStops - 1u; i++)
  {
    const float offsetEnd = Min(1.0f, Max(0.0f, offsets[i + 1]));

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
      float   ratio        = segmentWidth > 1.f ? static_cast<float>(j - segmentStart) / (segmentWidth - 1.f) : 0.f;
      Vector4 currentColor = preMulipliedStartColor * (1.f - ratio) + preMulipliedEndColor * ratio;
      pixels[k * bpp]      = static_cast<uint8_t>(255.f * Clamp(currentColor.r, 0.f, 1.f));
      pixels[k * bpp + 1]  = static_cast<uint8_t>(255.f * Clamp(currentColor.g, 0.f, 1.f));
      pixels[k * bpp + 2]  = static_cast<uint8_t>(255.f * Clamp(currentColor.b, 0.f, 1.f));
      if(hasAlpha)
      {
        pixels[k * bpp + 3] = static_cast<uint8_t>(255.f * Clamp(currentColor.a, 0.f, 1.f));
      }
      k++;
    }
    segmentStart = segmentEnd;
  }

  Texture texture = Texture::New(TextureType::TEXTURE_2D, pixelFormat, GRADIENT_TEXTURE_SIZE, 1u);
  if(DALI_UNLIKELY(Dali::Integration::IsGpuMemoryProfileEnabled()))
  {
    Dali::Integration::TextureUploadWithContent(texture, pixelData, Dali::Integration::ToDaliString(FormatStops(stops)), Dali::Integration::TextureContextTypeHint::GRADIENT_TEXTURE);
  }
  else
  {
    texture.Upload(pixelData);
  }

  return texture;
}

} // namespace Internal
} // namespace Toolkit
} // namespace DALI_NAMESPACE

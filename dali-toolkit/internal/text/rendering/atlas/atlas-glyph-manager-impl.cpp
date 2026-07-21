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
 */

// CLASS HEADER
#include <dali-toolkit/internal/text/rendering/atlas/atlas-glyph-manager-impl.h>

// EXTERNAL INCLUDES
#include <locale>

#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>

#include <locale>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::Concise, true, "LOG_TEXT_RENDERING");
#endif

} // unnamed namespace

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
AtlasGlyphManager::AtlasGlyphManager()
: mSlotDelegate(this)
{
  mAtlasManager = Dali::Toolkit::AtlasManager::New();
  mSampler      = Sampler::New();
  mSampler.SetFilterMode(FilterMode::LINEAR, FilterMode::LINEAR);
}

void AtlasGlyphManager::Add(const Text::GlyphInfo&                        glyph,
                            const Toolkit::AtlasGlyphManager::GlyphStyle& style,
                            const PixelData&                              bitmap,
                            Dali::Toolkit::AtlasManager::AtlasSlot&       slot)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "Added glyph, font: %d index: %d\n", glyph.fontId, glyph.index);

  // If glyph added to an existing or new atlas then a new glyph record is required.
  // Check if an existing atlas will fit the image, create a new one if required.
  if(mAtlasManager.Add(bitmap, slot))
  {
    // A new atlas was created so set the texture set details for the atlas
    Dali::Texture atlas      = mAtlasManager.GetAtlasContainer(slot.mAtlasId);
    TextureSet    textureSet = TextureSet::New();
    textureSet.SetTexture(0u, atlas);
    textureSet.SetSampler(0u, mSampler);
    mAtlasManager.SetTextures(slot.mAtlasId, textureSet);
  }

  GlyphRecordEntry record;
  record.mIndex        = glyph.index;
  record.mImageId      = slot.mImageId;
  record.mCount        = 1;
  record.mOutlineWidth = style.outline;
  record.isItalic      = style.isItalic;
  record.isBold        = style.isBold;

  // Have glyph records been created for this fontId in the current generation?
  bool foundGlyph = false;
  for(std::vector<FontGlyphRecord>::iterator fontGlyphRecordIt = mFontGlyphRecords.begin();
      fontGlyphRecordIt != mFontGlyphRecords.end();
      ++fontGlyphRecordIt)
  {
    if(fontGlyphRecordIt->mFontId == glyph.fontId &&
       fontGlyphRecordIt->mGeneration == mCacheGeneration)
    {
      fontGlyphRecordIt->mGlyphRecords.PushBack(record);
      foundGlyph = true;
      break;
    }
  }

  if(!foundGlyph)
  {
    // We need to add a new font entry
    FontGlyphRecord fontGlyphRecord;
    fontGlyphRecord.mFontId     = glyph.fontId;
    fontGlyphRecord.mGeneration = mCacheGeneration;
    fontGlyphRecord.mGlyphRecords.PushBack(record);
    mFontGlyphRecords.push_back(fontGlyphRecord);
  }
}

void AtlasGlyphManager::GenerateMeshData(uint32_t                       imageId,
                                         const Vector2&                 position,
                                         Toolkit::AtlasManager::Mesh2D& mesh)
{
  // Generate mesh data and tell Atlas Manager not to handle reference counting ( we'll do it )
  mAtlasManager.GenerateMeshData(imageId, position, mesh, false);
}

bool AtlasGlyphManager::IsCached(Text::FontId                                  fontId,
                                 Text::GlyphIndex                              index,
                                 const Toolkit::AtlasGlyphManager::GlyphStyle& style,
                                 Dali::Toolkit::AtlasManager::AtlasSlot&       slot)
{
  for(std::vector<FontGlyphRecord>::iterator fontGlyphRecordIt = mFontGlyphRecords.begin();
      fontGlyphRecordIt != mFontGlyphRecords.end();
      ++fontGlyphRecordIt)
  {
    if(fontGlyphRecordIt->mFontId == fontId &&
       fontGlyphRecordIt->mGeneration == mCacheGeneration)
    {
      for(Vector<GlyphRecordEntry>::Iterator glyphRecordIt = fontGlyphRecordIt->mGlyphRecords.Begin();
          glyphRecordIt != fontGlyphRecordIt->mGlyphRecords.End();
          ++glyphRecordIt)
      {
        if((glyphRecordIt->mIndex == index) &&
           (glyphRecordIt->mOutlineWidth == style.outline) &&
           (glyphRecordIt->isItalic == style.isItalic) &&
           (glyphRecordIt->isBold == style.isBold))
        {
          slot.mImageId = glyphRecordIt->mImageId;
          slot.mAtlasId = mAtlasManager.GetAtlas(slot.mImageId);
          return true;
        }
      }
    }
  }
  slot.mImageId = 0;
  return false;
}

Vector2 AtlasGlyphManager::GetAtlasSize(uint32_t atlasId)
{
  Toolkit::AtlasManager::AtlasSize size = mAtlasManager.GetAtlasSize(atlasId);
  return Vector2(static_cast<float>(size.mWidth), static_cast<float>(size.mHeight));
}

void AtlasGlyphManager::SetNewAtlasSize(uint32_t width, uint32_t height, uint32_t blockWidth, uint32_t blockHeight)
{
  Toolkit::AtlasManager::AtlasSize size;
  size.mWidth       = width;
  size.mHeight      = height;
  size.mBlockWidth  = blockWidth;
  size.mBlockHeight = blockHeight;
  mAtlasManager.SetNewAtlasSize(size);
}

Pixel::Format AtlasGlyphManager::GetPixelFormat(uint32_t atlasId)
{
  return mAtlasManager.GetPixelFormat(atlasId);
}

const Toolkit::AtlasGlyphManager::Metrics& AtlasGlyphManager::GetMetrics()
{
  std::ostringstream verboseMetrics;
  verboseMetrics.imbue(std::locale::classic());

  mMetrics.mGlyphCount = 0u;
  for(std::vector<FontGlyphRecord>::iterator fontGlyphRecordIt = mFontGlyphRecords.begin();
      fontGlyphRecordIt != mFontGlyphRecords.end();
      ++fontGlyphRecordIt)
  {
    mMetrics.mGlyphCount += fontGlyphRecordIt->mGlyphRecords.Size();

    verboseMetrics << "[FontId " << fontGlyphRecordIt->mFontId << " Glyph ";
    for(Vector<GlyphRecordEntry>::Iterator glyphRecordEntryIt = fontGlyphRecordIt->mGlyphRecords.Begin();
        glyphRecordEntryIt != fontGlyphRecordIt->mGlyphRecords.End();
        ++glyphRecordEntryIt)
    {
      verboseMetrics << glyphRecordEntryIt->mIndex << "(" << glyphRecordEntryIt->mCount << ") ";
    }
    verboseMetrics << "] ";
  }
  mMetrics.mVerboseGlyphCounts = verboseMetrics.str();

  mAtlasManager.GetMetrics(mMetrics.mAtlasMetrics);

  return mMetrics;
}

void AtlasGlyphManager::AdjustReferenceCount(Text::FontId fontId, uint32_t imageId, int32_t delta)
{
  if(0 == delta)
  {
    return;
  }

  DALI_LOG_INFO(gLogFilter,
                Debug::General,
                "AdjustReferenceCount fontId: %u, imageId: %u, delta: %d\n",
                fontId,
                imageId,
                delta);

  for(std::vector<FontGlyphRecord>::iterator fontGlyphRecordIt = mFontGlyphRecords.begin();
      fontGlyphRecordIt != mFontGlyphRecords.end();
      ++fontGlyphRecordIt)
  {
    // Do not filter by generation.
    // Renderers using a previous generation must still be able to release
    // the exact atlas image they reference.
    if(fontGlyphRecordIt->mFontId != fontId)
    {
      continue;
    }

    for(Vector<GlyphRecordEntry>::Iterator glyphRecordIt = fontGlyphRecordIt->mGlyphRecords.Begin();
        glyphRecordIt != fontGlyphRecordIt->mGlyphRecords.End();
        ++glyphRecordIt)
    {
      if(glyphRecordIt->mImageId == imageId)
      {
        glyphRecordIt->mCount += delta;

        DALI_ASSERT_DEBUG(glyphRecordIt->mCount >= 0 &&
                          "Glyph ref-count should not be negative");

        if(0 == glyphRecordIt->mCount)
        {
          mAtlasManager.Remove(glyphRecordIt->mImageId);
          fontGlyphRecordIt->mGlyphRecords.Remove(glyphRecordIt);

          // If this FontGlyphRecord has no more glyphs, remove the record
          if(fontGlyphRecordIt->mGlyphRecords.Empty())
          {
            mFontGlyphRecords.erase(fontGlyphRecordIt);
          }
        }
        return;
      }
    }
  }

  DALI_ASSERT_DEBUG(false && "Failed to adjust glyph ref-count");
}

TextureSet AtlasGlyphManager::GetTextures(uint32_t atlasId) const
{
  return mAtlasManager.GetTextures(atlasId);
}

void AtlasGlyphManager::InvalidateGlyphCache()
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "Invalidating atlas glyph cache generation\n");
  ++mCacheGeneration;
}

void AtlasGlyphManager::EnsureLocaleChangedConnection()
{
  if(mLocaleChangedConnected || !Dali::Adaptor::IsAvailable())
  {
    return;
  }

  Dali::Adaptor::Get().LocaleChangedSignal().Connect(mSlotDelegate, &AtlasGlyphManager::OnLocaleChanged);
  mLocaleChangedConnected = true;
}

void AtlasGlyphManager::OnLocaleChanged(std::string /* locale */)
{
  InvalidateGlyphCache();
}

AtlasGlyphManager::~AtlasGlyphManager()
{
  // mAtlasManager handle is automatically released here
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/debug.h>

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
{
  mAtlasManager = Dali::Toolkit::AtlasManager::New();
}

void AtlasGlyphManager::Add( const Text::GlyphInfo& glyph,
                             const PixelData& bitmap,
                             Dali::Toolkit::AtlasManager::AtlasSlot& slot )
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "Added glyph, font: %d index: %d\n", glyph.fontId, glyph.index );

  if ( mAtlasManager.Add( bitmap, slot ) )
  {
    // A new atlas was created so set the texture set details for the atlas
    Dali::Texture atlas = mAtlasManager.GetAtlasContainer( slot.mAtlasId );
    TextureSet textureSet = TextureSet::New();
    textureSet.SetTexture( 0u, atlas );
    mAtlasManager.SetTextures( slot.mAtlasId, textureSet );
  }

  GlyphRecordEntry record;
  record.mIndex = glyph.index;
  record.mImageId = slot.mImageId;
  record.mCount = 1;

  // Have glyph records been created for this fontId ?
  bool foundGlyph = false;
  for ( std::vector< FontGlyphRecord >::iterator fontGlyphRecordIt = mFontGlyphRecords.begin();
        fontGlyphRecordIt != mFontGlyphRecords.end(); ++fontGlyphRecordIt )
  {
    if ( fontGlyphRecordIt->mFontId == glyph.fontId )
    {
      fontGlyphRecordIt->mGlyphRecords.PushBack( record );
      foundGlyph = true;
      break;
    }
  }

  if ( !foundGlyph )
  {
    // We need to add a new font entry
    FontGlyphRecord fontGlyphRecord;
    fontGlyphRecord.mFontId = glyph.fontId;
    fontGlyphRecord.mGlyphRecords.PushBack( record );
    mFontGlyphRecords.push_back( fontGlyphRecord );
  }
}

void AtlasGlyphManager::GenerateMeshData( uint32_t imageId,
                                          const Vector2& position,
                                          Toolkit::AtlasManager::Mesh2D& mesh )
{
  // Generate mesh data and tell Atlas Manager not to handle reference counting ( we'll do it )
  mAtlasManager.GenerateMeshData( imageId, position, mesh, false );
}

bool AtlasGlyphManager::IsCached( Text::FontId fontId,
                                Text::GlyphIndex index,
                                Dali::Toolkit::AtlasManager::AtlasSlot& slot )
{
  for ( std::vector< FontGlyphRecord >::iterator fontGlyphRecordIt = mFontGlyphRecords.begin();
        fontGlyphRecordIt != mFontGlyphRecords.end();
        ++fontGlyphRecordIt )
  {
    if ( fontGlyphRecordIt->mFontId == fontId )
    {
      for ( Vector< GlyphRecordEntry >::Iterator glyphRecordIt = fontGlyphRecordIt->mGlyphRecords.Begin();
            glyphRecordIt != fontGlyphRecordIt->mGlyphRecords.End();
            ++glyphRecordIt )
      {
        if ( glyphRecordIt->mIndex == index )
        {
          slot.mImageId = glyphRecordIt->mImageId;
          slot.mAtlasId = mAtlasManager.GetAtlas( slot.mImageId );
          return true;
        }
      }
    }
  }
  slot.mImageId = 0;
  return false;
}

Vector2 AtlasGlyphManager::GetAtlasSize( uint32_t atlasId )
{
  Toolkit::AtlasManager::AtlasSize size = mAtlasManager.GetAtlasSize( atlasId );
  return Vector2( static_cast< float >( size.mWidth ), static_cast< float >( size.mHeight ) );
}

void AtlasGlyphManager::SetNewAtlasSize( uint32_t width, uint32_t height, uint32_t blockWidth, uint32_t blockHeight )
{
  Toolkit::AtlasManager::AtlasSize size;
  size.mWidth = width;
  size.mHeight = height;
  size.mBlockWidth = blockWidth;
  size.mBlockHeight = blockHeight;
  mAtlasManager.SetNewAtlasSize( size );
}

Pixel::Format AtlasGlyphManager::GetPixelFormat( uint32_t atlasId )
{
  return mAtlasManager.GetPixelFormat( atlasId );
}

const Toolkit::AtlasGlyphManager::Metrics& AtlasGlyphManager::GetMetrics()
{
  std::ostringstream verboseMetrics;

  mMetrics.mGlyphCount = 0u;
  for ( std::vector< FontGlyphRecord >::iterator fontGlyphRecordIt = mFontGlyphRecords.begin();
        fontGlyphRecordIt != mFontGlyphRecords.end();
        ++fontGlyphRecordIt )
  {
    mMetrics.mGlyphCount += fontGlyphRecordIt->mGlyphRecords.Size();

    verboseMetrics << "[FontId " << fontGlyphRecordIt->mFontId << " Glyph ";
    for ( Vector< GlyphRecordEntry >::Iterator glyphRecordEntryIt = fontGlyphRecordIt->mGlyphRecords.Begin();
          glyphRecordEntryIt != fontGlyphRecordIt->mGlyphRecords.End();
          ++glyphRecordEntryIt )
    {
      verboseMetrics << glyphRecordEntryIt->mIndex << "(" << glyphRecordEntryIt->mCount << ") ";
    }
    verboseMetrics << "] ";
  }
  mMetrics.mVerboseGlyphCounts = verboseMetrics.str();

  mAtlasManager.GetMetrics( mMetrics.mAtlasMetrics );

  return mMetrics;
}

void AtlasGlyphManager::AdjustReferenceCount( Text::FontId fontId, Text::GlyphIndex index, int32_t delta )
{
  if( 0 != delta )
  {
    DALI_LOG_INFO( gLogFilter, Debug::General, "AdjustReferenceCount %d, font: %d index: %d\n", delta, fontId, index );

    for ( std::vector< FontGlyphRecord >::iterator fontGlyphRecordIt = mFontGlyphRecords.begin();
          fontGlyphRecordIt != mFontGlyphRecords.end();
          ++fontGlyphRecordIt )
    {
      if ( fontGlyphRecordIt->mFontId == fontId )
      {
        for ( Vector< GlyphRecordEntry >::Iterator glyphRecordIt = fontGlyphRecordIt->mGlyphRecords.Begin();
              glyphRecordIt != fontGlyphRecordIt->mGlyphRecords.End();
              ++glyphRecordIt )
        {
          if ( glyphRecordIt->mIndex == index )
          {
            glyphRecordIt->mCount += delta;
            DALI_ASSERT_DEBUG( glyphRecordIt->mCount >= 0 && "Glyph ref-count should not be negative" );

            if ( !glyphRecordIt->mCount )
            {
              mAtlasManager.Remove( glyphRecordIt->mImageId );
              fontGlyphRecordIt->mGlyphRecords.Remove( glyphRecordIt );
            }
            return;
          }
        }
      }
    }

    // Should not arrive here
    DALI_ASSERT_DEBUG( false && "Failed to adjust ref-count" );
  }
}

TextureSet AtlasGlyphManager::GetTextures( uint32_t atlasId ) const
{
  return mAtlasManager.GetTextures( atlasId );
}

AtlasGlyphManager::~AtlasGlyphManager()
{
  // mAtlasManager handle is automatically released here
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

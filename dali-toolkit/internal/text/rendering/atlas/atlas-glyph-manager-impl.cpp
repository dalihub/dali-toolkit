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
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/common/stage.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

//#define DISPLAY_ATLAS

AtlasGlyphManager::AtlasGlyphManager()
: mCount( 0 )
{
  mAtlasManager = Dali::Toolkit::AtlasManager::New();
}

AtlasGlyphManager::~AtlasGlyphManager()
{
  // Clear up any remaining references
  for ( std::vector< FontGlyphRecord >::iterator fontGlyphRecordIt = mFontGlyphRecords.begin();
        fontGlyphRecordIt != mFontGlyphRecords.end();
        ++fontGlyphRecordIt )
  {
    for ( Vector< GlyphRecordEntry >::Iterator glyphRecordEntryIt = fontGlyphRecordIt->mGlyphRecords.Begin();
          glyphRecordEntryIt != fontGlyphRecordIt->mGlyphRecords.End();
          ++glyphRecordEntryIt )
    {
      mAtlasManager.Remove( glyphRecordEntryIt->mImageId );
    }
  }
}

AtlasGlyphManagerPtr AtlasGlyphManager::New()
{
  AtlasGlyphManagerPtr internal = new AtlasGlyphManager();
  return internal;
}

void AtlasGlyphManager::Add( Text::FontId fontId,
                             const Text::GlyphInfo& glyph,
                             const BufferImage& bitmap,
                             Dali::Toolkit::AtlasManager::AtlasSlot& slot )
{
  mAtlasManager.Add( bitmap, slot );

  GlyphRecordEntry record;
  record.mIndex = glyph.index;
  record.mImageId = slot.mImageId;
  record.mCount = 1;

  // Have glyph records been created for this fontId ?
  bool foundGlyph = false;
  for ( std::vector< FontGlyphRecord >::iterator fontGlyphRecordIt = mFontGlyphRecords.begin();
        fontGlyphRecordIt != mFontGlyphRecords.end(); ++fontGlyphRecordIt )
  {
    if ( fontGlyphRecordIt->mFontId == fontId )
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
    fontGlyphRecord.mFontId = fontId;
    fontGlyphRecord.mGlyphRecords.PushBack( record );
    mFontGlyphRecords.push_back( fontGlyphRecord );
  }

#ifdef DISPLAY_ATLAS
  {
    uint32_t atlasCount = mAtlasManager.GetAtlasCount();
    if ( atlasCount > mCount )
    {
      for ( uint32_t i = 0; i < atlasCount; ++i )
      {
        ImageActor actor = ImageActor::New( mAtlasManager.GetAtlasContainer( i + 1u ) );
        actor.SetParentOrigin( Vector3( 0.5f, 0.25f + ( static_cast< float >( i ) * 0.25f ), 0.5f ) );
        actor.SetAnchorPoint( AnchorPoint::CENTER );
        actor.SetSize( 256.0f, 256.0f );
        Stage::GetCurrent().Add( actor );
      }
    }
    mCount = atlasCount;
  }
#endif
}

void AtlasGlyphManager::GenerateMeshData( uint32_t imageId,
                                          const Vector2& position,
                                          MeshData& meshData )
{
  // Generate mesh data and tell Atlas Manager not to handle reference counting ( we'll do it )
  mAtlasManager.GenerateMeshData( imageId, position, meshData, false );
}

void AtlasGlyphManager::StitchMesh( MeshData& first,
                                    const MeshData& second )
{
  mAtlasManager.StitchMesh( first, second );
}

bool AtlasGlyphManager::Cached( Text::FontId fontId,
                                uint32_t index,
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
  mMetrics.mGlyphCount = mFontGlyphRecords.size();
  mAtlasManager.GetMetrics( mMetrics.mAtlasMetrics );
  return mMetrics;
}

void AtlasGlyphManager::AdjustReferenceCount( Text::FontId fontId, uint32_t imageId, int32_t delta )
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
        if ( glyphRecordIt->mImageId == imageId )
        {
          glyphRecordIt->mCount += delta;
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
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

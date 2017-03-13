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
#include <dali-toolkit/internal/text/rendering/atlas/atlas-manager-impl.h>

// EXTERNAL INCLUDES
#include <string.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/rendering/atlas/atlas-mesh-factory.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
  const uint32_t DEFAULT_ATLAS_WIDTH( 512u );
  const uint32_t DEFAULT_ATLAS_HEIGHT( 512u );
  const uint32_t DEFAULT_BLOCK_WIDTH( 16u );
  const uint32_t DEFAULT_BLOCK_HEIGHT( 16u );
  const uint32_t SINGLE_PIXEL_PADDING( 1u );
  const uint32_t DOUBLE_PIXEL_PADDING( SINGLE_PIXEL_PADDING << 1 );
  Toolkit::AtlasManager::AtlasSize EMPTY_SIZE;
}

AtlasManager::AtlasManager()
: mAddFailPolicy( Toolkit::AtlasManager::FAIL_ON_ADD_CREATES )
{
  mNewAtlasSize.mWidth = DEFAULT_ATLAS_WIDTH;
  mNewAtlasSize.mHeight = DEFAULT_ATLAS_HEIGHT;
  mNewAtlasSize.mBlockWidth = DEFAULT_BLOCK_WIDTH;
  mNewAtlasSize.mBlockHeight = DEFAULT_BLOCK_HEIGHT;
}

AtlasManagerPtr AtlasManager::New()
{
  AtlasManagerPtr internal = new AtlasManager();
  return internal;
}

AtlasManager::~AtlasManager()
{
}

Toolkit::AtlasManager::AtlasId AtlasManager::CreateAtlas( const Toolkit::AtlasManager::AtlasSize& size, Pixel::Format pixelformat )
{
  SizeType width = size.mWidth;
  SizeType height = size.mHeight;
  SizeType blockWidth = size.mBlockWidth;
  SizeType blockHeight = size.mBlockHeight;

  // Check to see if the atlas is large enough to hold a single block even ?
  if ( blockWidth + DOUBLE_PIXEL_PADDING + 1u > width || blockHeight + DOUBLE_PIXEL_PADDING + 1u > height )
  {
    DALI_LOG_ERROR("Atlas %i x %i too small. Dimensions need to be at least %ix%i\n",
                    width, height, blockWidth + DOUBLE_PIXEL_PADDING + 1u, blockHeight + DOUBLE_PIXEL_PADDING + 1u );
    return 0;
  }

  Dali::Texture atlas = Dali::Texture::New( TextureType::TEXTURE_2D, pixelformat, width, height );

  // Clear the background
  unsigned int bufferSize(  width * height * Dali::Pixel::GetBytesPerPixel( pixelformat ) );
  unsigned char* background = new unsigned char[bufferSize];
  memset( background, 0, bufferSize );
  PixelData backgroundPixels = PixelData::New( background, bufferSize, width, height, pixelformat, PixelData::DELETE_ARRAY );
  atlas.Upload( backgroundPixels, 0u, 0u, 0u, 0u, width, height );

  AtlasDescriptor atlasDescriptor;
  atlasDescriptor.mAtlas = atlas;
  atlasDescriptor.mSize = size;
  atlasDescriptor.mPixelFormat = pixelformat;
  atlasDescriptor.mTotalBlocks = ( ( width - 1u ) / blockWidth ) * ( ( height - 1u ) / blockHeight );
  atlasDescriptor.mAvailableBlocks = atlasDescriptor.mTotalBlocks;

  bufferSize = blockWidth * SINGLE_PIXEL_PADDING * Dali::Pixel::GetBytesPerPixel(pixelformat);
  unsigned char* bufferHorizontalStrip = new unsigned char[bufferSize];
  memset( bufferHorizontalStrip, 0, bufferSize );
  atlasDescriptor.mHorizontalStrip = PixelData::New( bufferHorizontalStrip, bufferSize, blockWidth, SINGLE_PIXEL_PADDING, pixelformat, PixelData::DELETE_ARRAY );

  bufferSize = SINGLE_PIXEL_PADDING * (blockHeight - DOUBLE_PIXEL_PADDING) * Dali::Pixel::GetBytesPerPixel(pixelformat);
  unsigned char* bufferVerticalStrip = new unsigned char[bufferSize];
  memset( bufferVerticalStrip, 0, bufferSize );
  atlasDescriptor.mVerticalStrip = PixelData::New( bufferVerticalStrip, bufferSize, SINGLE_PIXEL_PADDING, blockHeight - DOUBLE_PIXEL_PADDING, pixelformat, PixelData::DELETE_ARRAY );

  bufferSize = Dali::Pixel::GetBytesPerPixel(pixelformat);
  unsigned char* buffer = new unsigned char[bufferSize];
  memset( buffer, 0xFF, bufferSize );
  PixelData filledPixelImage = PixelData::New( buffer, bufferSize, 1u, 1u, pixelformat, PixelData::DELETE_ARRAY );
  atlas.Upload( filledPixelImage, 0u, 0u, 0u, 0u, 1u, 1u );
  mAtlasList.push_back( atlasDescriptor );
  return mAtlasList.size();
}

void AtlasManager::SetAddPolicy( Toolkit::AtlasManager::AddFailPolicy policy )
{
  mAddFailPolicy = policy;
}

bool AtlasManager::Add( const PixelData& image,
                        Toolkit::AtlasManager::AtlasSlot& slot,
                        Toolkit::AtlasManager::AtlasId atlas )
{
  bool created = false;
  Pixel::Format pixelFormat = image.GetPixelFormat();
  SizeType width = image.GetWidth();
  SizeType height = image.GetHeight();
  SizeType foundAtlas = 0;
  SizeType index = 0;
  slot.mImageId = 0;

  AtlasSlotDescriptor desc;

  // If there is a preferred atlas then check for room in that first
  if ( atlas-- )
  {
    foundAtlas = CheckAtlas( atlas, width, height, pixelFormat );
  }

  // Search current atlases to see if there is a good match
  while( !foundAtlas && index < mAtlasList.size() )
  {
    foundAtlas = CheckAtlas( index, width, height, pixelFormat );
    ++index;
  }

  // If we can't find a suitable atlas then check the policy to determine action
  if ( !foundAtlas-- )
  {
    if ( Toolkit::AtlasManager::FAIL_ON_ADD_CREATES == mAddFailPolicy )
    {
      foundAtlas = CreateAtlas( mNewAtlasSize, pixelFormat );
      if ( !foundAtlas-- )
      {
        DALI_LOG_ERROR("Failed to create an atlas of %i x %i blocksize: %i x %i.\n",
                       mNewAtlasSize.mWidth,
                       mNewAtlasSize.mHeight,
                       mNewAtlasSize.mBlockWidth,
                       mNewAtlasSize.mBlockHeight );
        return created;
      }
      created = true;
      foundAtlas = CheckAtlas( foundAtlas, width, height, pixelFormat );
    }

    if ( !foundAtlas-- || Toolkit::AtlasManager::FAIL_ON_ADD_FAILS == mAddFailPolicy )
    {
      // Haven't found an atlas for this image!!!!!!
      DALI_LOG_ERROR("Failed to create an atlas under current policy.\n");
      return created;
    }
  }

  // Work out which the block we're going to use
  // Is there currently a next free block available ?
  if ( mAtlasList[ foundAtlas ].mAvailableBlocks )
  {
    // Yes, so select our next block
    desc.mBlock = mAtlasList[ foundAtlas ].mTotalBlocks - mAtlasList[ foundAtlas ].mAvailableBlocks--;
  }
  else
  {
    // Our next block must be from the free list, fetch from the start of the list
    desc.mBlock = mAtlasList[ foundAtlas ].mFreeBlocksList[ 0 ];
    mAtlasList[ foundAtlas ].mFreeBlocksList.Remove( mAtlasList[ foundAtlas ].mFreeBlocksList.Begin() );
  }

  desc.mImageWidth = width;
  desc.mImageHeight = height;
  desc.mAtlasId = foundAtlas + 1u;
  desc.mCount = 1u;

  // See if there's a previously freed image ID that we can assign to this new image
  uint32_t imageId = 0u;
  for ( uint32_t i = 0u; i < mImageList.Size(); ++i )
  {
    if ( !mImageList[ i ].mCount )
    {
      imageId = i + 1u;
      break;
    }
  }
  if ( !imageId )
  {
    mImageList.PushBack( desc );
    slot.mImageId = mImageList.Size();
  }
  else
  {
    mImageList[ imageId - 1u ] = desc;
    slot.mImageId = imageId;
  }
  slot.mAtlasId = foundAtlas + 1u;

  // Upload the buffer image into the atlas
  UploadImage( image, desc );
  return created;
}

AtlasManager::SizeType AtlasManager::CheckAtlas( SizeType atlas,
                                                 SizeType width,
                                                 SizeType height,
                                                 Pixel::Format pixelFormat )
{
  AtlasManager::SizeType result = 0u;
  if ( pixelFormat == mAtlasList[ atlas ].mPixelFormat )
  {
    // Check to see if the image will fit in these blocks, if not we'll need to create a new atlas
    if ( ( mAtlasList[ atlas ].mAvailableBlocks + mAtlasList[ atlas ].mFreeBlocksList.Size() )
           && width + DOUBLE_PIXEL_PADDING <= mAtlasList[ atlas ].mSize.mBlockWidth
           && height + DOUBLE_PIXEL_PADDING <= mAtlasList[ atlas ].mSize.mBlockHeight )
    {
      result = atlas + 1u;
    }
  }
  return result;
}

void AtlasManager::UploadImage( const PixelData& image,
                                const AtlasSlotDescriptor& desc )
{
  // Get the atlas to upload the image to
  SizeType atlas = desc.mAtlasId - 1u;

  // Check to see that the pixel formats are compatible
  if ( image.GetPixelFormat() != mAtlasList[ atlas ].mPixelFormat )
  {
    DALI_LOG_ERROR("Cannot upload an image with a different PixelFormat to the Atlas.\n");
    return;
  }

  SizeType atlasBlockWidth = mAtlasList[ atlas ].mSize.mBlockWidth;
  SizeType atlasBlockHeight = mAtlasList[ atlas ].mSize.mBlockHeight;
  SizeType atlasWidthInBlocks = ( mAtlasList[ atlas ].mSize.mWidth - 1u ) / mAtlasList[ atlas ].mSize.mBlockWidth;

  SizeType blockX = desc.mBlock % atlasWidthInBlocks;
  SizeType blockY = desc.mBlock / atlasWidthInBlocks;
  SizeType blockOffsetX = ( blockX * atlasBlockWidth ) + 1u;
  SizeType blockOffsetY = ( blockY * atlasBlockHeight) + 1u;

  SizeType width = image.GetWidth();
  SizeType height = image.GetHeight();

  // Blit image 1 pixel to the right and down into the block to compensate for texture filtering
  if ( !mAtlasList[ atlas ].mAtlas.Upload( image, 0u, 0u,
                                           blockOffsetX + SINGLE_PIXEL_PADDING,
                                           blockOffsetY + SINGLE_PIXEL_PADDING,
                                           width, height) )
  {
    DALI_LOG_ERROR("Uploading image to Atlas Failed!.\n");
  }

  // Blit top strip
  if ( !mAtlasList[ atlas ].mAtlas.Upload( mAtlasList[ atlas ].mHorizontalStrip, 0u, 0u,
                                           blockOffsetX,
                                           blockOffsetY,
                                           mAtlasList[ atlas ].mHorizontalStrip.GetWidth(),
                                           mAtlasList[ atlas ].mHorizontalStrip.GetHeight()) )
  {
    DALI_LOG_ERROR("Uploading top strip to Atlas Failed!\n");
  }

  // Blit left strip
  if ( !mAtlasList[ atlas ].mAtlas.Upload( mAtlasList[ atlas ].mVerticalStrip, 0u, 0u,
                                           blockOffsetX,
                                           blockOffsetY + SINGLE_PIXEL_PADDING,
                                           mAtlasList[ atlas ].mVerticalStrip.GetWidth(),
                                           mAtlasList[ atlas ].mVerticalStrip.GetHeight() ) )
  {
    DALI_LOG_ERROR("Uploading left strip to Atlas Failed!\n");
  }

  // Blit bottom strip
  if ( blockOffsetY + height + DOUBLE_PIXEL_PADDING <= mAtlasList[ atlas ].mSize.mHeight )
  {
    if ( !mAtlasList[ atlas ].mAtlas.Upload( mAtlasList[ atlas ].mHorizontalStrip, 0u, 0u,
                                             blockOffsetX,
                                             blockOffsetY + height + SINGLE_PIXEL_PADDING,
                                             mAtlasList[ atlas ].mHorizontalStrip.GetWidth(),
                                             mAtlasList[ atlas ].mHorizontalStrip.GetHeight() ) )
    {
      DALI_LOG_ERROR("Uploading bottom strip to Atlas Failed!.\n");
    }
  }

  // Blit right strip
  if ( blockOffsetX + width + DOUBLE_PIXEL_PADDING <= mAtlasList[ atlas ].mSize.mWidth )
  {
    if ( !mAtlasList[ atlas ].mAtlas.Upload( mAtlasList[ atlas ].mVerticalStrip, 0u, 0u,
                                             blockOffsetX + width + SINGLE_PIXEL_PADDING,
                                             blockOffsetY + SINGLE_PIXEL_PADDING,
                                             mAtlasList[ atlas ].mVerticalStrip.GetWidth(),
                                             mAtlasList[ atlas ].mVerticalStrip.GetHeight() ) )
    {
      DALI_LOG_ERROR("Uploading right strip to Atlas Failed!.\n");
    }
  }
}

void AtlasManager::GenerateMeshData( ImageId id,
                                     const Vector2& position,
                                     Toolkit::AtlasManager::Mesh2D& meshData,
                                     bool addReference )
{
  if ( id )
  {
    // Read the atlas Id to use for this image
    SizeType imageId = id - 1u;
    SizeType atlas = mImageList[ imageId ].mAtlasId - 1u;
    SizeType width = mImageList[ imageId ].mImageWidth;
    SizeType height = mImageList[ imageId ].mImageHeight;

    AtlasMeshFactory::CreateQuad( width,
                                  height,
                                  mImageList[ imageId ].mBlock,
                                  mAtlasList[ atlas ].mSize,
                                  position,
                                  meshData );

    // Mesh created so increase the reference count, if we're asked to
    if ( addReference )
    {
      mImageList[ imageId ].mCount++;
    }
  }
  else
  {
    DALI_LOG_ERROR("Cannot generate mesh with invalid AtlasId\n");
  }
}

Dali::Texture AtlasManager::GetAtlasContainer( AtlasId atlas ) const
{
  DALI_ASSERT_DEBUG( atlas && atlas <= mAtlasList.size() );
  Dali::Texture atlasContainer;
  if ( atlas && atlas-- <= mAtlasList.size() )
  {
    atlasContainer = mAtlasList[ atlas ].mAtlas;
  }
  return atlasContainer;
}

bool AtlasManager::Remove( ImageId id )
{
  // Decrements the reference count of this image, and removes the blocks if zero.
  SizeType imageId = id - 1u;
  bool removed = false;

  if ( id > mImageList.Size() )
     {
    DALI_LOG_ERROR("Atlas was asked to free an invalid imageID: %i\n", id );
    return false;
  }

  // If we attempt to free an image that is already freed then do nothing, other than log
  if ( !mImageList[ imageId ].mCount )
  {
    DALI_LOG_ERROR("Atlas was asked to free an imageID: %i, that has already been freed!\n", id );
    return false;
  }

  if ( 2u > --mImageList[ imageId ].mCount )
  {
    // 'Remove the blocks' from this image and add to the atlas' freelist
    removed = true;
    mImageList[ imageId ].mCount = 0;
    SizeType atlas = mImageList[ imageId ].mAtlasId - 1u;
    mAtlasList[ atlas ].mFreeBlocksList.PushBack( mImageList[ imageId ].mBlock );
  }
  return removed;
}

AtlasManager::AtlasId AtlasManager::GetAtlas( ImageId id ) const
{
  DALI_ASSERT_DEBUG( id && id <= mImageList.Size() );
  AtlasManager::AtlasId atlasId = 0u;
  if ( id && id-- <= mImageList.Size() )
  {
    atlasId = mImageList[ id ].mAtlasId;
  }
  return atlasId;
}

void AtlasManager::SetNewAtlasSize( const Toolkit::AtlasManager::AtlasSize& size )
{
  mNewAtlasSize = size;

  // Add on padding for borders around atlas entries
  mNewAtlasSize.mBlockWidth += DOUBLE_PIXEL_PADDING;
  mNewAtlasSize.mBlockHeight += DOUBLE_PIXEL_PADDING;
}

const Toolkit::AtlasManager::AtlasSize& AtlasManager::GetAtlasSize( AtlasId atlas )
{
  DALI_ASSERT_DEBUG( atlas && atlas <= mAtlasList.size() );
  if ( atlas && atlas-- <= mAtlasList.size() )
  {
    return mAtlasList[ atlas ].mSize;
  }
  return EMPTY_SIZE;
}

AtlasManager::SizeType AtlasManager::GetFreeBlocks( AtlasId atlas ) const
{
  DALI_ASSERT_DEBUG( atlas && atlas <= mAtlasList.size() );
  AtlasManager::SizeType freeBlocks = 0u;
  if ( atlas && atlas-- <= mAtlasList.size() )
  {
    freeBlocks = mAtlasList[ atlas ].mAvailableBlocks + mAtlasList[ atlas ].mFreeBlocksList.Size();
  }
  return freeBlocks;
}

AtlasManager::SizeType AtlasManager::GetAtlasCount() const
{
  return mAtlasList.size();
}

Pixel::Format AtlasManager::GetPixelFormat( AtlasId atlas ) const
{
  DALI_ASSERT_DEBUG( atlas && atlas <= mAtlasList.size() );
  Pixel::Format pixelFormat = Pixel::RGBA8888;
  if ( atlas && atlas-- <= mAtlasList.size() )
  {
    pixelFormat = mAtlasList[ atlas ].mPixelFormat;
  }
  return pixelFormat;
}

void AtlasManager::GetMetrics( Toolkit::AtlasManager::Metrics& metrics )
{
  Toolkit::AtlasManager::AtlasMetricsEntry entry;
  uint32_t textureMemoryUsed = 0;
  uint32_t atlasCount = mAtlasList.size();
  metrics.mAtlasCount = atlasCount;
  metrics.mAtlasMetrics.Resize(0);

  for ( uint32_t i = 0; i < atlasCount; ++i )
  {
    entry.mSize = mAtlasList[ i ].mSize;
    entry.mTotalBlocks = mAtlasList[ i ].mTotalBlocks;
    entry.mBlocksUsed = entry.mTotalBlocks - mAtlasList[ i ].mAvailableBlocks + mAtlasList[ i ].mFreeBlocksList.Size();
    entry.mPixelFormat = GetPixelFormat( i + 1 );

    metrics.mAtlasMetrics.PushBack( entry );

    uint32_t size = entry.mSize.mWidth * entry.mSize.mHeight;
    if ( entry.mPixelFormat == Pixel::BGRA8888 )
    {
      size <<= 2;
    }

    textureMemoryUsed += size;

  }
  metrics.mTextureMemoryUsed = textureMemoryUsed;
}

TextureSet AtlasManager::GetTextures( AtlasId atlas ) const
{
  DALI_ASSERT_DEBUG( atlas && atlas <= mAtlasList.size() );
  TextureSet textureSet;
  if ( atlas && atlas-- <= mAtlasList.size() )
  {
    textureSet = mAtlasList[ atlas ].mTextureSet;
  }
  return textureSet;
}

void AtlasManager::SetTextures( AtlasId atlas, TextureSet& textureSet )
{
  DALI_ASSERT_DEBUG( atlas && atlas <= mAtlasList.size() );
  if ( atlas && atlas-- <= mAtlasList.size() )
  {
    mAtlasList[ atlas ].mTextureSet = textureSet;
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali



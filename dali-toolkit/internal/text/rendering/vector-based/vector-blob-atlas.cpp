/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/rendering/vector-based/vector-blob-atlas.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/images/texture-set-image.h>
#include <dali/integration-api/debug.h>

namespace
{

#if defined(DEBUG_ENABLED)
  Debug::Filter* gLogFilter = Debug::Filter::New(Debug::Concise, true, "LOG_TEXT_RENDERING");
#endif

}

namespace Dali
{

namespace Toolkit
{

namespace Text
{

static void
EncodeBlobCoordinate( unsigned int cornerX, unsigned int cornerY,
                      unsigned int atlasX, unsigned int atlasY,
                      unsigned int nominalWidth, unsigned int nominalHeight,
                      BlobCoordinate* v )
{
  DALI_ASSERT_DEBUG(0 == (atlasX & ~0x7F));
  DALI_ASSERT_DEBUG(0 == (atlasY & ~0x7F));
  DALI_ASSERT_DEBUG(0 == (cornerX & ~1));
  DALI_ASSERT_DEBUG(0 == (cornerY & ~1));
  DALI_ASSERT_DEBUG(0 == (nominalWidth & ~0x3F));
  DALI_ASSERT_DEBUG(0 == (nominalHeight & ~0x3F));

  unsigned int x = (((atlasX << 6) | nominalWidth) << 1)  | cornerX;
  unsigned int y = (((atlasY << 6) | nominalHeight) << 1) | cornerY;

  unsigned int encoded = (x << 16) | y;

  v->u = encoded >> 16;
  v->v = encoded & 0xFFFF;
}

VectorBlobAtlas::VectorBlobAtlas( unsigned int textureWidth,
                                  unsigned int textureHeight,
                                  unsigned int itemWidth,
                                  unsigned int itemHeightQuantum )
: mTextureWidth( textureWidth ),
  mTextureHeight( textureHeight ),
  mItemWidth( itemWidth ),
  mItemHeightQuantum( itemHeightQuantum ),
  mCursorX( 0 ),
  mCursorY( 0 ),
  mIsFull( false )
{
  DALI_LOG_INFO( gLogFilter, Debug::General, "Blob atlas %p size %dx%d, item width %d, height quantum %d\n", this, textureWidth, textureHeight, itemWidth, itemHeightQuantum );

  mAtlasTexture = BufferImage::New( textureWidth, textureHeight, Pixel::RGBA8888 );

  mTextureSet = TextureSet::New();
  TextureSetImage( mTextureSet, 0, mAtlasTexture );
}

bool VectorBlobAtlas::IsFull() const
{
  return mIsFull;
}

bool VectorBlobAtlas::FindGlyph( FontId fontId,
                                 GlyphIndex glyphIndex,
                                 BlobCoordinate* coords )
{
  const unsigned int size( mItemLookup.size() );

  for( unsigned int i=0; i<size; ++i )
  {
    if( mItemLookup[i].fontId     == fontId &&
        mItemLookup[i].glyphIndex == glyphIndex )
    {
      const Item& item = mItemCache[ mItemLookup[i].cacheIndex ];

      coords[0] = item.coords[0];
      coords[1] = item.coords[1];
      coords[2] = item.coords[2];
      coords[3] = item.coords[3];

      return true;
    }
  }

  return false;
}

bool VectorBlobAtlas::AddGlyph( unsigned int fontId,
                                unsigned int glyphIndex,
                                VectorBlob* blob,
                                unsigned int length,
                                unsigned int nominalWidth,
                                unsigned int nominalHeight,
                                BlobCoordinate* coords )
{
  if( mIsFull )
  {
    return false;
  }

  unsigned int w, h, x, y;

  w = mItemWidth;
  h = (length + w - 1) / w;

  if( mCursorY + h > mTextureHeight )
  {
    // Go to next column
    mCursorX += mItemWidth;
    mCursorY = 0;
  }

  if( mCursorX + w <= mTextureWidth && mCursorY + h <= mTextureHeight )
  {
    x = mCursorX;
    y = mCursorY;
    mCursorY += (h + mItemHeightQuantum - 1) & ~(mItemHeightQuantum - 1);
  }
  else
  {
    DALI_LOG_INFO( gLogFilter, Debug::General, "Blob atlas %p is now FULL\n", this );

    // The atlas is now considered to be full
    mIsFull = true;
    return false;
  }

  if (w * h == length)
  {
    TexSubImage( x, y, w, h, blob );
  }
  else
  {
    TexSubImage( x, y, w, h-1, blob );

    // Upload the last row separately
    TexSubImage( x, y + h - 1, length - (w * (h - 1)), 1 , blob + w * (h - 1));
  }

  DALI_LOG_INFO( gLogFilter, Debug::General, "Blob atlas %p capacity %d filled %d %f\%\n",
                 this,
                 mTextureWidth*mTextureHeight,
                 mCursorY*mItemWidth + mCursorX*mTextureHeight,
                 100.0f * (float)(mCursorY*mItemWidth + mCursorX*mTextureHeight) / (float)(mTextureWidth*mTextureHeight) );

  Key key;
  key.fontId = fontId;
  key.glyphIndex = glyphIndex;
  key.cacheIndex = mItemCache.size();
  mItemLookup.push_back( key );

  x /= mItemWidth;
  y /= mItemHeightQuantum;

  Item item;
  EncodeBlobCoordinate( 0, 0, x, y, nominalWidth, nominalHeight, &item.coords[0] ); // BOTTOM_LEFT
  EncodeBlobCoordinate( 0, 1, x, y, nominalWidth, nominalHeight, &item.coords[1] ); // TOP_LEFT
  EncodeBlobCoordinate( 1, 0, x, y, nominalWidth, nominalHeight, &item.coords[2] ); // BOTTOM_RIGHT
  EncodeBlobCoordinate( 1, 1, x, y, nominalWidth, nominalHeight, &item.coords[3] ); // TOP_RIGHT
  mItemCache.push_back( item );

  coords[0] = item.coords[0];
  coords[1] = item.coords[1];
  coords[2] = item.coords[2];
  coords[3] = item.coords[3];

  return true;
}

void VectorBlobAtlas::TexSubImage( unsigned int offsetX,
                                   unsigned int offsetY,
                                   unsigned int width,
                                   unsigned int height,
                                   VectorBlob* blob )
{
  PixelBuffer* pixbuf = mAtlasTexture.GetBuffer();
  size_t pos;
  size_t dataIndex = 0;
  for( size_t y= offsetY; y< height + offsetY; y++ )
  {
    pos = y * mTextureWidth * 4;
    for( size_t x = offsetX; x < width + offsetX; x++ )
    {
      pixbuf[pos+x*4] =  0xFF & blob[dataIndex].r;
      pixbuf[pos+x*4+1] = 0xFF & blob[dataIndex].g;
      pixbuf[pos+x*4+2] = 0xFF & blob[dataIndex].b;
      pixbuf[pos+x*4+3] = 0xFF & blob[dataIndex].a;
      dataIndex++;
    }
  }

  mAtlasTexture.Update();
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

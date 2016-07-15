#ifndef DALI_TOOLKIT_TEXT_VECTOR_BLOB_ATLAS_H
#define DALI_TOOLKIT_TEXT_VECTOR_BLOB_ATLAS_H

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
 */

// EXTERNAL INCLUDES
#include <dali/public-api/images/buffer-image.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/rendering/shader.h>
#include <dali/public-api/rendering/texture-set.h>
#include <dali/devel-api/text-abstraction/text-abstraction-definitions.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/text-definitions.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

typedef Dali::TextAbstraction::VectorBlob VectorBlob;

struct BlobCoordinate
{
  float u;
  float v;
};

/**
 * @brief An atlas for vector blob data
 *
 */
class VectorBlobAtlas : public RefObject
{
public:

  /**
   * @brief Create a blob atlas.
   *
   * @param[in] textureWidth The atlas width.
   * @param[in] textureHeight The atlas height.
   * @param[in] itemWidth The width of an item in the atlas.
   * @param[in] itemHeightQuantum The item height quantum.
   * When blobs are added to columns in the atlas, the Y position is advanced by a multiple of this value.
   */
  VectorBlobAtlas( unsigned int textureWidth,
                   unsigned int textureHeight,
                   unsigned int itemWidth,
                   unsigned int itemHeightQuantum );

  /**
   * @brief Query whether the atlas is full.
   *
   * @return True if the atlas is full.
   */
  bool IsFull() const;

  /**
   * @brief Find the UV coordinates for a glyph in the atlas.
   *
   * @param[in] fontId The ID of the font containing the glyph.
   * @param[in] glyphIndex The index of the glyph within the font.
   * @param[out] coords If the glyph was found, an array of 4 UV coordinates will be returned.
   * Otherwise coords will not be written into.
   * @return True if the glyph was found.
   */
  bool FindGlyph( FontId fontId,
                  GlyphIndex glyphIndex,
                  BlobCoordinate* coords );

  /**
   * @brief Add a glyph to the atlas.
   *
   * @param[in] fontId The ID of the font containing the glyph.
   * @param[in] glyphIndex The index of the glyph within the font.
   * @param[in] blobData A blob of vector data representing the glyph.
   * @param[in] length The length of the blob data.
   * @param[in] nominalWidth The width of the blob.
   * @param[in] nominalHeight The height of the blob.
   * @param[out] coords An array of 4 UV coordinates will be returned.
   * @return True if the glyph was added. Otherwise the atlas is now full.
   */
  bool AddGlyph( unsigned int fontId,
                 unsigned int glyphIndex,
                 VectorBlob* blob,
                 unsigned int length,
                 unsigned int nominalWidth,
                 unsigned int nominalHeight,
                 BlobCoordinate* coords );

  /**
   * @brief Get the info required by the GLyphy shader.
   *
   * @return The shader uniform value.
   */
  Vector4 GetInfo() const
  {
    return Vector4( mTextureWidth, mTextureHeight, mItemWidth, mItemHeightQuantum );
  }

  /**
   * @brief Retrieve the atlas texture.
   *
   * @return The texture used for rendering.
   */
  TextureSet GetTextureSet()
  {
    return mTextureSet;
  }

private:

  /**
   * @brief Helper for uploading data to the texture atlas.
   *
   * @param[in] offsetX The x position within the atlas.
   * @param[in] offsetY The y position within the atlas.
   * @param[in] width The width of the data to upload.
   * @param[in] height The height of the data to upload.
   * @param[in] blob The blob of data to upload.
   */
  void TexSubImage( unsigned int offsetX,
                    unsigned int offsetY,
                    unsigned int width,
                    unsigned int height,
                    VectorBlob* blob );

private:

  unsigned int mTextureWidth;
  unsigned int mTextureHeight;

  unsigned int mItemWidth;
  unsigned int mItemHeightQuantum;

  unsigned int mCursorX;
  unsigned int mCursorY;

  BufferImage mAtlasTexture;

  TextureSet mTextureSet;

  struct Key
  {
    unsigned int fontId;
    unsigned int glyphIndex;
    unsigned int cacheIndex;
  };

  struct Item
  {
    BlobCoordinate coords[4];
  };

  std::vector< Key >  mItemLookup;
  std::vector< Item > mItemCache;

  bool mIsFull;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_VECTOR_BLOB_ATLAS_H

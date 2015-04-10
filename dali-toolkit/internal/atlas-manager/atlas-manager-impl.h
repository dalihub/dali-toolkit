#ifndef __DALI_TOOLKIT_ATLAS_MANAGER_IMPL_H__
#define __DALI_TOOLKIT_ATLAS_MANAGER_IMPL_H__

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


// EXTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/atlas-manager/atlas-manager.h>

namespace Dali
{

namespace Toolkit
{

class AtlasManager;

} // namespace Toolkit

namespace Toolkit
{

namespace Internal
{

typedef Dali::Vector< Toolkit::AtlasManager::AtlasSlot > slotContainer;

class AtlasManager;
typedef IntrusivePtr<AtlasManager> AtlasManagerPtr;

class AtlasManager : public Dali::BaseObject
{
public:

  typedef uint32_t SizeType;
  typedef SizeType AtlasId;
  typedef SizeType ImageId;

  /**
   * @brief Internal storage of atlas attributes and image upload results
   */
  struct AtlasDescriptor
  {
    Dali::Atlas mAtlas;                                                 // atlas image
    SizeType mWidth;                                                    // width of atlas
    SizeType mHeight;                                                   // height of atlas
    SizeType mBlockWidth;                                               // width of a block in atlas
    SizeType mBlockHeight;                                              // height of a block in atlas
    Pixel::Format mPixelFormat;                                         // pixel format used by atlas
    BufferImage mHorizontalStrip;                                       // Image used to pad upload
    BufferImage mVerticalStrip;                                         // Image used to pad upload
    BufferImage mFilledPixelImage;                                      // Image used by atlas for operations such as underline
    PixelBuffer* mStripBuffer;                                          // Blank image buffer used to pad upload
    Material mMaterial;                                                 // material used for atlas texture
    SizeType mNextFreeBlock;                                            // next free block will be placed here ( actually +1 )
    Dali::Vector< SizeType > mFreeBlocksList;                           // unless there are any previously freed blocks
  };

  struct AtlasSlotDescriptor
  {
    SizeType mCount;                                                    // Reference count for this slot
    SizeType mImageWidth;                                               // Width of image stored
    SizeType mImageHeight;                                              // Height of image stored
    AtlasId mAtlasId;                                                   // Image is stored in this Atlas
    Dali::Vector< SizeType > mBlocksList;                               // List of blocks within atlas used for image
  };

  AtlasManager();

  /**
   * Create a new AtlasManager
   */
  static AtlasManagerPtr New();

  virtual ~AtlasManager();

  /**
   * @copydoc: Toolkit::AtlasManager::CreateAtlas
   */
  AtlasId CreateAtlas( SizeType width,
                       SizeType height,
                       SizeType blockWidth,
                       SizeType blockHeight,
                       Pixel::Format pixelformat );

  /**
   * @copydoc Toolkit::AtlasManager::SetAddPolicy
   */
  void SetAddPolicy( Toolkit::AtlasManager::AddFailPolicy policy );

  /**
   * @copydoc Toolkit::AtlasManager::Add
   */
  void Add( const BufferImage& image,
            Toolkit::AtlasManager::AtlasSlot& slot,
            Toolkit::AtlasManager::AtlasId atlas );

  /**
   * @copydoc Toolkit::AtlasManager::GenerateMeshData
   */
  void GenerateMeshData( ImageId id,
                         const Vector2& position,
                         MeshData& mesh );

  /**
   * @copydoc Toolkit::AtlasManager::StitchMesh
   */
  void StitchMesh( MeshData& first,
                   const MeshData& second,
                   bool optimize );

  /**
   * @copydoc Toolkit::AtlasManager::StitchMesh
   */
  void StitchMesh( const MeshData& first,
                   const MeshData& second,
                   MeshData& out, bool optimize );

  /**
   * @copydoc Toolkit::AtlasManager::Remove
   */
  bool Remove( ImageId id );

  /**
   * @copydoc Toolkit::AtlasManager::GetAtlasContainer
   */
  Dali::Atlas GetAtlasContainer( AtlasId atlas ) const;

  /**
   * @copydoc Toolkit::AtlasManager::GetAtlas
   */
  AtlasId GetAtlas( ImageId id ) const;

  /**
   * @copydoc Toolkit::AtlasManager::SetNewAtlasSize
   */
  void SetNewAtlasSize( const Vector2& size,
                        const Vector2& blockSize );

  /**
   * @copydoc Toolkit::AtlasManager::GetAtlasSize
   */
  Vector2 GetAtlasSize( AtlasId atlas );

  /**
   * @copydoc Toolkit::AtlasManager::GetBlockSize
   */
  Vector2 GetBlockSize( AtlasId atlas );

  /**
   * @copydoc Toolkit::AtlasManager::GetFreeBlocks
   */
  SizeType GetFreeBlocks( AtlasId atlas ) const;

  /*
   * @copydoc Toolkit::AtlasManager::GetAtlasCount
   */
  SizeType GetAtlasCount() const;

  /**
   * @copydoc Toolkit::AtlasManager::GetPixelFormat
   */
  Pixel::Format GetPixelFormat( AtlasId atlas );

  /**
   * @copydoc Toolkit::AtlasManager::GetMetrics
   */
  void GetMetrics( Toolkit::AtlasManager::Metrics& metrics );

private:

  std::vector< AtlasDescriptor > mAtlasList;        // List of atlases created
  std::vector< AtlasSlotDescriptor > mImageList;  // List of bitmaps store in atlases

  SizeType CheckAtlas( SizeType atlas,
                       SizeType width,
                       SizeType height,
                       Pixel::Format pixelFormat,
                       SizeType& blockArea,
                       SizeType& totalBlocks );

  void CreateMesh( SizeType atlas,
                   SizeType imageWidth,
                   SizeType imageHeight,
                   const Vector2& position,
                   SizeType widthInBlocks,
                   SizeType heightInBlocks,
                   Dali::MeshData& meshData,
                   AtlasSlotDescriptor& desc );

  void OptimizeVertices( const MeshData::VertexContainer& in,
                         MeshData::FaceIndices& faces,
                         MeshData::VertexContainer& out );

  void UploadImage( const BufferImage& image,
                    const AtlasSlotDescriptor& desc );

  void PrintMeshData( const MeshData& meshData );

  Vector2 mNewAtlasSize;
  Vector2 mNewBlockSize;
  Toolkit::AtlasManager::AddFailPolicy mAddFailPolicy;
  uint32_t mFilledPixel;
};

} // namespace Internal

inline const Internal::AtlasManager& GetImplementation(const Toolkit::AtlasManager& manager)
{
  DALI_ASSERT_ALWAYS( manager && "AtlasManager handle is empty" );

  const BaseObject& handle = manager.GetBaseObject();

  return static_cast<const Internal::AtlasManager&>(handle);
}

inline Internal::AtlasManager& GetImplementation(Toolkit::AtlasManager& manager)
{
  DALI_ASSERT_ALWAYS( manager && "AtlasManager handle is empty" );

  BaseObject& handle = manager.GetBaseObject();

  return static_cast<Internal::AtlasManager&>(handle);
}

} // namespace Toolkit

} // namespace Dali


 #endif // __DALI_TOOLKIT_ATLAS_MANAGER_IMPL_H__
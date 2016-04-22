#ifndef __DALI_TOOLKIT_ATLAS_MANAGER_H__
#define __DALI_TOOLKIT_ATLAS_MANAGER_H__

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
#include <stdint.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/images/buffer-image.h>
#include <dali/devel-api/images/atlas.h>
#include <dali/devel-api/rendering/texture-set.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{

class AtlasManager;

} // namespace Internal

class AtlasManager : public BaseHandle
{
public:

  typedef uint32_t SizeType;
  typedef SizeType AtlasId;
  typedef SizeType ImageId;

  struct AtlasSize
  {
    SizeType mWidth;              ///< width of the atlas in pixels
    SizeType mHeight;             ///< height of the atlas in pixels
    SizeType mBlockWidth;         ///< width of a block in pixels
    SizeType mBlockHeight;        ///< height of a block in pixels
  };

  /**
   * Metrics structures to describe Atlas Manager state
   *
   */
  struct AtlasMetricsEntry
  {
    AtlasSize mSize;                 ///< size of atlas and blocks
    SizeType mBlocksUsed;            ///< number of blocks used in the atlas
    SizeType mTotalBlocks;           ///< total blocks used by atlas
    Pixel::Format mPixelFormat;      ///< pixel format of the atlas
  };

  struct Metrics
  {
    Metrics()
    : mAtlasCount( 0u ),
      mTextureMemoryUsed( 0u )
    {}

    ~Metrics()
    {}

    SizeType mAtlasCount;                               ///< number of atlases
    SizeType mTextureMemoryUsed;                        ///< texture memory used by atlases
    Dali::Vector< AtlasMetricsEntry > mAtlasMetrics;    ///< container of atlas information
  };

  struct Vertex2D
  {
    Vector2 mPosition;        ///< Vertex posiiton
    Vector2 mTexCoords;       ///< Vertex texture co-ordinates
    Vector4 mColor;           ///< Vertex color
  };

  struct Mesh2D
  {
    Vector< Vertex2D > mVertices;       ///< container of vertices
    Vector< SizeType > mIndices;        ///< container of indices
  };

  /**
   * Create an AtlasManager handle; this can be initialised with AtlasManager::New()
   * Calling member functions with an uninitialised handle is not allowed.
   */
  AtlasManager();

  /**
   * @brief Get new instance of AtlasManager object.
   *
   * @return A handle to the AtlasManager control.
   */
  static AtlasManager New();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~AtlasManager();

  /**
   * Policy on failing to add an image
   */
  enum AddFailPolicy
  {
    FAIL_ON_ADD_FAILS,
    FAIL_ON_ADD_CREATES
  };

  /**
   * @brief Container to hold result of placing texture into atlas
   */
  struct AtlasSlot
  {
    ImageId mImageId;                           ///< Id of stored Image
    AtlasId mAtlasId;                           ///< Id of Atlas containing this slot
  };

  typedef Dali::Vector< AtlasManager::AtlasSlot > slotContainer;

  /**
   * @brief Create a blank atlas of specific dimensions and pixel format with a certain block size
   *
   * @param[in] size desired atlas dimensions
   * @param[in] pixelformat format of a pixel in atlas
   *
   * @return atlas Id
   */
  AtlasId CreateAtlas( const AtlasSize& size, Pixel::Format pixelformat = Pixel::RGBA8888 );

  /**
   * @brief Set the policy on failure to add an image to an atlas
   *
   * @param policy policy to carry out if add fails
   */
  void SetAddPolicy( AddFailPolicy policy );

  /**
   * @brief Attempts to add an image to the most suitable atlas
   *
   * @details Add Policy may dictate that a new atlas is created if it can't presently be placed.
   *          If an add is made before an atlas is created under this policy,
   *          then a default size atlas will be created
   *
   * @param[in] image reference to a bitmapimage
   * @param[out] slot result of add operation
   * @param[in] atlas optional preferred atlas
   *
   * @return true if a new atlas was created
   */
  bool Add( const BufferImage& image,
            AtlasSlot& slot,
            AtlasId atlas = 0 );

  /**
   * @brief Remove previously added bitmapimage from atlas
   *
   * @param[in] id ImageId returned in the AtlasSlot from the add operation
   *
   * @return if true then image has been removed from the atlas
   */
  bool Remove( ImageId id );

  /**
   * @brief Generate mesh data for a previously added image
   *
   * @param[in] id Image Id returned in the AtlasSlot from the add operation
   * @param[in] position position of the resulting mesh in model space
   * @param[out] mesh Mesh Data Object to populate with mesh data
   * @param[in] addReference Whether to increase the internal reference count for image or not
   */
  void GenerateMeshData( ImageId id,
                         const Vector2& position,
                         Mesh2D& mesh,
                         bool addReference = true );

  /**
   * @brief Get the BufferImage containing an atlas
   *
   * @param[in] atlas AtlasId returned when atlas was created
   *
   * @return Atlas Handle
   */
  Dali::Atlas GetAtlasContainer( AtlasId atlas ) const;

  /**
   * @brief Get the Id of the atlas containing an image
   *
   * @param[in] id ImageId
   *
   * @return Atlas Id
   */
  AtlasId GetAtlas( ImageId id );
  /**
   * @brief Get the current size of an atlas
   *
   * @param[in] atlas AtlasId
   *
   * @return AtlasSize structure for the atlas
   */
  const AtlasSize& GetAtlasSize( AtlasId atlas );

  /**
   * @brief Get the number of blocks available in an atlas
   *
   * @param[in] atlas AtlasId
   *
   * @return Number of blocks free in this atlas
   */
  SizeType GetFreeBlocks( AtlasId atlas );

  /**
   * @brief Sets the pixel area of any new atlas and also the individual block size
   *
   * @param[in] size Atlas size structure
   *
   * @param blockSize pixel area in atlas for a block
   */
  void SetNewAtlasSize( const AtlasSize& size );

  /**
   * @brief Get the number of atlases created
   *
   * @return number of atlases
   */
  SizeType GetAtlasCount() const;

  /**
   * @brief Get the pixel format used by an atlas
   *
   * @param[in] atlas AtlasId
   *
   * @return Pixel format used by this atlas
   */
  Pixel::Format GetPixelFormat( AtlasId atlas ) const;

  /**
   * @brief Fill in a metrics structure showing current status of this Atlas Manager
   *
   * @param[in] metrics metrics structure to be filled
   */
  void GetMetrics( Metrics& metrics );

  /**
   * @brief Get TextureSet used by atlas
   *
   * @param[in] atlas AtlasId
   *
   * @return TextureSet used by atlas
   */
  TextureSet GetTextures( AtlasId atlas ) const;

  /**
   * @brief Set the texture set used by an atlas
   *
   * @param[in] atlas AtlasId
   * @param[in] textureSet The texture set to assign
   */
  void SetTextures( AtlasId atlas, TextureSet& textureSet );

private:

  explicit DALI_INTERNAL AtlasManager(Internal::AtlasManager *impl);

};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_ATLAS_MANAGER_H__

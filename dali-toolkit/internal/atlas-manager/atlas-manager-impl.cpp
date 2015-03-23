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
#include <dali-toolkit/internal/atlas-manager/atlas-manager-impl.h>

// EXTERNAL INCLUDE
#include <iostream>
#include <string.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{
  const Vector2 DEFAULT_ATLAS_SIZE( 512.0f, 512.0f );
  const Vector2 DEFAULT_BLOCK_SIZE( 32.0f, 32.0f );
  const uint32_t SINGLE_PIXEL_PADDING( 1u );
  const uint32_t DOUBLE_PIXEL_PADDING( SINGLE_PIXEL_PADDING << 1 );
}

AtlasManager::AtlasManager()
: mNewAtlasSize( DEFAULT_ATLAS_SIZE ),
  mNewBlockSize( DEFAULT_BLOCK_SIZE ),
  mAddFailPolicy( Toolkit::AtlasManager::FAIL_ON_ADD_CREATES )
{
}

AtlasManagerPtr AtlasManager::New()
{
  AtlasManagerPtr internal = new AtlasManager();
  return internal;
}

AtlasManager::~AtlasManager()
{
  for ( uint32_t i = 0; i < mAtlasList.size(); ++i )
  {
    delete[] mAtlasList[ i ].mStripBuffer;
  }
}

Toolkit::AtlasManager::AtlasId AtlasManager::CreateAtlas( SizeType width,
                                                          SizeType height,
                                                          SizeType blockWidth,
                                                          SizeType blockHeight,
                                                          Pixel::Format pixelformat )
{
  // Check to see if the atlas is large enough to hold a single block even ?
  if ( blockWidth > width || blockHeight > height )
  {
    DALI_LOG_ERROR("Atlas %i x %i too small. Dimensions need to be at least %ix%i\n",
                    width, height, blockWidth, blockHeight );
    return 0;
  }

  Dali::Atlas atlas = Dali::Atlas::New( width, height, pixelformat );
  AtlasDescriptor atlasDescriptor;
  atlasDescriptor.mAtlas = atlas;
  atlasDescriptor.mWidth = width;
  atlasDescriptor.mHeight = height;
  atlasDescriptor.mBlockWidth = blockWidth;
  atlasDescriptor.mBlockHeight = blockHeight;
  atlasDescriptor.mPixelFormat = pixelformat;
  std::stringstream materialLabel;
  materialLabel << "Atlas Material - ";
  materialLabel << mAtlasList.size();
  atlasDescriptor.mMaterial = Material::New( materialLabel.str() );
  atlasDescriptor.mMaterial.SetDiffuseTexture( atlas );
  atlasDescriptor.mNextFreeBlock = 1u; // indicate next free block will be the first ( +1 )

  // What size do we need for this atlas' strip buffer ( assume 32bit pixel format )?
  uint32_t neededStripSize =( blockWidth > blockHeight - DOUBLE_PIXEL_PADDING ? blockWidth : blockHeight - DOUBLE_PIXEL_PADDING ) << 2;
  atlasDescriptor.mStripBuffer = new PixelBuffer[ neededStripSize ];
  memset( atlasDescriptor.mStripBuffer, 0, neededStripSize );

  atlasDescriptor.mHorizontalStrip = BufferImage::New( atlasDescriptor.mStripBuffer,
                                                       blockWidth,
                                                       SINGLE_PIXEL_PADDING,
                                                       pixelformat );

  atlasDescriptor.mVerticalStrip = BufferImage::New( atlasDescriptor.mStripBuffer,
                                                     SINGLE_PIXEL_PADDING,
                                                     blockHeight - DOUBLE_PIXEL_PADDING,
                                                     pixelformat );

  mAtlasList.push_back( atlasDescriptor );
  return mAtlasList.size();
}

void AtlasManager::SetAddPolicy( Toolkit::AtlasManager::AddFailPolicy policy )
{
  mAddFailPolicy = policy;
}

void AtlasManager::Add( const BufferImage& image,
                        Toolkit::AtlasManager::AtlasSlot& slot,
                        Toolkit::AtlasManager::AtlasId atlas )
{
  // See if there's a slot in an atlas that matches the requirements of this image
  // A bitmap must be sliceable into a single atlas
  Pixel::Format pixelFormat = image.GetPixelFormat();
  SizeType width = image.GetWidth();
  SizeType height = image.GetHeight();
  SizeType blockArea = 0;
  SizeType totalBlocks = 0;
  SizeType foundAtlas = 0;
  SizeType index = 0;
  slot.mImageId = 0;

  AtlasSlotDescriptor desc;

  // If there is a preferred atlas then check for room in that first
  if ( atlas-- )
  {
    foundAtlas = CheckAtlas( atlas, width, height, pixelFormat, blockArea, totalBlocks );
  }

  // Search current atlases to see if there is a good match

  while( !foundAtlas && index < mAtlasList.size() )
  {
    foundAtlas = CheckAtlas( index, width, height, pixelFormat, blockArea, totalBlocks );
    ++index;
  }

  // If we can't find a suitable atlas then check the policy to determine action
  if ( !foundAtlas-- )
  {
    if ( Toolkit::AtlasManager::FAIL_ON_ADD_CREATES == mAddFailPolicy )
    {
      SizeType newAtlas = CreateAtlas( mNewAtlasSize.x, mNewAtlasSize.y, mNewBlockSize.x, mNewBlockSize.y, pixelFormat );
      if ( !newAtlas-- )
      {
        return;
      }
      else
      {
        foundAtlas = CheckAtlas( newAtlas, width, height, pixelFormat, blockArea, totalBlocks );
      }
    }

    if ( Toolkit::AtlasManager::FAIL_ON_ADD_FAILS == mAddFailPolicy || !foundAtlas-- )
    {
      // Haven't found an atlas for this image!!!!!!
          return;
    }
  }

  // Work out where the blocks are we're going to use
  for ( SizeType i = 0; i < blockArea; ++i )
  {
    // Is there currently a next free block available ?
    if ( mAtlasList[ foundAtlas ].mNextFreeBlock )
    {
      // Yes, so use this for our next block
      SizeType selectedBlock = mAtlasList[ foundAtlas ].mNextFreeBlock - 1u;
      desc.mBlocksList.PushBack( selectedBlock );

      // Any blocks going to be available after this one (adjust to store +1 )?
      selectedBlock++;
      selectedBlock++;
      if ( selectedBlock > totalBlocks )
      {
        // No so start trying to use free blocks list
        selectedBlock = 0;
      }
      mAtlasList[ foundAtlas ].mNextFreeBlock = selectedBlock;
    }
    else
    {
      // Our next block must be from the free list, fetch from the start of the list
      desc.mBlocksList.PushBack( mAtlasList[ foundAtlas ].mFreeBlocksList[ 0 ] );
      mAtlasList[ foundAtlas ].mFreeBlocksList.Remove( mAtlasList[ foundAtlas ].mFreeBlocksList.Begin() );
    }
  }

  desc.mImageWidth = width;
  desc.mImageHeight = height;
  desc.mAtlasId = foundAtlas + 1u;
  desc.mCount = 1u;

  // See if there's a previously freed image ID that we can assign to this new image
  uint32_t imageId = 0;
  for ( uint32_t i = 0; i < mImageList.size(); ++i )
  {
    if ( !mImageList[ i ].mCount )
    {
      imageId = i + 1u;
      break;
    }
  }
  if ( !imageId )
  {
    mImageList.push_back( desc );
    slot.mImageId = mImageList.size();
  }
  else
  {
    mImageList[ imageId - 1u ] = desc;
    slot.mImageId = imageId;
  }
  slot.mAtlasId = foundAtlas + 1u;
  UploadImage( image, desc );
}

AtlasManager::SizeType AtlasManager::CheckAtlas( SizeType atlas,
                                                 SizeType width,
                                                 SizeType height,
                                                 Pixel::Format pixelFormat,
                                                 SizeType& blockArea,
                                                 SizeType& totalBlocks )
{
  if ( pixelFormat == mAtlasList[ atlas ].mPixelFormat )
  {
    // Check to see if there are any unused blocks in this atlas to accomodate our image
    SizeType blocksInX = mAtlasList[ atlas ].mWidth / mAtlasList[ atlas ].mBlockWidth;
    SizeType blocksInY = mAtlasList[ atlas ].mHeight / mAtlasList[ atlas ].mBlockHeight;
    totalBlocks = blocksInX * blocksInY;
    SizeType blocksFree = mAtlasList[ atlas ].mNextFreeBlock ? totalBlocks - mAtlasList[ atlas ].mNextFreeBlock + 1u : 0;

    // Check to see if the image will fit in these blocks, if not we'll need to create a new atlas
    if ( blocksFree
         && width + DOUBLE_PIXEL_PADDING <= mAtlasList[ atlas ].mBlockWidth
         && height + DOUBLE_PIXEL_PADDING <= mAtlasList[ atlas ].mBlockHeight )
    {
      blockArea = 1u;
      return ( atlas + 1u );
    }
  }
  return 0;
}

void AtlasManager::CreateMesh( SizeType atlas,
                               SizeType imageWidth,
                               SizeType imageHeight,
                               const Vector2& position,
                               SizeType widthInBlocks,
                               SizeType heightInBlocks,
                               Dali::MeshData& meshData,
                               AtlasSlotDescriptor& desc )
{
  Dali::MeshData::Vertex vertex;
  Dali::MeshData::VertexContainer vertices;
  Dali::MeshData::FaceIndices faces;
  Dali::MeshData::FaceIndex faceIndex = 0;
  meshData.SetHasNormals( false );
  meshData.SetHasColor( true );
  meshData.SetHasTextureCoords( true );

  SizeType blockWidth = mAtlasList[ atlas ].mBlockWidth;
  SizeType blockHeight = mAtlasList[ atlas ].mBlockHeight;

  float vertexBlockWidth = static_cast< float >( blockWidth );
  float vertexBlockHeight = static_cast< float >( blockHeight );

  SizeType width = mAtlasList[ atlas ].mWidth;
  SizeType height = mAtlasList[ atlas ].mHeight;

  SizeType atlasWidthInBlocks = width / blockWidth;

  // Get the normalized size of a texel in both directions
  // TODO when texture resizing and passing texture size via uniforms is available,
  //      we will encode pixel positions into the vertex data rather than normalized
  //      meaning that geometry needn't be changed on an atlas resize
  float texelX = 1.0f / static_cast< float >( width );
  float texelY = 1.0f / static_cast< float >( height );

  // Get the normalized size of a block in texels
  float texelBlockWidth = texelX * vertexBlockWidth;
  float texelBlockHeight = texelY * vertexBlockHeight;

  // Get partial block space
  float vertexEdgeWidth = static_cast< float >( imageWidth % blockWidth );
  float vertexEdgeHeight = static_cast< float >( imageHeight % blockHeight );

  // And in texels
  float texelEdgeWidth = vertexEdgeWidth * texelX;
  float texelEdgeHeight = vertexEdgeHeight * texelY;

   // Block by block create the two triangles for the quad
  SizeType blockIndex = 0;
  float ndcWidth;
  float ndcHeight;
  float ndcVWidth;
  float ndcVHeight;

  Vector2 topLeft = position;

  for ( SizeType y = 0; y < heightInBlocks; ++y )
  {

    float currentX = position.x;

    if ( ( heightInBlocks - 1u ) == y && vertexEdgeHeight > 0.0f )
    {
      ndcHeight = texelEdgeHeight;
      ndcVHeight = vertexEdgeHeight;
    }
    else
    {
      ndcHeight = texelBlockHeight;
      ndcVHeight = vertexBlockHeight;
    }

    for ( SizeType x = 0; x < widthInBlocks; ++x )
    {
      SizeType block = desc.mBlocksList[ blockIndex++ ];

      float fBlockX = texelBlockWidth * static_cast< float >( block % atlasWidthInBlocks );
      float fBlockY = texelBlockHeight * static_cast< float >( block / atlasWidthInBlocks );

      // Add on texture filtering compensation
      fBlockX += texelX;
      fBlockY += texelY;

      if (  ( widthInBlocks - 1u ) == x && vertexEdgeWidth > 0.0f )
      {
        ndcWidth = texelEdgeWidth;
        ndcVWidth = vertexEdgeWidth;
      }
      else
      {
        ndcWidth = texelBlockWidth;
        ndcVWidth = vertexBlockWidth;
      }

      // Top left
      vertex.x = topLeft.x;
      vertex.y = topLeft.y;
      vertex.z = 0.0f;
      vertex.u = fBlockX;
      vertex.v = fBlockY;

      vertices.push_back( vertex );

      // Top Right
      vertex.x = topLeft.x + ndcVWidth;
      vertex.y = topLeft.y;
      vertex.z = 0.0f;
      vertex.u = fBlockX + ndcWidth;
      vertex.v = fBlockY;

      vertices.push_back( vertex );

      // Bottom Left
      vertex.x = topLeft.x;
      vertex.y = topLeft.y + ndcVHeight;
      vertex.z = 0.0f;
      vertex.u = fBlockX;
      vertex.v = fBlockY + ndcHeight;

      vertices.push_back( vertex );

      // Bottom Right
      topLeft.x += ndcVWidth;
      vertex.x = topLeft.x;
      vertex.y = topLeft.y + ndcVHeight;
      vertex.z = 0.0f;
      vertex.u = fBlockX + ndcWidth;
      vertex.v = fBlockY + ndcHeight;

      vertices.push_back( vertex );

      // Six indices in counter clockwise winding
      faces.push_back( faceIndex + 1u );
      faces.push_back( faceIndex );
      faces.push_back( faceIndex + 2u );
      faces.push_back( faceIndex + 2u );
      faces.push_back( faceIndex + 3u );
      faces.push_back( faceIndex + 1u );
      faceIndex += 4;
    }

    // Move down a row
    topLeft.x = currentX;
    topLeft.y += vertexBlockHeight;
  }

  // If there's only one block then skip this next vertex optimisation
  if ( widthInBlocks * heightInBlocks > 1 )
  {
    Dali::MeshData::VertexContainer optimizedVertices;
    OptimizeVertices( vertices, faces, optimizedVertices );
    meshData.SetVertices( optimizedVertices );
  }
  else
  {
    meshData.SetVertices( vertices );
  }

  meshData.SetFaceIndices( faces );
  meshData.SetMaterial( mAtlasList[ atlas ].mMaterial );
  //PrintMeshData( meshData );
}

void AtlasManager::PrintMeshData( const MeshData& meshData )
{
  std::cout << "\nMesh Data for Image: VertexCount = " << meshData.GetVertexCount();
  std::cout << ", Triangles = " << meshData.GetFaceCount() << std::endl;

  Dali::MeshData::VertexContainer vertices = meshData.GetVertices();
  Dali::MeshData::FaceIndices faces = meshData.GetFaces();

  for ( SizeType v = 0; v < vertices.size(); ++v )
  {
    std::cout << " Vertex(" << v << ") x = " << vertices[v].x << ", ";
    std::cout << "y = " << vertices[v].y << ", " << "z = " << vertices[v].z << ", ";
    std::cout << "u = " << vertices[v].u << ", " << "v = " << vertices[v].v << std::endl;
  }

  std::cout << "\n Indices: ";
  for ( SizeType i = 0; i < faces.size(); ++i )
  {
    std::cout << " " << faces[ i ];
  }
  std::cout << std::endl;
}

void AtlasManager::OptimizeVertices( const MeshData::VertexContainer& in,
                                     MeshData::FaceIndices& faces,
                                     MeshData::VertexContainer& out )
{
  unsigned short vertexIndex = 0;

  // We could check to see if blocks are next to each other, but it's probably just as quick to compare verts
  for ( SizeType i = 0; i < faces.size(); ++i )
  {
    // Fetch a vertex, has it already been assigned?
    bool foundVertex = false;
    Dali::MeshData::Vertex v = in[ faces [ i ] ];
    for ( SizeType j = 0; j < vertexIndex; ++j )
    {
      if ( v.x == out[ j ].x && v.y == out[ j ].y && v.z == out[ j ].z &&
           v.u == out[ j ].u && v.v == out[ j ].v && v.nX == out[ j ].nX &&
           v.nY == out[ j ].nY && v.nZ == out[ j ].nZ )
      {
        // Yes, so store this down as the vertex to use
        faces[ i ] = j;
        foundVertex = true;
        break;
      }
    }

    // Did we find a vertex ?
    if ( !foundVertex )
    {
      // Add a new vertex
      faces[ i ] = vertexIndex++;
      out.push_back( v );
    }
  }
}

void AtlasManager::StitchMesh( MeshData& first,
                               const MeshData& second,
                               bool optimize )
{

  // Would be much quicker to be able to get a non-const reference to these containers and update in situ
  MeshData::VertexContainer v1 = first.GetVertices();
  MeshData::VertexContainer v2 = second.GetVertices();
  MeshData::FaceIndices f1 = first.GetFaces();
  MeshData::FaceIndices f2 = second.GetFaces();

  uint32_t vc1 = first.GetVertexCount();
  uint32_t vc2 = second.GetVertexCount();

  for ( uint32_t v = 0; v < vc2; ++v )
  {
    v1.push_back( v2[ v ] );
  }

  for ( uint32_t f = 0; f < f2.size(); ++f )
  {
    f1.push_back( f2[ f ] + vc1 );
  }

  if ( optimize )
  {
    MeshData::VertexContainer optimizedVertices;
    OptimizeVertices( v1, f1, optimizedVertices );
    first.SetVertices( optimizedVertices );
  }
  else
  {
    first.SetVertices( v1 );
  }

  first.SetFaceIndices( f1 );

  // TODO rather than set the material to the second, check to see if there's a match and return if not
  first.SetMaterial( second.GetMaterial() );
}

void AtlasManager::StitchMesh( const MeshData& first,
                               const MeshData& second,
                               MeshData& out,
                               bool optimize )
{
  MeshData::VertexContainer v1 = first.GetVertices();
  MeshData::VertexContainer v2 = second.GetVertices();
  MeshData::FaceIndices f1 = first.GetFaces();
  MeshData::FaceIndices f2 = second.GetFaces();

  uint32_t vc1 = first.GetVertexCount();
  uint32_t vc2 = second.GetVertexCount();

  MeshData::VertexContainer vertices;

  MeshData::FaceIndices faces;

  MeshData::Vertex vertex;

  for ( uint32_t v = 0; v < vc1; ++v )
  {
    vertices.push_back( v1[ v ] );
  }

  for ( uint32_t v = 0; v < vc2; ++v )
  {
    vertices.push_back( v2[ v  ] );
  }

  for ( uint32_t f = 0; f < f1.size(); ++f )
  {
    faces.push_back( f1[ f ] );
  }

  for ( uint32_t f = 0; f < f2.size(); ++f )
  {
    faces.push_back( f2[ f ] + vc1 );
  }

  if ( optimize )
  {
    MeshData::VertexContainer optimizedVertices;
    OptimizeVertices( vertices, faces, optimizedVertices );
    out.SetVertices( optimizedVertices );
  }
  else
  {
    out.SetVertices( vertices );
  }

  // TODO rather than set the material to the second, check to see if there's a match and return if not
  out.SetMaterial( second.GetMaterial() );
  out.SetFaceIndices( faces );
}

void AtlasManager::UploadImage( const BufferImage& image,
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

  SizeType atlasBlockWidth = mAtlasList[ atlas ].mBlockWidth;
  SizeType atlasBlockHeight = mAtlasList[ atlas ].mBlockHeight;
  SizeType atlasWidthInBlocks = mAtlasList[ atlas ].mWidth / mAtlasList[ atlas ].mBlockWidth;

  SizeType block = desc.mBlocksList[ 0 ];
  SizeType blockX = block % atlasWidthInBlocks;
  SizeType blockY = block / atlasWidthInBlocks;
  SizeType blockOffsetX = blockX * atlasBlockWidth;
  SizeType blockOffsetY = blockY * atlasBlockHeight;

  SizeType width = image.GetWidth();
  SizeType height = image.GetHeight();

  // Blit image 1 pixel to the right and down into the block to compensate for texture filtering
  if ( !mAtlasList[ atlas ].mAtlas.Upload( image,
                                           blockOffsetX + SINGLE_PIXEL_PADDING,
                                           blockOffsetY + SINGLE_PIXEL_PADDING ) )
  {
    DALI_LOG_ERROR("Uploading image to Atlas Failed!.\n");
  }

  // Blit top strip
  if ( !mAtlasList[ atlas ].mAtlas.Upload( mAtlasList[ atlas ].mHorizontalStrip,
                                           blockOffsetX,
                                           blockOffsetY ) )
  {
    DALI_LOG_ERROR("Uploading top strip to Atlas Failed!\n");
  }

  // Blit left strip
  if ( !mAtlasList[ atlas ].mAtlas.Upload( mAtlasList[ atlas ].mVerticalStrip,
                                           blockOffsetX,
                                           blockOffsetY + SINGLE_PIXEL_PADDING ) )
  {
    DALI_LOG_ERROR("Uploading left strip to Atlas Failed!\n");
  }

  // Blit bottom strip
  if ( blockOffsetY + height + DOUBLE_PIXEL_PADDING <= mAtlasList[ atlas ].mHeight )
  {
    if ( !mAtlasList[ atlas ].mAtlas.Upload( mAtlasList[ atlas ].mHorizontalStrip,
                                             blockOffsetX,
                                             blockOffsetY + height + SINGLE_PIXEL_PADDING ) )
    {
      DALI_LOG_ERROR("Uploading bottom strip to Atlas Failed!.\n");
    }
  }

  // Blit right strip
  if ( blockOffsetX + width + DOUBLE_PIXEL_PADDING <= mAtlasList[ atlas ].mWidth )
  {
    if ( !mAtlasList[ atlas ].mAtlas.Upload( mAtlasList[ atlas ].mVerticalStrip,
                                             blockOffsetX + width + SINGLE_PIXEL_PADDING,
                                             blockOffsetY + SINGLE_PIXEL_PADDING ) )
    {
      DALI_LOG_ERROR("Uploading right strip to Atlas Failed!.\n");
    }
  }
}

void AtlasManager::GenerateMeshData( ImageId id,
                                     const Vector2& position,
                                     MeshData& meshData )
{
  // Read the atlas Id to use for this image
  SizeType imageId = id - 1u;
  SizeType atlas = mImageList[ imageId ].mAtlasId - 1u;
  SizeType width = mImageList[ imageId ].mImageWidth;
  SizeType height = mImageList[ imageId ].mImageHeight;

  SizeType widthInBlocks = width / mAtlasList[ atlas ].mBlockWidth;
  if ( width % mAtlasList[ atlas ].mBlockWidth )
  {
    widthInBlocks++;
  }
  SizeType heightInBlocks = height / mAtlasList[ atlas ].mBlockHeight;
  if ( height % mAtlasList[ atlas ].mBlockHeight )
  {
    heightInBlocks++;
  }

  CreateMesh( atlas, width, height, position, widthInBlocks, heightInBlocks, meshData, mImageList[ imageId ] );

  // Mesh created so increase the reference count
  mImageList[ imageId ].mCount++;
}

Dali::Atlas AtlasManager::GetAtlasContainer( AtlasId atlas ) const
{
  Dali::Atlas null;
  if ( !atlas || atlas > mAtlasList.size( ) )
  {

    DALI_LOG_ERROR("Cannot get Atlas from AtlasID ( doesn't exist ).\n");
    return null;
  }
  return mAtlasList[ atlas -1u ].mAtlas;
}

bool AtlasManager::Remove( ImageId id )
{
  // Decrements the reference count of this image, and removes the blocks if zero.
  SizeType imageId = id - 1u;
  bool removed = false;

  if ( id > mImageList.size() )
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

  if ( 1u == --mImageList[ imageId ].mCount )
  {
    // 'Remove the blocks' from this image and add to the atlas' freelist
    removed = true;
    mImageList[ imageId ].mCount = 0;
    SizeType atlas = mImageList[ imageId ].mAtlasId - 1u;
    for ( uint32_t i = 0; i < mImageList[ imageId ].mBlocksList.Size(); ++i )
    {
      mAtlasList[ atlas ].mFreeBlocksList.PushBack( mImageList[ imageId ].mBlocksList[ i ] );
    }
  }
  return removed;
}

AtlasManager::AtlasId AtlasManager::GetAtlas( ImageId id ) const
{
  if ( id && id <= mImageList.size() )
  {
    return mImageList[ id - 1u ].mAtlasId;
  }
  else
  {
    return 0;
  }
}

void AtlasManager::SetNewAtlasSize( const Vector2& size,
                                    const Vector2& blockSize )
{
  mNewAtlasSize = size;
  mNewBlockSize = blockSize;
}

Vector2 AtlasManager::GetBlockSize( AtlasId atlas )
{
  if ( atlas && atlas <= mAtlasList.size() )
  {
  return Vector2( static_cast< float >( mAtlasList[ atlas - 1u ].mBlockWidth ),
                  static_cast< float >( mAtlasList[ atlas - 1u ].mBlockHeight) );
  }
  else
  {
    return Vector2( 0.0f, 0.0f );
  }
}

Vector2 AtlasManager::GetAtlasSize( AtlasId atlas )
{
  if ( atlas && atlas <= mAtlasList.size() )
  {
    return Vector2( static_cast< float >( mAtlasList[ atlas - 1u ].mWidth ),
                    static_cast< float >( mAtlasList[ atlas - 1u ].mHeight ) );
  }
  else
  {
    return Vector2( 0.0f, 0.0f );
  }
}

AtlasManager::SizeType AtlasManager::GetFreeBlocks( AtlasId atlas ) const
{
  if ( atlas && atlas <= mAtlasList.size() )
  {
    uint32_t index = atlas - 1u;
    uint32_t width = mAtlasList[ index ].mWidth;
    uint32_t height = mAtlasList[ index ].mHeight;
    uint32_t blockWidth = mAtlasList[ index ].mBlockWidth;
    uint32_t blockHeight = mAtlasList[ index ].mBlockHeight;

    SizeType widthInBlocks = width / blockWidth;
    SizeType heightInBlocks = height / blockHeight;
    uint32_t blockCount = widthInBlocks * heightInBlocks;

    // Check free previously unallocated blocks and any free blocks
    blockCount -= mAtlasList[ index ].mNextFreeBlock - mAtlasList[ index ].mFreeBlocksList.Size();
    return blockCount;
  }
  else
  {
    return 0;
  }
}

AtlasManager::SizeType AtlasManager::GetAtlasCount() const
{
  return mAtlasList.size();
}

Pixel::Format AtlasManager::GetPixelFormat( AtlasId atlas )
{
  if ( !atlas || atlas > mAtlasList.size( ) )
  {

    DALI_LOG_ERROR("Cannot get Atlas from AtlasID ( doesn't exist ).\n");
    return Pixel::L8;
  }
  return mAtlasList[ atlas -1u ].mPixelFormat;
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
    SizeType width = mAtlasList[ i ].mWidth;
    SizeType height = mAtlasList[ i ].mHeight;
    SizeType blockWidth = mAtlasList[ i ].mBlockWidth;
    SizeType blockHeight = mAtlasList[ i ].mBlockHeight;

    entry.mWidth = width;
    entry.mHeight = height;
    entry.mBlockWidth = blockWidth;
    entry.mBlockHeight = blockHeight;
    entry.mTotalBlocks = ( width / blockWidth ) * ( height / blockHeight );
    entry.mBlocksUsed = mAtlasList[ i ].mNextFreeBlock ? mAtlasList[ i ].mNextFreeBlock : entry.mTotalBlocks - mAtlasList[ i ].mFreeBlocksList.Size();
    entry.mPixelFormat = GetPixelFormat( i + 1 );

      metrics.mAtlasMetrics.PushBack( entry );

    uint32_t size = width * height;
    if ( entry.mPixelFormat == Pixel::BGRA8888 )
    {
      size <<= 2;
    }

    textureMemoryUsed += size;

  }
  metrics.mTextureMemoryUsed = textureMemoryUsed;
}


} // namespace Internal

} // namespace Toolkit

} // namespace Dali



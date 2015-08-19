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
#include <dali/devel-api/rendering/sampler.h>
#include <dali/devel-api/rendering/shader.h>
#include <dali/integration-api/debug.h>

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
  const uint32_t FILLED_PIXEL( -1 );
  Toolkit::AtlasManager::AtlasSize EMPTY_SIZE;

  #define MAKE_SHADER(A)#A

  const char* VERTEX_SHADER = MAKE_SHADER(
  attribute mediump vec2    aPosition;
  attribute mediump vec2    aTexCoord;
  uniform   mediump mat4    uMvpMatrix;
  uniform   mediump vec3    uSize;
  varying   mediump vec2    vTexCoord;

  void main()
  {
    mediump vec4 position = vec4( aPosition, 0.0, 1.0 );
    position.xyz *= uSize;
    gl_Position = uMvpMatrix * position;
    vTexCoord = aTexCoord;
  }
  );

  const char* FRAGMENT_SHADER_L8 = MAKE_SHADER(
  uniform lowp    vec4      uColor;
  uniform         sampler2D sTexture;
  varying mediump vec2      vTexCoord;

  void main()
  {
    mediump vec4 color = texture2D( sTexture, vTexCoord );
    gl_FragColor = vec4( uColor.rgb, uColor.a * color.r );
  }
  );

  const char* FRAGMENT_SHADER_RGBA = MAKE_SHADER(
  uniform         sampler2D sTexture;
  varying mediump vec2      vTexCoord;

  void main()
  {
    gl_FragColor = texture2D( sTexture, vTexCoord );
  }
  );

}

AtlasManager::AtlasManager()
: mAddFailPolicy( Toolkit::AtlasManager::FAIL_ON_ADD_CREATES ),
  mFilledPixel( FILLED_PIXEL )
{
  mNewAtlasSize.mWidth = DEFAULT_ATLAS_WIDTH;
  mNewAtlasSize.mHeight = DEFAULT_ATLAS_HEIGHT;
  mNewAtlasSize.mBlockWidth = DEFAULT_BLOCK_WIDTH;
  mNewAtlasSize.mBlockHeight = DEFAULT_BLOCK_HEIGHT;
  mShaderL8 = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER_L8 );
  mShaderRgba = Shader::New( VERTEX_SHADER, FRAGMENT_SHADER_RGBA );
}

AtlasManagerPtr AtlasManager::New()
{
  AtlasManagerPtr internal = new AtlasManager();
  return internal;
}

AtlasManager::~AtlasManager()
{
  for ( SizeType i = 0; i < mAtlasList.size(); ++i )
  {
    mAtlasList[ i ].mAtlas.UploadedSignal().Disconnect( this, &AtlasManager::OnUpload );
    delete[] mAtlasList[ i ].mStripBuffer;
  }

  // Are there any upload signals pending? Free up those buffer images now.
  for ( SizeType i = 0; i < mUploadedImages.Size(); ++i )
  {
    delete[] mUploadedImages[ i ];
  }
}

void AtlasManager::OnUpload( Image image )
{
  if ( mUploadedImages.Size() )
  {
    delete[] mUploadedImages[ 0 ];
    mUploadedImages.Erase( mUploadedImages.Begin() );
  }
  else
  {
    DALI_LOG_ERROR("Atlas Image Upload List should not be empty\n");
  }
}

Toolkit::AtlasManager::AtlasId AtlasManager::CreateAtlas( const Toolkit::AtlasManager::AtlasSize& size, Pixel::Format pixelformat )
{
  SizeType width = size.mWidth;
  SizeType height = size.mHeight;
  SizeType blockWidth = size.mBlockWidth;
  SizeType blockHeight = size.mBlockHeight;

  // Check to see if the atlas is large enough to hold a single block even ?
  if ( blockWidth > width || blockHeight > height )
  {
    DALI_LOG_ERROR("Atlas %i x %i too small. Dimensions need to be at least %ix%i\n",
                    width, height, blockWidth, blockHeight );
    return 0;
  }

  Dali::Atlas atlas = Dali::Atlas::New( width, height, pixelformat );
  atlas.Clear( Vector4::ZERO );
  mUploadedImages.PushBack( NULL );
  AtlasDescriptor atlasDescriptor;
  atlasDescriptor.mAtlas = atlas;
  atlasDescriptor.mSize = size;
  atlasDescriptor.mPixelFormat = pixelformat;
  atlasDescriptor.mTotalBlocks = ( width / blockWidth ) * ( height / blockHeight );
  atlasDescriptor.mAvailableBlocks = atlasDescriptor.mTotalBlocks - 1u;
  atlas.UploadedSignal().Connect( this, &AtlasManager::OnUpload );

  // What size do we need for this atlas' strip buffer ( assume 32bit pixel format )?
  SizeType neededStripSize =( blockWidth > blockHeight - DOUBLE_PIXEL_PADDING ? blockWidth : blockHeight - DOUBLE_PIXEL_PADDING ) << 2;
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
  mUploadedImages.PushBack( NULL );
  atlasDescriptor.mFilledPixelImage = BufferImage::New( reinterpret_cast< PixelBuffer* >( &mFilledPixel ), 1, 1, pixelformat );
  atlas.Upload( atlasDescriptor.mFilledPixelImage, 0, 0 );

  Sampler sampler = Sampler::New( atlas, "sTexture" );
  sampler.SetProperty( Sampler::Property::AFFECTS_TRANSPARENCY, true );
  atlasDescriptor.mMaterial = Material::New( pixelformat == Pixel::L8 ? mShaderL8 : mShaderRgba );
  atlasDescriptor.mMaterial.AddSampler( sampler );
  atlasDescriptor.mSampler = sampler;
  atlasDescriptor.mMaterial.SetBlendMode( BlendingMode::ON );
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
  SizeType foundAtlas = 0;
  SizeType index = 0;
  slot.mImageId = 0;

  AtlasSlotDescriptor desc;

  // If there is a preferred atlas then check for room in that first
  if ( atlas-- )
  {
    foundAtlas = CheckAtlas( atlas, width, height, pixelFormat, blockArea );
  }

  // Search current atlases to see if there is a good match
  while( !foundAtlas && index < mAtlasList.size() )
  {
    foundAtlas = CheckAtlas( index, width, height, pixelFormat, blockArea );
    ++index;
  }

  // If we can't find a suitable atlas then check the policy to determine action
  if ( !foundAtlas-- )
  {
    if ( Toolkit::AtlasManager::FAIL_ON_ADD_CREATES == mAddFailPolicy )
    {
      SizeType newAtlas = CreateAtlas( mNewAtlasSize, pixelFormat );
      if ( !newAtlas-- )
      {
        return;
      }
      else
      {
        foundAtlas = CheckAtlas( newAtlas, width, height, pixelFormat, blockArea );
      }
    }

    if ( !foundAtlas-- || Toolkit::AtlasManager::FAIL_ON_ADD_FAILS == mAddFailPolicy )
    {
      // Haven't found an atlas for this image!!!!!!
      return;
    }
  }

  // Work out where the blocks are we're going to use
  for ( SizeType i = 0; i < blockArea; ++i )
  {
    // Is there currently a next free block available ?
    if ( mAtlasList[ foundAtlas ].mAvailableBlocks )
    {
      // Yes, so select our next block
      desc.mBlocksList.PushBack( mAtlasList[ foundAtlas ].mTotalBlocks - mAtlasList[ foundAtlas ].mAvailableBlocks-- );
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
  uint32_t imageId = 0u;
  for ( uint32_t i = 0u; i < mImageList.size(); ++i )
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
                                                 SizeType& blockArea )
{
  if ( pixelFormat == mAtlasList[ atlas ].mPixelFormat )
  {
    // Check to see if the image will fit in these blocks, if not we'll need to create a new atlas
    if ( ( mAtlasList[ atlas ].mAvailableBlocks + mAtlasList[ atlas ].mFreeBlocksList.Size() )
           && width + DOUBLE_PIXEL_PADDING <= mAtlasList[ atlas ].mSize.mBlockWidth
           && height + DOUBLE_PIXEL_PADDING <= mAtlasList[ atlas ].mSize.mBlockHeight )
    {
      blockArea = 1u;
      return ( atlas + 1u );
    }
  }
  return 0u;
}

void AtlasManager::CreateMesh( SizeType atlas,
                               SizeType imageWidth,
                               SizeType imageHeight,
                               const Vector2& position,
                               SizeType widthInBlocks,
                               SizeType heightInBlocks,
                               Toolkit::AtlasManager::Mesh2D& mesh,
                               AtlasSlotDescriptor& desc )
{
  Toolkit::AtlasManager::Vertex2D vertex;
  uint32_t faceIndex = 0;       // TODO change to unsigned short when property type is available

  SizeType blockWidth = mAtlasList[ atlas ].mSize.mBlockWidth;
  SizeType blockHeight = mAtlasList[ atlas ].mSize.mBlockHeight;

  float vertexBlockWidth = static_cast< float >( blockWidth );
  float vertexBlockHeight = static_cast< float >( blockHeight );

  SizeType width = mAtlasList[ atlas ].mSize.mWidth;
  SizeType height = mAtlasList[ atlas ].mSize.mHeight;

  SizeType atlasWidthInBlocks = width / blockWidth;

  // Get the normalized size of a texel in both directions
  // TODO when texture resizing and passing texture size via uniforms is available,
  //      we will encode pixel positions into the vertex data rather than normalized
  //      meaning that geometry needn't be changed on an atlas resize
  float texelX = 1.0f / static_cast< float >( width );
  float texelY = 1.0f / static_cast< float >( height );

  float halfTexelX = texelX * 0.5f;
  float halfTexelY = texelY * 0.5f;

  // Get the normalized size of a block in texels
  float texelBlockWidth = texelX * vertexBlockWidth;
  float texelBlockHeight = texelY * vertexBlockHeight;

  // Get partial block space
  float vertexEdgeWidth = static_cast< float >( imageWidth % blockWidth );
  float vertexEdgeHeight = static_cast< float >( imageHeight % blockHeight );

  // And in texels
  float texelEdgeWidth = texelX * vertexEdgeWidth;
  float texelEdgeHeight = texelY * vertexEdgeHeight;

  // We're going to 'blit' half a pixel more on each edge
  vertexBlockWidth++;
  vertexEdgeWidth++;
  vertexBlockHeight++;
  vertexEdgeHeight++;

   // Block by block create the two triangles for the quad
  SizeType blockIndex = 0;
  float ndcWidth;
  float ndcHeight;
  float ndcVWidth;
  float ndcVHeight;

  // Move back half a pixel
  Vector2 topLeft = Vector2( position.x - 0.5f, position.y - 0.5f );

  for ( SizeType y = 0; y < heightInBlocks; ++y )
  {

    float currentX = position.x;

    if ( ( heightInBlocks - 1u ) == y && vertexEdgeHeight > 0.0f )
    {
      ndcHeight = texelEdgeHeight + texelY;
      ndcVHeight = vertexEdgeHeight;
    }
    else
    {
      ndcHeight = texelBlockHeight + texelY;
      ndcVHeight = vertexBlockHeight;
    }

    for ( SizeType x = 0; x < widthInBlocks; ++x )
    {
      SizeType block = desc.mBlocksList[ blockIndex++ ];

      float fBlockX = texelBlockWidth * static_cast< float >( block % atlasWidthInBlocks );
      float fBlockY = texelBlockHeight * static_cast< float >( block / atlasWidthInBlocks );

      // Add on texture filtering compensation
      fBlockX += halfTexelX;
      fBlockY += halfTexelY;

      if (  ( widthInBlocks - 1u ) == x && vertexEdgeWidth > 0.0f )
      {
        ndcWidth = texelEdgeWidth + texelX;
        ndcVWidth = vertexEdgeWidth;
      }
      else
      {
        ndcWidth = texelBlockWidth + texelX;
        ndcVWidth = vertexBlockWidth;
      }

      // Top left
      vertex.mPosition.x = topLeft.x;
      vertex.mPosition.y = topLeft.y;
      vertex.mTexCoords.x = fBlockX;
      vertex.mTexCoords.y = fBlockY;

      mesh.mVertices.PushBack( vertex );

      // Top Right
      vertex.mPosition.x = topLeft.x + ndcVWidth;
      vertex.mPosition.y = topLeft.y;
      vertex.mTexCoords.x = fBlockX + ndcWidth;
      vertex.mTexCoords.y = fBlockY;

      mesh.mVertices.PushBack( vertex );

      // Bottom Left
      vertex.mPosition.x = topLeft.x;
      vertex.mPosition.y = topLeft.y + ndcVHeight;
      vertex.mTexCoords.x = fBlockX;
      vertex.mTexCoords.y = fBlockY + ndcHeight;

      mesh.mVertices.PushBack( vertex );

      // Bottom Right
      topLeft.x += ndcVWidth;
      vertex.mPosition.x = topLeft.x;
      vertex.mPosition.y = topLeft.y + ndcVHeight;
      vertex.mTexCoords.x = fBlockX + ndcWidth;
      vertex.mTexCoords.y = fBlockY + ndcHeight;

      mesh.mVertices.PushBack( vertex );

      // Six indices in counter clockwise winding
      mesh.mIndices.PushBack( faceIndex + 1u );
      mesh.mIndices.PushBack( faceIndex );
      mesh.mIndices.PushBack( faceIndex + 2u );
      mesh.mIndices.PushBack( faceIndex + 2u );
      mesh.mIndices.PushBack( faceIndex + 3u );
      mesh.mIndices.PushBack( faceIndex + 1u );
      faceIndex += 4;
    }

    // Move down a row
    topLeft.x = currentX;
    topLeft.y += vertexBlockHeight;
  }

  // If there's only one block then skip this next vertex optimisation
  if ( widthInBlocks * heightInBlocks > 1 )
  {
    Toolkit::AtlasManager::Mesh2D optimizedMesh;
    OptimizeMesh( mesh, optimizedMesh );
  }
}

void AtlasManager::PrintMeshData( const Toolkit::AtlasManager::Mesh2D& mesh )
{
  uint32_t vertexCount = mesh.mVertices.Size();
  uint32_t indexCount = mesh.mIndices.Size();
  std::cout << "\nMesh Data for Image: VertexCount = " << vertexCount;
  std::cout << ", Triangles = " << indexCount / 3 << std::endl;

  for ( SizeType v = 0; v < vertexCount; ++v )
  {
    std::cout << " Vertex(" << v << ") x = " << mesh.mVertices[v].mPosition.x << ", ";
    std::cout << "y = " << mesh.mVertices[v].mPosition.y << ", ";
    std::cout << "u = " << mesh.mVertices[v].mTexCoords.x << ", ";
    std::cout << "v = " << mesh.mVertices[v].mTexCoords.y << std::endl;
  }

  std::cout << "\n Indices: ";
  for ( SizeType i = 0; i < indexCount; ++i )
  {
    std::cout << " " << mesh.mIndices[ i ];
  }
  std::cout << std::endl;
}

void AtlasManager::OptimizeMesh( const Toolkit::AtlasManager::Mesh2D& in,
                                 Toolkit::AtlasManager::Mesh2D& out )
{
  unsigned short vertexIndex = 0;

  // We could check to see if blocks are next to each other, but it's probably just as quick to compare verts
  for ( SizeType i = 0; i < in.mIndices.Size(); ++i )
  {
    // Fetch a vertex, has it already been assigned?
    bool foundVertex = false;
    Toolkit::AtlasManager::Vertex2D v = in.mVertices[ in.mIndices[ i ] ];
    for ( SizeType j = 0; j < out.mVertices.Size(); ++j )
    {
      if ( ( fabsf( v.mPosition.x - out.mVertices[ j ].mPosition.x ) < Math::MACHINE_EPSILON_1000 ) &&
           ( fabsf( v.mPosition.y - out.mVertices[ j ].mPosition.y ) < Math::MACHINE_EPSILON_1000 ) &&
           ( fabsf( v.mTexCoords.x - out.mVertices[ j ].mTexCoords.x ) < Math::MACHINE_EPSILON_1000 ) &&
           ( fabsf( v.mTexCoords.y - out.mVertices[ j ].mTexCoords.y ) < Math::MACHINE_EPSILON_1000 ) )
      {
        // Yes, so store this down as the vertex to use
        out.mIndices.PushBack( j );
        foundVertex = true;
        break;
      }
    }

    // Did we find a vertex ?
    if ( !foundVertex )
    {
      // No so add a new one
      out.mVertices.PushBack( v );
      vertexIndex++;
    }
  }
}

void AtlasManager::StitchMesh( Toolkit::AtlasManager::Mesh2D& first,
                               const Toolkit::AtlasManager::Mesh2D& second,
                               bool optimize )
{
  uint32_t vc = first.mVertices.Size();

  for ( uint32_t v = 0; v < second.mVertices.Size(); ++v )
  {
    first.mVertices.PushBack( second.mVertices[ v ] );
  }

  for ( uint32_t i = 0; i < second.mIndices.Size(); ++i )
  {
    first.mIndices.PushBack( second.mIndices[ i ] + vc );
  }

  if ( optimize )
  {
    Toolkit::AtlasManager::Mesh2D optimizedMesh;
    OptimizeMesh( first, optimizedMesh );
    first = optimizedMesh;
  }
}

void AtlasManager::StitchMesh( const Toolkit::AtlasManager::Mesh2D& first,
                               const Toolkit::AtlasManager::Mesh2D& second,
                               Toolkit::AtlasManager::Mesh2D& out,
                               bool optimize )
{
  uint32_t vc = first.mVertices.Size();

  for ( uint32_t v = 0; v < vc; ++v )
  {
    out.mVertices.PushBack( first.mVertices[ v ] );
  }

  for ( uint32_t v = 0; v < second.mVertices.Size(); ++v )
  {
    out.mVertices.PushBack( second.mVertices[ v ] );
  }

  for ( uint32_t i = 0; i < first.mIndices.Size(); ++i )
  {
    out.mIndices.PushBack( first.mIndices[ i ] );
  }

  for ( uint32_t i = 0; i < second.mIndices.Size(); ++i )
  {
    out.mIndices.PushBack( second.mIndices[ i ] + vc );
  }

  if ( optimize )
  {
    Toolkit::AtlasManager::Mesh2D optimizedMesh;
    OptimizeMesh( out, optimizedMesh );
    out = optimizedMesh;
  }
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

  SizeType atlasBlockWidth = mAtlasList[ atlas ].mSize.mBlockWidth;
  SizeType atlasBlockHeight = mAtlasList[ atlas ].mSize.mBlockHeight;
  SizeType atlasWidthInBlocks = mAtlasList[ atlas ].mSize.mWidth / mAtlasList[ atlas ].mSize.mBlockWidth;

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
  else
  {
     mUploadedImages.PushBack( const_cast< BufferImage& >( image ).GetBuffer() );
  }

  // Blit top strip
  if ( !mAtlasList[ atlas ].mAtlas.Upload( mAtlasList[ atlas ].mHorizontalStrip,
                                           blockOffsetX,
                                           blockOffsetY ) )
  {
    DALI_LOG_ERROR("Uploading top strip to Atlas Failed!\n");
  }
  else
  {
    mUploadedImages.PushBack( NULL );
  }

  // Blit left strip
  if ( !mAtlasList[ atlas ].mAtlas.Upload( mAtlasList[ atlas ].mVerticalStrip,
                                           blockOffsetX,
                                           blockOffsetY + SINGLE_PIXEL_PADDING ) )
  {
    DALI_LOG_ERROR("Uploading left strip to Atlas Failed!\n");
  }
  else
  {
    mUploadedImages.PushBack( NULL );
  }

  // Blit bottom strip
  if ( blockOffsetY + height + DOUBLE_PIXEL_PADDING <= mAtlasList[ atlas ].mSize.mHeight )
  {
    if ( !mAtlasList[ atlas ].mAtlas.Upload( mAtlasList[ atlas ].mHorizontalStrip,
                                             blockOffsetX,
                                             blockOffsetY + height + SINGLE_PIXEL_PADDING ) )
    {
      DALI_LOG_ERROR("Uploading bottom strip to Atlas Failed!.\n");
    }
    else
    {
     mUploadedImages.PushBack( NULL );
    }
  }

  // Blit right strip
  if ( blockOffsetX + width + DOUBLE_PIXEL_PADDING <= mAtlasList[ atlas ].mSize.mWidth )
  {
    if ( !mAtlasList[ atlas ].mAtlas.Upload( mAtlasList[ atlas ].mVerticalStrip,
                                             blockOffsetX + width + SINGLE_PIXEL_PADDING,
                                             blockOffsetY + SINGLE_PIXEL_PADDING ) )
    {
      DALI_LOG_ERROR("Uploading right strip to Atlas Failed!.\n");
    }
    else
    {
      mUploadedImages.PushBack( NULL );
    }
  }
}

void AtlasManager::GenerateMeshData( ImageId id,
                                     const Vector2& position,
                                     Toolkit::AtlasManager::Mesh2D& meshData,
                                     bool addReference )
{
  // Read the atlas Id to use for this image
  SizeType imageId = id - 1u;
  SizeType atlas = mImageList[ imageId ].mAtlasId - 1u;
  SizeType width = mImageList[ imageId ].mImageWidth;
  SizeType height = mImageList[ imageId ].mImageHeight;

  SizeType widthInBlocks = width / mAtlasList[ atlas ].mSize.mBlockWidth;
  if ( width % mAtlasList[ atlas ].mSize.mBlockWidth )
  {
    widthInBlocks++;
  }
  SizeType heightInBlocks = height / mAtlasList[ atlas ].mSize.mBlockHeight;
  if ( height % mAtlasList[ atlas ].mSize.mBlockHeight )
  {
    heightInBlocks++;
  }

  CreateMesh( atlas, width, height, position, widthInBlocks, heightInBlocks, meshData, mImageList[ imageId ] );

  // Mesh created so increase the reference count, if we're asked to
  if ( addReference )
  {
    mImageList[ imageId ].mCount++;
  }
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

  if ( 2u > --mImageList[ imageId ].mCount )
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

void AtlasManager::SetNewAtlasSize( const Toolkit::AtlasManager::AtlasSize& size )
{
  mNewAtlasSize = size;
  mNewAtlasSize.mBlockWidth += DOUBLE_PIXEL_PADDING;
  mNewAtlasSize.mBlockHeight += DOUBLE_PIXEL_PADDING;
}

const Toolkit::AtlasManager::AtlasSize& AtlasManager::GetAtlasSize( AtlasId atlas )
{
  if ( atlas && atlas-- <= mAtlasList.size() )
  {
    return mAtlasList[ atlas ].mSize;
  }
  return EMPTY_SIZE;
}

AtlasManager::SizeType AtlasManager::GetFreeBlocks( AtlasId atlas ) const
{
  if ( atlas && atlas-- <= mAtlasList.size() )
  {
    return ( mAtlasList[ atlas ].mAvailableBlocks + mAtlasList[ atlas ].mFreeBlocksList.Size() );
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
  return mAtlasList[ --atlas].mPixelFormat;
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

Material AtlasManager::GetMaterial( AtlasId atlas ) const
{
  if ( atlas && atlas-- <= mAtlasList.size() )
  {
    return mAtlasList[ atlas ].mMaterial;
  }
  Material null;
  return null;
}

Sampler AtlasManager::GetSampler( AtlasId atlas ) const
{
  if ( atlas && atlas-- <= mAtlasList.size() )
  {
    return mAtlasList[ atlas ].mSampler;
  }
  Sampler null;
  return null;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali



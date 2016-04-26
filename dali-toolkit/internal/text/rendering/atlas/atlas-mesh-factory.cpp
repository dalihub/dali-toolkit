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
#include <dali-toolkit/internal/text/rendering/atlas/atlas-mesh-factory.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace AtlasMeshFactory
{

void CreateQuad( SizeType imageWidth,
                 SizeType imageHeight,
                 SizeType block,
                 const Toolkit::AtlasManager::AtlasSize& atlasSize,
                 const Vector2& position,
                 Toolkit::AtlasManager::Mesh2D& mesh )
{
  Toolkit::AtlasManager::Vertex2D vertex;

  SizeType blockWidth = atlasSize.mBlockWidth;
  SizeType blockHeight = atlasSize.mBlockHeight;

  float vertexBlockWidth = static_cast< float >( blockWidth );
  float vertexBlockHeight = static_cast< float >( blockHeight );

  SizeType atlasWidth = atlasSize.mWidth;
  SizeType atlasHeight = atlasSize.mHeight;

  SizeType atlasWidthInBlocks = ( atlasWidth - 1u ) / blockWidth;

  // Get the normalized size of a texel in both directions
  float texelX = 1.0f / static_cast< float >( atlasWidth );
  float texelY = 1.0f / static_cast< float >( atlasHeight );

  float oneAndAHalfTexelX = texelX + ( texelX * 0.5f );
  float oneAndAHalfTexelY = texelY + ( texelY * 0.5f );

  float texelBlockWidth = texelX * vertexBlockWidth;
  float texelBlockHeight = texelY * vertexBlockHeight;

  uint32_t pixelsX = imageWidth % blockWidth;
  uint32_t pixelsY = imageHeight % blockHeight;

  if ( !pixelsX )
  {
    pixelsX = blockWidth;
  }
  if ( !pixelsY )
  {
    pixelsY = blockHeight;
  }
  float vertexWidth = static_cast< float >( pixelsX );
  float vertexHeight = static_cast< float >( pixelsY );
  float texelWidth = texelX * vertexWidth;
  float texelHeight = texelY * vertexHeight;

  // We're going to 'blit' half a pixel more on each edge
  vertexWidth++;
  vertexHeight++;

  // Move back half a pixel
  Vector2 topLeft = Vector2( position.x - 0.5f, position.y - 0.5f );

  float fBlockX = texelBlockWidth * static_cast< float >( block % atlasWidthInBlocks );
  float fBlockY = texelBlockHeight * static_cast< float >( block / atlasWidthInBlocks );

  // Add on texture filtering compensation ( half a texel plus compensation for filled pixel in top left corner )
  fBlockX += oneAndAHalfTexelX;
  fBlockY += oneAndAHalfTexelY;

  float texelWidthOffset = texelWidth + texelX;
  float texelHeightOffset = texelHeight + texelY;

  // Top left
  vertex.mPosition.x = topLeft.x;
  vertex.mPosition.y = topLeft.y;
  vertex.mTexCoords.x = fBlockX;
  vertex.mTexCoords.y = fBlockY;

  mesh.mVertices.Reserve( 4u );
  mesh.mVertices.PushBack( vertex );

  // Top Right
  vertex.mPosition.x = topLeft.x + vertexWidth;
  vertex.mPosition.y = topLeft.y;
  vertex.mTexCoords.x = fBlockX + texelWidthOffset;
  vertex.mTexCoords.y = fBlockY;

  mesh.mVertices.PushBack( vertex );

  // Bottom Left
  vertex.mPosition.x = topLeft.x;
  vertex.mPosition.y = topLeft.y + vertexHeight;
  vertex.mTexCoords.x = fBlockX;
  vertex.mTexCoords.y = fBlockY + texelHeightOffset;

  mesh.mVertices.PushBack( vertex );

  // Bottom Right
  vertex.mPosition.x = topLeft.x + vertexWidth;
  vertex.mPosition.y = topLeft.y + vertexHeight;
  vertex.mTexCoords.x = fBlockX + texelWidthOffset;
  vertex.mTexCoords.y = fBlockY + texelHeightOffset;

  mesh.mVertices.PushBack( vertex );

  // Six indices in counter clockwise winding
  mesh.mIndices.Reserve( 6u );
  mesh.mIndices.PushBack( 1u );
  mesh.mIndices.PushBack( 0u );
  mesh.mIndices.PushBack( 2u );
  mesh.mIndices.PushBack( 2u );
  mesh.mIndices.PushBack( 3u );
  mesh.mIndices.PushBack( 1u );
}

void AppendMesh( Toolkit::AtlasManager::Mesh2D& first,
                 const Toolkit::AtlasManager::Mesh2D& second )
{
  const uint32_t verticesCount = first.mVertices.Size();
  first.mVertices.Insert( first.mVertices.End(),
                          second.mVertices.Begin(),
                          second.mVertices.End() );

  const uint32_t indicesCount = first.mIndices.Size();
  first.mIndices.Insert( first.mIndices.End(),
                         second.mIndices.Begin(),
                         second.mIndices.End() );

  for( Vector<unsigned short>::Iterator it = first.mIndices.Begin() + indicesCount,
         endIt = first.mIndices.End();
       it != endIt;
       ++it )
  {
    *it += verticesCount;
  }
}

} // namespace AtlasMeshFactory

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

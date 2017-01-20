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
#include "atlas-packer.h"

// EXTERNAL HEADER
#include <stdlib.h> // For abs()
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

bool ApproximatelyEqual( uint32_t a, uint32_t b  )
{
  return abs( a-b ) <= 1;
}

uint16_t MaxDimension( const Uint16Pair& dimensions )
{
  return dimensions.GetWidth() >= dimensions.GetHeight() ? dimensions.GetWidth() : dimensions.GetHeight();
}

void Swap( Uint16Pair& first, Uint16Pair& second )
{
  Uint16Pair temp = first;
  first = second;
  second = temp;
}

}

AtlasPacker::Node::Node( Node* parent, SizeType x, SizeType y, SizeType width, SizeType height  )
: rectArea( x, y, width, height ),
  parent(parent),
  occupied( false )
{
  child[0] = NULL;
  child[1] = NULL;
}

AtlasPacker:: AtlasPacker( SizeType atlasWidth, SizeType atlasHeight )
: mAvailableArea( atlasWidth * atlasHeight )
{
  mRoot = new Node( NULL, 0u, 0u, atlasWidth, atlasHeight );
}

AtlasPacker::~AtlasPacker()
{
  DeleteNode( mRoot );
}

bool AtlasPacker::Pack( SizeType blockWidth, SizeType blockHeight,
                        SizeType& packPositionX, SizeType& packPositionY)
{
  Node* firstFit = InsertNode( mRoot, blockWidth, blockHeight );
  if( firstFit != NULL )
  {
    firstFit->occupied = true;
    packPositionX = firstFit->rectArea.x;
    packPositionY = firstFit->rectArea.y;
    mAvailableArea -= blockWidth*blockHeight;
    return true;
  }
  return false;
}

void AtlasPacker::DeleteBlock( SizeType packPositionX, SizeType packPositionY, SizeType blockWidth, SizeType blockHeight )
{
  Node* node =  SearchNode( mRoot, packPositionX, packPositionY, blockWidth, blockHeight  );
  if( node != NULL )
  {
    mAvailableArea += blockWidth*blockHeight;
    MergeToNonOccupied( node );
  }
}

unsigned int AtlasPacker::GetAvailableArea() const
{
  return mAvailableArea;
}

AtlasPacker::Node* AtlasPacker::InsertNode( Node* root, SizeType blockWidth, SizeType blockHeight )
{
  if( root == NULL )
  {
    return NULL;
  }

  if( root->occupied )
  {
    // if not the leaf, then try insert into the first child.
    Node* newNode = InsertNode(root->child[0], blockWidth, blockHeight);
    if( newNode == NULL )// no room, try insert into the second child.
    {
      newNode = InsertNode(root->child[1], blockWidth, blockHeight);
    }
    return newNode;
  }

  // too small, return
  if( root->rectArea.width < blockWidth || root->rectArea.height < blockHeight )
  {
    return NULL;
  }

  // right size, accept
  if( root->rectArea.width == blockWidth && root->rectArea.height == blockHeight )
  {
    return root;
  }

  //too much room, need to split
  SplitNode( root, blockWidth, blockHeight );
  // insert into the first child created.
  return InsertNode( root->child[0], blockWidth, blockHeight);
}

void AtlasPacker::SplitNode( Node* node, SizeType blockWidth, SizeType blockHeight )
{
  node->occupied = true;

  // decide which way to split
  SizeType remainingWidth = node->rectArea.width - blockWidth;
  SizeType remainingHeight = node->rectArea.height - blockHeight;

  if( remainingWidth > remainingHeight ) // split vertically
  {
    node->child[0] = new Node( node, node->rectArea.x, node->rectArea.y, blockWidth, node->rectArea.height  );
    node->child[1] = new Node( node, node->rectArea.x+blockWidth, node->rectArea.y, node->rectArea.width-blockWidth, node->rectArea.height );
  }
  else // split horizontally
  {
    node->child[0] = new Node( node, node->rectArea.x, node->rectArea.y, node->rectArea.width, blockHeight  );
    node->child[1] = new Node( node, node->rectArea.x, node->rectArea.y+blockHeight, node->rectArea.width, node->rectArea.height-blockHeight );
  }
}

AtlasPacker::Node* AtlasPacker::SearchNode( Node* node, SizeType packPositionX, SizeType packPositionY, SizeType blockWidth, SizeType blockHeight  )
{
  if( node != NULL )
  {
    if( node->child[0] != NULL) //not a leaf
    {
      Node* newNode = SearchNode(node->child[0], packPositionX, packPositionY, blockWidth, blockHeight);
      if( newNode == NULL )// try search from the second child.
      {
        newNode = SearchNode(node->child[1], packPositionX, packPositionY, blockWidth, blockHeight);
      }
      return newNode;
    }
    else if( ApproximatelyEqual(node->rectArea.x, packPositionX) && ApproximatelyEqual(node->rectArea.y, packPositionY )
        && ApproximatelyEqual(node->rectArea.width, blockWidth) && ApproximatelyEqual( node->rectArea.height, blockHeight) )
    {
      return node;
    }
  }

  return NULL;
}

void AtlasPacker::MergeToNonOccupied( Node* node )
{
  node->occupied = false;
  Node* parent = node->parent;
  // both child are not occupied, merge the space to parent
  if( parent != NULL && parent->child[0]->occupied == false && parent->child[1]->occupied == false)
  {
    delete parent->child[0];
    parent->child[0] = NULL;
    delete parent->child[1];
    parent->child[1] = NULL;

    MergeToNonOccupied( parent );
  }
}

void AtlasPacker::DeleteNode( Node *node )
{
  if( node != NULL )
  {
    DeleteNode( node->child[0] );
    DeleteNode( node->child[1] );
    delete node;
  }
}

Uint16Pair AtlasPacker::GroupPack( const Dali::Vector<Uint16Pair>& blockSizes, Dali::Vector<Uint16Pair>& packPositions )
{
  uint16_t count = blockSizes.Count();
  packPositions.Resize( count );

  // Sort the blocks according to its maximum dimension. The bigger blocks are packed first.
  Dali::Vector<Uint16Pair> packOrder;
  packOrder.Resize( count );
  for( uint16_t i = 0; i < count; i++ )
  {
    packOrder[i].SetX( MaxDimension( blockSizes[i] ) );
    packOrder[i].SetY( i );
  }
  for( uint16_t i = 0; i < count-1; i++ )
    for( uint16_t j = 0; j < count-i-1; j++ )
    {
      if( packOrder[j].GetX() < packOrder[j+1].GetX() )
      {
        Swap( packOrder[j], packOrder[j+1] );
      }
    }

  int index = packOrder[0].GetY();
  AtlasPacker packer( blockSizes[index].GetWidth(), blockSizes[index].GetHeight() );

  SizeType packPositionX, packPositionY;
  // pack the blocks one by one with descending size, grows as necessary to accommodate each subsequent block.
  for( uint16_t i = 0; i < count; i++ )
  {
    index = packOrder[i].GetY();
    packer.GrowPack( blockSizes[index].GetWidth(), blockSizes[index].GetHeight(),
                     packPositionX, packPositionY );
    packPositions[index].SetX( packPositionX );
    packPositions[index].SetY( packPositionY );
  }

  return Uint16Pair( packer.mRoot->rectArea.width, packer.mRoot->rectArea.height );
}

void AtlasPacker::GrowPack( SizeType blockWidth, SizeType blockHeight,
                            SizeType& packPositionX, SizeType& packPositionY )
{
  Node* firstFit = InsertNode( mRoot, blockWidth, blockHeight );
  if( firstFit == NULL )
  {
    // Could fit in the current left space, grow the partition tree to get more space.
    GrowNode( blockWidth, blockHeight );
    firstFit = InsertNode( mRoot->child[1], blockWidth, blockHeight );
  }

  DALI_ASSERT_ALWAYS( firstFit != NULL && "It should never happen!")

  firstFit->occupied = true;
  packPositionX = firstFit->rectArea.x;
  packPositionY = firstFit->rectArea.y;
}

void AtlasPacker::GrowNode( SizeType blockWidth, SizeType blockHeight )
{
  // Attempts to maintain a roughly square ratio when choosing the growing direction: right or down
  bool canGrowRight = blockWidth <= mRoot->rectArea.width;
  bool canGrowDown = blockHeight <= mRoot->rectArea.height;

  bool shouldGrowRight = canGrowRight && mRoot->rectArea.height >= mRoot->rectArea.width+blockWidth;
  bool shouldGrowDown = canGrowDown && mRoot->rectArea.width >= mRoot->rectArea.height+blockHeight;

  if( canGrowRight && canGrowDown )
  {
    shouldGrowRight = mRoot->rectArea.width+blockWidth <= mRoot->rectArea.height+blockHeight;
    shouldGrowDown = !shouldGrowRight;
  }

  if( shouldGrowRight || ( canGrowRight && !shouldGrowDown ) )
  {
    Node* newRoot = new Node( NULL, 0u, 0u, mRoot->rectArea.width+blockWidth, mRoot->rectArea.height );
    newRoot->occupied = true;
    newRoot->child[0] = mRoot;
    newRoot->child[1] = new Node( newRoot, mRoot->rectArea.width, 0u, blockWidth, mRoot->rectArea.height );

    mRoot = newRoot;
  }
  else if( shouldGrowDown || ( canGrowDown && !shouldGrowRight ) )
  {
    Node* newRoot = new Node( NULL, 0u, 0u, mRoot->rectArea.width, mRoot->rectArea.height+blockHeight );
    newRoot->occupied = true;
    newRoot->child[0] = mRoot;
    newRoot->child[1] = new Node( newRoot, 0u, mRoot->rectArea.height, mRoot->rectArea.width, blockHeight );

    mRoot = newRoot;
  }
  else
  {
    DALI_LOG_ERROR( " Atlas Packer failed to grow: make sure the packing order is sorted with the block size to avoid this happening");
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

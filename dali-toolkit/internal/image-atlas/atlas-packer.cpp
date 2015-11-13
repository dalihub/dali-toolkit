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
 *
 */

// CLASS HEADER
#include "atlas-packer.h"

// EXTERNAL HEADER
#include <stdlib.h> // For abs()

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
  if( node == NULL )
  {
    return NULL;
  }

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

  return NULL;
}

void AtlasPacker::MergeToNonOccupied( Node* node )
{
  node->occupied = false;
  Node* parent = node->parent;
  // both child are not occupied, merge the space to parent
  if( parent !=NULL && parent->child[0]->occupied == false && parent->child[1]->occupied == false)
  {
    delete parent->child[0];
    parent->child[0] = NULL;
    delete parent->child[1];
    parent->child[0] = NULL;

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

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

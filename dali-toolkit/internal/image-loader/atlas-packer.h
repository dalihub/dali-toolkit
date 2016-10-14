#ifndef __DALI_TOOLKIT_ATLAS_PACKER_H__
#define __DALI_TOOLKIT_ATLAS_PACKER_H__

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

#include <stdint.h>
#include <dali/public-api/math/rect.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * Binary space tree based bin packing algorithm.
 * It is initialised with a fixed width and height and will fit each block into the first node where it fits
 * and then split that node into 2 parts (down and right) to track the remaining empty space.
 */
class AtlasPacker
{
public:

  /**
   * rectangular area (x,y,width,height)
   */
  typedef uint32_t SizeType;
  typedef Rect<SizeType> RectArea;

  /**
   * Tree node.
   */
  struct Node
  {
    Node( Node* parent, SizeType x, SizeType y, SizeType width, SizeType height );

    RectArea rectArea;
    Node* parent;
    Node* child[2];
    bool occupied;
  };

  /**
   * Constructor.
   *
   * @param[in] atlasWidth The width of the atlas.
   * @param[in] atlasHeight The height of the atlas.
   */
  AtlasPacker( SizeType atlasWidth, SizeType atlasHeight );

  /**
   * Destructor
   */
  ~AtlasPacker();

  /**
   * Pack a block into the atlas.
   *
   * @param[in] blockWidth The width of the block to pack.
   * @param[in] blockHeight The height of the block to pack.
   * @param[out] packPositionX The x coordinate of the position to pack the block.
   * @param[out] packPositionY The y coordinate of the position to pack the block.
   * @return True if there are room for this block, false otherwise.
   */
  bool Pack( SizeType blockWidth, SizeType blockHeight,
             SizeType& packPositionX, SizeType& packPositionY);

  /**
   * Delete the block.
   *
   * @param[in] packPositionX The x coordinate of the pack position.
   * @param[in] packPositionY The y coordinate of the pack position.
   * @param[in] blockWidth The width of the block to delete.
   * @param[in] blockHeight The height of the block to delete.
   */
  void DeleteBlock( SizeType packPositionX, SizeType packPositionY, SizeType blockWidth, SizeType blockHeight );

  /**
   * Query how much empty space left.
   *
   * @return The area available for packing.
   */
  unsigned int GetAvailableArea() const;

private:

  /*
   * Search the node which can pack the block with given size.
   *
   * @param[in] root The root node of the subtree to be searched.
   * @param[in] blockWidth The width of the block to pack.
   * @param[in] blockHeight The height of the block to pack.
   * @return The poniter pointing to node that can pack the block.
   *          If it is NULL, there are no room in the subtree to pack the block.
   */
  Node* InsertNode( Node* root, SizeType blockWidth, SizeType blockHeight );

  /**
   * Split the node into two to fit the block width/size.
   *
   * @parm[in] node The node to split.
   * @param[in] blockWidth The width of the block to pack.
   * @param[in] blockHeight The height of the block to pack.
   */
  void SplitNode( Node* node, SizeType blockWidth, SizeType blockHeight );

  /**
   * Search the node at the given position and with the given size.

   * @param[in] node The root node of the subtree to be searched.
   * @param[in] packPositionX The x coordinate of the pack position.
   * @param[in] packPositionY The y coordinate of the pack position.
   * @param[in] blockWidth The width of the block.
   * @param[in] blockHeight The height of the block.
   */
  Node* SearchNode( Node* node, SizeType packPositionX, SizeType packPositionY, SizeType blockWidth, SizeType blockHeight  );

  /**
   * Merge the rect of the node to non-occupied area.
   *
   * @param[in] node The node to me merged to the non-occupied area
   */
  void MergeToNonOccupied( Node* node );

  /**
   * Delete a node and its subtree.
   *
   * @parm[in] node The node to delete.
   */
  void DeleteNode( Node* node );

  // Undefined
  AtlasPacker( const AtlasPacker& imageAtlas);

  // Undefined
  AtlasPacker& operator=( const AtlasPacker& imageAtlas );

private:

  Node* mRoot; ///< The root of the binary space tree
  unsigned int mAvailableArea;

};


} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_ATLAS_PACKER_H__ */

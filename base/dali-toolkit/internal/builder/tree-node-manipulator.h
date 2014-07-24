#ifndef __DALI_SCRIPT_TREE_NODE_MANIPULATOR_H__
#define __DALI_SCRIPT_TREE_NODE_MANIPULATOR_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include <utility> // pair
#include <iterator>
#include <vector>
#include <cstring>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali-toolkit/public-api/builder/tree-node.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
typedef std::vector<char> VectorChar;
typedef VectorChar::iterator VectorCharIter;

/*
 * TreeNodeManipulator performs modification operations on a TreeNode which are
 * otherwise prohibited on the TreeNode public interface.
 */
class TreeNodeManipulator
{
public:
  /*
   * Constructor
   * @param node The TreeNode to modify
   */
  explicit TreeNodeManipulator(TreeNode* node);

  /*
   * Create a new TreeNode instance
   * @return new TreeNode
   */
  static TreeNode* NewTreeNode();

  /*
   * Shallow copy node data
   * Shallow copy the data but doesnt parent or copy children
   * @param from Node to copy from
   * @param to Node to copy to
   */
  static void ShallowCopy(const TreeNode* from, TreeNode* to);

  /*
   * Moves all string data to a new buffer. There must be enough space for all string data.
   * @param start The buffer start
   * @param sentinel The end of the buffer
   */
  void MoveStrings(VectorCharIter& start, const VectorCharIter& sentinel);

  /*
   * Remove all children from the node
   */
  void RemoveChildren();

  /*
   * Make a deep copy of the tree.
   * @param tree The tree to copy
   * @param numberOfNodes The number of nodes that were copied
   * @param numberOfChars The size of string data.
   */
  static TreeNode* Copy(const TreeNode& tree, int& numberOfNodes, int& numberOfChars);

  /*
   * Add child to the node
   * @param child The child to add
   * @return the added child
   */
  TreeNode *AddChild(TreeNode *child);

  /*
   * Change the type of the Node
   * NB: If the type changes from a type with children to a value type without children then
   *     the children are removed
   * @param type The new type
   */
  void SetType( TreeNode::NodeType type);

  /*
   * Set the name of the node
   * @param name The name to set
   */
  void SetName( const char* name );

  /*
   * Set the substituion flag
   * The substitution flag indicates this nodes string value contains a reference to another node
   * in the tree.
   * @param on The state
   */
  void SetSubstitution( bool on );

  /*
   * Get the nodes type
   * @return The nodes type
   */
  TreeNode::NodeType GetType() const;

  /*
   * Get the number of children of the node
   * @return The number of children
   */
  size_t Size() const;

  /*
   * Set the node as a string value
   * @param string The string value
   */
  void SetString( const char* string );

  /*
   * Set the node as an integer value
   * @param i The integer
   */
  void SetInteger( int i );

  /*
   * Set the node as an float value
   * @param f The float
   */
  void SetFloat( float f );

  /*
   * Set the node as an boolean value
   * @param b The boolean
   */
  void SetBoolean( bool b );

  /*
   * Get the nodes parent
   * @return The nodes parent
   */
  TreeNode* GetParent() const;

  /*
   * Get the nodes child by name
   * @param name The childs name
   * @return The nodes if found, else NULL
   */
  const TreeNode* GetChild(const std::string& name) const;

  /*
   * @copydoc Dali::Scripting::JsonParser::Write()
   */
  void Write(std::ostream& output, int indent) const;

private:
  TreeNode *mNode;

  /*
   * Move the nodes strings to the buffer
   */
  void MoveNodeStrings(VectorCharIter& start, const VectorCharIter& sentinel);

  /*
   * Recursively move child strings to the buffer
   */
  void RecurseMoveChildStrings(VectorCharIter& start, const VectorCharIter& sentinel);

  /*
   * Recursively copy children
   */
  static void CopyChildren(const TreeNode* from, TreeNode* to, int& numberNodes, int& numberChars);

  /*
   * Do write to string stream
   */
  void DoWrite(const TreeNode *value, std::ostream& output, int ident) const;

};

/*
 * Collect nodes
 */
struct CollectNodes : public std::unary_function<TreeNode*, void>
{
  CollectNodes() {};

  /*
   * Call operator to add nodes to the list
   */
  result_type operator()(argument_type& n)
  {
    DALI_ASSERT_DEBUG(n && "Operation on NULL JSON node");
    nodes.push_back(n);
  }

  typedef std::vector<const TreeNode*> VectorNodes;
  typedef VectorNodes::iterator iterator;

  VectorNodes nodes; ///< List of collected nodes
};

/*
 * Depth first walk of nodes applying given operation (unary_function)
 */
template <typename Operation>
void DepthFirst( TreeNode* node, Operation& operation)
{
  DALI_ASSERT_DEBUG(node && "Operation on NULL JSON node");

  for(TreeNode::ConstIterator iter = node->CBegin(); iter != node->CEnd(); ++iter)
  {
    // iterator access is const for external api but were modifying
    DepthFirst( const_cast<TreeNode*>(&((*iter).second)), operation);
  }

  operation(node);

}

/*
 * Recursive search on the tree for the child with the given name
 * @param childName The name to find
 * @param tree The tree to search
 * @return the TreeNode if found, else NULL
 */
const TreeNode* FindIt(const std::string& childName, const TreeNode* tree);

/*
 * Copy string to a buffer
 * Raises if there is not enough space in the buffer
 * @param fromString The string
 * @param iter The start of the buffer
 * @param sentinel The buffer sentinel
 * @return The start of the given buffer
 */
char *CopyString( const char *fromString, VectorCharIter& iter, const VectorCharIter& sentinel);


} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_SCRIPT_TREE_NODE_MANIPULATOR_H__

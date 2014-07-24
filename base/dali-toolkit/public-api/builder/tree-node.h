#ifndef __DALI_SCRIPT_TREE_NODE_H__
#define __DALI_SCRIPT_TREE_NODE_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{

class TreeNodeManipulator;

} // namespace Internal


/*
 * TreeNode describes a tree of nodes.
 * TreeNode does not own its string data which is held by a container eg JsonParser
 * Modification operations should be done through a container.
 */
class TreeNode
{
public:
  /*
   * enum typedef describing the node type
   */
  enum NodeType
  {
    IS_NULL,
    OBJECT,
    ARRAY,
    STRING,
    INTEGER,
    FLOAT,
    BOOLEAN,
  };

  /*
   * Non virtual destructor; this class should not be inherited from.
   */
  ~TreeNode();

  typedef std::pair<const char*, const TreeNode&> KeyNodePair;

  /*
   * Iterator to iterate through children
   */
  class ConstIterator
  {
  public:
    typedef KeyNodePair          value_type;
    typedef KeyNodePair          *pointer;
    typedef const KeyNodePair    *const_pointer;
    typedef KeyNodePair          &reference;
    typedef const KeyNodePair    &const_reference;
    typedef size_t               size_type;
    typedef std::ptrdiff_t       difference_type;
    typedef std::forward_iterator_tag iterator_category;

    /*
     * constructor
     */
    explicit ConstIterator(TreeNode* v);

    /*
     * pre increment
     */
    ConstIterator& operator ++();

    /*
     * post increment
     */
    ConstIterator operator ++(int);

    /*
     * != test
     */
    bool operator!=( const ConstIterator& rhs ) const;

    /*
     * pointer semantics
     */
    KeyNodePair operator*();

    /*
     * pointer semantics
     */
    KeyNodePair operator->();
  private:
    TreeNode* mNode;
  };

  /*
   * Iterate begin() over the nodes children
   * @return a const interator
   */
  ConstIterator CBegin() const;

  /*
   * Iterate end()
   * @return a const interator
   */
  ConstIterator CEnd() const;

  /*
   * Size (number of children)
   * @return The number of children
   */
  size_t Size() const;

  /*
   * Count (the number of children of a sub child node)
   * @param childName The name of the child to find
   * @return the number of children in the found child
   */
  size_t Count(const std::string& childName) const;

  /*
   * Get the nodes name
   * @return The nodes name
   */
  const char* GetName() const;

  /*
   * Gets the nodes type
   * @return The nodes type
   */
  NodeType GetType() const;

  /*
   * Gets the nodes string value
   * Only valid if the type == TreeNode::STRING
   * @return The string value
   */
  const char* GetString() const;

  /*
   * Gets the nodes float value
   * Only valid if the type == TreeNode::FLOAT
   * @return The float value
   */
  float GetFloat() const;

  /*
   * Gets the nodes integer value
   * Only valid if the type == TreeNode::INTEGER
   * @return The integer value
   */
  int GetInteger() const;

  /*
   * Gets the nodes boolean value
   * Only valid if the type == TreeNode::BOOLEAN
   * @return The boolean value
   */
  bool GetBoolean() const;

  /*
   * Gets the substituion flag
   * Only valid if the type == TreeNode::STRING
   * @return The substitution flag
   */
  bool HasSubstitution() const;

  /*
   * Gets a child of the node
   * @param name The name of the child
   * @return The child if found, else NULL
   */
  const TreeNode* GetChild(const std::string& name) const;

  /*
   * Recursively search for a child of the node
   * @param name The name of the child
   * @return The child if found, else NULL
   */
  const TreeNode* Find(const std::string& name) const;

private:
  friend class Internal::TreeNodeManipulator;

  /*
   * Constructor
   */
  TreeNode();

  // non copyable or assignable
  TreeNode(TreeNode &);
  TreeNode& operator=(const TreeNode&);

  const char* mName;                   ///< The nodes name (if any)

  TreeNode* mParent;                   ///< The nodes parent
  TreeNode* mNextSibling;              ///< The nodes next sibling
  TreeNode* mFirstChild;               ///< The nodes first child
  TreeNode* mLastChild;                ///< The nodes last child

  union
  {
    const char* mStringValue;          ///< The node string value
    int mIntValue;                     ///< The node integer value
    float mFloatValue;                 ///< The node float value
  };

  NodeType mType;                      ///< The nodes type
  bool mSubstituion;                   ///< String substitution flag

};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_SCRIPT_TREE_NODE_H__

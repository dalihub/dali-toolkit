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

// EXTERNAL INCLUDES
#include <cstring>
#include <sstream>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/builder/tree-node-manipulator.h>

#include <dali-toolkit/devel-api/builder/tree-node.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

void Indent(std::ostream& o, int level, int indentWidth)
{
  for (int i = 0; i < level*indentWidth; ++i)
  {
    o << " ";
  }
}

std::string EscapeQuotes( const char* aString)
{
  std::string escapedString;
  int length = strlen(aString);
  escapedString.reserve(length);

  const char* end = aString+length;
  for( const char* iter = aString; iter != end ; ++iter)
  {
    if(*iter != '\"')
    {
      escapedString.push_back(*iter);
    }
    else
    {
      escapedString.append("\\\"");
    }
  }
  return escapedString;
}

} // anonymous namespace

TreeNodeManipulator::TreeNodeManipulator(TreeNode* node)
  : mNode(node)
{
}

TreeNode* TreeNodeManipulator::NewTreeNode()
{
  return new TreeNode();
}

void TreeNodeManipulator::ShallowCopy(const TreeNode* from, TreeNode* to)
{
  DALI_ASSERT_DEBUG(from);
  DALI_ASSERT_DEBUG(to);

  if( from )
  {
    to->mName         = from->mName;
    to->mType         = from->mType;
    to->mSubstituion  = from->mSubstituion;
    switch(from->mType)
    {
      case TreeNode::INTEGER:
      {
        to->mIntValue = from->mIntValue;
        break;
      }
      case TreeNode::FLOAT:
      {
        to->mFloatValue = from->mFloatValue;
        break;
      }
      case TreeNode::STRING:
      {
        to->mStringValue = from->mStringValue;
        break;
      }
      case TreeNode::BOOLEAN:
      {
        to->mIntValue = from->mIntValue;
        break;
      }
      case TreeNode::IS_NULL:
      case TreeNode::OBJECT:
      case TreeNode::ARRAY:
      {
        break;
      }
    }
  }

}

void TreeNodeManipulator::MoveNodeStrings(VectorCharIter& start, const VectorCharIter& sentinel)
{
  DALI_ASSERT_DEBUG(mNode && "Operation on NULL JSON node");
  if(mNode->mName)
  {
    mNode->mName = CopyString(mNode->mName, start, sentinel);
  }

  if(TreeNode::STRING == mNode->mType)
  {
    mNode->mStringValue = CopyString(mNode->mStringValue, start, sentinel);
  }
}

void TreeNodeManipulator::MoveStrings(VectorCharIter& start, const VectorCharIter& sentinel)
{
  DALI_ASSERT_DEBUG(mNode && "Operation on NULL JSON node");
  TreeNodeManipulator modify(mNode);
  modify.MoveNodeStrings(start, sentinel);
  RecurseMoveChildStrings(start, sentinel);
}

void TreeNodeManipulator::RecurseMoveChildStrings(VectorCharIter& start, const VectorCharIter& sentinel)
{
  DALI_ASSERT_DEBUG(mNode && "Operation on NULL JSON node");

  TreeNode* child = mNode->mFirstChild;
  while(child)
  {
    TreeNodeManipulator manipChild(child);
    manipChild.MoveNodeStrings(start, sentinel);
    child = child->mNextSibling;
  }

  child = mNode->mFirstChild;
  while(child)
  {
    TreeNodeManipulator manipChild(child);
    manipChild.RecurseMoveChildStrings(start, sentinel);
    child = child->mNextSibling;
  }
}

void TreeNodeManipulator::RemoveChildren()
{
  DALI_ASSERT_DEBUG(mNode && "Operation on NULL JSON node");

  CollectNodes collector;

  DepthFirst( mNode, collector );

  for(CollectNodes::iterator iter = collector.nodes.begin(); iter != collector.nodes.end(); ++iter)
  {
    if( *iter != mNode)
    {
      delete *iter;
    }
  }

  mNode->mFirstChild = NULL;
  mNode->mLastChild  = NULL;
}

TreeNode* TreeNodeManipulator::Copy(const TreeNode& tree, int& numberNodes, int& numberChars)
{
  TreeNode* root = NewTreeNode();

  ShallowCopy(&tree, root);

  if(tree.mName)
  {
    numberChars += std::strlen(tree.mName) + 1;
  }

  if(TreeNode::STRING == tree.mType)
  {
    numberChars += std::strlen(tree.mStringValue) + 1;
  }

  ++numberNodes;

  CopyChildren(&tree, root, numberNodes, numberChars);

  return root;
}

void TreeNodeManipulator::CopyChildren(const TreeNode* from, TreeNode* to, int& numberNodes, int& numberChars)
{
  DALI_ASSERT_DEBUG(from && "Operation on NULL JSON node");
  DALI_ASSERT_DEBUG(to);

  for( TreeNode::ConstIterator iter = from->CBegin(); iter != from->CEnd(); ++iter)
  {
    const TreeNode* child = &((*iter).second);
    if(child->mName)
    {
      numberChars += std::strlen(child->mName) + 1;
    }

    if(TreeNode::STRING == child->mType)
    {
      numberChars += std::strlen(child->mStringValue) + 1;
    }

    TreeNode* newNode = NewTreeNode();

    ShallowCopy(child, newNode);

    TreeNodeManipulator modify(to);

    modify.AddChild(newNode);

    ++numberNodes;

    CopyChildren(child, newNode, numberNodes, numberChars);
  }
}

TreeNode *TreeNodeManipulator::AddChild(TreeNode *rhs)
{
  DALI_ASSERT_DEBUG(mNode && "Operation on NULL JSON node");

  rhs->mParent = mNode;
  if (mNode->mLastChild)
  {
    mNode->mLastChild = mNode->mLastChild->mNextSibling = rhs;
  }
  else
  {
    mNode->mFirstChild = mNode->mLastChild = rhs;
  }
  return rhs;
}

TreeNode::NodeType TreeNodeManipulator::GetType() const
{
  DALI_ASSERT_DEBUG(mNode && "Operation on NULL JSON node");

  return mNode->GetType();
}

size_t TreeNodeManipulator::Size() const
{
  DALI_ASSERT_DEBUG(mNode && "Operation on NULL JSON node");

  return mNode->Size();
}

void TreeNodeManipulator::SetType( TreeNode::NodeType type)
{
  DALI_ASSERT_DEBUG(mNode && "Operation on NULL JSON node");

  if( mNode->mType != type )
  {
    mNode->mType = type;

    if( NULL != mNode->mFirstChild )
    {
      // value types have no children
      bool removeChildren = ! (TreeNode::OBJECT == type || TreeNode::ARRAY == type);

      // ie if swapping array for object
      removeChildren = (removeChildren == true) ? true : type != mNode->mType;

      // so remove any children
      if( removeChildren && NULL != mNode->mFirstChild)
      {
        RemoveChildren();
      }
    }
  }
  else if( TreeNode::ARRAY == mNode->mType )
  {
    if( mNode->mFirstChild != NULL )
    {
      TreeNode::NodeType type = mNode->mFirstChild->GetType();

      if( TreeNode::FLOAT == type || TreeNode::INTEGER == type )
      {
        // Arrays of numbers should be replaced, not appended to.
        RemoveChildren();
      }
    }
  }
}

void TreeNodeManipulator::SetName( const char* name )
{
  DALI_ASSERT_DEBUG(mNode && "Operation on NULL JSON node");
  mNode->mName = name;
}

void TreeNodeManipulator::SetSubstitution( bool b )
{
  DALI_ASSERT_DEBUG(mNode && "Operation on NULL JSON node");
  mNode->mSubstituion = b;
}

TreeNode* TreeNodeManipulator::GetParent() const
{
  DALI_ASSERT_DEBUG(mNode && "Operation on NULL JSON node");
  return NULL == mNode ? NULL : mNode->mParent;
}

const TreeNode* TreeNodeManipulator::GetChild(const std::string& name) const
{
  DALI_ASSERT_DEBUG(mNode && "Operation on NULL JSON node");
  return NULL == mNode ? NULL : mNode->GetChild(name);
}

void TreeNodeManipulator::SetString( const char* string )
{
  DALI_ASSERT_DEBUG(mNode && "Operation on NULL JSON node");
  SetType(TreeNode::STRING);
  mNode->mStringValue = string;
}

void TreeNodeManipulator::SetInteger( int i )
{
  DALI_ASSERT_DEBUG(mNode && "Operation on NULL JSON node");
  SetType(TreeNode::INTEGER);
  mNode->mIntValue = i;
}

void TreeNodeManipulator::SetFloat( float f )
{
  DALI_ASSERT_DEBUG(mNode && "Operation on NULL JSON node");
  SetType(TreeNode::FLOAT);
  mNode->mFloatValue = f;
}

void TreeNodeManipulator::SetBoolean( bool b )
{
  DALI_ASSERT_DEBUG(mNode && "Operation on NULL JSON node");
  SetType(TreeNode::BOOLEAN);
  mNode->mIntValue = b == true ? 1 : 0;
}

void TreeNodeManipulator::Write(std::ostream& output, int indent) const
{
  DALI_ASSERT_DEBUG(mNode && "Operation on NULL JSON node");
  DoWrite(mNode, output, 0, indent, false);
}

void TreeNodeManipulator::DoWrite(const TreeNode *value, std::ostream& output, int level, int indentWidth, bool groupChildren) const
{
  DALI_ASSERT_DEBUG(value && "Operation on NULL JSON node");

  if(!groupChildren)
  {
    Indent(output, level, indentWidth);
  }

  if (value->GetName())
  {
    output << "\"" << value->GetName() << "\":";
  }

  switch(value->GetType())
  {
    case TreeNode::IS_NULL:
    {
      output << "null";
      if(NULL != value->mNextSibling)
      {
        output << ", ";
      }
      if( !groupChildren )
      {
        output << std::endl;
      }
      break;
    }
    case TreeNode::OBJECT:
    case TreeNode::ARRAY:
    {
      bool groupMyChildren = false;

      if( TreeNode::ARRAY == value->GetType() &&
          ( TreeNode::INTEGER == value->mFirstChild->GetType() ||
            TreeNode::FLOAT   == value->mFirstChild->GetType() ) )
      {
        groupMyChildren = true;
      }

      if( value->GetType() == TreeNode::OBJECT)
      {
        output << std::endl;
        Indent(output, level, indentWidth);
        output << "{";
      }
      else
      {
        if( !groupMyChildren )
        {
          output << std::endl;
          Indent(output, level, indentWidth);
        }
        output << "[";
      }

      if( groupMyChildren )
      {
        output << " ";
      }
      else
      {
        output << std::endl;
      }

      for (TreeNode::ConstIterator it = value->CBegin(); it != value->CEnd(); ++it)
      {
        DoWrite( &((*it).second), output, level+1, indentWidth, groupMyChildren );
      }

      if( !groupMyChildren )
      {
        Indent(output, level, indentWidth);
      }

      if( value->GetType() == TreeNode::OBJECT )
      {
        output << "}";
      }
      else
      {
        output << "]";
      }

      if( NULL != value->mNextSibling )
      {
        output << ",";
      }

      if( !groupChildren )
      {
        output << std::endl;
      }

      groupChildren = false;
      break;
    }
    case TreeNode::STRING:
    {
      std::string escapedString = EscapeQuotes(value->GetString());
      output << "\"" << escapedString << "\"";
      if(NULL != value->mNextSibling)
      {
        output << ",";
      }

      if( groupChildren )
      {
        output << " ";
      }
      else
      {
        output << std::endl;
      }
      break;
    }
    case TreeNode::INTEGER:
    {
      output << value->GetInteger();
      if(NULL != value->mNextSibling)
      {
        output << ",";
      }

      if( groupChildren )
      {
        output << " ";
      }
      else
      {
        output << std::endl;
      }
      break;
    }
    case TreeNode::FLOAT:
    {
      output.setf( std::ios::floatfield );
      output << value->GetFloat();
      output.unsetf( std::ios::floatfield );
      if(NULL != value->mNextSibling)
      {
        output << ",";
      }

      if( groupChildren )
      {
        output << " ";
      }
      else
      {
        output << std::endl;
      }
      break;
    }
    case TreeNode::BOOLEAN:
    {
      if( value->GetInteger() )
      {
        output << "true";
      }
      else
      {
        output << "false";
      }

      if(NULL != value->mNextSibling)
      {
        output << ",";
      }

      if( groupChildren )
      {
        output << " ";
      }
      else
      {
        output << std::endl;
      }

      break;
    }
  } // switch
} // DoWrite


const TreeNode* FindIt(const std::string& childName, const TreeNode* node)
{
  DALI_ASSERT_DEBUG(node);

  const TreeNode* found = NULL;

  if( node )
  {
    if( NULL != (found = node->GetChild(childName)) )
    {
      return found;
    }
    else
    {
      for(TreeNode::ConstIterator iter = node->CBegin(); iter != node->CEnd(); ++iter)
      {
        if( NULL != (found = FindIt(childName, &((*iter).second)) ) )
        {
          return found;
        }
      }
    }
  }
  return found;
}

char *CopyString( const char *fromString, VectorCharIter& iter, const VectorCharIter& sentinel)
{
  DALI_ASSERT_DEBUG(fromString);
  DALI_ASSERT_DEBUG(iter != sentinel);

  char *start= &(*iter);
  const char *ptr = fromString;

  if(ptr)
  {
    while(*ptr != 0)
    {
      DALI_ASSERT_DEBUG(iter != sentinel);
      *iter++ = *ptr++;
    }

    *iter++ = 0;
  }
  return start;
}


} // namespace internal

} // namespace Toolkit

} // namespace Dali

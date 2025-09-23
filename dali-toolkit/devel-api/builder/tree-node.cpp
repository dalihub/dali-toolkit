/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>

// INTERNAL INCLUDES
#include "dali-toolkit/devel-api/builder/tree-node.h"
#include "dali-toolkit/internal/builder/tree-node-manipulator.h"

namespace Dali
{
/**
 * return true if the lower cased ASCII strings are equal.
 */
bool CaseInsensitiveStringCompare(std::string_view a, std::string_view b)
{
  return (a.length() == b.length()) && std::equal(a.begin(), a.end(), b.begin(), [](auto x, auto y)
  {
    return std::tolower(x) == std::tolower(y);
  });
}

namespace Toolkit
{
TreeNode::TreeNode()
: mName(NULL),
  mParent(NULL),
  mNextSibling(NULL),
  mFirstChild(NULL),
  mLastChild(NULL),
  mStringValue(NULL),
  mType(TreeNode::IS_NULL),
  mSubstituion(false)
{
}

TreeNode::~TreeNode()
{
}

const char* TreeNode::GetName() const
{
  return mName;
}

TreeNode::NodeType TreeNode::GetType() const
{
  return mType;
}

const char* TreeNode::GetString() const
{
  return mStringValue;
}

bool TreeNode::HasSubstitution() const
{
  return mSubstituion;
}

float TreeNode::GetFloat() const
{
  return mFloatValue;
}

int TreeNode::GetInteger() const
{
  return mIntValue;
}

bool TreeNode::GetBoolean() const
{
  return mIntValue == 1 ? true : false;
}

size_t TreeNode::Size() const
{
  size_t    c = 0;
  TreeNode* p = mFirstChild;
  while(p)
  {
    c++;
    p = p->mNextSibling;
  }
  return c;
}

size_t TreeNode::Count(std::string_view childName) const
{
  const TreeNode* c = GetChild(childName);
  if(c)
  {
    return c->Size();
  }
  else
  {
    return 0;
  }
}

const TreeNode* TreeNode::GetChild(std::string_view childName) const
{
  const TreeNode* p = mFirstChild;
  while(p)
  {
    if(p->mName && (std::string_view(p->mName) == childName))
    {
      return p;
    }
    p = p->mNextSibling;
  }
  return NULL;
}

const TreeNode* TreeNode::GetChildIgnoreCase(std::string_view childName) const
{
  const TreeNode* p = mFirstChild;
  while(p)
  {
    if(p->mName && CaseInsensitiveStringCompare(p->mName, childName))
    {
      return p;
    }
    p = p->mNextSibling;
  }
  return NULL;
}

const TreeNode* TreeNode::Find(std::string_view childName) const
{
  if(mName && std::string_view(mName) == childName)
  {
    return this;
  }
  else
  {
    return Internal::FindIt(childName, this);
  }
}

TreeNode::ConstIterator TreeNode::CBegin() const
{
  return ConstIterator(mFirstChild);
}

TreeNode::ConstIterator TreeNode::CEnd() const
{
  return ConstIterator(NULL);
}

TreeNode::ConstIterator::ConstIterator(TreeNode* v)
: mNode(v)
{
}

TreeNode::ConstIterator& TreeNode::ConstIterator::operator++()
{
  if(mNode)
  {
    mNode = mNode->mNextSibling;
  }
  else
  {
    mNode = NULL;
  }
  return *this;
}

TreeNode::ConstIterator TreeNode::ConstIterator::operator++(int)
{
  TreeNode::ConstIterator ret(mNode);

  if(mNode)
  {
    mNode = mNode->mNextSibling;
  }
  else
  {
    mNode = NULL;
  }
  return ret;
}

TreeNode::KeyNodePair TreeNode::ConstIterator::operator*()
{
  return KeyNodePair(mNode->mName, *mNode);
}

bool TreeNode::ConstIterator::operator!=(const TreeNode::ConstIterator& rhs) const
{
  return mNode != rhs.mNode;
}

} // namespace Toolkit

} // namespace Dali

#ifndef __DALI_TOOLKIT_INTERNAL_BUILDER_GET_IS_INL__
#define __DALI_TOOLKIT_INTERNAL_BUILDER_GET_IS_INL__

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

// INTERNAL INCLUDES
#include <dali-toolkit/internal/builder/builder-declarations.h>

inline OptionalChild IsChild(const TreeNode* node, const std::string& childName)
{
  if( node )
  {
    const TreeNode* c = node->GetChild(childName);
    if( NULL != c )
    {
      return OptionalChild( *c );
    }
    else
    {
      return OptionalChild();
    }
  }
  else
  {
    return OptionalChild();
  }
}

inline OptionalChild IsChild(const TreeNode& node, const std::string& childName)
{
  return IsChild(&node, childName);
}

inline OptionalString IsString(const OptionalChild& node)
{
  if( node && (*node).GetType() == TreeNode::STRING )
  {
    return OptionalString((*node).GetString());
  }
  else
  {
    return OptionalString();
  }
}
inline OptionalFloat IsFloat(const OptionalChild& node)
{
  OptionalFloat ret;

  if( node )
  {
    if( (*node).GetType() == TreeNode::FLOAT )
    {
      ret = (*node).GetFloat();
    }
    else if( (*node).GetType() == TreeNode::INTEGER )
    {
      // JSON has number not float/int but JsonParser discriminates.
      // Here we don't care so we allow coercion
      ret = static_cast<float>( (*node).GetInteger() );
    }
  }

  return ret;
}

inline OptionalInteger IsInteger(const OptionalChild &node)
{
  OptionalInteger ret;

  if( node )
  {
    if( (*node).GetType() == TreeNode::INTEGER )
    {
      ret = (*node).GetInteger();
    }
    else if( (*node).GetType() == TreeNode::FLOAT )
    {
      ret = static_cast<int>(  (*node).GetFloat() );
    }
  }

  return ret;
}

inline OptionalBoolean IsBoolean(const OptionalChild& node)
{
  if( node && (*node).GetType() == TreeNode::BOOLEAN )
  {
    return OptionalBoolean(1 == (*node).GetInteger());
  }
  else
  {
    return OptionalBoolean();
  }
}

// copy N Numbers
template <typename T>
inline bool CopyNumbers(TreeNode::ConstIterator iter, int N, T& vector)
{
  for(int i = 0; i < N; ++i)
  {
    if( (*iter).second.GetType() == TreeNode::FLOAT)
    {
      vector[i] = (*iter).second.GetFloat();
    }
    else if( (*iter).second.GetType() == TreeNode::INTEGER )
    {
      vector[i] = static_cast<float>((*iter).second.GetInteger());
    }
    else
    {
      return false;
    }
    iter++;
  }

  return true;
}

inline OptionalVector4 IsVector4(const OptionalChild& node)
{
  OptionalVector4 ret;

  if( node && (TreeNode::ARRAY == (*node).GetType()) && (*node).Size() >= 4 )
  {
    Dali::Vector4 v;
    if( CopyNumbers((*node).CBegin(), 4, v) )
    {
      ret = OptionalVector4(v);
    }
  }

  return ret;
}

inline OptionalVector3 IsVector3(const OptionalChild& node)
{
  OptionalVector3 ret;

  if( node && (TreeNode::ARRAY == (*node).GetType()) && (*node).Size() >= 3 )
  {
    Dali::Vector3 v;
    if( CopyNumbers((*node).CBegin(), 3, v) )
    {
      ret = OptionalVector3(v);
    }
  }

  return ret;
}

inline OptionalVector2 IsVector2(const OptionalChild& node)
{
  OptionalVector2 ret;

  if( node && (TreeNode::ARRAY == (*node).GetType()) && (*node).Size() >= 2 )
  {
    Dali::Vector2 v;
    if( CopyNumbers((*node).CBegin(), 2, v) )
    {
      ret = OptionalVector2(v);
    }
  }

  return ret;
}

inline OptionalMatrix IsMatrix(const OptionalChild &node)
{
  OptionalMatrix ret;

  if( node && (TreeNode::ARRAY == (*node).GetType()) && (*node).Size() >= 16 )
  {
    float v[16];
    if( CopyNumbers((*node).CBegin(), 16, v) )
    {
      ret = OptionalMatrix(Dali::Matrix(v));
    }
  }

  return ret;
}

inline OptionalMatrix3 IsMatrix3(const OptionalChild& node)
{
  OptionalMatrix3 ret;

  if( node && (TreeNode::ARRAY == (*node).GetType()) && (*node).Size() >= 9 )
  {
    float v[9];
    if( CopyNumbers((*node).CBegin(), 9, v) )
    {
      ret = OptionalMatrix3(Dali::Matrix3(v[0], v[1], v[2],
                                          v[3], v[4], v[5],
                                          v[6], v[7], v[8] ));
    }
  }

  return ret;
}

inline OptionalRect IsRect(const OptionalChild& node)
{
  OptionalRect ret;
  if(node && (*node).Size())
  {
    if((*node).Size() >= 4)
    {
      TreeNode::ConstIterator iter((*node).CBegin());
      int v[4];
      if( CopyNumbers((*node).CBegin(), 4, v) )
      {
        ret = OptionalRect(Dali::Rect<int>(v[0], v[1], v[2], v[3]));
      }
    }
  }
  return ret;
}

//
//
//
inline OptionalString IsString( const TreeNode& parent, const std::string& childName)
{
  return IsString( IsChild(&parent, childName) );
}

inline OptionalFloat IsFloat( const TreeNode& parent, const std::string& childName)
{
  return IsFloat( IsChild(&parent, childName) );
}

inline OptionalInteger IsInteger( const TreeNode& parent, const std::string& childName)
{
  return IsInteger( IsChild(&parent, childName) );
}

inline OptionalBoolean IsBoolean( const TreeNode& parent, const std::string& childName)
{
  return IsBoolean( IsChild(parent, childName) );
}

inline OptionalVector4 IsVector4(const TreeNode &parent, const std::string& childName)
{
  return IsVector4( IsChild(parent, childName) );
}

inline OptionalVector3 IsVector3(const TreeNode &parent, const std::string& childName)
{
  return IsVector3( IsChild(parent, childName) );
}

inline OptionalVector2 IsVector2(const TreeNode &parent, const std::string& childName)
{
  return IsVector2( IsChild(parent, childName) );
}

inline OptionalMatrix IsMatrix(const TreeNode &parent, const std::string& childName)
{
  return IsMatrix( IsChild(parent, childName) );
}

inline OptionalMatrix3 IsMatrix3(const TreeNode &parent, const std::string& childName)
{
  return IsMatrix3( IsChild(&parent, childName) );
}

inline OptionalRect IsRect(const TreeNode &parent, const std::string& childName)
{
  return IsRect( IsChild(&parent, childName) );
}

//
//
//
inline OptionalString IsString( const TreeNode& node )
{
  return IsString( OptionalChild( node ) );
}

inline OptionalFloat IsFloat( const TreeNode& node )
{
  return IsFloat( OptionalChild( node ) );
}

inline OptionalInteger IsInteger( const TreeNode& node )
{
  return IsInteger( OptionalChild( node ) );
}

inline OptionalBoolean IsBoolean( const TreeNode& node )
{
  return IsBoolean( OptionalChild( node ) );
}

inline OptionalVector4 IsVector4(const TreeNode &node )
{
  return IsVector4( OptionalChild( node ) );
}

inline OptionalVector3 IsVector3(const TreeNode &node )
{
  return IsVector3( OptionalChild( node ) );
}

inline OptionalVector2 IsVector2(const TreeNode &node )
{
  return IsVector2( OptionalChild( node ) );
}

inline OptionalMatrix IsMatrix(const TreeNode &node )
{
  return IsMatrix( OptionalChild( node ) );
}

inline OptionalMatrix3 IsMatrix3(const TreeNode &node )
{
  return IsMatrix3( OptionalChild( node ) );
}

inline OptionalRect IsRect(const TreeNode &node )
{
  return IsRect( OptionalChild( node ) );
}

//
//
//
inline Dali::Vector4 GetVector4(const TreeNode &child)
{
  OptionalVector4 v( IsVector4( OptionalChild( child ) ) );
  DALI_ASSERT_ALWAYS(v);
  return *v;
}

inline Dali::Vector3 GetVector3(const TreeNode &child)
{
  OptionalVector3 v( IsVector3( OptionalChild( child ) ) );
  DALI_ASSERT_ALWAYS(v);
  return *v;
}

inline Dali::Vector2 GetVector2(const TreeNode &child)
{
  OptionalVector2 v( IsVector2( OptionalChild( child ) ) );
  DALI_ASSERT_ALWAYS(v);
  return *v;
}

inline float GetFloat(const TreeNode &child)
{
  OptionalFloat v( IsFloat( OptionalChild( child ) ) );
  DALI_ASSERT_ALWAYS(v);
  return *v;
}

inline bool GetBoolean(const TreeNode &child)
{
  OptionalBoolean v( IsBoolean( OptionalChild( child ) ) );
  DALI_ASSERT_ALWAYS(v);
  return *v;
}

inline int GetInteger(const TreeNode &child)
{
  OptionalInteger v( IsInteger( OptionalChild( child ) ) );
  DALI_ASSERT_ALWAYS(v);
  return *v;
}



#endif // __DALI_TOOLKIT_INTERNAL_BUILDER_GET_IS_INL__

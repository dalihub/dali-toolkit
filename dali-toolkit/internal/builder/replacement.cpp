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
#include <dali-toolkit/internal/builder/replacement.h>
#include <dali-toolkit/internal/builder/builder-impl.h>
#include <dali-toolkit/internal/builder/builder-get-is.inl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace  // anon
{

Property::Value* FindReplacement( const std::string &str, const Property::Map& overrideMap, const Property::Map& defaultMap )
{
  Property::Value* ret  = overrideMap.Find( str );

  if ( !ret )
  {
    ret = defaultMap.Find( str );

    // @ todo
    // try localized text ie dgettext. Look for colon  {DOMAIN:TEXT} {LC_MESSAGE:ID_XXXX}
  }

  return ret;
}

std::size_t FirstUnescapedChar(const std::string &initialValue, const std::size_t& startPos, const char c)
{
  std::size_t pos = initialValue.find( c, startPos );

  if(pos > 0)
  {
    while( pos != std::string::npos )
    {
      if( '\\' == initialValue.at( pos-1 ) )
      {
        pos = initialValue.find( c, pos );
      }
      else
      {
        break;
      }
    }
  }

  return pos;
}

bool GetSubstitutionPosition( const std::string &initialValue, std::size_t &startPos, std::size_t &size )
{
  std::size_t pos = FirstUnescapedChar(initialValue, 0, '{');

  if( std::string::npos == pos )
  {
    startPos = std::string::npos;
    return false;
  }
  else
  {
    startPos = pos + 1;
  }

  pos = FirstUnescapedChar(initialValue, startPos, '}');

  if( std::string::npos == pos )
  {
    size = std::string::npos;
    return false;
  }
  else
  {
    size = pos - startPos;
  }

  return true;
}

bool ResolvePartialReplacement( const std::string &initialValue, Property::Value &out,
                                const Property::Map& overrideMap, const Property::Map& defaultMap )
{

  if( initialValue.size() >= 2 )
  {
    // eg '{"constants": { "IMAGE_DIR": "/share/images" },
    //      ...
    //        "filename":"{IMAGE_DIR}/theme/header.png",
    //
    std::size_t startPos = 0;
    std::size_t size     = std::string::npos;

    if( !GetSubstitutionPosition( initialValue, startPos, size ) )
    {
      out = initialValue;
      return true;
    }
    else
    {
      const std::string str( initialValue.substr( startPos, size ) );

      Property::Value* value = FindReplacement( str, overrideMap, defaultMap );

      if( !value )
      {
        DALI_SCRIPT_WARNING( "Cannot find replacement for '%s'\n", str.c_str() );
      }
      else
      {
        if( Property::STRING != value->GetType() )
        {
          DALI_SCRIPT_WARNING( "Cannot replace substring in non string property type='%s'. Initial value '%s'\n",
                               PropertyTypes::GetName( out.GetType() ), initialValue.c_str() );
        }
        else
        {
          std::string newString = \
            initialValue.substr(0, startPos - 1) +
            value->Get< std::string >() +
            initialValue.substr( startPos + size + 1 );

          return ResolvePartialReplacement( newString, out, overrideMap,  defaultMap );
        }
      }
    }
  }

  // if we get here we failed
  return false;
}

} // namespace anon


Replacement::Replacement( const Property::Map& overrideMap, const Property::Map& defaultMap )
  : mOverrideMap( &overrideMap ), mDefaultMap( &defaultMap )
{

}

namespace
{
Property::Map noMap;
}

Replacement::Replacement( const Property::Map& defaultMap )
  : mOverrideMap( &noMap ), mDefaultMap( &defaultMap )
{

}

Replacement::Replacement(  )
  : mOverrideMap( &noMap ), mDefaultMap( &noMap )
{

}

OptionalString Replacement::HasFullReplacement( const TreeNode & node ) const
{
  OptionalString ret;

  if( node.HasSubstitution() && ((*mOverrideMap).Count() || (*mDefaultMap).Count()) )
  {
    OptionalString v = ::IsString( node );
    if( v )
    {
      const std::string& initialValue = *v;
      if( (initialValue[ 0 ] == '{') && (initialValue[ initialValue.size() -1 ] == '}') )
      {
        ret = initialValue.substr( 1, initialValue.size() - 2 );
      }
    }
  }
  return ret;
}

Property::Value Replacement::GetFullReplacement( const std::string& replacementString ) const
{
  Property::Value out;
  DALI_ASSERT_DEBUG( mOverrideMap && "missing map");
  DALI_ASSERT_DEBUG( mDefaultMap && "missing map");

  Property::Value* value = FindReplacement( replacementString, *mOverrideMap, *mDefaultMap );

  if( !value )
  {
    DALI_SCRIPT_WARNING("Cannot find replacement for '%s'\n", replacementString.c_str());
  }
  else
  {
    out = *value;
#if defined(DEBUG_ENABLED)
    DALI_SCRIPT_VERBOSE("  Full replacement for '%s' => to Type '%s'\n",
                        replacementString.c_str(),
                        PropertyTypes::GetName( out.GetType()) );
#endif
  }

  return out;
}

OptionalBoolean Replacement::IsBoolean( const TreeNode & node ) const
{
  OptionalBoolean ret;
  if( OptionalString replace = HasFullReplacement( node ) )
  {
    Property::Value value = GetFullReplacement( *replace );
    if( Property::BOOLEAN == value.GetType() )
    {
      ret = value.Get<bool>();
    }
  }
  else
  {
    ret = ::IsBoolean( node );
  }
  return ret;
}

OptionalBoolean Replacement::IsBoolean( OptionalChild child ) const
{
  if( child )
  {
    return IsBoolean( *child );
  }
  else
  {
    return OptionalBoolean();
  }
}

OptionalFloat Replacement::IsFloat( const TreeNode & node ) const
{
  OptionalFloat ret;
  if( OptionalString replace = HasFullReplacement( node ) )
  {
    Property::Value value = GetFullReplacement( *replace );
    if( Property::FLOAT == value.GetType() )
    {
      ret = value.Get<float>();
    }
  }
  else
  {
    ret = ::IsFloat( node );
  }
  return ret;
}

OptionalString Replacement::IsString( const TreeNode& node ) const
{
  OptionalString ret;

  DALI_ASSERT_DEBUG( mOverrideMap && "missing map");
  DALI_ASSERT_DEBUG( mDefaultMap && "missing map");

  if( node.HasSubstitution() && ((*mOverrideMap).Count() || (*mDefaultMap).Count()) )
  {
    if( OptionalString v = ::IsString( node ) )
    {
      Property::Value value;
      if( ResolvePartialReplacement( *v, value, *mOverrideMap, *mDefaultMap ) )
      {
        if( Property::STRING == value.GetType() )
        {
          ret = value.Get<std::string>();
#if defined(DEBUG_ENABLED)
          DALI_SCRIPT_VERBOSE("  Resolved substring replacement for '%s' => '%s'\n", (*v).c_str(), (*ret).c_str());
#endif
        }
      }
      else
      {
        ret = v; // sets the unexpanded. Expansion may occur later in processing with include files
      }
    }
  }
  else
  {
    ret = ::IsString( node );
  }
  return ret;
}

OptionalInteger Replacement::IsInteger( const TreeNode & node ) const
{
  OptionalInteger ret;
  if( OptionalString replace = HasFullReplacement( node ) )
  {
    Property::Value value = GetFullReplacement( *replace );
    if( Property::INTEGER == value.GetType() )
    {
      ret = value.Get<int>();
    }
  }
  else
  {
    ret = ::IsInteger( node );
  }
  return ret;
}

OptionalVector2 Replacement::IsVector2( const TreeNode & node ) const
{
  OptionalVector2 ret;
  if( OptionalString replace = HasFullReplacement( node ) )
  {
    Property::Value value = GetFullReplacement( *replace );
    if( Property::VECTOR2 == value.GetType() )
    {
      ret = value.Get<Vector2>();
    }
  }
  else
  {
    ret = ::IsVector2( node );
  }
  return ret;
}

OptionalVector3 Replacement::IsVector3( const TreeNode & node ) const
{
  OptionalVector3 ret;
  if( OptionalString replace = HasFullReplacement( node ) )
  {
    Property::Value value = GetFullReplacement( *replace );
    if( Property::VECTOR3 == value.GetType() )
    {
      ret = value.Get<Vector3>();
    }
  }
  else
  {
    ret = ::IsVector3( node );
  }
  return ret;
}

OptionalVector4 Replacement::IsVector4( const TreeNode & node ) const
{
  OptionalVector4 ret;
  if( OptionalString replace = HasFullReplacement( node ) )
  {
    Property::Value value = GetFullReplacement( *replace );
    if( Property::VECTOR4 == value.GetType() )
    {
      ret = value.Get<Vector4>();
    }
  }
  else
  {
    ret = ::IsVector4( node );
  }
  return ret;
}

OptionalMatrix Replacement::IsMatrix( const TreeNode & node ) const
{
  OptionalMatrix ret;
  if( OptionalString replace = HasFullReplacement( node ) )
  {
    Property::Value value = GetFullReplacement( *replace );
    if( Property::MATRIX == value.GetType() )
    {
      ret = value.Get<Matrix>();
    }
  }
  else
  {
    ret = ::IsMatrix( node );
  }
  return ret;
}

OptionalMatrix3 Replacement::IsMatrix3( const TreeNode & node ) const
{
  OptionalMatrix3 ret;
  if( OptionalString replace = HasFullReplacement( node ) )
  {
    Property::Value value = GetFullReplacement( *replace );
    if( Property::MATRIX3 == value.GetType() )
    {
      ret = value.Get<Matrix3>();
    }
  }
  else
  {
    ret = ::IsMatrix3( node );
  }
  return ret;
}

OptionalRect Replacement::IsRect( const TreeNode & node ) const
{
  OptionalRect ret;
  if( OptionalString replace = HasFullReplacement( node ) )
  {
    Property::Value value = GetFullReplacement( *replace );
    if( Property::RECTANGLE == value.GetType() )
    {
      ret = value.Get<Rect<int> >();
    }
  }
  else
  {
    ret = ::IsRect( node );
  }
  return ret;
}



OptionalFloat Replacement::IsFloat( OptionalChild child ) const
{
  if( child )
  {
    return IsFloat( *child );
  }
  else
  {
    return OptionalFloat();
  }
}


OptionalString Replacement::IsString( OptionalChild child ) const
{
  if( child )
  {
    return IsString( *child );
  }
  else
  {
    return OptionalString();
  }
}

OptionalInteger Replacement::IsInteger( OptionalChild child ) const
{
  if( child )
  {
    return IsInteger( *child );
  }
  else
  {
    return OptionalInteger();
  }
}

OptionalVector2 Replacement::IsVector2( OptionalChild child ) const
{
  if( child )
  {
    return IsVector2( *child );
  }
  else
  {
    return OptionalVector2();
  }
}

OptionalVector3 Replacement::IsVector3( OptionalChild child ) const
{
  if( child )
  {
    return IsVector3( *child );
  }
  else
  {
    return OptionalVector3();
  }
}

OptionalVector4 Replacement::IsVector4( OptionalChild child ) const
{
  if( child )
  {
    return IsVector4( *child );
  }
  else
  {
    return OptionalVector4();
  }
}

OptionalMatrix Replacement::IsMatrix( OptionalChild child ) const
{
  if( child )
  {
    return IsMatrix( *child );
  }
  else
  {
    return OptionalMatrix();
  }
}

OptionalMatrix3 Replacement::IsMatrix3( OptionalChild child ) const
{
  if( child )
  {
    return IsMatrix3( *child );
  }
  else
  {
    return OptionalMatrix3();
  }
}

OptionalRect Replacement::IsRect( OptionalChild child ) const
{
  if( child )
  {
    return IsRect( *child );
  }
  else
  {
    return OptionalRect();
  }
}

bool Replacement::IsMap( OptionalChild child, Property::Value& out ) const
{
  bool ret = false;

  if( child )
  {
    if( OptionalString replace = HasFullReplacement( *child ) )
    {
      out = GetFullReplacement( *replace );
      if( Property::MAP == out.GetType() )
      {
        ret = true;
      }
    }
  }

  return ret;
}

bool Replacement::IsArray( OptionalChild child, Property::Value& out ) const
{
  bool ret = false;

  if( child )
  {
    if( OptionalString replace = HasFullReplacement( *child ) )
    {
      out = GetFullReplacement( *replace );
      if( Property::ARRAY == out.GetType() )
      {
        ret = true;
      }
    }
  }

  return ret;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

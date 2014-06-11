//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// INTERNAL INCLUDES
#include <dali-toolkit/internal/builder/builder-impl.h>
#include <dali-toolkit/internal/builder/builder-get-is.inl.h>
#include <dali-toolkit/internal/builder/replacement.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/*
 * Set a property value from a tree node.
 * This function guesses the type of the property from the format of the string in the node.
 * This is not always possible and could be surprising.
 * @param node  The node string to convert from
 * @param value The property value to set
 * @return true if the string could be converted.
 */
bool SetPropertyFromNode( const TreeNode& node, Property::Value& value );

/*
 * Set a property value from a tree node as SetPropertyFromNode() above
 * @param node  The node string to convert from
 * @param value The property value to set
 * @param replacement The overriding replacement map (if any)
 * @return true if the string could be converted.
 */
bool SetPropertyFromNode( const TreeNode& node, Property::Value& value,
                          const Replacement& replacement );

/*
 * Set a property value as the given type from a tree node.
 * @param node The node string to convert from
 * @param type The property type to convert to.
 * @param value The property value to set
 * @return true if the string could be converted to the correct type.
 */
bool SetPropertyFromNode( const TreeNode& node, Property::Type type, Property::Value& value );

/*
 * Set a property value as the given type from a tree node as SetPropertyFromNode() above
 * @param node The node string to convert from
 * @param type The property type to convert to.
 * @param value The property value to set
 * @param replacement The overriding replacement map (if any)
 * @return true if the string could be converted to the correct type.
 */
bool SetPropertyFromNode( const TreeNode& node, Property::Type type, Property::Value& value,
                          const Replacement& replacement );


namespace
{

/**
 * Converts a HTML style 'color' hex string ("#FF0000" for bright red) to a Vector4.
 * The Vector4 alpha component will be set to 1.0f
 * @param hexString The HTML style hex string
 * @return a Vector4 containing the new color value
 */
Vector4 HexStringToVector4( const char* s )
{
  unsigned int value(0u);
  std::istringstream( s ) >> std::hex >> value;
  return Vector4( ((value >> 16 ) & 0xff ) / 255.0f,
                  ((value >> 8 ) & 0xff ) / 255.0f,
                  (value & 0xff ) / 255.0f,
                  1.0f );
}

} // anon namespace


/**
 * A property value type can be forced when its unknown by a disambiguation convention in the json
 * ie  "myarray": [1,2,3,4] ; would be a vector but
 *     "myarray": {'type-cast':'array', 'value':[1,2,3,4]} would be an array
 * @param child The node whos string to search for a disambiguated type
 * @param value The value to set
 * @param overrideMap The user overriding constant map
 * @param defaultMap The default map.
 * @return True if child contained a disambiguated string that could be converted.
 */
bool Disambiguated(const TreeNode& child, // ConstantLut& constantLut,
                   Dali::Property::Value& value,
                   const Replacement& replacement )
{
  OptionalString childType = IsString( IsChild(child, "type-cast") );
  OptionalChild childValue = IsChild(child, "value");

  if( childType && childValue && (2 == child.Size()) )
  {
    // this case allows disambiguation but normally the type is guessed
    // 2 == child.count() is an extra check as the user could have a user dictionary/map with
    // type-cast and value keys. If they do then a work around is to add a bogus key to not run this case.
    if(*childType == "boolean")
    {
      return SetPropertyFromNode( *childValue, Dali::Property::BOOLEAN, value, replacement);
    }
    else if(*childType == "float")
    {
      return SetPropertyFromNode( *childValue, Dali::Property::FLOAT, value, replacement);
    }
    else if(*childType == "vector2")
    {
      return SetPropertyFromNode( *childValue, Dali::Property::VECTOR2, value, replacement);
    }
    else if(*childType == "vector3")
    {
      return SetPropertyFromNode( *childValue, Dali::Property::VECTOR3, value, replacement);
    }
    else if(*childType == "vector4")
    {
      return SetPropertyFromNode( *childValue, Dali::Property::VECTOR4, value, replacement);
    }
    else if(*childType == "rotation")
    {
      return SetPropertyFromNode( *childValue, Dali::Property::ROTATION, value, replacement);
    }
    else if(*childType == "rect")
    {
      return SetPropertyFromNode( *childValue, Dali::Property::RECTANGLE, value, replacement);
    }
    else if(*childType == "string")
    {
      return SetPropertyFromNode( *childValue, Dali::Property::STRING, value, replacement);
    }
    else if(*childType == "map")
    {
      return SetPropertyFromNode( *childValue, Dali::Property::MAP, value, replacement);
    }
    else if(*childType == "array")
    {
      return SetPropertyFromNode( *childValue, Dali::Property::ARRAY, value, replacement);
    }
  }

  // else we failed to disambiguate
  return false;
}


bool SetPropertyFromNode( const TreeNode& node, Property::Type type, Property::Value& value)
{
  Replacement noReplacement;
  return SetPropertyFromNode( node, type, value, noReplacement );
}

bool SetPropertyFromNode( const TreeNode& node, Property::Type type, Property::Value& value,
                          const Replacement& replacer )
{
  bool done = false;

  switch(type)
  {
    case Property::BOOLEAN:
    {
      if( OptionalBoolean v = replacer.IsBoolean(node) )
      {
        value = *v;
        done = true;
      }
      break;
    }
    case Property::FLOAT:
    {
      if( OptionalFloat v = replacer.IsFloat(node) )
      {
        value = *v;
        done = true;
      }
      break;
    }
    case Property::INTEGER:
    {
      if( OptionalInteger v = replacer.IsInteger(node) )
      {
        value = *v;
        done = true;
      }
      break;
    }
    case Property::UNSIGNED_INTEGER:
    {
      if( OptionalInteger v = replacer.IsInteger(node) )
      {
        if( *v >= 0 ) // with a loss of resolution....
        {
          value = *v;
          done = true;
        }
      }
      break;
    }
    case Property::VECTOR2:
    {
      if( OptionalVector2 v = replacer.IsVector2(node) )
      {
        value = *v;
        done = true;
      }
      break;
    }
    case Property::VECTOR3:
    {
      if( OptionalVector3 v = replacer.IsVector3(node) )
      {
        value = *v;
        done = true;
      }
      break;
    }
    case Property::VECTOR4:
    {
      if( OptionalVector4 v = replacer.IsVector4(node) )
      {
        value = *v;
        done = true;
      }
      else if( OptionalString s = replacer.IsString(node) )
      {
        if( (*s)[0] == '#' && 7 == (*s).size() )
        {
          value = HexStringToVector4( &(*s)[1] );
          done = true;
        }
      }
      else if( TreeNode::OBJECT == node.GetType() )
      {
        // check for "r", "g" and "b" child color component nodes
        OptionalInteger r = replacer.IsInteger( IsChild(node, "r") );
        OptionalInteger g = replacer.IsInteger( IsChild(node, "g") );
        OptionalInteger b = replacer.IsInteger( IsChild(node, "b") );
        if( r && g && b )
        {
          float red( (*r) * (1.0f/255.0f) );
          float green( (*g) * (1.0f/255.0f) );
          float blue( (*b) * (1.0f/255.0f) );
          // check for optional "a" (alpha) node, default to fully opaque if it is not found.
          float alpha( 1.0f );
          OptionalInteger a = replacer.IsInteger( IsChild(node, "a") );
          if( a )
          {
            alpha = (*a) * (1.0f/255.0f);
          }
          value = Vector4( red, green, blue, alpha );
          done = true;
        }
      }
      break;
    }
    case Property::MATRIX3:
    {
      if( OptionalMatrix3 v = replacer.IsMatrix3(node) )
      {
        value = *v;
        done = true;
      }
      break;
    }
    case Property::MATRIX:
    {
      if( OptionalMatrix v = replacer.IsMatrix(node) )
      {
        value = *v;
        done = true;
      }
      break;
    }
    case Property::RECTANGLE:
    {
      if( OptionalRect v = replacer.IsRect(node) )
      {
        value = *v;
        done = true;
      }
      break;
    }
    case Property::ROTATION:
    {
      if(4 == node.Size())
      {
        if( OptionalVector4 ov = replacer.IsVector4(node) )
        {
          const Vector4& v = *ov;
          // angle, axis as per spec
          value = Quaternion(Radian(Degree(v[3])),
                             Vector3(v[0],v[1],v[2]));
          done = true;
        }
      }
      else
      {
        // degrees Euler as per spec
        if( OptionalVector3 v = replacer.IsVector3(node) )
        {
          value = Quaternion(Radian(Degree((*v).x)),
                             Radian(Degree((*v).y)),
                             Radian(Degree((*v).z)));
          done = true;
        }
      }
      break;
    }
    case Property::STRING:
    {
      if( OptionalString v = replacer.IsString(node) )
      {
        value = *v;
        done = true;
      }
      break;
    }
    case Property::ARRAY:
    {
      if( replacer.IsArray( node, value ) )
      {
        done = true;
      }
      else if(node.Size())
      {
        value = Property::Value(Property::ARRAY);
        unsigned int i = 0;
        TreeNode::ConstIterator iter(node.CBegin());
        for( ; i < node.Size(); ++i, ++iter)
        {
          Property::Value v;
          if( SetPropertyFromNode( (*iter).second, v, replacer ) )
          {
            value.AppendItem(v);
          }
        }

        if( value.GetSize() == static_cast<int>(node.Size()) )
        {
          done = true;
        }
        else
        {
          done = false;
        }
      }
      break;
    }
    case Property::MAP:
    {
      if( replacer.IsMap( node, value ) )
      {
        done = true;
      }
      else if(node.Size())
      {
        value = Property::Value(Property::MAP);
        unsigned int i = 0;
        TreeNode::ConstIterator iter(node.CBegin());
        for( ; i < node.Size(); ++i, ++iter)
        {
          Property::Value v;
          if( SetPropertyFromNode( (*iter).second, v, replacer ) )
          {
            value.SetValue( (*iter).first, v );
          }
        }

        if( value.GetSize() == static_cast<int>(node.Size()) )
        {
          done = true;
        }
        else
        {
          done = false;
        }
      }
      break;
    }
    case Property::TYPE_COUNT:
    case Property::NONE:
    {
      break;
    }
  } // switch type

  return done;
}

bool SetPropertyFromNode( const TreeNode& node, Property::Value& value )

{
  Replacement replacer;
  return SetPropertyFromNode( node, value, replacer );
}

bool SetPropertyFromNode( const TreeNode& node, Property::Value& value,
                          const Replacement& replacer )
{
  bool done = false;

  // some values are ambiguous as we have no Property::Type but can be disambiguated in the json

  // Currently Rotations and Rectangle must always be disambiguated when a type isnt available
  if( Disambiguated( node, value, replacer ) )
  {
    done = true;
  }
  else
  {
    if( node.Size() )
    {
      // our current heuristic for deciding an array is actually a vector and not say a map
      // is to check if the values are all floats
      bool allNumbers = true;
      for(TreeConstIter iter = node.CBegin(); iter != node.CEnd(); ++iter)
      {
        OptionalFloat f = IsFloat((*iter).second);
        if(!f)
        {
          allNumbers = false;
          break;
        }
      }

      if( allNumbers )
      {
        // prefer finding vectors over presuming composite Property::Array...
        if( OptionalMatrix v = IsMatrix(node) )
        {
          value = *v;
          done = true;
        }
        else if( OptionalMatrix3 v = IsMatrix3(node) )
        {
          value = *v;
          done = true;
        }
        else if( OptionalVector4 v = IsVector4(node) )
        {
          value = *v;
          done = true;
        }
        else if( OptionalVector3 v = IsVector3(node) )
        {
          value = *v;
          done = true;
        }
        else if( OptionalVector2 v = IsVector2(node) )
        {
          value = *v;
          done = true;
        }
        else if( 4 == node.Size() )
        {
          if( OptionalVector4 v = IsVector4(node) )
          {
            value = *v;
            done = true;
          }
        }
        else
        {
          value = Property::Value(Property::ARRAY);
          Property::Value v;

          for(TreeConstIter iter = node.CBegin(); iter != node.CEnd(); ++iter)
          {
            if( SetPropertyFromNode( (*iter).second, v, replacer ) )
            {
              value.AppendItem(v);
              done = true;
            }
          }
        }
      }

      if(!done)
      {
        // presume an array or map
        // container of size 1
        TreeNode::ConstIterator iter = node.CBegin();

        // its seems legal with current json parser for a map to have an empty key
        // but here we take that to mean the structure is a list
        if( ((*iter).first) == 0 )
        {
          value = Property::Value(Property::ARRAY);
          Property::Value v;
          for(unsigned int i = 0; i < node.Size(); ++i, ++iter)
          {
            if( SetPropertyFromNode( (*iter).second, v, replacer ) )
            {
              value.AppendItem(v);
              done = true;
            }
          }
        }
        else
        {
          value = Property::Value(Property::MAP);
          Property::Value v;
          for(unsigned int i = 0; i < node.Size(); ++i, ++iter)
          {
            if( SetPropertyFromNode( (*iter).second, v, replacer ) )
            {
              value.SetValue((*iter).first, v);
              done = true;
            }
          }
        }
      } // if!done
    } // if node.size()
    else // if( 0 == node.size() )
    {
      // no children so either one of bool, float, integer, string
      OptionalBoolean aBool    = replacer.IsBoolean(node);
      OptionalInteger anInt    = replacer.IsInteger(node);
      OptionalFloat   aFloat   = replacer.IsFloat(node);
      OptionalString  aString  = replacer.IsString(node);

      if(aBool)
      {
        // a bool is also an int but here we presume int
        if(anInt)
        {
          value = *anInt;
          done = true;
        }
        else
        {
          value = *aBool;
          done = true;
        }
      }
      else
      {
        // Note: These are both floats and strings
        // {"value":"123"}
        // {"value":123}
        // This means we can't have a string with purely numeric content without disambiguation.
        if(aFloat)
        {
          value = *aFloat;
          done = true;
        }
        else if(anInt)
        {
          value = *anInt;
          done = true;
        }
        else
        {
          // string always succeeds with the current json parser so its last
          value = *aString;
          done = true;
        }

      } // if aBool

    } // if( node.size() )

  } // if Disambiguated()

  return done;
} // bool SetPropertyFromNode( const TreeNode& node, Property::Value& value )


} // namespace Internal

} // namespace Toolkit

} // namespace Dali

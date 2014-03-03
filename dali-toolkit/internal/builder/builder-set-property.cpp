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
 * Set a property value as the given type from a tree node.
 * @param node The node string to convert from
 * @param type The property type to convert to.
 * @param value The property value to set
 * @return true if the string could be converted to the correct type.
 */
bool SetPropertyFromNode( const TreeNode& node, Property::Type type, Property::Value& value );

/**
 * A property value type can be forced when its unknown by a disambiguation convention in the json
 * ie  "myarray": [1,2,3,4] ; would be a vector but
 *     "myarray": {'type-cast':'array', 'value':[1,2,3,4]} would be an array
 * @param child The node whos string to search for a disambiguated type
 * @param value The value to set
 * @return True if child contained a disambiguated string that could be converted.
 */
bool Disambiguated(const TreeNode& child, // ConstantLut& constantLut,
                   Dali::Property::Value& value)
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
      return SetPropertyFromNode( *childValue, Dali::Property::BOOLEAN, value);
    }
    else if(*childType == "float")
    {
      return SetPropertyFromNode( *childValue, Dali::Property::FLOAT, value);
    }
    else if(*childType == "vector2")
    {
      return SetPropertyFromNode( *childValue, Dali::Property::VECTOR2, value);
    }
    else if(*childType == "vector3")
    {
      return SetPropertyFromNode( *childValue, Dali::Property::VECTOR3, value);
    }
    else if(*childType == "vector4")
    {
      return SetPropertyFromNode( *childValue, Dali::Property::VECTOR4, value);
    }
    else if(*childType == "rotation")
    {
      return SetPropertyFromNode( *childValue, Dali::Property::ROTATION, value);
    }
    else if(*childType == "rect")
    {
      return SetPropertyFromNode( *childValue, Dali::Property::RECTANGLE, value);
    }
    else if(*childType == "string")
    {
      return SetPropertyFromNode( *childValue, Dali::Property::STRING, value);
    }
    else if(*childType == "map")
    {
      return SetPropertyFromNode( *childValue, Dali::Property::MAP, value);
    }
    else if(*childType == "array")
    {
      return SetPropertyFromNode( *childValue, Dali::Property::ARRAY, value);
    }
  }

  // else we failed to disambiguate
  return false;
}

bool SetPropertyFromNode( const TreeNode& node, Property::Type type, Property::Value& value )
{
  bool done = false;

  switch(type)
  {
    case Property::BOOLEAN:
    {
      if( OptionalBoolean v = IsBoolean(node) )
      {
        value = *v;
        done = true;
      }
      break;
    }
    case Property::FLOAT:
    {
      if( OptionalFloat v = IsFloat(node) )
      {
        value = *v;
        done = true;
      }
      break;
    }
    case Property::INTEGER:
    {
      if( OptionalInteger v = IsInteger(node) )
      {
        value = *v;
        done = true;
      }
      break;
    }
    case Property::UNSIGNED_INTEGER:
    {
      if( OptionalInteger v = IsInteger(node) )
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
      if( OptionalVector2 v = IsVector2(node) )
      {
        value = *v;
        done = true;
      }
      break;
    }
    case Property::VECTOR3:
    {
      if( OptionalVector3 v = IsVector3(node) )
      {
        value = *v;
        done = true;
      }
      break;
    }
    case Property::VECTOR4:
    {
      if( OptionalVector4 v = IsVector4(node) )
      {
        value = *v;
        done = true;
       }
      break;
    }
    case Property::MATRIX3:
    {
      if( OptionalMatrix3 v = IsMatrix3(node) )
      {
        value = *v;
        done = true;
      }
      break;
    }
    case Property::MATRIX:
    {
      if( OptionalMatrix v = IsMatrix(node) )
      {
        value = *v;
        done = true;
      }
      break;
    }
    case Property::RECTANGLE:
    {
      if( OptionalRect v = IsRect(node) )
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
        if( OptionalVector4 ov = IsVector4(node) )
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
        if( OptionalVector3 v = IsVector3(node) )
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
      if( OptionalString v = IsString(node) )
      {
        value = *v;
        done = true;
      }
      break;
    }
    case Property::ARRAY:
    {
      if(node.Size())
      {
        value = Property::Value(Property::ARRAY);
        unsigned int i = 0;
        TreeNode::ConstIterator iter(node.CBegin());
        for( ; i < node.Size(); ++i, ++iter)
        {
          Property::Value v;
          if( SetPropertyFromNode( (*iter).second, v) )
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
      if(node.Size())
      {
        value = Property::Value(Property::MAP);
        unsigned int i = 0;
        TreeNode::ConstIterator iter(node.CBegin());
        for( ; i < node.Size(); ++i, ++iter)
        {
          Property::Value v;
          if( SetPropertyFromNode( (*iter).second, v) )
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
  }

  return done;
}


bool SetPropertyFromNode( const TreeNode& node, Property::Value& value )
{
  bool done = false;

  // some values are ambiguous as we have no Property::Type but can be disambiguated in the json

  // Currently Rotations and Rectangle must always be disambiguated when a type isnt available
  if( Disambiguated( node, value ) )
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
        if( OptionalVector4 v = IsVector4(node) )
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
            if( SetPropertyFromNode( (*iter).second, v) )
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
            if( SetPropertyFromNode( (*iter).second, v) )
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
            if( SetPropertyFromNode( (*iter).second, v) )
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
      OptionalBoolean aBool    = IsBoolean(node);
      OptionalInteger anInt    = IsInteger(node);
      OptionalFloat   aFloat   = IsFloat(node);
      OptionalString  aString  = IsString(node);

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

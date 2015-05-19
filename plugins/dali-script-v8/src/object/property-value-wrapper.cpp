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
#include "property-value-wrapper.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/property-array.h>

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <dali-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

namespace
{

struct WrapTypeInfo
{
  Dali::Property::Type propertyType;
  BaseWrappedObject::Type wrappedType;
};
/**
 * lookup table to convert between dali property types and wrapped types
 * Wrapped types includes actors / timers / render tasks, hence we can't just
 * use the dali property types to encode what a wrapped pointer contains
 */
const WrapTypeInfo WrapTypeInfoLookup[]=
{
  { Dali::Property::VECTOR2,    BaseWrappedObject::VECTOR2 },
  { Dali::Property::VECTOR3,    BaseWrappedObject::VECTOR3 },
  { Dali::Property::VECTOR4,    BaseWrappedObject::VECTOR4 },
  { Dali::Property::RECTANGLE,  BaseWrappedObject::RECTANGLE  },
  { Dali::Property::ROTATION,   BaseWrappedObject::ROTATION   },
  { Dali::Property::MATRIX,     BaseWrappedObject::MATRIX     },
  { Dali::Property::MATRIX3,    BaseWrappedObject::MATRIX3    },


};
const unsigned int WrapTypeLookupCount = sizeof( WrapTypeInfoLookup ) / sizeof( WrapTypeInfo );

BaseWrappedObject::Type GetWrappedType( Dali::Property::Type type )
{
  for( unsigned int i = 0; i < WrapTypeLookupCount; ++i)
  {
    if( WrapTypeInfoLookup[ i ].propertyType == type )
    {
      return WrapTypeInfoLookup[ i ].wrappedType;
    }
  }
  DALI_LOG_ERROR("GetWrappedType failed \n");
  return BaseWrappedObject::UNKNOWN;
}

// Templates for setting members on propertys, like, x,y,z on a vector
template <typename T, typename S>
void SetX( Dali::Property::Value &value, S& v)
{
  // to set a property value, we need to read into a temporary
  // then assign it back.
  // E.g.   Vector2 temp = value.Get<Vector2>();
  //        temp.x = 10;
  //        value = temp;
  T tmp( value.Get<T>() );
  tmp.x = v;
  value = tmp;
}

template <typename T, typename S>
void SetY( Dali::Property::Value &value, S& v)
{
  T tmp = value.Get<T>();
  tmp.y = v;
  value = tmp;
}

template <typename T, typename S>
void SetZ( Dali::Property::Value &value, S& v)
{
  T tmp = value.Get<T>();
  tmp.z = v;
  value = tmp;
}

template <typename T, typename S>
void SetW( Dali::Property::Value &value, S& v)
{
  T tmp = value.Get<T>();
  tmp.w = v;
  value = tmp;
}
template <typename T, typename S>
void SetWidth( Dali::Property::Value &value, S& v)
{
  T tmp = value.Get<T>();
  tmp.width = v;
  value = tmp;
}

template <typename T, typename S>
void SetHeight( Dali::Property::Value &value, S& v)
{
  T tmp = value.Get<T>();
  tmp.height = v;
  value = tmp;
}

v8::Handle< v8::Value > GetV8ValueFrom( v8::Isolate* isolate,
                                        const Dali::Property::Value& value,
                                        const std::string& field)
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::Value> ret;

  Dali::Property::Type type = value.GetType();
  std::string objectType="Unknown type";

  // simple property values are handled by the actor wrapper so we just handle property values with sub fields
  switch(type)
  {
    case Dali::Property::VECTOR2:
    {
      objectType = "Vector2";

      if( field == "x" || field == "width" )
      {
        Dali::Vector2 v2 = value.Get<Dali::Vector2>();
        ret = v8::Number::New( isolate, v2.x);
      }
      else if(field == "y" || field == "height")
      {
        Dali::Vector2 v2 = value.Get<Dali::Vector2>();
        ret = v8::Number::New( isolate, v2.y);
      }

      break;
    }
    case Dali::Property::VECTOR3:
    {
      objectType = "Vector3";

      if(field == "x" || field == "width" || field == "r")
      {
        Dali::Vector3 v3 = value.Get<Dali::Vector3>();
        ret = v8::Number::New( isolate, v3.x);
      }
      else if(field == "y" || field == "height" || field == "g")
      {
        Dali::Vector3 v3 = value.Get<Dali::Vector3>();
        ret = v8::Number::New( isolate, v3.y);
      }
      else if(field == "z" || field == "depth" || field == "b")
      {
        Dali::Vector3 v3 = value.Get<Dali::Vector3>();
        ret = v8::Number::New( isolate, v3.z);
      }
      break;
    }
    case Dali::Property::VECTOR4:
    {
      objectType = "vector4";

      if(field == "x" || field == "s" || field == "r")
      {
        Dali::Vector4 v4 = value.Get<Dali::Vector4>();
        ret = v8::Number::New( isolate, v4.x);
      }
      else if(field == "y" || field == "g" || field == "t")
      {
        Dali::Vector4 v4 = value.Get<Dali::Vector4>();
        ret = v8::Number::New( isolate, v4.y);
      }
      else if(field == "z" || field == "r" || field == "b")
      {
        Dali::Vector4 v4 = value.Get<Dali::Vector4>();
        ret = v8::Number::New( isolate, v4.z);
      }
      else if(field == "w" || field == "a" || field == "q")
      {
        Dali::Vector4 v4 = value.Get<Dali::Vector4>();
        ret = v8::Number::New( isolate, v4.w);
      }

      break;
    }
    case Dali::Property::ROTATION:
    {
      objectType = "rotation";

      if(field == "roll")
      {
        Dali::Vector4 v4 = value.Get<Dali::Quaternion>().EulerAngles();
        ret = v8::Number::New( isolate, v4.z);
      }
      else if(field == "pitch")
      {
        Dali::Vector4 v4 = value.Get<Dali::Quaternion>().EulerAngles();
        ret = v8::Number::New( isolate, v4.x);
      }
      else if(field == "yaw")
      {
        Dali::Vector4 v4 = value.Get<Dali::Quaternion>().EulerAngles();
        ret = v8::Number::New( isolate, v4.y);
      }

       else if(field == "axis")
      {
        Dali::Vector3 axis;
        Radian angle;
        value.Get<Dali::Quaternion>().ToAxisAngle(axis, angle);
        ret = PropertyValueWrapper::WrapDaliProperty( isolate , Dali::Property::Value( axis ) );
      }
      else if(field == "angle")
      {
        Dali::Vector3 axis;
        Radian angle;
        value.Get<Dali::Quaternion>().ToAxisAngle(axis, angle);
        ret = PropertyValueWrapper::WrapDaliProperty( isolate , Dali::Property::Value( angle ) );
      }
      break;
    }
    case Dali::Property::RECTANGLE:
    {
      objectType = "rectangle";

      if(field == "x")
      {
        Dali::Rect<int> r = value.Get<Dali::Rect<int> >();
        ret = v8::Number::New( isolate, r.x);
      }
      else if(field == "y")
      {
        Dali::Rect<int> r = value.Get<Dali::Rect<int> >();
        ret = v8::Number::New( isolate, r.y);
      }
      else if(field == "width")
      {
        Dali::Rect<int> r = value.Get<Dali::Rect<int> >();
        ret = v8::Number::New( isolate, r.width);
      }
      else if(field == "height")
      {
        Dali::Rect<int> r = value.Get<Dali::Rect<int> >();
        ret = v8::Number::New( isolate, r.height);
      }
      break;
    }

    default:
    {
      // will trigger a type error as ret.IsEmpty() == true
      break;
    }

  } // switch type

  if ( ret.IsEmpty() )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "Cannot get property with field " + objectType+ ":" + field );
  }
  return handleScope.Escape(ret);
}


/*
 * Setting a field on a property,  e.g.  Position.x = 5 or Rect.Width = 100
 * The only property field that supports a struct is  rotation.axis
 */
void SetFromV8Value(v8::Isolate* isolate,
                     Dali::Property::Value& value,
                     const std::string& field,
                     v8::Local<v8::Value> v8Value )
{
  v8::EscapableHandleScope handleScope( isolate );

  Dali::Property::Type type = value.GetType();

  float asFloat = 0.f;
  int asInt = 0;

  // all are numbers at the moment
  if( v8Value->IsNumber() )
  {
    asFloat = static_cast<float>(v8Value->NumberValue());
    asInt   = static_cast<int>(asFloat);
  }
  else
  {
    // if the v8 value isn't a number, then then only Dali object we support is a Vector3 part of a rotation
    if( ! (type == Dali::Property::ROTATION && field == "axis") )
    {
      DALI_SCRIPT_EXCEPTION(isolate,std::string("Cannot set property with field (value is not a number):") + field);
      return;
    }
  }

  // simple property values are handled by the actor wrapper so we just handle property values with sub fields
  switch(type)
  {
    case Dali::Property::VECTOR2:
    {
      if(field == "x" || field == "width")
      {
        SetX<Dali::Vector2>(value, asFloat);
      }
      else if(field == "y" || field == "height")
      {
        SetY<Dali::Vector2>(value, asFloat);
      }
      else
      {
        DALI_SCRIPT_EXCEPTION(isolate, std::string("Cannot set property with field (unknown type):") + field);
        return;
      }
      break;
    }
    case Dali::Property::VECTOR3:
    {
      if(field == "x" || field == "width" || field == "r")
      {
        SetX<Dali::Vector3>(value, asFloat);
      }
      else if(field == "y" || field == "height" || field == "g")
      {
        SetY<Dali::Vector3>(value, asFloat);
      }
      else if(field == "z" || field == "depth" || field == "b")
      {
        SetZ<Dali::Vector3>(value, asFloat);
      }
      else
      {
        DALI_SCRIPT_EXCEPTION(isolate, std::string("Cannot set property with field (unknown type):") + field);
        return;
      }
      break;
    }
    case Dali::Property::VECTOR4:
    {
      if(field == "x" || field == "s" || field == "r")
      {
        SetX<Dali::Vector4>(value, asFloat);
      }
      else if(field == "y" || field == "g" || field == "t")
      {
        SetY<Dali::Vector4>(value, asFloat);
      }
      else if(field == "z" || field == "r" || field == "b")
      {
        SetZ<Dali::Vector4>(value, asFloat);
      }
      else if(field == "w" || field == "a" || field == "q")
      {
        SetW<Dali::Vector4>(value, asFloat);
      }
      else
      {
        DALI_SCRIPT_EXCEPTION( isolate, std::string("Cannot set property with field (unknown type):") + field);
        return;
      }
      break;
    }
    case Dali::Property::ROTATION:
    {
      if(field == "roll")
      {
        Dali::Vector4 v4 = value.Get<Dali::Quaternion>().EulerAngles();
        v4.z = asFloat;
        value = Dali::Quaternion( Radian(Degree(v4.x)), Radian(Degree(v4.y)), Radian(Degree(v4.z)) );
      }
      else if(field == "pitch")
      {
        Dali::Vector4 v4 = value.Get<Dali::Quaternion>().EulerAngles();
        v4.x = asFloat;
        value = Dali::Quaternion( Radian(Degree(v4.x)), Radian(Degree(v4.y)), Radian(Degree(v4.z)) );
      }
      else if(field == "yaw")
      {
        Dali::Vector4 v4 = value.Get<Dali::Quaternion>().EulerAngles();
        v4.y = asFloat;
        value = Dali::Quaternion( Radian(Degree(v4.x)), Radian(Degree(v4.y)), Radian(Degree(v4.z)) );
      }
      else if(field == "axis")
      {
        Dali::Vector3 axis;
        Radian angle;
        value.Get<Dali::Quaternion>().ToAxisAngle(axis, angle);

        if( v8Value->IsObject() )
        {
          v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast( v8Value );

          PropertyValueWrapper* wrapper = PropertyValueWrapper::Unwrap( isolate, object );
          Dali::Property::Value v3 = wrapper->GetValue();

          if(v3.GetType() == Dali::Property::VECTOR3)
          {
            value = Dali::Quaternion(angle, v3.Get<Dali::Vector3>());
          }
          else
          {
            DALI_SCRIPT_EXCEPTION( isolate, "Axis expects a Vector3:");
            return;
          }

        }
      }
      else if(field == "angle")
      {
        Dali::Vector3 axis;
        Radian angle;
        value.Get<Dali::Quaternion>().ToAxisAngle( axis, angle );
        value = Dali::Quaternion( Degree( asFloat ), axis );
      }
      else
      {
        DALI_SCRIPT_EXCEPTION(  isolate, std::string("Cannot set property with field (unknown type):") + field);
        return;
      }
      break;
    }
    case Dali::Property::RECTANGLE:
    {
      if(field == "x")
      {
        SetX<Dali::Rect<int> >(value, asInt);
      }
      else if(field == "y")
      {
        SetY<Dali::Rect<int> >(value, asInt);
      }
      else if(field == "width")
      {
        SetWidth<Dali::Rect<int> >(value, asInt);
      }
      else if(field == "height")
      {
        SetHeight<Dali::Rect<int> >(value, asInt);
      }
      else
      {
        DALI_SCRIPT_EXCEPTION( isolate, std::string("Cannot set property with field (unknown type):") + field);
        return;
      }
      break;
    }
    default:
    {
      DALI_SCRIPT_EXCEPTION( isolate, std::string("Cannot set property with field (unknown type):") + field);
      return;
      break;
    }

  } // switch type

} // SetFromV8Value


bool IsPrimitive( const Dali::Property::Value &value )
{
  switch( value.GetType() )
  {
    case Dali::Property::BOOLEAN:
    case Dali::Property::INTEGER:
    case Dali::Property::UNSIGNED_INTEGER:
    case Dali::Property::STRING:
    case Dali::Property::FLOAT:
    {
      return true;
    }
    default:
    {
      return false;
    }
  }
}
v8::Local<v8::Object> CreateJavaScriptPrimitive( v8::Isolate* isolate, const Dali::Property::Value &value )
{
  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::Value> v8Value;

  switch( value.GetType() )
   {
     case Dali::Property::FLOAT:
     {
       v8Value = v8::Number::New( isolate, value.Get<float>()  );
       break;
     }
     case Dali::Property::BOOLEAN:
     {
       v8Value = v8::Boolean::New(  isolate, value.Get<bool>());
       break;
     }
     case Dali::Property::INTEGER:
     {
       v8Value = v8::Integer::New( isolate, value.Get<int>());
       break;
     }
     case Dali::Property::UNSIGNED_INTEGER:
     {
       v8Value = v8::Integer::New( isolate, value.Get<unsigned int>());
       break;
     }
     case Dali::Property::STRING:
     {
       std::string string = value.Get< std::string >();
       v8Value = v8::String::NewFromUtf8( isolate,  string.c_str());
       break;
     }
     default:
     {
       DALI_LOG_ERROR("Primitive mismatch \n");
       break;
     }
   }
 v8::Local<v8::Object> ret = v8Value->ToObject();

 return handleScope.Escape( ret );
}


} // un-named namespace



Dali::Property::Value PropertyValueWrapper::VectorOrMatrixFromV8Array( v8::Isolate* isolate, const v8::Local<v8::Value>& v8Value)
{
  v8::EscapableHandleScope handleScope( isolate );

  Dali::Property::Value ret;

  if( !v8Value->IsArray() )
  {
    return ret;
  }
  v8::Local<v8::Array> array = v8::Local<v8::Array>::Cast( v8Value );//v8::Array::Cast(  *v8Value);
  uint32_t len = array->Length();
  float out[16] = {0.f, 0.f, 0.f, 0.f,0.f, 0.f, 0.f, 0.f,0.f, 0.f, 0.f, 0.f,0.f, 0.f, 0.f, 0.f};
  const unsigned int size = sizeof(out) / sizeof(float);
  for( uint32_t i=0; i < (len > size? size:len); ++i)
  {
    v8::Handle<v8::Value> entry = array->Get( i);// v8::Integer::New( isolate, i));
    if( V8Utils::IsNumberPrimitiveOrObject( entry ) )
    {
      out[i] = V8Utils::GetNumberValue( isolate, entry );
      //printf("entry is a number %d , entry is a int %f \n",(int)i , out[i]  );
    }
  }
  if(16 == len )
  {
    ret = Dali::Matrix( out[0] );
  }
  else if ( 9 == len )
  {
    ret = Dali::Matrix3( out[0],out[1],out[2],out[3],out[4],out[5],out[6],out[7],out[8] );
  }
  else if(4 == len)
  {
    ret = Dali::Vector4(out[0], out[1], out[2], out[3]);
  }
  else if(3 == len)
  {
    ret = Dali::Vector3(out[0], out[1], out[2]);
  }
  else if(2 == len)
  {
    ret = Dali::Vector2(out[0], out[1]);
  }
  else if(1 == len)
  {
    ret = out[0];
  }

  return ret;
}

Dali::Property::Value PropertyValueWrapper::ArrayFromV8Array( v8::Isolate* isolate, const v8::Local<v8::Value>& v8Value)
{
  v8::EscapableHandleScope handleScope( isolate );

  if( !v8Value->IsArray() )
  {
    return Dali::Property::Value();
  }

  //Cast v8::Value to v8::Array
  v8::Local<v8::Array> v8Array = v8::Local<v8::Array>::Cast( v8Value );
  uint32_t elementCount = v8Array->Length();
  Dali::Property::Array data;
  data.Reserve( elementCount );
  bool bFound(false);
  for( uint32_t i(0); i<elementCount; ++i )
  {
    //Get Property::Value for each element in the array
    data.PushBack( V8Utils::GetPropertyValueFromObject( bFound, isolate, v8Array->Get(i) ) );
  }

  //return the Property::Value
  return Dali::Property::Value( data );
}

v8::Persistent<v8::ObjectTemplate> PropertyValueWrapper::mTemplatePrimitive;


PropertyValueWrapper::~PropertyValueWrapper()
{

}

PropertyValueWrapper::PropertyValueWrapper( const Dali::Property::Value& value, GarbageCollectorInterface& gc )
  : BaseWrappedObject(  GetWrappedType( value.GetType() ), gc ),
    mValue(value)
{
}

Dali::Property::Value PropertyValueWrapper::ExtractPropertyValue( v8::Isolate* isolate, v8::Local< v8::Value> v8Value, Dali::Property::Type type)
{
  v8::HandleScope handleScope( isolate);

  Dali::Property::Value daliPropertyValue;

  // check if it's a dali object (vector, rect etc)
  if( v8Value->IsObject() )
  {
    v8::Local<v8::Object> object = v8::Handle<v8::Object>::Cast(v8Value);
    if( object->InternalFieldCount() == BaseWrappedObject::FIELD_COUNT )
    {
      if( BaseWrappedObject::IsWrappedType( isolate, object, GetWrappedType(type) ) )
      {
        PropertyValueWrapper* propertyWrapper = Unwrap( isolate, object );
        return propertyWrapper->GetValue();
      }
    }
  }

  // check if it's a javascript Array
  Dali::Property::Value array = VectorOrMatrixFromV8Array( isolate, v8Value );

  switch(type)
  {
    case Dali::Property::BOOLEAN:
    {
      if( V8Utils::IsBooleanPrimitiveOrObject( v8Value ) )
      {
        daliPropertyValue = Dali::Property::Value( V8Utils::GetBooleanValue( isolate, v8Value));
      }
      break;
    }
    case Dali::Property::FLOAT:
    {
      if( V8Utils::IsNumberPrimitiveOrObject( v8Value )  )
      {
        daliPropertyValue = Dali::Property::Value( V8Utils::GetNumberValue( isolate, v8Value) );
      }

      break;
    }
    case Dali::Property::INTEGER: // todo is it wise to distinguish int from float in javascript?!
    {
      if( v8Value->IsInt32() )
      {
        daliPropertyValue = Dali::Property::Value(  v8Value->Int32Value()  ) ;//static_cast<int>( V8Utils::GetNumberValue( isolate, v8Value) ));
      }
      break;
    }
    case Dali::Property::UNSIGNED_INTEGER:
    {
      if( v8Value->IsUint32() )
      {
        daliPropertyValue = Dali::Property::Value(  v8Value->Uint32Value() );//static_cast<unsigned int>( V8Utils::GetNumberValue( isolate, v8Value) ));
      }
      break;
    }
    case Dali::Property::STRING:
    {
      if( V8Utils::IsStringPrimitiveOrObject( v8Value) )
      {
        daliPropertyValue = Dali::Property::Value( V8Utils::GetStringValue( isolate, v8Value) );
      }


      break;
    }
    case Dali::Property::VECTOR2:
    {
      if( array.GetType() == Dali::Property::VECTOR2 )
      {
          daliPropertyValue = array;
      }
      break;
    }
    case Dali::Property::VECTOR3:
    {
      if( array.GetType() == Dali::Property::VECTOR3)
      {
        daliPropertyValue = array;
      }
      break;
    }
    case Dali::Property::VECTOR4:
    {

      if( array.GetType() == Dali::Property::VECTOR4)
      {
        daliPropertyValue = array;
      }
      break;
    }
    case Dali::Property::RECTANGLE:
    {
      if( array.GetType() == Dali::Property::VECTOR4)
      {
        Vector4 vec4( array.Get<Vector4>());
        daliPropertyValue = Property::Value(Rect<int>( vec4.x,vec4.y, vec4.z, vec4.w ));
      }
      break;
    }
    case Dali::Property::ROTATION:
    {
      if( array.GetType() == Dali::Property::VECTOR4)
      {
        // v4 treated as axis angle
        Dali::Vector4 v = array.Get<Dali::Vector4>();
        daliPropertyValue = Dali::AngleAxis(Dali::Degree(v[0]), Dali::Vector3(v[1], v[2], v[3]));
      }
      else if(array.GetType() == Dali::Property::VECTOR3)
      {
        // v3 treated as euler in degrees
        Dali::Vector3 v = array.Get<Dali::Vector3>();
        daliPropertyValue = Dali::Quaternion( Radian(Degree(v[0])), Radian(Degree(v[1])), Radian(Degree(v[2])) );
      }
      break;
    }
    case Dali::Property::MATRIX:
    {
      Dali::Matrix mat = array.Get<Dali::Matrix>();
      daliPropertyValue = mat;
      break;
    }
    case Dali::Property::MATRIX3:
    {
      Dali::Matrix3 mat = array.Get<Dali::Matrix3>();
      daliPropertyValue = mat;
      break;
    }
    case Dali::Property::ARRAY:
    {
      daliPropertyValue = ArrayFromV8Array( isolate, v8Value );
      break;
    }
    case Dali::Property::NONE:
    case Dali::Property::TYPE_COUNT:
    default:
    {
      break;
    }
   } // switch type

   return daliPropertyValue;
}

void PropertyValueWrapper::NewRotation( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate);

  if(!args.IsConstructCall())
  {
    DALI_SCRIPT_EXCEPTION( isolate, "Rotation constructor called without 'new'");
    return;
  }
  v8::Local<v8::Object > object;

  float v[4] = {0.f,0.f,0.f,0.f};

  bool foundAllArguments(false);
  V8Utils::ReadFloatArguments( foundAllArguments, v, 4, args, 0.f );

  int length = args.Length();

  // if length = 4 create AngleAxis, else create Quaternion

  if( length > 3 )
  {
    const Dali::AngleAxis axis(  Degree( v[0] ), Vector3(v[1], v[2], v[3]) );
    object = WrapDaliProperty( isolate, Dali::Property::Value( axis ) );
  }
  else if( length > 2 )
  {
    object = WrapDaliProperty( isolate, Dali::Quaternion( Degree(v[0]), Degree(v[1]), Degree(v[2]) ) );
  }
  else
  {
    const Dali::Quaternion quaternion( Dali::Quaternion( Dali::ANGLE_0, Dali::Vector3::YAXIS));
    object = WrapDaliProperty( isolate, quaternion );
  }

  args.GetReturnValue().Set( object );
}

//
void PropertyValueWrapper::NewMatrix( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate);

  if(!args.IsConstructCall())
  {
    DALI_SCRIPT_EXCEPTION( isolate, "Rotation constructor called without 'new'");
    return;
  }
  v8::Local<v8::Object > object;

  // we're constructing either a 4x4 Dali::Matrix or a 3x3 Dali::Matrix
  // we're expecting an array of floats
  // 4x4 = m00, m01, m02, m03,  m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33
  // 3x3 or m00, m01, m02, m10, m11, m12,m20, m21, m22, m30, m31, m32,
  bool foundAllArguments( false );

  if( args.Length() == 16 )
  {
    float v[16];
    V8Utils::ReadFloatArguments( foundAllArguments, v, 16, args, 0.f );
    if(! foundAllArguments )
    {
      DALI_SCRIPT_EXCEPTION( isolate, "invalid Matrix parameters");
      return;
    }
    Dali::Matrix matrix( v[0] );
    object = WrapDaliProperty( isolate ,  Dali::Property::Value( matrix ) );
  }
  else if ( args.Length() == 9 )
  {
    float v[9];
    V8Utils::ReadFloatArguments( foundAllArguments, v, 9, args, 0.f );
    if(! foundAllArguments )
    {
      DALI_SCRIPT_EXCEPTION( isolate, "invalid Matrix parameters");
      return;
    }
    Dali::Matrix3 matrix( v[0], v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8] );
    object = WrapDaliProperty( isolate ,  Dali::Property::Value( matrix ) );
  }
  args.GetReturnValue().Set( object );


}

v8::Handle<v8::ObjectTemplate> PropertyValueWrapper::MakeDaliPropertyTemplate( v8::Isolate* isolate )
{
  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::ObjectTemplate> objTemplate = v8::ObjectTemplate::New();

  objTemplate->SetInternalFieldCount( BaseWrappedObject::FIELD_COUNT );

  // property handle intercepts property getters and setters
  objTemplate->SetNamedPropertyHandler( PropertyGet, PropertySet);

  return handleScope.Escape( objTemplate );

}

v8::Handle<v8::Object> PropertyValueWrapper::WrapDaliProperty(v8::Isolate* isolate, const Dali::Property::Value &value)
{
  v8::EscapableHandleScope handleScope( isolate );

  // Simple Dali properties (ints, strings, bools etc) are stored as JavaScript primitives (v8::Boolean ...)
  // more complex properties (Vectors, Rectangles...) are wrapped by a JavaScript object
  if( IsPrimitive( value ))
  {
    return handleScope.Escape( CreateJavaScriptPrimitive( isolate, value) );
  }

  // This will wrap a Dali structure (Vector, Rect, Matrix etc) inside a JavaScript object.

  v8::Local<v8::ObjectTemplate> objectTemplate;

  if(PropertyValueWrapper::mTemplatePrimitive.IsEmpty() )
  {
    objectTemplate = MakeDaliPropertyTemplate( isolate );
    PropertyValueWrapper::mTemplatePrimitive.Reset( isolate, objectTemplate);
  }
  else
  {
    // get the object template
    objectTemplate = v8::Local<v8::ObjectTemplate>::New( isolate, PropertyValueWrapper::mTemplatePrimitive );
  }

  v8::Local<v8::Object> localObject = objectTemplate->NewInstance();

  PropertyValueWrapper* pointer =  new PropertyValueWrapper( value, Dali::V8Plugin::DaliWrapper::Get().GetDaliGarbageCollector() );

   // assign the JavaScript object to the wrapper.
  // This also stores Dali object, in an internal field inside the JavaScript object.
  pointer->SetJavascriptObject( isolate, localObject );

  return handleScope.Escape( localObject );

}

// Utility function that extracts the C++ map pointer from a wrapper
// object.

PropertyValueWrapper*  PropertyValueWrapper::Unwrap( v8::Isolate* isolate, v8::Handle< v8::Object> obj)
{
  v8::HandleScope handleScope( isolate );

  v8::Local<v8::External> field = v8::Local<v8::External>::Cast( obj->GetInternalField(0) );
  void* ptr = field->Value();
  return static_cast< PropertyValueWrapper *>(ptr);
}

void PropertyValueWrapper::PropertyGet( v8::Local<v8::String> propertyName,
                                        const v8::PropertyCallbackInfo<v8::Value>& info)
{
  v8::Isolate* isolate = info.GetIsolate();
  v8::HandleScope handleScope( isolate );

  std::string name = V8Utils::v8StringToStdString( propertyName );

  // unwrap the object
  PropertyValueWrapper* propWrapper = Unwrap( isolate, info.This() );

  v8::Local<v8::Value> value = GetV8ValueFrom( isolate, propWrapper->mValue, name );

  info.GetReturnValue().Set( value );
}

void PropertyValueWrapper::PropertySet( v8::Local<v8::String> propertyName,
                  v8::Local<v8::Value> javaScriptValue,
                  const v8::PropertyCallbackInfo<v8::Value>& info)
{
  v8::Isolate* isolate = info.GetIsolate();
  v8::HandleScope handleScope( isolate );

  std::string name =  V8Utils::v8StringToStdString( propertyName );

  PropertyValueWrapper* propWrapper = Unwrap( isolate, info.This() );

  SetFromV8Value( isolate, propWrapper->mValue, name, javaScriptValue );

}


} // namespace V8Plugin

} // namespace Dali

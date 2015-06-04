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
#include "dali-any-javascript-converter.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <object/property-value-wrapper.h>
#include <actors/actor-wrapper.h>
#include <events/event-object-generator.h>
#include <animation/animation-wrapper.h>
#include <image/image-wrapper.h>


namespace Dali
{

namespace V8Plugin
{

v8::Local<v8::Value> DaliAnyConverter::ConvertToJavaScriptObject(v8::Isolate* isolate, Dali::Any& value )
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::Value> returnValue;

  const std::type_info& typeInfo( value.GetType());

  // check the type of the Any and convert it to a JavaScript object
  // if we wanted to speed this up, it would be possible to hash the typeInfo.name() field.

  if( typeInfo == typeid( Dali::Actor ))
  {
    Actor actor =  value.Get<Actor>();
    if( actor )
    {
      returnValue = ActorWrapper::WrapActor( isolate, actor );
    }
    else
    {
      returnValue = v8::Undefined( isolate );
    }
  }
  else if(  typeInfo == typeid( std::string ) )
  {

    std::string string = value.Get< std::string >();
    //printf(" converting %s to a javascript object \n", string.c_str());
    returnValue =  v8::String::NewFromUtf8( isolate, string.c_str());
  }
  else if(  typeInfo == typeid( Dali::Vector3 ) )
  {
    returnValue = PropertyValueWrapper::WrapDaliProperty( isolate, value.Get<Vector3>() );
  }
  else if(  typeInfo == typeid( Dali::TouchEvent ) )
  {
    returnValue = EventObjectGenerator::CreateTouchEvent( isolate, value.Get<TouchEvent>() );
  }
  else if(  typeInfo == typeid( Dali::HoverEvent ) )
  {
    returnValue = EventObjectGenerator::CreateHoverEvent( isolate, value.Get<HoverEvent>() );
  }
  else if(  typeInfo == typeid( Dali::WheelEvent ) )
  {
    returnValue = EventObjectGenerator::CreateWheelEvent( isolate, value.Get<WheelEvent>() );
  }
  else if(  typeInfo == typeid( Dali::KeyEvent ) )
  {
    returnValue = EventObjectGenerator::CreateKeyEvent( isolate, value.Get<KeyEvent>() );
  }
  else if(  typeInfo == typeid( Dali::PanGesture ) )
  {
    returnValue = EventObjectGenerator::CreatePanGesture( isolate, value.Get<PanGesture>() );
  }
  else if(  typeInfo == typeid( Dali::Animation ) )
  {
    returnValue = AnimationWrapper::WrapAnimation( isolate, value.Get<Animation>() );
  }
  else if(  typeInfo == typeid( Dali::Image ) )
  {
    returnValue = ImageWrapper::WrapImage( isolate, value.Get<Image>() );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, " Failed to find Dali::Any to JavaScript converter \n" );
  }

  return handleScope.Escape( returnValue );
}

Dali::Any DaliAnyConverter::ConvertToDaliAny( v8::Isolate* isolate, v8::Local<v8::Value> value, const Dali::Any& requestedType)
{
  v8::HandleScope handleScope( isolate );

  if( value.IsEmpty() )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "Callback missing a return value \n");
    return Dali::Any();
  }
  // try to match the return value types
  if( requestedType.GetType() == typeid( bool ))
  {
    if( value->IsBoolean() )
    {
      return value->ToBoolean()->Value();
    }
    else
    {
      DALI_SCRIPT_EXCEPTION( isolate, "Invalid return type from callback, wanted a bool \n");
    }
  }
  else if( requestedType.GetType() == typeid( unsigned int ) )
  {
    if( value->IsUint32() )
    {
      return value->ToUint32()->Value();
    }
    else
    {
      DALI_SCRIPT_EXCEPTION( isolate, "Invalid return type from callback, wanted an unsigned int\n");
    }
  }
  // allow positive and negative numbers for int,@todo test how v8 behaves with numbers returned
  else if( requestedType.GetType() == typeid( int ))
  {
    if( value->IsInt32() )
    {
      return value->ToInt32()->Value();
    }
    else if( value->IsUint32() )
    {
      return static_cast<int>( value->ToUint32()->Value());
    }
    else
    {
      DALI_SCRIPT_EXCEPTION(isolate, "Invalid return type from callback, wanted an int\n");
    }
  }
  else if( requestedType.GetType() == typeid( float ))
  {
    if( value->IsNumber() )
    {
      return value->ToNumber()->Value();
    }
    else if( value->IsInt32() )
    {
      return static_cast<float>(value->ToInt32()->Value());
    }
    else if( value->IsUint32() )
    {
      return static_cast<float>( value->ToUint32()->Value() );
    }
    else
    {
      DALI_SCRIPT_EXCEPTION(isolate, "Invalid return type from callback, wanted a float\n");
    }
  }
  else if( requestedType.GetType() == typeid( Actor ))
  {
    if( value->IsObject()  )
    {
      HandleWrapper* handleWrapper = HandleWrapper::Unwrap( isolate, value->ToObject() );
      if( handleWrapper )
      {
        return Actor::DownCast( handleWrapper->mHandle );
      }
      else
      {
        DALI_SCRIPT_EXCEPTION(isolate, "Invalid return type from callback, wanted an Actor \n");
      }
    }
    else
    {
      // undefined is used to describe an empty actor handle
      if (value->IsUndefined())
      {
        return Actor();
      }
      else
      {
        DALI_SCRIPT_EXCEPTION(isolate, "missing return type, wanted an Actor \n");
      }
    }

  }
  return Dali::Any(); // empty any
}

} // namespace V8Plugin

} // namespace Dali

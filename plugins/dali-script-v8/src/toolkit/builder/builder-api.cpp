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
#include "builder-api.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <string>
#include <fstream>
#include <streambuf>

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <object/property-value-wrapper.h>
#include <object/handle-wrapper.h>
#include <toolkit/builder/builder-wrapper.h>
#include <actors/actor-wrapper.h>
#include <image/image-wrapper.h>
#include <animation/animation-wrapper.h>
#include <shader-effects/shader-effect-wrapper.h>
#include <rendering/shader-wrapper.h>


namespace Dali
{

namespace V8Plugin
{

namespace // un named namespace
{


Dali::Toolkit::Builder GetBuilder( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate );

  v8::Local<v8::Object> object = args.This();
  v8::Local<v8::External> field = v8::Local<v8::External>::Cast( object->GetInternalField( 0 ) );
  void* ptr = field->Value();

  BuilderWrapper* wrapper = static_cast<BuilderWrapper *>( ptr );
  return wrapper->GetBuilder();
}


void GetMapAndString( v8::Isolate* isolate,
                      std::string stringKey,
                      std::string& stringValue,
                      std::string mapKey,
                      Dali::Property::Map& map,
                      v8::Local<v8::Object> object)
{
    v8::Local<v8::Value> stringNameValue = object->Get( v8::String::NewFromUtf8( isolate, stringKey.c_str() ) );
    if( !stringNameValue->IsString() )
    {
      std::string message = "missing field :"+stringKey;
      DALI_SCRIPT_EXCEPTION( isolate,  message);
      return;
    }
    stringValue = V8Utils::v8StringToStdString( stringNameValue );

    // constants is optional

    v8::Local<v8::Value> constants = object->Get( v8::String::NewFromUtf8( isolate, mapKey.c_str() ) );
    if( constants->IsObject() )
    {
      map = V8Utils::GetPropertyMapFromObject( isolate, constants->ToObject() );
    }
}


} // un-named namespace


Dali::Toolkit::Builder BuilderApi::New( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  return Dali::Toolkit::Builder::New();
}

void BuilderApi::LoadFromString( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Dali::Toolkit::Builder builder = GetBuilder( isolate, args );

  bool found(false);
  std::string str = V8Utils::GetStringParameter( 0, found, isolate, args );
  if( found )
  {
    builder.LoadFromString( str );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate,  "missing string parameter");
  }
}

void BuilderApi::LoadFromFile( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Dali::Toolkit::Builder builder = GetBuilder( isolate, args );

  bool found(false);
  std::string fileName= V8Utils::GetStringParameter( 0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate,  "missing string parameter");
    return;
  }

  std::ifstream fileStream( fileName.c_str() );
  if( !fileStream.is_open() )
  {
    std::string message = "failed to open JSON file" + fileName;
    DALI_SCRIPT_EXCEPTION( isolate,  message );
    return;
  }

  //single-pass input iterator that reads successive characters from the input stream
  std::string json = std::string( (std::istreambuf_iterator< char >( fileStream )), std::istreambuf_iterator< char >() );

  builder.LoadFromString( json );

}

void BuilderApi::AddConstants( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Dali::Toolkit::Builder builder = GetBuilder( isolate, args );

  bool found(false);
  Dali::Property::Map map = V8Utils::GetPropertyMapParameter( 0,found, isolate, args );

  if( found )
  {
    builder.AddConstants( map );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate,  "no constants found" );
  }

}

void BuilderApi::GetConstants( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Dali::Toolkit::Builder builder = GetBuilder( isolate, args );

  const Dali::Property::Map& map  = builder.GetConstants();

  v8::Local<v8::Object> object = v8::Object::New( isolate );

  V8Utils::CreatePropertyMap( isolate, map, object );

  args.GetReturnValue().Set( object );

}
void BuilderApi::CreateAnimation( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  // options =
  // {
  //    animation: ..
  //    constants:
  //    actor:
  // }
  //

  Dali::Toolkit::Builder builder = GetBuilder( isolate, args );

  if( !args[0]->IsObject() )
  {
    DALI_SCRIPT_EXCEPTION( isolate,  "invalid property map, expecting { animation: x, constants: y, actor: z}" );
    return;
  }
  v8::Local<v8::Object> object = args[0]->ToObject();

  std::string animationName;
  Dali::Property::Map map;

  //  get the animation name and constant map

  GetMapAndString( isolate,
                   "animation",
                   animationName,
                   "constants",
                   map,
                   object );

  // actor is optional
  Actor actor;
  v8::Local<v8::Value> actorValue = object->Get( v8::String::NewFromUtf8( isolate, "actor" ) );
  if( actorValue->IsObject() )
  {
    HandleWrapper* handleWrapper = HandleWrapper::Unwrap( isolate, actorValue->ToObject() );
    if( handleWrapper )
    {
      actor = Actor::DownCast( handleWrapper->mHandle );
    }
  }
  Animation anim;
  if( actor )
  {
    //  Animation CreateAnimation( const std::string& animationName, const Dali::Property::Map& map, Dali::Actor sourceActor );
    anim = builder.CreateAnimation( animationName, map, actor );
  }
  else
  {
    anim = builder.CreateAnimation( animationName, map);
  }
  v8::Local<v8::Object> localObject = AnimationWrapper::WrapAnimation( isolate, anim );
  args.GetReturnValue().Set( localObject );
}

void BuilderApi::Create( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Dali::Toolkit::Builder builder = GetBuilder( isolate, args );

  if( !args[0]->IsObject() )
  {
    DALI_SCRIPT_EXCEPTION( isolate,  "invalid param");
    return;
  }
  v8::Local<v8::Object> object = args[0]->ToObject();

  // options =
  // {
  //    template: "my-temppalte",
  //    constants: {  IMAGE_DIR: "/usr/apps" ,  SHADER_DIR: "/usr/apps/.."}
  // }
  //
  std::string templateName;
  Dali::Property::Map map;

  GetMapAndString( isolate,
                   "template",
                   templateName,
                   "constants",
                   map,
                   object );

  BaseHandle handle = builder.Create( templateName, map );
  std::string typeName = handle.GetTypeName();
  ActorWrapper::ActorType actorType = ActorWrapper::GetActorType( typeName );

  if( actorType != ActorWrapper::UNKNOWN_ACTOR )
  {
    v8::Local<v8::Object> actorObject =  ActorWrapper::WrapActor(isolate, Actor::DownCast(handle),actorType);
    args.GetReturnValue().Set( actorObject );
    return;
  }

  if( typeName == "Animation")
  {
    v8::Local<v8::Object> animation = AnimationWrapper::WrapAnimation(isolate, Animation::DownCast(handle) );
    args.GetReturnValue().Set( animation );
  }

  if( typeName == "Image")
  {
    v8::Local<v8::Object> image = ImageWrapper::WrapImage(isolate, Image::DownCast(handle) );
    args.GetReturnValue().Set( image );
  }
  if( typeName == "ShaderEffect")
  {
    v8::Local<v8::Object> shaderEffect = ShaderEffectWrapper::WrapShaderEffect(isolate, ShaderEffect::DownCast( handle ));
    args.GetReturnValue().Set( shaderEffect );
  }
  if( typeName == "Shader")
  {
    v8::Local<v8::Object> shader = ShaderWrapper::WrapShader(isolate, Shader::DownCast( handle ));
    args.GetReturnValue().Set( shader );
  }
}
void BuilderApi::ApplyStyle( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Dali::Toolkit::Builder builder = GetBuilder( isolate, args );

  // bool ApplyStyle( const std::string& styleName, Handle& handle );

  bool found(false);
  std::string styleName = V8Utils::GetStringParameter( PARAMETER_0, found, isolate, args);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate,  "missing style name" );
    return;
  }

  Dali::Handle handle = V8Utils::GetHandleParameter( PARAMETER_1, found, isolate, args  );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate,  "missing handle parameter" );
    return;
  }

  builder.ApplyStyle( styleName, handle );

}

void BuilderApi::ApplyFromJson( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Dali::Toolkit::Builder builder = GetBuilder( isolate, args );

  bool found;

  Dali::Handle handle = V8Utils::GetHandleParameter( PARAMETER_0, found, isolate, args  );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate,  "missing handle parameter" );
    return;
  }
  std::string jsonString = V8Utils::GetStringParameter( PARAMETER_1, found, isolate, args);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate,  "missing JSON string" );
    return;
  }
  builder.ApplyFromJson( handle, jsonString );
}

void BuilderApi::AddActors( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Dali::Toolkit::Builder builder = GetBuilder( isolate, args );

  //{
  //  actor:actor
  //  section: name
  //}

  Actor actor;

  if( !args[0]->IsObject() )
  {
    DALI_SCRIPT_EXCEPTION( isolate,  "invalid param");
    return;
  }
  v8::Local<v8::Object> object = args[0]->ToObject();

  v8::Local<v8::Value> actorValue = object->Get( v8::String::NewFromUtf8( isolate, "actor" ) );
  if( actorValue->IsObject() )
  {
    HandleWrapper* handleWrapper = HandleWrapper::Unwrap( isolate, actorValue->ToObject() );
    if( handleWrapper )
    {
      actor = Actor::DownCast( handleWrapper->mHandle );
    }
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate,  "actor field not found in param");
    return;
  }

  v8::Local<v8::Value> stringNameValue = object->Get( v8::String::NewFromUtf8( isolate, "section" ) );
  if( !stringNameValue->IsString() )
  {
    std::string message = "missing field: section";
    DALI_SCRIPT_EXCEPTION( isolate,  message);
    return;
  }
  std::string sectionName = V8Utils::v8StringToStdString( stringNameValue );
  if( !sectionName.empty() )
  {
    builder.AddActors( sectionName, actor );
  }
  else
  {
    builder.AddActors(  actor );
  }

}

void BuilderApi::CreateRenderTask( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Dali::Toolkit::Builder builder = GetBuilder( isolate, args );
  bool found(false);
  std::string str = V8Utils::GetStringParameter( 0, found, isolate, args );

  if( found )
  {
    builder.CreateRenderTask( str );
  }
  else
  {
     DALI_SCRIPT_EXCEPTION( isolate, "render task name");
  }
}

void BuilderApi::GetShaderEffect( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Dali::Toolkit::Builder builder = GetBuilder( isolate, args );

  bool found(false);
  std::string str = V8Utils::GetStringParameter( 0, found, isolate, args );
  if( found )
  {
    ShaderEffect handle = builder.GetShaderEffect( str );
    v8::Local<v8::Object> shader = ShaderEffectWrapper::WrapShaderEffect(isolate, handle);
    args.GetReturnValue().Set( shader );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "missing shader name");
  }
}

void BuilderApi::GetFrameBufferImage( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
   v8::HandleScope handleScope( isolate );
   Dali::Toolkit::Builder builder = GetBuilder( isolate, args );

   bool found(false);
   std::string str = V8Utils::GetStringParameter( 0, found, isolate, args );
   if( found )
   {
     // wrap the image
     // @TODO Test this, may need to implement a Framebuffer image wrapper.?
     v8::Local<v8::Object> localObject = ImageWrapper::WrapImage( isolate, builder.GetFrameBufferImage(str)  );
     args.GetReturnValue().Set( localObject );
   }
   else
   {
     DALI_SCRIPT_EXCEPTION( isolate,  "frame buffer name");
   }

}

} // namespace V8Plugin

} // namespace Dali

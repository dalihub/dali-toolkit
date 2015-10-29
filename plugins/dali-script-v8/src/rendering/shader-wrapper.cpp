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
#include "shader-wrapper.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <rendering/shader-api.h>
#include <shared/api-function.h>
#include <shared/object-template-helper.h>
#include <dali-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

v8::Persistent<v8::ObjectTemplate> ShaderWrapper::mShaderTemplate;

ShaderWrapper::ShaderWrapper( const Dali::Shader& shader, GarbageCollectorInterface& gc )
:  HandleWrapper(  BaseWrappedObject::SHADER , shader, gc )
{
    mShader = shader;
}

v8::Handle<v8::Object> ShaderWrapper::WrapShader(v8::Isolate* isolate, const Dali::Shader& shader )
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  objectTemplate = GetShaderTemplate( isolate);

  // create an instance of the template
  v8::Local<v8::Object> localObject = objectTemplate->NewInstance();

  // create the Shader wrapper
  ShaderWrapper* pointer =  new ShaderWrapper( shader, Dali::V8Plugin::DaliWrapper::Get().GetDaliGarbageCollector() );

  // assign the JavaScript object to the wrapper.
  pointer->SetJavascriptObject( isolate, localObject );

  return handleScope.Escape( localObject );
}

v8::Local<v8::ObjectTemplate> ShaderWrapper::GetShaderTemplate( v8::Isolate* isolate)
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  if( mShaderTemplate.IsEmpty() )
  {
    objectTemplate = MakeShaderTemplate( isolate );
    mShaderTemplate.Reset( isolate, objectTemplate );
  }
  else
  {
    // get the object template
    objectTemplate = v8::Local<v8::ObjectTemplate>::New( isolate, mShaderTemplate );
  }
  return handleScope.Escape( objectTemplate );
}

v8::Handle<v8::ObjectTemplate> ShaderWrapper::MakeShaderTemplate( v8::Isolate* isolate )
{
  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::ObjectTemplate> objTemplate = v8::ObjectTemplate::New();

  // property handle intercepts property getters and setters and signals
  HandleWrapper::AddInterceptsToTemplate( isolate, objTemplate );

  objTemplate->SetInternalFieldCount( BaseWrappedObject::FIELD_COUNT );

  return handleScope.Escape( objTemplate );
}

void ShaderWrapper::NewShader( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate);

  if(!args.IsConstructCall())
  {
      DALI_SCRIPT_EXCEPTION( isolate, "Shader constructor called without 'new'");
      return;
  }
  Dali::Shader shader = ShaderApi::New( isolate, args );

  if(shader)
  {
    v8::Local<v8::Object> localObject = WrapShader( isolate, shader );
    args.GetReturnValue().Set( localObject );
  }
}


Shader ShaderWrapper::GetShader()
{
  return mShader;
}


} // namespace V8Plugin

} // namespace Dali

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
#include "shader-effect-wrapper.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <shader-effects/shader-effect-api.h>
#include <shared/api-function.h>
#include <shared/object-template-helper.h>
#include <signals/signal-manager.h>
#include <dali-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

v8::Persistent<v8::ObjectTemplate> ShaderEffectWrapper::mShaderEffectTemplate;

namespace // un-named name space
{

/**
 * Contains a list of all functions that can be called
 */
const ApiFunction ShaderEffectFunctionTable[]=
{
    /**************************************
    * ShaderEffect API (in order of shaderEffect.h)
    **************************************/

   { "SetEffectImage"             , ShaderEffectApi::SetEffectImage },
   { "SetUniform"                 , ShaderEffectApi::SetUniform     },
};

const unsigned int ShaderEffectFunctionTableCount = sizeof(ShaderEffectFunctionTable)/sizeof(ShaderEffectFunctionTable[0]);
} //un-named space


ShaderEffectWrapper::ShaderEffectWrapper( const Dali::ShaderEffect& shaderEffect, GarbageCollectorInterface& gc )
:  HandleWrapper(  BaseWrappedObject::SHADER_EFFECT , shaderEffect, gc )
{
    mShaderEffect = shaderEffect;
}

v8::Handle<v8::Object> ShaderEffectWrapper::WrapShaderEffect(v8::Isolate* isolate, const Dali::ShaderEffect& shaderEffect )
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  objectTemplate = GetShaderEffectTemplate( isolate);

  // create an instance of the template
  v8::Local<v8::Object> localObject = objectTemplate->NewInstance();

  // create the ShaderEffect wrapper
  ShaderEffectWrapper* pointer =  new ShaderEffectWrapper( shaderEffect, Dali::V8Plugin::DaliWrapper::Get().GetDaliGarbageCollector() );

  // assign the JavaScript object to the wrapper.
  pointer->SetJavascriptObject( isolate, localObject );

  return handleScope.Escape( localObject );
}

v8::Local<v8::ObjectTemplate> ShaderEffectWrapper::GetShaderEffectTemplate( v8::Isolate* isolate)
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  if( mShaderEffectTemplate.IsEmpty() )
  {
    objectTemplate = MakeShaderEffectTemplate( isolate );
    mShaderEffectTemplate.Reset( isolate, objectTemplate );
  }
  else
  {
    // get the object template
    objectTemplate = v8::Local<v8::ObjectTemplate>::New( isolate, mShaderEffectTemplate );
  }
  return handleScope.Escape( objectTemplate );
}

v8::Handle<v8::ObjectTemplate> ShaderEffectWrapper::MakeShaderEffectTemplate( v8::Isolate* isolate )
{
  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::ObjectTemplate> objTemplate = v8::ObjectTemplate::New();

  // property handle intercepts property getters and setters and signals
  HandleWrapper::AddInterceptsToTemplate( isolate, objTemplate );

  objTemplate->SetInternalFieldCount( BaseWrappedObject::FIELD_COUNT );

  // add our function properties
  ObjectTemplateHelper::InstallFunctions( isolate, objTemplate, ShaderEffectFunctionTable, ShaderEffectFunctionTableCount );

  return handleScope.Escape( objTemplate );
}

void ShaderEffectWrapper::NewShaderEffect( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate);

  if(!args.IsConstructCall())
  {
      DALI_SCRIPT_EXCEPTION( isolate, "ShaderEffect constructor called without 'new'");
      return;
  }
  Dali::ShaderEffect shaderEffect = ShaderEffectApi::New( isolate, args );

  v8::Local<v8::Object> localObject = WrapShaderEffect( isolate, shaderEffect );
  args.GetReturnValue().Set( localObject );
}


ShaderEffect ShaderEffectWrapper::GetShaderEffect()
{
  return mShaderEffect;
}


} // namespace V8Plugin

} // namespace Dali

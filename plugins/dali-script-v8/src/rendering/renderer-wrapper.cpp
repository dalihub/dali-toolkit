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
#include "renderer-wrapper.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <rendering/renderer-api.h>
#include <shared/api-function.h>
#include <shared/object-template-helper.h>
#include <dali-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

v8::Persistent<v8::ObjectTemplate> RendererWrapper::mRendererTemplate;

namespace // un-named name space
{

/**
 * Contains a list of all functions that can be called
 */
const ApiFunction RendererFunctionTable[]=
{
    /**************************************
    * Renderer API (in order of Renderer.h)
    **************************************/

   { "SetGeometry"             , RendererApi::SetGeometry },
   { "GetGeometry"             , RendererApi::GetGeometry },
   { "SetMaterial"             , RendererApi::SetMaterial },
   { "GetMaterial"             , RendererApi::GetMaterial },
};

const unsigned int RendererFunctionTableCount = sizeof(RendererFunctionTable)/sizeof(RendererFunctionTable[0]);
} //un-named space


RendererWrapper::RendererWrapper( const Dali::Renderer& renderer, GarbageCollectorInterface& gc )
:  HandleWrapper(  BaseWrappedObject::RENDERER , renderer, gc )
{
    mRenderer = renderer;
}

v8::Handle<v8::Object> RendererWrapper::WrapRenderer(v8::Isolate* isolate, const Dali::Renderer& renderer )
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  objectTemplate = GetRendererTemplate( isolate);

  // create an instance of the template
  v8::Local<v8::Object> localObject = objectTemplate->NewInstance();

  // create the Renderer wrapper
  RendererWrapper* pointer =  new RendererWrapper( renderer, Dali::V8Plugin::DaliWrapper::Get().GetDaliGarbageCollector() );

  // assign the JavaScript object to the wrapper.
  pointer->SetJavascriptObject( isolate, localObject );

  return handleScope.Escape( localObject );
}

v8::Local<v8::ObjectTemplate> RendererWrapper::GetRendererTemplate( v8::Isolate* isolate)
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  if( mRendererTemplate.IsEmpty() )
  {
    objectTemplate = MakeRendererTemplate( isolate );
    mRendererTemplate.Reset( isolate, objectTemplate );
  }
  else
  {
    // get the object template
    objectTemplate = v8::Local<v8::ObjectTemplate>::New( isolate, mRendererTemplate );
  }
  return handleScope.Escape( objectTemplate );
}

v8::Handle<v8::ObjectTemplate> RendererWrapper::MakeRendererTemplate( v8::Isolate* isolate )
{
  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::ObjectTemplate> objTemplate = v8::ObjectTemplate::New();

  // property handle intercepts property getters and setters and signals
  HandleWrapper::AddInterceptsToTemplate( isolate, objTemplate );

  objTemplate->SetInternalFieldCount( BaseWrappedObject::FIELD_COUNT );

  // add our function properties
  ObjectTemplateHelper::InstallFunctions( isolate, objTemplate, RendererFunctionTable, RendererFunctionTableCount );

  return handleScope.Escape( objTemplate );
}

void RendererWrapper::NewRenderer( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate);

  if(!args.IsConstructCall())
  {
      DALI_SCRIPT_EXCEPTION( isolate, "Renderer constructor called without 'new'");
      return;
  }
  Dali::Renderer renderer = RendererApi::New( args );

  if(renderer)
  {
    v8::Local<v8::Object> localObject = WrapRenderer( isolate, renderer );
    args.GetReturnValue().Set( localObject );
  }
}


Renderer RendererWrapper::GetRenderer()
{
  return mRenderer;
}


} // namespace V8Plugin

} // namespace Dali

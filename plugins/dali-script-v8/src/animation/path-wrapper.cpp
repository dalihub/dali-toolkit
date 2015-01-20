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
#include "path-wrapper.h"

// INTERNAL INCLUDES
#include <animation/path-api.h>
#include <v8-utils.h>
#include <dali-wrapper.h>
#include <shared/object-template-helper.h>

namespace Dali
{

namespace V8Plugin
{

namespace
{

const ApiFunction PathFunctions[]=
{
 { "GenerateControlPoints",  PathApi::GenerateControlPoints },
 { "AddPoint",               PathApi::AddPoint },
 { "AddControlPoint",        PathApi::AddControlPoint },
 { "Sample",                 PathApi::Sample }
};

const unsigned int PathFunctionTableCount = sizeof(PathFunctions)/sizeof(PathFunctions[0]);
} //un-named space


PathWrapper::PathWrapper( Path path, GarbageCollectorInterface& gc )
:HandleWrapper( BaseWrappedObject::PATH, path, gc ),
 mPath( path )
{
}

v8::Handle<v8::ObjectTemplate> PathWrapper::MakePathTemplate( v8::Isolate* isolate )
{
  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::ObjectTemplate> objTemplate = v8::ObjectTemplate::New();
  objTemplate->SetInternalFieldCount( BaseWrappedObject::FIELD_COUNT );

  // add our function properties
  ObjectTemplateHelper::InstallFunctions( isolate, objTemplate, PathFunctions, PathFunctionTableCount );

  // property handle intercepts property getters and setters and signals
  HandleWrapper::AddInterceptsToTemplate( isolate, objTemplate );

  return handleScope.Escape( objTemplate );
}

v8::Handle<v8::Object> PathWrapper::WrapPath( v8::Isolate* isolate, Path path )
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  objectTemplate = MakePathTemplate( isolate );

  // create an instance of the template
  v8::Local<v8::Object> localObject = objectTemplate->NewInstance();

  // create teh actor object
  PathWrapper* pointer = new PathWrapper( path, Dali::V8Plugin::DaliWrapper::Get().GetDaliGarbageCollector() );

  // assign the JavaScript object to the wrapper.
  // This also stores Dali object, in an internal field inside the JavaScript object.
  pointer->SetJavascriptObject( isolate, localObject );

  return handleScope.Escape( localObject );
}

Path PathWrapper::GetPath()
{
  return mPath;
}

/**
 * Create an initialized Path handle.
 * @constructor
 * @for Path
 * @method Path
 */
void PathWrapper::NewPath( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  if( !args.IsConstructCall() )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "constructor called without 'new" );
    return;
  }

  // Create a new path
  Path path = Path::New();
  v8::Local<v8::Object> localObject = WrapPath( isolate, path );
  args.GetReturnValue().Set( localObject );
}


} // namespace V8Plugin

} // namespace Dali

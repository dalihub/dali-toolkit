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
#include "path-constrainer-wrapper.h"

// INTERNAL INCLUDES
#include <animation/constrainer-api.h>
#include <dali-wrapper.h>
#include <shared/object-template-helper.h>
#include <v8-utils.h>

namespace Dali
{

namespace V8Plugin
{

namespace
{

const ApiFunction ConstrainerFunctions[]=
{
 { "ApplyConstraint",  ConstrainerApi::Apply },
 { "RemoveConstraint", ConstrainerApi::Remove }
};
const unsigned int ConstrainerFunctionTableCount = sizeof(ConstrainerFunctions)/sizeof(ConstrainerFunctions[0]);

} //un-named space

PathConstrainerWrapper::PathConstrainerWrapper( PathConstrainer pathConstrainer, GarbageCollectorInterface& gc )
:HandleWrapper( BaseWrappedObject::PATH_CONSTRAINER, pathConstrainer, gc ),
 mPathConstrainer( pathConstrainer )
{
}

v8::Handle<v8::ObjectTemplate> PathConstrainerWrapper::MakePathConstrainerTemplate( v8::Isolate* isolate )
{
  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::ObjectTemplate> objTemplate = v8::ObjectTemplate::New();
  objTemplate->SetInternalFieldCount( BaseWrappedObject::FIELD_COUNT );

  // add our function properties
  ObjectTemplateHelper::InstallFunctions( isolate, objTemplate, ConstrainerFunctions, ConstrainerFunctionTableCount );

  // property handle intercepts property getters and setters and signals
  HandleWrapper::AddInterceptsToTemplate( isolate, objTemplate );

  return handleScope.Escape( objTemplate );
}

v8::Handle<v8::Object> PathConstrainerWrapper::WrapPathConstrainer( v8::Isolate* isolate, PathConstrainer pathConstrainer )
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  objectTemplate = MakePathConstrainerTemplate( isolate );

  // create an instance of the template
  v8::Local<v8::Object> localObject = objectTemplate->NewInstance();

  // create the pathconstrainer object
  PathConstrainerWrapper* pointer = new PathConstrainerWrapper( pathConstrainer, Dali::V8Plugin::DaliWrapper::Get().GetDaliGarbageCollector() );

  // assign the JavaScript object to the wrapper.
  // This also stores Dali object, in an internal field inside the JavaScript object.
  pointer->SetJavascriptObject( isolate, localObject );

  return handleScope.Escape( localObject );
}

PathConstrainer PathConstrainerWrapper::GetPathConstrainer()
{
  return mPathConstrainer;
}

/**
 * Create an initialized PathConstrainer handle.
 * @constructor
 * @for PathConstrainer
 * @method PathConstrainer
 */
void PathConstrainerWrapper::NewPathConstrainer( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  if( !args.IsConstructCall() )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "constructor called without 'new" );
    return;
  }

  PathConstrainer pathConstrainer = PathConstrainer::New();
  v8::Local<v8::Object> localObject = WrapPathConstrainer( isolate, pathConstrainer );
  args.GetReturnValue().Set( localObject );
}

} // namespace V8Plugin

} // namespace Dali

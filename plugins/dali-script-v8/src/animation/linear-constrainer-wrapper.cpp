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
#include "linear-constrainer-wrapper.h"

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

LinearConstrainerWrapper::LinearConstrainerWrapper( LinearConstrainer linearConstrainer, GarbageCollectorInterface& gc )
:HandleWrapper( BaseWrappedObject::LINEAR_CONSTRAINER, linearConstrainer, gc ),
 mLinearConstrainer( linearConstrainer )
{
}

v8::Handle<v8::ObjectTemplate> LinearConstrainerWrapper::MakeLinearConstrainerTemplate( v8::Isolate* isolate )
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

v8::Handle<v8::Object> LinearConstrainerWrapper::WrapLinearConstrainer( v8::Isolate* isolate, LinearConstrainer linearConstrainer )
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  objectTemplate = MakeLinearConstrainerTemplate( isolate );

  // create an instance of the template
  v8::Local<v8::Object> localObject = objectTemplate->NewInstance();

  // create the pathconstrainer object
  LinearConstrainerWrapper* pointer = new LinearConstrainerWrapper( linearConstrainer, Dali::V8Plugin::DaliWrapper::Get().GetDaliGarbageCollector() );

  // assign the JavaScript object to the wrapper.
  // This also stores Dali object, in an internal field inside the JavaScript object.
  pointer->SetJavascriptObject( isolate, localObject );

  return handleScope.Escape( localObject );
}

LinearConstrainer LinearConstrainerWrapper::GetLinearConstrainer()
{
  return mLinearConstrainer;
}

/**
 * Create an initialized PathConstrainer handle.
 * @constructor
 * @for PathConstrainer
 * @method PathConstrainer
 */
void LinearConstrainerWrapper::NewLinearConstrainer( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  if( !args.IsConstructCall() )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "constructor called without 'new" );
    return;
  }

  LinearConstrainer linearConstrainer = LinearConstrainer::New();
  v8::Local<v8::Object> localObject = WrapLinearConstrainer( isolate, linearConstrainer );
  args.GetReturnValue().Set( localObject );
}


} // namespace V8Plugin

} // namespace Dali

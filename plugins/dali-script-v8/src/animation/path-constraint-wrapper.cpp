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
#include "path-constraint-wrapper.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <dali-wrapper.h>
#include <shared/object-template-helper.h>

namespace Dali
{

namespace V8Plugin
{

PathConstraintWrapper::PathConstraintWrapper( PathConstraint pathConstraint, GarbageCollectorInterface& gc )
:HandleWrapper( BaseWrappedObject::PATH_CONSTRAINT, pathConstraint, gc ),
 mPathConstraint( pathConstraint )
{
}

v8::Handle<v8::ObjectTemplate> PathConstraintWrapper::MakePathConstraintTemplate( v8::Isolate* isolate )
{
  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::ObjectTemplate> objTemplate = v8::ObjectTemplate::New();
  objTemplate->SetInternalFieldCount( BaseWrappedObject::FIELD_COUNT );

  // property handle intercepts property getters and setters and signals
  HandleWrapper::AddInterceptsToTemplate( isolate, objTemplate );

  return handleScope.Escape( objTemplate );
}

v8::Handle<v8::Object> PathConstraintWrapper::WrapPathConstraint( v8::Isolate* isolate, PathConstraint pathConstraint )
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  objectTemplate = MakePathConstraintTemplate( isolate );

  // create an instance of the template
  v8::Local<v8::Object> localObject = objectTemplate->NewInstance();

  // create the pathconstraint object
  PathConstraintWrapper* pointer = new PathConstraintWrapper( pathConstraint, Dali::V8Plugin::DaliWrapper::Get().GetDaliGarbageCollector() );

  // assign the JavaScript object to the wrapper.
  // This also stores Dali object, in an internal field inside the JavaScript object.
  pointer->SetJavascriptObject( isolate, localObject );

  return handleScope.Escape( localObject );
}

PathConstraint PathConstraintWrapper::GetPathConstraint()
{
  return mPathConstraint;
}

/**
 * Create an initialized PathConstraint handle.
 * @constructor
 * @for Path
 * @method Path
 */
void PathConstraintWrapper::NewPathConstraint( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  if( !args.IsConstructCall() )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "constructor called without 'new" );
    return;
  }

  //Extract Path Handle
  bool parameterFound;
  Handle pathHandle = V8Utils::GetHandleParameter( PARAMETER_0, parameterFound, isolate, args );
  if( !parameterFound )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter 0 (Path)" );
    return;
  }
  Dali::Path path = Path::DownCast(pathHandle);

  //Extract range
  Vector2 range = V8Utils::GetVector2Parameter( PARAMETER_1, parameterFound, isolate, args );
  if( !parameterFound )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter 1 (Range)" );
    return;
  }

  PathConstraint pathConstraint = PathConstraint::New(path, range );
  v8::Local<v8::Object> localObject = WrapPathConstraint( isolate, pathConstraint );
  args.GetReturnValue().Set( localObject );
}


} // namespace V8Plugin

} // namespace Dali

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
#include "pan-gesture-detector-wrapper.h"

// INTERNAL INCLUDES
#include <events/pan-gesture-detector-api.h>
#include <v8-utils.h>
#include <dali-wrapper.h>
#include <shared/object-template-helper.h>

namespace Dali
{

namespace V8Plugin
{

namespace
{

const ApiFunction PanGestureDetectorFunctions[]=
{
 { "Attach",  PanGestureDetectorApi::Attach },
 { "Detach",  PanGestureDetectorApi::Detach }
};

const unsigned int PanGestureDetectorFunctionTableCount = sizeof(PanGestureDetectorFunctions)/sizeof(PanGestureDetectorFunctions[0]);
} //un-named space


PanGestureDetectorWrapper::PanGestureDetectorWrapper( PanGestureDetector panGestureDetector, GarbageCollectorInterface& gc )
:HandleWrapper( BaseWrappedObject::PAN_GESTURE_DETECTOR, panGestureDetector, gc ),
 mPanGestureDetector( panGestureDetector )
{
}

v8::Handle<v8::ObjectTemplate> PanGestureDetectorWrapper::MakePanGestureDetectorTemplate( v8::Isolate* isolate )
{
  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::ObjectTemplate> objTemplate = v8::ObjectTemplate::New();
  objTemplate->SetInternalFieldCount( BaseWrappedObject::FIELD_COUNT );

  // add our function properties
  ObjectTemplateHelper::InstallFunctions( isolate, objTemplate, PanGestureDetectorFunctions, PanGestureDetectorFunctionTableCount );

  // property handle intercepts property getters and setters and signals
  HandleWrapper::AddInterceptsToTemplate( isolate, objTemplate );

  return handleScope.Escape( objTemplate );
}

v8::Handle<v8::Object> PanGestureDetectorWrapper::WrapPanGestureDetector( v8::Isolate* isolate, PanGestureDetector panGestureDetector )
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  objectTemplate = MakePanGestureDetectorTemplate( isolate );

  // create an instance of the template
  v8::Local<v8::Object> localObject = objectTemplate->NewInstance();

  // create the Dali object
  PanGestureDetectorWrapper* pointer = new PanGestureDetectorWrapper( panGestureDetector, Dali::V8Plugin::DaliWrapper::Get().GetDaliGarbageCollector() );

  // assign the JavaScript object to the wrapper.
  // This also stores Dali object, in an internal field inside the JavaScript object.
  pointer->SetJavascriptObject( isolate, localObject );

  return handleScope.Escape( localObject );
}

PanGestureDetector PanGestureDetectorWrapper::GetPanGestureDetector()
{
  return mPanGestureDetector;
}

/**
 * Create an initialized PanGestureDetector handle.
 * @constructor
 * @for PanGestureDetector
 * @method PanGestureDetector
 */
void PanGestureDetectorWrapper::NewPanGestureDetector( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  if( !args.IsConstructCall() )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "constructor called without 'new" );
    return;
  }

  // Create a new path
  PanGestureDetector panGestureDetector = PanGestureDetector::New();
  v8::Local<v8::Object> localObject = WrapPanGestureDetector( isolate, panGestureDetector );
  args.GetReturnValue().Set( localObject );
}


} // namespace V8Plugin

} // namespace Dali

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
#include "pan-gesture-detector-api.h"

// INTERNAL INCLUDES
#include <events/pan-gesture-detector-wrapper.h>
#include <object/property-value-wrapper.h>
#include <v8-utils.h>

namespace Dali
{

namespace V8Plugin
{

namespace // unnamed namespace
{

PanGestureDetector GetPanGestureDetector( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate );

  v8::Local<v8::Object> object = args.This();
  v8::Local<v8::External> field = v8::Local<v8::External>::Cast( object->GetInternalField( 0 ) );
  void* ptr = field->Value();

  PanGestureDetectorWrapper* wrapper = static_cast<PanGestureDetectorWrapper *>( ptr );
  return wrapper->GetPanGestureDetector();
}

} // unnamed namespace

/**
 * Constructor
 *
 * @constructor
 * @for PanGestureDetector
 * @method PanGestureDetector
 */
PanGestureDetector PanGestureDetectorApi::New( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  return PanGestureDetector::New();
}

/**
 * Attaches an actor to the pan gesture.
 *
 * The pan-detected signal will be dispatched when the pan gesture occurs on
 * the attached actor. You can attach several actors to a pan gesture detector.
 * @method attach
 * @for PanGestureDetector
 * @param {Actor} actor The actor to attach to the pan gesture detector
 * @example
 *        panGestureDetector.attach(actor);
 */
void PanGestureDetectorApi::Attach( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  bool found( false );
  //Get actor
  Dali::Actor actor = V8Utils::GetActorParameter( PARAMETER_0, found, isolate, args );
  if( found )
  {
    PanGestureDetector panGestureDetector = GetPanGestureDetector( isolate, args );
    panGestureDetector.Attach( actor );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter" );
  }
}


/**
 * Detaches the attached actor from the pan gesture detector.
 *
 * The specified actor should have been attached to the pan gesture detector
 * @method detach
 * @for PanGestureDetector
 * @param {Actor} actor The actor to detach from the pan gesture detector
 * @example
 *        panGestureDetector.detach(actor);
 */
void PanGestureDetectorApi::Detach( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  bool found( false );
  //Get actor
  Dali::Actor actor = V8Utils::GetActorParameter( PARAMETER_0, found, isolate, args );
  if( found )
  {
    PanGestureDetector panGestureDetector = GetPanGestureDetector( isolate, args );
    panGestureDetector.Detach( actor );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter" );
  }
}


} // namespace V8Plugin

} // namespace Dali

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
#include "camera-actor-api.h"

// INTERNAL INCLUDES
#include <object/handle-wrapper.h>
#include <v8-utils.h>
#include <shared/base-wrapped-object.h>
#include <object/property-value-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

namespace // un-named namespace
{

CameraActor GetCameraActor( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  HandleWrapper* handleWrapper = HandleWrapper::Unwrap( isolate, args.This() );
  return CameraActor::DownCast( handleWrapper->mHandle );
}

} // un-named name space

/***************************************
 * CAMERA ACTOR FUNCTIONS
 ****************************************/
/**
 * Constructor
 *
 * @constructor
 * @method CameraActor
 * @for CameraActor
 * @return {Object} CameraActor
 */
Actor CameraActorApi::New( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  return CameraActor::New();
}

/**
 *
 * Sets the default camera perspective projection for the given canvas size.
 *
 * Sets the near and far clipping planes, the field of view, the aspect ratio
 * and the Z position of the actor based on the canvas size so that 1 unit in
 * XY (z=0) plane is 1 pixel on screen.
 *
 * If the canvas size is ZERO, it sets the default camera perspective
 * projection for the stage's size.
 *
 * If size is non ZERO, \e width and \e height must be greater than zero.
 *
 *
 * @example
 *     var camera = dali.stage.getRenderTaskList().getTask(0).getCameraActor();
 *
 *     camera.setPerspectiveProjection( [100, 150] );
 *
 *
 * @for CameraActor
 * @method setPerspectiveProjection
 * @param {Object}  The canvas size, array of 2 numbers
 */
void CameraActorApi::SetPerspectiveProjection( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  CameraActor cameraActor = GetCameraActor( isolate, args );

  bool found;
  Vector2 size = V8Utils::GetVector2Parameter( PARAMETER_0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter" );
    return;
  }
  cameraActor.SetPerspectiveProjection( size );
}

/**
 *
 * Sets the camera projection to use orthographic projection.
 *
 * The XY plane is centered on the camera axis. The units in the X/Y
 * plane directly equate to pixels on an equivalently sized
 * framebuffer.
 *
 * The Z position of the actor, and the near and far clip planes of the
 * bounding box match those that would be created by using
 * SetPerspectiveProjection with the same size.
 *
 *
 * @example
 *     var camera = dali.stage.getRenderTaskList().getTask(0).getCameraActor();
 *     camera.setOrthographicProjection( [1920, 1080] );
 *
 * @for CameraActor
 * @method setOrthographicProjection
 * @param {Object}  Size Size of XY plane (normal to camera axis)
 */
void CameraActorApi::SetOrthographicProjection( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  CameraActor cameraActor = GetCameraActor( isolate, args );

  bool found;
  Vector2 size = V8Utils::GetVector2Parameter( PARAMETER_0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter" );
    return;
  }
  cameraActor.SetOrthographicProjection( size );
}

} // namespace V8Plugin

} // namespace Dali

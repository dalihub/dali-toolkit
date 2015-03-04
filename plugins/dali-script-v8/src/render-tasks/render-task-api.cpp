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
#include "render-task-api.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <render-tasks/render-task-wrapper.h>
#include <image/image-wrapper.h>
#include <actors/actor-wrapper.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/actors/camera-actor.h>


namespace Dali
{

namespace V8Plugin
{

namespace // un named namespace
{


RenderTask GetRenderTask( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate );

  v8::Local<v8::Object> object = args.This();
  v8::Local<v8::External> field = v8::Local<v8::External>::Cast( object->GetInternalField(0) );
  void* ptr = field->Value();

  RenderTaskWrapper* wrapper = static_cast< RenderTaskWrapper *>(ptr);
  return wrapper->GetRenderTask();
}

}

/**
 * Set the actors to be rendered
 * @method setSourceActor
 * @for RenderTask
 * @param {Object} actor This actor and its children will be rendered.
 */
void RenderTaskApi::SetSourceActor( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderTask renderTask = GetRenderTask( isolate, args );
  bool found( false );
  Actor sourceActor = V8Utils::GetActorParameter( PARAMETER_0, found, isolate, args );

  if( found )
  {
    renderTask.SetSourceActor( sourceActor );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "source actor not found" );
  }

}
/**
 * Get the actor to be rendered
 * @method getSourceActor
 * @for RenderTask
 * @param {Object} actor source actor
 */
void RenderTaskApi::GetSourceActor(const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderTask renderTask = GetRenderTask( isolate, args );

  Actor source = renderTask.GetSourceActor();

  v8::Handle < v8::Object > wrappedActor = ActorWrapper::WrapActor( isolate, source );
  args.GetReturnValue().Set( wrappedActor );
}

/**
 * Set whether the RenderTask has exclusive access to the source actors; the default is false.
 * @method setExclusive
 * @for RenderTask
 * @param {Boolean} exclusive True if the source actors will only be rendered by this render-task.
 */
void RenderTaskApi::SetExclusive( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderTask renderTask = GetRenderTask( isolate, args );

  bool found( false );
  bool exclusive = V8Utils::GetBooleanParameter( PARAMETER_0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "boolean parameter missing" );
    return;
  }
  renderTask.SetExclusive( exclusive );

}

/**
 * Query whether the RenderTask has exclusive access to the source actors.
 * @method isExclusive
 * @for RenderTask
 * @return {Boolean} exclusive True if the source actors will only be rendered by this render-task.
 */
void RenderTaskApi::IsExclusive( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderTask renderTask = GetRenderTask( isolate, args );

  args.GetReturnValue().Set( v8::Boolean::New( isolate, renderTask.IsExclusive() ) );

}

/**
 * Set whether the render-task should be considered for input handling; the default is true.
 *
 * The task used for input handling will be last task in the RenderTaskList which has input enabled,
 * and has a valid source & camera actor.
 * A RenderTask targetting a frame-buffer can still be hit-tested, provided that the screen->frame-buffer
 * coordinate conversion is successful; see also SetScreenToFrameBufferFunction().
 * @method setInputEnabled
 * @for RenderTask
 * @param {Boolean} enabled True if the render-task should be considered for input handling.
 */
void RenderTaskApi::SetInputEnabled( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderTask renderTask = GetRenderTask( isolate, args );

  bool found( false );
  bool inputEnabled = V8Utils::GetBooleanParameter( PARAMETER_0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "boolean parameter missing" );
    return;
  }
  renderTask.SetInputEnabled( inputEnabled );

}

/**
 * Query whether the render-task should be considered for input handling.
 * @method getInputEnabled
 * @for RenderTask
 * @return {Boolean} enabled True if the render-task should be considered for input handling.
 */
void RenderTaskApi::GetInputEnabled( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderTask renderTask = GetRenderTask( isolate, args );

  args.GetReturnValue().Set( v8::Boolean::New( isolate, renderTask.GetInputEnabled() ) );


}

/**
 * Set the actor from which the scene is viewed.
 * @method setCameraActor
 * @for RenderTask
 * @param {Object} cameraActor The scene is viewed from the perspective of this actor.
 */
void RenderTaskApi::SetCameraActor( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderTask renderTask = GetRenderTask( isolate, args );
  bool found (false );
  CameraActor cameraActor;
  Actor actor =  V8Utils::GetActorParameter( PARAMETER_0, found, isolate, args );
  if( found )
  {
    cameraActor = CameraActor::DownCast( actor );
  }

  if( cameraActor )
  {
     renderTask.SetCameraActor( cameraActor );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "source actor not found" );
  }

}

/**
 * Retrieve the actor from which the scene is viewed..
 * @method getCameraActor
 * @for RenderTask
 * @return {Object} cameraActor The scene is viewed from the perspective of this actor.
 */
void RenderTaskApi::GetCameraActor( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderTask renderTask = GetRenderTask( isolate, args );

  Actor cameraActor = renderTask.GetSourceActor();

  v8::Handle < v8::Object > wrappedActor = ActorWrapper::WrapActor( isolate, cameraActor );
  args.GetReturnValue().Set( wrappedActor );

}

/**
 * Set the frame-buffer used as a render target.
 * @method setTargetFrameBuffer
 * @for RenderTask
 * @param {Object}  frameBuffer A valid frame-buffer handle to enable off-screen rendering, or an uninitialized handle to disable.
 */
void RenderTaskApi::SetTargetFrameBuffer(const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderTask renderTask = GetRenderTask( isolate, args );

  bool found( false );
  // @TODO Test this, may need to implement a Framebuffer image.?
  Image image = V8Utils::GetImageParameter( PARAMETER_0, found, isolate, args );
  if( !found )
  {
     DALI_SCRIPT_EXCEPTION( isolate, "bad parameters" );
     return;
  }

  FrameBufferImage frameBufferImage = FrameBufferImage::DownCast(image);

  renderTask.SetTargetFrameBuffer( frameBufferImage );

}


/**
 * Get the frame-buffer used as a render target.
 * @method getTargetFrameBuffer
 * @for RenderTask
 * @return {Object}  frameBuffer A valid frame-buffer handle to enable off-screen rendering, or an uninitialized handle to disable.
 */
void RenderTaskApi::GetTargetFrameBuffer( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderTask renderTask = GetRenderTask( isolate, args );

  // wrap the image
  // @TODO Test this, may need to implement a Framebuffer image wrapper.?
  v8::Local<v8::Object> localObject = ImageWrapper::WrapImage( isolate, renderTask.GetTargetFrameBuffer(),ImageWrapper::FRAME_BUFFER_IMAGE );
  args.GetReturnValue().Set( localObject );
}


/**
 * Set the actor used to convert screen coordinates to frame-buffer coordinates.
 *
 * The local coordinates of the actor are mapped as frame-buffer coordinates.
 * This is useful for hit-testing actors which are rendered off-screen.
 * Note: The mapping actor needs to be rendered by the default render task to make the mapping work properly.
 * @method setScreenToFrameBufferMappingActor
 * @for RenderTask
 * @param {Object}  mappingActor The actor used for conversion.
 */
void RenderTaskApi::SetScreenToFrameBufferMappingActor( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderTask renderTask = GetRenderTask( isolate, args );
  bool found( false );
  Actor sourceActor = V8Utils::GetActorParameter( PARAMETER_0, found, isolate, args );

  if( found )
  {
    renderTask.SetScreenToFrameBufferMappingActor( sourceActor );
  }
  else
  {
      DALI_SCRIPT_EXCEPTION( isolate, "source actor not found" );
  }
}

/**
 * Retrieve the actor used to convert screen coordinates to frame-buffer coordinates.
 *
 * @method getScreenToFrameBufferMappingActor
 * @for RenderTask
 * @return {Object} actor used for conversion.
 */
void RenderTaskApi::GetScreenToFrameBufferMappingActor( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderTask renderTask = GetRenderTask( isolate, args );

  Actor source = renderTask.GetScreenToFrameBufferMappingActor();

  v8::Handle < v8::Object > wrappedActor = ActorWrapper::WrapActor( isolate, source );
  args.GetReturnValue().Set( wrappedActor );
}

/**
 * Retrieve the GL viewport used when rendering.
 *
 * @method getViewport
 * @for RenderTask
 * @return {Object} object with the properties { x,y,width,height }
 */
void RenderTaskApi::GetViewport( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderTask renderTask = GetRenderTask( isolate, args );

  v8::Local<v8::Object> viewportObject = v8::Object::New( isolate );

  Viewport viewPort = renderTask.GetViewport();

  viewportObject->Set( v8::String::NewFromUtf8( isolate, "x"), v8::Number::New( isolate,viewPort.x ));
  viewportObject->Set( v8::String::NewFromUtf8( isolate, "y"), v8::Number::New( isolate,viewPort.y ));
  viewportObject->Set( v8::String::NewFromUtf8( isolate, "width"), v8::Number::New( isolate,viewPort.width ));
  viewportObject->Set( v8::String::NewFromUtf8( isolate, "height"), v8::Number::New( isolate,viewPort.height ));

  args.GetReturnValue().Set( viewportObject );

}

/**
 * Set the GL viewport used when rendering.
 * This specifies the transformation between normalized device coordinates and target window (or frame-buffer) coordinates.
 * By default this will match the target window or frame-buffer size.
 * Unlike the glViewport method, the x & y coordinates refer to the top-left of the viewport rectangle.
 * @method setViewport
 * @for RenderTask
 * @param {Object} object with the properties { x,y,width,height }
 */
void RenderTaskApi::SetViewport( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderTask renderTask = GetRenderTask( isolate, args );

  if( args.Length() != 1 )
  {
     DALI_SCRIPT_EXCEPTION( isolate, "missing / invalid parameters" );
     return;
  }
  v8::Local<v8::Value > viewportValue = args[0];
  if( !viewportValue->IsObject() )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid parameters" );
    return;
  }
  v8::Local<v8::Object> obj = viewportValue->ToObject();
  v8::Local<v8::Value> xValue = obj->Get( v8::String::NewFromUtf8( isolate, "x" ) );
  v8::Local<v8::Value> yValue = obj->Get( v8::String::NewFromUtf8( isolate, "y" ) );
  v8::Local<v8::Value> widthValue = obj->Get( v8::String::NewFromUtf8( isolate, "width" ) );
  v8::Local<v8::Value> heightValue = obj->Get( v8::String::NewFromUtf8( isolate, "height" ) );

  if( xValue->IsNumber() &&  yValue->IsNumber() &&   widthValue->IsNumber() &&  heightValue->IsNumber())
  {
    Viewport viewPort;
    viewPort.x= xValue->ToNumber()->Value();
    viewPort.y = yValue->ToNumber()->Value();
    viewPort.width = widthValue->ToNumber()->Value();
    viewPort.height = heightValue->ToNumber()->Value();
    renderTask.SetViewport( viewPort );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "missing x,y,width,height property" );
    return;
  }
}

/**
 * Set whether the render-task will clear the results of previous render-tasks.
 *
 * The default is false.
 *
 * The default GL surface is cleared automatically at the
 * beginning of each frame; this setting is only useful when 2+
 * render-tasks are used, and the result of the first task needs to
 * be (partially) cleared before rendering the second. * @method setViewport
 * @for RenderTask
 * @method setClearEnabled
 * @param {Boolean} enabled True if the render-task should clear
 */
void RenderTaskApi::SetClearEnabled( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderTask renderTask = GetRenderTask( isolate, args );

  bool found( false );
  bool clearEnabled = V8Utils::GetBooleanParameter( PARAMETER_0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "boolean parameter missing" );
    return;
  }
  renderTask.SetClearEnabled( clearEnabled );

}

/**
 * Query whether the render-task will clear the results of previous render-tasks.
 *
 * @for RenderTask
 * @method getClearEnabled
 * @return {Boolean} True if the render-task should clear.
 */
void RenderTaskApi::GetClearEnabled( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderTask renderTask = GetRenderTask( isolate, args );

  args.GetReturnValue().Set( v8::Boolean::New( isolate, renderTask.GetClearEnabled() ) );
}

/**
 * Set whether the render task will cull the actors to the camera's view frustum.
 * Note that this will only affect image actors that use the default vertex shader.
 * The default mode is to cull actors.
 * @for RenderTask
 * @method setCullMode
 * @param {Boolean}  cullMode True if the renderers should be culled.
 */
void RenderTaskApi::SetCullMode ( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderTask renderTask = GetRenderTask( isolate, args );

  bool found( false );
  bool cullMode = V8Utils::GetBooleanParameter( PARAMETER_0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "boolean parameter missing" );
    return;
  }
  renderTask.SetCullMode( cullMode );
}

/**
 * Get the cull mode
 * @for RenderTask
 * @method getCullMode
 * @return {Boolean}  cullMode True if the renderers should be culled.
 */
void RenderTaskApi::GetCullMode( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderTask renderTask = GetRenderTask( isolate, args );

  args.GetReturnValue().Set( v8::Boolean::New( isolate, renderTask.GetCullMode() ) );
}

/**
 * Set the refresh-rate of the RenderTask.
 *
 * The default is dali.REFRESH_ALWAYS (1), meaning that the RenderTask will be processed every frame.
 * It may be desirable to process less frequently e.g. SetRefreshRate(3) will process once every 3 frames.
 * The dali.REFRESH_ONCE value means that the RenderTask will be processed once only, to take a snap-shot of the scene.
 * Repeatedly calling setRefreshRate(dali.REFRESH_ONCE) will cause more snap-shots to be taken.
 * @method setRefreshRate
 * @param {Integer} refresh rate  (0 =  dali.REFRESH_ONCE, 1= dali.REFRESH_ALWAYS)
 */
void RenderTaskApi::SetRefreshRate( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderTask renderTask = GetRenderTask( isolate, args );

  bool found( false );
  int rate = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 );
  if( !found )
  {
     DALI_SCRIPT_EXCEPTION( isolate, "Integer parameter missing" );
     return;
  }
  renderTask.SetRefreshRate( rate );
}

/**
 *  Query the refresh-rate of the RenderTask.
 * @method getRefreshRate
 * @return {Integer} refresh rate  (0 =  dali.REFRESH_ONCE, 1= dali.REFRESH_ALWAYS)
 */
void RenderTaskApi::GetRefreshRate( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderTask renderTask = GetRenderTask( isolate, args );

  args.GetReturnValue().Set( v8::Integer::New( isolate, renderTask.GetRefreshRate() ) );
}


} // namespace V8Plugin

} // namespace Dali

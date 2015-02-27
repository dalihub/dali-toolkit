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
#include "render-task-list-api.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <render-tasks/render-task-list-wrapper.h>
#include <render-tasks/render-task-wrapper.h>
#include <render-tasks/render-task-api.h>
#include <object/property-value-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

namespace // un named namespace
{

/**
 *
## RenderTaskList API


An ordered list of Dali::RenderTasks

These tasks describe how the Dali scene should be rendered;
See {{#crossLink "RenderTask"}}{{/crossLink}}

 @class RenderTaskList
*/


RenderTaskList GetRenderTaskList( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate );

  v8::Local<v8::Object> object = args.This();
  v8::Local<v8::External> field = v8::Local<v8::External>::Cast( object->GetInternalField( 0 ) );
  void* ptr = field->Value();

  RenderTaskListWrapper* wrapper = static_cast<RenderTaskListWrapper *>( ptr );
  return wrapper->GetRenderTaskList();
}


} // un-named namespace

/**
 * Create a new RenderTask.
 * This will be appended to the list of render-tasks.
 * @method createTask
 * @for RenderTaskList
 * @return {Object} RenderTask
 */
void RenderTaskListApi::CreateTask( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderTaskList taskList = GetRenderTaskList( isolate, args);

  RenderTask renderTask = taskList.CreateTask();

  v8::Handle<v8::Object> wrappedTask = RenderTaskWrapper::WrapRenderTask(isolate, renderTask);
  args.GetReturnValue().Set( wrappedTask  );

}

/**
 * Remove a RenderTask from the list of render-tasks.
 * @method removeTask
 * @for RenderTaskList
 * @param{Object} RenderTask to remove
 */
void RenderTaskListApi::RemoveTask( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderTaskList taskList = GetRenderTaskList( isolate, args );

  bool found( false );
  RenderTask renderTask = V8Utils::GetRenderTaskParameter( PARAMETER_0, found, isolate, args );
  if(! found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "render-task parameter missing" );
    return;
  }
  taskList.RemoveTask( renderTask );
}
/**
 * Query the number of render-tasks in the list.
 * This is ordered i.e. the task with index 0 is the first to be processed each frame.
 * @method getTaskCount
 * @for RenderTaskList
 * @return {Number}  The number of render-tasks.
 */
void RenderTaskListApi::GetTaskCount( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderTaskList taskList = GetRenderTaskList( isolate, args );

  args.GetReturnValue().Set( v8::Integer::New( isolate, taskList.GetTaskCount() ) );

}
/**
 * Retrieve a render-task.
 * index should be in range i.e. less than GetTaskCount().
 * @method getTask
 * @for RenderTaskList
 * @param {Number} index The index of the render task to retrieve
 * @return {Object}  The render-task
 */
void RenderTaskListApi::GetTask( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  RenderTaskList taskList = GetRenderTaskList( isolate, args );

  bool found( false );
  int index = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 );
  if( !found )
  {
     DALI_SCRIPT_EXCEPTION( isolate, "Integer parameter missing" );
     return;
  }
  RenderTask renderTask = taskList.GetTask( index );

  v8::Handle<v8::Object> wrappedTask = RenderTaskWrapper::WrapRenderTask(isolate, renderTask);
  args.GetReturnValue().Set( wrappedTask  );
}

} // namespace V8Plugin

} // namespace Dali

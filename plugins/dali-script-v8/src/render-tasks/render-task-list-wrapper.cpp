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
#include "render-task-list-wrapper.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <dali-wrapper.h>
#include <render-tasks/render-task-list-api.h>
#include <shared/api-function.h>
#include <shared/object-template-helper.h>

namespace Dali
{
namespace V8Plugin
{

v8::Persistent<v8::ObjectTemplate> RenderTaskListWrapper::mRenderTaskListTemplate;

namespace
{

/**
 * Contains a list of all functions that can be called
 */
const ApiFunction RenderTaskListFunctionTable[]=
{
    /**************************************
    * RenderTaskList API (in order of renderTaskList.h)
    **************************************/

    { "CreateTask"             , RenderTaskListApi::CreateTask     },
    { "RemoveTask"             , RenderTaskListApi::RemoveTask     },
    { "GetTaskCount"           , RenderTaskListApi::GetTaskCount   },
    { "GetTask"                , RenderTaskListApi::GetTask        },


};

const unsigned int RenderTaskListFunctionTableCount = sizeof(RenderTaskListFunctionTable)/sizeof(RenderTaskListFunctionTable[0]);
} //un-named space


RenderTaskListWrapper::RenderTaskListWrapper( const Dali::RenderTaskList& renderTaskList, GarbageCollectorInterface& gc )
: BaseWrappedObject( BaseWrappedObject::RENDER_TASK_LIST , gc )
{
    mRenderTaskList = renderTaskList;
}

RenderTaskListWrapper::~RenderTaskListWrapper()
{

}

v8::Handle<v8::Object> RenderTaskListWrapper::WrapRenderTaskList(v8::Isolate* isolate, const Dali::RenderTaskList& renderTaskList )
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  objectTemplate = GetRenderTaskListTemplate( isolate);

  // create an instance of the template
  v8::Local<v8::Object> localObject = objectTemplate->NewInstance();

  // create the RenderTaskList wrapper
  RenderTaskListWrapper* pointer =  new RenderTaskListWrapper( renderTaskList, Dali::V8Plugin::DaliWrapper::Get().GetDaliGarbageCollector() );

  // assign the JavaScript object to the wrapper.
  pointer->SetJavascriptObject( isolate, localObject );

  return handleScope.Escape( localObject );
}

v8::Local<v8::ObjectTemplate> RenderTaskListWrapper::GetRenderTaskListTemplate( v8::Isolate* isolate)
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  if( mRenderTaskListTemplate.IsEmpty() )
  {
    objectTemplate = MakeRenderTaskListTemplate( isolate );
    mRenderTaskListTemplate.Reset( isolate, objectTemplate );
  }
  else
  {
    // get the object template
    objectTemplate = v8::Local<v8::ObjectTemplate>::New( isolate, mRenderTaskListTemplate );
  }
  return handleScope.Escape( objectTemplate );
}

v8::Handle<v8::ObjectTemplate> RenderTaskListWrapper::MakeRenderTaskListTemplate( v8::Isolate* isolate )
{
  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::ObjectTemplate> objTemplate = v8::ObjectTemplate::New();

  // add intercepts for Signals, we can't use HandleWrapper::AddIntercepts because RenderTaskList doesn't inherit
  // from Handle ( just baseHandle)
  ObjectTemplateHelper::AddSignalConnectAndDisconnect( isolate, objTemplate );

  objTemplate->SetInternalFieldCount( BaseWrappedObject::FIELD_COUNT );

  // add our function properties
  ObjectTemplateHelper::InstallFunctions( isolate, objTemplate, RenderTaskListFunctionTable, RenderTaskListFunctionTableCount );

  return handleScope.Escape( objTemplate );
}

//  v8::Local<v8::Object> localObject = WrapRenderTaskList( isolate, renderTaskList );
 // args.GetReturnValue().Set( localObject );



RenderTaskList RenderTaskListWrapper::GetRenderTaskList()
{
  return mRenderTaskList;
}


} // namespace V8Plugin

} // namespace Dali

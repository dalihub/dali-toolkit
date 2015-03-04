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
#include "render-task-wrapper.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <render-tasks/render-task-api.h>
#include <shared/api-function.h>
#include <shared/object-template-helper.h>
#include <signals/signal-manager.h>
#include <dali-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

v8::Persistent<v8::ObjectTemplate> RenderTaskWrapper::mRenderTaskTemplate;

namespace // un-named name space
{

/**
 * Contains a list of all functions that can be called
 */
const ApiFunction RenderTaskFunctionTable[]=
{
    /**************************************
    * RenderTask API (in order of renderTask.h)
    **************************************/

   { "SetSourceActor"             , RenderTaskApi::SetSourceActor },
   { "GetSourceActor"             , RenderTaskApi::GetSourceActor },
   { "SetExclusive"               , RenderTaskApi::SetExclusive },
   { "IsExclusive"                , RenderTaskApi::IsExclusive },
   { "SetInputEnabled"            , RenderTaskApi::SetInputEnabled },
   { "GetInputEnabled"            , RenderTaskApi::GetInputEnabled },
   { "SetCameraActor"             , RenderTaskApi::SetCameraActor },
   { "GetCameraActor"             , RenderTaskApi::GetCameraActor },
   { "SetTargetFrameBuffer"       , RenderTaskApi::SetTargetFrameBuffer },
   { "GetTargetFrameBuffer"        , RenderTaskApi::GetTargetFrameBuffer },
   { "SetScreenToFrameBufferMappingActor" , RenderTaskApi::SetScreenToFrameBufferMappingActor },
   { "GetScreenToFrameBufferMappingActor" , RenderTaskApi::GetScreenToFrameBufferMappingActor },

   // USE viewportPosition property "SetViewportPosition" , RenderTaskApi::SetViewportPosition },
   // USE viewportPosition property{ "GetCurrentViewportPosition" , RenderTaskApi::GetCurrentViewportPosition },
   // USE viewportSize property { "SetViewportSize" ,       RenderTaskApi::SetViewportSize },
   // USE viewportSize property { "SetViewportSize" ,       RenderTaskApi::SetViewportSize },

   { "GetViewport" ,                RenderTaskApi::GetViewport },
   { "SetViewport" ,                RenderTaskApi::SetViewport },

   // USE clearColor property, { "SetClearColor" ,        RenderTaskApi::SetClearColor },
   // USE clearColor property, // { "GetClearColor" ,     RenderTaskApi::GetClearColor },

   { "SetClearEnabled" ,            RenderTaskApi::SetClearEnabled },
   { "GetClearEnabled" ,            RenderTaskApi::GetClearEnabled },
   { "SetCullMode" ,                RenderTaskApi::SetCullMode },
   { "GetCullMode" ,                RenderTaskApi::GetCullMode },
   { "SetRefreshRate" ,             RenderTaskApi::SetRefreshRate },
   { "GetRefreshRate" ,             RenderTaskApi::GetRefreshRate },

};

const unsigned int RenderTaskFunctionTableCount = sizeof(RenderTaskFunctionTable)/sizeof(RenderTaskFunctionTable[0]);
} //un-named space


RenderTaskWrapper::RenderTaskWrapper( const Dali::RenderTask& renderTask, GarbageCollectorInterface& gc )
:  HandleWrapper(  BaseWrappedObject::RENDER_TASK , renderTask, gc )
{
    mRenderTask = renderTask;
}

v8::Handle<v8::Object> RenderTaskWrapper::WrapRenderTask(v8::Isolate* isolate, const Dali::RenderTask& renderTask )
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  objectTemplate = GetRenderTaskTemplate( isolate);

  // create an instance of the template
  v8::Local<v8::Object> localObject = objectTemplate->NewInstance();

  // create the RenderTask wrapper
  RenderTaskWrapper* pointer =  new RenderTaskWrapper( renderTask, Dali::V8Plugin::DaliWrapper::Get().GetDaliGarbageCollector() );

  // assign the JavaScript object to the wrapper.
  pointer->SetJavascriptObject( isolate, localObject );

  return handleScope.Escape( localObject );
}

v8::Local<v8::ObjectTemplate> RenderTaskWrapper::GetRenderTaskTemplate( v8::Isolate* isolate)
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  if( mRenderTaskTemplate.IsEmpty() )
  {
    objectTemplate = MakeRenderTaskTemplate( isolate );
    mRenderTaskTemplate.Reset( isolate, objectTemplate );
  }
  else
  {
    // get the object template
    objectTemplate = v8::Local<v8::ObjectTemplate>::New( isolate, mRenderTaskTemplate );
  }
  return handleScope.Escape( objectTemplate );
}

v8::Handle<v8::ObjectTemplate> RenderTaskWrapper::MakeRenderTaskTemplate( v8::Isolate* isolate )
{
  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::ObjectTemplate> objTemplate = v8::ObjectTemplate::New();

  // property handle intercepts property getters and setters and signals
  HandleWrapper::AddInterceptsToTemplate( isolate, objTemplate );

  objTemplate->SetInternalFieldCount( BaseWrappedObject::FIELD_COUNT );

  // add our function properties
  ObjectTemplateHelper::InstallFunctions( isolate, objTemplate, RenderTaskFunctionTable, RenderTaskFunctionTableCount );

  return handleScope.Escape( objTemplate );
}


RenderTask RenderTaskWrapper::GetRenderTask()
{
  return mRenderTask;
}


} // namespace V8Plugin

} // namespace Dali

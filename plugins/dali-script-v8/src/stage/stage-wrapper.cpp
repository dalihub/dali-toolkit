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
#include "stage-wrapper.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <dali-wrapper.h>
#include <stage/stage-api.h>
#include <shared/api-function.h>
#include <shared/object-template-helper.h>

namespace Dali
{

namespace V8Plugin
{

namespace
{

/**
 * Contains a list of all functions that can be called on the stage
 */
const ApiFunction StageFunctionTable[]=
{
    /**************************************
    * Stage API (in order of stage.h)
    * Any properties that have accessor functions are ignored to avoid duplication
    **************************************/
    { "Add",                  StageApi::Add       },
    { "Remove",               StageApi::Remove    },
    { "GetSize",              StageApi::GetSize   },
    { "GetRenderTaskList",    StageApi::GetRenderTaskList   },
    { "GetLayerCount",        StageApi::GetLayerCount   },
    { "GetLayer",             StageApi::GetLayer        },
    { "GetRootLayer",         StageApi::GetRootLayer   },
    { "SetBackgroundColor",   StageApi::SetBackgroundColor   },
    { "GetBackgroundColor",   StageApi::GetBackgroundColor   },
    { "GetDpi",               StageApi::GetDpi       },
};

const unsigned int StageFunctionTableCount = sizeof(StageFunctionTable)/sizeof(StageFunctionTable[0]);
} //un-named space


StageWrapper::StageWrapper( const Dali::Stage& stage, GarbageCollectorInterface& gc )
: BaseWrappedObject( BaseWrappedObject::STAGE , gc )
{
    mStage = stage;
}

v8::Handle<v8::Object> StageWrapper::WrapStage(v8::Isolate* isolate, const Dali::Stage& stage )
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  objectTemplate = GetStageTemplate( isolate);

  // create an instance of the template
  v8::Local<v8::Object> localObject = objectTemplate->NewInstance();

  // create the Stage wrapper
  StageWrapper* pointer =  new StageWrapper( stage, Dali::V8Plugin::DaliWrapper::Get().GetDaliGarbageCollector() );

  // assign the JavaScript object to the wrapper.
  pointer->SetJavascriptObject( isolate, localObject );

  return handleScope.Escape( localObject );
}

v8::Local<v8::ObjectTemplate> StageWrapper::GetStageTemplate( v8::Isolate* isolate)
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;
  objectTemplate = MakeStageTemplate( isolate );
  return handleScope.Escape( objectTemplate );
}

v8::Handle<v8::ObjectTemplate> StageWrapper::MakeStageTemplate( v8::Isolate* isolate )
{
  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::ObjectTemplate> objTemplate = v8::ObjectTemplate::New();

  // add intercepts for Signals, we can't use HandleWrapper::AddIntercepts because Stage doesn't inherit
  // from Handle ( just baseHandle)
  ObjectTemplateHelper::AddSignalConnectAndDisconnect( isolate, objTemplate );

  objTemplate->SetInternalFieldCount( BaseWrappedObject::FIELD_COUNT );

  // add our function properties
  ObjectTemplateHelper::InstallFunctions( isolate, objTemplate, StageFunctionTable, StageFunctionTableCount );

  return handleScope.Escape( objTemplate );
}

Stage StageWrapper::GetStage()
{
  return mStage;
}


} // namespace V8Plugin

} // namespace Dali

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
#include "builder-wrapper.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <dali-wrapper.h>
#include <toolkit/builder/builder-api.h>
#include <shared/api-function.h>
#include <shared/object-template-helper.h>

namespace Dali
{
namespace V8Plugin
{
namespace
{

/**
 * Contains a list of all functions that can be called
 */
const ApiFunction BuilderFunctionTable[]=
{
    /**************************************
    * Builder API (in order of builder.h)
    **************************************/

    { "LoadFromString"          , BuilderApi::LoadFromString           },
    { "LoadFromFile"            , BuilderApi::LoadFromFile             },
    { "AddConstants"            , BuilderApi::AddConstants             },
    { "GetConstants"            , BuilderApi::GetConstants             },
    { "CreateAnimation"         , BuilderApi::CreateAnimation          },
    { "Create"                  , BuilderApi::Create                   },
    { "ApplyStyle"              , BuilderApi::ApplyStyle               },
    { "ApplyFromJson"           , BuilderApi::ApplyFromJson            },
    { "AddActors"               , BuilderApi::AddActors                },
    { "CreateRenderTask"        , BuilderApi::CreateRenderTask         },
    { "GetShaderEffect"         , BuilderApi::GetShaderEffect          },
    { "GetFrameBufferImage"     , BuilderApi::GetFrameBufferImage      }
};

const unsigned int BuilderFunctionTableCount = sizeof(BuilderFunctionTable)/sizeof(BuilderFunctionTable[0]);
} //un-named space


BuilderWrapper::BuilderWrapper( const Dali::Toolkit::Builder& builder, GarbageCollectorInterface& gc )
: BaseWrappedObject( BaseWrappedObject::BUILDER , gc )
{
    mBuilder = builder;
}

BuilderWrapper::~BuilderWrapper()
{

}

v8::Handle<v8::Object> BuilderWrapper::WrapBuilder(v8::Isolate* isolate, const Dali::Toolkit::Builder& builder )
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  objectTemplate = GetBuilderTemplate( isolate);

  // create an instance of the template
  v8::Local<v8::Object> localObject = objectTemplate->NewInstance();

  // create the Builder wrapper
  BuilderWrapper* pointer =  new BuilderWrapper( builder, Dali::V8Plugin::DaliWrapper::Get().GetDaliGarbageCollector() );

  // assign the JavaScript object to the wrapper.
  pointer->SetJavascriptObject( isolate, localObject );

  return handleScope.Escape( localObject );
}

v8::Local<v8::ObjectTemplate> BuilderWrapper::GetBuilderTemplate( v8::Isolate* isolate)
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  v8::Local<v8::ObjectTemplate> objTemplate = v8::ObjectTemplate::New();

  objTemplate->SetInternalFieldCount( BaseWrappedObject::FIELD_COUNT );

  // add our function properties
  ObjectTemplateHelper::InstallFunctions( isolate, objTemplate, BuilderFunctionTable, BuilderFunctionTableCount );

  return handleScope.Escape( objTemplate );

}

void BuilderWrapper::NewBuilder( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate);

  if( !args.IsConstructCall() )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "Builder constructor called without 'new'" );
    return;
  }

  Dali::Toolkit::Builder builder = BuilderApi::New( args );
  v8::Local<v8::Object> localObject = WrapBuilder( isolate, builder );
  args.GetReturnValue().Set( localObject );
}


Dali::Toolkit::Builder BuilderWrapper::GetBuilder()
{
  return mBuilder;
}


} // namespace V8Plugin

} // namespace Dali

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
#include "sampler-wrapper.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <dali-wrapper.h>
#include <shared/api-function.h>
#include <shared/object-template-helper.h>
#include <rendering/sampler-api.h>

namespace Dali
{
namespace V8Plugin
{

v8::Persistent<v8::ObjectTemplate> SamplerWrapper::mSamplerTemplate;

namespace
{

/**
 * Contains a list of all functions that can be called
 */
const ApiFunction SamplerFunctionTable[]=
{
    /**************************************
    * Sampler API (in order of sampler.h)
    **************************************/

    { "SetFilterMode",           SamplerApi::SetFilterMode },
    { "SetWrapMode",             SamplerApi::SetWrapMode   },
};

const unsigned int SamplerFunctionTableCount = sizeof(SamplerFunctionTable)/sizeof(SamplerFunctionTable[0]);
} //un-named space


SamplerWrapper::SamplerWrapper( const Sampler& sampler, GarbageCollectorInterface& gc )
: BaseWrappedObject( BaseWrappedObject::SAMPLER , gc )
{
    mSampler = sampler;
}

v8::Handle<v8::Object> SamplerWrapper::WrapSampler(v8::Isolate* isolate, const Sampler& sampler )
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  objectTemplate = GetSamplerTemplate( isolate);

  // create an instance of the template
  v8::Local<v8::Object> localObject = objectTemplate->NewInstance();

  // create the Sampler wrapper
  SamplerWrapper* pointer =  new SamplerWrapper( sampler, Dali::V8Plugin::DaliWrapper::Get().GetDaliGarbageCollector() );

  // assign the JavaScript object to the wrapper.
  pointer->SetJavascriptObject( isolate, localObject );

  return handleScope.Escape( localObject );
}

v8::Local<v8::ObjectTemplate> SamplerWrapper::GetSamplerTemplate( v8::Isolate* isolate)
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  if( mSamplerTemplate.IsEmpty() )
  {
    objectTemplate = MakeSamplerTemplate( isolate );
    mSamplerTemplate.Reset( isolate, objectTemplate );
  }
  else
  {
    // get the object template
    objectTemplate = v8::Local<v8::ObjectTemplate>::New( isolate, mSamplerTemplate );
  }
  return handleScope.Escape( objectTemplate );
}

v8::Handle<v8::ObjectTemplate> SamplerWrapper::MakeSamplerTemplate( v8::Isolate* isolate )
{
  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::ObjectTemplate> objTemplate = v8::ObjectTemplate::New();

  objTemplate->SetInternalFieldCount( BaseWrappedObject::FIELD_COUNT );

  // add our function properties
  ObjectTemplateHelper::InstallFunctions( isolate, objTemplate, SamplerFunctionTable, SamplerFunctionTableCount );

  return handleScope.Escape( objTemplate );
}

void SamplerWrapper::NewSampler( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate);

  if(!args.IsConstructCall())
  {
      DALI_SCRIPT_EXCEPTION( isolate, "Sampler constructor called without 'new'");
      return;
  }
  Dali::Sampler sampler = SamplerApi::New( args );

  if(sampler)
  {
    v8::Local<v8::Object> localObject = WrapSampler( isolate, sampler );
    args.GetReturnValue().Set( localObject );
  }
}

Sampler SamplerWrapper::GetSampler()
{
  return mSampler;
}


} // namespace V8Plugin

} // namespace Dali

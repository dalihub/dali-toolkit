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
#include "property-buffer-wrapper.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <object/property-buffer-api.h>
#include <shared/api-function.h>
#include <shared/object-template-helper.h>
#include <dali-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

v8::Persistent<v8::ObjectTemplate> PropertyBufferWrapper::mPropertyBufferTemplate;

namespace // un-named name space
{

/**
 * Contains a list of all functions that can be called
 */
const ApiFunction PropertyBufferFunctionTable[]=
{
    /**************************************
    * PropertyBuffer API (in order of property-buffer.h)
    **************************************/

   { "SetData"             , PropertyBufferApi::SetData },
};

const unsigned int PropertyBufferFunctionTableCount = sizeof(PropertyBufferFunctionTable)/sizeof(PropertyBufferFunctionTable[0]);
} //un-named space


PropertyBufferWrapper::PropertyBufferWrapper( const Dali::PropertyBuffer& buffer, GarbageCollectorInterface& gc )
:  BaseWrappedObject(  BaseWrappedObject::PROPERTY_BUFFER , gc )
{
    mPropertyBuffer = buffer;
}

v8::Handle<v8::Object> PropertyBufferWrapper::WrapPropertyBuffer(v8::Isolate* isolate, const Dali::PropertyBuffer& buffer )
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  objectTemplate = GetPropertyBufferTemplate( isolate);

  // create an instance of the template
  v8::Local<v8::Object> localObject = objectTemplate->NewInstance();

  // create the PropertyBuffer wrapper
  PropertyBufferWrapper* pointer =  new PropertyBufferWrapper( buffer, Dali::V8Plugin::DaliWrapper::Get().GetDaliGarbageCollector() );

  // assign the JavaScript object to the wrapper.
  pointer->SetJavascriptObject( isolate, localObject );

  return handleScope.Escape( localObject );
}

v8::Local<v8::ObjectTemplate> PropertyBufferWrapper::GetPropertyBufferTemplate( v8::Isolate* isolate)
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  if( mPropertyBufferTemplate.IsEmpty() )
  {
    objectTemplate = MakePropertyBufferTemplate( isolate );
    mPropertyBufferTemplate.Reset( isolate, objectTemplate );
  }
  else
  {
    // get the object template
    objectTemplate = v8::Local<v8::ObjectTemplate>::New( isolate, mPropertyBufferTemplate );
  }
  return handleScope.Escape( objectTemplate );
}

v8::Handle<v8::ObjectTemplate> PropertyBufferWrapper::MakePropertyBufferTemplate( v8::Isolate* isolate )
{
  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::ObjectTemplate> objTemplate = v8::ObjectTemplate::New();

  objTemplate->SetInternalFieldCount( BaseWrappedObject::FIELD_COUNT );

  // add our function properties
  ObjectTemplateHelper::InstallFunctions( isolate, objTemplate, PropertyBufferFunctionTable, PropertyBufferFunctionTableCount );

  return handleScope.Escape( objTemplate );
}

void PropertyBufferWrapper::NewPropertyBuffer( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate);

  if(!args.IsConstructCall())
  {
      DALI_SCRIPT_EXCEPTION( isolate, "PropertyBuffer constructor called without 'new'");
      return;
  }
  Dali::PropertyBuffer buffer = PropertyBufferApi::New( isolate, args );

  if(buffer)
  {
    v8::Local<v8::Object> localObject = WrapPropertyBuffer( isolate, buffer );
    args.GetReturnValue().Set( localObject );
  }
}


PropertyBuffer PropertyBufferWrapper::GetPropertyBuffer()
{
  return mPropertyBuffer;
}


} // namespace V8Plugin

} // namespace Dali

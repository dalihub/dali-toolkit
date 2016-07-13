/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include "geometry-wrapper.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <rendering/geometry-api.h>
#include <shared/api-function.h>
#include <shared/object-template-helper.h>
#include <dali-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

v8::Persistent<v8::ObjectTemplate> GeometryWrapper::mGeometryTemplate;

namespace // un-named name space
{

/**
 * Contains a list of all functions that can be called
 */
const ApiFunction GeometryFunctionTable[]=
{
    /**************************************
    * Geometry API (in order of Geometry.h)
    **************************************/

   { "AddVertexBuffer"             , GeometryApi::AddVertexBuffer },
   { "GetNumberOfVertexBuffers"    , GeometryApi::GetNumberOfVertexBuffers },
   { "RemoveVertexBuffer"          , GeometryApi::RemoveVertexBuffer },
   { "SetIndexBuffer"              , GeometryApi::SetIndexBuffer },
   { "SetGeometryType"             , GeometryApi::SetGeometryType },
   { "GetGeometryType"             , GeometryApi::GetGeometryType },
};

const unsigned int GeometryFunctionTableCount = sizeof(GeometryFunctionTable)/sizeof(GeometryFunctionTable[0]);
} //un-named space


GeometryWrapper::GeometryWrapper( const Dali::Geometry& geometry, GarbageCollectorInterface& gc )
:  BaseWrappedObject(  BaseWrappedObject::GEOMETRY, gc )
{
    mGeometry = geometry;
}

v8::Handle<v8::Object> GeometryWrapper::WrapGeometry(v8::Isolate* isolate, const Dali::Geometry& geometry )
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  objectTemplate = GetGeometryTemplate( isolate);

  // create an instance of the template
  v8::Local<v8::Object> localObject = objectTemplate->NewInstance();

  // create the Geometry wrapper
  GeometryWrapper* pointer =  new GeometryWrapper( geometry, Dali::V8Plugin::DaliWrapper::Get().GetDaliGarbageCollector() );

  // assign the JavaScript object to the wrapper.
  pointer->SetJavascriptObject( isolate, localObject );

  return handleScope.Escape( localObject );
}

v8::Local<v8::ObjectTemplate> GeometryWrapper::GetGeometryTemplate( v8::Isolate* isolate)
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  if( mGeometryTemplate.IsEmpty() )
  {
    objectTemplate = MakeGeometryTemplate( isolate );
    mGeometryTemplate.Reset( isolate, objectTemplate );
  }
  else
  {
    // get the object template
    objectTemplate = v8::Local<v8::ObjectTemplate>::New( isolate, mGeometryTemplate );
  }
  return handleScope.Escape( objectTemplate );
}

v8::Handle<v8::ObjectTemplate> GeometryWrapper::MakeGeometryTemplate( v8::Isolate* isolate )
{
  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::ObjectTemplate> objTemplate = v8::ObjectTemplate::New();

  objTemplate->SetInternalFieldCount( BaseWrappedObject::FIELD_COUNT );

  // add our function properties
  ObjectTemplateHelper::InstallFunctions( isolate, objTemplate, GeometryFunctionTable, GeometryFunctionTableCount );

  return handleScope.Escape( objTemplate );
}

void GeometryWrapper::NewGeometry( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate);

  if(!args.IsConstructCall())
  {
      DALI_SCRIPT_EXCEPTION( isolate, "Geometry constructor called without 'new'");
      return;
  }
  Dali::Geometry geometry = GeometryApi::New( args );

  if(geometry)
  {
    v8::Local<v8::Object> localObject = WrapGeometry( isolate, geometry );
    args.GetReturnValue().Set( localObject );
  }
}


Geometry GeometryWrapper::GetGeometry()
{
  return mGeometry;
}


} // namespace V8Plugin

} // namespace Dali

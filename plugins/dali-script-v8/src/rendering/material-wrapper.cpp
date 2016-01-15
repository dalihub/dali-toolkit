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
#include "material-wrapper.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <rendering/material-api.h>
#include <shared/api-function.h>
#include <shared/object-template-helper.h>
#include <dali-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

v8::Persistent<v8::ObjectTemplate> MaterialWrapper::mMaterialTemplate;

namespace // un-named name space
{

/**
 * Contains a list of all functions that can be called
 */
const ApiFunction MaterialFunctionTable[]=
{
    /**************************************
    * Material API (in order of Material.h)
    **************************************/

   { "SetShader"                       , MaterialApi::SetShader },
   { "GetShader"                       , MaterialApi::GetShader },
   { "AddTexture"                      , MaterialApi::AddTexture },
   { "RemoveTexture"                   , MaterialApi::RemoveTexture },
   { "SetTextureImage"                 , MaterialApi::SetTextureImage },
   { "SetTextureSampler"               , MaterialApi::SetTextureSampler },
   { "SetTextureUniformName"           , MaterialApi::SetTextureUniformName },
   { "GetTextureIndex"                 , MaterialApi::GetTextureIndex },
   { "GetNumberOfTextures"             , MaterialApi::GetNumberOfTextures },
   { "SetFaceCullingMode"              , MaterialApi::SetFaceCullingMode },
   { "SetBlendMode"                    , MaterialApi::SetBlendMode },
   { "GetBlendMode"                    , MaterialApi::GetBlendMode },
   { "SetBlendFunc"                    , MaterialApi::SetBlendFunc },
   { "GetBlendFunc"                    , MaterialApi::GetBlendFunc },
   { "SetBlendEquation"                , MaterialApi::SetBlendEquation },
   { "GetBlendEquation"                , MaterialApi::GetBlendEquation },
};

const unsigned int MaterialFunctionTableCount = sizeof(MaterialFunctionTable)/sizeof(MaterialFunctionTable[0]);
} //un-named space


MaterialWrapper::MaterialWrapper( const Dali::Material& material, GarbageCollectorInterface& gc )
:  HandleWrapper(  BaseWrappedObject::MATERIAL , material, gc )
{
    mMaterial = material;
}

v8::Handle<v8::Object> MaterialWrapper::WrapMaterial(v8::Isolate* isolate, const Dali::Material& material )
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  objectTemplate = GetMaterialTemplate( isolate);

  // create an instance of the template
  v8::Local<v8::Object> localObject = objectTemplate->NewInstance();

  // create the Material wrapper
  MaterialWrapper* pointer =  new MaterialWrapper( material, Dali::V8Plugin::DaliWrapper::Get().GetDaliGarbageCollector() );

  // assign the JavaScript object to the wrapper.
  pointer->SetJavascriptObject( isolate, localObject );

  return handleScope.Escape( localObject );
}

v8::Local<v8::ObjectTemplate> MaterialWrapper::GetMaterialTemplate( v8::Isolate* isolate)
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  if( mMaterialTemplate.IsEmpty() )
  {
    objectTemplate = MakeMaterialTemplate( isolate );
    mMaterialTemplate.Reset( isolate, objectTemplate );
  }
  else
  {
    // get the object template
    objectTemplate = v8::Local<v8::ObjectTemplate>::New( isolate, mMaterialTemplate );
  }
  return handleScope.Escape( objectTemplate );
}

v8::Handle<v8::ObjectTemplate> MaterialWrapper::MakeMaterialTemplate( v8::Isolate* isolate )
{
  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::ObjectTemplate> objTemplate = v8::ObjectTemplate::New();

  // property handle intercepts property getters and setters and signals
  HandleWrapper::AddInterceptsToTemplate( isolate, objTemplate );

  objTemplate->SetInternalFieldCount( BaseWrappedObject::FIELD_COUNT );

  // add our function properties
  ObjectTemplateHelper::InstallFunctions( isolate, objTemplate, MaterialFunctionTable, MaterialFunctionTableCount );

  return handleScope.Escape( objTemplate );
}

void MaterialWrapper::NewMaterial( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate);

  if(!args.IsConstructCall())
  {
      DALI_SCRIPT_EXCEPTION( isolate, "Material constructor called without 'new'");
      return;
  }
  Dali::Material material = MaterialApi::New( args );

  if(material)
  {
    v8::Local<v8::Object> localObject = WrapMaterial( isolate, material );
    args.GetReturnValue().Set( localObject );
  }
}


Material MaterialWrapper::GetMaterial()
{
  return mMaterial;
}


} // namespace V8Plugin

} // namespace Dali

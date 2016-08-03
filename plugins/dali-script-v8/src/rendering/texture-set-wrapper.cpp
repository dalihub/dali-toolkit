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
#include "texture-set-wrapper.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <rendering/texture-set-api.h>
#include <shared/api-function.h>
#include <shared/object-template-helper.h>
#include <dali-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

v8::Persistent<v8::ObjectTemplate> TextureSetWrapper::mTextureSetTemplate;

namespace // un-named name space
{

/**
 * Contains a list of all functions that can be called
 */
const ApiFunction TextureSetFunctionTable[]=
{
    /**************************************
    * TextureSet API (in order of texture-set.h)
    **************************************/

   { "SetSampler"                      , TextureSetApi::SetSampler },
};

const unsigned int TextureSetFunctionTableCount = sizeof(TextureSetFunctionTable)/sizeof(TextureSetFunctionTable[0]);
} //un-named space


TextureSetWrapper::TextureSetWrapper( const Dali::TextureSet& textureSet, GarbageCollectorInterface& gc )
:  BaseWrappedObject(  BaseWrappedObject::TEXTURE_SET, gc )
{
    mTextureSet = textureSet;
}

v8::Handle<v8::Object> TextureSetWrapper::WrapTextureSet(v8::Isolate* isolate, const Dali::TextureSet& textureSet )
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  objectTemplate = GetTextureSetTemplate( isolate);

  // create an instance of the template
  v8::Local<v8::Object> localObject = objectTemplate->NewInstance();

  // create the texture set wrapper
  TextureSetWrapper* pointer =  new TextureSetWrapper( textureSet, Dali::V8Plugin::DaliWrapper::Get().GetDaliGarbageCollector() );

  // assign the JavaScript object to the wrapper.
  pointer->SetJavascriptObject( isolate, localObject );

  return handleScope.Escape( localObject );
}

v8::Local<v8::ObjectTemplate> TextureSetWrapper::GetTextureSetTemplate( v8::Isolate* isolate)
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  if( mTextureSetTemplate.IsEmpty() )
  {
    objectTemplate = MakeTextureSetTemplate( isolate );
    mTextureSetTemplate.Reset( isolate, objectTemplate );
  }
  else
  {
    // get the object template
    objectTemplate = v8::Local<v8::ObjectTemplate>::New( isolate, mTextureSetTemplate );
  }
  return handleScope.Escape( objectTemplate );
}

v8::Handle<v8::ObjectTemplate> TextureSetWrapper::MakeTextureSetTemplate( v8::Isolate* isolate )
{
  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::ObjectTemplate> objTemplate = v8::ObjectTemplate::New();

  objTemplate->SetInternalFieldCount( BaseWrappedObject::FIELD_COUNT );

  // add our function properties
  ObjectTemplateHelper::InstallFunctions( isolate, objTemplate, TextureSetFunctionTable, TextureSetFunctionTableCount );

  return handleScope.Escape( objTemplate );
}

void TextureSetWrapper::NewTextureSet( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate);

  if(!args.IsConstructCall())
  {
      DALI_SCRIPT_EXCEPTION( isolate, "TextureSet constructor called without 'new'");
      return;
  }
  Dali::TextureSet textureSet = TextureSetApi::New( args );

  if(textureSet)
  {
    v8::Local<v8::Object> localObject = WrapTextureSet( isolate, textureSet );
    args.GetReturnValue().Set( localObject );
  }
}


TextureSet TextureSetWrapper::GetTextureSet()
{
  return mTextureSet;
}


} // namespace V8Plugin

} // namespace Dali

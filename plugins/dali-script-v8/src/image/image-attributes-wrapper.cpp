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
#include "image-attributes-wrapper.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <image/image-attributes-api.h>
#include <dali-wrapper.h>
#include <shared/api-function.h>
#include <shared/object-template-helper.h>

namespace Dali
{

namespace V8Plugin
{

v8::Persistent<v8::ObjectTemplate> ImageAttributesWrapper::mImageAttributesTemplate;

namespace
{

/**
 * Contains a list of all functions that can be called
 */
const ApiFunction ImageAttributeFunctionTable[]=
{
    /**************************************
    * ImageAttributes API (in order of image-attributes.h)
    **************************************/
    { "SetSize",                    ImageAttributesApi::SetSize },
    { "SetScalingMode",             ImageAttributesApi::SetScalingMode },
    { "SetOrientationCorrection",   ImageAttributesApi::SetOrientationCorrection },
    { "GetWidth",                   ImageAttributesApi::GetWidth },
    { "GetHeight",                  ImageAttributesApi::GetHeight },
    { "GetSize",                    ImageAttributesApi::GetSize },
    { "GetScalingMode",             ImageAttributesApi::GetScalingMode },
    { "GetOrientationCorrection",   ImageAttributesApi::GetOrientationCorrection },
};

const unsigned int ImageAttributeFunctionTableCount = sizeof(ImageAttributeFunctionTable)/sizeof(ImageAttributeFunctionTable[0]);
} //un-named space


ImageAttributesWrapper::ImageAttributesWrapper( const Dali::ImageAttributes& imageAttributes, GarbageCollectorInterface& gc )
: BaseWrappedObject( BaseWrappedObject::IMAGE_ATTRIBUTES , gc )
{
    mImageAttributes = imageAttributes;
}

v8::Handle<v8::Object> ImageAttributesWrapper::WrapImageAttributes(v8::Isolate* isolate, const Dali::ImageAttributes& attributes )
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  objectTemplate = GetImageAttributesTemplate( isolate);

  // create an instance of the template
  v8::Local<v8::Object> localObject = objectTemplate->NewInstance();

  // create the ImageAttributes wrapper
  ImageAttributesWrapper* pointer =  new ImageAttributesWrapper( attributes, Dali::V8Plugin::DaliWrapper::Get().GetDaliGarbageCollector() );

  // assign the JavaScript object to the wrapper.
  pointer->SetJavascriptObject( isolate, localObject );

  return handleScope.Escape( localObject );
}

v8::Local<v8::ObjectTemplate> ImageAttributesWrapper::GetImageAttributesTemplate( v8::Isolate* isolate)
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;


  if( mImageAttributesTemplate.IsEmpty() )
  {
    objectTemplate = MakeImageAttributesTemplate( isolate );
    mImageAttributesTemplate.Reset( isolate, objectTemplate );
  }
  else
  {
    // get the object template
    objectTemplate = v8::Local<v8::ObjectTemplate>::New( isolate, mImageAttributesTemplate );
  }
  return handleScope.Escape( objectTemplate );
}

v8::Handle<v8::ObjectTemplate> ImageAttributesWrapper::MakeImageAttributesTemplate( v8::Isolate* isolate )
{
  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::ObjectTemplate> objTemplate = v8::ObjectTemplate::New();

  objTemplate->SetInternalFieldCount( BaseWrappedObject::FIELD_COUNT );

  // add our function properties
  ObjectTemplateHelper::InstallFunctions( isolate, objTemplate, ImageAttributeFunctionTable, ImageAttributeFunctionTableCount );

  return handleScope.Escape( objTemplate );
}

void ImageAttributesWrapper::NewImageAttributes( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate);

  if(!args.IsConstructCall())
  {
      DALI_SCRIPT_EXCEPTION(isolate, "ImageAttributes constructor called without 'new'");
      return;
  }

  // attribs can be passed by value
  Dali::ImageAttributes attribs = ImageAttributesApi::New( args );
  v8::Local<v8::Object> localObject = WrapImageAttributes( isolate, attribs );
  args.GetReturnValue().Set( localObject );
}


ImageAttributes& ImageAttributesWrapper::GetImageAttributes()
{
  return mImageAttributes;
}


} // namespace V8Plugin

} // namespace Dali

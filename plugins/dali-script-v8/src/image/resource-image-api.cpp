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
#include "resource-image-api.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <image/image-wrapper.h>
#include <image/image-attributes-wrapper.h>
#include <image/image-attributes-api.h>


namespace Dali
{

namespace V8Plugin
{

ResourceImage ResourceImageApi::GetResourceImage( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate );

  v8::Local<v8::Object> object = args.This();
  v8::Local<v8::External> field = v8::Local<v8::External>::Cast( object->GetInternalField(0) );
  void* ptr = field->Value();

  ImageWrapper* wrapper = static_cast< ImageWrapper *>(ptr);
  return ResourceImage::DownCast( wrapper->GetImage() );
}

/**
 * Create a new resource image object.
 *
 * @constructor
 * @method ResourceImage
 * @for ResourceImage
 * @param {Object} options
 * @param {String} options.url The URL of the image file to use.
 * @param {Object} [option.imageAttributes] image attributes object
 * @param {Integer} [options.loadPolicy] The LoadPolicy to apply when loading the image resource.
 * @param {Integer} [options.releasePolicy] optionally release memory when image is not visible on screen.
 * @return {Object} Image
 */
Image ResourceImageApi::New( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  std::string url;
  ResourceImage::LoadPolicy loadPolicy( ResourceImage::IMMEDIATE );
  Image::ReleasePolicy releasePolicy( Image::NEVER);
  ImageAttributes imageAttributes;
  bool useImageAttributes = false;

  v8::Local<v8::Value> options( args[0] );

  if( !options->IsObject() )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "Missing params" );
    return Image();
  }

  v8::Local<v8::Object> optionsObject = options->ToObject();

  v8::Local<v8::Value> urlValue = optionsObject->Get( v8::String::NewFromUtf8( isolate, "url" ) );
  if( urlValue->IsString() )
  {
    url = V8Utils::v8StringToStdString( urlValue );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "Missing url");
    return Image();
  }

  v8::Local<v8::Value> imageAttribsValue= optionsObject->Get( v8::String::NewFromUtf8( isolate, "imageAttributes" ) );
  if( imageAttribsValue->IsObject() )
  {
    imageAttributes = ImageAttributesApi::GetImageAttributesFromObject( isolate, imageAttribsValue->ToObject() );
  }



  v8::Local<v8::Value> releasePolicyValue = optionsObject->Get( v8::String::NewFromUtf8( isolate, "releasePolicy" ) );
  if( releasePolicyValue->IsUint32() )
  {
    releasePolicy = static_cast<Image::ReleasePolicy>( releasePolicyValue->ToUint32()->Value() );
  }

  v8::Local<v8::Value> loadPolicyValue = optionsObject->Get( v8::String::NewFromUtf8( isolate, "loadPolicy" ) );
  if( loadPolicyValue->IsUint32() )
  {
    loadPolicy = static_cast< ResourceImage::LoadPolicy >( loadPolicyValue->ToUint32()->Value());
  }

  if( useImageAttributes )
  {
    return ResourceImage::New( url, imageAttributes, loadPolicy, releasePolicy);
  }
  else
  {
    return ResourceImage::New( url, loadPolicy, releasePolicy);
  }
}

/**
 * Get the load policy
 *
 * @method getLoadPolicy
 * @for ResourceImage
 * @return {Integer} load policy either dali.IMAGE_LOAD_POLICY_ON_DEMAND or dali.IMAGE_LOAD_POLICY_IMMEDIATE
 */
void ResourceImageApi::GetLoadPolicy( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  ResourceImage image = GetResourceImage( isolate, args );

  args.GetReturnValue().Set( v8::Integer::New( isolate, image.GetLoadPolicy() ) );
}

/**
 * Query whether the image data has loaded.
 *
 * The asynchronous loading begins when the Image object is created.
 * After the Image object is discarded, the image data will be released from memory
 * this will occur when the object is garbage collected.
 * @method getLoadPolicy
 * @for ResourceImage
 * @return {Integer} loading state either dali.RESOURCE_LOADING, dali.RESOURCE_LOADING_SUCCEEDED or dali.RESOUCE_LOADING_FAILED
 */
void ResourceImageApi::GetLoadingState( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  ResourceImage image = GetResourceImage( isolate, args );

  args.GetReturnValue().Set( v8::Integer::New( isolate, image.GetLoadingState() ) );
}

/**
 * Return the image url
 *
 * @method getUrl
 * @for ResourceImage
 * @return {String} filename
 */
void ResourceImageApi::GetUrl( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  ResourceImage image = GetResourceImage( isolate, args );

  v8::Local<v8::String> v8String = v8::String::NewFromUtf8( isolate, image.GetUrl().c_str() );
  args.GetReturnValue().Set( v8String  );
}


/**
 * Reload the image
 * The set ImageAttributes are used when requesting the image again.
 * @note if Image is offstage and OnDemand policy is set, reload request is ignored.
 * @method reload
 * @for ResourceImage
 */
void ResourceImageApi::Reload( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  ResourceImage image = GetResourceImage( isolate, args );
  image.Reload();
}


/**
 * Return attributes for the image
 * Only to be used after the image has finished loading.
 * (Ticket's LoadingSucceeded callback was called)
 * The returned value will reflect the true image dimensions once the asynchronous loading has finished.
 *
 * @method getAttributes
 * @for ResourceImage
 * @return {Object} ImageAttributes
 */
void ResourceImageApi::GetAttributes( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  ResourceImage image = GetResourceImage( isolate, args );

  v8::Local<v8::Object> localObject = ImageAttributesWrapper::WrapImageAttributes(isolate, image.GetAttributes());

  args.GetReturnValue().Set( localObject );
}
} // namespace V8Plugin

} // namespace Dali

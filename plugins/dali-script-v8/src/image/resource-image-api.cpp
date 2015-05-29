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

// EXTERNAL INCLUDES
#include <dali/public-api/images/image-operations.h>

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <image/image-wrapper.h>

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
 * @param {Integer} [options.loadPolicy] The LoadPolicy to apply when loading the image resource.
 * @param {Integer} [options.releasePolicy] optionally release memory when image is not visible on screen.
 * @return {Object} Image
 */
Image ResourceImageApi::New( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  std::string url;
  ImageDimensions dimensions;
  FittingMode::Type fittingMode = FittingMode::DEFAULT;
  SamplingMode::Type samplingMode = SamplingMode::DEFAULT;
  bool orientationCorrection = true;
  ResourceImage::LoadPolicy loadPolicy( ResourceImage::IMMEDIATE );
  Image::ReleasePolicy releasePolicy( Image::NEVER);

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

  v8::Local<v8::Value> widthValue = optionsObject->Get( v8::String::NewFromUtf8( isolate, "width" ) );
  if( widthValue->IsUint32() )
  {
    const uint32_t width = widthValue->ToUint32()->Value();
    dimensions = ImageDimensions( width, dimensions.GetHeight() );
  }

  v8::Local<v8::Value> heightValue = optionsObject->Get( v8::String::NewFromUtf8( isolate, "height" ) );
  if( heightValue->IsUint32() )
  {
    const uint32_t height = heightValue->ToUint32()->Value();
    dimensions = ImageDimensions( dimensions.GetWidth(), height );
  }

  v8::Local<v8::Value> fittingModeValue = optionsObject->Get( v8::String::NewFromUtf8( isolate, "fittingMode" ) );
  if( fittingModeValue->IsUint32() )
  {
    fittingMode = static_cast<FittingMode::Type>( fittingModeValue->ToUint32()->Value() );
  }

  v8::Local<v8::Value> samplingModeValue = optionsObject->Get( v8::String::NewFromUtf8( isolate, "samplingMode" ) );
  if( samplingModeValue->IsUint32() )
  {
    samplingMode = static_cast<SamplingMode::Type>( samplingModeValue->ToUint32()->Value() );
  }

  v8::Local<v8::Value> orientationCorrectionValue = optionsObject->Get( v8::String::NewFromUtf8( isolate, "orientationCorrection" ) );
  if( orientationCorrectionValue->IsBoolean() )
  {
    orientationCorrection = orientationCorrectionValue->ToBoolean()->Value();
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

  return ResourceImage::New( url, loadPolicy, releasePolicy, dimensions, fittingMode, samplingMode, orientationCorrection );
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

} // namespace V8Plugin

} // namespace Dali

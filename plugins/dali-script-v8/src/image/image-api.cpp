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
#include "image-api.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <image/image-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

Image ImageApi::GetImage( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate );

  v8::Local<v8::Object> object = args.This();
  v8::Local<v8::External> field = v8::Local<v8::External>::Cast( object->GetInternalField(0) );
  void* ptr = field->Value();

  ImageWrapper* wrapper = static_cast< ImageWrapper *>(ptr);
  return wrapper->GetImage();
}

/**
 * @constructor Image
 * @method Image
 * @for Image
 * @return {Object} Image
 */
Image ImageApi::New( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  DALI_SCRIPT_EXCEPTION( isolate, "Image base class can not be constructed, try new dali.ResourceImage()");
  return Image();
}

/**
 * Return the image width in pixels
 * @method getWidth
 * @for Image
 */
void ImageApi::GetWidth( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Image image = GetImage( isolate, args );

  args.GetReturnValue().Set( v8::Integer::New( isolate, image.GetWidth() ) );
}
/**
 * Return the image height in pixels
 * @method getHeight
 * @for Image
 */
void ImageApi::GetHeight( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Image image = GetImage( isolate, args );

  args.GetReturnValue().Set( v8::Integer::New( isolate, image.GetHeight() ) );
}

} // namespace V8Plugin

} // namespace Dali

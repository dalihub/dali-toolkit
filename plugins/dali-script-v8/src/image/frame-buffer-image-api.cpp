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
#include "frame-buffer-image-api.h"

// EXTERNAL INCLUDES
#include <dali/public-api/images/frame-buffer-image.h>

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <image/image-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

namespace FrameBufferImageApi
{

/**
 * FrameBufferImage represents a GLES Frame Buffer Object and contains the result
 * of an 'off screen' render pass of a RenderTask.
 * The FrameBufferImage can then be used with a Material (with optional shader
 * effects) and rendered to the screen.
 * @class FrameBufferImage
 * @extends Image
 */

/**
 * @constructor
 * @method FrameBufferImage
 * @for FrameBufferImage
 * @param {Object} options
 * @param {Uint32Array} options.pixelBuffer Array of RGBA pixel data
 * @param {Integer} options.width image width
 * @param {Integer} options.height image height
 * @param {Object } [options.nativeImage] ** currently not supported **
 * @param {Integer} [options.pixelFormat] pixel format ( see dali constants, e.g. dali.PIXEL_FORMAT_RGB8888)
 * @return {Object} Image
 */
Image New( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  v8::Local<v8::Value> options( args[0] );
  if( !options->IsObject() )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "Missing param");
    return FrameBufferImage();
  }

  v8::Local<v8::Object> obj = options->ToObject();

  v8::Local<v8::Value> widthValue = obj->Get( v8::String::NewFromUtf8( isolate, "width" ) );
  v8::Local<v8::Value> heightValue= obj->Get( v8::String::NewFromUtf8( isolate, "height" ) );

  unsigned int width = 0;
  unsigned int height = 0;

  if( widthValue->IsUint32() && heightValue->IsUint32() )
  {
    width = widthValue->ToUint32()->Value();
    height = heightValue->ToUint32()->Value();
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "Missing valid width and height params");
    return FrameBufferImage();
  }

  Pixel::Format  pixelFormat = Pixel::RGB8888;
  v8::Local<v8::Value> pixelFormatValue = obj->Get( v8::String::NewFromUtf8( isolate, "pixelFormat" ) );
  if( pixelFormatValue->IsUint32() )
  {
     pixelFormat = static_cast<Pixel::Format>( pixelFormatValue->ToUint32()->Value() );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "Pixel format not specified");
    return FrameBufferImage();
  }

  return FrameBufferImage::New( width, height, pixelFormat );
}

} // FrameBufferImageApi

} // namespace V8Plugin

} // namespace Dali

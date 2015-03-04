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
#include "buffer-image-api.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <image/image-wrapper.h>


namespace Dali
{

namespace V8Plugin
{

/**
 * ## BufferImage
 * Bitmap represents an image resource that can be added to ImageActors.
 * Its pixel buffer data is provided by the application developer.
 *
 * If the pixel format of the pixel buffer contains an alpha channel,
 * then the image is considered to be have transparent pixels without
 * regard for the actual content of the channel, and will be blended.
 * @class BufferImage
 * @extends Image
 */
BufferImage BufferImageApi::GetBufferImage( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate );

  v8::Local<v8::Object> object = args.This();
  v8::Local<v8::External> field = v8::Local<v8::External>::Cast( object->GetInternalField(0) );
  void* ptr = field->Value();

  ImageWrapper* wrapper = static_cast< ImageWrapper *>(ptr);
  return BufferImage::DownCast( wrapper->GetImage() );
}

/**
 * Create a new buffer image object ** ( work in progress, will currently only work with no parameters ) **
 * If no parameters are passed in, a single pixel white buffer image is created.
 *
 *
 * For better performance and portability use power of two dimensions.
 * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
 *
 * @note in case releasePolicy is "Unused", application has to call
 * BufferImage::Update() whenever image is re-added to the stage
 *
 * Example of creating a Uint32Array buffer
 * ```
 * "var ab = new ArrayBuffer(256 x 256 );"
 *  var pixelBuffer = new Uint32Array(ab );
 * ```
 * @constructor
 * @method BufferImage
 * @for BufferImage
 * @param {Object} options
 * @param {Uint32Array} options.pixelBuffer Array of RGBA pixel data
 * @param {Integer} options.width image width
 * @param {Integer} options.height image height
 * @param {Integer} options.pixelFormat pixel format ( see dali constants, e.g. dali.PIXEL_FORMAT_RGB8888)
 * @param {Integer} [options.stride the] internal stride of the pixelbuffer in pixels (normally the width)
 * @param {Integer} [options.releasePolicy] optionally release memory when image is not visible on screen.
 * @return {Object} Image
 */
Image BufferImageApi::New( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  v8::Local<v8::Value> options( args[0] );
  if( !options->IsObject() )
  {
    return BufferImage::WHITE();
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
    return BufferImage();
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
    return BufferImage();
  }

  v8::Local<v8::Value> pixelArray= obj->Get( v8::String::NewFromUtf8( isolate, "pixelBuffer" ) );
  if( pixelArray->IsUint32Array() )
  {
    //v8::Local<v8::Uint32Array> v8Array = v8::Local<v8::Uint32Array>::Cast( pixelArray );
   // uint32_t elementCount = v8Array->Length();
    DALI_SCRIPT_EXCEPTION( isolate, "pixel buffer currently not supported \n");
    return BufferImage::WHITE();
   }


  unsigned int stride = width;
  v8::Local<v8::Value> strideValue = obj->Get( v8::String::NewFromUtf8( isolate, "stride" ) );
  if( strideValue->IsUint32() )
  {
    stride = strideValue->ToUint32()->Value();
  }

  Image::ReleasePolicy releasePolicy =  Dali::Image::NEVER;
  v8::Local<v8::Value> releasePolicyValue = obj->Get( v8::String::NewFromUtf8( isolate, "releasePolicy" ) );
  if( releasePolicyValue->IsUint32() )
  {
    releasePolicy = static_cast<Image::ReleasePolicy>( releasePolicyValue->ToUint32()->Value() );
  }

  DALI_SCRIPT_EXCEPTION( isolate, "pixel buffer currently not supported \n");

  return BufferImage::New( NULL, width, height, pixelFormat, stride, releasePolicy);

}
/**
 * @brief Returns the pixel buffer of the Image **( currently not supported ) **
 * @method getBuffer
 * @for BufferImage
 * @return {Object}
 */
void BufferImageApi::GetBuffer( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  BufferImage image = GetBufferImage( isolate, args );

  //@todo figure out what the best thing to do here is...
  // we could copy the data into a javascript array
}

/**
 * @brief Returns buffer size in bytes.
 * @method getBufferSize
 * @for BufferImage
 * @return {Object} buffer
 */
void BufferImageApi::GetBufferSize( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  BufferImage image = GetBufferImage( isolate, args );

  args.GetReturnValue().Set( v8::Integer::New( isolate, image.GetBufferSize() ) );

}
/**
 * @brief Returns buffer stride in bytes.
 * @method getBufferStride
 * @for BufferImage
 * @return {Object}
 */
void BufferImageApi::GetBufferStride( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  BufferImage image = GetBufferImage( isolate, args );

  args.GetReturnValue().Set( v8::Integer::New( isolate, image.GetBufferStride() ) );

}
/**
 * @brief Returns pixel format
 * @method getPixelFormat
 * @for BufferImage
 * @return {Integer} pixel format
 */
void BufferImageApi::GetPixelFormat( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  BufferImage image = GetBufferImage( isolate, args );

  args.GetReturnValue().Set( v8::Integer::New( isolate, image.GetPixelFormat() ) );

}

/**
 * @brief  Inform Dali that the contents of the buffer have changed
 * @todo support update an area
 * @method update
 * @for BufferImage
 */
void BufferImageApi::Update( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  BufferImage image = GetBufferImage( isolate, args );

  image.Update();

}
/**
 * @brief  returns whether BufferImage uses external data source or not.
 * @method isDataExternal
 * @for BufferImage
 * @return {Boolean} true if data is external
 */
void BufferImageApi::IsDataExternal( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  BufferImage image = GetBufferImage( isolate, args );

  args.GetReturnValue().Set( v8::Boolean::New( isolate, image.IsDataExternal() ) );

}
} // namespace V8Plugin

} // namespace Dali

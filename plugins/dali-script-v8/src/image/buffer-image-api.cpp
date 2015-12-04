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
 * Bitmap represents an image resource as a pixel data buffer.
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
 * Create a new buffer image object using an external data source.
 *
 * For better performance and portability use power of two dimensions.
 * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
 *
 * Example of creating a buffer image from a pixel buffer
 * ```
 *  var pixelBufferData = [255, 0, 0, 255,   // red
 *                         0, 255, 0, 255,   // green
 *                         0, 0, 255, 255,   // blue
 *                         255, 0, 0, 255];  // red
 *
 *  var pixelBuffer = new Uint8Array(pixelBufferData.length);
 *  pixelBuffer.set(pixelBufferData, 0);
 *
 *  var option = {
 *                 width       : 2,
 *                 height      : 2,
 *                 pixelFormat : dali.PIXEL_FORMAT_RGBA888,  // optional
 *                 stride      : 2                           // optional
 *               }
 *
 *  var bufferImage = new dali.BufferImage(pixelBuffer, option);
 * ```
 * @constructor
 * @method BufferImage
 * @for BufferImage
 * @param {Uint8Array} pixelBuffer Array of RGBA pixel data
 * @param {Object} options
 * @param {Integer} options.width image width
 * @param {Integer} options.height image height
 * @param {Integer} [options.pixelFormat] pixel format (see dali constants, the default value is dali.PIXEL_FORMAT_RGBA8888)
 * @param {Integer} [options.stride] the internal stride of the pixelbuffer in pixels (the default value is the image width)
 * @return {Object} Image
 */
Image BufferImageApi::New( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  bool found( false );

  PixelBuffer* pixelBuffer = static_cast<PixelBuffer*>(V8Utils::GetArrayBufferViewParameter( PARAMETER_0, found, isolate, args));
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid pixelBuffer parameter" );
    return BufferImage();
  }

  v8::Local<v8::Value> options( args[1] );
  if( !options->IsObject() )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid option parameters" );
    return BufferImage();
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

  Pixel::Format  pixelFormat = Pixel::RGBA8888;
  v8::Local<v8::Value> pixelFormatValue = obj->Get( v8::String::NewFromUtf8( isolate, "pixelFormat" ) );
  if( pixelFormatValue->IsUint32() )
  {
     pixelFormat = static_cast<Pixel::Format>( pixelFormatValue->ToUint32()->Value() );
  }

  unsigned int stride = width;
  v8::Local<v8::Value> strideValue = obj->Get( v8::String::NewFromUtf8( isolate, "stride" ) );
  if( strideValue->IsUint32() )
  {
    stride = strideValue->ToUint32()->Value();
  }

  return BufferImage::New( pixelBuffer, width, height, pixelFormat, stride);
}
/**
 * Returns the pixel buffer of the Image
 * The application can write to the buffer to modify its contents.
 *
 * @method getBuffer
 * @for BufferImage
 * @return {Object} The pixel buffer
 */
void BufferImageApi::GetBuffer( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  BufferImage image = GetBufferImage( isolate, args );

  args.GetReturnValue().Set( v8::ArrayBuffer::New( isolate, static_cast<void*>( image.GetBuffer() ), image.GetBufferSize() ) );
}

/**
 * Returns buffer size in bytes.
 * @method getBufferSize
 * @for BufferImage
 * @return {Integer} buffer size
 */
void BufferImageApi::GetBufferSize( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  BufferImage image = GetBufferImage( isolate, args );

  args.GetReturnValue().Set( v8::Integer::New( isolate, image.GetBufferSize() ) );
}

/**
 * Returns buffer stride in bytes.
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
 * Returns pixel format
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
 * Inform Dali that the contents of the buffer have changed
 *
 * Example of updating the pixel buffer in the buffer image
 * ```
 *  var newPixelBufferData = [0, 255, 0, 255,   // green
 *                            255, 0, 0, 255,   // red
 *                            255, 0, 0, 255,   // red
 *                            0, 0, 255, 255];  // blue
 *
 *  var pixelBuffer = bufferImage.getBuffer();
 *  var pixelBufferDataArray = new Uint8Array(pixelBuffer);
 *  pixelBufferDataArray.set(newPixelBufferData, 0);
 *
 *  bufferImage.update();
 * ```
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
 * Return whether BufferImage uses external data source or not.
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

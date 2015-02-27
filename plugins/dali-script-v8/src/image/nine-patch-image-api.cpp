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
#include "nine-patch-image-api.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <image/image-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

/**
 *
 * NinePatchImage represents an image resource that can be added to ImageActors.
 * It contains a bitmap that is synchronously loaded from the file system that contains
 * a 9 patch border - a 1 pixel border that describes the stretch borders and the child
 * area.
 *
 * The class offers an API to read the stretch area and child area, but it does not
 * remove the border from it's bitmap. An API can be used to obtain a BufferImage with
 * the border removed.
 *
 * Adding this image to an ImageActor using an Image handle will automatically convert
 * to use the cropped BufferImage - if you don't retain a handle to this object, it will
 * be automatically destroyed.
 * @class NinePatchImage
 * @extends ResourceImage
 */

NinePatchImage NinePatchImageApi::GetNinePatchImage( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate );

  v8::Local<v8::Object> object = args.This();
  v8::Local<v8::External> field = v8::Local<v8::External>::Cast( object->GetInternalField(0) );
  void* ptr = field->Value();

  ImageWrapper* wrapper = static_cast< ImageWrapper *>(ptr);
  return NinePatchImage ::DownCast( wrapper->GetImage() );
}


/**
 * Create a new nine patch image object.
 *
 * @constructor
 * @method NinePatchImage
 * @for NinePatchImage
 * @param {Object} options
 * @param {String} options.url The URL of the nine patch image file to use.
 * @return {Object} Image
 */
Image NinePatchImageApi::New( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  std::string url;
  v8::Local<v8::Value> options( args[0] );

  if( !options->IsObject() )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "Missing params");
    return NinePatchImage();
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
    return NinePatchImage();
  }

  return NinePatchImage::New( url );
}


/**
 * Get the Stretch Borders
 *
 * @method getStretchBorders
 * @for NinePatchImage
 * @return object containing x,y,w,h properties
 */
void NinePatchImageApi::GetStretchBorders( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  NinePatchImage image = GetNinePatchImage( isolate, args );

  v8::Local<v8::Object> rectObject = v8::Object::New( isolate );

  Vector4 borders = image.GetStretchBorders();
  // Set the direction

  rectObject->Set( v8::String::NewFromUtf8( isolate, "x" ), v8::Integer::New( isolate,borders.x ) );
  rectObject->Set( v8::String::NewFromUtf8( isolate, "y" ), v8::Integer::New( isolate,borders.y ) );
  rectObject->Set( v8::String::NewFromUtf8( isolate, "w" ), v8::Integer::New( isolate,borders.z ) );
  rectObject->Set( v8::String::NewFromUtf8( isolate, "h" ), v8::Integer::New( isolate,borders.w ) );

  args.GetReturnValue().Set( rectObject );
}

/**
 * Get the child rectangle
 * @method getChildRectangle
 * @for NinePatchImage
 * @return {Object} position/size of of the child rectangle with x,y,w,h properties
 *
 */
void NinePatchImageApi::GetChildRectangle( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  NinePatchImage image = GetNinePatchImage( isolate, args );

  v8::Local<v8::Object> rectObject = v8::Object::New( isolate );
  Rect<int> childRect = image.GetChildRectangle();

  rectObject->Set( v8::String::NewFromUtf8( isolate, "x" ), v8::Integer::New( isolate,childRect.x ) );
  rectObject->Set( v8::String::NewFromUtf8( isolate, "y" ), v8::Integer::New( isolate,childRect.y ) );
  rectObject->Set( v8::String::NewFromUtf8( isolate, "w" ), v8::Integer::New( isolate,childRect.width ) );
  rectObject->Set( v8::String::NewFromUtf8( isolate, "h" ), v8::Integer::New( isolate,childRect.height ) );

  args.GetReturnValue().Set( rectObject );
}

/**
 * Creates a buffer image from the bitmap with the 1 pixel border cropped off.
 * This does not change the internal bitmap.
 * @method createCroppedBufferImage
 * @for NinePatchImage
 */
void NinePatchImageApi::CreateCroppedBufferImage( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Image image = GetNinePatchImage( isolate, args );

  args.GetReturnValue().Set( v8::Integer::New( isolate, image.GetWidth() ) );
}


} // namespace V8Plugin

} // namespace Dali

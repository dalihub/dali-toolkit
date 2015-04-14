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
#include "image-actor-api.h"

// INTERNAL INCLUDES
#include <object/handle-wrapper.h>
#include <v8-utils.h>
#include <object/property-value-wrapper.h>
#include <image/image-api.h>
#include <image/image-wrapper.h>


namespace Dali
{

namespace V8Plugin
{

namespace //unnamed name space
{

ImageActor GetImageActor( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  HandleWrapper* handleWrapper = HandleWrapper::Unwrap( isolate, args.This() );
  return ImageActor::DownCast( handleWrapper->mHandle );
}

} //unnamed name space

/**
 * Constructor
 *
 * @constructor
 * @method ImageActor
 * @for ImageActor
 * @param {Object} [image] Image object
 * @param {Object} [pixelArea] Vector4
 * @return {Object} ImageActor
 * @example
 *    new DALI.imageActor( image, [10,23,35,56] );
 */
Actor ImageActorApi::New( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  // Image actor has 3 different constructors
  // New();
  // New( image);
  // New( image, PixelArea pixelArea);

  if( args.Length() == 0 )
  {
    return ImageActor::New();
  }

  // args.Length> 0, must have an Image parameter
  bool found( false );
  Image image = V8Utils::GetImageParameter( PARAMETER_0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "missing image from param 0" );
    return ImageActor();
  }

  // check for PixelArea, accept a DALI Vector4 object ( which can be a JavaScript array)
  // e.g.  new DALI.imageActor( image, [10,23,35,56] );
  // or    new DALI.imageActor( image, Vector4 );

  if( args.Length() > 1 )
  {
    Vector4 rect = V8Utils::GetVector4Parameter( PARAMETER_1, found, isolate, args );
    if( !found )
    {
      DALI_SCRIPT_EXCEPTION( isolate, " bad parameters" );
      return ImageActor();
    }
    Rect<int>rectangle( static_cast<int>(rect.x),
                        static_cast<int>(rect.y),
                        static_cast<int>(rect.z),
                        static_cast<int>(rect.w));

    return ImageActor::New( image, rectangle );
  }
  else
  {
    return ImageActor::New( image );
  }
}

/**
 * Set the image rendered by the actor.
 *
 * When the image is loaded the actor's size will be reset to the image size,
 * unless a custom size was chosen, e.g. via actor.size or a pixel area
 * was set.
 * Note: The old image will continue to be displayed until the given image has loaded.
 * @for ImageActor
 * @method setImage
 * @param {Object} image The image to display.
 *
 */
void ImageActorApi::SetImage( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  bool found( false );
  Image image = V8Utils::GetImageParameter( PARAMETER_0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameters" );
    return;
  }
  ImageActor imageActor = GetImageActor( isolate, args );
  imageActor.SetImage( image );
}

/**
 * brief Retrieve the image rendered by the actor's attachment.
 * @for ImageActor
 * @method getImage
 * @return {Object} the image.
 */
void ImageActorApi::GetImage( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  ImageActor imageActor = GetImageActor( isolate, args );
  Image image = imageActor.GetImage();

  // wrap the image
  v8::Local<v8::Object> localObject = ImageWrapper::WrapImage( isolate, image );
  args.GetReturnValue().Set( localObject );
}

/**
 * Query whether a pixel area has been set.
 * @for ImageActor
 * @method isPixelAreaSet
 * @return {Boolean} True if a pixel area has been set.
 */
void ImageActorApi::IsPixelAreaSet( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  ImageActor imageActor = GetImageActor( isolate, args );

  args.GetReturnValue().Set( v8::Boolean::New( isolate, imageActor.IsPixelAreaSet() ) );
}

/**
 * Remove any pixel areas specified with SetPixelArea; the entire image will be displayed.
 *
 * The actor size will change to that of the Image unless a custom size was set, e.g. via
 * actor.size
 * @for ImageActor
 * @method clearPixelArea
 */
void ImageActorApi::ClearPixelArea( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  ImageActor imageActor = GetImageActor( isolate, args );

  imageActor.ClearPixelArea();
}



} // namespace V8Plugin

} // namespace Dali

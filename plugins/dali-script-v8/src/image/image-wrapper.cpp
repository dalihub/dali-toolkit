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
#include "image-wrapper.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <dali-wrapper.h>
#include <image/image-api.h>
#include <image/frame-buffer-image-api.h>
#include <image/resource-image-api.h>
#include <image/buffer-image-api.h>
#include <shared/api-function.h>
#include <shared/object-template-helper.h>

namespace Dali
{

namespace V8Plugin
{

v8::Persistent<v8::ObjectTemplate> ImageWrapper::mImageTemplate;
v8::Persistent<v8::ObjectTemplate> ImageWrapper::mResourceImageTemplate;
v8::Persistent<v8::ObjectTemplate> ImageWrapper::mBufferImageTemplate;
v8::Persistent<v8::ObjectTemplate> ImageWrapper::mFrameBufferImageTemplate;

/**
 * pointer to a persistent template handle
 */
struct ImageTemplate
{
  v8::Persistent<v8::ObjectTemplate>* imageTemplate;
};

/**
 * array of templates for each type of image
 */
const ImageTemplate ImageTemplateLookup[]=
{
    { &ImageWrapper::mImageTemplate },              // IMAGE
    { &ImageWrapper::mResourceImageTemplate },      // RESOURCE_IMAGE
    { &ImageWrapper::mBufferImageTemplate  },       // BITMAP_IMAGE
    { &ImageWrapper::mFrameBufferImageTemplate },   // FRAME_BUFFER_IMAGE

};


namespace // un-named name space
{

/**
 * Bitmask of API's that an image can support
 */
enum ImageApiBitMask
{
  IMAGE_API              = 1 << 0,
  BITMAP_IMAGE_API       = 1 << 1,
  RESOURCE_IMAGE_API     = 1 << 2,
  FRAME_BUFFER_IMAGE_API = 1 << 3,

};

/**
 * structure used for the ImageApiLookup.
 */
struct ImageApiStruct
{
  const char* imageName;                  ///< name of the image, used to find out what type of image to construct
  ImageWrapper::ImageType imageType;      ///< image type
  Image (*constructor)( const v8::FunctionCallbackInfo< v8::Value >& args); ///< constructor
  int supportApis;                        ///< supported API's
};

/**
 * Lookup table to match a image type with a constructor and supported API's.
 */
const ImageApiStruct ImageApiLookup[]=
{
  {"Image",           ImageWrapper::IMAGE,              ImageApi::New,            IMAGE_API },
  {"ResourceImage",   ImageWrapper::RESOURCE_IMAGE,     ResourceImageApi::New,    IMAGE_API | RESOURCE_IMAGE_API },
  {"BufferImage",     ImageWrapper::BITMAP_IMAGE,       BufferImageApi::New,      IMAGE_API | BITMAP_IMAGE_API },
  {"FrameBufferImage",ImageWrapper::FRAME_BUFFER_IMAGE, FrameBufferImageApi::New, IMAGE_API | FRAME_BUFFER_IMAGE_API },
};

const unsigned int ImageApiLookupCount = sizeof(ImageApiLookup)/sizeof(ImageApiLookup[0]);


/**
 * given an image type return what api's it supports
 */
int GetImageSupportedApis( ImageWrapper::ImageType type )
{
  return ImageApiLookup[ type].supportApis;
}

/**
 * Used for the ImageFunctionTable to map function names to functions
 * with for a specific API
 */
struct ImageFunctions
{
  const char* name;               ///< function name
  void (*function)( const v8::FunctionCallbackInfo< v8::Value >& args);
  ImageApiBitMask api;
};

/**
 * Contains a list of all functions that can be called
 */
const ImageFunctions ImageFunctionTable[]=
{
    /**************************************
    * Image API (in order of image.h)
    **************************************/
    { "GetWidth",                ImageApi::GetWidth , IMAGE_API },
    { "GetHeight",               ImageApi::GetHeight, IMAGE_API },

    // resource-image API
    { "GetLoadingState",         ResourceImageApi::GetLoadingState,     RESOURCE_IMAGE_API },
    { "GetUrl",                  ResourceImageApi::GetUrl,              RESOURCE_IMAGE_API },
    { "Reload",                  ResourceImageApi::Reload,              RESOURCE_IMAGE_API },

    // buffer image API
    { "GetBuffer",              BufferImageApi::GetBuffer,           BITMAP_IMAGE_API },
    { "GetBufferSize",          BufferImageApi::GetBufferSize,       BITMAP_IMAGE_API },
    { "GetBufferStride",        BufferImageApi::GetBufferStride,     BITMAP_IMAGE_API },
    { "GetPixelFormat",         BufferImageApi::GetPixelFormat,      BITMAP_IMAGE_API },
    { "Update",                 BufferImageApi::Update,              BITMAP_IMAGE_API },
    { "IsDataExternal",         BufferImageApi::IsDataExternal,      BITMAP_IMAGE_API },

    // Frame buffer image has no API
    // Native image has no API

};

const unsigned int ImageFunctionTableCount = sizeof(ImageFunctionTable)/sizeof(ImageFunctionTable[0]);
} //un-named space


ImageWrapper::ImageWrapper( const Dali::Image& image, GarbageCollectorInterface& gc )
: BaseWrappedObject( BaseWrappedObject::IMAGE , gc )
{
    mImage = image;
}

v8::Handle<v8::Object> ImageWrapper::WrapImage(v8::Isolate* isolate, const Dali::Image& image )
{
  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::Object> object = WrapImage( isolate, image, GetImageType( image.GetTypeName() ) );

  return handleScope.Escape( object );
}
v8::Handle<v8::Object> ImageWrapper::WrapImage(v8::Isolate* isolate, const Dali::Image& image, ImageType imageType )
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  objectTemplate = GetImageTemplate( isolate, imageType);

  // create an instance of the template
  v8::Local<v8::Object> localObject = objectTemplate->NewInstance();

  // create the Image wrapper
  ImageWrapper* pointer =  new ImageWrapper( image, Dali::V8Plugin::DaliWrapper::Get().GetDaliGarbageCollector() );

  // assign the JavaScript object to the wrapper.
  // This also stores the Image object, in an internal field inside the JavaScript object.
  pointer->SetJavascriptObject( isolate, localObject );

  return handleScope.Escape( localObject );
}

v8::Local<v8::ObjectTemplate> ImageWrapper::GetImageTemplate( v8::Isolate* isolate, ImageType imageType )
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  if( ImageTemplateLookup[ imageType ].imageTemplate->IsEmpty() )
  {
    objectTemplate = MakeImageTemplate( isolate, imageType );
    ImageTemplateLookup[ imageType ].imageTemplate->Reset( isolate, objectTemplate );
  }
  else
  {
    // get the object template
    objectTemplate = v8::Local<v8::ObjectTemplate>::New( isolate, *ImageTemplateLookup[ imageType ].imageTemplate );
  }

  return handleScope.Escape( objectTemplate );
}

v8::Handle<v8::ObjectTemplate> ImageWrapper::MakeImageTemplate( v8::Isolate* isolate, ImageType imageType )
{
  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::ObjectTemplate> objTemplate = v8::ObjectTemplate::New();

  objTemplate->SetInternalFieldCount( BaseWrappedObject::FIELD_COUNT );

  // add intercepts for Signals on ResourceImage, we can't use HandleWrapper::AddIntercepts because Image doesn't inherit
  // from Handle ( just baseHandle)
  if ( imageType == RESOURCE_IMAGE )
  {
     ObjectTemplateHelper::AddSignalConnectAndDisconnect( isolate, objTemplate );
  }

  // find out what API's this image supports
  int supportApis = GetImageSupportedApis( imageType );

  // add our function properties
  for( unsigned int i = 0; i < ImageFunctionTableCount; ++i )
  {
    const ImageFunctions property =  ImageFunctionTable[i];

    // check to see if the image supports a certain type of API
    // e.g. Bitmap will support IMAGE_API and BITMAP_IMAGE_API
    if( supportApis &  property.api )
    {
      std::string funcName = V8Utils::GetJavaScriptFunctionName( property.name);

      objTemplate->Set( v8::String::NewFromUtf8(   isolate, funcName.c_str() ),
                      v8::FunctionTemplate::New( isolate, property.function ) );
    }
  }

  return handleScope.Escape( objTemplate );
}


void ImageWrapper::NewImage( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate);

  if(!args.IsConstructCall())
  {
      DALI_SCRIPT_EXCEPTION( isolate, "Image constructor called without 'new'");
      return;
  }

  // find out the callee function name...e.g. BufferImage, ResourceImage
  v8::Local<v8::Function> callee = args.Callee();
  v8::Local<v8::Value> v8String = callee->GetName();
  std::string typeName = V8Utils::v8StringToStdString( v8String );

  ImageType imageType = GetImageType( typeName );

  if( imageType == UNKNOWN_IMAGE_TYPE )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "unknown image type");
    return;
  }
  Image image = (ImageApiLookup[imageType].constructor)( args );

  if( ! image )
  {
    // a v8 exception will have been thrown by the constructor
    return;
  }

  v8::Local<v8::Object> localObject = WrapImage( isolate, image, imageType );

  args.GetReturnValue().Set( localObject );
}

Image ImageWrapper::GetImage()
{
  return mImage;
}

/**
 * given an image type name, e.g. returns the type, e.g. ImageWrapper::BITMAP_IMAGE
 */
ImageWrapper::ImageType ImageWrapper::GetImageType( const std::string& name )
{
  for( unsigned int i = 0 ; i < ImageApiLookupCount ; i++ )
  {
    if( strncmp( ImageApiLookup[i].imageName, name.c_str(), name.length() ) == 0 )
    {
      return ImageApiLookup[i].imageType;
    }
  }
  return ImageWrapper::UNKNOWN_IMAGE_TYPE;
}



} // namespace V8Plugin

} // namespace Dali

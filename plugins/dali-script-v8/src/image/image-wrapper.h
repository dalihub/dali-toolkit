#ifndef __DALI_V8PLUGIN_IMAGE_WRAPPER_H__
#define __DALI_V8PLUGIN_IMAGE_WRAPPER_H__

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

// EXTERNAL INCLUDES
#include <v8.h>
#include <dali/public-api/images/image.h>

// INTERNAL INCLUDES
#include <shared/base-wrapped-object.h>
#include <signals/signal-manager.h>


namespace Dali
{

namespace V8Plugin
{


/**
 * An Image  wrapper.
 * Provides access to Image specific functionality and V8 memory handling.
 */
class ImageWrapper : public BaseWrappedObject
{

public:

  /**
   * Image type used an index,
   * please update the table in ImageApiLookup if this list changes ( the enum is used as an index)
   */
  enum ImageType
  {
    UNKNOWN_IMAGE_TYPE = -1,
    IMAGE               = 0,
    RESOURCE_IMAGE      = 1,
    NINE_PATCH_IMAGE    = 2,
    BITMAP_IMAGE        = 3,
    FRAME_BUFFER_IMAGE  = 4,
    NATIVE_IMAGE        = 5
   };

  /**
   * Constructor
   */
  ImageWrapper( const Image& image,
                GarbageCollectorInterface& gc );

  /**
   * Destructor
   */
  virtual ~ImageWrapper()
  {
  };

  /**
   * @brief Creates a new Image wrapped inside a Javascript Object.
   * @note: the actor type ie 'ImageImage' is expected to be the name of the callee function.
   * @param[in] args v8 function call arguments interpreted
   */
  static void NewImage( const v8::FunctionCallbackInfo< v8::Value >& args);

  /**
   * Wraps an image of a given type
   */
  static v8::Handle<v8::Object> WrapImage(v8::Isolate* isolate, const Dali::Image& image, ImageType imageType );

  /**
   * Wraps an image, the type is looked up from the image
   */
  static v8::Handle<v8::Object> WrapImage(v8::Isolate* isolate, const Dali::Image& image );

  /**
   * @return Image
   */
  Image GetImage();

  /**
   * @return signal manager pointer
   */
  virtual SignalManager* GetSignalManager() { return &mSignalManager;}

  // The Image ObjectTemplate, we cache templates so we don't have
  // keep generating them everytime we want to create an Image
  static v8::Persistent<v8::ObjectTemplate> mImageTemplate;
  static v8::Persistent<v8::ObjectTemplate> mResourceImageTemplate;
  static v8::Persistent<v8::ObjectTemplate> mNinePatchImageTemplate;
  static v8::Persistent<v8::ObjectTemplate> mBufferImageTemplate;
  static v8::Persistent<v8::ObjectTemplate> mFrameBufferImageTemplate;
  static v8::Persistent<v8::ObjectTemplate> mNativeImageTemplate;

private:

  /**
   * Helper
   */
  static ImageType GetImageType( const std::string& name );

  static v8::Handle<v8::ObjectTemplate> MakeImageTemplate( v8::Isolate* isolate, ImageType imageType );
  static v8::Local<v8::ObjectTemplate> GetImageTemplate( v8::Isolate* isolate , ImageType imageType );


  Image mImage;                     ///< Image handle
  SignalManager mSignalManager;     ///< Signal Manager

};

} // namespace V8Plugin

} // namespace Dali

#endif // __DALI_V8PLUGIN_IMAGE_WRAPPER_H__

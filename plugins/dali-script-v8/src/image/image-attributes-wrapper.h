#ifndef __DALI_V8PLUGIN_IMAGE_ATTRIBUTES_WRAPPER_H__
#define __DALI_V8PLUGIN_IMAGE_ATTRIBUTES_WRAPPER_H__

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
#include <dali/public-api/images/image-attributes.h>

// INTERNAL INCLUDES
#include <shared/base-wrapped-object.h>

namespace Dali
{

namespace V8Plugin
{


/**
 * An Image Attributes wrapper.
 * Provides access to ImageAttributes specific functionality and V8 memory handling.
 */
class ImageAttributesWrapper : public BaseWrappedObject
{

public:

  /**
   * Constructor
   */
  ImageAttributesWrapper( const ImageAttributes& imageAttributes,
                GarbageCollectorInterface& gc );

  /**
    * Destructor
    */
  virtual ~ImageAttributesWrapper(){};

  /**
   * Constructors
   */
  static void NewImageAttributes( const v8::FunctionCallbackInfo< v8::Value >& args);

  /**
   * Wraps an image attributes
   */
  static v8::Handle<v8::Object> WrapImageAttributes(v8::Isolate* isolate, const Dali::ImageAttributes& attributes);


  // The ImageAttribute ObjectTemplate, we cache templates so we don't have
  // keep generating them everytime we want to create an ImageAttribute
  static v8::Persistent<v8::ObjectTemplate> mImageAttributesTemplate;

  ImageAttributes& GetImageAttributes();
private:

  // ImageAttributes
  ImageAttributes mImageAttributes;

  static v8::Handle<v8::ObjectTemplate> MakeImageAttributesTemplate( v8::Isolate* isolate );
  static v8::Local<v8::ObjectTemplate> GetImageAttributesTemplate( v8::Isolate* isolate );

};

} // namespace V8Plugin

} // namespace Dali

#endif // header

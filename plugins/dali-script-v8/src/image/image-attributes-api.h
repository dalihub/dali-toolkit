#ifndef __DALI_V8PLUGIN_IMAGE_ATTRIBUTES_API_H__
#define __DALI_V8PLUGIN_IMAGE_ATTRIBUTES_API_H__

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


namespace Dali
{

namespace V8Plugin
{

namespace ImageAttributesApi
{
  ImageAttributes& GetImageAttributes( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args );

  ImageAttributes GetImageAttributesFromObject( v8::Isolate* isolate, v8::Local<v8::Object> object );

  /**
   * @brief Creates a new Image Attribute object based on some JavaScript parameters.
   * @param[in] args constructor parameters
   * @return ImageAttributes
   */
  ImageAttributes New( const v8::FunctionCallbackInfo< v8::Value >& args );

  /**
   * ImageAttributes API see image-attributes.h for a description
   */
  void SetSize( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetScalingMode( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetOrientationCorrection( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetWidth( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetHeight( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetSize( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetScalingMode( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetOrientationCorrection( const v8::FunctionCallbackInfo< v8::Value >& args );

}; // namespace ImageAttributesApi

} // namespace V8Plugin

} // namespace Dali

#endif // header __DALI_V8PLUGIN_IMAGE_ATTRIBUTES_API_H__

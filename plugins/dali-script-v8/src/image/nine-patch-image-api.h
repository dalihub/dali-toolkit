#ifndef __DALI_V8PLUGIN_NINE_PATCH_IMAGE_API_H__
#define __DALI_V8PLUGIN_NINE_PATCH_IMAGE_API_H__

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
#include <dali/public-api/images/nine-patch-image.h>

namespace Dali
{

namespace V8Plugin
{

namespace NinePatchImageApi
{
  /**
   * Helper
   */
  NinePatchImage GetNinePatchImage( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args );


  /**
   * Constructor
   */
  Image New( const v8::FunctionCallbackInfo< v8::Value >& args );


  /**
   * Ninepatch Image API see nine-patch-image.h for a description
   */
  void GetStretchBorders( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetChildRectangle( const v8::FunctionCallbackInfo< v8::Value >& args );
  void CreateCroppedBufferImage( const v8::FunctionCallbackInfo< v8::Value >& args );

}; // namespace NinePatchImageApi

} // namespace V8Plugin

} // namespace Dali

#endif // header __DALI_V8PLUGIN_NINE_PATCH_IMAGE_API_H__

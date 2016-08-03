#ifndef DALI_V8PLUGIN_TEXURE_SET_API_H
#define DALI_V8PLUGIN_TEXURE_SET_API_H

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

// EXTERNAL INCLUDES
#include <v8.h>
#include <dali/public-api/rendering/texture-set.h>


namespace Dali
{

namespace V8Plugin
{

namespace TextureSetApi
{

  /**
   * Helper to get TextureSet from args.This()
   */
  TextureSet GetTextureSet( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args );

  /**
   * @brief Helper to get texture set from the JavaScript object held in the given function argument
   * @param[in] paramIndex Argument index the object is held in
   * @param[in] found Whether texture set is found in the given function parameter
   * @param[in] isolate v8 isolated instance
   * @param[in] args v8 function call arguments interpreted
   */
  TextureSet GetTextureSetFromParams( int paramIndex,
                                     bool& found,
                                     v8::Isolate* isolate,
                                     const v8::FunctionCallbackInfo< v8::Value >& args );

  /**
   * Constructor
   */
  TextureSet New( const v8::FunctionCallbackInfo< v8::Value >& args );

   /**
    * TextureSet API see texture-set.h for a description
    */
   void SetSampler( const v8::FunctionCallbackInfo< v8::Value >& args );

}; // namespace TextureSetApi

} // namespace V8Plugin

} // namespace Dali

#endif // DALI_V8PLUGIN_TEXURE_SET_API_H

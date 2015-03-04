#ifndef __DALI_V8PLUGIN_API_FUNCTION_H__
#define __DALI_V8PLUGIN_API_FUNCTION_H__

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

namespace Dali
{

namespace V8Plugin
{


/**
 * @brief Used for mapping a string to a static function, for defining functions that are exposed to JavaScript.
 * E.g.
 * @code
 * const ApiFunction AnimationFunctionTable[]=
 * {
 *  { "setDuration"             , AnimationApi::SetDuration  },
 *  { "getDuration"             , AnimationApi::GetDuration  },
 *  { "setLooping"              , AnimationApi::SetLooping   },
 * }
 * @endcode
 */
struct ApiFunction
{
  const char* const name;      ///< function name
  void (*function)( const v8::FunctionCallbackInfo< v8::Value >& args);
};


} // V8Plugin

} // Dali

#endif

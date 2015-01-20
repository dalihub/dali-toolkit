#ifndef __DALI_V8PLUGIN_FONT_WRAPPER_H__
#define __DALI_V8PLUGIN_FONT_WRAPPER_H__

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
#include <dali/public-api/text/font.h>

// INTERNAL INCLUDES
#include <shared/base-wrapped-object.h>

namespace Dali
{

namespace V8Plugin
{


/**
 * An Font  wrapper.
 * Provides access to Font specific functionality and V8 memory handling.
 */
class FontWrapper : public BaseWrappedObject
{

public:

  FontWrapper( const Font& font,
                GarbageCollectorInterface& gc );

  virtual ~FontWrapper()
  {
  };

  /**
   * @brief Creates a new Font wrapped inside a Javascript Object.
   * @note: the actor type ie 'FontFont' is expected to be the name of the callee function.
   * @param[in] args v8 function call arguments interpreted
   */
  static void NewFont( const v8::FunctionCallbackInfo< v8::Value >& args);

  /**
   * Wraps a font
   */
  static v8::Handle<v8::Object> WrapFont(v8::Isolate* isolate, const Dali::Font& );

  Font GetFont();

  static v8::Handle<v8::Object> GetStaticFontObject(v8::Isolate* isolate);

private:

  Font mFont;
  static v8::Handle<v8::ObjectTemplate> MakeFontTemplate( v8::Isolate* isolate );

};

} // namespace V8Plugin

} // namespace Dali

#endif // __DALI_V8PLUGIN_FONT_WRAPPER_H__

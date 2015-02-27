#ifndef __DALI_V8PLUGIN_FONT_API_H__
#define __DALI_V8PLUGIN_FONT_API_H__

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


namespace Dali
{

namespace V8Plugin
{

namespace FontApi
{
  Font GetFont( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args );
  Font GetFont( v8::Isolate* isolate, v8::Local<v8::Value>& value );

  Font GetFontFromParams( int paramIndex,
                            bool& found,
                            v8::Isolate* isolate,
                            const v8::FunctionCallbackInfo< v8::Value >& args );

  /**
   * Constructor
   */
  Font New( const v8::FunctionCallbackInfo< v8::Value >& args );

  /**
   * Font API see image.h for a description
   */
  void GetFamilyForText( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetLineHeightFromCapsHeight( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetInstalledFonts( const v8::FunctionCallbackInfo< v8::Value >& args );
  void MeasureTextWidth( const v8::FunctionCallbackInfo< v8::Value >& args );
  void MeasureTextHeight( const v8::FunctionCallbackInfo< v8::Value >& args );
  void MeasureText( const v8::FunctionCallbackInfo< v8::Value >& args );
  void AllGlyphsSupported( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetProperties( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetMetrics( const v8::FunctionCallbackInfo< v8::Value >& args );
  void PointsToPixels( const v8::FunctionCallbackInfo< v8::Value >& args );
  void PixelsToPoints( const v8::FunctionCallbackInfo< v8::Value >& args );

}; // namespace FontApi

} // namespace V8Plugin

} // namespace Dali

#endif // header __DALI_V8PLUGIN_FONT_API_H__

#ifndef __DALI_V8PLUGIN_SCROLL_VIEW_API_H__
#define __DALI_V8PLUGIN_SCROLL_VIEW_API_H__

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
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view.h>

namespace Dali
{

namespace V8Plugin
{

namespace ScrollViewApi
{

  /**
   * constructor
   */
  Toolkit::Control New( const v8::FunctionCallbackInfo< v8::Value >& args );

  /**
   * ScrollView API. See scroll-view.h for description of functions
   */
  void SetScrollMode( const v8::FunctionCallbackInfo< v8::Value >& args);
  void GetCurrentPage( const v8::FunctionCallbackInfo< v8::Value >& args );
  void ScrollToPosition( const v8::FunctionCallbackInfo< v8::Value >& args );
  void ScrollToPage( const v8::FunctionCallbackInfo< v8::Value >& args );
  void ScrollToActor( const v8::FunctionCallbackInfo< v8::Value >& args );
  void ScrollToSnapInterval( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetScrollFlickAlphaFunction( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetScrollSnapAlphaFunction( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetSnapOvershootAlphaFunction( const v8::FunctionCallbackInfo< v8::Value >& args );

}; // namespace ScrollViewApi

} // namespace V8Plugin

} // namespace Dali

#endif // header __DALI_V8PLUGIN_SCROLL_VIEW_API_H__

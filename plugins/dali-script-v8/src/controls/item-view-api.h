#ifndef __DALI_V8PLUGIN_ITEM_VIEW_API_H__
#define __DALI_V8PLUGIN_ITEM_VIEW_API_H__

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
#include <dali-toolkit/public-api/controls/scrollable/item-view/item-view.h>

namespace Dali
{

namespace V8Plugin
{

namespace ItemViewApi
{

  /**
   * constructor
   */
  Toolkit::Control New( const v8::FunctionCallbackInfo< v8::Value >& args );

  /**
   * ItemView API. See item-view.h for description of functions
   */
  void GetLayoutCount( const v8::FunctionCallbackInfo< v8::Value >& args);
  void AddLayout( const v8::FunctionCallbackInfo< v8::Value >& args );
  void RemoveLayout( const v8::FunctionCallbackInfo< v8::Value >& args );
  void ActivateLayout( const v8::FunctionCallbackInfo< v8::Value >& args);
  void GetItemSize( const v8::FunctionCallbackInfo< v8::Value >& args);
  void SetItemSize( const v8::FunctionCallbackInfo< v8::Value >& args);
  void ScrollToItem( const v8::FunctionCallbackInfo< v8::Value >& args);
  void GetItem( const v8::FunctionCallbackInfo< v8::Value >& args);
  void GetItemId( const v8::FunctionCallbackInfo< v8::Value >& args);
  void GetItemsRange( const v8::FunctionCallbackInfo< v8::Value >& args);

}; // namespace ItemViewApi

} // namespace V8Plugin

} // namespace Dali

#endif // header __DALI_V8PLUGIN_ITEM_VIEW_API_H__

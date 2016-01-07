#ifndef __DALI_V8PLUGIN_ITEM_FACTORY_WRAPPER_H__
#define __DALI_V8PLUGIN_ITEM_FACTORY_WRAPPER_H__

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
#include <dali/public-api/common/dali-vector.h>
#include <dali-toolkit/public-api/controls/scrollable/item-view/item-factory.h>
#include <dali-toolkit/public-api/controls/scrollable/item-view/item-view.h>

// INTERNAL INCLUDES
#include <shared/base-wrapped-object.h>

namespace Dali
{

namespace V8Plugin
{


/**
 * Wraps a Dali ItemFactory.
 */
class ItemFactoryWrapper : public BaseWrappedObject
{

public:

  /**
   * Constructor
   * @param factory DALi ItemFactory
   * @param gc garbage collection interface
   */
  ItemFactoryWrapper( Toolkit::ItemFactory& factory,
                GarbageCollectorInterface& gc );

  /**
   * destructor
   */
  virtual ~ItemFactoryWrapper();

  /**
   * @brief Creates a new ItemFactory wrapped inside a Javascript Object.
   * @note: the item template and data are passed as a parameter e.g. 'template'
   * @param[in] args v8 function call arguments interpreted
   */
  static void NewItemFactory( const v8::FunctionCallbackInfo< v8::Value >& args);

  /**
   * @brief Wraps an ItemFactory
   */
  static v8::Handle<v8::Object> WrapItemFactory(v8::Isolate* isolate, Toolkit::ItemFactory& factory );

  // The ItemFactory ObjectTemplates.
  static v8::Persistent<v8::ObjectTemplate> mItemFactoryTemplate;

  /**
   * @brief Helper to get ItemFactory from the JavaScript object held in the given function argument
   * @param[in] paramIndex Argument index the object is held in
   * @param[in] found Whether ItemFactory is found in the given function parameter
   * @param[in] isolate v8 isolated instance
   * @param[in] args v8 function call arguments interpreted
   */
  static Toolkit::ItemFactory& GetItemFactoryFromParams( int paramIndex,
                                                             bool& found,
                                                             v8::Isolate* isolate,
                                                             const v8::FunctionCallbackInfo< v8::Value >& args );

  /**
   * @brief Helper to store a weak handle of ItemView in the given ItemFactory
   * @param[in] itemFactory The item factory used to provide items to the given item view
   * @param[in] itemView The ItemView which uses the given item factory to create items
   */
  static void SetItemView(Toolkit::ItemFactory& itemFactory, Toolkit::ItemView itemView);

  /**
   * @return the wrapped item factory
   */
  Toolkit::ItemFactory& GetItemFactory();

private:

  /**
   * Helper to make the item factory template
   */
  static v8::Handle<v8::ObjectTemplate> MakeItemFactoryTemplate( v8::Isolate* isolate );

  /**
   * Helper, get a item factory template
   */
  static v8::Local<v8::ObjectTemplate> GetItemFactoryTemplate( v8::Isolate* isolate );

  /**
   * @brief get the value for a property for JavaScript object than contains a Dali ItemFactory.
   * E.g. Get( "data", JavaScript object that wraps a Dali ItemFactory )
   * @param[in] propertyName property name
   * @param[in] info reference to PropertyCallbackInfo structure (contains the Javascript
   * object and the return value).
   */
  static void PropertyGet( v8::Local<v8::String> propertyName,
                              const v8::PropertyCallbackInfo<v8::Value>& info);

  /**
   * @brief Set the value for a property for JavaScript object than contains a Dali ItemFactory.
   * E.g. Set( "data", itemData, JavaScript object that wraps a Dali ItemFactory)
   * @param[in] propertyName property name
   * @param[in] javaScriptValue javascript value to set, this is typically a number
   * @param[in] info reference to PropertyCallbackInfo structure (contains the Javascript
   * object).
   */
  static void PropertySet( v8::Local<v8::String> propertyName,
                              v8::Local<v8::Value> javaScriptValue,
                              const v8::PropertyCallbackInfo<v8::Value>& info);


  /**
   * @brief Extract a item factory wrapper from a javascript object
   * @return item factory wrapper
   */
  static ItemFactoryWrapper* Unwrap( v8::Isolate* isolate, v8::Handle< v8::Object> obj);

  Toolkit::ItemFactory& mItemFactory;

};

} // namespace V8Plugin

} // namespace Dali

#endif // header

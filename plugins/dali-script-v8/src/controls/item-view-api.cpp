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

// CLASS HEADER
#include "item-view-api.h"

// EXTERNAL INCLUDES
#include <fstream>
#include <dali-toolkit/public-api/controls/scrollable/item-view/item-layout.h>
#include <dali-toolkit/public-api/controls/scrollable/item-view/item-factory.h>
#include <dali-toolkit/public-api/controls/scrollable/item-view/default-item-layout.h>

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <actors/actor-wrapper.h>
#include <controls/control-wrapper.h>
#include <controls/item-factory-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

namespace  // unanmed namespace
{

Toolkit::ItemView GetItemView( v8::Isolate* isolate, const v8::FunctionCallbackInfo<v8::Value>& args )
{
  HandleWrapper* handleWrapper = HandleWrapper::Unwrap( isolate, args.This() );
  return Toolkit::ItemView::DownCast( handleWrapper->mHandle );
}

} //unanmed namespace

/***************************************
 * ITEMVIEW API FUNCTIONS
 ***************************************/

/**
 * Constructor
 *
 * @for ItemView
 * @constructor
 * @method ItemView
 * @return {Object} itemView
 */
Toolkit::Control ItemViewApi::New( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  bool found( false );
  Toolkit::ItemFactory& factory = ItemFactoryWrapper::GetItemFactoryFromParams( 1, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid ItemFactory parameter" );
    return Toolkit::Control();
  }
  else
  {
    Toolkit::ItemView itemView = Toolkit::ItemView::New(factory);
    ItemFactoryWrapper::SetItemView(factory, itemView);
    return itemView;
  }
}

/**
 * Query the number of layouts.
 *
 * @for ItemView
 * @method getLayoutCount
 * @return {Integer} The number of layouts.
 */
void ItemViewApi::GetLayoutCount( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Toolkit::ItemView itemView = GetItemView( isolate, args );

  args.GetReturnValue().Set( v8::Integer::New( isolate, itemView.GetLayoutCount() ) );
}

/**
 * Add a layout
 *
 * @for ItemView
 * @method addLayout
 * @param {Integer} layout The layout to be added
 * @example
 *      // layout is one of the following
 *      dali.ITEM_LAYOUT_LIST
 *      dali.ITEM_LAYOUT_GRID
 *
 *      itemView.addLayout( dali.ITEM_LAYOUT_LIST );
 */
void ItemViewApi::AddLayout( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Toolkit::ItemView itemView = GetItemView( isolate, args );

  bool found( false );
  int layout = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 /* default */);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid layout parameter" );
    return;
  }

  Toolkit::ItemLayoutPtr layoutPtr = Toolkit::DefaultItemLayout::New( static_cast<Toolkit::DefaultItemLayout::Type>(layout) );
  itemView.AddLayout( *layoutPtr );
}

/**
 * Remove a layout.
 *
 * @for ItemView
 * @method removeLayout
 * @param {Integer} layoutIndex The index of the ItemView layouts which must be less than getLayoutCount().
 */
void ItemViewApi::RemoveLayout( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Toolkit::ItemView itemView = GetItemView( isolate, args );

  bool found( false );
  int layoutIndex = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 /* default */);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid index parameter" );
    return;
  }

  itemView.RemoveLayout( layoutIndex );
}

/**
 * Activate one of the layouts. This will resize the ItemView and relayout actors within the ItemView.
 *
 * @for ItemView
 * @method activateLayout
 * @param {Integer} layoutIndex The index of the ItemView layout which must be less than getLayoutCount().
 * @param {Object} targetSize An array of 3 numbers for the target ItemView and layout size.
 * @param {Float} [durationSeconds] The time taken to relayout in seconds (0 by default for immediate).
 */
void ItemViewApi::ActivateLayout( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Toolkit::ItemView itemView = GetItemView( isolate, args );

  bool found( false );
  int layoutIndex = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid layout index parameter" );
    return;
  }

  found = false;
  Vector3 targetSize = V8Utils::GetVector3Parameter( PARAMETER_1, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "Vector3 targetSize size parameter missing" );
    return;
  }

  found = false;
  float durationSeconds = V8Utils::GetFloatParameter( PARAMETER_2, found, isolate, args, 0.0f ); // 0 by default for immediate activation

  itemView.ActivateLayout( layoutIndex, targetSize, durationSeconds );
}

/**
 * Retrieve the target size of an item in the given layout.
 * This will return the default size for the layout unless overridden by calling setLayoutItemSize().
 *
 * @for ItemView
 * @method getItemSize
 * @param {Integer} layoutIndex The index of the ItemView layout which must be less than getLayoutCount().
 * @param {Integer} itemId The ID of an item in the layout.
 * @param {Object} targetLayoutSize An array of 3 numbers for the target ItemView and layout size.
 * @return {Object} The target size of the item {x, y, z}.
 */
void ItemViewApi::GetItemSize( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Toolkit::ItemView itemView = GetItemView( isolate, args );

  bool found( false );
  int layoutIndex = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 /* default */);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid layout index parameter" );
    return;
  }

  found = false;
  int itemId = V8Utils::GetIntegerParameter( PARAMETER_1, found, isolate, args, 0 /* default */);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid item ID parameter" );
    return;
  }

  found = false;
  Vector3 targetLayoutSize = V8Utils::GetVector3Parameter( PARAMETER_2, found, isolate, args );
  if( found )
  {
    Toolkit::ItemLayoutPtr layoutPtr = itemView.GetLayout(layoutIndex);
    Vector3 itemSize;
    layoutPtr->GetItemSize( itemId, targetLayoutSize, itemSize );

    v8::Local<v8::Object> itemSizeObject = v8::Object::New( isolate );

    itemSizeObject->Set( v8::String::NewFromUtf8( isolate, "x" ), v8::Integer::New( isolate, itemSize.width ) );
    itemSizeObject->Set( v8::String::NewFromUtf8( isolate, "y" ), v8::Integer::New( isolate, itemSize.height ) );
    itemSizeObject->Set( v8::String::NewFromUtf8( isolate, "z" ), v8::Integer::New( isolate, itemSize.depth ) );

    args.GetReturnValue().Set( itemSizeObject );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid Vector3 target size parameter" );
  }
}

/**
 * Set the size of the item for the given layout which overrides the default item size for the layout.
 *
 * @for ItemView
 * @method setItemSize
 * @param {Integer} layoutIndex The index of the ItemView layout which must be less than getLayoutCount().
 * @param {Object} itemSize An array of 3 numbers for the size of the item.
 * @example
 *      itemView.setLayoutItemSize( 0, [100.0, 50.0, 0.0] );
 */
void ItemViewApi::SetItemSize( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Toolkit::ItemView itemView = GetItemView( isolate, args );

  bool found( false );
  int layoutIndex = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 /* default */);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid layout index parameter" );
    return;
  }

  found = false;
  Vector3 itemSize = V8Utils::GetVector3Parameter( PARAMETER_1, found, isolate, args );
  if( found )
  {
    Toolkit::ItemLayoutPtr layoutPtr = itemView.GetLayout(layoutIndex);
    layoutPtr->SetItemSize( itemSize );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid item size parameter" );
  }
}

/**
 * Scroll the current layout to a particular item.
 * If calling this with zero second of duration immediately after calling activateLayout(),
 * it will not work unless the duration of relayout animation for activateLayout is also
 * set to zero.
 *
 * @for ItemView
 * @method scrollToItem
 * @param {Integer} itemId The ID of an item in the layout.
 * @param {Float} [durationSeconds] How long the scrolling takes in seconds (0 by default for instant scrolling to the particular item).
 */
void ItemViewApi::ScrollToItem( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Toolkit::ItemView itemView = GetItemView( isolate, args );

  bool found( false );
  int itemId = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 /* default */);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid item Id parameter" );
    return;
  }

  found = false;
  float durationSeconds = V8Utils::GetFloatParameter( PARAMETER_1, found, isolate, args, 0.0f ); // 0 by default for instant scrolling

  itemView.ScrollToItem( itemId, durationSeconds );
}

/**
 * Given the Item ID, this returns the accompanying actor.
 *
 * @for ItemView
 * @method getItem
 * @param {Integer} itemId The Item ID of the actor required.
 * @return {Object} The Actor corresponding to the Item ID.
 */
void ItemViewApi::GetItem( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Toolkit::ItemView itemView = GetItemView( isolate, args );

  bool found( false );
  int itemId = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 /* default */);
  if( found )
  {
    found = false;
    Actor actor = itemView.GetItem( itemId );
    if( actor )
    {
      found = true;
      // wrap the actor
      v8::Handle < v8::Object > wrappedActor = ActorWrapper::WrapActor( isolate, actor );
      args.GetReturnValue().Set( wrappedActor );
    }
  }

  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid item ID" );
    return;
  }
}

/**
 * Returns the Item ID of the specified actor. The actor must be an item of ItemView.
 *
 * @for ItemView
 * @method getItemId
 * @param {Object} actor The actor whose Item ID is required.
 * @return {Integer} The Item ID of the item.
 */
void ItemViewApi::GetItemId( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Toolkit::ItemView itemView = GetItemView( isolate, args );

  bool found( false );
  Actor actor = V8Utils::GetActorParameter( 0, found, isolate, args );
  if( found )
  {
    args.GetReturnValue().Set( itemView.GetItemId(actor) );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid item actor parameter" );
    return;
  }
}

/**
 * Get the range of items that are currently in ItemView.
 *
 * @for ItemView
 * @method getItemsRange
 * @return {Object} The range of items in the item ID {begin, end}.
 */
void ItemViewApi::GetItemsRange( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Toolkit::ItemView itemView = GetItemView( isolate, args );

  Toolkit::ItemRange range(0, 0);
  itemView.GetItemsRange(range);

  v8::Local<v8::Object> itemRangeObject = v8::Object::New( isolate );

  itemRangeObject->Set( v8::String::NewFromUtf8( isolate, "begin" ), v8::Integer::New( isolate, range.begin ) );
  itemRangeObject->Set( v8::String::NewFromUtf8( isolate, "end" ), v8::Integer::New( isolate, range.end ) );

  args.GetReturnValue().Set( itemRangeObject );
}

} // namespace V8Plugin

} // namespace Dali

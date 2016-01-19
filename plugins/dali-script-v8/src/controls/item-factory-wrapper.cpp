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
#include "item-factory-wrapper.h"

// EXTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/devel-api/object/weak-handle.h>
#include <dali-toolkit/devel-api/builder/builder.h>
#include <dali-toolkit/public-api/controls/scrollable/item-view/item-view.h>
#include <dali-toolkit/public-api/controls/scrollable/item-view/item-layout.h>

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <dali-wrapper.h>
#include <shared/api-function.h>
#include <shared/object-template-helper.h>

namespace Dali
{

namespace V8Plugin
{

v8::Persistent<v8::ObjectTemplate> ItemFactoryWrapper::mItemFactoryTemplate;

namespace
{

typedef std::vector< Property::Map > ItemDataContainer;

// Implementation of ItemFactory for providing actors to ItemView
class ItemFactory : public Toolkit::ItemFactory
{
public:

  /**
   * Constructor
   * @param application class, stored as reference
   */
  ItemFactory()
  : mJsonFileLoaded(false),
    mNumberOfItems(0)
  {
    mBuilder = Toolkit::Builder::New();
  }

  /**
   * Set the name of the JSON file which defines the templates of items.
   * @param jsonFile The JSON file
   */
  void SetJsonTemplateFile(std::string jsonFile)
  {
    if(mJsonFile != jsonFile)
    {
      mJsonFile = jsonFile;
      LoadJsonFile(mJsonFile);

      // Check whether any layout activated in ItemView
      Toolkit::ItemView itemView = mItemView.GetHandle();
      if(itemView && itemView.GetActiveLayout() != NULL)
      {
        // Refresh ItemView if item templates are changed
        itemView.Refresh();
      }
    }
  }

  /**
   * Returns the name of the JSON file.
   * @return The JSON file name
   */
  std::string GetJsonTemplate()
  {
    return mJsonFile;
  }

  /**
   * Set the data to be used to create new items.
   *
   * If ItemView is already created, this will immediately update ItemView with the
   * new data.
   *
   * The data is an array of property maps in which each map contains the data for
   * each item, including the template to be used to build the actor and the pairs
   * of key/value to be used to replace the constants defined in the template.
   * The order of property maps in the array represents the actual order of items
   * in ItemView.
   *
   * @param data The array of property maps
   */
  void SetData(ItemDataContainer data)
  {
    ItemDataContainer currentData = mData;
    mData = data;
    mNumberOfItems = mData.size();

    // Check whether any layout activated in ItemView
    Toolkit::ItemView itemView = mItemView.GetHandle();
    if(itemView && itemView.GetActiveLayout() != NULL)
    {
      unsigned int currentNumberOfItems = currentData.size();
      unsigned int newNumberOfItems = data.size();

      // Check whether any items added or deleted from the data
      // which requires ItemView to be refreshed with the new data
      if(currentNumberOfItems != newNumberOfItems)
      {
        itemView.Refresh();
      }
      else
      {
        for( unsigned int itemId = 0; itemId < newNumberOfItems; itemId++)
        {
          // Check whether the item is already built in ItemView
          Actor itemActor = itemView.GetItem(itemId);
          if(itemActor)
          {
            // Check if the item needs to be rebuilt
            if( !V8Utils::IsPropertyMapIdentical(currentData[itemId], data[itemId]) )
            {
              // Rebuild the item with the new data
              Actor newItemActor = NewItem(itemId);

              // Replace the old item with the new one
              itemView.ReplaceItem( Toolkit::Item( itemId, newItemActor ), 0.0f );
            }
          }
        }
      }
    }
  }

  /**
   * Retrieve the data.
   * @return the data.
   */
  ItemDataContainer GetData()
  {
    return mData;
  }

  /**
   * Store a weak handle of ItemView in order to access ItemView APIs
   * from this ItemFactory implementation
   * @return the data.
   */
  void SetItemView(Toolkit::ItemView itemView)
  {
    mItemView = itemView;
  }

public: // From Toolkit::ItemFactory

  /**
   * Query the number of items available from the factory.
   * The maximum available item has an ID of GetNumberOfItems() - 1.
   */
  virtual unsigned int GetNumberOfItems()
  {
    return mJsonFileLoaded ? mNumberOfItems : 0;
  }

  /**
   * Create an Actor to represent a visible item.
   * @param itemId
   * @return the created actor.
   */
  virtual Actor NewItem(unsigned int itemId)
  {
    std::string itemTemplate;

    Property::Map constantsMap = mData[itemId];
    for ( unsigned int i = 0, count = constantsMap.Count(); i < count; ++i )
    {
      Property::Value& constantValue = constantsMap.GetValue(i);
      if(constantsMap.GetKey(i) == "template")
      {
        constantValue.Get(itemTemplate);
      }
      else
      {
        mBuilder.AddConstant( constantsMap.GetKey(i), constantValue );
      }
    }

    Actor item = Actor::DownCast( mBuilder.Create(itemTemplate) );
    return item;
  }

private:

  /**
   * Load the JSON file.
   * @param The JSON file name
   */
  void LoadJsonFile(std::string jsonFile)
  {
    try
    {
      std::string data;
      V8Utils::GetFileContents(jsonFile, data);

      mBuilder.LoadFromString(data);

      mJsonFileLoaded = true;
    }
    catch(...)
    {
//      printf("invalid JSON data\n");
      mJsonFileLoaded = false;
    }
  }

private:

  std::string mJsonFile;
  bool mJsonFileLoaded;
  Toolkit::Builder mBuilder;
  unsigned int mNumberOfItems;
  ItemDataContainer mData;
  WeakHandle< Toolkit::ItemView > mItemView;
};

} //un-named space

ItemFactoryWrapper::ItemFactoryWrapper( Toolkit::ItemFactory& factory, GarbageCollectorInterface& gc )
: BaseWrappedObject( BaseWrappedObject::ITEMFACTORY , gc ),
  mItemFactory( factory )
{
}

ItemFactoryWrapper::~ItemFactoryWrapper()
{
}

v8::Handle<v8::Object> ItemFactoryWrapper::WrapItemFactory(v8::Isolate* isolate, Toolkit::ItemFactory& factory )
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  objectTemplate = GetItemFactoryTemplate( isolate );

  // create an instance of the template
  v8::Local<v8::Object> localObject = objectTemplate->NewInstance();

  // create the ItemFactory wrapper
  ItemFactoryWrapper* pointer =  new ItemFactoryWrapper( factory, Dali::V8Plugin::DaliWrapper::Get().GetDaliGarbageCollector() );

  // assign the JavaScript object to the wrapper.
  pointer->SetJavascriptObject( isolate, localObject );

  return handleScope.Escape( localObject );
}

v8::Local<v8::ObjectTemplate> ItemFactoryWrapper::GetItemFactoryTemplate( v8::Isolate* isolate)
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  if( mItemFactoryTemplate.IsEmpty() )
  {
    objectTemplate = MakeItemFactoryTemplate( isolate );
    mItemFactoryTemplate.Reset( isolate, objectTemplate );
  }
  else
  {
    // get the object template
    objectTemplate = v8::Local<v8::ObjectTemplate>::New( isolate, mItemFactoryTemplate );
  }
  return handleScope.Escape( objectTemplate );
}

v8::Handle<v8::ObjectTemplate> ItemFactoryWrapper::MakeItemFactoryTemplate( v8::Isolate* isolate )
{
  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::ObjectTemplate> objTemplate = v8::ObjectTemplate::New();

  objTemplate->SetInternalFieldCount( BaseWrappedObject::FIELD_COUNT );

  // set property setter and getter
  objTemplate->SetNamedPropertyHandler( ItemFactoryWrapper::PropertyGet, ItemFactoryWrapper::PropertySet);

  return handleScope.Escape( objTemplate );
}

void ItemFactoryWrapper::NewItemFactory( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate);

  if( !args.IsConstructCall() )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "ItemFactory constructor called without 'new'" );
    return;
  }

  Toolkit::ItemFactory* factory = new ItemFactory();

  v8::Local<v8::Object> localObject = WrapItemFactory( isolate, *factory );
  args.GetReturnValue().Set( localObject );
}

Toolkit::ItemFactory& ItemFactoryWrapper::GetItemFactoryFromParams( int paramIndex,
                                             bool& found,
                                             v8::Isolate* isolate,
                                             const v8::FunctionCallbackInfo< v8::Value >& args )
{
  found = false;

  v8::HandleScope handleScope( isolate );
  BaseWrappedObject* wrappedObject = V8Utils::GetWrappedDaliObjectParameter( paramIndex, BaseWrappedObject::ITEMFACTORY, isolate, args );
  if( wrappedObject )
  {
    found = true;
    ItemFactoryWrapper* wrapper = static_cast< ItemFactoryWrapper *>(wrappedObject);
    return wrapper->GetItemFactory();
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "no valid ItemFactory parameter" );
    Toolkit::ItemFactory* dummyFactory = new ItemFactory();
    return *dummyFactory; // avoid build error
  }
}

ItemFactoryWrapper* ItemFactoryWrapper::Unwrap( v8::Isolate* isolate, v8::Handle< v8::Object> obj)
{
  v8::HandleScope handleScope( isolate );

  v8::Local<v8::External> field = v8::Local<v8::External>::Cast( obj->GetInternalField(0) );
  void* ptr = field->Value();
  return static_cast< ItemFactoryWrapper *>(ptr);
}

void ItemFactoryWrapper::PropertyGet( v8::Local<v8::String> propertyName,
                                        const v8::PropertyCallbackInfo<v8::Value>& info)
{
  v8::Isolate* isolate = info.GetIsolate();
  v8::HandleScope handleScope( isolate );

  // get the property name
  std::string name = V8Utils::v8StringToStdString( propertyName );

  if( std::isupper( name[0] ))
  {
    return;
  }

  // unwrap the object
  ItemFactoryWrapper* itemFactoryWrapper = Unwrap( isolate, info.This() );
  if( !itemFactoryWrapper )
  {
    return;
  }

  ItemFactory& factory = static_cast<ItemFactory&>( itemFactoryWrapper->GetItemFactory() );

  if( name == "jsonTemplateFile" )
  {
    std::string jsonTemplateFile = factory.GetJsonTemplate();
    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, jsonTemplateFile.c_str()));
  }
  else if( name == "data" )
  {
    ItemDataContainer data = factory.GetData();
    unsigned int itemCount = data.size();

    v8::Local<v8::Array> array= v8::Array::New( isolate, itemCount );
    for( unsigned int i = 0; i < itemCount; i++)
    {
      v8::Local<v8::Object> mapObject = v8::Object::New( isolate );
      V8Utils::CreatePropertyMap( isolate, data[i], mapObject );

      array->Set( i, mapObject);
    }

    info.GetReturnValue().Set(array);
  }
  else
  {
    std::string error="Invalid property Get for "+name + "\n";
    DALI_SCRIPT_EXCEPTION( isolate, error );
  }
}

void ItemFactoryWrapper::PropertySet( v8::Local<v8::String> propertyName,
                  v8::Local<v8::Value> javaScriptValue,
                  const v8::PropertyCallbackInfo<v8::Value>& info)
{

  v8::Isolate* isolate = info.GetIsolate();
  v8::HandleScope handleScope( isolate );

  // get the property name
  std::string name = V8Utils::v8StringToStdString( propertyName );

  // unwrap the object
  ItemFactoryWrapper* itemFactoryWrapper = Unwrap( isolate, info.This() );
  if( !itemFactoryWrapper )
  {
    return;
  }

  ItemFactory& factory = static_cast<ItemFactory&>( itemFactoryWrapper->GetItemFactory() );

  if( name == "jsonTemplateFile" && javaScriptValue->IsString() )
  {
    std::string jsonTemplateFile = V8Utils::v8StringToStdString( javaScriptValue );
    factory.SetJsonTemplateFile(jsonTemplateFile);
  }
  else if( name == "data" && javaScriptValue->IsArray() )
  {
    v8::Local<v8::Array> array = v8::Local<v8::Array>::Cast(javaScriptValue);

    ItemDataContainer data;

    for( unsigned int i = 0; i < array->Length(); ++i )
    {
      v8::Local<v8::Value> itemData = array->Get(i);

      if( itemData->IsObject() )
      {
        Dali::Property::Map map = V8Utils::GetPropertyMapFromObject( isolate, itemData->ToObject() );
        data.push_back(map);
      }
    }

    factory.SetData(data);
  }
  else
  {
    std::string error = "Invalid property Set for " + name + "\n";
    DALI_SCRIPT_EXCEPTION( isolate, error );
  }
}

Toolkit::ItemFactory& ItemFactoryWrapper::GetItemFactory()
{
  return mItemFactory;
}

void ItemFactoryWrapper::SetItemView(Toolkit::ItemFactory& itemFactory, Toolkit::ItemView itemView)
{
  ItemFactory& factory = static_cast<ItemFactory&>( itemFactory );
  factory.SetItemView(itemView);
}

} // namespace V8Plugin

} // namespace Dali

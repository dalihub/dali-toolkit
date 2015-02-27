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
#include "handle-wrapper.h"

// EXTERNAL INCLUDES
#include <sstream>

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <shared/base-wrapped-object.h>
#include <shared/object-template-helper.h>
#include <object/property-value-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

HandleWrapper::HandleWrapper( BaseWrappedObject::Type type,
    Handle handle,
    GarbageCollectorInterface& gc ) :
 BaseWrappedObject( type, gc ),
 mHandle( handle )
{
}

HandleWrapper::~HandleWrapper()
{

}
HandleWrapper*  HandleWrapper::Unwrap( v8::Isolate* isolate, v8::Handle< v8::Object> obj)
{
  v8::HandleScope handleScope( isolate );

  v8::Local<v8::External> field = v8::Local<v8::External>::Cast( obj->GetInternalField(0) );
  void* ptr = field->Value();
  return static_cast< HandleWrapper *>(ptr);
}

// may have to do this IsUpper to intercept function calls or as function?
void HandleWrapper::PropertyGet( v8::Local<v8::String> propertyName,
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
  HandleWrapper* handleWrapper = Unwrap( isolate, info.This() );
  Handle handle =  handleWrapper->mHandle;

  // get the property index
  // convert from camel case to dali property style with hyphens
  std::string daliPropertyName = V8Utils::JavaScriptNameToPropertyName(name);
  Dali::Property::Index index = handle.GetPropertyIndex( daliPropertyName );

  if(index != Dali::Property::INVALID_INDEX)
  {
    Dali::Property::Value value = handle.GetProperty(index);

    // Simple Dali properties (ints, strings, bools etc) are stored as JavaScript primitives (v8::Boolean ...)
    // more complex properties (Vectors, Rectangles...) are wrapped by a JavaScript object
    v8::Local<v8::Object> ret = PropertyValueWrapper::WrapDaliProperty( isolate, value );

    info.GetReturnValue().Set( ret );
  }
  else
  {
    //  std::string error="Invalid property Get for "+name + "\n";
    // DALI_SCRIPT_EXCEPTION( isolate, error );
  }

}
void HandleWrapper::PropertySet( v8::Local<v8::String> propertyName,
                  v8::Local<v8::Value> javaScriptValue,
                  const v8::PropertyCallbackInfo<v8::Value>& info)
{

  v8::Isolate* isolate = info.GetIsolate();
  v8::HandleScope handleScope( isolate );

  // get the property name
  std::string name = V8Utils::v8StringToStdString( propertyName );

  // try to filter out function calls before going to the property system
  // @todo use installed functions to generate a map
  if( ( name.compare(0,2,"is") == 0 )  ||
      ( name.compare(0,3,"get") == 0 ) ||
      ( name.compare(0,3,"add") == 0 ) ||
      ( name.compare(0,3,"set") == 0 ) ||
      ( name.compare(0,3,"get") == 0 ) ||
      ( name.compare(0,4,"find") == 0 ) ||
      ( name.compare(0,6,"remove") == 0 )
     )
  {
    //
    return;
  }
  // unwrap the object
  HandleWrapper* handleWrapper = Unwrap( isolate, info.This() );
  if( !handleWrapper )
  {
    // printf("setting property name %s \n", name.c_str());
    return;
  }

 // DALI_ASSERT_DEBUG( handleWrapper && "not a dali object");
  Handle handle =  handleWrapper->mHandle;

  // convert from camel case to dali property style with hyphens
  std::string daliPropertyName = V8Utils::JavaScriptNameToPropertyName(name);
  Dali::Property::Index index = handle.GetPropertyIndex( daliPropertyName );

  if(index != Dali::Property::INVALID_INDEX)
  {
    Dali::Property::Type type = handle.GetPropertyType(index);

    // we know the type we want to set ( int, vector, etc..)
    // try and convert the javascript value in to the type we want.
    Dali::Property::Value value = PropertyValueWrapper::ExtractPropertyValue( isolate, javaScriptValue, type);

    if( Dali::Property::NONE == value.GetType() )
      {
        std::stringstream msg;
        msg << "Invalid property Set: '";
        msg << name;
        msg << "(Index = ";
        msg << index;
        msg << ")";
        msg << "' Cannot convert value to correct type: (";
        msg << type;
        msg << ")";
        msg << Dali::PropertyTypes::GetName(type);
        DALI_SCRIPT_EXCEPTION( isolate, msg.str().c_str());
      }
      else
      {
        handle.SetProperty( index, value );
      }
  }
  else
  {
    std::string error="Invalid property Set for "+name + "\n";
    DALI_SCRIPT_EXCEPTION( isolate, error );
  }
}

void HandleWrapper::AddInterceptsToTemplate( v8::Isolate* isolate, v8::Local<v8::ObjectTemplate>& objTemplate )
{
  v8::HandleScope handleScope( isolate );

  objTemplate->SetNamedPropertyHandler( HandleWrapper::PropertyGet, HandleWrapper::PropertySet);

  ObjectTemplateHelper::AddSignalConnectAndDisconnect( isolate, objTemplate );

}


} // namespace V8Plugin

} // namespace Dali

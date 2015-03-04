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
#include "base-wrapped-object.h"

namespace Dali
{

namespace V8Plugin
{

BaseWrappedObject::BaseWrappedObject( Type type, GarbageCollectorInterface& gc )
: mWrappedType( type ),
  mGarbageCollector( gc  )
{
  mGarbageCollector.Register( this );
}

BaseWrappedObject::~BaseWrappedObject()
{
  mGarbageCollector.UnRegister( this );
}

bool BaseWrappedObject::IsReferenced()
{
  if( mWeakPersistentHandle.IsEmpty() )
  {
    return false;
  }
  if( mWeakPersistentHandle.IsNearDeath() )
  {
    return false;
  }
  return true;
}
void BaseWrappedObject::WeakCallback( const v8::WeakCallbackData<v8::Object,BaseWrappedObject >& data)
{
  BaseWrappedObject* wrappedObject = data.GetParameter();
  wrappedObject->mWeakPersistentHandle.Reset();  // moved from destructor due to crash on shutdown
  delete wrappedObject;
}

void BaseWrappedObject::SetJavascriptObject( v8::Isolate* isolate, v8::Local<v8::Object>& object )
{
  v8::HandleScope handleScope( isolate );

  v8::Handle<v8::External> ptr = v8::External::New( isolate, this );
  object->SetInternalField( FIELD_POINTER, ptr );

  v8::Local<v8::Value> theType = v8::Integer::New( isolate ,mWrappedType );
  object->SetInternalField( FIELD_TYPE, theType );

  // reset sets the the handle
  mWeakPersistentHandle.Reset( isolate, object );

  // set the weak call back which is triggered when nothing else is referencing the object
  // note, this may never called.
  mWeakPersistentHandle.SetWeak( this, &WeakCallback );
}

bool BaseWrappedObject::IsWrappedType(v8::Isolate* isolate, const v8::Local<v8::Object>& object,  BaseWrappedObject::Type type)
{
  v8::HandleScope handleScope( isolate );

  // we've been passed a javascript object, it could be anything!
  // so we safely check the internal field count matches the number of fields
  // in BaseWrappedObject. Then check if the second field is an integer value
  // possibly alternative v8 functions we could use
  // GetConstructorName
  // GetIdentityHash
  if( object->InternalFieldCount() == BaseWrappedObject::FIELD_COUNT )
  {
    v8::Handle<v8::Value> value = object->GetInternalField(BaseWrappedObject::FIELD_TYPE);

    if( value->IsInt32() )
    {
      BaseWrappedObject::Type objectType = static_cast<BaseWrappedObject::Type>( value->ToInt32()->Value());
      return (type == objectType);
    }
  }
  return false;
}

bool BaseWrappedObject::IsWrappedTypeAPropertyValue(const v8::Local<v8::Object>& object)
{

   if( object->InternalFieldCount() == BaseWrappedObject::FIELD_COUNT )
   {
     v8::Handle<v8::Value> value = object->GetInternalField(BaseWrappedObject::FIELD_TYPE);

     if( value->IsInt32() )
     {
       BaseWrappedObject::Type objectType = static_cast<BaseWrappedObject::Type>( value->ToInt32()->Value());
       return ( (objectType > PROPERTY_VALUE_START_RANGE) && ( objectType < PROPERTY_VALUE_END_RANGE) );
     }
   }
   return false;
}

BaseWrappedObject* BaseWrappedObject::UnWrap( v8::Isolate* isolate, const v8::Local<v8::Object>& object)
{
  v8::HandleScope handleScope( isolate );

  v8::Local<v8::External> field = v8::Local<v8::External>::Cast( object->GetInternalField(BaseWrappedObject::FIELD_POINTER) );
  void* ptr = field->Value();
  return static_cast< BaseWrappedObject *>(ptr);
}

BaseWrappedObject::Type BaseWrappedObject::GetType()
{
  return  mWrappedType;
}

SignalManager* BaseWrappedObject::GetSignalManager()
{
  return NULL;
}
} // V8Plugin

} // Dali


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

// HEADER
#include "v8-utils.h"

// EXTERNAL INCLUDES
#include <iostream>
#include <fstream>
#include <sstream>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <object/property-value-wrapper.h>
#include <actors/actor-wrapper.h>
#include <object/handle-wrapper.h>
#include <image/image-wrapper.h>
#include <render-tasks/render-task-wrapper.h>
#include <object/property-value-wrapper.h>


/**
 * Similar to DALI_LOG_ERROR except the PRETTY_FUNCTION
 * is removed because it makes no sense for scripting errors.
 */
#define DALI_LOG_SCRIPT_ERROR(format, args...) Dali::Integration::Log::LogMessage(Dali::Integration::Log::DebugError, format, ## args)

namespace Dali
{

namespace V8Plugin
{

namespace V8Utils
{

void Log(const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::HandleScope handleScope( args.GetIsolate());

  bool first = true;
  for (int i = 0; i < args.Length(); i++)
  {
    if (first)
    {
      first = false;
    }
    else
    {
      std::cout << " ";
    }
    v8::String::Utf8Value utf8_value( args[i] );
    std::cout << *utf8_value;
  }
}

void LogError(const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::HandleScope handleScope( args.GetIsolate());
  std::string output;
  bool first = true;
  for (int i = 0; i < args.Length(); i++)
  {
    if (first)
    {
      first = false;
    }
    else
    {
      output +=" ";
    }
    v8::String::Utf8Value utf8_value( args[i] );
    output += *utf8_value;
  }
  DALI_LOG_ERROR_NOFN( "JavaScript: %s",output.c_str() );
}

void GetFileContents(const std::string &fileName, std::string& contents)
{
   std::ifstream t(fileName.c_str());
   contents = std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
};

void GetFileDirectory( const std::string& fileName, std::string& directory )
{
  directory = "";

  // get the position of the last slash
  size_t pos = fileName.find_last_of("\\/");

  // if it doesn't exist, return nothing
  if( (std::string::npos == pos ) )
  {
    return;
  }
  else
  {
    // check an edge case where the string ends in a forward slash "mydir/"
    if( (pos+1) < fileName.length() )
    {
      directory = fileName.substr(0, pos+1);
      return;
    }
  }
}

void GetFileName( const std::string& fullPathName, std::string& fileName )
{
  // look for last slash
  size_t pos = fullPathName.find_last_of("\\/");

  if( std::string::npos == pos )
  {
    fileName = fullPathName;
  }
  else
  {
    fileName = fullPathName.substr(pos,fileName.length());
  }
}

void GetModuleName( const std::string& fileName, std::string& moduleName )
{
  std::string fileNameNoPath;
   GetFileName( fileName , fileNameNoPath );
  size_t pos = fileNameNoPath.find_last_of(".");
  if( std::string::npos == pos )
  {
    moduleName = fileNameNoPath;
  }
  else
  {
    moduleName = fileName.substr(0, pos );
  }
}

void ReportException(  v8::Isolate* isolate, v8::TryCatch* tryCatch)
{
  v8::HandleScope handleScope( isolate );

  v8::String::Utf8Value exception(tryCatch->Exception());
  v8::Handle<v8::Message> message   = tryCatch->Message();

  if (message.IsEmpty())
  {
    // V8 didn't provide any extra information about this error; just
    // print the exception.
    DALI_LOG_SCRIPT_ERROR("%s\n", *exception);
  }
  else
  {

    // Print (filename):(line number): (message).
    v8::String::Utf8Value filename(message->GetScriptResourceName());

    DALI_LOG_SCRIPT_ERROR("\n\n====== Error found in JavaScript: ========= \n");


    int linenum = message->GetLineNumber();
    DALI_LOG_SCRIPT_ERROR("File: %s\n", *filename, linenum, *exception);

     DALI_LOG_SCRIPT_ERROR("Error: :%s\n", *exception );
     DALI_LOG_SCRIPT_ERROR("Line: :%i\n",  linenum );

    // Print line of source code.
    v8::String::Utf8Value sourceline(message->GetSourceLine());

    DALI_LOG_SCRIPT_ERROR("Source: %s\n", *sourceline);

    // Print wavy underline (GetUnderline is deprecated).

    std::stringstream msg;

    int start = message->GetStartColumn();
    for (int i = 0; i < start; i++)
    {
      msg << " ";
    }
    int end = message->GetEndColumn();
    for (int i = start; i < end; i++)
    {
      msg << "↑";
    }

    DALI_LOG_SCRIPT_ERROR("        %s\n", msg.str().c_str());

    v8::String::Utf8Value stack_trace(tryCatch->StackTrace());
    if (stack_trace.length() > 0)
    {
      DALI_LOG_SCRIPT_ERROR("%s\n", *stack_trace);
    }
    DALI_LOG_SCRIPT_ERROR("\n=========================================== \n");

  }
}

std::string GetJavaScriptFunctionName( const char* functionName  )
{
  // @todo if we are 100% decided on lower case, go through
  // every api and manually change the function names to lower case first character
  std::string name( functionName );
  name[0]=tolower( functionName[0] );
  return name;
}

void Version(const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::HandleScope handleScope( args.GetIsolate());

  v8::Handle<v8::String>  ver = v8::String::NewFromUtf8(args.GetIsolate(), v8::V8::GetVersion());

  args.GetReturnValue().Set(ver);
}


std::string v8StringToStdString( const v8::Handle<v8::Value>& value )
{
  v8::String::Utf8Value utf8(value);
  return std::string(*utf8);
}


std::string PropertyNameToJavaScriptName(const std::string& hyphenatedName)
{
  std::string ret;

  ret.reserve(hyphenatedName.size());

  bool capitlizeNext = false ;
  for(unsigned int i = 0; i < hyphenatedName.size(); ++i)
  {
    char c = hyphenatedName[i];
    if(c == '-')
    {
      capitlizeNext = true;
    }
    else
    {
      if(capitlizeNext)
      {
        ret.push_back(std::toupper(c));
        capitlizeNext = false;
      }
      else
      {
        ret.push_back(c);
      }
    }
  }

  return ret;
}



std::string JavaScriptNameToPropertyName(const std::string& camelCase)
{
  std::string ret;

  int countUpper = 0;
  for(unsigned int i = 0; i < camelCase.size(); ++i)
  {
    if(std::isupper(camelCase[i]))
    {
      countUpper++;
    }
  }

  if(countUpper)
  {
    ret.reserve(camelCase.size() + countUpper);

    for(unsigned int i = 0; i < camelCase.size(); ++i)
    {
      char c = camelCase[i];
      if(std::isupper(c))
      {
        ret.push_back('-');
      }

      ret.push_back(std::tolower(c));
    }
  }
  else
  {
    return camelCase ;
  }

  return ret;
}

void ScriptError( const char* function, v8::Isolate* isolate, std::string errorString )
{
  v8::EscapableHandleScope scope( isolate);
  std::string errorMsg = std::string(function) + std::string("(), ") + errorString;

  // log out to DALI_LOG_ERROR first, so we know something has gone wrong
  DALI_LOG_ERROR("%s \n", errorMsg.c_str() );

  // throw a V8 exception, DALi will keep running but we will get a print out
  // of where the error occured in the JavaScript source
  isolate->ThrowException( v8::String::NewFromUtf8( isolate, errorMsg.c_str()) );
}

bool IsBooleanPrimitiveOrObject( const v8::Local<v8::Value>& value )
{
  return ( value->IsBoolean() || value->IsBooleanObject());
}

bool GetBooleanValue( v8::Isolate* isolate, const v8::Local<v8::Value>& value )
{
  v8::EscapableHandleScope scope( isolate); // may not be required.

  if( value->IsBoolean() )
  {
    return value->ToBoolean()->Value();
  }
  else if (value->IsBooleanObject() )
  {
    const v8::Local<v8::BooleanObject> object = v8::Local<v8::BooleanObject>::Cast(value);
    return object->ValueOf();
  }
  DALI_SCRIPT_EXCEPTION(isolate, "no bool found");
  return false;
}

bool IsNumberPrimitiveOrObject( const v8::Local<v8::Value>& value )
{
  return ( value->IsNumber() || value->IsNumberObject());
}

float GetNumberValue( v8::Isolate* isolate, const v8::Local<v8::Value>& value )
{
  v8::EscapableHandleScope scope( isolate); // may not be required.

  if( value->IsNumber() )
  {
    return value->ToNumber()->Value();
  }
  else if (value->IsNumberObject() )
  {
    const v8::Local<v8::NumberObject> object = v8::Local<v8::NumberObject>::Cast(value);
    return object->ValueOf();
  }

  DALI_SCRIPT_EXCEPTION(isolate, "no number found?");
  return 0.f;
}

bool IsStringPrimitiveOrObject( const v8::Local<v8::Value>& value )
{
  return ( value->IsString() || value->IsStringObject());
}

std::string GetStringValue( v8::Isolate* isolate, const v8::Local<v8::Value>& value )
{
  v8::EscapableHandleScope scope( isolate); // may not be required.

  if( value->IsString() )
  {
    return V8Utils::v8StringToStdString(value);
  }
  else if (value->IsStringObject() )
  {
    const v8::Local<v8::StringObject> object = v8::Local<v8::StringObject>::Cast(value);
    return V8Utils::v8StringToStdString( object->ValueOf() );
  }

  DALI_SCRIPT_EXCEPTION(isolate, "no string found?");
  return "";
}


Property::Value GetPropertyValueFromObject( bool& found, v8::Isolate* isolate, const   v8::Local<v8::Value >& value  )
{
  v8::HandleScope handleScope( isolate);

  Property::Value  daliPropertyValue;// creates a property with Property::INVALID

  found = false;

  if( value->IsObject() )
  {
    v8::Local<v8::Object> object = v8::Handle<v8::Object>::Cast( value );

    if( BaseWrappedObject::IsWrappedTypeAPropertyValue( object ) )
    {
      found = true;
      PropertyValueWrapper* propertyWrapper = PropertyValueWrapper::Unwrap( isolate, object );
      return propertyWrapper->GetValue();
    }
    else if( value->IsArray() )
    {
      found = true;
      return PropertyValueWrapper::VectorOrMatrixFromV8Array( isolate, object);//todo check for V8 array / map?
    }
  }
  else if( value->IsBoolean() )
  {
    found = true;
    v8::Local<v8::Boolean> v = value->ToBoolean();
    return Dali::Property::Value(v->Value());
  }
  else if( value->IsNumber() )
  {
    found = true;
    v8::Local<v8::Number> v = value->ToNumber();
    return Dali::Property::Value(static_cast<float>(v->Value()));
  }
  else if( value->IsInt32() )
  {
    found = true;
    v8::Local<v8::Int32> v = value->ToInt32();
    return Dali::Property::Value(static_cast<int>(v->Value()));
  }
  else if ( value->IsUint32() )
  {
    found = true;
    v8::Local<v8::Uint32> v = value->ToUint32();
    return Dali::Property::Value(static_cast<unsigned int>(v->Value()));
  }
  return daliPropertyValue;

}

Property::Map GetPropertyMapFromObject( v8::Isolate* isolate, const v8::Local<v8::Object>& object)
{
  v8::Local<v8::Array> properties = object->GetPropertyNames();
  Property::Map propertyMap;    // empty map

  for(  unsigned int i = 0; i <  properties->Length(); ++i)
  {
    // Get the key
    v8::Local<v8::Value> key = properties->Get( i );
    std::string keyString = v8StringToStdString( key );

    // Get the value
    v8::Local<v8::Value> value = object->Get( key );
    std::string valueString = V8Utils::v8StringToStdString( value );

    propertyMap[ keyString ] = valueString.c_str();

  }
  return propertyMap;
}

Actor GetActorFromObject( v8::Isolate* isolate, bool& found, v8::Local<v8::Object>& object)
{
  v8::HandleScope handleScope( isolate);
  found = false;

  if( BaseWrappedObject::IsWrappedType ( isolate, object, BaseWrappedObject::ACTOR ))
  {
    HandleWrapper* handleWrapper = HandleWrapper::Unwrap( isolate, object );
    return Actor::DownCast( handleWrapper->mHandle );
  }
  return Actor();
}


int GetIntegerParameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args, int defaultValue  )
{
  found = false;
  unsigned int length = args.Length();
  if( index >= length )
  {
    return defaultValue;
  }
  if( args[ index ]->IsInt32() )
  {
    found = true;
    return args[ index ]->Int32Value();
  }
  {
    return defaultValue;
  }
}

float GetFloatParameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args, float defaultValue  )
{
  found = false;
  unsigned int length = args.Length();
  if( index >= length )
  {
    return defaultValue;
  }
  if( args[ index ]->IsNumber() )
  {
    found = true;
    return args[ index ]->NumberValue();
  }
  {
    return defaultValue;
  }
}

std::string GetStringParameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  found = false;
  unsigned int length = args.Length();

  if( index >= length )
  {
    return std::string();
  }
  if( args[ index ]->IsString() )
  {
    found = true;
    return v8StringToStdString( args[ index ]);
  }
  else
  {
    return std::string();
  }
}

bool GetBooleanParameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate);

  found = false;
  unsigned int length = args.Length();
  if( index >= length )
  {
    return false;
  }
  if( args[ index ]->IsBoolean() )
  {
    found = true;
    v8::Local<v8::Boolean> v = args[ index ]->ToBoolean();
    return v->Value();
  }
  else
  {
    return false;
  }
}

Handle GetHandleParameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate);

  found = false;
  unsigned int length = args.Length();
  if( index >= length )
  {
    return Handle();
  }

  if( args[ index ]->IsObject() )
  {
    v8::Local<v8::Object> object = args[ index ]->ToObject();
    v8::Local<v8::External> field = v8::Local<v8::External>::Cast( object->GetInternalField(0) );
    void* ptr = field->Value();
    if( ptr )
    {
        found = true;
        HandleWrapper* wrapper = static_cast< HandleWrapper *>(ptr);
        return wrapper->GetHandle();
    }
  }
  return Handle();
}

Vector2 GetVector2Parameter( unsigned int index,  bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate);
  unsigned int length =  args.Length();
  Vector2 ret;
  found = false;

  if( index < length )
  {
    if( args[ index ]->IsObject() )
    {
      Dali::Property::Value value;
      value = PropertyValueWrapper::ExtractPropertyValue( isolate, args[index], Dali::Property::VECTOR2 );
      if( value.GetType() == Dali::Property::VECTOR2)
      {
        found = true;
        value.Get(ret);
      }
      else
      {
        DALI_SCRIPT_EXCEPTION(isolate, "Missing Vector2 parameter");
      }
    }
  }
  else
  {
    DALI_SCRIPT_EXCEPTION(isolate, "Missing Vector2 parameter");
  }

  return ret;
}

Vector2 GetVector2ParameterFrom2Float( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  Vector2 ret(0.0f,0.0f);
  bool bFound(false);
  unsigned int argCount( args.Length() );

  if( index+2 >= argCount )
  {
    DALI_SCRIPT_EXCEPTION(isolate, "Missing parameter");
  }

  found = true;
  ret.x = V8Utils::GetFloatParameter( index, bFound, isolate, args, 0.0f );
  found = found && bFound;
  ret.y = V8Utils::GetFloatParameter( index+1, bFound, isolate, args, 0.0f );
  found = found && bFound;

  return ret;
}

Vector3 GetVector3Parameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::HandleScope handleScope( isolate);
  unsigned int argCount( args.Length() );
  Vector3 ret;
  found = false;
  if( index < argCount )
  {
    if( args[ index ]->IsObject() )
    {
      Dali::Property::Value value;
      value = PropertyValueWrapper::ExtractPropertyValue( isolate, args[index], Dali::Property::VECTOR3 );
      if( value.GetType() == Dali::Property::VECTOR3)
      {
        found = true;
        value.Get(ret);
      }
      else
      {
        DALI_SCRIPT_EXCEPTION(isolate, "Missing Vector3 parameter");
      }
    }
  }
  else
  {
    DALI_SCRIPT_EXCEPTION(isolate, "Missing Vector3 parameter");

  }

  return ret;
}

Vector4 GetVector4Parameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::HandleScope handleScope( isolate);
  unsigned int argCount( args.Length() );
  Vector4 ret;
  found = false;

  if( index < argCount )
  {
    if( args[ index ]->IsObject() )
    {
      Dali::Property::Value value;
      value = PropertyValueWrapper::ExtractPropertyValue( isolate, args[index], Dali::Property::VECTOR4 );
      if( value.GetType() == Dali::Property::VECTOR4)
      {
        found = true;
        value.Get(ret);
      }
      else
      {
        DALI_SCRIPT_EXCEPTION(isolate, "Missing Vector4 parameter");
      }
    }
  }
  else
  {
    DALI_SCRIPT_EXCEPTION(isolate, "Missing Vector4 parameter");
  }

  return ret;
}


Rect<int> GetRectIntParameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate);

   found = false;
   int length = args.Length() - index;

   // if it's an array read the 2 numbers into a vector2
   if( length == 4 )
   {
     if( args[ 0 + index ]->IsInt32() &&
         args[ 1 + index ]->IsInt32() &&
         args[ 2 + index ]->IsInt32() &&
         args[ 3 + index ]->IsInt32() )
     {
       found = true;
       Rect<int> rect( args[ 0 + index ]->Int32Value(),
                       args[ 1 + index ]->Int32Value(),
                       args[ 2 + index ]->Int32Value(),
                       args[ 3 + index ]->Int32Value() );
       return rect;
     }
   }
   // this will extract a Vector4, if it is a Vector4 or a Javascript array object
   if( args[ index ]->IsObject() )
   {
     Dali::Property::Value value;
     value = PropertyValueWrapper::ExtractPropertyValue( isolate, args[index], Dali::Property::RECTANGLE );
     if( value.GetType() == Dali::Property::RECTANGLE)
     {
       found = true;
       Rect<int> rect;
       value.Get(rect);
       return rect;
     }

     // @todo support vector4 as well?
   }
   return Rect<int>();
}

Actor GetActorParameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  BaseWrappedObject* wrapper = GetWrappedDaliObjectParameter( index, BaseWrappedObject::ACTOR, isolate, args);
  ActorWrapper* actorWrapper = static_cast< ActorWrapper*>( wrapper );
  if( actorWrapper )
  {
    found = true;
    return actorWrapper->GetActor();
  }
  else
  {
    return Actor();
  }
}

Layer GetLayerParameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  Actor actor = GetActorParameter( index, found, isolate, args );
  return Layer::DownCast( actor );
}

Image GetImageParameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  BaseWrappedObject* wrappedObject = V8Utils::GetWrappedDaliObjectParameter( index, BaseWrappedObject::IMAGE, isolate, args );
  if( wrappedObject )
  {
    found = true;
    ImageWrapper* wrapper = static_cast< ImageWrapper *>(wrappedObject);
    return wrapper->GetImage();
  }
  else
  {
    return Image();
  }

}
RenderTask GetRenderTaskParameter( unsigned int paramIndex, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  found = false;
  BaseWrappedObject* wrappedObject = V8Utils::GetWrappedDaliObjectParameter( paramIndex, BaseWrappedObject::RENDER_TASK, isolate, args );
  if( wrappedObject )
  {
    found = true;
    RenderTaskWrapper* wrapper = static_cast< RenderTaskWrapper *>(wrappedObject);
    return wrapper->GetRenderTask();
  }
  else
  {
    return RenderTask(); // empty handle
  }
}


BaseWrappedObject* GetWrappedDaliObjectParameter( unsigned int index,  BaseWrappedObject::Type type,  v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate);
  unsigned int length = args.Length();

  if( index >= length )
  {
    return NULL;
  }

  if( !args[ index ]->IsObject() )
  {
    return NULL;
  }

  v8::Local<v8::Object> object = args[ index ]->ToObject();

  if( BaseWrappedObject::IsWrappedType ( isolate, object, type ))
  {
    v8::Local<v8::External> field = v8::Local<v8::External>::Cast( object->GetInternalField(0) );
    void* ptr = field->Value();
    BaseWrappedObject* wrapper = static_cast< BaseWrappedObject *>(ptr);
    return wrapper;
  }
  return NULL;
}


Property::Value GetPropertyValueParameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate);

  Property::Value  daliPropertyValue;// creates a property with Property::INVALID

  found = false;
  unsigned int length = args.Length();

  if( index >= length )
  {
    return daliPropertyValue;
  }
  v8::Local<v8::Value > value = args[ index ];

  return GetPropertyValueFromObject( found, isolate, value);
}

Property::Map GetPropertyMapParameter( unsigned int index, bool& found, v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate);

  Property::Map propertyMap;    // empty map

  found = false;
  unsigned int length = args.Length();

  if( index >= length )
  {
    return propertyMap;
  }

  if( !args[ index ]->IsObject() )
  {
    return propertyMap;
  }
  found = true;

  // go through each key value pair
  v8::Local<v8::Object> obj = args[ index ]->ToObject();

  return GetPropertyMapFromObject( isolate, obj );

}

void CreatePropertyMap( v8::Isolate* isolate, const Property::Map& map, v8::Local<v8::Object>& object )
{
  v8::HandleScope handleScope( isolate);

  // we're converting a dali property map in to a JavaScript property map
  if( map.Count() == 0 )
  {
    return;
  }

  for( unsigned int index = 0; index < map.Count() - 1; ++index )
  {
    const std::string& key = map.GetKey( index );
    Property::Value& value = map.GetValue( index );
    v8::Local<v8::Value> v8Value;

    switch( value.GetType() )
    {
      case Dali::Property::FLOAT:
      {
        v8Value = v8::Number::New( isolate, value.Get<float>()  );
        break;
      }
      case Dali::Property::BOOLEAN:
      {
        v8Value = v8::Boolean::New(  isolate, value.Get<bool>());
        break;
      }
      case Dali::Property::INTEGER:
      {
        v8Value = v8::Integer::New( isolate, value.Get<int>());
        break;
      }
      case Dali::Property::UNSIGNED_INTEGER:
      {
        v8Value = v8::Integer::New( isolate, value.Get<unsigned int>());
        break;
      }
      case Dali::Property::STRING:
      {
        std::string string = value.Get< std::string >();
        v8Value = v8::String::NewFromUtf8( isolate,  string.c_str());
        break;
      }
      case Dali::Property::VECTOR2:
      {
        // create a vector2
        Vector2 vec = value.Get<Vector2>();
        v8::Local<v8::Array> array= v8::Array::New( isolate, 2 );
        array->Set( 0 , v8::Number::New(isolate, vec.x));
        array->Set( 1 , v8::Number::New(isolate, vec.y));
        v8Value = array;
        break;
      }
      case Dali::Property::VECTOR3:
      {
        // create a vector 3
        Vector3 vec = value.Get<Vector3>();
        v8::Local<v8::Array> array= v8::Array::New( isolate, 3 );
        array->Set( 0 , v8::Number::New(isolate, vec.x));
        array->Set( 1 , v8::Number::New(isolate, vec.y));
        array->Set( 2 , v8::Number::New(isolate, vec.z));
        v8Value = array;
        break;
      }
      case Dali::Property::VECTOR4:
      {
        // create a vector 4
        Vector4 vec = value.Get<Vector4>();
        v8::Local<v8::Array> array= v8::Array::New( isolate, 4 );
        array->Set( 0 , v8::Number::New(isolate, vec.x));
        array->Set( 1 , v8::Number::New(isolate, vec.y));
        array->Set( 2 , v8::Number::New(isolate, vec.z));
        array->Set( 3 , v8::Number::New(isolate, vec.w));
        v8Value = array;
        break;
      }

      default:
      {
        DALI_SCRIPT_EXCEPTION( isolate, "Primitive mismatch \n");
        return;
      }
    }
    object->Set( v8::String::NewFromUtf8( isolate, key.c_str() ), v8Value );
  }
}

void ReadFloatArguments( bool& foundAllArguments, float* data, unsigned int dataSize, const v8::FunctionCallbackInfo< v8::Value >& args, float defaultValue )
{
  foundAllArguments = true;
  unsigned int length = args.Length();

  if( length < dataSize )
  {
    foundAllArguments = false;
  }

  for( unsigned int i = 0; i< dataSize ;i++ )
  {
    if( i < length )
    {
      if( args[ i ]->IsNumber()  )
      {
        data[i] = args[i]->NumberValue();
      }
      else
      {
        data[i] = defaultValue;
        foundAllArguments = false;   // bad argument
      }
    }
    else
    {
      data[i] = defaultValue; // not enough arguments
    }
  }

}

void ReadIntegerArguments( bool& foundAllArguments, int* data, int dataSize, const v8::FunctionCallbackInfo< v8::Value >& args, int defaultValue )
{
  foundAllArguments = true;
  int length = args.Length();
  if( length < dataSize )
  {
    foundAllArguments = false;
  }

  for( int i = 0; i< dataSize ;i++ )
  {
    if( i < length )
    {
      if( args[ i ]->IsInt32()  )
      {
        data[i] = args[i]->Int32Value();
      }
      else
      {
        data[i] = defaultValue;
        foundAllArguments = false;   // bad argument
      }
    }
    else
    {
      data[i] = defaultValue; // not enough arguments
    }
  }

}
} // namespace V8Utils

} // namespace V8Plugin

} // namespace Dali

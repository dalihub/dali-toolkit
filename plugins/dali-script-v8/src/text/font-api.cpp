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
#include "font-api.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <text/font-wrapper.h>
#include <object/property-value-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

namespace // un named namespace
{




} //un named namespace

/***************************************
 * IMAGE  FUNCTIONS
 *
 ****************************************/
Font FontApi::GetFont( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate );

  v8::Local<v8::Object> object = args.This();
  v8::Local<v8::External> field = v8::Local<v8::External>::Cast( object->GetInternalField(0) );
  void* ptr = field->Value();

  FontWrapper* wrapper = static_cast< FontWrapper *>(ptr);
  return wrapper->GetFont();
}
Font FontApi::GetFont( v8::Isolate* isolate, v8::Local<v8::Value>& value )
{
  v8::HandleScope handleScope( isolate );
  v8::Local<v8::Object> object = value->ToObject();
  v8::Local<v8::External> field = v8::Local<v8::External>::Cast( object->GetInternalField(0) );
  void* ptr = field->Value();

  FontWrapper* wrapper = static_cast< FontWrapper *>(ptr);
  return wrapper->GetFont();
}

struct FontParams
{
  typedef enum
  {
    NO_SIZE_SET,
    USE_POINT_SIZE,
    USE_PIXEL_SIZE,
    USE_CAP_SIZE,
  } SizeType;


  FontParams()
  :pointSize( 0 ),
   pixelSize( 0 ),
   capsSize( 0 ),
   sizeType( FontParams::NO_SIZE_SET )
  {
  }

  std::string family;
  std::string style;
  PointSize pointSize;
  PixelSize pixelSize;
  CapsHeight capsSize;
  SizeType sizeType;


};


void ReadFontParameters( v8::Isolate* isolate,
                         v8::Local<v8::Value > options,
                         FontParams& fontParams )
{
  // foont options is an optional parameter passed in which holds
  // optional settings
  // var fontOptions = {
  //  family: "arial",
  //  style:  "bold",
  //  // one of the following
  //  pixelSize: xx
  //  pointSize: xx
  //  capsHeight:xx // height of a capital letter above the baseline for a particular typeface.
  //
  // };
  v8::HandleScope handleScope( isolate );
  if( !options->IsObject() )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter 0 ( font parameters)" );
    return;
  }
  v8::Local<v8::Object> obj = options->ToObject();

  v8::Local<v8::Value> familyValue = obj->Get( v8::String::NewFromUtf8( isolate, "family" ) );
  if( familyValue->IsString() )
  {
    fontParams.family =   V8Utils::v8StringToStdString( familyValue );
  }

  v8::Local<v8::Value> styleValue = obj->Get( v8::String::NewFromUtf8( isolate, "style" ) );
  if( styleValue->IsString() )
  {
    fontParams.style =   V8Utils::v8StringToStdString( styleValue );
  }

  v8::Local<v8::Value> pixelSize = obj->Get( v8::String::NewFromUtf8( isolate, "pixelSize" ) );
  v8::Local<v8::Value> pointSize = obj->Get( v8::String::NewFromUtf8( isolate, "pointSize" ) );
  v8::Local<v8::Value> capsHeight = obj->Get( v8::String::NewFromUtf8( isolate, "capsHeight" ) );

  if( pixelSize->IsUint32() )
  {
    fontParams.pixelSize.value =   pixelSize->ToUint32()->Value();
    fontParams.sizeType = FontParams::USE_PIXEL_SIZE;
  }
  else if( pointSize->IsUint32() )
  {
    fontParams.pointSize.value =   pointSize->ToUint32()->Value();
    fontParams.sizeType = FontParams::USE_POINT_SIZE;
  }
  else if( capsHeight->IsUint32() )
  {
    fontParams.capsSize.value = capsHeight->ToUint32()->Value();
    fontParams.sizeType = FontParams::USE_CAP_SIZE;
  }

}

Font FontApi::New( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  // if no parameters just create a default font
  if ( args.Length() == 0)
  {
    return Font::New();
  }
  FontParams params;

  ReadFontParameters( isolate, args[0], params );

  FontParameters fontParams;
  // construct a dali font parameters object
  switch( params.sizeType)
  {
    case  FontParams::USE_PIXEL_SIZE:
    {
      fontParams = FontParameters( params.family, params.style, params.pixelSize  );
      break;
    }
    case  FontParams::USE_POINT_SIZE:
    {
      fontParams = FontParameters( params.family, params.style, params.pointSize  );
      break;
    }
    case  FontParams::USE_CAP_SIZE:
    {
      fontParams = FontParameters( params.family, params.style, params.capsSize  );
      break;
    }
    default:
    {
      fontParams = FontParameters( params.family, params.style, PointSize(0.f));
      break;
    }
  }
  return Font::New( fontParams );

}


void FontApi::GetFamilyForText( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );


  bool foundString;
  std::string text = V8Utils::GetStringParameter( PARAMETER_0, foundString, isolate, args );
  if( !foundString )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter 0 ( text )" );
    return;
  }
  std::string family = Font::GetFamilyForText( text );

  args.GetReturnValue().Set( v8::String::NewFromUtf8( isolate, family.c_str()) );

}
void FontApi::GetLineHeightFromCapsHeight( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  FontParams params;
  ReadFontParameters( isolate, args[0], params );
  if( params.sizeType != FontParams::USE_CAP_SIZE )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "caps height not found" );
    return;
  }

  PixelSize size = Font::GetLineHeightFromCapsHeight( params.family, params.style, params.capsSize);
  args.GetReturnValue().Set( v8::Integer::New( isolate, size.value ) );
}

void FontApi::GetInstalledFonts( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  bool found( false );
  std::vector<std::string> fontList;

  std::string mode = V8Utils::GetStringParameter( PARAMETER_0, found, isolate, args );

  if( mode == "LIST_APPLICATION_FONTS" )
  {
    fontList = Font::GetInstalledFonts( Font::LIST_APPLICATION_FONTS );
  }
  else if( mode == "LIST_ALL_FONTS")
  {
    fontList = Font::GetInstalledFonts( Font::LIST_ALL_FONTS );
  }
  else  // default
  {
    fontList = Font::GetInstalledFonts( Font::LIST_SYSTEM_FONTS );
  }
  // create a javascript array
  v8::Local<v8::Array> array = v8::Array::New(isolate, fontList.size() );
  for( std::size_t i = 0; i < fontList.size(); i++)
  {
    const char* fontName = fontList[i].c_str();
    array->Set(v8::Integer::New(args.GetIsolate(), i), v8::String::NewFromUtf8( isolate,fontName));
  }

  args.GetReturnValue().Set( array );
}

void FontApi::MeasureTextWidth( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Font font = GetFont( isolate, args );

  //float MeasureTextWidth(const std::string& text, float textHeightPx) const;

  bool found( false );
  std::string text = V8Utils::GetStringParameter( PARAMETER_0, found, isolate, args );
  if(! found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "text not found" );
    return;
  }
  int height = V8Utils::GetIntegerParameter( PARAMETER_1, found, isolate, args, 0 );
  if( !found )
  {
     DALI_SCRIPT_EXCEPTION( isolate, "missing text height" );
     return;
  }
  float width = font.MeasureTextWidth( text, height );

  args.GetReturnValue().Set( v8::Integer::New( isolate, width ) );

}
void FontApi::MeasureTextHeight( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Font font = GetFont( isolate, args );

  //float MeasureTextHeight(const std::string& text, float textHeightPx) const;

  bool found( false );
  std::string text = V8Utils::GetStringParameter( PARAMETER_0, found, isolate, args );
  if(! found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "text not found" );
    return;
  }
  int width= V8Utils::GetIntegerParameter( PARAMETER_1, found, isolate, args, 0 );
  if( !found )
  {
     DALI_SCRIPT_EXCEPTION( isolate, "missing text height" );
     return;
  }
  float height = font.MeasureTextHeight( text, width );

  args.GetReturnValue().Set( v8::Integer::New( isolate, height ) );
}
void FontApi::MeasureText( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Font font = GetFont( isolate, args );

  bool found( false );
  std::string text = V8Utils::GetStringParameter( PARAMETER_0, found, isolate, args );
  if(! found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "text not found" );
    return;
  }

  Vector3 vec3 = font.MeasureText( text );
  Dali::Property::Value value( vec3 );
  v8::Local <v8::Object> object = PropertyValueWrapper::WrapDaliProperty( isolate, value);
  args.GetReturnValue().Set(  object );

}
void FontApi::AllGlyphsSupported( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Font font = GetFont( isolate, args );

  bool found( false );
  std::string text = V8Utils::GetStringParameter( PARAMETER_0, found, isolate, args );
  if(! found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "text not found" );
    return;
  }

  bool supported = font.AllGlyphsSupported( text );
  args.GetReturnValue().Set( v8::Boolean::New(  isolate, supported) );

}

// This one function is use to create a property map, instead of the many individual property
// getters used by the C++ Dali API.
// propertyMap
// {
//    name:
//    style:
//    pointSize:
//    pixelSize:
//    lineHeight:
//    ascender:
//    underlineThickness:
//    underlinePosition:
//    isDefaultSystemFont:
//    isDefaultSystemSize:
// }

void FontApi::GetProperties( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Font font = GetFont( isolate, args );

  v8::Local<v8::Object> keyObject = v8::Object::New( isolate );

  keyObject->Set( v8::String::NewFromUtf8( isolate, "family" ), v8::String::NewFromUtf8( isolate, font.GetName().c_str() ) );
  keyObject->Set( v8::String::NewFromUtf8( isolate, "style" ), v8::String::NewFromUtf8( isolate, font.GetStyle().c_str() ) );
  keyObject->Set( v8::String::NewFromUtf8( isolate, "pointSize" ),  v8::Integer::New( isolate, font.GetPointSize() ) );
  keyObject->Set( v8::String::NewFromUtf8( isolate, "pixelSize" ),  v8::Integer::New( isolate, font.GetPixelSize() ) );
  keyObject->Set( v8::String::NewFromUtf8( isolate, "lineHeight" ),  v8::Integer::New( isolate, font.GetLineHeight() ) );
  keyObject->Set( v8::String::NewFromUtf8( isolate, "ascender" ),  v8::Integer::New( isolate, font.GetAscender() ) );
  keyObject->Set( v8::String::NewFromUtf8( isolate, "underlineThickness" ),  v8::Integer::New( isolate, font.GetUnderlineThickness() ) );
  keyObject->Set( v8::String::NewFromUtf8( isolate, "underlinePosition" ),  v8::Integer::New( isolate, font.GetUnderlinePosition()) );
  keyObject->Set( v8::String::NewFromUtf8( isolate, "isDefaultSystemFont" ),  v8::Boolean::New( isolate, font.IsDefaultSystemFont() ) );
  keyObject->Set( v8::String::NewFromUtf8( isolate, "isDefaultSystemSize" ),  v8::Boolean::New( isolate,  font.IsDefaultSystemSize() ) );

  args.GetReturnValue().Set( keyObject );

}

void FontApi::GetMetrics( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Font font = GetFont( isolate, args );

  bool found( false );
  std::string text = V8Utils::GetStringParameter( PARAMETER_0, found, isolate, args );

  if(! found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "character not found" );
    return;
  }

  Text textObject( text );

  Font::Metrics metric = font.GetMetrics( textObject[0] );

  v8::Local<v8::Object> keyObject = v8::Object::New( isolate );
  keyObject->Set( v8::String::NewFromUtf8( isolate, "advance" ), v8::Integer::New( isolate, metric.GetAdvance() ) );
  keyObject->Set( v8::String::NewFromUtf8( isolate, "bearing" ), v8::Integer::New( isolate, metric.GetBearing() ) );
  keyObject->Set( v8::String::NewFromUtf8( isolate, "width" ),   v8::Integer::New( isolate, metric.GetWidth() ) );
  keyObject->Set( v8::String::NewFromUtf8( isolate, "height" ),  v8::Integer::New( isolate, metric.GetHeight() ) );

  args.GetReturnValue().Set( keyObject );

}


void FontApi::PointsToPixels( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  bool found(false);
  int pointSize= V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 );
  if( !found )
  {
      DALI_SCRIPT_EXCEPTION( isolate, "missing pointSize" );
      return;
  }
  args.GetReturnValue().Set( v8::Integer::New( isolate, Font::PointsToPixels(pointSize) ) );

}
void FontApi::PixelsToPoints( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  bool found(false);
  int pixelSize= V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 );
  if( !found )
  {
      DALI_SCRIPT_EXCEPTION( isolate, "missing pixelSize" );
      return;
  }
  args.GetReturnValue().Set( v8::Integer::New( isolate, Font::PixelsToPoints(pixelSize) ) );
}



} // namespace V8Plugin

} // namespace Dali

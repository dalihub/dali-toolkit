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
#include "font-wrapper.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <dali-wrapper.h>
#include <text/font-api.h>
#include <shared/api-function.h>
#include <shared/object-template-helper.h>

namespace Dali
{

namespace V8Plugin
{

namespace // un-named name space
{

/**
 * Contains a list of all functions that can be called on the font object
 */
const ApiFunction FontFunctionTable[]=
{
    /**************************************
    * Font API (in order of font.h)
    **************************************/
    { "MeasureTextWidth",               FontApi::MeasureTextWidth },
    { "MeasureTextHeight",              FontApi::MeasureTextHeight },
    { "MeasureText",                    FontApi::MeasureText },
    { "AllGlyphsSupported",             FontApi::AllGlyphsSupported },
    { "GetProperties",                  FontApi::GetProperties }, // replace all getters
    { "GetMetrics",                     FontApi::GetMetrics },
};

const unsigned int FontFunctionTableCount = sizeof(FontFunctionTable)/sizeof(FontFunctionTable[0]);

/**
 * Contains a list of all functions that can be called
 */
const ApiFunction StaticFontFunctionTable[]=
{
    /**************************************
    * Static font functions, called without a font object
    **************************************/
    { "GetFamilyForText",               FontApi::GetFamilyForText },
    { "GetLineHeightFromCapsHeight",    FontApi::GetLineHeightFromCapsHeight },
    { "GetInstalledFonts",              FontApi::GetInstalledFonts },
    { "PointsToPixels",                 FontApi::PointsToPixels },
    { "PixelsToPoints",                 FontApi::PixelsToPoints },
};

const unsigned int StaticFontFunctionTableCount = sizeof(StaticFontFunctionTable)/sizeof(FontFunctionTable[0]);

} //un-named space


FontWrapper::FontWrapper( const Dali::Font& font, GarbageCollectorInterface& gc )
: BaseWrappedObject( BaseWrappedObject::FONT , gc )
{
    mFont = font;
}

v8::Handle<v8::Object> FontWrapper::WrapFont(v8::Isolate* isolate, const Dali::Font& font )
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  objectTemplate = MakeFontTemplate( isolate );

  // create an instance of the template
  v8::Local<v8::Object> localObject = objectTemplate->NewInstance();

  // create the Font wrapper
  FontWrapper* pointer =  new FontWrapper( font, Dali::V8Plugin::DaliWrapper::Get().GetDaliGarbageCollector() );

  // assign the JavaScript object to the wrapper.
  pointer->SetJavascriptObject( isolate, localObject );

  printf("Created Font!\n");
  return handleScope.Escape( localObject );
}


v8::Handle<v8::ObjectTemplate> FontWrapper::MakeFontTemplate( v8::Isolate* isolate )
{
  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::ObjectTemplate> objTemplate = v8::ObjectTemplate::New();

  objTemplate->SetInternalFieldCount( BaseWrappedObject::FIELD_COUNT );

  // add our function properties
  ObjectTemplateHelper::InstallFunctions( isolate, objTemplate, FontFunctionTable, FontFunctionTableCount );

  return handleScope.Escape( objTemplate );
}

void FontWrapper::NewFont( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate);

  if(!args.IsConstructCall())
  {
    DALI_SCRIPT_EXCEPTION( isolate, "Font constructor called without 'new'");
    return;
  }

  // attribs can be passed by value
  Dali::Font font = FontApi::New( args );
  if( !font )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad font parameters\n");
    return;
  }

  v8::Local<v8::Object> localObject = WrapFont( isolate, font );
  args.GetReturnValue().Set( localObject );
}

Font FontWrapper::GetFont()
{
  return mFont;
}

v8::Handle<v8::Object> FontWrapper::GetStaticFontObject(v8::Isolate* isolate)
{
  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::ObjectTemplate> objTemplate = v8::ObjectTemplate::New();

   // add our functions properties
  ObjectTemplateHelper::InstallFunctions( isolate, objTemplate, StaticFontFunctionTable, StaticFontFunctionTableCount );

  v8::Local<v8::Object> localObject = objTemplate->NewInstance();

  return handleScope.Escape( localObject );

 }


} // namespace V8Plugin

} // namespace Dali

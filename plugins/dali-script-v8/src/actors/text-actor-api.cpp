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
#include "text-actor-api.h"

// EXTERNAL INCLUDES
#include <dali/public-api/text/text-actor-parameters.h>

// INTERNAL INCLUDES
#include <object/handle-wrapper.h>
#include <v8-utils.h>
#include <object/property-value-wrapper.h>
#include <text/font-api.h>


namespace Dali
{

namespace V8Plugin
{

namespace //unnamed name space
{

struct TextActorParametersInternal
{
  TextActorParametersInternal()
  : fontDetection( true )
  {
  }
  bool fontDetection;
  Font font;
};


TextActor GetTextActor( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  HandleWrapper* handleWrapper = HandleWrapper::Unwrap( isolate, args.This() );
  return TextActor::DownCast( handleWrapper->mHandle );
}


void GetTextOptions( v8::Isolate* isolate,
                          v8::Local<v8::Value > options,
                          TextActorParametersInternal& textParams )
{
  //      fontDetection: true / false  ( default  true)
  //      font: dali font object
  v8::HandleScope handleScope( isolate );

  if( options->IsObject() )
  {
    v8::Local<v8::Object> obj = options->ToObject();

    v8::Local<v8::Value> fontDetect = obj->Get( v8::String::NewFromUtf8( isolate, "fontDetection" ) );
    if( fontDetect->IsBoolean() )
    {

      textParams.fontDetection = fontDetect->ToBoolean()->Value();
    }

    v8::Local<v8::Value> fontValue = obj->Get( v8::String::NewFromUtf8( isolate, "font" ) );
    if( fontValue->IsObject() )
    {
      textParams.font = FontApi::GetFont( isolate, fontValue );
    }

  }
}

}

/**
 * @constructor
 * @for TextActor
 * @method TextActor
 * @param {String} text
 * @param {Object} [textOptions] data
 * Options text options struct
 * @param {Boolean} [textOptions.fontDetection]
 * if true the fontDetection is used to make sure the text is displayed.
 * E.g. if the current font used by the text-actor does not support certain characters
 * it will find a new font that does. Default = true.
 * @param {Object}  [textOptions.font]
 * Dali font object
 * @return {Object} TextActor
 */
Actor TextActorApi::New( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  //
  // TextActor( text, options (optional) )
  //
  // options =
  // {
  //    font:   font
  //    fontDetection: true / false   ( default  true)
  // }

  // get the text (if passed in)

  bool found( false );
  std::string text = V8Utils::GetStringParameter( PARAMETER_0, found, isolate, args );

  TextActorParametersInternal params;
  TextActor actor;

  GetTextOptions( isolate, args[1], params );

  TextStyle style;

  if( params.font )
  {
    style.SetFontName( params.font.GetName() );
    style.SetFontStyle( params.font.GetStyle() );
    style.SetFontPointSize( PointSize(params.font.GetPointSize()));

  }
  TextActorParameters textActorParameters( style, params.fontDetection? TextActorParameters::FONT_DETECTION_OFF:TextActorParameters::FONT_DETECTION_ON );

  actor = TextActor::New( text, textActorParameters );

  return actor;

}

/**
 * Set text to the natural size of the text string.
 *
 * After this method the text actor always uses the natural size of the text
 * when SetText is called unless SetSize is called to override the size.
 *
 * @for TextActor
 * @method setToNaturalSize
 */
void TextActorApi::SetToNaturalSize( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  TextActor textActor = GetTextActor( isolate, args );
  textActor.SetToNaturalSize();
}

} // namespace V8Plugin

} // namespace Dali

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
#include "javascript-application-options.h"

// EXTERNAL INCLUDES
#include <cstring>

// INTERNAL INCLUDES
#include <v8-utils.h>

namespace DaliNodeAddon
{

namespace
{

struct StereoInfo
{
  const char* const name;
  Dali::ViewMode mode;
};
StereoInfo StereoModeTable[] = {
    { "mono",                 Dali::MONO},
    { "stereo-horizontal",    Dali::STEREO_HORIZONTAL },
    { "stereo-vertical",      Dali::STEREO_VERTICAL },
    { "stereo-interlaced",    Dali::STEREO_INTERLACED },
};

const unsigned int numberViewModes = sizeof( StereoModeTable ) / sizeof( StereoModeTable[0] );

bool GetViewMode( const std::string& modeString, Dali::ViewMode& mode )
{
  for( unsigned int i = 0; i < numberViewModes; ++i )
  {
    const StereoInfo& info (StereoModeTable[i]);

    if( strcmp ( modeString.c_str() , info.name ) == 0 )
    {
      mode = info.mode;
      return true;
    }
  }
  // mode not found
  mode = Dali::MONO;
  return false;
}


// Note we can't parse the enviroment options for window width / height because
// adaptor which holds the environment option class has not been created
// and we can't create it, until we have a window
bool ParseWindowOptions( v8::Isolate* isolate, const v8::Local<v8::Object>& obj,  WindowOptions& window )
{
  v8::HandleScope scope(isolate);

  v8::Local<v8::Value> xValue = obj->Get( v8::String::NewFromUtf8( isolate, "x" ) );
  v8::Local<v8::Value> yValue = obj->Get( v8::String::NewFromUtf8( isolate, "y" ) );
  v8::Local<v8::Value> widthValue = obj->Get( v8::String::NewFromUtf8( isolate, "width" ) );
  v8::Local<v8::Value> heightValue = obj->Get( v8::String::NewFromUtf8( isolate, "height" ) );
  v8::Local<v8::Value> nameValue = obj->Get( v8::String::NewFromUtf8( isolate, "name" ) );
  v8::Local<v8::Value> transparencyValue = obj->Get( v8::String::NewFromUtf8( isolate, "transparent" ) );

  // if x,y are optional
  if( xValue->IsUint32() )
  {
    window.positionSize.x = xValue->ToUint32()->Value();
  }
  if( yValue->IsUint32() )
  {
    window.positionSize.y = yValue->ToUint32()->Value();
  }

  // width and height are optional but will only accept them if they are both set
  if( widthValue->IsUint32() &&  heightValue->IsUint32() )
  {
    window.positionSize.width = widthValue->ToUint32()->Value();
    window.positionSize.height = heightValue->ToUint32()->Value();
  }

  // get the window name
  if( nameValue->IsString() )
  {
    window.name = Dali::V8Plugin::V8Utils::v8StringToStdString( nameValue );
  }
  else
  {
    window.name ="DALi application";
  }

  if( transparencyValue->IsBoolean() )
  {
    window.transparent = transparencyValue->ToBoolean()->Value();
  }
  return true;
}

bool ParseStereoScopicOptions( v8::Isolate* isolate, const v8::Local<v8::Object>& stereoObject,  StereoScopicOptions& options )
{
  v8::HandleScope scope(isolate);

  v8::Local<v8::Value> modeValue = stereoObject->Get( v8::String::NewFromUtf8( isolate, "stereoscopic-mode" ) );
  v8::Local<v8::Value> stereoBaseValue = stereoObject->Get( v8::String::NewFromUtf8( isolate, "stereo-base" ) );

  if( !modeValue->IsString() )
  {
    return true;
  }

  std::string mode = Dali::V8Plugin::V8Utils::v8StringToStdString( modeValue );
  bool ok = GetViewMode( mode,  options.viewMode);
  if( !ok )
  {
    return false;
  }
  if( stereoBaseValue->IsNumber() )
  {
    options.stereoBase = stereoBaseValue->ToNumber()->Value();
  }

  return true;
}

} // unnamed namespace

bool GetApplicationOptions(const v8::FunctionCallbackInfo<v8::Value>& args, ApplicationOptions& options )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope scope(isolate);
  bool ok( false );

  if( !args[ 0 ]->IsObject() )
  {
    return false;
  }

  v8::Local<v8::Object> object = args[ 0 ]->ToObject();

  // get the window settings
  v8::Local<v8::Value> windowValue= object->Get( v8::String::NewFromUtf8( isolate, "window" ) );
  if( windowValue->IsObject() )
  {
    ok = ParseWindowOptions( isolate, windowValue->ToObject(), options.window );
    if( !ok )
    {
      return false; // missing window size
    }
  }

  // get the stereoscopic settings
  v8::Local<v8::Value> stereoValue= object->Get( v8::String::NewFromUtf8( isolate, "view-mode" ) );
  if( stereoValue->IsObject() )
  {
    ok = ParseStereoScopicOptions( isolate,  stereoValue->ToObject(), options.stereo );
    if( !ok )
    {
      return false;  // incorrect stereoscopic mode
    }
  }

  // get the style sheet
  v8::Local<v8::Value> stylesheetValue= object->Get( v8::String::NewFromUtf8( isolate, "style-sheet" ) );
  if( stylesheetValue->IsString() )
  {
    options.stylesheet = Dali::V8Plugin::V8Utils::v8StringToStdString( stylesheetValue );
  }

  return true;
}

} // namespace DaliNodeAddon

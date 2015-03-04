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
#include "image-attributes-api.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <image/image-attributes-wrapper.h>
#include <object/property-value-wrapper.h>
#include <shared/base-wrapped-object.h>

namespace Dali
{

namespace V8Plugin
{

ImageAttributes ImageAttributesApi::GetImageAttributesFromObject( v8::Isolate* isolate, v8::Local<v8::Object> object )
{
  v8::HandleScope handleScope( isolate);

  if( BaseWrappedObject::IsWrappedType ( isolate, object, BaseWrappedObject::IMAGE_ATTRIBUTES ))
  {
    v8::Local<v8::External> field = v8::Local<v8::External>::Cast( object->GetInternalField(0) );
    void* ptr = field->Value();
    BaseWrappedObject* wrappedObject = static_cast< BaseWrappedObject *>(ptr);
    ImageAttributesWrapper* wrapper = static_cast< ImageAttributesWrapper*>( wrappedObject );;
    return  wrapper->GetImageAttributes();
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad image attributes url");
    return ImageAttributes();
  }

}

/***************************************
 * IMAGE ATTRIBUTES FUNCTIONS
 *
 ****************************************/
ImageAttributes ImageAttributesApi::New( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  // two combinations of constructor
  // 1 = no parameters
  // 2 = width, height
  bool foundParams( false );
  int params[2];

  V8Utils::ReadIntegerArguments( foundParams, &params[0],2,args,0);
  if( !foundParams )
  {
    if( args.Length() != 0 )
    {
      DALI_SCRIPT_EXCEPTION( isolate, " ImageAttributes::New invalid params");
      return Dali::ImageAttributes();
    }
    return Dali::ImageAttributes::New();
  }
  else
  {
    return  Dali::ImageAttributes::New( params[0], params[1] );
  }

}

ImageAttributes& ImageAttributesApi::GetImageAttributes( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Local<v8::Object> object = args.This();
  v8::Local<v8::External> field = v8::Local<v8::External>::Cast( object->GetInternalField(0) );
  void* ptr = field->Value();

  ImageAttributesWrapper* wrapper = static_cast< ImageAttributesWrapper *>(ptr);
  return wrapper->GetImageAttributes();
}

void ImageAttributesApi::SetSize( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  ImageAttributes& attribs( GetImageAttributes( isolate, args ));

  bool found;
  Vector2 size = V8Utils::GetVector2Parameter( PARAMETER_0, found, isolate, args );
  if (!found)
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid size parameter" );
    return;
  }
  else
  {
    attribs.SetSize( size );
  }
}

void ImageAttributesApi::SetScalingMode( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  ImageAttributes& attribs( GetImageAttributes( isolate, args ));

  bool found(false);
  int value = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0);
  if( !found  )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid scaling mode parameter");
    return;
  }
  if( value!= ImageAttributes::ShrinkToFit ||
      value!= ImageAttributes::ScaleToFill ||
      value!= ImageAttributes::FitWidth ||
      value!= ImageAttributes::FitHeight )
  {
     DALI_SCRIPT_EXCEPTION( isolate, "invalid scaling mode parameter");
     return;
  }

  attribs.SetScalingMode(static_cast<ImageAttributes::ScalingMode >( value) );
}

void ImageAttributesApi::SetOrientationCorrection( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  ImageAttributes& attribs( GetImageAttributes( isolate, args ));

  bool parameterFound(false);
  bool value = V8Utils::GetBooleanParameter( PARAMETER_0 , parameterFound, isolate, args );

  if( !parameterFound )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "boolean parameter not found");
    return;
  }
  attribs.SetOrientationCorrection( value );
}

void ImageAttributesApi::GetWidth( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  ImageAttributes& attribs( GetImageAttributes( isolate, args ));

  args.GetReturnValue().Set( v8::Integer::New( isolate, attribs.GetWidth() ) );
}

void ImageAttributesApi::GetHeight( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  ImageAttributes& attribs( GetImageAttributes( isolate, args ));

  args.GetReturnValue().Set( v8::Integer::New( isolate, attribs.GetHeight() ) );

}

void ImageAttributesApi::GetSize( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  ImageAttributes& attribs( GetImageAttributes( isolate, args ));

  Vector2 vec( attribs.GetSize() );
  Dali::Property::Value value( vec );

  v8::Local <v8::Object> object = PropertyValueWrapper::WrapDaliProperty( isolate, value);
  args.GetReturnValue().Set(  object );
}

void ImageAttributesApi::GetScalingMode( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  ImageAttributes& attribs( GetImageAttributes( isolate, args ));

  args.GetReturnValue().Set( v8::Integer::New( isolate, attribs.GetScalingMode() ) );

}

void ImageAttributesApi::GetOrientationCorrection( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  ImageAttributes& attribs( GetImageAttributes( isolate, args ));

  args.GetReturnValue().Set(  v8::Boolean::New(  isolate, attribs.GetOrientationCorrection() ) );

}


} // namespace V8Plugin

} // namespace Dali

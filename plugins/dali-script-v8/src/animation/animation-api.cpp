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
#include "animation-api.h"
#include "path-wrapper.h"

// EXTERNAL INCLUDES
#include <cstring> // for strcmp
#include <dali/integration-api/debug.h>


// INTERNAL INCLUDES
#include <v8-utils.h>
#include <animation/animation-wrapper.h>
#include <object/property-value-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

namespace // un named namespace
{

// @todo think about alternative ways of passing around
struct AlphaFuncStruct
{
  const char* const name;
  AlphaFunction alphaFunc;
};
/**
 * Contains a list of alpha functions that can be used.
 */
const AlphaFuncStruct AlphaFunctionTable[]=
{
 {"default"               , AlphaFunctions::Default              },
 {"linear"                , AlphaFunctions::Linear               },
 {"square"                , AlphaFunctions::Square               },
 {"reverse"               , AlphaFunctions::Reverse              },
 {"easeIn"                , AlphaFunctions::EaseIn               },
 {"easeOut"               , AlphaFunctions::EaseOut              },
 {"easeInOut"             , AlphaFunctions::EaseInOut            },
 {"easeInSine"            , AlphaFunctions::EaseInSine           },
 {"easeOutSine"           , AlphaFunctions::EaseOutSine          },
 {"easeInOutSine"         , AlphaFunctions::EaseInOutSine        },
 {"easeInSine33"          , AlphaFunctions::EaseInSine33         },
 {"easeOutSine33"         , AlphaFunctions::EaseOutSine33        },
 {"easeInOutSine33"       , AlphaFunctions::EaseInOutSine33      },
 {"easeInOutSine50"       , AlphaFunctions::EaseInOutSine50      },
 {"easeInOutSine60"       , AlphaFunctions::EaseInOutSine60      },
 {"easeInOutSine70"       , AlphaFunctions::EaseInOutSine70      },
 {"easeInOutSine80"       , AlphaFunctions::EaseInOutSine80      },
 {"easeInOutSine90"       , AlphaFunctions::EaseInOutSine90      },
 {"doubleEaseInOutSine60" , AlphaFunctions::DoubleEaseInOutSine60},
 {"easeOutQuint50"        , AlphaFunctions::EaseOutQuint50       },
 {"easeOutQuint80"        , AlphaFunctions::EaseOutQuint80       },
 {"bounce"                , AlphaFunctions::Bounce               },
 {"bounceBack"            , AlphaFunctions::BounceBack           },
 {"easeInBack"            , AlphaFunctions::EaseInBack           },
 {"easeOutBack"           , AlphaFunctions::EaseOutBack          },
 {"easeInOutBack"         , AlphaFunctions::EaseInOutBack        },
 {"sin"                   , AlphaFunctions::Sin                  },
 {"sin2x"                 , AlphaFunctions::Sin2x                }
};
const unsigned int AlphaFunctionTableCount = sizeof(AlphaFunctionTable)/sizeof(AlphaFunctionTable[0]);
const char* const DEFAULT_ALPHA_NAME = "default";
static AlphaFunction DEFAULT_ALPHA_FUNCTION = AlphaFunctions::Default;



AlphaFunction GetAlphaFunction( const std::string& alphaFuncName )
{
  // This will normally get called just a few times during the application, so no point in doing anything clever
  for( unsigned int i = 0; i < AlphaFunctionTableCount; i++)
  {
    const AlphaFuncStruct& alphaStruct( AlphaFunctionTable[i] );

    if( std::strcmp( alphaStruct.name , alphaFuncName.c_str() ) == 0 )
    {
      return alphaStruct.alphaFunc;
    }
  }

  DALI_LOG_ERROR("Failed to find alpha func |%s| \n", alphaFuncName.c_str() );
  return AlphaFunctions::Default;
}

const char* const GetAlphaFunctionName(  AlphaFunction alphaFunc )
{
  // This may get called 3 times during the application, so no point
  // in doing anything clever

  for( unsigned int i = 0; i < AlphaFunctionTableCount; i++)
  {
    const AlphaFuncStruct& alphaStruct( AlphaFunctionTable[i] );


    if( alphaStruct.alphaFunc  == alphaFunc )
    {
      return alphaStruct.name;
    }
  }
  return "default";
}


struct AnimationParameters
{
  AnimationParameters( const Animation& anim)
  : propertyIndex( Property::INVALID_INDEX  ),
    alphaFunction( DEFAULT_ALPHA_FUNCTION),
    delay( 0.f ),
    duration(anim.GetDuration()),
    optionsFound( false )
  {
  }

  Handle target;
  Property::Index propertyIndex;
  Property::Value value;
  KeyFrames keyFrames;
  AlphaFunction alphaFunction;
  float delay;
  float duration;
  bool optionsFound;
};

void GetAnimationOptions( v8::Isolate* isolate,
                          v8::Local<v8::Value > options,
                          AnimationParameters& animParams )
{
  // animation options is an optional parameter passed in which holds
  // optional settings
  // var animOptions = {
  //  alpha: "Bounce",
  //  delay: 5,
  //  duration: 20
  // };
  v8::HandleScope handleScope( isolate );

  if( options->IsObject() )
  {
    v8::Local<v8::Object> obj = options->ToObject();
    v8::Local<v8::Value> alphaValue = obj->Get( v8::String::NewFromUtf8( isolate, "alpha" ) );
    if( alphaValue->IsString() )
    {
      animParams.optionsFound = true;
      std::string alphaName = V8Utils::v8StringToStdString( alphaValue );
      animParams.alphaFunction = GetAlphaFunction( alphaName );
    }

    v8::Local<v8::Value> delayValue = obj->Get( v8::String::NewFromUtf8( isolate, "delay" ) );
    if( delayValue->IsNumber() )
    {
      animParams.optionsFound = true;
      v8::Local<v8::Number> num = delayValue->ToNumber();
      animParams.delay = num->Value();
    }

    v8::Local<v8::Value> durationValue = obj->Get( v8::String::NewFromUtf8( isolate, "duration" ) );
    if( durationValue->IsNumber() )
    {
      animParams.optionsFound = true;
      v8::Local<v8::Number> num = durationValue->ToNumber();
      animParams.duration = num->Value();
    }

  }
}
KeyFrames GetKeyFrames( v8::Isolate* isolate, v8::Local<v8::Value > keyFrameArray )
{
  // keyframe object is an array of
  // {
  //   float: progress
  //   value: property value ( position/ rotation etc)
  //   alpha: function
  //
  v8::HandleScope handleScope( isolate );

  if( !keyFrameArray->IsObject() || !keyFrameArray->IsArray() )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "missing keyframe array" );
    return KeyFrames();
  }

  KeyFrames keyframes = KeyFrames::New();

  v8::Local<v8::Array> array = v8::Local<v8::Array>::Cast( keyFrameArray );
  for( uint32_t i=0; i < array->Length() ; ++i)
  {
    v8::Handle<v8::Value> arrayItem =  array->Get( i );

    if(!arrayItem->IsObject() )
    {
      DALI_SCRIPT_EXCEPTION( isolate, "missing keyframe object" );
      return KeyFrames();
    }
    v8::Handle<v8::Object> keyFrameObject = arrayItem->ToObject();

    // get keyframe.progress
    v8::Handle<v8::Value> progress = keyFrameObject->Get(v8::String::NewFromUtf8( isolate, "progress"));
    if( !progress->IsNumber() )
    {
      DALI_SCRIPT_EXCEPTION( isolate, "keyframe missing progress property" );
      return keyframes;
    }

    // get keyframe.value
    bool found(false);
    v8::Handle<v8::Value> propertyValue = keyFrameObject->Get(v8::String::NewFromUtf8( isolate, "value"));
    Property::Value value =  V8Utils::GetPropertyValueFromObject( found, isolate, propertyValue );
    if( !found )
    {
      DALI_SCRIPT_EXCEPTION( isolate, "keyframe missing value property" );
      return keyframes;
    }

    // get keyframe.alpha
    v8::Handle<v8::Value> alphaValue = keyFrameObject->Get(v8::String::NewFromUtf8( isolate, "alpha"));
    if( alphaValue->IsString() )
    {
      std::string alphaName = V8Utils::v8StringToStdString( alphaValue );
      AlphaFunction alphaFunction = GetAlphaFunction( alphaName );
      keyframes.Add( progress->NumberValue(), value, alphaFunction );
    }
    else
    {
      keyframes.Add( progress->NumberValue(), value );
    }

  }
  return keyframes;

}
bool GetAnimationParameters(  v8::Isolate* isolate,
                              const v8::FunctionCallbackInfo< v8::Value >& args,
                              AnimationParameters& animParams,
                              AnimationApi::AnimationParameterType type)
{
  // used for things like anim.AnimateBy(  myImageActor, property-name,  property-value (or Javascript number array));
  // 1 extract property handle from param1.
  // 2 extract property name from param2  ( can be in the format "u-color" or "uColor"
  // 3 extract PropertyValue from param3
  // 4 extract animation options ( delay, duration, alpha func)

  // 1 extract HANDLE
  bool foundHandle;
  animParams.target = V8Utils::GetHandleParameter( PARAMETER_0, foundHandle, isolate, args );
  if( !foundHandle )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter 0 (Handle)" );
    return false;
  }

  // 2 extract property name
  bool foundPropName;
  std::string propertyName = V8Utils::GetStringParameter( PARAMETER_1, foundPropName, isolate, args );
  if( !foundPropName )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter 1 ( PropertyName )" );
    return false;
  }
  // try both properties with dashes and without
  Property::Index index = animParams.target.GetPropertyIndex( propertyName );

  if( index == Property::INVALID_INDEX )
  {
    // convert the property name from "uColor" to "u-color"
    std::string convetedName = V8Utils::JavaScriptNameToPropertyName( propertyName );
    index = animParams.target.GetPropertyIndex( convetedName );
  }

  animParams.propertyIndex = index;

  if( type == AnimationApi::PROPERTY_VALUE )
  {
    // 3 extract property value
    bool foundPropValue( false );
    animParams.value = V8Utils::GetPropertyValueParameter( PARAMETER_2, foundPropValue, isolate, args );
    if( !foundPropValue )
    {
      DALI_SCRIPT_EXCEPTION( isolate, "bad parameter 2 ( value )" );
      return false;
    }
  }
  else  // type == KEYFRAMES
  {
    animParams.keyFrames = GetKeyFrames(isolate, args[2]);
  }
  // 4 extract animation options
  GetAnimationOptions( isolate, args[3], animParams );

  return true;
}

Animation GetAnimation( v8::Isolate* isolate, const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::HandleScope handleScope( isolate );

  v8::Local<v8::Object> object = args.This();
  v8::Local<v8::External> field = v8::Local<v8::External>::Cast( object->GetInternalField( 0 ) );
  void* ptr = field->Value();

  AnimationWrapper* wrapper = static_cast<AnimationWrapper *>( ptr );
  return wrapper->GetAnimation();
}


} // un-named namespace

/**
 * Constructor
 *
 * @constructor
 * @for Animation
 * @method Animation
 * @param {float} duration
 *
 */
Animation AnimationApi::New( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  bool found( false );
  float value = V8Utils::GetFloatParameter( PARAMETER_0, found, isolate, args, 1.f /* default */);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter" );
    return Animation();
  }
  // get the duration
  return Animation::New( value );
}

/**
 * Set the animation duration.
 * @method setDuration
 * @for Animation
 * @param {float} duration in seconds
 *
 */
void AnimationApi::SetDuration( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Animation anim = GetAnimation( isolate, args );

  bool found( false );
  float value = V8Utils::GetFloatParameter( PARAMETER_0, found, isolate, args, 1.f /* default */);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter" );
    return;
  }
  anim.SetDuration( value );
}
/**
 * Get the animation duration.
 * @method getDuration
 * @for Animation
 * @return {float} duration in seconds
 *
 */
void AnimationApi::GetDuration( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Animation anim = GetAnimation( isolate, args );

  args.GetReturnValue().Set( v8::Number::New( isolate, anim.GetDuration() ) );

}
/**
 * Set whether the animation will loop.
 * @method setLooping
 * @for Animation
 * @param {bool} looping enabled
 *
 */
void AnimationApi::SetLooping( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Animation anim = GetAnimation( isolate, args );

  bool found( false );
  bool value = V8Utils::GetBooleanParameter( PARAMETER_0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter" );
  }
  else
  {
    anim.SetLooping( value );
  }

}
/**
 * Query whether the animation will loop.
 * @method isLooping
 * @for Animation
 * @return {bool} looping enabled
 *
 */
void AnimationApi::IsLooping( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Animation anim = GetAnimation( isolate, args );

  args.GetReturnValue().Set( v8::Boolean::New( isolate, anim.IsLooping() ) );
}

/**
 * Set the end action of the animation.
 *
 * This action is performed when the animation ends.
 * Default end action is bake
 * @method setEndAction
 * @for Animation
 * @param {integer} bake mode
 * @example
 *       anim.setEndAction( dali.ANIMATION_BAKE ); // When the animation ends, the animated property values are saved.
 *       anim.setEndAction( dali.ANIMATION_DISCARD ); //  When the animation ends, the animated property values are forgotten.
 *       anim.setEndAction( dali.ANIMATION_BAKE_FINAL ); // If the animation is stopped, the animated property values are saved as if the animation had run to completion, otherwise behaves like Bake.
 */
void AnimationApi::SetEndAction( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Animation anim = GetAnimation( isolate, args );

  bool found( false );
  int value = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter" );
  }
  else
  {
    anim.SetEndAction( static_cast<Animation::EndAction>( value ) );
  }
}

/**
 * Returns the end action of the animation.

 * @method getEndAction
 * @for Animation
 * @return {integer} bake mode
 *
 * There are 3 different bake modes
 * @example
 *     dali.ANIMATION_BAKE  // When the animation ends, the animated property values are saved.
 *     dali.ANIMATION_DISCARD // When the animation ends, the animated property values are forgotten.
 *     dali.ANIMATION_BAKE_FINAL  // If the animation is stopped, the animated property values are saved as if the animation had run to completion, otherwise behaves like Bake.
 */
void AnimationApi::GetEndAction( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Animation anim = GetAnimation( isolate, args );

  args.GetReturnValue().Set( v8::Integer::New( isolate, anim.GetEndAction() ) );
}

/**
 * Set the disconnect action of the animation.
 * If any of the animated property owners are disconnected from the stage, this action is performed.
 * Default disconnection action is BakeFinal.
 * @method setDisconnectAction
 * @for Animation
 * @param {integer} end mode
 *
 * There are 3 different end modes
 * @example
 *     dali.ANIMATION_BAKE  // When the animation is destroyed, the animated property values are saved.
 *     dali.ANIMATION_DISCARD // When the animation is destroyed, the animated property values are forgotten.
 *     dali.ANIMATION_BAKE_FINAL  // When the animation is destroyed, the animated property values are saved as if the animation had run to completion, otherwise behaves like Bake.
 */
void AnimationApi::SetDisconnectAction( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Animation anim = GetAnimation( isolate, args );

  bool found( false );
  int value = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter" );
  }
  else
  {
    anim.SetDisconnectAction( static_cast<Animation::EndAction>( value ) );
  }
}

/**
 * Returns the disconnect action of the animation.
 * @method getDisconnectAction
 * @for Animation
 * @return {integer} end mode
 */
void AnimationApi::GetDisconnectAction( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Animation anim = GetAnimation( isolate, args );

  args.GetReturnValue().Set( v8::Integer::New( isolate, anim.GetDisconnectAction() ) );
}
/**
 * Set the default alpha function for an animation.
 * @method setDefaultAlphaFunction
 * @for Animation
 * @param {string} alpha function
 */
void AnimationApi::SetDefaultAlphaFunction( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Animation anim = GetAnimation( isolate, args );

  bool found( false );
  std::string alphaFunc = V8Utils::GetStringParameter( PARAMETER_0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter" );
  }
  else
  {
    AlphaFunction func = GetAlphaFunction( alphaFunc );
    anim.SetDefaultAlphaFunction( func );
  }

}
/**
 * Get the default alpha function for an animation.
 * @method getDefaultAlphaFunction
 * @for Animation
 * @return {string} alpha function
 */
void AnimationApi::GetDefaultAlphaFunction( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Animation anim = GetAnimation( isolate, args );

  std::string alphaName = GetAlphaFunctionName(  anim.GetDefaultAlphaFunction() );


  args.GetReturnValue().Set( v8::String::NewFromUtf8( isolate, alphaName.c_str() ) );
}

/**
 * Get the current progress of the animation.
 * @method getCurrentProgress
 * @for Animation
 * @return {float} The current progress as a normalized value between [0..1].
 *
 */
void AnimationApi::GetCurrentProgress( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Animation anim = GetAnimation( isolate, args );

  args.GetReturnValue().Set( v8::Number::New( isolate, anim.GetCurrentProgress() ) );
}

/**
 * Specifies an speed factor for the animation.
 *
 * The speed factor is a multiplier of the normal velocity of the animation. Values between [0,1] will
 * slow down the animation and values above one will speed up the animation. It is also possible to specify a negative multiplier
 * to play the animation in reverse.
 *
 * @method setSpeedFactor
 * @for Animation
 * @param {float}  value which will multiply the velocity.
 * @example
 *     anim.setSpeedFactor(2);
 *     anim.play();             // plays the animation twice as fast
 *
 *
 *     anim.setSpeedFactor(0.5);
 *     anim.play();             // plays the animation half speed
 *
 */
void AnimationApi::SetSpeedFactor( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Animation anim = GetAnimation( isolate, args );

  bool found( false );
  float speedFactor = V8Utils::GetFloatParameter( PARAMETER_0, found, isolate, args, 0.f );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "float parameter missing" );
  }
  else
  {
    anim.SetSpeedFactor( speedFactor );
  }

}

/**
 * Retrieve the speed factor of the animation
 *
 *
 * @method getSpeedFactor
 * @for Animation
 * @return {float} speed factor
 */
void AnimationApi::GetSpeedFactor( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Animation anim = GetAnimation( isolate, args );

  args.GetReturnValue().Set( v8::Number::New( isolate, anim.GetSpeedFactor() ) );
}

/**
 * Set the playing range.
 * Animation will play between the values specified.
 * Both values ( range.x and range.y ) should be between 0-1,
 * otherwise they will be ignored.
 * If the range provided is not in proper order ( minimum,maximum), it will be reordered.
 * @method setPlayRange
 * @for Animation
 * @param {Object} Range
 * @param {Float} Range.start
 * @param {Float} Range.end
 * @example
 *     var range = {  start:0.1, end:0.6 };
 *     anim.setPlayRange( range );
 */
void AnimationApi::SetPlayRange( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Animation anim = GetAnimation( isolate, args );
  Vector2 range(0,0);

  if( args.Length() != 1 )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "missing / invalid parameters" );
    return;
  }
  v8::Local<v8::Value > rangeValue = args[0];
  if( !rangeValue->IsObject() )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid parameters" );
    return;
  }
  v8::Local<v8::Object> obj = rangeValue->ToObject();
  v8::Local<v8::Value> startValue = obj->Get( v8::String::NewFromUtf8( isolate, "start" ) );
  v8::Local<v8::Value> endValue = obj->Get( v8::String::NewFromUtf8( isolate, "end" ) );

  if( startValue->IsNumber() &&  endValue->IsNumber())
  {
        range.x= startValue->ToNumber()->Value();
        range.y = endValue->ToNumber()->Value();
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "missing start/end value" );
    return;
  }

  anim.SetPlayRange( range );

}

/**
 * Get the playing range.
 * @method getPlayRange
 * @for Animation
 * @return {Object} Range with { start: ,  end: } properties.
 */
void AnimationApi::GetPlayRange( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::EscapableHandleScope handleScope( isolate );
  Animation anim = GetAnimation( isolate, args );


  v8::Local<v8::Object> rangeObject = v8::Object::New( isolate );

  Vector2 range = anim.GetPlayRange();

 // set device id
  rangeObject->Set( v8::String::NewFromUtf8( isolate, "start"), v8::Number::New( isolate,range.x ));

   // set state
  rangeObject->Set( v8::String::NewFromUtf8( isolate, "end"), v8::Number::New( isolate,range.y ));

  args.GetReturnValue().Set( rangeObject );
}


/**
 * Sets the progress of the animation.
 * The animation will play (or continue playing) from this point. The progress
 * must be in the 0-1 interval or in the play range interval if defined ( See SetPlayRange ),
 * otherwise, it will be ignored.
 *
 * @method setCurrentProgress
 * @for Animation
 * @param {float}  progress The new progress as a normalized value between [0,1] or between the
 * play range if specified.
 */
void AnimationApi::SetCurrentProgress( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Animation anim = GetAnimation( isolate, args );

  bool found( false );

  float progress = V8Utils::GetFloatParameter( PARAMETER_0, found, isolate, args, 0.f );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "float parameter missing" );
    return;
  }
  anim.SetCurrentProgress( progress );

}

/**
 *
 *  Play the animation from a given point.
 * The progress must be in the 0-1 interval or in the play range interval if defined ( See SetPlayRange ),
 * otherwise, it will be ignored.
 * @method playFrom
 * @for Animation
 * @param {float} progress A value between [0,1], or between the play range if specified, form where the animation should start playing
 */
void AnimationApi::PlayFrom( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Animation anim = GetAnimation( isolate, args );

  bool found( false );

  float progress = V8Utils::GetFloatParameter( PARAMETER_0, found, isolate, args, 0.f );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "float parameter missing" );
    return;
  }
  anim.PlayFrom( progress );

}

/**
 * Play the animation
 * @method play
 * @for Animation
 */
void AnimationApi::Play( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Animation anim = GetAnimation( isolate, args );
  anim.Play();
}
/**
 * Pause the animation
 * @method pause
 * @for Animation
 */
void AnimationApi::Pause( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Animation anim = GetAnimation( isolate, args );
  anim.Pause();
}
/**
 * Stop the animation
 * @method stop
 * @for Animation
 */
void AnimationApi::Stop( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Animation anim = GetAnimation( isolate, args );
  anim.Stop();
}
/**
 * Clear the animation
 * This disconnects any objects that were being animated, effectively stopping the animation.
 * @method clear
 * @for Animation
 */
void AnimationApi::Clear( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Animation anim = GetAnimation( isolate, args );
  anim.Clear();
}


void AnimationApi::Animate( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Animation anim = GetAnimation( isolate, args );
  AnimationParameters animParams( anim );
  bool found(false);

  //Get actor
  Dali::Actor actor = V8Utils::GetActorParameter( PARAMETER_0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "Missing actor parameter");
    return;
  }

  //Get path
  Dali::Path path;
  BaseWrappedObject* wrapper = V8Utils::GetWrappedDaliObjectParameter( PARAMETER_1, BaseWrappedObject::PATH, isolate, args);
  PathWrapper* pathWrapper = static_cast< PathWrapper*>( wrapper );
  if( !pathWrapper )
  {
    return;
  }
  path = pathWrapper->GetPath();

  //Get forward vector
  bool bFound(false);
  Vector3 forward = V8Utils::GetVector3Parameter( PARAMETER_2, bFound, isolate, args );
  if( !bFound )
  {
    return;
  }

  //Get animation options
  GetAnimationOptions( isolate, args[3], animParams );
  if( animParams.optionsFound )
  {
    anim.Animate( actor, path, forward, animParams.alphaFunction, TimePeriod( animParams.delay, animParams.duration) );
  }
  else
  {
    anim.Animate( actor, path, forward );
  }
}

/**
 *
 * Animate a property value by a relative amount.
 *
 * The effect will start & end when the animation begins & ends.
 * @method animateBy
 * @for Animation
 * @param {Object} target object that contains a property to be animated (e.g. myActor )
 * @param {String} property name (e.g. "position" )
 * @param {Object} relativeValue The property value will change by this amount.
 * @param {Object} [options] Animation options.
 * @param {Float} [options.delay] amount to delay the start of the animation in seconds
 * @param {Float} [options.duration] duration of the animation
 * @param {String} [options.alpha] Animation alpha function (e.g. "linear")
 *
 * @example
 *
 *     // animation x position
 *     var anim = new dali.Animation( 1 );
 *     anim.animateBy( imageActor,"positionX", 30 );
 *     anim.play();
 *
 *     // animate x,y,z position with the optional animation options
 *     var options = {
 *        delay: 3,     // 3 second delay before starting
 *        duration: 5,  // 5 second duration
 *        alpha:"easeInOutSine"   // Speeds up and slows to a gradual stop
 *     }
 *
 *     anim.animateBy( imageActor,"position", [100,200,0], options );
 *
 */
void AnimationApi::AnimateBy( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Animation anim = GetAnimation( isolate, args );
  AnimationParameters animParams( anim );

  // grab all the parameters
  bool ok = GetAnimationParameters( isolate, args, animParams, AnimationApi::PROPERTY_VALUE);
  if( !ok )
  {
    // GetAnimationParameters will log
    return;
  }

  if( animParams.optionsFound )
  {
    anim.AnimateBy( Property( animParams.target, animParams.propertyIndex ),
                              animParams.value,
                              animParams.alphaFunction,
                              TimePeriod( animParams.delay, animParams.duration) );
  }
  else
  {
    anim.AnimateBy( Property( animParams.target, animParams.propertyIndex ), animParams.value );
  }

}

/**
 *
 * Animate a property to a destination value.
 *
 * The effect will start & end when the animation begins & ends.
 * @method animateTo
 * @for Animation
 * @param {Object} target object that contains a property to be animated (e.g. myActor )
 * @param {String} property name (e.g. "position" )
 * @param {Object} destinationValue The property value will changed to this value
 * @param {Object} [options] Animation options.
 * @param {Float} [options.delay] amount to delay the start of the animation in seconds
 * @param {Float} [options.duration] duration of the animation
 * @param {String} [options.alpha] Animation alpha function (e.g. "linear")
 *
 * @example
 *
 *     var anim = new dali.Animation( 1 );
 *     anim.animateTo( imageActor,"positionX", 30 );
 *     anim.play();
 *
 *
 *     // with the optional animation options object
 *     var options = {
 *        delay: 3,     // 3 second delay before starting
 *        duration: 5,  // 5 second duration
 *        alpha:"easeInOutSine"   // Speeds up and slows to a gradual stop
 *     }
 *
 *     anim.animateTo( imageActor,"position", [100,200,0], options );
 *
 */
void AnimationApi::AnimateTo( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Animation anim = GetAnimation( isolate, args );
  AnimationParameters animParams( anim );

  // grab all the parameters
  bool ok = GetAnimationParameters( isolate, args, animParams, AnimationApi::PROPERTY_VALUE );
  if( !ok )
  {
    // GetAnimationParameters will log
    return;
  }

  if( animParams.optionsFound )
  {

    anim.AnimateTo( Property( animParams.target,animParams.propertyIndex ),
                                  animParams.value,
                                  animParams.alphaFunction,
                                  TimePeriod( animParams.delay, animParams.duration) );
  }
  else
  {
    anim.AnimateTo( Property( animParams.target, animParams.propertyIndex ), animParams.value );
  }
}

/**
 *
 * Animate a property between keyframes.
 *
 * The effect will start & end when the animation begins & ends.
 * @method animateBetween
 * @for Animation
 * @param {Object} target object that contains a property to be animated (e.g. myActor )
 * @param {String} property name (e.g. "position" )
 * @param {Object} keyframes array of keyframe objects
 * @param {Object} [options] Animation options.
 * @param {Float} [options.delay] amount to delay the start of the animation in seconds
 * @param {Float} [options.duration] duration of the animation
 * @param {String} [options.alpha] Animation alpha function (e.g. "linear")
 *
 *
 * @example
 *
 *  create some keyframes to move an actor around a square, and return to the start
 * </br >
 *  <img src="../assets/img/keyframe-animation.png">
 *
 *
 *     var keyframes = [
 *     {
 *       progress:0.0,
 *       value: [0,0,0]
 *     },
 *     {
 *       progress:0.25,
 *       value: [500,0,0]
 *     },
 *
 *     {
 *       progress:0.5,
 *       value: [500,500,0]
 *     },
 *     {
 *       progress:0.75,
 *       value: [0,500,0]
 *     },
 *     {
 *       progress:1.0,
 *       value: [0,0,0]
 *     } ];
 *
 *
 *     anim.animateBetween( imageActor,"position", keyframes );
 *
 */
void AnimationApi::AnimateBetween( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  Animation anim = GetAnimation( isolate, args );
  AnimationParameters animParams( anim );

  // grab all the parameters
  bool ok = GetAnimationParameters( isolate, args, animParams, AnimationApi::KEYFRAMES );
  if( !ok )
  {
    // GetAnimationParameters will log
    return;
  }

  // if animation options exist...
  if( animParams.optionsFound )
  {

    anim.AnimateBetween( Property( animParams.target,animParams.propertyIndex ),
                                  animParams.keyFrames,
                                  animParams.alphaFunction,
                                  TimePeriod( animParams.delay, animParams.duration) );
  }
  else
  {
    anim.AnimateBetween( Property( animParams.target, animParams.propertyIndex ), animParams.keyFrames );
  }
}

/**
 * show an actor during the animation.
 *
 * This is a helper, which simulates animating the visibility property of an actor
 * with zero duration ( it is just a boolean).
 * e.g. it performs  anim.AnimateTo( actor,"visible",true, { delay:delay: duration:0 } );
 * @method show
 * @for Animation
 * @param {Object} Actor
 * @param {float} delay until the actor is shown
 */
void AnimationApi::Show( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Animation anim = GetAnimation( isolate, args );
  bool found( false );

  Actor actor = V8Utils::GetActorParameter( PARAMETER_0, found, isolate, args );
  if( !found)
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter 0 actor" );
    return;
  }
  // get the duration
  float delay = V8Utils::GetFloatParameter( PARAMETER_1, found, isolate, args, 1.f /* default */);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter 1 delay" );
    return;
  }
  anim.Show( actor, delay );

}

/**
 * hide an actor during the animation.
 *
 * This is a helper, which simulates animating the visibility property of an actor
 * with zero duration ( it is just a boolean).
 * e.g. it performs  anim.AnimateTo( actor,"visible",false, { delay:delay: duration:0 } );
 * @method hide
 * @for Animation
 * @param {Object} Actor
 * @param {float} delay until the actor is hidden
 */
void AnimationApi::Hide( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Animation anim = GetAnimation( isolate, args );
  bool found( false );
  Actor actor = V8Utils::GetActorParameter( PARAMETER_0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter 0 actor" );
    return;
  }

  // get the duration
  float delay = V8Utils::GetFloatParameter( PARAMETER_1, found, isolate, args, 1.f /* default */);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "bad parameter 1 delay" );
    return;
  }
  anim.Hide( actor, delay );
}




} // namespace V8Plugin

} // namespace Dali

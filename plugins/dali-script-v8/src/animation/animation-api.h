#ifndef __DALI_V8PLUGIN_ANIMATION_API_H__
#define __DALI_V8PLUGIN_ANIMATION_API_H__

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


// EXTERNAL INCLUDES
#include <v8.h>
#include <dali/public-api/animation/animation.h>


namespace Dali
{

namespace V8Plugin
{

namespace AnimationApi
{
  /**
   * Constructor
   */
  Animation New( const v8::FunctionCallbackInfo< v8::Value >& args );


  /**
   * Animation API see animation.h for a description
   */
  void SetDuration( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetDuration( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetLooping( const v8::FunctionCallbackInfo< v8::Value >& args );
  void IsLooping( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetEndAction( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetEndAction( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetDisconnectAction( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetDisconnectAction( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetDefaultAlphaFunction( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetDefaultAlphaFunction( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetCurrentProgress( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetSpeedFactor( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetSpeedFactor( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetPlayRange( const v8::FunctionCallbackInfo< v8::Value >& args );
  void GetPlayRange( const v8::FunctionCallbackInfo< v8::Value >& args );
  void SetCurrentProgress( const v8::FunctionCallbackInfo< v8::Value >& args );
  void PlayFrom( const v8::FunctionCallbackInfo< v8::Value >& args );
  void Play( const v8::FunctionCallbackInfo< v8::Value >& args );
  void Pause( const v8::FunctionCallbackInfo< v8::Value >& args );
  void Stop( const v8::FunctionCallbackInfo< v8::Value >& args );
  void Clear( const v8::FunctionCallbackInfo< v8::Value >& args );
  void Animate( const v8::FunctionCallbackInfo< v8::Value >& args );
  void AnimateBy( const v8::FunctionCallbackInfo< v8::Value >& args );
  void AnimateTo( const v8::FunctionCallbackInfo< v8::Value >& args );
  void AnimateBetween( const v8::FunctionCallbackInfo< v8::Value >& args );
  void Show( const v8::FunctionCallbackInfo< v8::Value >& args );
  void Hide( const v8::FunctionCallbackInfo< v8::Value >& args );

  enum AnimationParameterType
  {
    PROPERTY_VALUE,
    KEYFRAMES
  };

}; // namespace AnimationApi

} // namespace V8Plugin

} // namespace Dali

#endif // header __DALI_V8PLUGIN_ANIMATION_API_H__

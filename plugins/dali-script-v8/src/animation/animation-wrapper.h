#ifndef __DALI_V8PLUGIN_ANIMATION_WRAPPER_H__
#define __DALI_V8PLUGIN_ANIMATION_WRAPPER_H__

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

// INTERNAL INCLUDES
#include <shared/base-wrapped-object.h>
#include <signals/signal-manager.h>

namespace Dali
{

namespace V8Plugin
{

/**
 * Animation  wrapper.
 * Provides access to Animation specific functionality
 */
class AnimationWrapper : public BaseWrappedObject
{

public:

  AnimationWrapper( const Animation& animation,
                GarbageCollectorInterface& gc );

  virtual ~AnimationWrapper();

  /**
   * @brief Creates a new Animation wrapped inside a Javascript Object.
   * @param[in] args v8 function call arguments interpreted
   */
  static void NewAnimation( const v8::FunctionCallbackInfo< v8::Value >& args);

  /**
   * Wraps an animation
   */
  static v8::Handle<v8::Object> WrapAnimation(v8::Isolate* isolate, const Dali::Animation& );


  // The AnimationAttribute ObjectTemplate, we cache templates so we don't have
  // keep generating them everytime we want to create an Animation
  static v8::Persistent<v8::ObjectTemplate> mAnimationTemplate;

  Animation GetAnimation();
  virtual SignalManager* GetSignalManager() { return &mSignalManager;}

private:

  Animation mAnimation;           ///< animation object
  SignalManager mSignalManager;   ///< signal manager

  static v8::Handle<v8::ObjectTemplate> MakeAnimationTemplate( v8::Isolate* isolate );
  static v8::Local<v8::ObjectTemplate> GetAnimationTemplate( v8::Isolate* isolate );

};

} // namespace V8Plugin

} // namespace Dali

#endif // __DALI_V8PLUGIN_ANIMATION_WRAPPER_H__

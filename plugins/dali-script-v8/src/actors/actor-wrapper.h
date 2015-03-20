#ifndef __DALI_V8PLUGIN_ACTOR_WRAPPER_H__
#define __DALI_V8PLUGIN_ACTOR_WRAPPER_H__

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
#include <dali/public-api/actors/actor.h>

// INTERNAL INCLUDES
#include <object/handle-wrapper.h>

namespace Dali
{

namespace V8Plugin
{


/**
 * Wraps a Dali Actor.
 */
class ActorWrapper : public HandleWrapper
{

public:

  /**
   * Actor type used an index
   */
  enum ActorType
  {
    UNKNOWN_ACTOR = -1,
    ACTOR       = 0,
    IMAGE_ACTOR  =1,
    MESH_ACTOR   =2,
    LAYER_ACTOR  =3,
    CAMERA_ACTOR =4,
    LIGHT_ACTOR  =5,
    TEXT_LABEL   =6
  };

  /**
   * Constructor
   * @param actor DALi actor
   * @param gc garbage collection interface
   */
  ActorWrapper( Actor actor,
                GarbageCollectorInterface& gc );

  /**
   * destructor
   */
  virtual ~ActorWrapper()
  {
  }

  /**
   * @brief Creates a new Actor wrapped inside a Javascript Object.
   * @note: the actor type ie 'ImageActor' is expected to be the name of the callee function.
   * @param[in] args v8 function call arguments interpreted
   */
  static void NewActor( const v8::FunctionCallbackInfo< v8::Value >& args);

  /**
   * @brief Wraps an actor of a given type
   */
  static v8::Handle<v8::Object> WrapActor(v8::Isolate* isolate, Dali::Actor actor,ActorType actorType);

  /**
   * @brief Wraps an actor, the type is looked up from the actor
   */
  static v8::Handle<v8::Object> WrapActor(v8::Isolate* isolate, Dali::Actor actor );

  // The Actor ObjectTemplates.
  static v8::Persistent<v8::ObjectTemplate> mActorTemplate;
  static v8::Persistent<v8::ObjectTemplate> mImageActorTemplate;
  static v8::Persistent<v8::ObjectTemplate> mMeshActorTemplate;
  static v8::Persistent<v8::ObjectTemplate> mCameraActorTemplate;
  static v8::Persistent<v8::ObjectTemplate> mLayerActorTemplate;
  static v8::Persistent<v8::ObjectTemplate> mTextLabelTemplate;

  /**
   * @return the wrapped actor
   */
  Actor GetActor();

  /**
   * @return they actor type
   */
  static ActorWrapper::ActorType GetActorType( const std::string& name );

private:

  /**
   * Helper to make the actor template
   *
   */
  static v8::Handle<v8::ObjectTemplate> MakeDaliActorTemplate( v8::Isolate* isolate, ActorType actorType );

  /**
   * Helper, get an actor template given an actor type
   */
  static v8::Local<v8::ObjectTemplate> GetActorTemplate( v8::Isolate* isolate, ActorType type );

  Actor mActor;

};

} // namespace V8Plugin

} // namespace Dali

#endif // header

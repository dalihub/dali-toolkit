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
#include "actor-api.h"

// EXTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/text-controls/text-label.h>

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <actors/actor-wrapper.h>
#include <animation/path-constrainer-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

namespace  // unanmed namespace
{
Actor GetActor( v8::Isolate* isolate, const v8::FunctionCallbackInfo<v8::Value>& args )
{
  HandleWrapper* handleWrapper = HandleWrapper::Unwrap( isolate, args.This() );
  return Actor::DownCast( handleWrapper->mHandle );
}
} //unanmed namespace


namespace TextLabelApi
{
 Actor New( const v8::FunctionCallbackInfo< v8::Value >& args )
 {
   return Dali::Toolkit::TextLabel::New();
 }
}

/***************************************
 * ACTOR API FUNCTIONS
 ****************************************/
/**
 * Constructor
 *
 * @for Actor
 * @constructor
 * @method Actor
 * @return {Object} actor
 */
Actor ActorApi::New( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  return Actor::New();
}

/**
 * get the actors unique id
 *
 * @for Actor
 * @method getId
 * @return {Integer} id
 */
void ActorApi::GetId( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Actor actor = GetActor( isolate, args );

  args.GetReturnValue().Set( v8::Integer::New( isolate, actor.GetId() ) );
}

/**
 * Query whether an actor is the root actor, which is owned by the Stage
 *
 * @for Actor
 * @method isRoot
 * @return {Boolean} true if it is root
 *
 */
void ActorApi::IsRoot( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Actor actor = GetActor( isolate, args );

  args.GetReturnValue().Set( v8::Boolean::New( isolate, actor.IsRoot() ) );
}

/**
 *
 * Query whether the actor is connected to the Stage.
 * When an actor is connected, it will be directly or indirectly parented to the root Actor.
 * The root Actor is provided automatically by dali.stage, and is always considered to be connected.
 *
 * @for Actor
 * @method onStage
 * @return {Boolean} True if the actor is connected to the Stage
 */
void ActorApi::OnStage( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Actor actor = GetActor( isolate, args );

  args.GetReturnValue().Set( v8::Boolean::New( isolate, actor.OnStage() ) );
}

/**
 * Query whether an actor is a layer
 *
 * @for Actor
 * @method isLayer
 * @return {Boolean} true if it is a layer
 */
void ActorApi::IsLayer( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Actor actor = GetActor( isolate, args );

  args.GetReturnValue().Set( v8::Boolean::New( isolate, actor.IsLayer() ) );
}

/**
 * Gets the layer in which the actor is present.
 *
 * @for Actor
 * @method getLayer
 * @return {Object} Layer
 */
void ActorApi::GetLayer( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Actor actor = GetActor( isolate, args );
  Layer layer = actor.GetLayer();
  if( layer ) // actors don't always have a layer
  {
    v8::Handle < v8::Object > wrappedLayer = ActorWrapper::ActorWrapper::WrapActor( isolate, layer, ActorWrapper::LAYER_ACTOR );
    args.GetReturnValue().Set( wrappedLayer );
  }
  // else return an empty object
}

/**
 * Adds a child Actor to this Actor.
 *
 * NOTE! if the child already has a parent, it will be removed from old parent
 * and reparented to this actor. This may change childs position, color, shader effect,
 * scale etc as it now inherits them from this actor
 *
 * Pre-conditions
 * - The child actor is not the same as the parent actor.
 * - The actor is not the Root actor

 * Once added The child will be referenced by its parent. This means that the child will be kept alive,
 * even if the handle passed into this method is reset or destroyed.
 *
 * @for Actor
 * @method add
 * @param {Object} Actor
 */
void ActorApi::AddActor( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Actor parent = GetActor( isolate, args );
  bool found(false);
  Actor child = V8Utils::GetActorParameter( PARAMETER_0, found, isolate, args );
  if( found )
  {
    parent.Add( child );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "child parameter missing" );
  }
}

/**
 * Removes a child Actor from this Actor.
 *
 * If the actor was not a child of this actor, this is a no-op.
 *
 * Preconditions:
 * -  The child actor is not the same as the parent actor.
 *
 * @for Actor
 * @param{Object} Actor the child actor
 * @method Remove
 */
void ActorApi::RemoveActor( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Actor parent = GetActor( isolate, args );
  bool found( false );
  Actor child = V8Utils::GetActorParameter( PARAMETER_0, found, isolate, args );

  if( found )
  {
    parent.Remove( child );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "child parameter missing" );
  }
}

/**
 * Checks whether an Actor is equal to this Actor.
 *
 * @for Actor
 * @method iIsEqualTo
 * @param {Object} Actor
 */
void ActorApi::IsEqualTo( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Actor self = GetActor( isolate, args );
  bool found( false );

  Actor actor = V8Utils::GetActorParameter( PARAMETER_0, found, isolate, args );
  if( found )
  {
    args.GetReturnValue().Set( v8::Boolean::New( isolate, (actor == self) ) );
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "actor parameter missing" );
  }
}

/** Removes an actor from its parent.
 *
 * If the actor has no parent, this method does nothing.
 *
 * @for Actor
 * @method Unparent
 */
void ActorApi::Unparent( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Actor actor = GetActor( isolate, args );
  actor.Unparent();
}

/**
 * get number of child actors
 *
 * @for Actor
 * @method getChildCount
 * @return {Integer} count
 */
void ActorApi::GetChildCount( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Actor actor = GetActor( isolate, args );

  args.GetReturnValue().Set( v8::Integer::New( isolate, actor.GetChildCount() ) );
}

/**
 * Retrieve and child actor by index.
 *
 * @for Actor
 * @method getChildAt
 * @param {Integer} actor index
 * @return {Object} actor on success, empty actor handle if not found
 */
void ActorApi::GetChildAt( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Actor parent = GetActor( isolate, args );
  bool found( false );
  int id = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "Integer parameter missing" );
    return;
  }
  Actor childActor = parent.GetChildAt( id );
  if( childActor )
  {
    // wrap the child
    v8::Handle < v8::Object > wrappedActor = ActorWrapper::WrapActor( isolate, childActor );
    args.GetReturnValue().Set( wrappedActor );
  }
}

/**
 * Search through this actor's hierarchy for an actor with the given name
 * The actor itself is also considered in the search
 *
 * @for Actor
 * @method findChildByName
 * @param {String} actor name
 * @return {Object} actor on success, empty actor handle if not found
 */
void ActorApi::FindChildByName( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Actor parent = GetActor( isolate, args );
  bool found( false );
  std::string name = V8Utils::GetStringParameter( PARAMETER_0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "string parameter missing" );
    return;
  }
  Actor childActor = parent.FindChildByName( name );
  if( childActor )
  {
    // wrap the child
    v8::Handle < v8::Object > wrappedLayer = ActorWrapper::WrapActor( isolate, childActor );
    args.GetReturnValue().Set( wrappedLayer );
  }
}

/**
 * Search through this actor's hierarchy for an actor with the given unique ID.
 * The actor itself is also considered in the search
 *
 * @for Actor
 * @method findChildById
 * @param {Integer} id
 * @return {Object} actor on success, empty actor handle if not found
 */
void ActorApi::FindChildById( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Actor parent = GetActor( isolate, args );

  bool found( false );
  int id = V8Utils::GetIntegerParameter( PARAMETER_0, found, isolate, args, 0 );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "Integer parameter missing" );
    return;
  }
  Actor childActor = parent.FindChildById( id );
  if( childActor )
  {
    // wrap the child
    v8::Local < v8::Object > wrappedLayer = ActorWrapper::WrapActor( isolate, childActor );
    args.GetReturnValue().Set( wrappedLayer );
  }
}


/**
 * retrieve the actor's parent.
 *
 * @for Actor
 * @method getParent
 * @return {Object} actor on success, empty actor handle if actor has no parent
 */
void ActorApi::GetParent( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Actor actor = GetActor( isolate, args );
  Actor parent = actor.GetParent();

  if( parent )
  {
    v8::Local < v8::Object > wrappedLayer = ActorWrapper::WrapActor( isolate, parent );
    args.GetReturnValue().Set( wrappedLayer );
  }
}
/**
 * Converts screen coordinates into the actor's coordinate system using the default camera.
 *
 * The actor coordinates are relative to the top-left (0.0, 0.0, 0.5)
 *
 * @example
 *    var local = actor.screenToLocal( [ 10, 53 ]);
 *    var xPos = local.x;
 *    var yPos = local.y;
 *
 *
 * @for Actor
 * @method screenToLocal
 * @param {Object}  ScreenCoordinates array of 2 objects
 * @return {Object} local coordinates object with x,y properties
 */
void ActorApi::ScreenToLocal( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Actor actor = GetActor( isolate, args );

  //ool ScreenToLocal(float& localX, float& localY, float screenX, float screenY) const;
  bool found( false );

  int argCount( args.Length() );
  Vector2 vector;

  if( argCount == 1 )
  {
    vector = V8Utils::GetVector2Parameter( PARAMETER_0, found, isolate, args );
    if( !found )
    {
      DALI_SCRIPT_EXCEPTION( isolate, "invalid parameters (x,y)" );
      return;
    }
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "invalid parameters (x,y)" );
    return;
  }
  float localX, localY;
  actor.ScreenToLocal( localX, localY, vector.x, vector.y );

  v8::Local < v8::Object > localCoordinates = v8::Object::New( isolate );

  localCoordinates->Set( v8::String::NewFromUtf8( isolate, "x" ), v8::Number::New( isolate, localX ) );
  localCoordinates->Set( v8::String::NewFromUtf8( isolate, "y" ), v8::Number::New( isolate, localY ) );

  args.GetReturnValue().Set( localCoordinates );

}

/**
 * Sets whether the actor should be focusable by keyboard navigation.
 *
 * @for Actor
 * @method setKeyboardFocusable
 * @param {Boolean}  folcusable
 */
void ActorApi::SetKeyboardFocusable( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Actor actor = GetActor( isolate, args );
  bool parameterFound( false );
  bool focus = V8Utils::GetBooleanParameter( PARAMETER_0, parameterFound, isolate, args );
  if( !parameterFound )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "boolean parameter missing" );
    return;
  }

  actor.SetKeyboardFocusable( focus );
}

/**
 * Returns whether the actor is focusable by keyboard navigation.
 *
 *
 * @for Actor
 * @method isKeyboardFocusable
 * @return {Boolean}  folcusable
 */
void ActorApi::IsKeyboardFocusable( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Actor actor = GetActor( isolate, args );

  args.GetReturnValue().Set( v8::Boolean::New( isolate, actor.IsKeyboardFocusable() ) );

}
/**
 * retrieve the actor type
 *
 * @for Actor
 * @method getActorType
 * @return {String} Actor, ImageActor, MeshActor, Layer, CameraActor ...
 */
void ActorApi::GetActorType( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Actor actor = GetActor( isolate, args );

  std::string name = actor.GetTypeName();
  v8::Local < v8::String > v8String = v8::String::NewFromUtf8( isolate, name.c_str() );
  args.GetReturnValue().Set( v8String );
}
/**
 * Move an actor relative to its existing position.
 * @example
 *
 *    // using an array
 *    actor.translateBy( [20,40,0] );
 *
 * @for Actor
 * @method translateBy
 * @param {object} an array of 3 numbers
 */
void ActorApi::TranslateBy( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Actor actor = GetActor( isolate, args );

  //Get displacement vector
  Vector3 vector;
  int argCount( args.Length() );
  if( argCount == 1 )
  {
    bool found(false);
    vector = V8Utils::GetVector3Parameter( PARAMETER_0, found, isolate, args );
    if( !found )
    {
      DALI_SCRIPT_EXCEPTION( isolate, "Vector3 move parameter missing" );
      return;
    }
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "Vector3 move parameter missing" );
    return;
  }
  actor.TranslateBy( vector );

}


/**
 * Apply a relative rotation to an actor.
 * @example
 *
 *     var rotation =new dali.Rotation( pitch, roll, yaw );
 *     actor.rotateBy( rotation );
 *
 * @for Actor
 * @method rotateBy
 * @param {object} dali rotation object
 */
void ActorApi::RotateBy( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Actor actor = GetActor( isolate, args );

  bool found( false );
  Property::Value rotation = V8Utils::GetPropertyValueParameter( PARAMETER_0, found, isolate, args );

  if( rotation.GetType() != Property::ROTATION )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "Rotation parameter missing" );
    return;
  }
  // the rotation parameter has to be either a AngleAxis or a Quaternion
  // both will work when calling Get( Quaternion);

  Quaternion quaternionValue;
  rotation.Get( quaternionValue );

  actor.RotateBy( quaternionValue );
}

/**
 * Apply a relative scale to an actor.
 * @example
 *    // Double actor width and height ( keep depth the same )
 *    // using an array
 *    actor.scaleBy( [2,2,1] );
 *
 *
 * @for Actor
 * @method scaleBy
 * @param {object} JavaScript array
 */
void ActorApi::ScaleBy( const v8::FunctionCallbackInfo<v8::Value>& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  Actor actor = GetActor( isolate, args );

  Vector3 vector;
  int argCount( args.Length() );
  if( argCount == 1 )
  {
    bool found(false);
    vector = V8Utils::GetVector3Parameter( PARAMETER_0, found, isolate, args );
    if( !found )
    {
      DALI_SCRIPT_EXCEPTION( isolate, "Vector3 move parameter missing" );
      return;
    }
  }
  else
  {
    DALI_SCRIPT_EXCEPTION( isolate, "Vector3 parameter missing" );
    return;
  }
  actor.ScaleBy( vector );
}

} // namespace V8Plugin

} // namespace Dali

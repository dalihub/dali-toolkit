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
#include "keyboard-focus-manager-api.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <dali/integration-api/debug.h>
#include <dali-toolkit/dali-toolkit.h>
#include <actors/actor-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

namespace // un named namespace
{


Toolkit::Control::KeyboardFocusNavigationDirection  GetDirection( std::string name,  v8::Isolate* isolate )
{
  if( name == "left")
  {
    return Dali::Toolkit::Control::Left;
  }
  if( name == "right")
  {
    return  Dali::Toolkit::Control::Right;
  }
  if( name == "up")
  {
    return  Dali::Toolkit::Control::Up;
  }
  if( name == "down")
  {
    return  Dali::Toolkit::Control::Down;
  }

  DALI_SCRIPT_EXCEPTION( isolate,  "direction not found ( wanted left,right,up,down)" );


  return Dali::Toolkit::Control::Up;

}
}; //un-named namespace

/**
 * Move the keyboard focus to the given actor.
 * Only one actor can be focused at the same time.  The actor must
 * be in the stage already and be keyboard focusable.
 *
 * @method setCurrentFocusActor
 * @for KeyboardFocusManager
 * @param {Object} Actor
 */
void KeyboardFocusManagerApi::SetCurrentFocusActor( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  bool found( false );
  Actor actor = V8Utils::GetActorParameter( PARAMETER_0, found, isolate, args );
  if( ! found )
  {
    DALI_SCRIPT_EXCEPTION( isolate,  "no actor found" );
    return;
  }

  Toolkit::KeyboardFocusManager::Get().SetCurrentFocusActor( actor );
}

/**
 * Get the current focused actor.
 *
 * @method getCurrentFocusActor
 * @for KeyboardFocusManager
 * @return {Object} Actor
 */
void KeyboardFocusManagerApi::GetCurrentFocusActor( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  v8::Handle < v8::Object > wrappedActor = ActorWrapper::WrapActor( isolate, Toolkit::KeyboardFocusManager::Get().GetCurrentFocusActor() );
  args.GetReturnValue().Set( wrappedActor );
}

/**
 * Move the focus to the next focusable actor in the focus
 * chain in the given direction (according to the focus traversal
 * order).
 *
 * @method moveFocus
 * @for KeyboardFocusManager
 * @param {String} direction The direction of focus movement ( left, right, up, down)
 */
void KeyboardFocusManagerApi::MoveFocus( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  bool found(false);
  std::string direction = V8Utils::GetStringParameter( PARAMETER_0, found,isolate, args);

  if( ! found )
  {
    DALI_SCRIPT_EXCEPTION( isolate,  "no direction found" );
    return;
  }

  Toolkit::Control::KeyboardFocusNavigationDirection dir = GetDirection( direction, isolate );

  Toolkit::KeyboardFocusManager::Get().MoveFocus( dir );
}
/**
 * Clear the focus from the current focused actor if any, so
 * that no actor is focused in the focus chain.
 * It will emit focus changed signal without current focused actor
 * @method clearFocus
 * @for KeyboardFocusManager
 */
void KeyboardFocusManagerApi::ClearFocus( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  Toolkit::KeyboardFocusManager::Get().ClearFocus();
}

/**
 * Set whether an actor is a focus group that can limit the
 * scope of focus movement to its child actors in the focus chain.
 *
 * @method setAsFocusGroup
 * @param {Boolean} enabled Whether the focus movement should be looped
 * @for KeyboardFocusManager
 */
void KeyboardFocusManagerApi::SetAsFocusGroup( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  //  void SetAsFocusGroup(Actor actor, bool isFocusGroup);
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  bool found( false );

  Actor actor = V8Utils::GetActorParameter( PARAMETER_0, found, isolate, args );

  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate,  "missing actor param" );
    return;
  }

  bool isFocusGroup = V8Utils::GetBooleanParameter( PARAMETER_1, found,isolate, args);
  if( ! found )
  {
    DALI_SCRIPT_EXCEPTION( isolate,  "boolean param missing" );
    return;
  }

  Toolkit::KeyboardFocusManager::Get().SetAsFocusGroup( actor,isFocusGroup );

}
/**
 * Check whether the actor is set as a focus group or not.
 * @method isFocusGroup
 * @param {Object} Actor  The actor to be checked.
 * @for KeyboardFocusManager
 */
void KeyboardFocusManagerApi::IsFocusGroup( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  bool found( false );
  Actor actor = V8Utils::GetActorParameter( PARAMETER_0, found, isolate, args );

  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "Missing actor parameter");
    return;
  }
  args.GetReturnValue().Set( v8::Boolean::New( isolate,   Toolkit::KeyboardFocusManager::Get().IsFocusGroup(actor) ) );
}

/**
 * Returns the closest ancestor of the given actor that is a focus group.
 * @method getFocusGroup
 * @param {Object} Actor  The actor to be checked.
 * @for KeyboardFocusManager
 */
void KeyboardFocusManagerApi::GetFocusGroup( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  bool found( false );

  Actor actor = V8Utils::GetActorParameter( PARAMETER_0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "Missing actor parameter");
    return;
  }
  Actor retActor  = Toolkit::KeyboardFocusManager::Get().GetFocusGroup( actor );
  v8::Handle < v8::Object > wrappedActor = ActorWrapper::WrapActor( isolate, retActor );
  args.GetReturnValue().Set( wrappedActor );

}

/**
 * Get whether the focus movement should be looped within the same focus group.
 * @method setFocusGroupLoop
 * @param {Object} Actor  he actor to be set as a focus group.
 * @param {Boolean} isFocusGroup  Whether to set the actor as a focus group or not.
 * @for KeyboardFocusManager
 */
void KeyboardFocusManagerApi::SetFocusGroupLoop( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  bool found(false);
  bool enable = V8Utils::GetBooleanParameter( PARAMETER_0, found,isolate, args);
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate,  "boolean param missing" );
    return;
  }
  Toolkit::KeyboardFocusManager::Get().SetFocusGroupLoop( enable );
}

/**
 * Get whether the focus movement should be looped within the same focus group.
 * @method getFocusGroupLoop
 * @return  {Boolean} Whether the focus movement should be looped
 * @for KeyboardFocusManager
 */
void KeyboardFocusManagerApi::GetFocusGroupLoop( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  args.GetReturnValue().Set( v8::Boolean::New( isolate,   Toolkit::KeyboardFocusManager::Get().GetFocusGroupLoop()) );
}

/**
 * Set the focus indicator actor.
 *
 * This will replace the default focus indicator actor in
 * KeyboardFocusManager and will be added to the focused actor as a
 * highlight.
 * @method setFocusIndicatorActor
 * @param {Object} Actor The indicator actor to be added
 * @for KeyboardFocusManager
 */
void KeyboardFocusManagerApi::SetFocusIndicatorActor( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );
  bool found(false);

  Actor actor = V8Utils::GetActorParameter( PARAMETER_0, found, isolate, args );

  if( !found )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "Missing actor parameter");
    return;
  }

  // the actor may be an empty handle
  Toolkit::KeyboardFocusManager::Get().SetFocusIndicatorActor( actor );
}


/**
 *  Get the focus indicator actor.
 *
 * @method getFocusIndicatorActor
 * @param {Object} Actor The indicator actor to be added
 * @for KeyboardFocusManager
 */
void KeyboardFocusManagerApi::GetFocusIndicatorActor( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  v8::Handle < v8::Object > wrappedActor = ActorWrapper::WrapActor( isolate, Toolkit::KeyboardFocusManager::Get().GetFocusIndicatorActor() );
  args.GetReturnValue().Set( wrappedActor );

}

} // namespace V8Plugin

} // namespace Dali

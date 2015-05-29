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
#include "event-object-generator.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <actors/actor-wrapper.h>
#include <object/property-value-wrapper.h>

namespace Dali
{

namespace V8Plugin
{

namespace
{

struct PointState
{
  TouchPoint::State state;
  const char* name;
};

const PointState PointStateLookup[]=
{
    { TouchPoint::Down,         "down"        },       /**< Screen touched */
    { TouchPoint::Up,           "up"          },       /**< Touch stopped */
    { TouchPoint::Motion,       "motion"      },       /**< Touch stopped */
    { TouchPoint::Leave,        "leave"       },       /**< Touch stopped */
    { TouchPoint::Stationary,   "stationary"  },       /**< Touch stopped */
    { TouchPoint::Interrupted,  "interrupt"   },       /**< Touch stopped */
    { TouchPoint::Started,      "started"     },       /**< Touch or hover started */
    { TouchPoint::Finished,     "finished"    },       /**< Touch or hover finished */
};

const unsigned int PointStateLookupCount = sizeof(PointStateLookup)/sizeof(PointStateLookup[0]);

struct GestureState
{
  Gesture::State state;
  const char* name;
};

const GestureState GestureStateLookup[]=
{
    { Gesture::Clear,      "clear"        },       /**< No state associated */
    { Gesture::Started,    "started"      },       /**< Gesture started */
    { Gesture::Continuing, "continuing"   },       /**< Gesture continuing */
    { Gesture::Finished,   "finished"     },       /**< Gesture finished */
    { Gesture::Cancelled,  "cancelled"    },       /**< Gesture cancelled */
    { Gesture::Possible,   "possible"     },       /**< Gesture is possible */
};

const unsigned int GestureStateLookupCount = sizeof(GestureStateLookup)/sizeof(GestureStateLookup[0]);

const char* GetTouchPointStateName( TouchPoint::State state )
{
  // could use the enum as index, but dali-core may change, so for now just do a lookup
  for( unsigned int i = 0; i < PointStateLookupCount; i++ )
  {
    if( PointStateLookup[i].state == state )
    {
      return PointStateLookup[i].name;
    }
  }
  return "error point state not found";
}

const char* GetGestureStateName( Gesture::State state )
{
  // could use the enum as index, but dali-core may change, so for now just do a lookup
  for( unsigned int i = 0; i < GestureStateLookupCount; i++ )
  {
    if( GestureStateLookup[i].state == state )
    {
      return GestureStateLookup[i].name;
    }
  }
  return "error gesture state not found";
}

// create a point object, x,y
v8::Local<v8::Object> CreateTouchPoint( v8::Isolate* isolate, const TouchPoint& touchPoint )
{
  /*
   * touch point object
   *
   *   { "deviceId",  int }  Each touch point has a unique device ID
   *   { "state",   string } touch state ="Down,Up,Motion,Leave,Stationary, Interrupted }
   *   { "hitActor", actor } actor that was hit ( wrapped dali object)
   *   { "local",  {x,y} } co-ordinates of top left of hit actor
   *   { "screen", {x,y} } co-ordinates of top left of hit actor
   */
  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::Object> pointObject = v8::Object::New( isolate );

  // set device id
  pointObject->Set( v8::String::NewFromUtf8( isolate, "deviceId"), v8::Integer::New( isolate, touchPoint.deviceId ));

  // set state
  pointObject->Set( v8::String::NewFromUtf8( isolate, "state"), v8::String::NewFromUtf8( isolate, GetTouchPointStateName(touchPoint.state)));

  // set the hit actor
  pointObject->Set( v8::String::NewFromUtf8( isolate, "hitActor"), ActorWrapper::WrapActor(isolate, touchPoint.hitActor ));

  // Think about changing these Vector 2 from wrapped objects to JavaScript objects...

  // Set the local co-ordinates
  v8::Local<v8::Object> localPointObject = v8::Object::New( isolate );
  localPointObject->Set( v8::String::NewFromUtf8( isolate, "x"),v8::Integer::New( isolate, touchPoint.local.x) );
  localPointObject->Set( v8::String::NewFromUtf8( isolate, "y"),v8::Integer::New( isolate, touchPoint.local.y) );
  pointObject->Set( v8::String::NewFromUtf8( isolate, "local"),  localPointObject);


  // set the screen co-ordinates
  v8::Local<v8::Object> screenPointObject = v8::Object::New( isolate );
  screenPointObject->Set( v8::String::NewFromUtf8( isolate, "x"),v8::Integer::New( isolate, touchPoint.screen.x) );
  screenPointObject->Set( v8::String::NewFromUtf8( isolate, "y"),v8::Integer::New( isolate, touchPoint.screen.y) );
  pointObject->Set( v8::String::NewFromUtf8( isolate, "screen"), screenPointObject );


  return handleScope.Escape( pointObject );
}

} // un-named namespace


v8::Handle<v8::Object> EventObjectGenerator::CreateTouchEvent( v8::Isolate* isolate, const TouchEvent& touchEvent)
{
  // we are creating a touch event object that looks like this
  //
  //  event.pointCount = points touched
  //  event.time       = The time (in ms) that the touch event occurred.
  //  event.point[]    = array of TouchPoints
  //
  // A TouchPoint =
  //   { "deviceId",  int }  Each touch point has a unique device ID
  //   { "state",   string } touch state ="Down,Up,Motion,Leave,Stationary, Interrupted }
  //   { "sourceActor", actor }  the actor that is emitting the callback (the actor that is hit maybe a child of it)
  //   { "hitActor", actor } actor that was hit
  //   { "local",  {x,y} } co-ordinates of top left of hit actor
  //   { "screen", {x,y} } co-ordinates of top left of hit actor
  //

  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::Object> touchObject = v8::Object::New( isolate );

  // Set the pointCount
  touchObject->Set( v8::String::NewFromUtf8( isolate, "pointCount" ), v8::Integer::New( isolate, touchEvent.GetPointCount() ) );

  // Set the time
  touchObject->Set( v8::String::NewFromUtf8( isolate, "time" ), v8::Number::New( isolate, touchEvent.time ) );

  // Set the emitting actor
  // touchObject->Set( v8::String::NewFromUtf8( isolate, "sourceActor" ), ActorWrapper::WrapActor(isolate, emittingActor));

  // Create the array of touch points
  v8::Local < v8::Array > pointArrayObject = v8::Array::New( isolate, touchEvent.GetPointCount() );
  for( unsigned int i = 0 ; i < touchEvent.GetPointCount() ; ++i )
  {
    v8::Local < v8::Object > pointObject = CreateTouchPoint( isolate, touchEvent.points[i] );
    pointArrayObject->Set( v8::Number::New( isolate, i ), pointObject  );
  }

  // set the array
  touchObject->Set( v8::String::NewFromUtf8( isolate, "points" ), pointArrayObject );

  return handleScope.Escape( touchObject );
}

v8::Handle<v8::Object> EventObjectGenerator::CreateHoverEvent( v8::Isolate* isolate, const HoverEvent& hoverEvent)
{
  // we are creating a hover event object that looks like this
  //
  //  event.pointCount = points hovered
  //  event.time       = The time (in ms) that the hover event occurred.
  //  event.point[]    = array of TouchPoints
  //
  // A TouchPoint =
  //   { "deviceId",  int }  Each touch point has a unique device ID
  //   { "state",   string } touch state ="Down,Up,Motion,Leave,Stationary, Interrupted }
  //   { "sourceActor", actor }  the actor that is emitting the callback (the actor that is hit maybe a child of it)
  //   { "hitActor", actor } actor that was hit
  //   { "local",  {x,y} } co-ordinates of top left of hit actor
  //   { "screen", {x,y} } co-ordinates of top left of hit actor
  //

  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::Object> hoverObject = v8::Object::New( isolate );

  // Set the pointCount
  hoverObject->Set( v8::String::NewFromUtf8( isolate, "pointCount" ), v8::Integer::New( isolate, hoverEvent.GetPointCount() ) );

  // Set the time
  hoverObject->Set( v8::String::NewFromUtf8( isolate, "time" ), v8::Number::New( isolate, hoverEvent.time ) );

  // Set the emitting actor
  // hoverObject->Set( v8::String::NewFromUtf8( isolate, "sourceActor" ), ActorWrapper::WrapActor(isolate, emittingActor));

  // Create the array of touch points
  v8::Local < v8::Array > pointArrayObject = v8::Array::New( isolate, hoverEvent.GetPointCount() );
  for( unsigned int i = 0 ; i < hoverEvent.GetPointCount() ; ++i )
  {
    v8::Local < v8::Object > pointObject = CreateTouchPoint( isolate, hoverEvent.points[i] );
    pointArrayObject->Set( v8::Number::New( isolate, i ), pointObject  );
  }

  // set the array
  hoverObject->Set( v8::String::NewFromUtf8( isolate, "points" ), pointArrayObject );

  return handleScope.Escape( hoverObject );
}

v8::Handle<v8::Object> EventObjectGenerator::CreateWheelEvent( v8::Isolate* isolate, const WheelEvent& wheelEvent)
{
  //  we are creating a wheel event object that looks like this
  //
  //  event.type = "mouseWheel" or "customWheel" type of the wheel event
  //  event.direction = "vertical" or "horizontal" direction the wheel is being rolled
  //  event.shiftPressed       = boolean, shift key is held
  //  event.ctrlPressed        = boolean, ctrl key is held
  //  event.altPressed     = boolean, alt key is held
  //  event.keyModifiers = bitmask of keys pressed
  //  event.point {x,y}    = The co-ordinates of the mouse cursor relative to the top-left of the screen when the wheel is being rolled.
  //  event.rolled          = offset of wheel rolling, positive = rolling down or clockwise, negative = rolling up or counter-clockwise
  //  event.timestamp    = The time (in ms) that the touch event occurred

  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::Object> wheelObject = v8::Object::New( isolate );

  // Set the type
  std::string type = wheelEvent.type ? "mouseWheel" : "customWheel";
  wheelObject->Set( v8::String::NewFromUtf8( isolate, "type" ), v8::String::NewFromUtf8( isolate, type.c_str() ) );

  // Set the direction
  std::string direction = wheelEvent.direction ? "vertical" : "horizontal";
  wheelObject->Set( v8::String::NewFromUtf8( isolate, "direction" ), v8::String::NewFromUtf8( isolate, direction.c_str() ) );

  // set shift / ctrl/ alt keys
  wheelObject->Set( v8::String::NewFromUtf8( isolate, "shiftPressed" ), v8::Boolean::New( isolate, wheelEvent.IsShiftModifier()) );
  wheelObject->Set( v8::String::NewFromUtf8( isolate, "ctrlPressed" ),  v8::Boolean::New( isolate, wheelEvent.IsCtrlModifier()) );
  wheelObject->Set( v8::String::NewFromUtf8( isolate, "altPressed" ),   v8::Boolean::New( isolate, wheelEvent.IsAltModifier()) );

  // set keyModifiers
  wheelObject->Set( v8::String::NewFromUtf8( isolate, "keyModifiers" ),  v8::Integer::New( isolate, wheelEvent.modifiers) );

  // set Point
  v8::Local<v8::Object> wheelPointObject = v8::Object::New( isolate );
  wheelPointObject->Set( v8::String::NewFromUtf8( isolate, "x"),v8::Integer::New( isolate, wheelEvent.point.x ) );
  wheelPointObject->Set( v8::String::NewFromUtf8( isolate, "y"),v8::Integer::New( isolate, wheelEvent.point.y ) );
  wheelObject->Set( v8::String::NewFromUtf8( isolate, "point"), wheelPointObject );

  // set rolledAmout
  wheelObject->Set( v8::String::NewFromUtf8( isolate, "rolled" ),  v8::Integer::New( isolate, wheelEvent.z) );

  // set timestamp
  wheelObject->Set( v8::String::NewFromUtf8( isolate, "timestamp" ),  v8::Integer::New( isolate, wheelEvent.timeStamp) );

  return handleScope.Escape( wheelObject );

}

v8::Handle<v8::Object> EventObjectGenerator::CreateKeyEvent( v8::Isolate* isolate, const KeyEvent& keyEvent)
{
  // keyCode
  // keyModifier
  // event.keyCode       = key code
  // event.key           = key presssed
  // event.keyDescription = key description, e.g. BackSpace
  // event.state = "down" or "up" key state
  // event.shiftPressed  = boolean, shift key is held
  // event.ctrlPressed   = boolean, ctrl key is held
  // event.altPressed    = boolean, alt key is held
  // event.keyModifiers  = key modifiers
  // event.timestamp     = Time stamp

  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::Object> keyObject = v8::Object::New( isolate );

  // Set the direction
  std::string state ("up");
  if ( keyEvent.state == KeyEvent::Down)
  {
    state = "down";
  }

  keyObject->Set( v8::String::NewFromUtf8( isolate, "state" ), v8::String::NewFromUtf8( isolate, state.c_str() ) );

  // set keyCode
   keyObject->Set( v8::String::NewFromUtf8( isolate, "keyCode" ),  v8::Integer::New( isolate, keyEvent.keyCode) );

  // set keyModifiers
  keyObject->Set( v8::String::NewFromUtf8( isolate, "keyModifier" ),  v8::Integer::New( isolate, keyEvent.keyModifier) );

  // set Key pressed name
  keyObject->Set( v8::String::NewFromUtf8( isolate, "keyDescription" ),  v8::String::NewFromUtf8( isolate, keyEvent.keyPressedName.c_str() )  );

  // set Key pressed
  keyObject->Set( v8::String::NewFromUtf8( isolate, "key" ),  v8::String::NewFromUtf8( isolate, keyEvent.keyPressed.c_str() )  );

  // set time stamp
  keyObject->Set( v8::String::NewFromUtf8( isolate, "timestamp" ),  v8::Integer::New( isolate, keyEvent.time) );

  // set shift / ctrl/ alt keys
  keyObject->Set( v8::String::NewFromUtf8( isolate, "shiftPressed" ), v8::Boolean::New( isolate, keyEvent.IsShiftModifier()) );
  keyObject->Set( v8::String::NewFromUtf8( isolate, "ctrlPressed" ),  v8::Boolean::New( isolate, keyEvent.IsCtrlModifier()) );
  keyObject->Set( v8::String::NewFromUtf8( isolate, "altPressed" ),   v8::Boolean::New( isolate, keyEvent.IsAltModifier()) );

  return handleScope.Escape( keyObject );

}

v8::Handle<v8::Object> EventObjectGenerator::CreatePanGesture( v8::Isolate* isolate, const PanGesture& panGesture)
{
  // gesture.numberOfTouches     = total number of fingers touching the screen
  // gesture.state               = pan state {Clear, Started, Continuing, Finished, Cancelled, Possible}
  // gesture.time                = Time stamp
  // gesture.velocity            = pan velocity in actor coordinates
  // gesture.displacement        = pan displacement in actor coordinates
  // gesture.position            = pan position in actor coordinates
  // gesture.screenVelocity      = pan velocity in screen coordinates
  // gesture.screenDisplacement  = pan displacement in screen coordinates
  // gesture.screenPosition      = pan position in screen coordinates

  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::Object> panObject = v8::Object::New( isolate );

  // Set the numberOfTouches
  panObject->Set( v8::String::NewFromUtf8( isolate, "numberOfTouches" ), v8::Integer::New( isolate, panGesture.numberOfTouches ) );

  // Set the state
  panObject->Set( v8::String::NewFromUtf8( isolate, "state"), v8::String::NewFromUtf8( isolate, GetGestureStateName(panGesture.state)));

  // Set the time
  panObject->Set( v8::String::NewFromUtf8( isolate, "time" ), v8::Number::New( isolate, panGesture.time ) );

  // Set the local velocity
  v8::Local<v8::Object> velocityObject = v8::Object::New( isolate );
  velocityObject->Set( v8::String::NewFromUtf8( isolate, "x"),v8::Number::New( isolate, panGesture.velocity.x) );
  velocityObject->Set( v8::String::NewFromUtf8( isolate, "y"),v8::Number::New( isolate, panGesture.velocity.y) );
  panObject->Set( v8::String::NewFromUtf8( isolate, "velocity"),  velocityObject);

  // Set the local displacement
  v8::Local<v8::Object> displacementObject = v8::Object::New( isolate );
  displacementObject->Set( v8::String::NewFromUtf8( isolate, "x"),v8::Number::New( isolate, panGesture.displacement.x) );
  displacementObject->Set( v8::String::NewFromUtf8( isolate, "y"),v8::Number::New( isolate, panGesture.displacement.y) );
  panObject->Set( v8::String::NewFromUtf8( isolate, "displacement"),  displacementObject);

  // Set the local position
  v8::Local<v8::Object> positionObject = v8::Object::New( isolate );
  positionObject->Set( v8::String::NewFromUtf8( isolate, "x"),v8::Number::New( isolate, panGesture.position.x) );
  positionObject->Set( v8::String::NewFromUtf8( isolate, "y"),v8::Number::New( isolate, panGesture.position.y) );
  panObject->Set( v8::String::NewFromUtf8( isolate, "position"),  positionObject);

  // Set the screen velocity
  v8::Local<v8::Object> screenVelocityObject = v8::Object::New( isolate );
  screenVelocityObject->Set( v8::String::NewFromUtf8( isolate, "x"),v8::Number::New( isolate, panGesture.screenVelocity.x) );
  screenVelocityObject->Set( v8::String::NewFromUtf8( isolate, "y"),v8::Number::New( isolate, panGesture.screenVelocity.y) );
  panObject->Set( v8::String::NewFromUtf8( isolate, "screenVelocity"),  screenVelocityObject);

  // Set the screen displacement
  v8::Local<v8::Object> screenDisplacementObject = v8::Object::New( isolate );
  screenDisplacementObject->Set( v8::String::NewFromUtf8( isolate, "x"),v8::Number::New( isolate, panGesture.screenDisplacement.x) );
  screenDisplacementObject->Set( v8::String::NewFromUtf8( isolate, "y"),v8::Number::New( isolate, panGesture.screenDisplacement.y) );
  panObject->Set( v8::String::NewFromUtf8( isolate, "screenDisplacement"),  screenDisplacementObject);

  // Set the screen position
  v8::Local<v8::Object> screenPositionObject = v8::Object::New( isolate );
  screenPositionObject->Set( v8::String::NewFromUtf8( isolate, "x"),v8::Number::New( isolate, panGesture.screenPosition.x) );
  screenPositionObject->Set( v8::String::NewFromUtf8( isolate, "y"),v8::Number::New( isolate, panGesture.screenPosition.y) );
  panObject->Set( v8::String::NewFromUtf8( isolate, "screenPosition"),  screenPositionObject);

  return handleScope.Escape( panObject );

}



} // V8Plugin

} // namespace Dali

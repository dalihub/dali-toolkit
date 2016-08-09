#ifndef __DALI_V8PLUGIN_EVENT_OBJECT_GENERATOR_H__
#define __DALI_V8PLUGIN_EVENT_OBJECT_GENERATOR_H__

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
#include <dali/public-api/events/touch-data.h>
#include <dali/public-api/events/hover-event.h>
#include <dali/public-api/events/wheel-event.h>
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/events/pan-gesture.h>

namespace Dali
{

namespace V8Plugin
{


/**
 * Used to convert Dali event objects into pure JavaScript objects.
 * The objects created are not wrappers around a Dali object,
 * however TouchPoint.Actor is a wrapped actor
 */
namespace EventObjectGenerator
{

  v8::Handle<v8::Object> CreateTouchData( v8::Isolate* isolate, const TouchData& touch );
  v8::Handle<v8::Object> CreateHoverEvent( v8::Isolate* isolate, const HoverEvent& hoverEvent);
  v8::Handle<v8::Object> CreateWheelEvent( v8::Isolate* isolate, const WheelEvent& wheelEvent);
  v8::Handle<v8::Object> CreateKeyEvent( v8::Isolate* isolate, const KeyEvent& keyEvent);
  v8::Handle<v8::Object> CreatePanGesture( v8::Isolate* isolate, const PanGesture& panGesture);



};

} // namespace V8Plugin

} // namespace Dali

#endif // header

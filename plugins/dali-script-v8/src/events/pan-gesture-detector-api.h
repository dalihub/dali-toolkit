#ifndef DALI_V8PLUGIN_PAN_GESTURE_DETECTOR_API_H
#define DALI_V8PLUGIN_PAN_GESTURE_DETECTOR_API_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/events/pan-gesture-detector.h>

namespace Dali
{

namespace V8Plugin
{

namespace PanGestureDetectorApi
{
  /**
   * Constructor
   */
  PanGestureDetector New( const v8::FunctionCallbackInfo< v8::Value >& args );

  /**
   * PanGestureDetector API see gesture-detector.h for a description
   */
  void Attach( const v8::FunctionCallbackInfo< v8::Value >& args );
  void Detach( const v8::FunctionCallbackInfo< v8::Value >& args );

}; // namespace PanGestureDetectorApi

} // namespace V8Plugin

} // namespace Dali

#endif // DALI_V8PLUGIN_PAN_GESTURE_DETECTOR_API_H

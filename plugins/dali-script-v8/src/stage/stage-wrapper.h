#ifndef __DALI_V8PLUGIN_STAGE_WRAPPER_H__
#define __DALI_V8PLUGIN_STAGE_WRAPPER_H__

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
#include <dali/public-api/common/stage.h>

// INTERNAL INCLUDES
#include <shared/base-wrapped-object.h>
#include <signals/signal-manager.h>


namespace Dali
{

namespace V8Plugin
{


/**
 * An Stage  wrapper.
 * Provides access to Stage specific functionality and V8 memory handling.
 */
class DALI_INTERNAL StageWrapper : public BaseWrappedObject
{

public:

  StageWrapper( const Stage& stage,
                GarbageCollectorInterface& gc );

  virtual ~StageWrapper() {};

  /**
   * Wraps a stage
   */
  static v8::Handle<v8::Object> WrapStage(v8::Isolate* isolate, const Dali::Stage& );

  Stage GetStage();

  virtual SignalManager* GetSignalManager() { return &mSignalManager;}


private:

  // Stage
  Stage mStage;
  SignalManager mSignalManager;

  static v8::Handle<v8::ObjectTemplate> MakeStageTemplate( v8::Isolate* isolate );
  static v8::Local<v8::ObjectTemplate> GetStageTemplate( v8::Isolate* isolate );

};

} // namespace V8Plugin

} // namespace Dali

#endif // __DALI_V8PLUGIN_STAGE_WRAPPER_H__

#ifndef __DALI_V8PLUGIN_SIGNAL_MANAGER_H__
#define __DALI_V8PLUGIN_SIGNAL_MANAGER_H__

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
#include <string>
#include <dali/public-api/common/dali-vector.h>

// INTERNAL INCLUDES
#include <signals/emit-notification-interface.h>



namespace Dali
{

namespace V8Plugin
{

class BaseCallBack;
class HandleWrapper;

/**
 * Manages signal connection / disconnection.
 *
 * How it works...
 * 1) SignalConnect, SignalDisconnect is installed on to a dali-wrapped object template. E.g.
 *
 *   objTemplate->Set( v8::String::NewFromUtf8( isolate, "Connect"),
                     v8::FunctionTemplate::New( isolate, SignalManager::SignalConnect) );
 *
 * 2) When the  actor.Connect(...) is called, v8 runs SignalManager::SignalConnect( args )
 *
 * args.This() == Dali object ( e.g. Actor / Animation / Image ).
 * args[ 0 ] == Signal name, e.g. "touched"
 * args[ 1 ] ==  JavaScript function to run when the signal is fired.
 *
 * 3) SignalManager uses the signal name, to decide what type of Dali Signal is required.
 * It then creates a C++ object which can be connected to the Signal.
 * That object also holds a pointer to the JavaScript callback function
 *
 * 4) When the signal is fired, it triggers the signal in the C++ object, which then fires
 * the JavaScript function. See ActorCallback::OnTouch
 *
 *  When the object that holds SignalManager dies (e.g. an ActorWrapper), the signal manager for that
 *  object is destroyed, which automatically disconnects any signal connections.
 *
 *  @TODO
 *  Currently DALi Core supports 'DoConnectSignal' which passes in no parameters and expects no return value.
 *  If we could extend this, so DoConnect supports a generic function such as
 *  Dali::Any (*CallbackFunction) (Dali::Vector< Dali::Any > parameters );
 *  then we should be able to remove most of the code in this file.
 *  As this is how BaseJavaScriptCallback::Call() in SignalManager.cpp works.
 *
 *
 *
 */
class SignalManager : public EmitNotificationInterface
{
public:

  /**
   * Constructor
   */
  SignalManager();

  /**
   * Destructor
   */
  virtual ~SignalManager();

  /**
   * signal connect function, installed on dali-wrapped object
   */
  static void SignalConnect( const v8::FunctionCallbackInfo< v8::Value >& args );

  /**
   * signal disconnect function, installed on dali-wrapped object
   */
  static void SignalDisconnect( const v8::FunctionCallbackInfo< v8::Value >& args );

  /**
   * Add a callback
   * @param[in] callback
   */
  void AddCallback( BaseCallBack* callback );

  /**
   * Remove a callback
   * @param[in] signalName name
   * @param[in] function to remove
   */
  void RemoveCallback( v8::Isolate* isolate, const std::string& signalName, const v8::Local<v8::Function>& function );

private:

public:

  /**
   * @copydoc EmitNotificationInterface::CallbackEnter()
   */
  virtual void CallbackEnter();

  /**
   * @copydoc EmitNotificationInterface::CallbackExit()
   */
  virtual void CallbackExit();


  // typically an object will only have 1 callback, so a vector is ok, e.g.
  // there's no point in storing a map of callbacks for an actor that will only have 1 connection
  // for OnTouch.
  typedef Dali::Vector< BaseCallBack* > CallBackContainer;

  CallBackContainer mCallbacks;               ///< List of callbacks
  CallBackContainer mDisconnectedCallbacks;   ///< List of disconnected callbacks
  bool mInsideCallback;                       ///< To prevent recursive loops

};


} // namespace V8Plugin

} // namespace Dali

#endif // header

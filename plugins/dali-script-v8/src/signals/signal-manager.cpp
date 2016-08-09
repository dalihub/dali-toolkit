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
#include "signal-manager.h"

// EXTERNAL INCLUDES
#include <cstring> // for strcmp
#include <dali/public-api/images/resource-image.h>
#include <dali/public-api/object/any.h>
#include <dali/public-api/events/pan-gesture-detector.h>
#include <dali/public-api/object/any.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/events/touch-data.h>
#include <dali/public-api/events/hover-event.h>
#include <dali/public-api/events/wheel-event.h>
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/events/pan-gesture.h>
#include <dali/devel-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <object/handle-wrapper.h>
#include <object/property-value-wrapper.h>
#include <actors/actor-wrapper.h>
#include <image/image-wrapper.h>
#include <animation/animation-wrapper.h>
#include <stage/stage-wrapper.h>
#include <events/pan-gesture-detector-wrapper.h>
#include <toolkit/focus-manager/keyboard-focus-manager-wrapper.h>
#include <signals/dali-any-javascript-converter.h>



namespace Dali
{

namespace V8Plugin
{

namespace // un-named namespace
{
const char* const SIGNAL_TOUCH = "touch";
const char* const SIGNAL_HOVERED = "hovered";
const char* const SIGNAL_WHEEL_EVENT = "wheelEvent";
const char* const SIGNAL_ON_STAGE = "onStage";
const char* const SIGNAL_OFF_STAGE = "offStage";
const char* const ANIMATION_SIGNAL_FINISHED = "finished";
const char* const SIGNAL_PAN_DETECTED = "panDetected";

const char* const STAGE_SIGNAL_KEY_EVENT = "keyEvent";
const char* const STAGE_SIGNAL_EVENT_PROCESSING_FINISHED = "eventProcessingFinished";
const char* const STAGE_SIGNAL_TOUCH = "touch";
const char* const SIGNAL_IMAGE_LOADING_FINISHED = "imageLoadingFinished";
const char* const SIGNAL_IMAGE_UPLOADED = "uploaded";

const char* const KEYBOARD_FOCUS_MANAGER_SIGNAL_PRE_FOCUS_CHANGE = "keyboardPreFocusChange";
const char* const KEYBOARD_FOCUS_MANAGER_SIGNAL_FOCUS_CHANGED = "keyboardFocusChanged";
const char* const KEYBOARD_FOCUS_MANAGER_SIGNAL_FOCUS_GROUP_CHANGED = "keyboardFocusGroupChanged";
const char* const KEYBOARD_FOCUS_MANAGER_SIGNAL_FOCUSED_ACTOR_ENTER_KEY = "keyboardFocusedActorEnterKey";
const unsigned int MAX_SIGNAL_NAME_LENGTH = 64;

} // un-named name space

class BaseJavaScriptCallback
{
public:

  BaseJavaScriptCallback(  v8::Isolate* isolate,  const v8::Local<v8::Function>& callback )
  :mIsolate( isolate ),mEmitInterface( NULL )
  {
    mCallback.Reset( isolate, callback);
  }
  ~BaseJavaScriptCallback()
  {
    mCallback.Reset();
  };

  bool IsFunction( const v8::Local<v8::Function>& function)
  {
    v8::HandleScope handleScope( mIsolate );
    v8::Local<v8::Function> localCallback =  v8::Local<v8::Function>::New( mIsolate, mCallback );
    return ( localCallback == function );
  }

  void Call( Dali::Any& returnValue, std::vector< Dali::Any >& data )
  {
    mEmitInterface->CallbackEnter();

    v8::HandleScope handleScope( mIsolate );
    v8::TryCatch tryCatch;

    // create a vector of JavaScript arguments to pass to the JavaScript function
    std::vector< v8::Local<v8::Value> > arguments;

    for( unsigned int i = 0; i < data.size(); ++i )
    {
      // convert each Dali::Any object into a JavaScript object based on it's type
      arguments.push_back( DaliAnyConverter::ConvertToJavaScriptObject( mIsolate, data[i]));
    }

    // call the javascript function
    v8::Local<v8::Function> localCallback =  v8::Local<v8::Function>::New( mIsolate, mCallback );
    v8::Local<v8::Value>  javaScriptReturnVal = v8::Local<v8::Value>::Cast( localCallback->Call( localCallback, arguments.size(), &arguments[0] ));

    // Print errors that happened during execution.
    if( tryCatch.HasCaught() )
    {
      V8Utils::ReportException( mIsolate, &tryCatch);
      DALI_ASSERT_ALWAYS( 0 && "Script error \n");
    }

    // convert will log an error, if the return type we want isn't found
    returnValue = DaliAnyConverter::ConvertToDaliAny( mIsolate,  javaScriptReturnVal, returnValue );

    mEmitInterface->CallbackExit();

  }

  void SetEmitInterface( EmitNotificationInterface* interface)
  {
    mEmitInterface = interface;
  }
protected:
  v8::Isolate*                 mIsolate;
  v8::Persistent<v8::Function> mCallback;
  EmitNotificationInterface*   mEmitInterface;

};
/**
 * base class for callback
 */
class BaseCallBack : public ConnectionTracker
{
public:
    BaseCallBack( v8::Isolate* isolate,  const v8::Local<v8::Function>& callback, const std::string& signalName )
    :mSignalName( signalName ),
     mJavaFunctor( isolate, callback)
    {

    }
    void CallJavaScript(  Dali::Any& returnValue, std::vector< Dali::Any >& data )
    {
      mJavaFunctor.Call( returnValue, data);
    }

    bool IsFunction( const v8::Local<v8::Function>& function)
    {
      return mJavaFunctor.IsFunction(function);
    }

    void SetEmitInterface( EmitNotificationInterface* interface)
    {
      mJavaFunctor.SetEmitInterface( interface );
    }
    virtual ~BaseCallBack() {}

    std::string mSignalName;
    BaseJavaScriptCallback mJavaFunctor;
};

namespace
{

class ActorCallback :  public BaseCallBack
{
public:

  ActorCallback(v8::Isolate* isolate,  const v8::Local<v8::Function>& callback ,const std::string& signalName, Actor actor )
  : BaseCallBack( isolate, callback, signalName ),
    mActor(actor)
  {
  }
  bool OnTouch( Actor actor, const TouchData& touch )
  {
    std::vector< Dali::Any > arguments;  // Dali::Vector considers Dali::Any to be a non trivial type so won't compile
    Dali::Any returnValue(false);
    arguments.push_back( actor );
    arguments.push_back( touch );
    CallJavaScript( returnValue, arguments );
    bool ret;
    returnValue.Get(ret);
    return ret;
  }
  bool OnHover( Actor actor, const HoverEvent& event)
  {
    std::vector< Dali::Any > arguments;  // Dali::Vector considers Dali::Any to be a non trivial type so won't compile
    Dali::Any returnValue(false);
    arguments.push_back( actor );
    arguments.push_back( event );
    CallJavaScript( returnValue, arguments );
    bool ret;
    returnValue.Get(ret);
    return ret;
  }
  bool OnWheel( Actor actor, const WheelEvent& event)
  {
    std::vector< Dali::Any > arguments;
    Dali::Any returnValue(false);
    arguments.push_back( actor );
    arguments.push_back( event );
    CallJavaScript( returnValue, arguments );
    bool ret;
    returnValue.Get(ret);
    return ret;
  }
  void OnStage( Actor actor)
  {
    std::vector< Dali::Any > arguments;
    Dali::Any returnValue;          //no return
    arguments.push_back( actor );
    CallJavaScript( returnValue, arguments );
  }
  void OffStage( Actor actor)
  {
    std::vector< Dali::Any > arguments;
    Dali::Any returnValue;          //no return
    arguments.push_back( actor );
    CallJavaScript( returnValue, arguments );
  }

  WeakHandle< Actor > mActor;

};

struct ActorGenericCallbackFunctor
{
  ActorGenericCallbackFunctor(ActorCallback& callback) : callback(callback) { }
  void operator()()
  {
    std::vector< Dali::Any > arguments;
    Dali::Any returnValue;          //no return
    arguments.push_back(callback.mActor.GetHandle()); //pass the actor handle as the parameter
    callback.CallJavaScript( returnValue, arguments );
  }

  ActorCallback& callback;
};


class AnimationCallback :  public BaseCallBack
{
public:

  AnimationCallback(v8::Isolate* isolate,  const v8::Local<v8::Function>& callback ,const std::string& signalName )
  : BaseCallBack( isolate, callback, signalName )
  {
  }
  void Finished( Animation& animation)
  {
    std::vector< Dali::Any > arguments;  // Dali::Vector considers Dali::Any to be a non trivial type so won't compile
    Dali::Any returnValue;    //no return
    arguments.push_back( animation );
    CallJavaScript( returnValue, arguments );
  }
};


class ImageCallback :  public BaseCallBack
{
public:

  ImageCallback(v8::Isolate* isolate,  const v8::Local<v8::Function>& callback ,const std::string& signalName )
  : BaseCallBack( isolate, callback, signalName )
  {
  }
  void LoadingFinished( ResourceImage image)
  {
    std::vector< Dali::Any > arguments;  // Dali::Vector considers Dali::Any to be a non trivial type so won't compile
    Dali::Any returnValue;  //no return
    arguments.push_back( image );
    CallJavaScript( returnValue, arguments );
  }
};
class StageCallback :  public BaseCallBack
{
public:

  StageCallback(v8::Isolate* isolate,  const v8::Local<v8::Function>& callback ,const std::string& signalName )
  : BaseCallBack( isolate, callback, signalName )
  {
  }

  void KeyEventSignal( const KeyEvent& keyEvent )
  {
    std::vector< Dali::Any > arguments;
    Dali::Any returnValue;  //no return
    arguments.push_back( keyEvent );
    CallJavaScript( returnValue, arguments );
  }
  void EventProcessingFinishedSignal()
  {
    std::vector< Dali::Any > arguments;
    Dali::Any returnValue;  //no return
    CallJavaScript( returnValue, arguments );
  }
  void TouchSignal( const TouchData& touch )
  {
    std::vector< Dali::Any > arguments;
    Dali::Any returnValue;   //no return
    arguments.push_back( touch );
    CallJavaScript( returnValue, arguments );
  }
};

class KeyboardFocusCallback :  public BaseCallBack
{
public:

  KeyboardFocusCallback(v8::Isolate* isolate,  const v8::Local<v8::Function>& callback ,const std::string& signalName )
  : BaseCallBack( isolate, callback, signalName )
  {
  }

  Actor PreFocusChangeSignal(Actor currentFocusedActor,
                             Actor proposedActorToFocus,
                             Toolkit::Control::KeyboardFocus::Direction direction  )
  {
    std::vector< Dali::Any > arguments;
    Dali::Any returnValue = Actor();  // we want an actor as a return value

    arguments.push_back( currentFocusedActor );
    arguments.push_back( proposedActorToFocus );

    std::string name = KeyboardFocusManagerWrapper::GetDirectionName( direction );
    arguments.push_back( name );

    CallJavaScript( returnValue, arguments );

    Actor ret;
    returnValue.Get(ret);
    return ret;
  }
  void FocusChangedSignal(Actor originalFocusedActor, Actor currentFocusedActor)
  {
    std::vector< Dali::Any > arguments;
    Dali::Any returnValue;  //no return
    arguments.push_back( originalFocusedActor );
    arguments.push_back( currentFocusedActor );
    CallJavaScript( returnValue, arguments );
  }
  void FocusGroupSignal( Actor currentFocusedActor, bool forward )
  {
    std::vector< Dali::Any > arguments;
    Dali::Any returnValue;   //no return
    arguments.push_back( currentFocusedActor );
    arguments.push_back( forward );
    CallJavaScript( returnValue, arguments );
  }
  void EnterKeyPressedActorSignal( Actor enterKeyPressedActor )
  {
    std::vector< Dali::Any > arguments;
    Dali::Any returnValue;   //no return
    arguments.push_back( enterKeyPressedActor );
    CallJavaScript( returnValue, arguments );
  }
};

class PanGestureDetectorCallback :  public BaseCallBack
{
public:

  PanGestureDetectorCallback(v8::Isolate* isolate,  const v8::Local<v8::Function>& callback ,const std::string& signalName )
  : BaseCallBack( isolate, callback, signalName )
  {
  }
  void OnPan( Actor actor, const PanGesture& panGesture)
  {
    std::vector< Dali::Any > arguments;
    Dali::Any returnValue;    //no return
    arguments.push_back( actor );
    arguments.push_back( panGesture );
    CallJavaScript( returnValue, arguments );
  }
};

void ActorConnection( v8::Isolate* isolate,
                                     v8::Local<v8::Function> javaScriptCallback,
                                     ActorWrapper* actorWrapper,
                                     const std::string& signalName,
                                     Actor actor )
{
  bool connected(true);

  ActorCallback* callback =new ActorCallback( isolate, javaScriptCallback, signalName, actor );

  if( strcmp( signalName.c_str(), SIGNAL_TOUCH ) == 0 )
  {
    actor.TouchSignal().Connect( callback, &ActorCallback::OnTouch );
  }
  else if( strcmp( signalName.c_str(), SIGNAL_HOVERED ) == 0 )
  {
    actor.HoveredSignal().Connect( callback, &ActorCallback::OnHover );
  }
  else if ( strcmp( signalName.c_str(), SIGNAL_WHEEL_EVENT ) == 0 )
  {
    actor.WheelEventSignal().Connect( callback, &ActorCallback::OnWheel );
  }
  else if ( strcmp( signalName.c_str(), SIGNAL_ON_STAGE ) == 0 )
  {
    actor.OnStageSignal().Connect( callback, &ActorCallback::OnStage);
  }
  else if ( strcmp( signalName.c_str(), SIGNAL_OFF_STAGE ) == 0 )
  {
    actor.OffStageSignal().Connect( callback, &ActorCallback::OffStage);
  }
  else
  {
    connected = actor.ConnectSignal( callback, signalName, ActorGenericCallbackFunctor(*callback) );
  }

  if( connected )
  {
    actorWrapper->GetSignalManager()->AddCallback( static_cast<BaseCallBack*>(callback) );
  }
  else
  {
    delete callback;
    DALI_SCRIPT_EXCEPTION( isolate, "the signal is not available in this object.");
  }
}

void AnimationConnection( v8::Isolate* isolate,
                                     v8::Local<v8::Function> javaScriptCallback,
                                     AnimationWrapper* animWrapper,
                                     const std::string& signalName,
                                     Animation anim )
{
  if( signalName ==  ANIMATION_SIGNAL_FINISHED)
  {
    AnimationCallback* callback =new AnimationCallback( isolate, javaScriptCallback, signalName );
    anim.FinishedSignal().Connect( callback, &AnimationCallback::Finished );
    animWrapper->GetSignalManager()->AddCallback( static_cast<BaseCallBack*>(callback) );
  }
}

void ImageConnection( v8::Isolate* isolate,
                                     v8::Local<v8::Function> javaScriptCallback,
                                     ImageWrapper* imageWrapper,
                                     const std::string& signalName,
                                     Image image)
{
  // Nine patch will downcast to ResourceImage
  ResourceImage resourceImage( ResourceImage::DownCast( image ) );
  if( !resourceImage )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "can only connect to Resource / NinePatch Images");
    return;
  }

  if( strcmp( signalName.c_str(), SIGNAL_IMAGE_LOADING_FINISHED ) == 0 )
  {
    ImageCallback* callback =new ImageCallback( isolate, javaScriptCallback, signalName );

    resourceImage.LoadingFinishedSignal().Connect( callback, &ImageCallback::LoadingFinished );
    imageWrapper->GetSignalManager()->AddCallback( static_cast<BaseCallBack*>(callback) );
  }
  else if ( strcmp( signalName.c_str() , SIGNAL_IMAGE_UPLOADED ) == 0 )
  {
    ImageCallback* callback =new ImageCallback( isolate, javaScriptCallback, signalName );
    resourceImage.LoadingFinishedSignal().Connect( callback, &ImageCallback::LoadingFinished );
    imageWrapper->GetSignalManager()->AddCallback( static_cast<BaseCallBack*>(callback) );
  }
}

void StageConnection( v8::Isolate* isolate,
                                     v8::Local<v8::Function> javaScriptCallback,
                                     StageWrapper* stageWrapper,
                                     const std::string& signalName,
                                     Stage stage)
{
  bool connected( true );
  StageCallback* callback =new StageCallback( isolate, javaScriptCallback, signalName );

  if( strcmp( signalName.c_str() ,STAGE_SIGNAL_KEY_EVENT) == 0 )
  {
    stage.KeyEventSignal().Connect( callback, &StageCallback::KeyEventSignal );
   }
  else if ( strcmp( signalName.c_str(),  STAGE_SIGNAL_EVENT_PROCESSING_FINISHED ) == 0)
  {
    stage.EventProcessingFinishedSignal().Connect( callback, &StageCallback::EventProcessingFinishedSignal );
  }
  else if (strcmp( signalName.c_str(), STAGE_SIGNAL_TOUCH ) == 0 )
  {
    stage.TouchSignal().Connect( callback, &StageCallback::TouchSignal );
  }
  else
  {
    connected = false;
  }

  if( connected )
  {
    stageWrapper->GetSignalManager()->AddCallback( static_cast<BaseCallBack*>(callback) );
  }
  else
  {
    delete  callback;
  }
}

void KeyboardFocusManagerConnection( v8::Isolate* isolate,
      v8::Local<v8::Function> javaScriptCallback,
      KeyboardFocusManagerWrapper* keyboardFocusWrapper,
      const std::string& signalName,
      Toolkit::KeyboardFocusManager keyboardFocusManager)
{
  bool connected( true );
  KeyboardFocusCallback* callback =new KeyboardFocusCallback( isolate, javaScriptCallback, signalName );

  if( strcmp( signalName.c_str(), KEYBOARD_FOCUS_MANAGER_SIGNAL_PRE_FOCUS_CHANGE ) == 0 )
  {
    keyboardFocusManager.PreFocusChangeSignal().Connect( callback, &KeyboardFocusCallback::PreFocusChangeSignal );
  }
  else if ( strcmp( signalName.c_str(),  KEYBOARD_FOCUS_MANAGER_SIGNAL_FOCUS_CHANGED ) == 0 )
  {
    keyboardFocusManager.FocusChangedSignal().Connect( callback, &KeyboardFocusCallback::FocusChangedSignal );
  }
  else if ( strcmp( signalName.c_str(), KEYBOARD_FOCUS_MANAGER_SIGNAL_FOCUS_GROUP_CHANGED ) == 0 )
  {
    keyboardFocusManager.FocusGroupChangedSignal().Connect( callback, &KeyboardFocusCallback::FocusGroupSignal );
  }
  else if ( strcmp( signalName.c_str(), KEYBOARD_FOCUS_MANAGER_SIGNAL_FOCUSED_ACTOR_ENTER_KEY ) == 0 )
  {
    keyboardFocusManager.FocusedActorEnterKeySignal().Connect( callback, &KeyboardFocusCallback::EnterKeyPressedActorSignal );
  }
  else
  {
    connected = false;
  }

  if( connected )
  {
    keyboardFocusWrapper->GetSignalManager()->AddCallback( static_cast<BaseCallBack*>(callback) );
  }
  else
  {
    delete  callback;
  }
}

void PanGestureDetectorConnection( v8::Isolate* isolate,
                                     v8::Local<v8::Function> javaScriptCallback,
                                     PanGestureDetectorWrapper* panGestureDetectorWrapper,
                                     const std::string& signalName,
                                     PanGestureDetector panGestureDetector )
{
  if( strcmp( signalName.c_str() , SIGNAL_PAN_DETECTED ) == 0 )
  {
    PanGestureDetectorCallback* callback =new PanGestureDetectorCallback( isolate, javaScriptCallback, signalName );
    panGestureDetector.DetectedSignal().Connect( callback, &PanGestureDetectorCallback::OnPan );
    panGestureDetectorWrapper->GetSignalManager()->AddCallback( static_cast<BaseCallBack*>(callback) );
  }
}

} // un-named namespace


SignalManager::SignalManager()
:mInsideCallback(false)
{

}
SignalManager::~SignalManager()
{
  // the object owning the signal manager ( e.g. an actor wrapper ) has been deleted
  // delete the callbacks objects
  for( CallBackContainer::Iterator iter = mCallbacks.Begin(); iter != mCallbacks.End(); ++iter )
  {
      BaseCallBack* callback = (*iter);
      delete callback;
  }
  for( CallBackContainer::Iterator iter = mDisconnectedCallbacks.Begin(); iter != mDisconnectedCallbacks.End(); ++iter )
  {
    BaseCallBack* callback = (*iter);
    delete callback;
  }
}


void SignalManager::SignalConnect( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  // format is
  // first paramter =  signal to connect to
  // Second parameter = function ( to run )
  // args.This() = myActor
  // e.g. myActor.Connect("touch", myJavaScriptActorTouched );

  // Inside Callback on myJavaScriptActorTouched
  // myActor.Disconnect("touch", myJavaScriptActorTouched );

  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  if( !args[1]->IsFunction() )
  {
    DALI_SCRIPT_EXCEPTION(isolate, "function parameter not found ");
    return;
  }

  // get the function
  v8::Local<v8::Function> func =v8::Handle<v8::Function>::Cast(args[1]);

  // get the handle wrapper
  BaseWrappedObject* baseObject = BaseWrappedObject::UnWrap( isolate, args.This() );

  // First paramter = signal name as a string
  bool found( false );
  std::string signal = V8Utils::GetStringParameter( PARAMETER_0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION(isolate, "signal name parameter not found ");
    return;
  }

  if( signal.empty() || ( signal.length() > MAX_SIGNAL_NAME_LENGTH ) )
  {
    DALI_SCRIPT_EXCEPTION(isolate, "signal name length out of range");
    return;
  }
  // We need to determine what type of function needs to be hooked
  // see if we're connecting to an Actor
  switch( baseObject->GetType() )
  {
    case BaseWrappedObject::ACTOR:
    {
      ActorWrapper* actorWrapper = static_cast< ActorWrapper*>( baseObject );
      ActorConnection( isolate, func, actorWrapper, signal,  actorWrapper->GetActor() );
      break;
    }
    case BaseWrappedObject::ANIMATION:
    {
      AnimationWrapper* animWrapper = static_cast< AnimationWrapper*>( baseObject );
      AnimationConnection( isolate, func, animWrapper, signal, animWrapper->GetAnimation() );
      break;
    }
    case BaseWrappedObject::IMAGE:
    {
      ImageWrapper* imageWrapper = static_cast< ImageWrapper*>( baseObject );
      ImageConnection( isolate, func, imageWrapper, signal, imageWrapper->GetImage() );
      break;
    }
    case BaseWrappedObject::STAGE:
    {
      StageWrapper* stageWrapper = static_cast< StageWrapper*>( baseObject );
      StageConnection( isolate, func, stageWrapper, signal, stageWrapper->GetStage() );
      break;
    }
    case BaseWrappedObject::KEYBOARD_FOCUS_MANAGER:
    {
      KeyboardFocusManagerWrapper* keyboardFocusWrapper = static_cast< KeyboardFocusManagerWrapper*>( baseObject );
      KeyboardFocusManagerConnection( isolate, func, keyboardFocusWrapper, signal, keyboardFocusWrapper->GetKeyboardFocusManager() );
      break;
    }
    case BaseWrappedObject::PAN_GESTURE_DETECTOR:
    {
      PanGestureDetectorWrapper* panGestureDetectorWrapper = static_cast< PanGestureDetectorWrapper*>( baseObject );
      PanGestureDetectorConnection( isolate, func, panGestureDetectorWrapper, signal, panGestureDetectorWrapper->GetPanGestureDetector() );
      break;
    }
    default:
    {
      DALI_SCRIPT_EXCEPTION( isolate, "object does not support connections");
      break;
    }
  }

}

void SignalManager::SignalDisconnect( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  // we can get the function name.... use that to disconnect from the object
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate );

  if( !args[1]->IsFunction() )
  {
    DALI_SCRIPT_EXCEPTION(isolate, "function parameter not found ");
    return;
  }

  // get the function
  v8::Local<v8::Function> func =v8::Handle<v8::Function>::Cast(args[1]);

  // get the handle wrapper
  BaseWrappedObject* baseObject = BaseWrappedObject::UnWrap( isolate, args.This() );

  // First parameter = signal name as a string
  bool found( false );
  std::string signal = V8Utils::GetStringParameter( PARAMETER_0, found, isolate, args );
  if( !found )
  {
    DALI_SCRIPT_EXCEPTION(isolate, "signal name parameter not found ");
    return;
  }

  SignalManager* signalManager = baseObject->GetSignalManager();
  if(signalManager )
  {
    signalManager->RemoveCallback( isolate, signal, func);
  }


}
void SignalManager::AddCallback( BaseCallBack* callback )
{
  // used by the callback to inform signal manager when it is being triggered.
   callback->SetEmitInterface( this );

   mCallbacks.PushBack( callback );
}
void SignalManager::RemoveCallback( v8::Isolate* isolate, const std::string& signalName, const v8::Local<v8::Function>& function )
{
  // lets disconnect
  for( CallBackContainer::Iterator iter = mCallbacks.Begin(); iter != mCallbacks.End(); ++iter )
  {
    BaseCallBack* callback = (*iter);
    if( ( callback->mSignalName == signalName ) &&
        ( callback->IsFunction(  function  )  )  )
    {
      mCallbacks.Erase( iter );

      // if we're inside a callback::Call function, then deleting it will cause a seg fault
      if( mInsideCallback )
      {
        callback->DisconnectAll();
        mDisconnectedCallbacks.PushBack( callback );
      }
      else
      {
        delete callback;
      }
      return;
    }
  }
}

void SignalManager::CallbackEnter()
{
  mInsideCallback = true;
}

void SignalManager::CallbackExit()
{
  // this gets called when we've finished calling into JavaScript, e.g. as a result of animation finishing
  // which is being listened to by a javascript function.

  // it possible that inside the JavaScript call, it tries to disconnect.
  // If this happens the callback is added to the mDisconnectedCallbacks list.
  // We then delay deleting of the callback object until the JavaScript call has finished in this function.

  mInsideCallback = false;

  // lets delete any call backs that were disconnect during the JavaScript call
  for( CallBackContainer::Iterator iter = mDisconnectedCallbacks.Begin(); iter != mDisconnectedCallbacks.End(); ++iter )
  {
    BaseCallBack* callback = (*iter);
    delete callback;
  }
  mDisconnectedCallbacks.Clear();

}

} // namespace V8Plugin

} // namespace Dali

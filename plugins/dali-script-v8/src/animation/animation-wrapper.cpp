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
#include "animation-wrapper.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <dali-wrapper.h>
#include <animation/animation-api.h>
#include <shared/api-function.h>
#include <shared/object-template-helper.h>

namespace Dali
{
namespace V8Plugin
{

v8::Persistent<v8::ObjectTemplate> AnimationWrapper::mAnimationTemplate;

namespace
{

/**
 * Contains a list of all functions that can be called
 */
const ApiFunction AnimationFunctionTable[]=
{
    /**************************************
    * Animation API (in order of animation.h)
    **************************************/

    { "SetDuration"             , AnimationApi::SetDuration               },
    { "GetDuration"             , AnimationApi::GetDuration               },
    { "SetLooping"              , AnimationApi::SetLooping                },
    { "IsLooping"               , AnimationApi::IsLooping                 },
    { "SetEndAction"            , AnimationApi::SetEndAction              },
    { "GetEndAction"            , AnimationApi::GetEndAction              },
    { "SetDisconnectAction"     , AnimationApi::SetDisconnectAction       },
    { "GetDisconnectAction"     , AnimationApi::GetDisconnectAction       },
    { "SetDefaultAlphaFunction" , AnimationApi::SetDefaultAlphaFunction   },
    { "GetDefaultAlphaFunction" , AnimationApi::GetDefaultAlphaFunction   },
    { "GetCurrentProgress"      , AnimationApi::GetCurrentProgress        },
    { "SetSpeedFactor"          , AnimationApi::SetSpeedFactor            },
    { "GetSpeedFactor"          , AnimationApi::GetSpeedFactor            },
    { "SetPlayRange"            , AnimationApi::SetPlayRange              },
    { "GetPlayRange"            , AnimationApi::GetPlayRange              },
    { "SetCurrentProgress"      , AnimationApi::SetCurrentProgress        },
    { "Play"                    , AnimationApi::Play                      },
    { "PlayFrom"                , AnimationApi::PlayFrom                  },
    { "Pause"                   , AnimationApi::Pause                     },
    { "Stop"                    , AnimationApi::Stop                      },
    { "Clear"                   , AnimationApi::Clear                     },
    { "Animate"                 , AnimationApi::Animate                   },
    { "AnimateBy"               , AnimationApi::AnimateBy                 },
    { "AnimateTo"               , AnimationApi::AnimateTo                 },
    { "AnimateBetween"          , AnimationApi::AnimateBetween            },
    { "Show"                    , AnimationApi::Show                      },
    { "Hide"                    , AnimationApi::Hide                      },

};

const unsigned int AnimationFunctionTableCount = sizeof(AnimationFunctionTable)/sizeof(AnimationFunctionTable[0]);
} //un-named space


AnimationWrapper::AnimationWrapper( const Dali::Animation& animation, GarbageCollectorInterface& gc )
: BaseWrappedObject( BaseWrappedObject::ANIMATION , gc )
{
    mAnimation = animation;
}

AnimationWrapper::~AnimationWrapper()
{

}

v8::Handle<v8::Object> AnimationWrapper::WrapAnimation(v8::Isolate* isolate, const Dali::Animation& animation )
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  objectTemplate = GetAnimationTemplate( isolate);

  // create an instance of the template
  v8::Local<v8::Object> localObject = objectTemplate->NewInstance();

  // create the Animation wrapper
  AnimationWrapper* pointer =  new AnimationWrapper( animation, Dali::V8Plugin::DaliWrapper::Get().GetDaliGarbageCollector() );

  // assign the JavaScript object to the wrapper.
  pointer->SetJavascriptObject( isolate, localObject );

  return handleScope.Escape( localObject );
}

v8::Local<v8::ObjectTemplate> AnimationWrapper::GetAnimationTemplate( v8::Isolate* isolate)
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  if( mAnimationTemplate.IsEmpty() )
  {
    objectTemplate = MakeAnimationTemplate( isolate );
    mAnimationTemplate.Reset( isolate, objectTemplate );
  }
  else
  {
    // get the object template
    objectTemplate = v8::Local<v8::ObjectTemplate>::New( isolate, mAnimationTemplate );
  }
  return handleScope.Escape( objectTemplate );
}

v8::Handle<v8::ObjectTemplate> AnimationWrapper::MakeAnimationTemplate( v8::Isolate* isolate )
{
  v8::EscapableHandleScope handleScope( isolate );

  v8::Local<v8::ObjectTemplate> objTemplate = v8::ObjectTemplate::New();

  // add intercepts for Signals, we can't use HandleWrapper::AddIntercepts because Animation doesn't inherit
  // from Handle ( just baseHandle)
  ObjectTemplateHelper::AddSignalConnectAndDisconnect( isolate, objTemplate );

  objTemplate->SetInternalFieldCount( BaseWrappedObject::FIELD_COUNT );

  // add our function properties
  ObjectTemplateHelper::InstallFunctions( isolate, objTemplate, AnimationFunctionTable, AnimationFunctionTableCount );

  return handleScope.Escape( objTemplate );
}

void AnimationWrapper::NewAnimation( const v8::FunctionCallbackInfo< v8::Value >& args)
{
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handleScope( isolate);

  if( !args.IsConstructCall() )
  {
    DALI_SCRIPT_EXCEPTION( isolate, "Animation constructor called without 'new'" );
    return;
  }

  Dali::Animation animation = AnimationApi::New( args );
  v8::Local<v8::Object> localObject = WrapAnimation( isolate, animation );
  args.GetReturnValue().Set( localObject );
}


Animation AnimationWrapper::GetAnimation()
{
  return mAnimation;
}


} // namespace V8Plugin

} // namespace Dali

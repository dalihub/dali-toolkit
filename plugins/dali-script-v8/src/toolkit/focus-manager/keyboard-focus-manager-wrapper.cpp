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
#include "keyboard-focus-manager-wrapper.h"

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <dali-wrapper.h>
#include <shared/api-function.h>
#include <shared/object-template-helper.h>
#include <toolkit/focus-manager/keyboard-focus-manager-api.h>

namespace Dali
{
namespace V8Plugin
{
namespace
{

/**
 * Contains a list of all functions that can be called
 */
const ApiFunction KeyboardFocusManagerFunctionTable[]=
{
    /**************************************
    * KeyboardFocusManager API (in order of keyboard-focus-manager.h)
    **************************************/

    { "SetCurrentFocusActor"    , KeyboardFocusManagerApi::SetCurrentFocusActor   },
    { "GetCurrentFocusActor"    , KeyboardFocusManagerApi::GetCurrentFocusActor   },
    { "MoveFocus"               , KeyboardFocusManagerApi::MoveFocus              },
    { "ClearFocus"              , KeyboardFocusManagerApi::ClearFocus             },
    { "SetAsFocusGroup"         , KeyboardFocusManagerApi::SetAsFocusGroup        },
    { "IsFocusGroup"            , KeyboardFocusManagerApi::IsFocusGroup           },
    { "GetFocusGroup"           , KeyboardFocusManagerApi::GetFocusGroup          },
    { "SetFocusGroupLoop"       , KeyboardFocusManagerApi::SetFocusGroupLoop      },
    { "GetFocusGroupLoop"       , KeyboardFocusManagerApi::GetFocusGroupLoop      },
    { "SetFocusIndicatorActor"  , KeyboardFocusManagerApi::SetFocusIndicatorActor },
    { "GetFocusIndicatorActor"  , KeyboardFocusManagerApi::GetFocusIndicatorActor },
};

const unsigned int KeyboardFocusManagerFunctionTableCount = sizeof(KeyboardFocusManagerFunctionTable)/sizeof(KeyboardFocusManagerFunctionTable[0]);
} //un-named space


KeyboardFocusManagerWrapper::KeyboardFocusManagerWrapper( const Dali::Toolkit::KeyboardFocusManager& keyboardFocusManager, GarbageCollectorInterface& gc )
: BaseWrappedObject( BaseWrappedObject::KEYBOARD_FOCUS_MANAGER , gc )
{
    mKeyboardFocusManager = keyboardFocusManager;
}

KeyboardFocusManagerWrapper::~KeyboardFocusManagerWrapper()
{

}

v8::Handle<v8::Object> KeyboardFocusManagerWrapper::WrapKeyboardFocusManager(v8::Isolate* isolate, const Dali::Toolkit::KeyboardFocusManager& keyboardFocusManager )
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  objectTemplate = GetKeyboardFocusManagerTemplate( isolate);

  // create an instance of the template
  v8::Local<v8::Object> localObject = objectTemplate->NewInstance();

  // create the KeyboardFocusManager wrapper
  KeyboardFocusManagerWrapper* pointer =  new KeyboardFocusManagerWrapper( keyboardFocusManager, Dali::V8Plugin::DaliWrapper::Get().GetDaliGarbageCollector() );

  // assign the JavaScript object to the wrapper.
  pointer->SetJavascriptObject( isolate, localObject );

  return handleScope.Escape( localObject );
}

v8::Local<v8::ObjectTemplate> KeyboardFocusManagerWrapper::GetKeyboardFocusManagerTemplate( v8::Isolate* isolate)
{
  v8::EscapableHandleScope handleScope( isolate );
  v8::Local<v8::ObjectTemplate> objectTemplate;

  v8::Local<v8::ObjectTemplate> objTemplate = v8::ObjectTemplate::New();

  objTemplate->SetInternalFieldCount( BaseWrappedObject::FIELD_COUNT );

  // add intercepts for Signals, we can't use HandleWrapper::AddIntercepts because KeyboardFocusManager doesn't inherit
  // from Handle ( just baseHandle)
  ObjectTemplateHelper::AddSignalConnectAndDisconnect( isolate, objTemplate );

  // add our function properties
  ObjectTemplateHelper::InstallFunctions( isolate, objTemplate, KeyboardFocusManagerFunctionTable, KeyboardFocusManagerFunctionTableCount );

  return handleScope.Escape( objTemplate );

}


std::string KeyboardFocusManagerWrapper::GetDirectionName(  Toolkit::Control::KeyboardFocus::Direction dir )
{
  switch( dir )
  {
    case Dali::Toolkit::Control::KeyboardFocus::LEFT:
    {
      return "left";
      break;
    }
    case Dali::Toolkit::Control::KeyboardFocus::RIGHT:
    {
      return "right";
      break;
    }
    case Dali::Toolkit::Control::KeyboardFocus::UP:
    {
      return "up";
      break;
    }
    case Dali::Toolkit::Control::KeyboardFocus::DOWN:
    {
      return "down";
      break;
    }
    case Dali::Toolkit::Control::KeyboardFocus::PAGE_UP:
    {
      return "pageUp";
      break;
    }
    case Dali::Toolkit::Control::KeyboardFocus::PAGE_DOWN:
    {
      return "pageDown";
      break;
    }
  }
  return "unknown direction";
}


Dali::Toolkit::KeyboardFocusManager KeyboardFocusManagerWrapper::GetKeyboardFocusManager()
{
  return mKeyboardFocusManager;
}


} // namespace V8Plugin

} // namespace Dali

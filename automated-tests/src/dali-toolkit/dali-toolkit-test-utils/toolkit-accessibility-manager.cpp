/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include "toolkit-accessibility-manager.h"

#include <dali/public-api/object/base-object.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * Stub for the AccessibilityManager
 */
class AccessibilityManager : public BaseObject
{
public: // Creation & Destruction

  static Dali::AccessibilityManager Get();

  AccessibilityManager();
  ~AccessibilityManager();

public:

  bool IsEnabled() const;
  void SetActionHandler(Dali::AccessibilityActionHandler& handler);
  void SetGestureHandler(Dali::AccessibilityGestureHandler& handler);

public: // Signals

  Dali::AccessibilityManager::AccessibilityActionSignalType& StatusChangedSignal();
  Dali::AccessibilityManager::AccessibilityActionSignalType& ActionNextSignal();
  Dali::AccessibilityManager::AccessibilityActionSignalType& ActionPreviousSignal();
  Dali::AccessibilityManager::AccessibilityActionSignalType& ActionActivateSignal();
  Dali::AccessibilityManager::AccessibilityActionSignalType& ActionReadSignal();
  Dali::AccessibilityManager::AccessibilityActionSignalType& ActionReadNextSignal();
  Dali::AccessibilityManager::AccessibilityActionSignalType& ActionReadPreviousSignal();
  Dali::AccessibilityManager::AccessibilityActionSignalType& ActionOverSignal();
  Dali::AccessibilityManager::AccessibilityActionSignalType& ActionUpSignal();
  Dali::AccessibilityManager::AccessibilityActionSignalType& ActionDownSignal();
  Dali::AccessibilityManager::AccessibilityActionSignalType& ActionClearFocusSignal();
  Dali::AccessibilityManager::AccessibilityActionSignalType& ActionBackSignal();
  Dali::AccessibilityManager::AccessibilityActionSignalType& ActionControlPanelOpenSignal();

private:

  Dali::AccessibilityManager::AccessibilityActionSignalType mStatusChangedSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalType mActionNextSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalType mActionPreviousSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalType mActionActivateSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalType mActionReadSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalType mActionReadNextSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalType mActionReadPreviousSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalType mActionOverSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalType mActionUpSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalType mActionDownSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalType mActionClearFocusSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalType mActionBackSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalType mActionControlPanelOpenSignal;

  bool mIsEnabled;
  Dali::AccessibilityActionHandler* mActionHandler;
  Dali::AccessibilityGestureHandler* mGestureHandler;

  static Dali::AccessibilityManager mToolkitAccessibilityManager;
};

Dali::AccessibilityManager AccessibilityManager::mToolkitAccessibilityManager;


Dali::AccessibilityManager AccessibilityManager::Get()
{
  if( ! mToolkitAccessibilityManager )
  {
    mToolkitAccessibilityManager = Dali::AccessibilityManager( new Dali::Internal::Adaptor::AccessibilityManager() );
  }
  return mToolkitAccessibilityManager;
}

AccessibilityManager::AccessibilityManager()
: mIsEnabled(false)
{
}

AccessibilityManager::~AccessibilityManager()
{
}

bool AccessibilityManager::IsEnabled() const
{
  return mIsEnabled;
}

void AccessibilityManager::SetActionHandler(Dali::AccessibilityActionHandler& handler)
{
  mActionHandler = &handler;
}

void AccessibilityManager::SetGestureHandler(Dali::AccessibilityGestureHandler& handler)
{
  mGestureHandler = &handler;
}

Dali::AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::StatusChangedSignal()
{
  return mStatusChangedSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionNextSignal()
{
  return mActionNextSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionPreviousSignal()
{
  return mActionPreviousSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionActivateSignal()
{
  return mActionActivateSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionReadSignal()
{
  return mActionReadSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionReadNextSignal()
{
  return mActionReadNextSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionReadPreviousSignal()
{
  return mActionReadPreviousSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionOverSignal()
{
  return mActionOverSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionUpSignal()
{
  return mActionUpSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionDownSignal()
{
  return mActionDownSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionClearFocusSignal()
{
  return mActionClearFocusSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionBackSignal()
{
  return mActionBackSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionControlPanelOpenSignal()
{
  return mActionControlPanelOpenSignal;
}

static Internal::Adaptor::AccessibilityManager& GetImplementation(Dali::AccessibilityManager& manager)
{
  BaseObject& handle = manager.GetBaseObject();
  return static_cast<Internal::Adaptor::AccessibilityManager&>(handle);
}


} // namespace Adaptor
} // namespace Internal

////////////////////////////////////////////////////////////////////////////////////////////////////

AccessibilityManager::AccessibilityManager()
{
}

AccessibilityManager AccessibilityManager::Get()
{
  return Internal::Adaptor::AccessibilityManager::Get();
}

AccessibilityManager::~AccessibilityManager()
{
}

Vector2 AccessibilityManager::GetReadPosition() const
{
  //return Internal::Adaptor::GetImplementation(*this).GetReadPosition();
  return Vector2::ZERO;
}

bool AccessibilityManager::IsEnabled() const
{
  //return Internal::Adaptor::GetImplementation(*this).IsEnabled();
  return false;
}

void AccessibilityManager::SetActionHandler(AccessibilityActionHandler& handler)
{
  Internal::Adaptor::GetImplementation(*this).SetActionHandler(handler);
}

void AccessibilityManager::SetGestureHandler(AccessibilityGestureHandler& handler)
{
  Internal::Adaptor::GetImplementation(*this).SetGestureHandler(handler);
}

bool AccessibilityManager::HandleActionNextEvent()
{
  return true; //Internal::Adaptor::GetImplementation(*this).HandleActionNextEvent();
}

bool AccessibilityManager::HandleActionPreviousEvent()
{
  return true; //Internal::Adaptor::GetImplementation(*this).HandleActionPreviousEvent();
}

bool AccessibilityManager::HandleActionActivateEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionActivateEvent();
}

bool AccessibilityManager::HandleActionReadEvent(unsigned int x, unsigned int y,  bool allowReadAgain)
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionReadEvent(x, y, allowReadAgain);
}

bool AccessibilityManager::HandleActionReadNextEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionReadNextEvent();
}

bool AccessibilityManager::HandleActionReadPreviousEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionReadPreviousEvent();
}

bool AccessibilityManager::HandleActionUpEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionUpEvent();
}

bool AccessibilityManager::HandleActionDownEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionDownEvent();
}

bool AccessibilityManager::HandleActionClearFocusEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionClearFocusEvent();
}

bool AccessibilityManager::HandleActionScrollEvent(TouchPoint& point, unsigned long timeStamp)
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionScrollEvent(point, timeStamp);
}

bool AccessibilityManager::HandleActionBackEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionBackEvent();
}

void AccessibilityManager::HandleActionEnableEvent()
{
  //Internal::Adaptor::GetImplementation(*this).HandleActionEnableEvent();
}

void AccessibilityManager::HandleActionDisableEvent()
{
  //Internal::Adaptor::GetImplementation(*this).HandleActionDisableEvent();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::StatusChangedSignal()
{
  return Internal::Adaptor::GetImplementation(*this).StatusChangedSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionNextSignal()
{
  return Internal::Adaptor::GetImplementation(*this).ActionNextSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionPreviousSignal()
{
  return Internal::Adaptor::GetImplementation(*this).ActionPreviousSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionActivateSignal()
{
  return Internal::Adaptor::GetImplementation(*this).ActionActivateSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionOverSignal()
{
  return Internal::Adaptor::GetImplementation(*this).ActionOverSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionReadSignal()
{
  return Internal::Adaptor::GetImplementation(*this).ActionReadSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionReadNextSignal()
{
  return Internal::Adaptor::GetImplementation(*this).ActionReadNextSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionReadPreviousSignal()
{
  return Internal::Adaptor::GetImplementation(*this).ActionReadPreviousSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionUpSignal()
{
  return Internal::Adaptor::GetImplementation(*this).ActionUpSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionDownSignal()
{
  return Internal::Adaptor::GetImplementation(*this).ActionDownSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionClearFocusSignal()
{
  return Internal::Adaptor::GetImplementation(*this).ActionClearFocusSignal();
}

AccessibilityManager::AccessibilityActionSignalType& AccessibilityManager::ActionBackSignal()
{
  return Internal::Adaptor::GetImplementation(*this).ActionBackSignal();
}

AccessibilityManager::AccessibilityManager( Internal::Adaptor::AccessibilityManager* manager )
: BaseHandle( manager )
{
}

} // namespace Dali

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

  Dali::AccessibilityManager::AccessibilityActionSignalV2& StatusChangedSignal();
  Dali::AccessibilityManager::AccessibilityActionSignalV2& ActionNextSignal();
  Dali::AccessibilityManager::AccessibilityActionSignalV2& ActionPreviousSignal();
  Dali::AccessibilityManager::AccessibilityActionSignalV2& ActionActivateSignal();
  Dali::AccessibilityManager::AccessibilityActionSignalV2& ActionReadSignal();
  Dali::AccessibilityManager::AccessibilityActionSignalV2& ActionReadNextSignal();
  Dali::AccessibilityManager::AccessibilityActionSignalV2& ActionReadPreviousSignal();
  Dali::AccessibilityManager::AccessibilityActionSignalV2& ActionOverSignal();
  Dali::AccessibilityManager::AccessibilityActionSignalV2& ActionUpSignal();
  Dali::AccessibilityManager::AccessibilityActionSignalV2& ActionDownSignal();
  Dali::AccessibilityManager::AccessibilityActionSignalV2& ActionClearFocusSignal();
  Dali::AccessibilityManager::AccessibilityActionSignalV2& ActionBackSignal();
  Dali::AccessibilityManager::AccessibilityActionSignalV2& ActionControlPanelOpenSignal();

private:

  Dali::AccessibilityManager::AccessibilityActionSignalV2 mStatusChangedSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalV2 mActionNextSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalV2 mActionPreviousSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalV2 mActionActivateSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalV2 mActionReadSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalV2 mActionReadNextSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalV2 mActionReadPreviousSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalV2 mActionOverSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalV2 mActionUpSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalV2 mActionDownSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalV2 mActionClearFocusSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalV2 mActionBackSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalV2 mActionControlPanelOpenSignal;

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

Dali::AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::StatusChangedSignal()
{
  return mStatusChangedSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionNextSignal()
{
  return mActionNextSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionPreviousSignal()
{
  return mActionPreviousSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionActivateSignal()
{
  return mActionActivateSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionReadSignal()
{
  return mActionReadSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionReadNextSignal()
{
  return mActionReadNextSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionReadPreviousSignal()
{
  return mActionReadPreviousSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionOverSignal()
{
  return mActionOverSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionUpSignal()
{
  return mActionUpSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionDownSignal()
{
  return mActionDownSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionClearFocusSignal()
{
  return mActionClearFocusSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionBackSignal()
{
  return mActionBackSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionControlPanelOpenSignal()
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

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::StatusChangedSignal()
{
  return Internal::Adaptor::GetImplementation(*this).StatusChangedSignal();
}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionNextSignal()
{
  return Internal::Adaptor::GetImplementation(*this).ActionNextSignal();
}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionPreviousSignal()
{
  return Internal::Adaptor::GetImplementation(*this).ActionPreviousSignal();
}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionActivateSignal()
{
  return Internal::Adaptor::GetImplementation(*this).ActionActivateSignal();
}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionOverSignal()
{
  return Internal::Adaptor::GetImplementation(*this).ActionOverSignal();
}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionReadSignal()
{
  return Internal::Adaptor::GetImplementation(*this).ActionReadSignal();
}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionReadNextSignal()
{
  return Internal::Adaptor::GetImplementation(*this).ActionReadNextSignal();
}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionReadPreviousSignal()
{
  return Internal::Adaptor::GetImplementation(*this).ActionReadPreviousSignal();
}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionUpSignal()
{
  return Internal::Adaptor::GetImplementation(*this).ActionUpSignal();
}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionDownSignal()
{
  return Internal::Adaptor::GetImplementation(*this).ActionDownSignal();
}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionClearFocusSignal()
{
  return Internal::Adaptor::GetImplementation(*this).ActionClearFocusSignal();
}

AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::ActionBackSignal()
{
  return Internal::Adaptor::GetImplementation(*this).ActionBackSignal();
}

AccessibilityManager::AccessibilityManager( Internal::Adaptor::AccessibilityManager* manager )
: BaseHandle( manager )
{
}

} // namespace Dali

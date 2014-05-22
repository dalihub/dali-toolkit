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

#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/dali-signal-v2.h>

namespace Dali
{

namespace
{
ToolkitAccessibilityManager* gToolkitAccessibilityManager(NULL);
} // unnamed namespace

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
  AccessibilityManager(ToolkitAccessibilityManager *accessibilityActionDetector);
  ~AccessibilityManager();

public:

  bool IsEnabled() const;
  void SetActionHandler(Dali::AccessibilityActionHandler& handler);
  void SetGestureHandler(Dali::AccessibilityGestureHandler& handler);

public: // Signals

  Dali::AccessibilityManager::AccessibilityActionSignalV2& SignalStatusChanged();
  Dali::AccessibilityManager::AccessibilityActionSignalV2& SignalActionNext();
  Dali::AccessibilityManager::AccessibilityActionSignalV2& SignalActionPrevious();
  Dali::AccessibilityManager::AccessibilityActionSignalV2& SignalActionActivate();
  Dali::AccessibilityManager::AccessibilityActionSignalV2& SignalActionRead();
  Dali::AccessibilityManager::AccessibilityActionSignalV2& SignalActionReadNext();
  Dali::AccessibilityManager::AccessibilityActionSignalV2& SignalActionReadPrevious();
  Dali::AccessibilityManager::AccessibilityActionSignalV2& SignalActionUp();
  Dali::AccessibilityManager::AccessibilityActionSignalV2& SignalActionDown();
  Dali::AccessibilityManager::AccessibilityActionSignalV2& SignalActionClearFocus();
  Dali::AccessibilityManager::AccessibilityActionSignalV2& SignalActionBack();
  Dali::AccessibilityManager::AccessibilityActionSignalV2& SignalActionControlPanelOpen();

private:

  ToolkitAccessibilityManager* mToolkitAccessibilityManager;
  Dali::AccessibilityManager::AccessibilityActionSignalV2 mStatusChangedSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalV2 mActionNextSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalV2 mActionPreviousSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalV2 mActionActivateSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalV2 mActionReadSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalV2 mActionReadNextSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalV2 mActionReadPreviousSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalV2 mActionUpSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalV2 mActionDownSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalV2 mActionClearFocusSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalV2 mActionBackSignal;
  Dali::AccessibilityManager::AccessibilityActionSignalV2 mActionControlPanelOpenSignal;

  bool mIsEnabled;
  Dali::AccessibilityActionHandler* mActionHandler;
  Dali::AccessibilityGestureHandler* mGestureHandler;

};

Dali::AccessibilityManager AccessibilityManager::Get()
{
  return gToolkitAccessibilityManager->GetAccessibilityManager();
}

AccessibilityManager::AccessibilityManager()
: mToolkitAccessibilityManager(NULL),
  mIsEnabled(false)
{
}

AccessibilityManager::AccessibilityManager(ToolkitAccessibilityManager *accessibilityActionDetector)
: mToolkitAccessibilityManager(accessibilityActionDetector),
  mIsEnabled(false)
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

Dali::AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::SignalStatusChanged()
{
  return mStatusChangedSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::SignalActionNext()
{
  return mActionNextSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::SignalActionPrevious()
{
  return mActionPreviousSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::SignalActionActivate()
{
  return mActionActivateSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::SignalActionRead()
{
  return mActionReadSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::SignalActionReadNext()
{
  return mActionReadNextSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::SignalActionReadPrevious()
{
  return mActionReadPreviousSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::SignalActionUp()
{
  return mActionUpSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::SignalActionDown()
{
  return mActionDownSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::SignalActionClearFocus()
{
  return mActionClearFocusSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::SignalActionBack()
{
  return mActionBackSignal;
}

Dali::AccessibilityManager::AccessibilityActionSignalV2& AccessibilityManager::SignalActionControlPanelOpen()
{
  return mActionControlPanelOpenSignal;
}

} // namespace Adaptor

} // namespace Internal

////////////////////////////////////////////////////////////////////////////////////////////////////

ToolkitAccessibilityManager::ToolkitAccessibilityManager()
: mAccessibilityManagerStub(new Internal::Adaptor::AccessibilityManager(this)),
  mAccessibilityManager( mAccessibilityManagerStub )
{
  gToolkitAccessibilityManager = this;
}

ToolkitAccessibilityManager::~ToolkitAccessibilityManager()
{
  gToolkitAccessibilityManager = NULL;
}

AccessibilityManager ToolkitAccessibilityManager::GetAccessibilityManager()
{
  return mAccessibilityManager;
}

} // namespace Dali

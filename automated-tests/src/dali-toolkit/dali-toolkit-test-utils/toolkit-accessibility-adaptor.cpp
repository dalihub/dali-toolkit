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

#include "toolkit-accessibility-adaptor.h"

#include <dali/public-api/object/base-object.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * Stub for the AccessibilityAdaptor
 */
class AccessibilityAdaptor : public BaseObject
{
public: // Creation & Destruction

  static Dali::AccessibilityAdaptor Get();

  AccessibilityAdaptor();
  ~AccessibilityAdaptor();

public:

  bool IsEnabled() const;
  void SetActionHandler(Dali::AccessibilityActionHandler& handler);
  void SetGestureHandler(Dali::AccessibilityGestureHandler& handler);

public: // Signals

private:

  bool mIsEnabled;
  Dali::AccessibilityActionHandler* mActionHandler;
  Dali::AccessibilityGestureHandler* mGestureHandler;

  static Dali::AccessibilityAdaptor mToolkitAccessibilityAdaptor;
};

Dali::AccessibilityAdaptor AccessibilityAdaptor::mToolkitAccessibilityAdaptor;


Dali::AccessibilityAdaptor AccessibilityAdaptor::Get()
{
  if( ! mToolkitAccessibilityAdaptor )
  {
    mToolkitAccessibilityAdaptor = Dali::AccessibilityAdaptor( new Dali::Internal::Adaptor::AccessibilityAdaptor() );
  }
  return mToolkitAccessibilityAdaptor;
}

AccessibilityAdaptor::AccessibilityAdaptor()
: mIsEnabled(false)
{
}

AccessibilityAdaptor::~AccessibilityAdaptor()
{
}

bool AccessibilityAdaptor::IsEnabled() const
{
  return mIsEnabled;
}

void AccessibilityAdaptor::SetActionHandler(Dali::AccessibilityActionHandler& handler)
{
  mActionHandler = &handler;
}

void AccessibilityAdaptor::SetGestureHandler(Dali::AccessibilityGestureHandler& handler)
{
  mGestureHandler = &handler;
}

static Internal::Adaptor::AccessibilityAdaptor& GetImplementation(Dali::AccessibilityAdaptor& adaptor)
{
  BaseObject& handle = adaptor.GetBaseObject();
  return static_cast<Internal::Adaptor::AccessibilityAdaptor&>(handle);
}


} // namespace Adaptor
} // namespace Internal

////////////////////////////////////////////////////////////////////////////////////////////////////

AccessibilityAdaptor::AccessibilityAdaptor()
{
}

AccessibilityAdaptor AccessibilityAdaptor::Get()
{
  return Internal::Adaptor::AccessibilityAdaptor::Get();
}

AccessibilityAdaptor::~AccessibilityAdaptor()
{
}

Vector2 AccessibilityAdaptor::GetReadPosition() const
{
  //return Internal::Adaptor::GetImplementation(*this).GetReadPosition();
  return Vector2::ZERO;
}

bool AccessibilityAdaptor::IsEnabled() const
{
  //return Internal::Adaptor::GetImplementation(*this).IsEnabled();
  return false;
}

void AccessibilityAdaptor::SetActionHandler(AccessibilityActionHandler& handler)
{
  Internal::Adaptor::GetImplementation(*this).SetActionHandler(handler);
}

void AccessibilityAdaptor::SetGestureHandler(AccessibilityGestureHandler& handler)
{
  Internal::Adaptor::GetImplementation(*this).SetGestureHandler(handler);
}

bool AccessibilityAdaptor::HandleActionNextEvent()
{
  return true; //Internal::Adaptor::GetImplementation(*this).HandleActionNextEvent();
}

bool AccessibilityAdaptor::HandleActionPreviousEvent()
{
  return true; //Internal::Adaptor::GetImplementation(*this).HandleActionPreviousEvent();
}

bool AccessibilityAdaptor::HandleActionActivateEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionActivateEvent();
}

bool AccessibilityAdaptor::HandleActionReadEvent(unsigned int x, unsigned int y,  bool allowReadAgain)
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionReadEvent(x, y, allowReadAgain);
}

bool AccessibilityAdaptor::HandleActionReadNextEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionReadNextEvent();
}

bool AccessibilityAdaptor::HandleActionReadPreviousEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionReadPreviousEvent();
}

bool AccessibilityAdaptor::HandleActionUpEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionUpEvent();
}

bool AccessibilityAdaptor::HandleActionDownEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionDownEvent();
}

bool AccessibilityAdaptor::HandleActionClearFocusEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionClearFocusEvent();
}

bool AccessibilityAdaptor::HandleActionScrollEvent(TouchPoint& point, unsigned long timeStamp)
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionScrollEvent(point, timeStamp);
}

bool AccessibilityAdaptor::HandleActionBackEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionBackEvent();
}

void AccessibilityAdaptor::HandleActionEnableEvent()
{
  //Internal::Adaptor::GetImplementation(*this).HandleActionEnableEvent();
}

void AccessibilityAdaptor::HandleActionDisableEvent()
{
  //Internal::Adaptor::GetImplementation(*this).HandleActionDisableEvent();
}

bool AccessibilityAdaptor::HandleActionScrollUpEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionScrollUpEvent();
}

bool AccessibilityAdaptor::HandleActionScrollDownEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionScrollDownEvent();
}

bool AccessibilityAdaptor::HandleActionPageLeftEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionPageLeftEvent();
}

bool AccessibilityAdaptor::HandleActionPageRightEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionPageRightEvent();
}

bool AccessibilityAdaptor::HandleActionPageUpEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionPageUpEvent();
}

bool AccessibilityAdaptor::HandleActionPageDownEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionPageDownEvent();
}

bool AccessibilityAdaptor::HandleActionMoveToFirstEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionMoveToFirstEvent();
}

bool AccessibilityAdaptor::HandleActionMoveToLastEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionMoveToLastEvent();
}

bool AccessibilityAdaptor::HandleActionReadFromTopEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionReadFromTopEvent();
}

bool AccessibilityAdaptor::HandleActionReadFromNextEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionReadFromNextEvent();
}

bool AccessibilityAdaptor::HandleActionZoomEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionZoomEvent();
}

bool AccessibilityAdaptor::HandleActionReadIndicatorInformationEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionReadIndicatorInformationEvent();
}

bool AccessibilityAdaptor::HandleActionReadPauseResumeEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionReadPauseResumeEvent();
}

bool AccessibilityAdaptor::HandleActionStartStopEvent()
{
  return true;//Internal::Adaptor::GetImplementation(*this).HandleActionStartStopEvent();
}

AccessibilityAdaptor::AccessibilityAdaptor( Internal::Adaptor::AccessibilityAdaptor* adaptor )
: BaseHandle( adaptor )
{
}

} // namespace Dali

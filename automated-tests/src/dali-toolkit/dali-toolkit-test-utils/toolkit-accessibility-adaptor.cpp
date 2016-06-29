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

#include <dali/public-api/object/base-object.h>
#include <dali/integration-api/events/pan-gesture-event.h>
#include <dali/devel-api/adaptor-framework/accessibility-adaptor.h>
#include <dali/devel-api/adaptor-framework/accessibility-action-handler.h>
#include <dali/devel-api/adaptor-framework/accessibility-gesture-handler.h>

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

  // Functions to modify mock returns:

  void MockSetReadPosition( Vector2& position );

  void SetEnabled(bool enabled)
  {
    mIsEnabled = enabled;
  }

  void SendPanGesture( const Dali::Integration::PanGestureEvent& panEvent );

public:

  bool IsEnabled() const;
  void SetActionHandler(Dali::AccessibilityActionHandler& handler);
  void SetGestureHandler(Dali::AccessibilityGestureHandler& handler);

  Vector2 GetReadPosition() const;

  bool HandleActionNextEvent(bool);
  bool HandleActionPreviousEvent(bool);
  bool HandleActionActivateEvent();
  bool HandleActionReadEvent(unsigned int x, unsigned int y, bool allowReadAgain);
  bool HandleActionReadNextEvent(bool);
  bool HandleActionReadPreviousEvent(bool);
  bool HandleActionUpEvent();
  bool HandleActionDownEvent();
  bool HandleActionClearFocusEvent();
  bool HandleActionScrollEvent(const TouchPoint& point, unsigned long timeStamp);
  bool HandleActionTouchEvent(const TouchPoint& point, unsigned long timeStamp);
  bool HandleActionBackEvent();
  bool HandleActionEnableEvent();
  bool HandleActionDisableEvent();
  bool HandleActionScrollUpEvent();
  bool HandleActionScrollDownEvent();
  bool HandleActionPageLeftEvent();
  bool HandleActionPageRightEvent();
  bool HandleActionPageUpEvent();
  bool HandleActionPageDownEvent();
  bool HandleActionMoveToFirstEvent();
  bool HandleActionMoveToLastEvent();
  bool HandleActionReadFromTopEvent();
  bool HandleActionReadFromNextEvent();
  bool HandleActionZoomEvent();
  bool HandleActionReadIndicatorInformationEvent();
  bool HandleActionReadPauseResumeEvent();
  bool HandleActionStartStopEvent();

private:

  bool mIsEnabled;
  Dali::AccessibilityActionHandler* mActionHandler;
  Dali::AccessibilityGestureHandler* mGestureHandler;
  Vector2 mReadPosition;

  static Dali::AccessibilityAdaptor mToolkitAccessibilityAdaptor;
};

Dali::AccessibilityAdaptor AccessibilityAdaptor::mToolkitAccessibilityAdaptor;


Dali::AccessibilityAdaptor AccessibilityAdaptor::Get()
{
  if( !mToolkitAccessibilityAdaptor )
  {
    mToolkitAccessibilityAdaptor = Dali::AccessibilityAdaptor( new Dali::Internal::Adaptor::AccessibilityAdaptor() );
  }
  return mToolkitAccessibilityAdaptor;
}

AccessibilityAdaptor::AccessibilityAdaptor()
: mIsEnabled(false),
  mReadPosition( 0.0f, 0.0f )
{
}

AccessibilityAdaptor::~AccessibilityAdaptor()
{
}

Vector2 AccessibilityAdaptor::GetReadPosition() const
{
  return mReadPosition;
}

void AccessibilityAdaptor::MockSetReadPosition( Vector2& position )
{
  mReadPosition = position;
}

bool AccessibilityAdaptor::IsEnabled() const
{
  return mIsEnabled;
}

void AccessibilityAdaptor::SendPanGesture( const Integration::PanGestureEvent& panEvent )
{
  mGestureHandler->HandlePanGesture( panEvent );
}

void AccessibilityAdaptor::SetActionHandler(Dali::AccessibilityActionHandler& handler)
{
  mActionHandler = &handler;
}

void AccessibilityAdaptor::SetGestureHandler(Dali::AccessibilityGestureHandler& handler)
{
  mGestureHandler = &handler;
}

bool AccessibilityAdaptor::HandleActionNextEvent(bool allowEndFeedback)
{
  if( mActionHandler )
  {
    return mActionHandler->AccessibilityActionNext( true );
  }
  return false;
}

bool AccessibilityAdaptor::HandleActionPreviousEvent(bool allowEndFeedback)
{
  if( mActionHandler )
  {
    return mActionHandler->AccessibilityActionPrevious( true );
  }
  return false;
}

bool AccessibilityAdaptor::HandleActionActivateEvent()
{
  if( mActionHandler )
  {
    return mActionHandler->AccessibilityActionActivate();
  }
  return false;
}

bool AccessibilityAdaptor::HandleActionReadEvent(unsigned int x, unsigned int y,  bool allowReadAgain)
{
  if( mActionHandler )
  {
    return mActionHandler->AccessibilityActionRead( allowReadAgain );
  }
  return false;
}

bool AccessibilityAdaptor::HandleActionReadNextEvent(bool allowEndFeedback)
{
  if( mActionHandler )
  {
    return mActionHandler->AccessibilityActionReadNext( true );
  }
  return false;
}

bool AccessibilityAdaptor::HandleActionReadPreviousEvent(bool allowEndFeedback)
{
  if( mActionHandler )
  {
    return mActionHandler->AccessibilityActionReadPrevious( true );
  }
  return false;
}

bool AccessibilityAdaptor::HandleActionUpEvent()
{
  if( mActionHandler )
  {
    return mActionHandler->AccessibilityActionUp();
  }
  return false;
}

bool AccessibilityAdaptor::HandleActionDownEvent()
{
  if( mActionHandler )
  {
    return mActionHandler->AccessibilityActionDown();
  }
  return false;
}

bool AccessibilityAdaptor::HandleActionClearFocusEvent()
{
  if( mActionHandler )
  {
    return mActionHandler->ClearAccessibilityFocus();
  }
  return false;
}

bool AccessibilityAdaptor::HandleActionScrollEvent(const TouchPoint& point, unsigned long timeStamp)
{
  if( mActionHandler )
  {
    Dali::TouchEvent touchEvent;
    touchEvent.points.push_back( point );
    return mActionHandler->AccessibilityActionScroll( touchEvent );
  }
  return false;
}

bool AccessibilityAdaptor::HandleActionTouchEvent(const TouchPoint& point, unsigned long timeStamp)
{
  if( mActionHandler )
  {
    Dali::TouchEvent touchEvent;
    touchEvent.points.push_back( point );
    return mActionHandler->AccessibilityActionTouch( touchEvent );
  }
  return false;
}

bool AccessibilityAdaptor::HandleActionBackEvent()
{
  if( mActionHandler )
  {
    return mActionHandler->AccessibilityActionBack();
  }
  return false;
}

bool AccessibilityAdaptor::HandleActionEnableEvent()
{
  if( mActionHandler )
  {
    return mActionHandler->ChangeAccessibilityStatus();
  }
  return false;
}

bool AccessibilityAdaptor::HandleActionDisableEvent()
{
  if( mActionHandler )
  {
    return mActionHandler->ChangeAccessibilityStatus();
  }
  return false;
}

bool AccessibilityAdaptor::HandleActionScrollUpEvent()
{
  if( mActionHandler )
  {
    return mActionHandler->AccessibilityActionScrollUp();
  }
  return false;
}

bool AccessibilityAdaptor::HandleActionScrollDownEvent()
{
  if( mActionHandler )
  {
    return mActionHandler->AccessibilityActionScrollDown();
  }
  return false;
}

bool AccessibilityAdaptor::HandleActionPageLeftEvent()
{
  if( mActionHandler )
  {
    return mActionHandler->AccessibilityActionPageLeft();
  }
  return false;
}

bool AccessibilityAdaptor::HandleActionPageRightEvent()
{
  if( mActionHandler )
  {
    return mActionHandler->AccessibilityActionPageRight();
  }
  return false;
}

bool AccessibilityAdaptor::HandleActionPageUpEvent()
{
  if( mActionHandler )
  {
    return mActionHandler->AccessibilityActionPageUp();
  }
  return false;
}

bool AccessibilityAdaptor::HandleActionPageDownEvent()
{
  if( mActionHandler )
  {
    return mActionHandler->AccessibilityActionPageDown();
  }
  return false;
}

bool AccessibilityAdaptor::HandleActionMoveToFirstEvent()
{
  if( mActionHandler )
  {
    return mActionHandler->AccessibilityActionMoveToFirst();
  }
  return false;
}

bool AccessibilityAdaptor::HandleActionMoveToLastEvent()
{
  if( mActionHandler )
  {
    return mActionHandler->AccessibilityActionMoveToLast();
  }
  return false;
}

bool AccessibilityAdaptor::HandleActionReadFromTopEvent()
{
  if( mActionHandler )
  {
    return mActionHandler->AccessibilityActionReadFromTop();
  }
  return false;
}

bool AccessibilityAdaptor::HandleActionReadFromNextEvent()
{
  if( mActionHandler )
  {
    return mActionHandler->AccessibilityActionReadFromNext();
  }
  return false;
}

bool AccessibilityAdaptor::HandleActionZoomEvent()
{
  if( mActionHandler )
  {
    return mActionHandler->AccessibilityActionZoom();
  }
  return false;
}

bool AccessibilityAdaptor::HandleActionReadIndicatorInformationEvent()
{
  if( mActionHandler )
  {
    return mActionHandler->AccessibilityActionReadIndicatorInformation();
  }
  return false;
}

bool AccessibilityAdaptor::HandleActionReadPauseResumeEvent()
{
  if( mActionHandler )
  {
    return mActionHandler->AccessibilityActionReadPauseResume();
  }
  return false;
}

bool AccessibilityAdaptor::HandleActionStartStopEvent()
{
  if( mActionHandler )
  {
    return mActionHandler->AccessibilityActionStartStop();
  }
  return false;
}

static Internal::Adaptor::AccessibilityAdaptor& GetImplementation(Dali::AccessibilityAdaptor& adaptor)
{
  BaseObject& handle = adaptor.GetBaseObject();
  return static_cast<Internal::Adaptor::AccessibilityAdaptor&>(handle);
}

static const Internal::Adaptor::AccessibilityAdaptor& GetImplementation(const Dali::AccessibilityAdaptor& adaptor)
{
  const BaseObject& handle = adaptor.GetBaseObject();
  return static_cast<const Internal::Adaptor::AccessibilityAdaptor&>(handle);
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

// Methods:

Vector2 AccessibilityAdaptor::GetReadPosition() const
{
  return Internal::Adaptor::GetImplementation(*this).GetReadPosition();
}

bool AccessibilityAdaptor::IsEnabled() const
{
  return Internal::Adaptor::GetImplementation(*this).IsEnabled();
}

void AccessibilityAdaptor::SetActionHandler(AccessibilityActionHandler& handler)
{
  Internal::Adaptor::GetImplementation(*this).SetActionHandler(handler);
}

void AccessibilityAdaptor::SetGestureHandler(AccessibilityGestureHandler& handler)
{
  Internal::Adaptor::GetImplementation(*this).SetGestureHandler(handler);
}

bool AccessibilityAdaptor::HandleActionNextEvent(bool allowEndFeedback)
{
  return Internal::Adaptor::GetImplementation(*this).HandleActionNextEvent(allowEndFeedback);
}

bool AccessibilityAdaptor::HandleActionPreviousEvent(bool allowEndFeedback)
{
  return Internal::Adaptor::GetImplementation(*this).HandleActionPreviousEvent(allowEndFeedback);
}

bool AccessibilityAdaptor::HandleActionActivateEvent()
{
  return Internal::Adaptor::GetImplementation(*this).HandleActionActivateEvent();
}

bool AccessibilityAdaptor::HandleActionReadEvent(unsigned int x, unsigned int y,  bool allowReadAgain)
{
  return Internal::Adaptor::GetImplementation(*this).HandleActionReadEvent( x, y, allowReadAgain );
}

bool AccessibilityAdaptor::HandleActionReadNextEvent(bool allowEndFeedback)
{
  return Internal::Adaptor::GetImplementation(*this).HandleActionReadNextEvent(allowEndFeedback);
}

bool AccessibilityAdaptor::HandleActionReadPreviousEvent(bool allowEndFeedback)
{
  return Internal::Adaptor::GetImplementation(*this).HandleActionReadPreviousEvent(allowEndFeedback);
}

bool AccessibilityAdaptor::HandleActionUpEvent()
{
  return Internal::Adaptor::GetImplementation(*this).HandleActionUpEvent();
}

bool AccessibilityAdaptor::HandleActionDownEvent()
{
  return Internal::Adaptor::GetImplementation(*this).HandleActionDownEvent();
}

bool AccessibilityAdaptor::HandleActionClearFocusEvent()
{
  return Internal::Adaptor::GetImplementation(*this).HandleActionClearFocusEvent();
}

bool AccessibilityAdaptor::HandleActionScrollEvent(const TouchPoint& point, unsigned long timeStamp)
{
  return Internal::Adaptor::GetImplementation(*this).HandleActionScrollEvent(point, timeStamp);
}

bool AccessibilityAdaptor::HandleActionTouchEvent(const TouchPoint& point, unsigned long timeStamp)
{
  return Internal::Adaptor::GetImplementation(*this).HandleActionTouchEvent(point, timeStamp);
}

bool AccessibilityAdaptor::HandleActionBackEvent()
{
  return Internal::Adaptor::GetImplementation(*this).HandleActionBackEvent();
}

void AccessibilityAdaptor::HandleActionEnableEvent()
{
  Internal::Adaptor::GetImplementation(*this).HandleActionEnableEvent();
}

void AccessibilityAdaptor::HandleActionDisableEvent()
{
  Internal::Adaptor::GetImplementation(*this).HandleActionDisableEvent();
}

bool AccessibilityAdaptor::HandleActionScrollUpEvent()
{
  return Internal::Adaptor::GetImplementation(*this).HandleActionScrollUpEvent();
}

bool AccessibilityAdaptor::HandleActionScrollDownEvent()
{
  return Internal::Adaptor::GetImplementation(*this).HandleActionScrollDownEvent();
}

bool AccessibilityAdaptor::HandleActionPageLeftEvent()
{
  return Internal::Adaptor::GetImplementation(*this).HandleActionPageLeftEvent();
}

bool AccessibilityAdaptor::HandleActionPageRightEvent()
{
  return Internal::Adaptor::GetImplementation(*this).HandleActionPageRightEvent();
}

bool AccessibilityAdaptor::HandleActionPageUpEvent()
{
  return Internal::Adaptor::GetImplementation(*this).HandleActionPageUpEvent();
}

bool AccessibilityAdaptor::HandleActionPageDownEvent()
{
  return Internal::Adaptor::GetImplementation(*this).HandleActionPageDownEvent();
}

bool AccessibilityAdaptor::HandleActionMoveToFirstEvent()
{
  return Internal::Adaptor::GetImplementation(*this).HandleActionMoveToFirstEvent();
}

bool AccessibilityAdaptor::HandleActionMoveToLastEvent()
{
  return Internal::Adaptor::GetImplementation(*this).HandleActionMoveToLastEvent();
}

bool AccessibilityAdaptor::HandleActionReadFromTopEvent()
{
  return Internal::Adaptor::GetImplementation(*this).HandleActionReadFromTopEvent();
}

bool AccessibilityAdaptor::HandleActionReadFromNextEvent()
{
  return Internal::Adaptor::GetImplementation(*this).HandleActionReadFromNextEvent();
}

bool AccessibilityAdaptor::HandleActionZoomEvent()
{
  return Internal::Adaptor::GetImplementation(*this).HandleActionZoomEvent();
}

bool AccessibilityAdaptor::HandleActionReadIndicatorInformationEvent()
{
  return Internal::Adaptor::GetImplementation(*this).HandleActionReadIndicatorInformationEvent();
}

bool AccessibilityAdaptor::HandleActionReadPauseResumeEvent()
{
  return Internal::Adaptor::GetImplementation(*this).HandleActionReadPauseResumeEvent();
}

bool AccessibilityAdaptor::HandleActionStartStopEvent()
{
  return Internal::Adaptor::GetImplementation(*this).HandleActionStartStopEvent();
}

AccessibilityAdaptor::AccessibilityAdaptor( Internal::Adaptor::AccessibilityAdaptor* adaptor )
: BaseHandle( adaptor )
{
}

} // namespace Dali


namespace Test
{
namespace AccessibilityAdaptor
{

// Mock setup:

void MockSetReadPosition( Dali::AccessibilityAdaptor adaptor, Dali::Vector2& position )
{
  Dali::Internal::Adaptor::GetImplementation(adaptor).MockSetReadPosition( position );
}

void SetEnabled( Dali::AccessibilityAdaptor adaptor, bool enabled )
{
  Dali::Internal::Adaptor::GetImplementation(adaptor).SetEnabled(enabled);
}

void SendPanGesture( Dali::AccessibilityAdaptor adaptor, const Dali::Integration::PanGestureEvent& panEvent )
{
  Dali::Internal::Adaptor::GetImplementation(adaptor).SendPanGesture( panEvent );
}

} // namespace AccessibilityAdaptor
} // namespace Test

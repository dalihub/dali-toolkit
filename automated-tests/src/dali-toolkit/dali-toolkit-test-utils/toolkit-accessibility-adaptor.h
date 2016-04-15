// Prevent normal accessibility adaptor declaration from loading
#define __DALI_ACCESSIBILITY_ADAPTOR_H__

#ifndef __DALI_TOOLKIT_ACCESSIBILITY_ADAPTOR_H__
#define __DALI_TOOLKIT_ACCESSIBILITY_ADAPTOR_H__

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
#include <string>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/events/touch-point.h>

namespace Dali
{
namespace Internal
{
namespace Adaptor
{
class AccessibilityAdaptor;
}
}
class AccessibilityActionHandler;
class AccessibilityGestureHandler;
class TouchPoint;

/**
 * This creates a stubbed AccessibilityAdaptor so that internal Toolkit Adaptor calls work.
 */
class AccessibilityAdaptor : public BaseHandle
{
public: // Construction & Destruction
  AccessibilityAdaptor();
  ~AccessibilityAdaptor();

public: // Mock object setup.

  void MockSetReadPosition( Vector2& position );

public: // Getters

  static AccessibilityAdaptor Get();

  Vector2 GetReadPosition() const;
  bool IsEnabled() const;
  void SetActionHandler(AccessibilityActionHandler& handler);
  void SetGestureHandler(AccessibilityGestureHandler& handler);

  bool HandleActionNextEvent();
  bool HandleActionPreviousEvent();
  bool HandleActionActivateEvent();
  bool HandleActionReadEvent(unsigned int x, unsigned int y, bool allowReadAgain);
  bool HandleActionReadNextEvent();
  bool HandleActionReadPreviousEvent();
  bool HandleActionUpEvent();
  bool HandleActionDownEvent();
  bool HandleActionClearFocusEvent();
  bool HandleActionScrollEvent(TouchPoint& point, unsigned long timeStamp);
  bool HandleActionBackEvent();
  void HandleActionEnableEvent();
  void HandleActionDisableEvent();
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

  AccessibilityAdaptor( Internal::Adaptor::AccessibilityAdaptor* adaptor );
};

} // namespace Dali

#endif // __DALI_TOOLKIT_ACCESSIBILITY_ADAPTOR_H__

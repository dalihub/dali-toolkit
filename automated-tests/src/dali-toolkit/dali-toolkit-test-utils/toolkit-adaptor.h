#ifndef __DALI_TOOLKIT_TOOLKIT_ADAPTOR_H__
#define __DALI_TOOLKIT_TOOLKIT_ADAPTOR_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// EXTERNAL INCLUDES
#include <boost/function.hpp>

// INTERNAL INCLUDES
#include "toolkit-style-monitor.h"
#include "toolkit-accessibility-manager.h"
#include "toolkit-timer.h"
#include "toolkit-imf-manager.h"
#include "toolkit-clipboard-event-notifier.h"

#include <dali/public-api/events/touch-point.h>
#include <dali/public-api/math/rect.h>

namespace Dali
{

class Adaptor;

typedef Dali::Rect<int> PositionSize;

/**
 * This creates a stubbed Adaptor so that internal Toolkit Adaptor calls work.
 * Furthermore, it provides an interface to see if certain methods were invoked.
 */
class ToolkitAdaptor
{
public: // Construction & Destruction

  ToolkitAdaptor();
  ~ToolkitAdaptor();

public: // Getters

  boost::function<void(void)> GetLastIdleAdded() const
  {
    return mLastIdleAdded;
  }

  TouchPoint GetLastTouchPointFed() const
  {
    return mLastTouchPointFed;
  }

  int GetLastTimeStampFed() const
  {
    return mLastTimeStampFed;
  }

  PositionSize GetLastSizeSet() const
  {
    return mLastSizeSet;
  }

  StyleMonitor& GetToolkitStyleMonitor()
  {
    return mStyleMonitor;
  }

  AccessibilityManager& GetAccessibilityManager()
  {
    return mAccessibilityManager;
  }

  ToolkitClipboardEventNotifier& GetClipboardEventNotifier()
  {
    return mClipboardEventNotifier;
  }

public: // Signal Emissions

  void EmitSignalResize();

public: // TEST FUNCTIONS

  // Enumeration of Adaptor methods
  enum TestFuncEnum
  {
    StartType,
    PauseType,
    ResumeType,
    StopType,
    AddIdleType,
    FeedEventType,
    MoveResizeType,
    SurfaceResizedType,
    ReplaceSurfaceType,
    RenderSyncType,
    GetSurfaceType,
    GetType,
    IsAvailableType,
    RegisterSingletonType,
    GetSingletonType,
    SignalResizeType,
  };

  void Reset()
  {
    mFunctionsCalled.Reset();
  }

  bool WasCalled(TestFuncEnum func)
  {
    switch(func)
    {
      case StartType:                             return mFunctionsCalled.Start;
      case PauseType:                             return mFunctionsCalled.Pause;
      case ResumeType:                            return mFunctionsCalled.Resume;
      case StopType:                              return mFunctionsCalled.Stop;
      case AddIdleType:                           return mFunctionsCalled.AddIdle;
      case FeedEventType:                         return mFunctionsCalled.FeedEvent;
      case MoveResizeType:                        return mFunctionsCalled.MoveResize;
      case SurfaceResizedType:                    return mFunctionsCalled.SurfaceResized;
      case ReplaceSurfaceType:                    return mFunctionsCalled.ReplaceSurface;
      case RenderSyncType:                        return mFunctionsCalled.RenderSync;
      case GetSurfaceType:                        return mFunctionsCalled.GetSurface;
      case GetType:                               return mFunctionsCalled.Get;
      case IsAvailableType:                       return mFunctionsCalled.IsAvailable;
      case RegisterSingletonType:                 return mFunctionsCalled.RegisterSingleton;
      case GetSingletonType:                      return mFunctionsCalled.GetSingleton;
      case SignalResizeType:                      return mFunctionsCalled.SignalResize;
    }
    return false;
  }

  void ResetCallStatistics(TestFuncEnum func)
  {
    switch(func)
    {
      case StartType:                             mFunctionsCalled.Start = false; break;
      case PauseType:                             mFunctionsCalled.Pause = false; break;
      case ResumeType:                            mFunctionsCalled.Resume = false; break;
      case StopType:                              mFunctionsCalled.Stop = false; break;
      case AddIdleType:                           mFunctionsCalled.AddIdle = false; break;
      case FeedEventType:                         mFunctionsCalled.FeedEvent = false; break;
      case MoveResizeType:                        mFunctionsCalled.MoveResize = false; break;
      case SurfaceResizedType:                    mFunctionsCalled.SurfaceResized = false; break;
      case ReplaceSurfaceType:                    mFunctionsCalled.ReplaceSurface = false; break;
      case RenderSyncType:                        mFunctionsCalled.RenderSync = false; break;
      case GetSurfaceType:                        mFunctionsCalled.GetSurface = false; break;
      case GetType:                               mFunctionsCalled.Get = false; break;
      case IsAvailableType:                       mFunctionsCalled.IsAvailable = false; break;
      case RegisterSingletonType:                 mFunctionsCalled.RegisterSingleton = false; break;
      case GetSingletonType:                      mFunctionsCalled.GetSingleton = false; break;
      case SignalResizeType:                      mFunctionsCalled.SignalResize = false; break;
    }
  }

private:

  struct TestFunctions
  {
    TestFunctions()
    : Start(false),
      Pause(false),
      Resume(false),
      Stop(false),
      AddIdle(false),
      FeedEvent(false),
      MoveResize(false),
      SurfaceResized(false),
      ReplaceSurface(false),
      RenderSync(false),
      GetSurface(false),
      Get(false),
      IsAvailable(false),
      RegisterSingleton(false),
      GetSingleton(false),
      SignalResize(false)
    {
    }

    void Reset()
    {
      Start = false;
      Pause = false;
      Resume = false;
      Stop = false;
      AddIdle = false;
      FeedEvent = false;
      MoveResize = false;
      SurfaceResized = false;
      ReplaceSurface = false;
      RenderSync = false;
      GetSurface = false;
      Get = false;
      IsAvailable = false;
      RegisterSingleton = false;
      GetSingleton = false;
      SignalResize = false;
    }

    bool Start;
    bool Pause;
    bool Resume;
    bool Stop;
    bool AddIdle;
    bool FeedEvent;
    bool MoveResize;
    bool SurfaceResized;
    bool ReplaceSurface;
    bool RenderSync;
    bool GetSurface;
    bool Get;
    bool IsAvailable;
    bool RegisterSingleton;
    bool GetSingleton;
    bool SignalResize;
  };

  TestFunctions mFunctionsCalled;

  // Last set information
  boost::function<void(void)> mLastIdleAdded;
  TouchPoint mLastTouchPointFed;
  int mLastTimeStampFed;
  PositionSize mLastSizeSet;

  // Contains Test functions for the Style Monitor
  StyleMonitor mStyleMonitor;

  // Stub for AccessibilityManager
  AccessibilityManager mAccessibilityManager;

  // Stub for ClipboardEventNotifier
  ToolkitClipboardEventNotifier mClipboardEventNotifier;

  ImfManager mImfManager;

  // The Adaptor Stub
  Adaptor* mAdaptorStub;
  friend class Adaptor;
};

} // namespace Dali

#endif // __DALI_TOOLKIT_TOOLKIT_ADAPTOR_H__

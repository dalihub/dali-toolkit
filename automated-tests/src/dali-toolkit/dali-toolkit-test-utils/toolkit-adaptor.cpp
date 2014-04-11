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

#define __DALI_ADAPTOR_H__
#define __DALI_ACCESSIBILITY_MANAGER_H__
#define __DALI_TIMER_H__
#define __DALI_CLIPBOARD_H__
#define IMFMANAGER_H

#include "toolkit-adaptor.h"
#include <map>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/signals/dali-signal-v2.h>

namespace Dali
{

////////////////////////////////////////////////////////////////////////////////////////////////////

class TestRenderSurface : public RenderSurface
{
public:
  TestRenderSurface(){}
  virtual ~TestRenderSurface(){}
  virtual SurfaceType GetType() { return RenderSurface::WINDOW; }
  virtual Dali::Any GetSurface() { return Dali::Any(); }
  virtual Dali::Any GetDisplay() { return Dali::Any(); }
  virtual PositionSize GetPositionSize() const { return PositionSize(0, 0, 640, 480);}
  virtual void SetRenderMode(RenderMode mode){}
  virtual RenderMode GetRenderMode() const { return RenderSurface::RENDER_60FPS; }
};

typedef Dali::Rect<int> PositionSize;

/**
 * Stub for the Adaptor
 */
class Adaptor
{
public:

  typedef SignalV2< void ( Adaptor& ) > AdaptorSignalV2;

  typedef std::pair<std::string, Dali::BaseHandle> SingletonPair;
  typedef std::map<std::string, Dali::BaseHandle>  SingletonContainer;
  typedef SingletonContainer::const_iterator       SingletonConstIter;

public:

  Adaptor(ToolkitAdaptor& toolkitAdaptor);
  ~Adaptor();

public:

  void Start();
  void Pause();
  void Resume();
  void Stop();
  bool AddIdle(boost::function<void(void)> callBack);
  void FeedEvent(TouchPoint& point, int timeStamp);
  bool MoveResize(const PositionSize& positionSize);
  void SurfaceResized(const PositionSize& positionSize);
  void ReplaceSurface(RenderSurface& surface);
  void RenderSync();
  RenderSurface& GetSurface();

  void RegisterSingleton(const std::type_info& info, Dali::BaseHandle singleton);
  Dali::BaseHandle GetSingleton(const std::type_info& info) const;

public: // static methods
  static Adaptor& Get();
  static bool IsAvailable();

public:  // Signals

  AdaptorSignalV2& SignalResize();

  void EmitSignalResize()
  {
    mResizeSignal.Emit( *this );
  }

private:

  // Undefined
  Adaptor(const Adaptor&);
  Adaptor& operator=(Adaptor&);

  AdaptorSignalV2 mResizeSignal;
  TestRenderSurface mRenderSurface;
  ToolkitAdaptor& mToolkitAdaptor;

  SingletonContainer mSingletonContainer;
};

namespace
{
Adaptor* gAdaptor = NULL;

}

Adaptor::Adaptor(ToolkitAdaptor& toolkitAdaptor)
: mToolkitAdaptor(toolkitAdaptor)
{
}

Adaptor::~Adaptor()
{

}

void Adaptor::Start()
{
  mToolkitAdaptor.mFunctionsCalled.Start = true;
}

void Adaptor::Pause()
{
  mToolkitAdaptor.mFunctionsCalled.Pause = true;
}

void Adaptor::Resume()
{
  mToolkitAdaptor.mFunctionsCalled.Resume = true;
}

void Adaptor::Stop()
{
  mToolkitAdaptor.mFunctionsCalled.Stop = true;
}

bool Adaptor::AddIdle(boost::function<void(void)> callBack)
{
  mToolkitAdaptor.mFunctionsCalled.AddIdle = true;
  mToolkitAdaptor.mLastIdleAdded = callBack;
  return true;
}

void Adaptor::FeedEvent(TouchPoint& point, int timeStamp)
{
  mToolkitAdaptor.mFunctionsCalled.FeedEvent = true;
  mToolkitAdaptor.mLastTouchPointFed = point;
  mToolkitAdaptor.mLastTimeStampFed = timeStamp;
}

bool Adaptor::MoveResize(const PositionSize& positionSize)
{
  mToolkitAdaptor.mFunctionsCalled.MoveResize = true;
  mToolkitAdaptor.mLastSizeSet = positionSize;
  return true;
}

void Adaptor::SurfaceResized(const PositionSize& positionSize)
{
  mToolkitAdaptor.mFunctionsCalled.SurfaceResized = true;
  mToolkitAdaptor.mLastSizeSet = positionSize;
}

void Adaptor::ReplaceSurface(RenderSurface& surface)
{
  mToolkitAdaptor.mFunctionsCalled.ReplaceSurface = true;
}

void Adaptor::RenderSync()
{
  mToolkitAdaptor.mFunctionsCalled.RenderSync = true;
}

RenderSurface& Adaptor::GetSurface()
{
  mToolkitAdaptor.mFunctionsCalled.GetSurface = true;
  return mRenderSurface;
}

Adaptor& Adaptor::Get()
{
  DALI_ASSERT_ALWAYS(gAdaptor);
  gAdaptor->mToolkitAdaptor.mFunctionsCalled.Get = true;
  return *gAdaptor;
}

bool Adaptor::IsAvailable()
{
  bool available(false);

  if (gAdaptor)
  {
    gAdaptor->mToolkitAdaptor.mFunctionsCalled.IsAvailable = true;
    available = true;
  }

  return available;
}

void Adaptor::RegisterSingleton(const std::type_info& info, Dali::BaseHandle singleton)
{
  mToolkitAdaptor.mFunctionsCalled.RegisterSingleton = true;

  if(singleton)
  {
    mSingletonContainer.insert(SingletonPair(info.name(), singleton));
  }
}

Dali::BaseHandle Adaptor::GetSingleton(const std::type_info& info) const
{
  mToolkitAdaptor.mFunctionsCalled.GetSingleton = true;

  Dali::BaseHandle object = Dali::BaseHandle();

  SingletonConstIter iter = mSingletonContainer.find(info.name());
  if(iter != mSingletonContainer.end())
  {
    object = (*iter).second;
  }

  return object;
}

Adaptor::AdaptorSignalV2& Adaptor::SignalResize()
{
  mToolkitAdaptor.mFunctionsCalled.SignalResize = true;
  return mResizeSignal;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ToolkitAdaptor::ToolkitAdaptor()
: mLastTouchPointFed(0, TouchPoint::Down, 0.0f, 0.0f),
  mLastTimeStampFed(0),
  mStyleMonitor(StyleMonitor::Get()),
  mAccessibilityManager(AccessibilityManager::Get()),
  mImfManager(ImfManager::Get()),
  mAdaptorStub(new Adaptor(*this))
{
  gAdaptor = mAdaptorStub;
}

ToolkitAdaptor::~ToolkitAdaptor()
{
  delete mAdaptorStub;
  gAdaptor = NULL;
}

void ToolkitAdaptor::EmitSignalResize()
{
  mAdaptorStub->EmitSignalResize();
}

} // namespace Dali

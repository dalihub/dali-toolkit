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

// CLASS HEADER
#include <dali/integration-api/adaptors/adaptor.h>

#include <dali/public-api/object/base-object.h>
#include <dali/devel-api/adaptor-framework/render-surface.h>

namespace Dali
{

class EglInterface;
class DisplayConnection;
class ThreadSynchronizationInterface;

namespace Integration
{

class GlAbstraction;

} // namespace Integration

class TestRenderSurface : public RenderSurface
{
public:
  virtual PositionSize GetPositionSize() const { PositionSize size; return size; }

  virtual void InitializeEgl( EglInterface& egl ) {}

  virtual void CreateEglSurface( EglInterface& egl ) {}

  virtual void DestroyEglSurface( EglInterface& egl ) {}

  virtual bool ReplaceEGLSurface( EglInterface& egl ) { return false; }

  virtual void MoveResize( Dali::PositionSize positionSize ) {}

  virtual void SetViewMode( ViewMode viewMode ) {}

  virtual void StartRender() {}

  virtual bool PreRender( EglInterface& egl, Integration::GlAbstraction& glAbstraction ) { return false; }

  virtual void PostRender( EglInterface& egl, Integration::GlAbstraction& glAbstraction, DisplayConnection* displayConnection, bool replacingSurface ) {}

  virtual void StopRender() {}

  virtual void ReleaseLock() {}

  virtual void SetThreadSynchronization( ThreadSynchronizationInterface& threadSynchronization ) {}

};

namespace Internal
{
namespace Adaptor
{

class Adaptor: public BaseObject
{
public:
  static Dali::Adaptor& Get();
  Adaptor();
  ~Adaptor();

public:
  static Dali::RenderSurface& GetSurface();
  static Dali::Adaptor::AdaptorSignalType& AdaptorSignal();
};

Dali::Adaptor& Adaptor::Get()
{
  Dali::Adaptor* adaptor = new Dali::Adaptor;
  return *adaptor;
}

Dali::RenderSurface& Adaptor::GetSurface()
{
  Dali::RenderSurface *renderSurface = new Dali::TestRenderSurface;
  return *renderSurface;
}

Dali::Adaptor::AdaptorSignalType& Adaptor::AdaptorSignal()
{
  Dali::Adaptor::AdaptorSignalType* signal = new Dali::Adaptor::AdaptorSignalType;
  return *signal;
}

}
}
}

namespace Dali
{

Adaptor& Adaptor::New( Window window )
{
  return Internal::Adaptor::Adaptor::Get();
}

Adaptor& Adaptor::New( Window window, Configuration::ContextLoss configuration )
{
  return Internal::Adaptor::Adaptor::Get();
}

Adaptor& Adaptor::New( Any nativeWindow, const Dali::RenderSurface& surface )
{
  return Internal::Adaptor::Adaptor::Get();
}

Adaptor& Adaptor::New( Any nativeWindow, const Dali::RenderSurface& surface, Configuration::ContextLoss configuration )
{
  return Internal::Adaptor::Adaptor::Get();
}

Adaptor::~Adaptor()
{
}

void Adaptor::Start()
{
}

void Adaptor::Pause()
{
}

void Adaptor::Resume()
{
}

void Adaptor::Stop()
{
}

bool Adaptor::AddIdle( CallbackBase* callback )
{
  return false;
}

void Adaptor::ReplaceSurface( Any nativeWindow, Dali::RenderSurface& surface )
{
}

Adaptor::AdaptorSignalType& Adaptor::ResizedSignal()
{
  return Internal::Adaptor::Adaptor::AdaptorSignal();
}

Adaptor::AdaptorSignalType& Adaptor::LanguageChangedSignal()
{
  return Internal::Adaptor::Adaptor::AdaptorSignal();
}

RenderSurface& Adaptor::GetSurface()
{
  return Internal::Adaptor::Adaptor::GetSurface();
}

Any Adaptor::GetNativeWindowHandle()
{
  Any window;
  return window;
}

void Adaptor::ReleaseSurfaceLock()
{
}

void Adaptor::SetRenderRefreshRate( unsigned int numberOfVSyncsPerRender )
{
}

void Adaptor::SetUseHardwareVSync(bool useHardware)
{
}

Adaptor& Adaptor::Get()
{
  return Internal::Adaptor::Adaptor::Get();
}

bool Adaptor::IsAvailable()
{
  return false;
}

void Adaptor::NotifySceneCreated()
{
}

void Adaptor::NotifyLanguageChanged()
{
}

void Adaptor::SetMinimumPinchDistance(float distance)
{
}

void Adaptor::FeedTouchPoint( TouchPoint& point, int timeStamp )
{
}

void Adaptor::FeedWheelEvent( WheelEvent& wheelEvent )
{
}

void Adaptor::FeedKeyEvent( KeyEvent& keyEvent )
{
}

void Adaptor::SceneCreated()
{
}

void Adaptor::SetViewMode( ViewMode mode )
{
}

void Adaptor::SetStereoBase(  float stereoBase )
{
}

Adaptor::Adaptor()
: mImpl( NULL )
{
}

} // namespace Dali

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include <toolkit-adaptor-impl.h>

namespace Dali
{

namespace Internal
{
namespace Adaptor
{

bool Adaptor::mAvailable = false;
Vector<CallbackBase*> Adaptor::mCallbacks = Vector<CallbackBase*>();

Dali::Adaptor& Adaptor::Get()
{
  Dali::Adaptor* adaptor = new Dali::Adaptor;
  Adaptor::mAvailable = true;
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

} // namespace Adaptor
} // namespace Internal

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
  const bool isAvailable = IsAvailable();

  if( isAvailable )
  {
    Internal::Adaptor::Adaptor::mCallbacks.PushBack( callback );
  }

  return isAvailable;
}

void Adaptor::RemoveIdle( CallbackBase* callback )
{
  const bool isAvailable = IsAvailable();

  if( isAvailable )
  {
    for( Vector<CallbackBase*>::Iterator it = Internal::Adaptor::Adaptor::mCallbacks.Begin(),
           endIt = Internal::Adaptor::Adaptor::mCallbacks.End();
         it != endIt;
         ++it )
    {
      if( callback == *it )
      {
        Internal::Adaptor::Adaptor::mCallbacks.Remove( it );
        return;
      }
    }
  }
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
  return Internal::Adaptor::Adaptor::mAvailable;
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

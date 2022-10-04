/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include "toolkit-window-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/object/base-object.h>

#define DALI_WINDOW_H
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <toolkit-adaptor-impl.h>

using AdaptorImpl = Dali::Internal::Adaptor::Adaptor;

namespace Dali
{

class Window;

/********************************************************************************
 * Stub for Dali::Internal::Adaptor::Window
 ********************************************************************************/

namespace Internal
{
namespace Adaptor
{

Window::Window( const PositionSize& positionSize )
: SceneHolder( positionSize ),
  mFocusChangeSignal(),
  mResizeSignal(),
  mRotationAngle(90), // dummy angle for test coverage
  mVisible(true),
  mVisibilityChangedSignal()
{
}

Window* Window::New(const PositionSize& positionSize, const std::string& name, const std::string& className, bool isTransparent)
{
  return new Window( positionSize );
}

Dali::Window::WindowPosition  Window::GetPosition() const
{
  PositionSize positionSize = mRenderSurface.GetPositionSize();

  return Dali::Window::WindowPosition(positionSize.x, positionSize.y);
}

PositionSize Window::GetPositionSize() const
{
  return mRenderSurface.GetPositionSize();
}

Dali::Window::WindowSize Window::GetSize() const
{
  PositionSize positionSize = mRenderSurface.GetPositionSize();

  return Dali::Window::WindowSize(positionSize.width, positionSize.height);
}

void Window::SetPositionSize(PositionSize positionSize)
{
  mRenderSurface.MoveResize(positionSize);

  Uint16Pair newSize(positionSize.width, positionSize.height);
  Dali::Window handle(this);
  mResizeSignal.Emit(handle, newSize);
}

} // Adaptor
} // Internal

inline Internal::Adaptor::Window& GetImplementation(Dali::Window& window)
{
  DALI_ASSERT_ALWAYS( window && "Window handle is empty" );
  BaseObject& object = window.GetBaseObject();
  return static_cast<Internal::Adaptor::Window&>(object);
}

inline const Internal::Adaptor::Window& GetImplementation(const Dali::Window& window)
{
  DALI_ASSERT_ALWAYS( window && "Window handle is empty" );
  const BaseObject& object = window.GetBaseObject();
  return static_cast<const Internal::Adaptor::Window&>(object);
}

Window::Window()
{
}

Window::~Window()
{
}

Window::Window( const Window& copy ) = default;

Window& Window::operator=( const Window& rhs ) = default;

Window::Window( Window&& rhs ) = default;

Window& Window::operator=( Window&& rhs ) = default;

Dali::Window Window::New( PositionSize windowPosition, const std::string& name, bool isTransparent )
{
  return New( windowPosition, name, "", isTransparent );
}

Dali::Window Window::New(PositionSize windowPosition, const std::string& name, const std::string& className, bool isTransparent )
{
  Internal::Adaptor::Window* window = Internal::Adaptor::Window::New( windowPosition, name, className, isTransparent );

  Dali::Window result( window );

  // This will also emit the window created signals
  AdaptorImpl::GetImpl( AdaptorImpl::Get() ).AddWindow( window );

  return result;
}

Window::Window( Internal::Adaptor::Window* window )
: BaseHandle( window )
{
}

Integration::Scene Window::GetScene()
{
  return GetImplementation( *this ).GetScene();
}

Dali::RenderSurfaceInterface& Window::GetRenderSurface()
{
  return GetImplementation( *this ).GetRenderSurface();
}

void Window::Add( Actor actor )
{
  GetImplementation( *this ).Add( actor );
}

void Window::Remove( Actor actor )
{
  GetImplementation( *this ).Remove( actor );
}

Dali::Layer Window::GetRootLayer() const
{
  return GetImplementation( *this ).GetRootLayer();
}

void Window::SetBackgroundColor( const Vector4& color )
{
  GetImplementation( *this ).SetBackgroundColor( color );
}

Vector4 Window::GetBackgroundColor() const
{
  return GetImplementation( *this ).GetBackgroundColor();
}

void Window::Raise()
{
  GetImplementation( *this ).mFocusChangeSignal.Emit(*this, true);
}

void Window::Hide()
{
  GetImplementation( *this ).mVisibilityChangedSignal.Emit( *this, false );
  GetImplementation( *this ).mVisible = false;
}

bool Window::IsVisible() const
{
  return GetImplementation( *this ).mVisible;
}

FocusChangeSignalType& Window::FocusChangeSignal()
{
  return GetImplementation( *this ).mFocusChangeSignal;
}

ResizeSignalType& Window::ResizeSignal()
{
  return GetImplementation( *this ).mResizeSignal;
}

Window::KeyEventSignalType& Window::KeyEventSignal()
{
  return GetImplementation( *this ).KeyEventSignal();
}

Window::TouchEventSignalType& Window::TouchedSignal()
{
  return GetImplementation( *this ).TouchedSignal();
}

namespace DevelWindow
{

Window Get( Actor actor )
{
  Internal::Adaptor::Window* windowImpl = nullptr;

  if ( Dali::Adaptor::IsAvailable() )
  {
    windowImpl = static_cast<Internal::Adaptor::Window*>( AdaptorImpl::GetImpl( AdaptorImpl::Get() ).GetWindow( actor ) );
  }

  return Dali::Window( windowImpl );
}

Window DownCast( BaseHandle handle )
{
  Internal::Adaptor::Window* windowImpl = nullptr;
  if ( Dali::Adaptor::IsAvailable() )
  {
    windowImpl = dynamic_cast<Dali::Internal::Adaptor::Window*>( handle.GetObjectPtr());
  }
  return Dali::Window( windowImpl );
}

void SetPositionSize(Window window, PositionSize positionSize)
{
  GetImplementation( window ).SetPositionSize(positionSize);
}

int GetPhysicalOrientation(Window window)
{
  return GetImplementation( window ).mRotationAngle;
}

void AddFrameRenderedCallback( Window window, std::unique_ptr< CallbackBase > callback, int32_t frameId )
{
  CallbackBase::Execute( *callback, frameId );
}

void AddFramePresentedCallback( Window window, std::unique_ptr< CallbackBase > callback, int32_t frameId )
{
  CallbackBase::Execute( *callback, frameId );
}

EventProcessingFinishedSignalType& EventProcessingFinishedSignal( Window window )
{
  return GetImplementation( window ).GetScene().EventProcessingFinishedSignal();
}

KeyEventGeneratedSignalType& KeyEventGeneratedSignal( Window window )
{
  return GetImplementation( window ).KeyEventGeneratedSignal();
}

WheelEventSignalType& WheelEventSignal( Window window )
{
  return GetImplementation( window ).WheelEventSignal();
}

WheelEventGeneratedSignalType& WheelEventGeneratedSignal( Window window )
{
  return GetImplementation( window ).WheelEventGeneratedSignal();
}

VisibilityChangedSignalType& VisibilityChangedSignal( Window window )
{
  return GetImplementation( window ).mVisibilityChangedSignal;
}

} // namespace DevelWindow

} // Dali

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include "toolkit-window.h"

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/object/base-object.h>

#include <dali/integration-api/adaptors/scene-holder.h>
#include <toolkit-scene-holder-impl.h>

#define DALI_WINDOW_H
#include <dali/integration-api/adaptors/adaptor.h>
#include <toolkit-adaptor-impl.h>

// INTERNAL INCLUDES
#include "test-render-surface.h"

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
class Window : public SceneHolder
{
public:

  Window( const PositionSize& positionSize )
  : SceneHolder( positionSize )
  {
  }

  virtual ~Window() = default;

  static Window* New(const PositionSize& positionSize, const std::string& name, const std::string& className, bool isTransparent)
  {
    return new Window( positionSize );
  }
};

} // Adaptor
} // Internal

inline Internal::Adaptor::Window& GetImplementation(Dali::Window& window)
{
  DALI_ASSERT_ALWAYS( window && "Window handle is empty" );
  BaseObject& object = window.GetBaseObject();
  return static_cast<Internal::Adaptor::Window&>(object);
}

Window::Window()
{
}

Window::~Window()
{
}

Window::Window(const Window& handle)
: BaseHandle( handle )
{
}

Window& Window::operator=(const Window& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

Dali::Window Window::New( PositionSize windowPosition, const std::string& name, bool isTransparent )
{
  return New( windowPosition, name, "", isTransparent );
}

Dali::Window Window::New(PositionSize windowPosition, const std::string& name, const std::string& className, bool isTransparent )
{
  Internal::Adaptor::Window* window = Internal::Adaptor::Window::New( windowPosition, name, className, isTransparent );

  Dali::Window result( window );

  Dali::Integration::SceneHolder sceneHolder( static_cast<Dali::Internal::Adaptor::SceneHolder*>( window ) );
  Dali::Internal::Adaptor::Adaptor::Get().WindowCreatedSignal().Emit( sceneHolder );

  return result;
}

Dali::Layer Window::GetRootLayer() const
{
  return Dali::Stage::GetCurrent().GetRootLayer();
}

Window::Window( Internal::Adaptor::Window* window )
: BaseHandle( window )
{
}

Integration::Scene Window::GetScene()
{
  return GetImplementation( *this ).GetScene();
}

Integration::RenderSurface& Window::GetRenderSurface()
{
  return GetImplementation( *this ).GetRenderSurface();
}

namespace DevelWindow
{

Window Get( Actor actor )
{
  return Window();
}

EventProcessingFinishedSignalType& EventProcessingFinishedSignal( Window window )
{
  return GetImplementation( window ).GetScene().EventProcessingFinishedSignal();
}

KeyEventSignalType& KeyEventSignal( Window window )
{
  return GetImplementation( window ).KeyEventSignal();
}

KeyEventGeneratedSignalType& KeyEventGeneratedSignal( Window window )
{
  return GetImplementation( window ).KeyEventGeneratedSignal();
}

TouchSignalType& TouchSignal( Window window )
{
  return GetImplementation( window ).TouchSignal();
}

WheelEventSignalType& WheelEventSignal( Window window )
{
  return GetImplementation( window ).WheelEventSignal();
}
} // namespace DevelWindow

} // Dali

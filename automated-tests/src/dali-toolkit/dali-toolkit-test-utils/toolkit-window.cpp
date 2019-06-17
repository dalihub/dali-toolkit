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

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include "toolkit-window.h"

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

class Window : public Dali::BaseObject
{
public:

  Window()
  {
  }

  virtual ~Window()
  {
  }

  static Window* New(const PositionSize& positionSize, const std::string& name, const std::string& className, bool isTransparent)
  {
    return new Window();
  }

  static Dali::Window Get( Dali::Actor actor )
  {
    return Dali::Window();
  }
};

} // Adaptor
} // Internal

Window::Window()
{
}

Window::~Window()
{
}

Dali::Window Window::New( PositionSize windowPosition, const std::string& name, bool isTransparent )
{
  Internal::Adaptor::Window* window = Internal::Adaptor::Window::New( windowPosition, name, "", isTransparent );
  return Window( window );
}

Dali::Window Window::New(PositionSize windowPosition, const std::string& name, const std::string& className, bool isTransparent )
{
  Internal::Adaptor::Window* window = Internal::Adaptor::Window::New( windowPosition, name, className, isTransparent );
  return Window( window );
}

Dali::Layer Window::GetRootLayer() const
{
  return Dali::Stage::GetCurrent().GetRootLayer();
}

Window::Window( Internal::Adaptor::Window* window )
: BaseHandle( window )
{
}

namespace DevelWindow
{

Window Get( Actor actor )
{
  return Internal::Adaptor::Window::Get( actor );
}

KeyEventSignalType& KeyEventSignal( Window window )
{
  return Dali::Stage::GetCurrent().KeyEventSignal();
}

TouchSignalType& TouchSignal( Window window )
{
  return Dali::Stage::GetCurrent().TouchSignal();
}

} // namespace DevelWindow

} // Dali

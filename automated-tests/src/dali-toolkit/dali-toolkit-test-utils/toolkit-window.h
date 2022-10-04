#ifndef TOOLKIT_WINDOW_H
#define TOOLKIT_WINDOW_H

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

//EXTERNAL INCLUDES
#include <string>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal.h>
#include <dali/integration-api/scene.h>

namespace Dali
{

class Actor;
class Layer;
class RenderSurfaceInterface;
class KeyEvent;
class TouchEvent;
class WheelEvent;

typedef Dali::Rect<int> PositionSize;

namespace Internal
{
namespace Adaptor
{
class Window;
}
}

class Window;
typedef Signal< void (Window,bool) > FocusChangeSignalType;
typedef Signal< void (Window,Uint16Pair) > ResizeSignalType;

class Window : public BaseHandle
{
public:
    class IntPair
    {
    public:
      IntPair()
      : mX(0),
        mY(0)
      {
      };

      IntPair(int x, int y)
      {
        mX = x;
        mY = y;
      }

      // Default operation
    public:
      IntPair(const IntPair&) = default;            ///< Default copy constructor
      IntPair(IntPair&&)      = default;            ///< Default move constructor
      IntPair& operator=(const IntPair&) = default; ///< Default copy assignment operator
      IntPair& operator=(IntPair&&) = default;      ///< Default move assignment operator

      // member data
    private:
      int mX;
      int mY;
    };

public:
  using WindowSize     = Uint16Pair;
  using WindowPosition = Dali::Window::IntPair;

  using KeyEventSignalType = Signal< void (const KeyEvent&) >;
  using TouchEventSignalType = Signal< void (const TouchEvent&) >;
  using ResizeSignalType      = Signal<void(Window, WindowSize)>;

  static Window New(PositionSize windowPosition, const std::string& name, bool isTransparent = false);
  static Window New(PositionSize windowPosition, const std::string& name, const std::string& className, bool isTransparent = false);

  Window();
  ~Window();
  Window(const Window& handle);
  Window& operator=(const Window& rhs);
  Window( Window&& rhs );
  Window& operator=( Window&& rhs );

  Integration::Scene GetScene();
  Dali::RenderSurfaceInterface& GetRenderSurface();
  void Add( Dali::Actor actor );
  void Remove( Dali::Actor actor );
  Dali::Layer GetRootLayer() const;
  void SetBackgroundColor( const Vector4& color );
  Vector4 GetBackgroundColor() const;
  void Raise();
  void Hide();
  bool IsVisible() const;
  FocusChangeSignalType& FocusChangeSignal();
  KeyEventSignalType& KeyEventSignal();
  TouchEventSignalType& TouchedSignal();
  ResizeSignalType& ResizeSignal();

public:
  explicit Window( Internal::Adaptor::Window* window );
};

Internal::Adaptor::Window& GetImplementation(Dali::Window& window);
const Internal::Adaptor::Window& GetImplementation(const Dali::Window& window);

namespace DevelWindow
{
typedef Signal< void () > EventProcessingFinishedSignalType;
typedef Signal< bool (const KeyEvent&) > KeyEventGeneratedSignalType;
typedef Signal< void (const WheelEvent&) > WheelEventSignalType;
typedef Signal< bool (const WheelEvent&) > WheelEventGeneratedSignalType;
typedef Signal< void ( Window, bool ) > VisibilityChangedSignalType;

Dali::Window Get( Actor actor );
Dali::Window DownCast(  BaseHandle handle );
void SetPositionSize(Window window, PositionSize positionSize);
int GetPhysicalOrientation(Window window);
void AddFrameRenderedCallback( Window window, std::unique_ptr< CallbackBase > callback, int32_t frameId );
void AddFramePresentedCallback( Window window, std::unique_ptr< CallbackBase > callback, int32_t frameId );

EventProcessingFinishedSignalType& EventProcessingFinishedSignal( Window window );
KeyEventGeneratedSignalType& KeyEventGeneratedSignal( Dali::Window window );
WheelEventSignalType& WheelEventSignal( Window window );
WheelEventGeneratedSignalType& WheelEventGeneratedSignal( Dali::Window window );
VisibilityChangedSignalType& VisibilityChangedSignal( Window window );
}

} // namespace Dali

#endif // TOOLKIT_WINDOW_H

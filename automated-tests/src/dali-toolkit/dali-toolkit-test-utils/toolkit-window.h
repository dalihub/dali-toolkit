#ifndef TOOLKIT_WINDOW_H
#define TOOLKIT_WINDOW_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/scene.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/math/int-pair.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
class Actor;
class Layer;
class RenderSurfaceInterface;
class KeyEvent;
class TouchEvent;
class WheelEvent;

typedef Dali::BoundsInteger PositionSize;

namespace Internal
{
namespace Adaptor
{
class Window;
}
} // namespace Internal

class Window;
typedef Signal<void(Window, bool)>      FocusChangedSignalType;
typedef Signal<void(Window, Int32Pair)> ResizedSignalType;

class Window : public BaseHandle
{
public:
  using WindowSize     = Int32Pair;
  using WindowPosition = Int32Pair;

  using KeyEventSignalType          = Signal<void(Window, KeyEvent)>;
  using TouchEventSignalType        = Signal<void(Window, TouchEvent)>;
  using WheelEventSignalType        = Signal<void(Window, WheelEvent)>;
  using ResizedSignalType           = Signal<void(Window, WindowSize)>;
  using VisibilityChangedSignalType = Signal<void(Window, bool)>;

  static Window New(PositionSize windowPosition, const Dali::String& name, bool isTransparent = false);
  static Window New(PositionSize windowPosition, const Dali::String& name, const Dali::String& className, bool isTransparent = false);

  Window();
  ~Window();
  Window(const Window& handle);
  Window& operator=(const Window& rhs);
  Window(Window&& rhs);
  Window&       operator=(Window&& rhs);
  static Window DownCast(BaseHandle handle);
  static Window Get(Actor actor);

  void         SetPositionSize(PositionSize positionSize);
  PositionSize GetPositionSize() const;

  Dali::Integration::Scene                   GetScene();
  Dali::Integration::RenderSurfaceInterface& GetRenderSurface();
  void                                       Add(Dali::Actor actor);
  void                                       Remove(Dali::Actor actor);
  Dali::Layer                                GetRootLayer() const;
  void                                       SetBackgroundColor(const Vector4& color);
  Vector4                                    GetBackgroundColor() const;
  void                                       Raise();
  void                                       Lower();
  void                                       Show();
  void                                       Hide();
  bool                                       IsVisible() const;
  FocusChangedSignalType&                    FocusChangedSignal();
  ResizedSignalType&                         ResizedSignal();
  KeyEventSignalType&                        KeyEventSignal();
  TouchEventSignalType&                      TouchEventSignal();
  WheelEventSignalType&                      WheelEventSignal();
  VisibilityChangedSignalType&               VisibilityChangedSignal();

  Dali::RenderTaskList GetRenderTaskList();
  void                 KeepRendering(float durationSeconds);

public:
  explicit Window(Internal::Adaptor::Window* window);
};

Internal::Adaptor::Window&       GetImplementation(Dali::Window& window);
const Internal::Adaptor::Window& GetImplementation(const Dali::Window& window);

namespace DevelWindow
{
typedef Signal<void()>                   EventProcessingFinishedSignalType;
typedef Signal<bool(Window, KeyEvent)>   KeyEventGeneratedSignalType;
typedef Signal<bool(Window, WheelEvent)> WheelEventGeneratedSignalType;

int GetPhysicalOrientation(Window window);

EventProcessingFinishedSignalType& EventProcessingFinishedSignal(Window window);
KeyEventGeneratedSignalType&       KeyEventGeneratedSignal(Dali::Window window);
WheelEventGeneratedSignalType&     WheelEventGeneratedSignal(Dali::Window window);
} // namespace DevelWindow

} // namespace Dali

#endif // TOOLKIT_WINDOW_H

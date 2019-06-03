#ifndef TOOLKIT_WINDOW_H
#define TOOLKIT_WINDOW_H

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

//EXTERNAL INCLUDES
#include <string>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{

class Actor;
class Layer;
class KeyEvent;
class TouchData;

typedef Dali::Rect<int> PositionSize;

namespace Internal
{
namespace Adaptor
{
class Window;
}
}

class Window : public BaseHandle
{
public:

  static Window New(PositionSize windowPosition, const std::string& name, bool isTransparent = false);
  static Window New(PositionSize windowPosition, const std::string& name, const std::string& className, bool isTransparent = false);
  Window();
  ~Window();
  Window(const Window& handle);
  Window& operator=(const Window& rhs);
  Layer GetRootLayer() const;

public:
  explicit Window( Internal::Adaptor::Window* window );
};

namespace DevelWindow
{

typedef Signal< void (const KeyEvent&) > KeyEventSignalType;
typedef Signal< void (const TouchData&) > TouchSignalType;

Dali::Window Get( Actor actor );
KeyEventSignalType& KeyEventSignal( Dali::Window window );
TouchSignalType& TouchSignal( Dali::Window window );

}

} // namespace Dali

#endif // TOOLKIT_WINDOW_H

#ifndef TOOLKIT_WINDOW_IMPL_H
#define TOOLKIT_WINDOW_IMPL_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <string>

// INTERNAL INCLUDES
#include <toolkit-scene-holder-impl.h>
#include <toolkit-window.h>

namespace Dali
{
using PositionSize = Dali::Rect<int>;

namespace Internal
{
namespace Adaptor
{
class Window : public SceneHolder
{
public:
  Window(const PositionSize& positionSize);
  virtual ~Window() = default;
  static Window* New(const PositionSize& positionSize, const std::string& name, const std::string& className, bool isTransparent);

  Dali::Window::WindowPosition GetPosition() const;
  PositionSize                 GetPositionSize() const;
  Dali::Window::WindowSize     GetSize() const;
  std::string                  GetNativeResourceId() const;

  void SetPositionSize(PositionSize positionSize);

  FocusChangeSignalType                    mFocusChangeSignal;
  ResizeSignalType                         mResizeSignal;
  int                                      mRotationAngle;
  bool                                     mVisible;
  DevelWindow::VisibilityChangedSignalType mVisibilityChangedSignal;
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // TOOLKIT_WINDOW_IMPL_H

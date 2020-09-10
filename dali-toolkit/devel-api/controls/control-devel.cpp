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
#include "control-devel.h"

// EXTERNAL INCLUDES
#include <dali/public-api/animation/animation.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visual-factory/transition-data.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/public-api/controls/control-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace DevelControl
{
void RegisterVisual(Internal::Control& control, Dali::Property::Index index, Toolkit::Visual::Base& visual)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  controlDataImpl.RegisterVisual(index, visual);
}

void RegisterVisual(Internal::Control& control, Dali::Property::Index index, Toolkit::Visual::Base& visual, int depthIndex)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  controlDataImpl.RegisterVisual(index, visual, depthIndex);
}

void RegisterVisual(Internal::Control& control, Dali::Property::Index index, Toolkit::Visual::Base& visual, bool enabled)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  controlDataImpl.RegisterVisual(index, visual, enabled);
}

void RegisterVisual(Internal::Control& control, Dali::Property::Index index, Toolkit::Visual::Base& visual, bool enabled, int depthIndex)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  controlDataImpl.RegisterVisual(index, visual, enabled, depthIndex);
}

void UnregisterVisual(Internal::Control& control, Dali::Property::Index index)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  controlDataImpl.UnregisterVisual(index);
}

Toolkit::Visual::Base GetVisual(const Internal::Control& control, Dali::Property::Index index)
{
  const Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  return controlDataImpl.GetVisual(index);
}

void EnableVisual(Internal::Control& control, Dali::Property::Index index, bool enable)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  controlDataImpl.EnableVisual(index, enable);
}

bool IsVisualEnabled(const Internal::Control& control, Dali::Property::Index index)
{
  const Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  return controlDataImpl.IsVisualEnabled(index);
}

Dali::Animation CreateTransition(Internal::Control& control, const Toolkit::TransitionData& handle)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  return controlDataImpl.CreateTransition(handle);
}

void AddTransitions(Internal::Control&             control,
                    Dali::Animation                animation,
                    const Toolkit::TransitionData& transitionData)
{
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(control);
  controlDataImpl.AddTransitions(animation, transitionData);
}

void DoAction(Control& control, Dali::Property::Index visualIndex, Dali::Property::Index actionId, const Dali::Property::Value attributes)
{
  Internal::Control&       controlInternal = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(controlInternal);
  controlDataImpl.DoAction(visualIndex, actionId, attributes);
}

void SetInputMethodContext(Internal::Control& control, InputMethodContext& inputMethodContext)
{
  Internal::Control::Impl::Get(control).SetInputMethodContext(inputMethodContext);
}

VisualEventSignalType& VisualEventSignal(Control control)
{
  Internal::Control&       internalControl = Toolkit::Internal::GetImplementation(control);
  Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(internalControl);
  return controlDataImpl.VisualEventSignal();
}

} // namespace DevelControl

} // namespace Toolkit

} // namespace Dali

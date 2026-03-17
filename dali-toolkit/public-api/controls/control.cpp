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

// CLASS HEADER
#include <dali-toolkit/public-api/controls/control.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/control/control-internal.h>
#include <dali-toolkit/public-api/controls/control-impl.h>

using Dali::Integration::ToDaliString;
using Dali::Integration::ToStdString;

namespace Dali
{
namespace Toolkit
{
Control Control::New()
{
  return ControlImpl::New();
}

Control Control::New(ControlBehaviour additionalBehaviour)
{
  return ControlImpl::New(static_cast<ControlImpl::ControlBehaviour>(additionalBehaviour));
}

Control::Control()
{
}

Control::Control(const Control& uiControl) = default;

Control::Control(Control&& rhs) noexcept = default;

Control::~Control()
{
}

Control& Control::operator=(const Control& handle) = default;

Control& Control::operator=(Control&& rhs) noexcept = default;

Control Control::DownCast(BaseHandle handle)
{
  return DownCast<Control, ControlImpl>(handle);
}

void Control::SetKeyInputFocus()
{
  Toolkit::GetImplementation(*this).SetKeyInputFocus();
}

bool Control::HasKeyInputFocus()
{
  return Toolkit::GetImplementation(*this).HasKeyInputFocus();
}

void Control::ClearKeyInputFocus()
{
  Toolkit::GetImplementation(*this).ClearKeyInputFocus();
}

PinchGestureDetector Control::GetPinchGestureDetector() const
{
  return Toolkit::GetImplementation(*this).GetPinchGestureDetector();
}

PanGestureDetector Control::GetPanGestureDetector() const
{
  return Toolkit::GetImplementation(*this).GetPanGestureDetector();
}

TapGestureDetector Control::GetTapGestureDetector() const
{
  return Toolkit::GetImplementation(*this).GetTapGestureDetector();
}

LongPressGestureDetector Control::GetLongPressGestureDetector() const
{
  return Toolkit::GetImplementation(*this).GetLongPressGestureDetector();
}

void Control::SetStyleName(const Dali::String& styleName)
{
  Toolkit::GetImplementation(*this).SetStyleName(styleName);
}

const Dali::String& Control::GetStyleName() const
{
  return Toolkit::GetImplementation(*this).GetStyleName();
}

void Control::SetBackgroundColor(const Vector4& color)
{
  Toolkit::GetImplementation(*this).SetBackgroundColor(color);
}

void Control::ClearBackground()
{
  Toolkit::GetImplementation(*this).ClearBackground();
}

void Control::SetRenderEffect(Toolkit::RenderEffect effect)
{
  Toolkit::GetImplementation(*this).SetRenderEffect(effect);
}

Toolkit::RenderEffect Control::GetRenderEffect() const
{
  return Toolkit::GetImplementation(*this).GetRenderEffect();
}

void Control::ClearRenderEffect()
{
  Toolkit::GetImplementation(*this).ClearRenderEffect();
}

bool Control::IsResourceReady() const
{
  return Toolkit::GetImplementation(*this).IsResourceReady();
}

Toolkit::Visual::ResourceStatus Control::GetVisualResourceStatus(Dali::Property::Index index)
{
  const ControlImpl&       controlImpl     = Toolkit::GetImplementation(*this);
  const Internal::Control& internalControl = Internal::Control::Get(controlImpl);
  return internalControl.GetVisualResourceStatus(index);
}

Control::KeyEventSignalType& Control::KeyEventSignal()
{
  return Toolkit::GetImplementation(*this).KeyEventSignal();
}

Control::KeyInputFocusSignalType& Control::KeyInputFocusGainedSignal()
{
  return Toolkit::GetImplementation(*this).KeyInputFocusGainedSignal();
}

Control::KeyInputFocusSignalType& Control::KeyInputFocusLostSignal()
{
  return Toolkit::GetImplementation(*this).KeyInputFocusLostSignal();
}

Control::ResourceReadySignalType& Control::ResourceReadySignal()
{
  ControlImpl&       controlImpl     = Toolkit::GetImplementation(*this);
  Internal::Control& internalControl = Internal::Control::Get(controlImpl);

  return internalControl.mResourceReadySignal;
}

Control::OffScreenRenderingFinishedSignalType& Control::OffScreenRenderingFinishedSignal()
{
  ControlImpl&       controlImpl     = Toolkit::GetImplementation(*this);
  Internal::Control& internalControl = Internal::Control::Get(controlImpl);

  return internalControl.mOffScreenRenderingFinishedSignal;
}

Control::Control(ControlImpl& implementation)
: CustomActor(implementation)
{
}

Control::Control(Dali::Internal::CustomActor* internal)
: CustomActor(internal)
{
  VerifyCustomActorPointer<ControlImpl>(internal);
}

} // namespace Toolkit

} // namespace Dali

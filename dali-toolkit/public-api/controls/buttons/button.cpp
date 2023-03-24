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
#include <dali-toolkit/public-api/controls/buttons/button.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/buttons/button-impl.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/public-api/visuals/text-visual-properties.h>

namespace Dali
{
namespace Toolkit
{
Button::Button()
{
}

Button::Button(const Button& button) = default;

Button::Button(Button&& rhs) noexcept = default;

Button& Button::operator=(const Button& button) = default;

Button& Button::operator=(Button&& rhs) noexcept = default;

Button::~Button()
{
}

Button Button::DownCast(BaseHandle handle)
{
  return Control::DownCast<Button, Internal::Button>(handle);
}

Button::ButtonSignalType& Button::PressedSignal()
{
  return Dali::Toolkit::GetImplementation(*this).PressedSignal();
}

Button::ButtonSignalType& Button::ReleasedSignal()
{
  return Dali::Toolkit::GetImplementation(*this).ReleasedSignal();
}

Button::ButtonSignalType& Button::ClickedSignal()
{
  return Dali::Toolkit::GetImplementation(*this).ClickedSignal();
}

Button::ButtonSignalType& Button::StateChangedSignal()
{
  return Dali::Toolkit::GetImplementation(*this).StateChangedSignal();
}

Button::Button(Internal::Button& implementation)
: Control(implementation)
{
}

Button::Button(Dali::Internal::CustomActor* internal)
: Control(internal)
{
  VerifyCustomActorPointer<Internal::Button>(internal);
}

} // namespace Toolkit

} // namespace Dali

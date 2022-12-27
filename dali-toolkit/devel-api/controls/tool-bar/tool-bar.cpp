/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

#include "tool-bar.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES

#include <dali-toolkit/internal/controls/tool-bar/tool-bar-impl.h>
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Toolkit
{
const Toolkit::Alignment::Padding ToolBar::DEFAULT_PADDING(0.f, 0.f, 0.f, 0.f);

ToolBar::ToolBar()
{
}

ToolBar::ToolBar(const ToolBar& handle) = default;

ToolBar& ToolBar::operator=(const ToolBar& handle) = default;

ToolBar::ToolBar(ToolBar&& handle) = default;

ToolBar& ToolBar::operator=(ToolBar&& handle) = default;

ToolBar::~ToolBar()
{
}

ToolBar ToolBar::New()
{
  return Internal::ToolBar::New();
}

ToolBar ToolBar::DownCast(BaseHandle handle)
{
  return Control::DownCast<ToolBar, Internal::ToolBar>(handle);
}

void ToolBar::AddControl(Actor control, float relativeSize, Toolkit::Alignment::Type alignment, const Toolkit::Alignment::Padding& padding)
{
  GetImpl(*this).AddControl(control, relativeSize, alignment, padding);
}

void ToolBar::RemoveControl(Actor control)
{
  GetImpl(*this).RemoveControl(control);
}

ToolBar::ToolBar(Internal::ToolBar& implementation)
: Control(implementation)
{
}

ToolBar::ToolBar(Dali::Internal::CustomActor* internal)
: Control(internal)
{
  VerifyCustomActorPointer<Internal::ToolBar>(internal);
}

} // namespace Toolkit

} // namespace Dali

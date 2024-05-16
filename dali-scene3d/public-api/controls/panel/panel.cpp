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

// CLASS HEADER
#include <dali-scene3d/public-api/controls/panel/panel.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/controls/panel/panel-impl.h>
#include <dali-scene3d/public-api/model-components/model-node.h>

namespace Dali::Scene3D
{
Panel::Panel() = default;

Panel::Panel(const Panel& model) = default;

Panel::Panel(Panel&& rhs) noexcept = default;

Panel& Panel::operator=(const Panel& model) = default;

Panel& Panel::operator=(Panel&& rhs) noexcept = default;

Panel::~Panel() = default;

Panel Panel::New()
{
  return Internal::Panel::New();
}

Panel Panel::DownCast(BaseHandle handle)
{
  return Control::DownCast<Panel, Internal::Panel>(handle);
}

Panel::Panel(Internal::Panel& implementation)
: Control(implementation)
{
}

Panel::Panel(Dali::Internal::CustomActor* internal)
: Control(internal)
{
  VerifyCustomActorPointer<Internal::Panel>(internal);
}

void Panel::SetPanelResolution(const Vector2& resolution)
{
  GetImpl(*this).SetPanelResolution(resolution);
}

Vector2 Panel::GetPanelResolution() const
{
  return GetImpl(*this).GetPanelResolution();
}

void Panel::SetContent(Dali::Actor rootActor)
{
  GetImpl(*this).SetContent(rootActor);
}

Dali::Actor Panel::GetContent() const
{
  return GetImpl(*this).GetContent();
}

void Panel::ClearPanel()
{
  GetImpl(*this).ClearPanel();
}

void Panel::CastShadow(bool castShadow)
{
  GetImpl(*this).CastShadow(castShadow);
}

bool Panel::IsShadowCasting() const
{
  return GetImpl(*this).IsShadowCasting();
}

void Panel::ReceiveShadow(bool receiveShadow)
{
  GetImpl(*this).ReceiveShadow(receiveShadow);
}

bool Panel::IsShadowReceiving() const
{
  return GetImpl(*this).IsShadowReceiving();
}

} // namespace Dali::Scene3D

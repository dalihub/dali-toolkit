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
#include <dali-toolkit/internal/controls/render-effects/render-effect-impl.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/control/control-renderers.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>

namespace
{
static constexpr float SIZE_STEP_CONDITION = 3.0f;
} // namespace

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
RenderEffectImpl::~RenderEffectImpl()
{
  if(mOwnerControl)
  {
    mOwnerControl.ClearRenderEffect();
  }
}

void RenderEffectImpl::SetOwnerControl(Dali::Toolkit::Control control)
{
  if(control && (control != mOwnerControl))
  {
    mOwnerControl = control;

    mTargetSize = mOwnerControl.GetProperty<Vector2>(Actor::Property::SIZE);
    mRenderer   = CreateRenderer(SHADER_RENDER_EFFECT_VERT, SHADER_RENDER_EFFECT_FRAG);

    mSizeNotification = control.AddPropertyNotification(Actor::Property::SIZE, StepCondition(SIZE_STEP_CONDITION));
    mSizeNotification.NotifySignal().Connect(this, &RenderEffectImpl::OnSizeSet);
  }
}

void RenderEffectImpl::ClearOwnerControl()
{
  if(mOwnerControl)
  {
    Renderer renderer = GetTargetRenderer();
    mOwnerControl.RemoveRenderer(renderer);
    mOwnerControl.RemovePropertyNotification(mSizeNotification);
    mOwnerControl.Reset();
  }
}

Toolkit::Control RenderEffectImpl::GetOwnerControl() const
{
  return mOwnerControl;
}

void RenderEffectImpl::OnSizeSet(PropertyNotification& source)
{
  if(mOwnerControl)
  {
    mTargetSize = mOwnerControl.GetCurrentProperty<Vector2>(Actor::Property::SIZE);
    Deactivate();
    Activate();
  }
}

Renderer RenderEffectImpl::GetTargetRenderer() const
{
  return mRenderer;
}

Vector2 RenderEffectImpl::GetTargetSize() const
{
  return mTargetSize;
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali

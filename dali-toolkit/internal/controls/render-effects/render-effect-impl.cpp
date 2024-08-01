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

// EXTERNAL INCLUDES
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/integration-api/debug.h>

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
#if defined(DEBUG_ENABLED)
// Keep this log filter inside of Dali::Toolkit::Internal, so subclass of RenderEffect can also use this.
Debug::Filter* gRenderEffectLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_RENDER_EFFECT");
#endif

RenderEffectImpl::RenderEffectImpl()
: mRenderer(),
  mOwnerControl(),
  mSizeNotification(),
  mTargetSize(Vector2::ZERO),
  mIsActivated(false)
{
  DALI_LOG_INFO(gRenderEffectLogFilter, Debug::Verbose, "[RenderEffect:%p] Constructor\n", this);
}

RenderEffectImpl::~RenderEffectImpl()
{
  DALI_LOG_INFO(gRenderEffectLogFilter, Debug::Verbose, "[RenderEffect:%p] Destructor. [ID:%d]\n", this, mOwnerControl ? mOwnerControl.GetProperty<int>(Actor::Property::ID) : -1);
  if(mOwnerControl)
  {
    mOwnerControl.ClearRenderEffect();
  }
}

void RenderEffectImpl::SetOwnerControl(Dali::Toolkit::Control control)
{
  if(mOwnerControl != control)
  {
    // Clear previous owner control
    ClearOwnerControl();

    mOwnerControl = control;

    DALI_LOG_INFO(gRenderEffectLogFilter, Debug::General, "[RenderEffect:%p] SetOwnerControl [ID:%d]\n", this, mOwnerControl ? mOwnerControl.GetProperty<int>(Actor::Property::ID) : -1);

    if(mOwnerControl)
    {
      mTargetSize = mOwnerControl.GetProperty<Vector2>(Actor::Property::SIZE);
      if(!mRenderer)
      {
        mRenderer = CreateRenderer(SHADER_RENDER_EFFECT_VERT, SHADER_RENDER_EFFECT_FRAG);
      }

      mOwnerControl.InheritedVisibilityChangedSignal().Connect(this, &RenderEffectImpl::OnControlInheritedVisibilityChanged);

      mSizeNotification = mOwnerControl.AddPropertyNotification(Actor::Property::SIZE, StepCondition(SIZE_STEP_CONDITION));
      mSizeNotification.NotifySignal().Connect(this, &RenderEffectImpl::OnSizeSet);

      Activate(); // Dev note : Activate after set the owner control.
    }
  }
}

void RenderEffectImpl::ClearOwnerControl()
{
  DALI_LOG_INFO(gRenderEffectLogFilter, Debug::General, "[RenderEffect:%p] ClearOwnerControl [ID:%d]\n", this, mOwnerControl ? mOwnerControl.GetProperty<int>(Actor::Property::ID) : -1);
  if(mOwnerControl)
  {
    Deactivate(); // Dev note : Deactivate before clearing the owner control.

    mOwnerControl.InheritedVisibilityChangedSignal().Disconnect(this, &RenderEffectImpl::OnControlInheritedVisibilityChanged);

    mOwnerControl.RemovePropertyNotification(mSizeNotification);
    mSizeNotification.Reset();

    auto previousOwnerControl = mOwnerControl;
    mOwnerControl.Reset();

    // Make previous owner don't have render effect, after make we don't have owner control now.
    previousOwnerControl.ClearRenderEffect();
  }
}

bool RenderEffectImpl::IsActivated() const
{
  return mIsActivated;
}

void RenderEffectImpl::Initialize()
{
  OnInitialize();
}

Toolkit::Control RenderEffectImpl::GetOwnerControl() const
{
  return mOwnerControl;
}

Renderer RenderEffectImpl::GetTargetRenderer() const
{
  return mRenderer;
}

Vector2 RenderEffectImpl::GetTargetSize() const
{
  return mTargetSize;
}

void RenderEffectImpl::Activate()
{
  if(!IsActivated() && IsActivateValid())
  {
    DALI_LOG_INFO(gRenderEffectLogFilter, Debug::General, "[RenderEffect:%p] Activated! [ID:%d]\n", this, mOwnerControl ? mOwnerControl.GetProperty<int>(Actor::Property::ID) : -1);
    mIsActivated = true;

    // Activate logic for subclass.
    OnActivate();
  }
}

void RenderEffectImpl::Deactivate()
{
  if(IsActivated() || !IsActivateValid())
  {
    DALI_LOG_INFO(gRenderEffectLogFilter, Debug::General, "[RenderEffect:%p] Deactivated! [ID:%d]\n", this, mOwnerControl ? mOwnerControl.GetProperty<int>(Actor::Property::ID) : -1);
    mIsActivated = false;

    // Deactivate logic for subclass.
    OnDeactivate();
  }
}

bool RenderEffectImpl::IsActivateValid() const
{
  // Activate is valid if
  // - Owner control is on scene
  // - All actors of owner control are visible
  // - The SceneHolder is exist, and it is visible
  // TODO : Currently we don't check SceneHolder's visibility.
  bool ret = false;

  if(mOwnerControl && mOwnerControl.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    Integration::SceneHolder sceneHolder = Integration::SceneHolder::Get(mOwnerControl);
    if(sceneHolder)
    {
      ret = true;

      // Check visibility of owner control's parents.
      // TODO : We'd better check the control visibility at core side.
      // TODO : Window visibility will be consider at dali-core actor side in future.
      Dali::Actor self = mOwnerControl;
      while(self)
      {
        if(!self.GetProperty<bool>(Dali::Actor::Property::VISIBLE))
        {
          ret = false;
          break;
        }
        self = self.GetParent();
      }
    }
  }

  DALI_LOG_INFO(gRenderEffectLogFilter, Debug::Concise, "[RenderEffect:%p] IsActivateValid? [ID:%d][ret:%d]\n", this, mOwnerControl ? mOwnerControl.GetProperty<int>(Actor::Property::ID) : -1, ret);

  return ret;
}

void RenderEffectImpl::OnSizeSet(PropertyNotification& source)
{
  if(mOwnerControl)
  {
    mTargetSize = mOwnerControl.GetCurrentProperty<Vector2>(Actor::Property::SIZE);
    if(IsActivated())
    {
      Deactivate();
      Activate();
    }
  }
}

void RenderEffectImpl::OnControlInheritedVisibilityChanged(Actor actor, bool visible)
{
  DALI_LOG_INFO(gRenderEffectLogFilter, Debug::Concise, "[RenderEffect:%p] visibility changed [ID:%d][visible:%d]\n", this, mOwnerControl ? mOwnerControl.GetProperty<int>(Actor::Property::ID) : -1, visible);
  if(visible)
  {
    Activate();
  }
  else
  {
    Deactivate();
  }
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/internal/controls/control/control-renderers.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
static constexpr uint32_t RENDER_EFFECT_RENDER_PASS_TAG = 11;
} // namespace

#if defined(DEBUG_ENABLED)
// Keep this log filter inside of Dali::Toolkit::Internal, so subclass of RenderEffect can also use this.
Debug::Filter* gRenderEffectLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_RENDER_EFFECT");
#endif

uint32_t RenderEffectImpl::GetRenderPassTag()
{
  return RENDER_EFFECT_RENDER_PASS_TAG;
}

RenderEffectImpl::RenderEffectImpl()
: mRenderer(),
  mOwnerControl(),
  mTargetSize(Vector2::ZERO),
  mIsActivated(false)
{
  DALI_LOG_INFO(gRenderEffectLogFilter, Debug::Verbose, "[RenderEffect:%p] Constructor\n", this);
}

RenderEffectImpl::~RenderEffectImpl()
{
  DALI_LOG_INFO(gRenderEffectLogFilter, Debug::Verbose, "[RenderEffect:%p] Destructor.\n", this);

  // Reset weak handle first. (Since it might not valid during destruction.)
  mOwnerControl.Reset();
  mPlacementSceneHolder.Reset();

  // Don't call Deactivate here, since we cannot call virtual function during destruction.
  // Deactivate already be called at Control's destructor, and InheritVisibilityChanged signal.
}

void RenderEffectImpl::SetOwnerControl(Dali::Toolkit::Control control)
{
  Dali::Toolkit::Control ownerControl = mOwnerControl.GetHandle();
  if(ownerControl != control)
  {
    // Clear previous owner control
    ClearOwnerControl();

    mOwnerControl = (ownerControl = control);

    DALI_LOG_INFO(gRenderEffectLogFilter, Debug::General, "[RenderEffect:%p] SetOwnerControl [ID:%d]\n", this, ownerControl ? ownerControl.GetProperty<int>(Actor::Property::ID) : -1);

    if(ownerControl)
    {
      UpdateTargetSize();

      if(mAnimationConstraints.empty())
      {
        Renderer        renderer    = GetTargetRenderer();
        Property::Index radiusIndex = renderer.GetPropertyIndex("uCornerRadius");
        if(radiusIndex != Property::INVALID_INDEX)
        {
          Constraint cornerRadiusConstraint = Constraint::New<Vector4>(renderer, radiusIndex, EqualToConstraint());
          cornerRadiusConstraint.AddSource(Source(ownerControl, DevelControl::Property::CORNER_RADIUS));
          cornerRadiusConstraint.Apply();
          mAnimationConstraints.push_back(cornerRadiusConstraint);
        }

        Property::Index squarenessIndex = renderer.GetPropertyIndex("uCornerSquareness");
        if(squarenessIndex != Property::INVALID_INDEX)
        {
          Constraint cornerSquarenessConstraint = Constraint::New<Vector4>(renderer, squarenessIndex, EqualToConstraint());
          cornerSquarenessConstraint.AddSource(Source(ownerControl, DevelControl::Property::CORNER_SQUARENESS));
          cornerSquarenessConstraint.Apply();
          mAnimationConstraints.push_back(cornerSquarenessConstraint);
        }
      }

      ownerControl.InheritedVisibilityChangedSignal().Connect(this, &RenderEffectImpl::OnControlInheritedVisibilityChanged);

      Activate(); // Dev note : Activate after set the owner control.
    }
  }
}

void RenderEffectImpl::ClearOwnerControl()
{
  Deactivate(); // Dev note : Deactivate before clearing the owner control.

  for(auto constraint : mAnimationConstraints)
  {
    constraint.Remove();
  }
  mAnimationConstraints.clear();

  Dali::Toolkit::Control ownerControl = mOwnerControl.GetHandle();
  DALI_LOG_INFO(gRenderEffectLogFilter, Debug::General, "[RenderEffect:%p] ClearOwnerControl [ID:%d]\n", this, ownerControl ? ownerControl.GetProperty<int>(Actor::Property::ID) : -1);
  if(ownerControl)
  {
    ownerControl.InheritedVisibilityChangedSignal().Disconnect(this, &RenderEffectImpl::OnControlInheritedVisibilityChanged);

    auto previousOwnerControl = ownerControl;
    mOwnerControl.Reset();
    mPlacementSceneHolder.Reset();

    // Make previous owner don't have render effect, after make we don't have owner control now.
    if(previousOwnerControl.GetRenderEffect().GetObjectPtr() == this)
    {
      previousOwnerControl.ClearRenderEffect();
    }
  }
}

bool RenderEffectImpl::IsActivated() const
{
  return mIsActivated;
}

void RenderEffectImpl::Initialize()
{
  if(!mRenderer)
  {
    mRenderer = CreateRenderer(SHADER_RENDER_EFFECT_VERT, SHADER_RENDER_EFFECT_FRAG, static_cast<Shader::Hint::Value>(Shader::Hint::FILE_CACHE_SUPPORT | Shader::Hint::INTERNAL), "RENDER_EFFECT", Uint16Pair(1, 1));
    mRenderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, true); // Always use pre-multiply alpha

    mRenderer.RegisterProperty("uCornerRadius", Vector4::ZERO);
    mRenderer.RegisterProperty("uCornerSquareness", Vector4::ZERO);
    mRenderer.RegisterProperty("uCornerRadiusPolicy", static_cast<float>(1.0f));
  }

  OnInitialize();
}

Toolkit::Control RenderEffectImpl::GetOwnerControl() const
{
  return mOwnerControl.GetHandle();
}

Integration::SceneHolder RenderEffectImpl::GetSceneHolder() const
{
  return mPlacementSceneHolder.GetHandle();
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
    Dali::Toolkit::Control ownerControl = mOwnerControl.GetHandle();
    DALI_LOG_INFO(gRenderEffectLogFilter, Debug::General, "[RenderEffect:%p] Activated! [ID:%d]\n", this, ownerControl ? ownerControl.GetProperty<int>(Actor::Property::ID) : -1);

    // Keep sceneHolder as weak handle.
    Integration::SceneHolder sceneHolder = Integration::SceneHolder::Get(ownerControl);
    if(DALI_UNLIKELY(!sceneHolder))
    {
      DALI_LOG_ERROR("RenderEffect Could not be activated due to ownerControl's SceneHolder is not exist\n");
      return;
    }
    mIsActivated = true;

    mPlacementSceneHolder = sceneHolder;

    // Activate logic for subclass.
    OnActivate();

    // Set round corner. Default is to sync to owner control's BACKGROUND.
    Vector4 cornerRadius = ownerControl.GetProperty<Vector4>(Toolkit::DevelControl::Property::CORNER_RADIUS);
    if(cornerRadius != Vector4::ZERO)
    {
      int32_t cornerRadiusPolicy = ownerControl.GetProperty<int32_t>(Toolkit::DevelControl::Property::CORNER_RADIUS_POLICY);
      Vector4 cornerSquareness   = ownerControl.GetProperty<Vector4>(Toolkit::DevelControl::Property::CORNER_SQUARENESS);

      Property::Map map;
      map.Insert(Toolkit::DevelVisual::Property::CORNER_RADIUS, cornerRadius);
      map.Insert(Toolkit::DevelVisual::Property::CORNER_RADIUS_POLICY, static_cast<Toolkit::Visual::Transform::Policy::Type>(cornerRadiusPolicy));
      map.Insert(Toolkit::DevelVisual::Property::CORNER_SQUARENESS, cornerSquareness);

      SetCornerConstants(map);
    }
  }
}

void RenderEffectImpl::Deactivate()
{
  if(IsActivated())
  {
    mIsActivated = false;

    Dali::Toolkit::Control ownerControl = mOwnerControl.GetHandle();
    DALI_LOG_INFO(gRenderEffectLogFilter, Debug::General, "[RenderEffect:%p] Deactivated! [ID:%d]\n", this, ownerControl ? ownerControl.GetProperty<int>(Actor::Property::ID) : -1);

    // Deactivate logic for subclass.
    OnDeactivate();
  }
}

void RenderEffectImpl::Refresh()
{
  Dali::Toolkit::Control ownerControl = mOwnerControl.GetHandle();
  if(ownerControl)
  {
    UpdateTargetSize();

    if(IsActivateValid())
    {
      if(!IsActivated())
      {
        Activate();
      }
      else
      {
        OnRefresh();
      }
    }
    else
    {
      Deactivate();
    }
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

  Vector2 size = GetTargetSize();
  if(size.x > Math::MACHINE_EPSILON_1000 && size.y > Math::MACHINE_EPSILON_1000)
  {
    Dali::Toolkit::Control ownerControl = mOwnerControl.GetHandle();
    if(ownerControl && ownerControl.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
    {
      Integration::SceneHolder sceneHolder = Integration::SceneHolder::Get(ownerControl);
      if(sceneHolder)
      {
        ret = true;

        // Check visibility of owner control's parents.
        // TODO : We'd better check the control visibility at core side.
        // TODO : Window visibility will be consider at dali-core actor side in future.
        Dali::Actor self = ownerControl;
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

    DALI_LOG_INFO(gRenderEffectLogFilter, Debug::Concise, "[RenderEffect:%p] IsActivateValid? [ID:%d][ret:%d]\n", this, ownerControl ? ownerControl.GetProperty<int>(Actor::Property::ID) : -1, ret);
  }

  return ret;
}

void RenderEffectImpl::UpdateTargetSize()
{
  Vector2 size = GetOwnerControl().GetProperty<Vector2>(Actor::Property::SIZE);
  if(size == Vector2::ZERO)
  {
    size = GetOwnerControl().GetNaturalSize();
  }

  if(size == Vector2::ZERO || size.x < 0.0f || size.y < 0.0f)
  {
    mTargetSize = Vector2::ZERO;
  }

  const uint32_t maxTextureSize = Dali::GetMaxTextureSize();
  if(uint32_t(size.x) > maxTextureSize || uint32_t(size.y) > maxTextureSize)
  {
    uint32_t denominator = std::max(size.x, size.y);
    size.x               = (size.x * maxTextureSize / denominator);
    size.y               = (size.y * maxTextureSize / denominator);
  }
  mTargetSize = size;
}

void RenderEffectImpl::OnControlInheritedVisibilityChanged(Actor actor, bool visible)
{
  Dali::Toolkit::Control ownerControl = mOwnerControl.GetHandle();
  DALI_LOG_INFO(gRenderEffectLogFilter, Debug::Concise, "[RenderEffect:%p] visibility changed [ID:%d][visible:%d]\n", this, ownerControl ? ownerControl.GetProperty<int>(Actor::Property::ID) : -1, visible);
  if(visible)
  {
    Activate();
  }
  else
  {
    Deactivate();
  }
}

void RenderEffectImpl::SetCornerConstants(const Property::Map& map)
{
  DALI_LOG_INFO(gRenderEffectLogFilter, Debug::Verbose, "[BlurEffect:%p] Set corner radius constants to shader\n", this);

  Vector4 radius = Vector4::ZERO;
  map[Toolkit::DevelVisual::Property::CORNER_RADIUS].Get(radius);

  Vector4 squareness = Vector4::ZERO;
  map[Toolkit::DevelVisual::Property::CORNER_SQUARENESS].Get(squareness);

  Toolkit::Visual::Transform::Policy::Type policy = Toolkit::Visual::Transform::Policy::Type::ABSOLUTE;
  map[Toolkit::DevelVisual::Property::CORNER_RADIUS_POLICY].Get(policy);

  Renderer renderer = GetTargetRenderer();
  renderer.RegisterProperty("uCornerRadius", radius);
  renderer.RegisterProperty("uCornerSquareness", squareness);
  renderer.RegisterProperty("uCornerRadiusPolicy", static_cast<float>(policy));
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali

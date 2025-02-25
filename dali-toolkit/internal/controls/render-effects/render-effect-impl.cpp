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
  mCamera(),
  mOwnerControl(),
  mSizeNotification(),
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

  mSizeNotification.Reset();

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

      ownerControl.InheritedVisibilityChangedSignal().Connect(this, &RenderEffectImpl::OnControlInheritedVisibilityChanged);

      mSizeNotification = ownerControl.AddPropertyNotification(Actor::Property::SIZE, StepCondition(SIZE_STEP_CONDITION));
      mSizeNotification.NotifySignal().Connect(this, &RenderEffectImpl::OnSizeSet);

      Activate(); // Dev note : Activate after set the owner control.
    }
  }
}

void RenderEffectImpl::ClearOwnerControl()
{
  Deactivate(); // Dev note : Deactivate before clearing the owner control.

  Dali::Toolkit::Control ownerControl = mOwnerControl.GetHandle();
  DALI_LOG_INFO(gRenderEffectLogFilter, Debug::General, "[RenderEffect:%p] ClearOwnerControl [ID:%d]\n", this, ownerControl ? ownerControl.GetProperty<int>(Actor::Property::ID) : -1);
  if(ownerControl)
  {
    ownerControl.InheritedVisibilityChangedSignal().Disconnect(this, &RenderEffectImpl::OnControlInheritedVisibilityChanged);

    ownerControl.RemovePropertyNotification(mSizeNotification);
    mSizeNotification.Reset();

    auto previousOwnerControl = ownerControl;
    mOwnerControl.Reset();
    mPlacementSceneHolder.Reset();

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
  if(!mCamera)
  {
    mCamera = CameraActor::New();
    mCamera.SetInvertYAxis(true);
    mCamera.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mCamera.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mCamera.SetType(Dali::Camera::FREE_LOOK);
  }

  if(!mRenderer)
  {
    mRenderer = CreateRenderer(SHADER_RENDER_EFFECT_VERT, SHADER_RENDER_EFFECT_FRAG);
    mRenderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, true); // Always use pre-multiply alpha

    Shader shader = mRenderer.GetShader();
    shader.RegisterProperty("uCornerRadius", Vector4::ZERO);
    shader.RegisterProperty("uCornerSquareness", Vector4::ZERO);
    shader.RegisterProperty("uCornerRadiusPolicy", static_cast<float>(1.0f));
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

CameraActor RenderEffectImpl::GetCameraActor() const
{
  return mCamera;
}

void RenderEffectImpl::Activate()
{
  if(!IsActivated() && IsActivateValid())
  {
    mIsActivated = true;

    Dali::Toolkit::Control ownerControl = mOwnerControl.GetHandle();
    DALI_LOG_INFO(gRenderEffectLogFilter, Debug::General, "[RenderEffect:%p] Activated! [ID:%d]\n", this, ownerControl ? ownerControl.GetProperty<int>(Actor::Property::ID) : -1);

    // Keep sceneHolder as weak handle.
    Integration::SceneHolder sceneHolder = Integration::SceneHolder::Get(ownerControl);
    if(DALI_UNLIKELY(!sceneHolder))
    {
      DALI_LOG_ERROR("RenderEffect Could not be activated due to ownerControl's SceneHolder is not exist\n");
      return;
    }
    mPlacementSceneHolder = sceneHolder;

    Vector2 size = GetTargetSize();
    if(size != Vector2::ZERO)
    {
      mCamera.SetPerspectiveProjection(size);
      ownerControl.Add(mCamera);

      // Activate logic for subclass.
      OnActivate();

      // Set round corner. Default is to sync to owner control's BACKGROUND.
      Vector4 cornerRadius = ownerControl.GetProperty<Vector4>(Toolkit::DevelControl::Property::CORNER_RADIUS);
      if(cornerRadius != Vector4::ZERO)
      {
        int32_t cornerRadiusPolicy = ownerControl.GetProperty<int32_t>(Toolkit::DevelControl::Property::CORNER_RADIUS_POLICY);

        Property::Map map;
        map[Toolkit::DevelVisual::Property::CORNER_RADIUS]        = cornerRadius;
        map[Toolkit::DevelVisual::Property::CORNER_RADIUS_POLICY] = static_cast<Toolkit::Visual::Transform::Policy::Type>(cornerRadiusPolicy);

        SetCornerConstants(map);
      }
    }
  }
}

void RenderEffectImpl::Deactivate()
{
  if(IsActivated() || !IsActivateValid())
  {
    mIsActivated = false;

    Dali::Toolkit::Control ownerControl = mOwnerControl.GetHandle();
    DALI_LOG_INFO(gRenderEffectLogFilter, Debug::General, "[RenderEffect:%p] Deactivated! [ID:%d]\n", this, ownerControl ? ownerControl.GetProperty<int>(Actor::Property::ID) : -1);

    // Deactivate logic for subclass.
    OnDeactivate();

    mCamera.Unparent();
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

void RenderEffectImpl::OnSizeSet(PropertyNotification& source)
{
  Dali::Toolkit::Control ownerControl = mOwnerControl.GetHandle();
  if(ownerControl)
  {
    const auto targetSize = ownerControl.GetCurrentProperty<Vector2>(Actor::Property::SIZE);
    if(mTargetSize != targetSize && IsActivated())
    {
      UpdateTargetSize();
      mCamera.SetPerspectiveProjection(GetTargetSize());
      OnRefresh();
    }
  }
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

void RenderEffectImpl::SetCornerConstants(Property::Map map)
{
  DALI_LOG_INFO(gRenderEffectLogFilter, Debug::Verbose, "[BlurEffect:%p] Set corner radius constants to shader\n", this);

  Vector4 radius = Vector4::ZERO;
  map[Toolkit::DevelVisual::Property::CORNER_RADIUS].Get(radius);

  Vector4 squareness = Vector4::ZERO;
  map[Toolkit::DevelVisual::Property::CORNER_SQUARENESS].Get(squareness);

  Toolkit::Visual::Transform::Policy::Type policy;
  map[Toolkit::DevelVisual::Property::CORNER_RADIUS_POLICY].Get(policy);

  Renderer renderer = GetTargetRenderer();
  renderer.RegisterProperty("uCornerRadius", radius);
  renderer.RegisterProperty("uCornerSquareness", squareness);
  renderer.RegisterProperty("uCornerRadiusPolicy", static_cast<float>(policy));
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali

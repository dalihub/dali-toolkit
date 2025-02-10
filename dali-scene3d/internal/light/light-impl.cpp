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
#include <dali-scene3d/internal/light/light-impl.h>

// EXTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/controls/scene-view/scene-view-impl.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
namespace
{
static constexpr uint32_t         MAX_NUMBER_OF_LIGHT = 5;
static constexpr std::string_view LIGHT_COUNT_STRING("uLightCount");
static constexpr std::string_view LIGHT_DIRECTION_STRING("uLightDirection");
static constexpr std::string_view LIGHT_COLOR_STRING("uLightColor");
static constexpr std::string_view SHADOW_ENABLED_STRING("uIsShadowEnabled");
static constexpr std::string_view SHADOW_VIEW_PROJECTION_MATRIX_STRING("uShadowLightViewProjectionMatrix");

/**
 * Creates control through type registry
 */
BaseHandle Create()
{
  return Scene3D::Light::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(Scene3D::Light, Dali::CustomActor, Create);
DALI_TYPE_REGISTRATION_END()
} // unnamed namespace

Dali::Scene3D::Light Light::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr<Light> nodeImpl = new Light();

  // Pass ownership to handle
  Scene3D::Light handle(*nodeImpl);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  nodeImpl->Initialize();

  return handle;
}

Light::Light()
: Control(static_cast<ControlBehaviour>(ControlBehaviour::DISABLE_VISUALS | ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS | ActorFlags::DISABLE_SIZE_NEGOTIATION))
{
}

Light::~Light()
{
}

// From Internal::Control.

void Light::OnInitialize()
{
  Actor self = Self();

  // TODO : We need to check this is enough.
  Toolkit::DevelControl::EnableCreateAccessible(Toolkit::Control::DownCast(self), false);

  self.SetProperty(Dali::Actor::Property::COLOR, Color::WHITE);

  // Directional Light setting
  mLightSourceActor = Dali::CameraActor::New();
  mLightSourceActor.SetProjectionMode(Dali::Camera::ORTHOGRAPHIC_PROJECTION);
  mLightSourceActor.SetProperty(Dali::Actor::Property::POSITION, Vector3::ZERO);
  mLightSourceActor.SetProperty(Dali::Actor::Property::ORIENTATION, Quaternion());
  self.Add(mLightSourceActor);
}

// From CustomActorImpl.

void Light::OnSceneConnection(int depth)
{
  Actor parent = Self().GetParent();
  while(parent)
  {
    Scene3D::SceneView sceneView = Scene3D::SceneView::DownCast(parent);
    if(sceneView)
    {
      mParentSceneView = sceneView;
      if(mIsEnabled)
      {
        GetImpl(sceneView).AddLight(Scene3D::Light::DownCast(Self()));
      }
      if(mIsShadowEnabled)
      {
        GetImpl(sceneView).SetShadow(Scene3D::Light::DownCast(Self()));
      }
      break;
    }
    parent = parent.GetParent();
  }
}

void Light::OnSceneDisconnection()
{
  Scene3D::SceneView sceneView = mParentSceneView.GetHandle();
  if(sceneView)
  {
    mParentSceneView.Reset();
    GetImpl(sceneView).RemoveLight(Scene3D::Light::DownCast(Self()));
  }
}

void Light::OnChildAdd(Actor& child)
{
}

void Light::OnChildRemove(Actor& child)
{
}

void Light::OnSizeSet(const Vector3& targetSize)
{
}

void Light::OnSizeAnimation(Animation& animation, const Vector3& targetSize)
{
  // @todo size negotiate background to new size, animate as well?
}

void Light::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
}

void Light::OnSetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension)
{
}

Vector3 Light::GetNaturalSize()
{
  return Vector3::ZERO;
}

float Light::CalculateChildSize(const Dali::Actor& child, Dimension::Type dimension)
{
  return 0.0f;
}

float Light::GetHeightForWidth(float width)
{
  return 0.0f;
}

float Light::GetWidthForHeight(float height)
{
  return 0.0f;
}

bool Light::RelayoutDependentOnChildren(Dimension::Type dimension)
{
  return false;
}

void Light::OnCalculateRelayoutSize(Dimension::Type dimension)
{
}

void Light::OnLayoutNegotiated(float size, Dimension::Type dimension)
{
}

Light& GetImplementation(Dali::Scene3D::Light& handle)
{
  CustomActorImpl& customInterface = handle.GetImplementation();
  Light&           impl            = dynamic_cast<Internal::Light&>(customInterface);
  return impl;
}

const Light& GetImplementation(const Dali::Scene3D::Light& handle)
{
  const CustomActorImpl& customInterface = handle.GetImplementation();
  // downcast to control
  const Light& impl = dynamic_cast<const Internal::Light&>(customInterface);
  return impl;
}

// Public Method

void Light::Enable(bool enable)
{
  if(enable == mIsEnabled)
  {
    return;
  }
  mIsEnabled = enable;

  Scene3D::SceneView sceneView = mParentSceneView.GetHandle();
  if(!sceneView)
  {
    return;
  }

  if(mIsEnabled)
  {
    GetImpl(sceneView).AddLight(Scene3D::Light::DownCast(Self()));
  }
  else
  {
    GetImpl(sceneView).RemoveLight(Scene3D::Light::DownCast(Self()));
  }
}

bool Light::IsEnabled() const
{
  return mIsEnabled;
}

void Light::EnableShadow(bool enable)
{
  if(enable == mIsShadowEnabled)
  {
    return;
  }
  mIsShadowEnabled = enable;

  Scene3D::SceneView sceneView = mParentSceneView.GetHandle();
  if(!sceneView)
  {
    return;
  }

  if(mIsShadowEnabled)
  {
    GetImpl(sceneView).SetShadow(Scene3D::Light::DownCast(Self()));
  }
  else
  {
    GetImpl(sceneView).RemoveShadow(Scene3D::Light::DownCast(Self()));
  }
}

bool Light::IsShadowEnabled() const
{
  return mIsShadowEnabled;
}

CameraActor Light::GetCamera() const
{
  return mLightSourceActor;
}

void Light::EnableShadowSoftFiltering(bool useSoftFiltering)
{
  mUseSoftFiltering = useSoftFiltering;
  UpdateShadowUniforms();
}

bool Light::IsShadowSoftFilteringEnabled() const
{
  return mUseSoftFiltering;
}

void Light::SetShadowIntensity(float shadowIntensity)
{
  mShadowIntensity = shadowIntensity;
  UpdateShadowUniforms();
}

float Light::GetShadowIntensity() const
{
  return mShadowIntensity;
}

void Light::SetShadowBias(float shadowBias)
{
  mShadowBias = shadowBias;
  UpdateShadowUniforms();
}

float Light::GetShadowBias() const
{
  return mShadowBias;
}

// Public Static Method

uint32_t Light::GetMaximumEnabledLightCount()
{
  return MAX_NUMBER_OF_LIGHT;
}

std::string_view Light::GetLightCountUniformName()
{
  return LIGHT_COUNT_STRING;
}

std::string_view Light::GetLightDirectionUniformName()
{
  return LIGHT_DIRECTION_STRING;
}

std::string_view Light::GetLightColorUniformName()
{
  return LIGHT_COLOR_STRING;
}

std::string_view Light::GetShadowEnabledUniformName()
{
  return SHADOW_ENABLED_STRING;
}

std::string_view Light::GetShadowViewProjectionMatrixUniformName()
{
  return SHADOW_VIEW_PROJECTION_MATRIX_STRING;
}

// Private Method

void Light::UpdateShadowUniforms()
{
  Scene3D::SceneView sceneView = mParentSceneView.GetHandle();
  if(!sceneView)
  {
    return;
  }

  if(mIsShadowEnabled)
  {
    GetImpl(sceneView).UpdateShadowUniform(Scene3D::Light::DownCast(Self()));
  }
}

} // namespace Internal

} // namespace Scene3D

} // namespace Dali

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
#include <dali-scene3d/public-api/light/light.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/light/light-impl.h>

namespace Dali
{
namespace Scene3D
{
Light::Light()
{
}

Light::Light(const Light& light) = default;

Light::Light(Light&& rhs) noexcept = default;

Light& Light::operator=(const Light& light) = default;

Light& Light::operator=(Light&& rhs) noexcept = default;

Light::~Light()
{
}

Light Light::New()
{
  return Internal::Light::New();
}

Light Light::DownCast(BaseHandle handle)
{
  Light result;

  CustomActor custom = Dali::CustomActor::DownCast(handle);
  if(custom)
  {
    CustomActorImpl& customImpl = custom.GetImplementation();

    Internal::Light* impl = dynamic_cast<Internal::Light*>(&customImpl);

    if(impl)
    {
      result = Light(customImpl.GetOwner());
    }
  }

  return result;
}

void Light::Enable(bool enable)
{
  Internal::GetImplementation(*this).Enable(enable);
}

bool Light::IsEnabled() const
{
  return Internal::GetImplementation(*this).IsEnabled();
}

uint32_t Light::GetMaximumEnabledLightCount()
{
  return Internal::Light::GetMaximumEnabledLightCount();
}

void Light::EnableShadow(bool enable)
{
  Internal::GetImplementation(*this).EnableShadow(enable);
}

bool Light::IsShadowEnabled() const
{
  return Internal::GetImplementation(*this).IsShadowEnabled();
}

void Light::EnableShadowSoftFiltering(bool useSoftFiltering)
{
  Internal::GetImplementation(*this).EnableShadowSoftFiltering(useSoftFiltering);
}

bool Light::IsShadowSoftFilteringEnabled() const
{
  return Internal::GetImplementation(*this).IsShadowSoftFilteringEnabled();
}

void Light::SetShadowIntensity(float shadowIntensity)
{
  Internal::GetImplementation(*this).SetShadowIntensity(shadowIntensity);
}

float Light::GetShadowIntensity() const
{
  return Internal::GetImplementation(*this).GetShadowIntensity();
}

void Light::SetShadowBias(float shadowBias)
{
  Internal::GetImplementation(*this).SetShadowBias(shadowBias);
}

float Light::GetShadowBias() const
{
  return Internal::GetImplementation(*this).GetShadowBias();
}

Light::Light(Internal::Light& implementation)
: Control(implementation)
{
}

Light::Light(Dali::Internal::CustomActor* internal)
: Control(internal)
{
  // Can have a NULL pointer so we only need to check if the internal implementation is our class
  // when there is a value.
  if(internal)
  {
    DALI_ASSERT_DEBUG(dynamic_cast<Internal::Light*>(&CustomActor(internal).GetImplementation()));
  }
}

} // namespace Scene3D

} // namespace Dali

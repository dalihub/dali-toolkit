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
#include <dali-toolkit/public-api/controls/render-effects/gaussian-blur-effect.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/render-effects/gaussian-blur-effect-impl.h>

namespace Dali
{
namespace Toolkit
{
GaussianBlurEffect::GaussianBlurEffect() = default;

GaussianBlurEffect::GaussianBlurEffect(const GaussianBlurEffect& handle)
: RenderEffect(handle)
{
}

GaussianBlurEffect::GaussianBlurEffect(Internal::GaussianBlurEffectImpl* GaussianBlurEffectImpl)
: RenderEffect(GaussianBlurEffectImpl)
{
}

GaussianBlurEffect::~GaussianBlurEffect() = default;

GaussianBlurEffect GaussianBlurEffect::New()
{
  Internal::GaussianBlurEffectImplPtr internal = Internal::GaussianBlurEffectImpl::New();
  return GaussianBlurEffect(internal.Get());
}

GaussianBlurEffect GaussianBlurEffect::New(uint32_t blurRadius)
{
  Internal::GaussianBlurEffectImplPtr internal = Internal::GaussianBlurEffectImpl::New(blurRadius);
  return GaussianBlurEffect(internal.Get());
}

void GaussianBlurEffect::SetBlurOnce(bool blurOnce)
{
  GetImplementation(*this).SetBlurOnce(blurOnce);
}

bool GaussianBlurEffect::GetBlurOnce() const
{
  return GetImplementation(*this).GetBlurOnce();
}

void GaussianBlurEffect::SetBlurRadius(uint32_t blurRadius)
{
  GetImplementation(*this).SetBlurRadius(blurRadius);
}

uint32_t GaussianBlurEffect::GetBlurRadius() const
{
  return GetImplementation(*this).GetBlurRadius();
}

void GaussianBlurEffect::SetBlurDownscaleFactor(float downscaleFactor)
{
  GetImplementation(*this).SetBlurDownscaleFactor(downscaleFactor);
}

float GaussianBlurEffect::GetBlurDownscaleFactor() const
{
  return GetImplementation(*this).GetBlurDownscaleFactor();
}

void GaussianBlurEffect::AddBlurStrengthAnimation(Animation& animation, AlphaFunction alphaFunction, TimePeriod timePeriod, float fromValue, float toValue)
{
  GetImplementation(*this).AddBlurStrengthAnimation(animation, alphaFunction, timePeriod, fromValue, toValue);
}

void GaussianBlurEffect::AddBlurOpacityAnimation(Animation& animation, AlphaFunction alphaFunction, TimePeriod timePeriod, float fromValue, float toValue)
{
  GetImplementation(*this).AddBlurOpacityAnimation(animation, alphaFunction, timePeriod, fromValue, toValue);
}

GaussianBlurEffect::FinishedSignalType& GaussianBlurEffect::FinishedSignal()
{
  return GetImplementation(*this).FinishedSignal();
}

} // namespace Toolkit
} // namespace Dali

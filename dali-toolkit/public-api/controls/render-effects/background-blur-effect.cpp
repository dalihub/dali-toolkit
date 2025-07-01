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
#include <dali-toolkit/public-api/controls/render-effects/background-blur-effect.h>
// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/render-effects/background-blur-effect-impl.h>
namespace Dali
{
namespace Toolkit
{
BackgroundBlurEffect::BackgroundBlurEffect() = default;
BackgroundBlurEffect::BackgroundBlurEffect(const BackgroundBlurEffect& handle)
: RenderEffect(handle)
{
}

BackgroundBlurEffect::BackgroundBlurEffect(Internal::BackgroundBlurEffectImpl* backgroundBlurEffectImpl)
: RenderEffect(backgroundBlurEffectImpl)
{
}

BackgroundBlurEffect::~BackgroundBlurEffect() = default;

BackgroundBlurEffect BackgroundBlurEffect::New()
{
  Internal::BackgroundBlurEffectImplPtr internal = Internal::BackgroundBlurEffectImpl::New();
  return BackgroundBlurEffect(internal.Get());
}

BackgroundBlurEffect BackgroundBlurEffect::New(uint32_t blurRadius)
{
  Internal::BackgroundBlurEffectImplPtr internal = Internal::BackgroundBlurEffectImpl::New(blurRadius);
  return BackgroundBlurEffect(internal.Get());
}

void BackgroundBlurEffect::SetBlurOnce(bool blurOnce)
{
  GetImplementation(*this).SetBlurOnce(blurOnce);
}

bool BackgroundBlurEffect::GetBlurOnce() const
{
  return GetImplementation(*this).GetBlurOnce();
}

void BackgroundBlurEffect::SetBlurRadius(uint32_t blurRadius)
{
  GetImplementation(*this).SetBlurRadius(blurRadius);
}

uint32_t BackgroundBlurEffect::GetBlurRadius() const
{
  return GetImplementation(*this).GetBlurRadius();
}

void BackgroundBlurEffect::SetBlurDownscaleFactor(float downscaleFactor)
{
  GetImplementation(*this).SetBlurDownscaleFactor(downscaleFactor);
}

float BackgroundBlurEffect::GetBlurDownscaleFactor() const
{
  return GetImplementation(*this).GetBlurDownscaleFactor();
}

void BackgroundBlurEffect::AddBlurStrengthAnimation(Animation& animation, AlphaFunction alphaFunction, TimePeriod timePeriod, float fromValue, float toValue)
{
  GetImplementation(*this).AddBlurStrengthAnimation(animation, alphaFunction, timePeriod, fromValue, toValue);
}

void BackgroundBlurEffect::AddBlurOpacityAnimation(Animation& animation, AlphaFunction alphaFunction, TimePeriod timePeriod, float fromValue, float toValue)
{
  GetImplementation(*this).AddBlurOpacityAnimation(animation, alphaFunction, timePeriod, fromValue, toValue);
}

BackgroundBlurEffect::FinishedSignalType& BackgroundBlurEffect::FinishedSignal()
{
  return GetImplementation(*this).FinishedSignal();
}

} // namespace Toolkit
} // namespace Dali

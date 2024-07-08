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
#include <dali-toolkit/public-api/controls/render-effects/background-blur-effect.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/render-effects/blur-effect-impl.h>

namespace Dali
{
namespace Toolkit
{
BackgroundBlurEffect::BackgroundBlurEffect() = default;

BackgroundBlurEffect::BackgroundBlurEffect(const BackgroundBlurEffect& handle)
: RenderEffect(handle)
{
}

BackgroundBlurEffect::BackgroundBlurEffect(Internal::BlurEffectImpl* blurEffectImpl)
: RenderEffect(blurEffectImpl)
{
}

BackgroundBlurEffect::~BackgroundBlurEffect() = default;

BackgroundBlurEffect BackgroundBlurEffect::New()
{
  Internal::BlurEffectImplPtr internal = Internal::BlurEffectImpl::New(true);
  return BackgroundBlurEffect(internal.Get());
}

BackgroundBlurEffect BackgroundBlurEffect::New(float downscaleFactor, uint32_t blurRadius)
{
  Internal::BlurEffectImplPtr internal = Internal::BlurEffectImpl::New(downscaleFactor, blurRadius, true);
  return BackgroundBlurEffect(internal.Get());
}

} // namespace Toolkit
} // namespace Dali

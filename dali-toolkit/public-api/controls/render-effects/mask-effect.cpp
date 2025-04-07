
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
#include <dali-toolkit/public-api/controls/render-effects/mask-effect.h>
// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/render-effects/mask-effect-impl.h>
namespace Dali
{
namespace Toolkit
{
MaskEffect::MaskEffect() = default;
MaskEffect::MaskEffect(const MaskEffect& handle)
: RenderEffect(handle)
{
}
MaskEffect::MaskEffect(Internal::MaskEffectImpl* maskEffectImpl)
: RenderEffect(maskEffectImpl)
{
}
MaskEffect::~MaskEffect() = default;
MaskEffect MaskEffect::New(Toolkit::Control maskControl)
{
  Internal::MaskEffectImplPtr internal = Internal::MaskEffectImpl::New(maskControl);
  return MaskEffect(internal.Get());
}
MaskEffect MaskEffect::New(Toolkit::Control maskControl, MaskMode maskMode, Vector2 maskPosition, Vector2 maskScale)
{
  Internal::MaskEffectImplPtr internal = Internal::MaskEffectImpl::New(maskControl, maskMode, maskPosition, maskScale);
  return MaskEffect(internal.Get());
}
} // namespace Toolkit
} // namespace Dali

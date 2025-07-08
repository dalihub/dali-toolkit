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
#include <dali-toolkit/public-api/controls/render-effects/render-effect.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/render-effects/render-effect-impl.h>

namespace Dali
{
namespace Toolkit
{
RenderEffect::RenderEffect(const RenderEffect& handle)
: BaseHandle(handle)
{
}

RenderEffect::RenderEffect(Internal::RenderEffectImpl* renderEffectImpl)
: BaseHandle(renderEffectImpl)
{
}

void RenderEffect::Activate()
{
  GetImplementation(*this).Activate();
}

void RenderEffect::Deactivate()
{
  GetImplementation(*this).Deactivate();
}

void RenderEffect::Refresh()
{
  GetImplementation(*this).Refresh();
}

bool RenderEffect::IsActivated()
{
  return GetImplementation(*this).IsActivated();
}

} // namespace Toolkit
} // namespace Dali

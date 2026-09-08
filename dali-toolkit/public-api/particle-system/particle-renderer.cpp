/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit/internal/particle-system/particle-renderer-impl.h>
#include <dali-toolkit/public-api/particle-system/particle-renderer.h>

namespace DALI_NAMESPACE::Toolkit::ParticleSystem
{
ParticleRenderer ParticleRenderer::New()
{
  return {new Internal::ParticleRenderer()};
}

ParticleRenderer::ParticleRenderer(Internal::ParticleRenderer* impl)
: Dali::BaseHandle(impl)
{
}

void ParticleRenderer::SetTexture(const Dali::Texture& texture)
{
  GetImplementation(*this).SetTexture(texture);
}

void ParticleRenderer::SetBlendingMode(BlendingMode blendingMode)
{
  GetImplementation(*this).SetBlendingMode(blendingMode);
}

BlendingMode ParticleRenderer::GetBlendingMode() const
{
  return GetImplementation(*this).GetBlendingMode();
}

ParticleRenderer ParticleRenderer::DownCast(BaseHandle handle)
{
  return {dynamic_cast<Internal::ParticleRenderer*>(handle.GetObjectPtr())};
}

} //namespace DALI_NAMESPACE::Toolkit::ParticleSystem
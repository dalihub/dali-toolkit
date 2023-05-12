/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-toolkit/internal/particle-system/particle-impl.h>

namespace Dali::Toolkit::ParticleSystem
{

Particle Particle::DownCast(BaseHandle handle)
{
  return {dynamic_cast<Internal::Particle*>(handle.GetObjectPtr())};
}

void* Particle::Get(ParticleStreamTypeFlagBit streamBit)
{
  return GetImplementation(*this).Get(streamBit);
}

void* Particle::GetByIndex(uint32_t streamIndex)
{
  return GetImplementation(*this).GetByIndex(streamIndex);
}

Particle::Particle(Dali::Toolkit::ParticleSystem::Internal::Particle* impl)
: Dali::BaseHandle(impl)
{
}

uint32_t Particle::GetIndex() const
{
  return GetImplementation(*this).GetIndex();
}

} // namespace Dali::Toolkit::ParticleSystem

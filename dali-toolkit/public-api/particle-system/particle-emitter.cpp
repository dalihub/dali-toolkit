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

// CLASS HEADER
#include <dali-toolkit/public-api/particle-system/particle-emitter.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/particle-system/particle-emitter-impl.h>
#include <dali-toolkit/public-api/particle-system/particle-source.h>

// EXTERNAL INCLUDES
#include <utility>

namespace Dali::Toolkit::ParticleSystem
{
ParticleEmitter ParticleEmitter::New()
{
  return {new Dali::Toolkit::ParticleSystem::Internal::ParticleEmitter()};
}

ParticleEmitter ParticleEmitter::DownCast(BaseHandle handle)
{
  return {dynamic_cast<Internal::ParticleEmitter*>(handle.GetObjectPtr())};
}

void ParticleEmitter::SetSource(const ParticleSource& particleSource)
{
  GetImplementation(*this).SetSource(particleSource);
}

ParticleSource ParticleEmitter::GetSource() const
{
  return GetImplementation(*this).GetSource();
}

void ParticleEmitter::SetDomain(const ParticleDomain& particleDomain)
{
  GetImplementation(*this).SetDomain(particleDomain);
}

void ParticleEmitter::SetRenderer(const ParticleRenderer& particleRenderer)
{
  GetImplementation(*this).SetRenderer(particleRenderer);
}

void ParticleEmitter::SetParticleCount(uint32_t maxParticleCount)
{
  GetImplementation(*this).SetParticleCount(maxParticleCount);
}

ParticleDomain ParticleEmitter::GetDomain() const
{
  return GetImplementation(*this).GetDomain();
}

ParticleRenderer ParticleEmitter::GetRenderer() const
{
  return GetImplementation(*this).GetRenderer();
}

uint32_t ParticleEmitter::AddModifier(const ParticleModifier& particleModifier)
{
  return GetImplementation(*this).AddModifier(particleModifier);
}

ParticleModifier ParticleEmitter::GetModifierAt(uint32_t index)
{
  return GetImplementation(*this).GetModifierAt(index);
}

void ParticleEmitter::RemoveModifierAt(uint32_t index)
{
  return GetImplementation(*this).RemoveModifierAt(index);
}

void ParticleEmitter::AttachTo(Actor actor)
{
  GetImplementation(*this).AttachTo(std::move(actor));
}

ParticleEmitter::ParticleEmitter(Internal::ParticleEmitter* impl)
: Dali::BaseHandle(impl)
{
}

ParticleEmitter::Status ParticleEmitter::GetStatus() const
{
  return GetImplementation(*this).GetStatus();
}

ParticleList& ParticleEmitter::GetParticleList()
{
  return GetImplementation(*this).GetParticleList();
}

void ParticleEmitter::Start()
{
  GetImplementation(*this).Start();
}

void ParticleEmitter::Stop()
{
  GetImplementation(*this).Stop();
}

void ParticleEmitter::SetEmissionRate(uint32_t ratePerSecond)
{
  GetImplementation(*this).SetEmissionRate(ratePerSecond);
}

uint32_t ParticleEmitter::GetEmissionRate() const
{
  return GetImplementation(*this).GetEmissionRate();
}

[[maybe_unused]] void ParticleEmitter::EnableParallelProcessing(bool enabled)
{
  GetImplementation(*this).EnableParallelProcessing(enabled);
}

bool ParticleEmitter::IsParallelProcessingEnabled() const
{
  return GetImplementation(*this).IsParallelProcessingEnabled();
}

void ParticleEmitter::SetInitialParticleCount(uint32_t count)
{
  GetImplementation(*this).SetInitialParticleCount(count);
}

uint32_t ParticleEmitter::GetInitialParticleCount() const
{
  return GetImplementation(*this).GetInitialParticleCount();
}

void ParticleEmitter::SetActiveParticlesLimit(uint32_t count)
{
  GetImplementation(*this).SetActiveParticlesLimit(count);
}

uint32_t ParticleEmitter::GetActiveParticlesLimit() const
{
  return GetImplementation(*this).GetActiveParticlesLimit();
}

} // namespace Dali::Toolkit::ParticleSystem

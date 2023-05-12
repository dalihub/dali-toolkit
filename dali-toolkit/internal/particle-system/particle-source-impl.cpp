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

#include <dali-toolkit/internal/particle-system/particle-emitter-impl.h>
#include <dali-toolkit/internal/particle-system/particle-list-impl.h>
#include <dali-toolkit/internal/particle-system/particle-source-impl.h>
#include <memory>

namespace Dali::Toolkit::ParticleSystem::Internal
{
ParticleSource::ParticleSource(std::unique_ptr<ParticleSourceInterface>&& sourceUpdater)
: mUpdater(std::move(sourceUpdater))
{
}

void ParticleSource::Update(ParticleSystem::ParticleList& list, uint32_t count)
{
  // updating the source
  mUpdater->Update(list, count);
}

ParticleSourceInterface& ParticleSource::GetUpdater() const
{
  return *mUpdater;
}

} // namespace Dali::Toolkit::ParticleSystem::Internal
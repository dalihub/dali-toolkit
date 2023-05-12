#ifndef DALI_TOOLKIT_PARTICLE_SYSTEM_INTERNAL_PARTICLE_DOMAIN_H
#define DALI_TOOLKIT_PARTICLE_SYSTEM_INTERNAL_PARTICLE_DOMAIN_H
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
#include <dali-toolkit/public-api/particle-system/particle-domain.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-object.h>
#include <memory>

namespace Dali::Toolkit::ParticleSystem::Internal
{
class ParticleDomain : public Dali::BaseObject
{
};

} // namespace Dali::Toolkit::ParticleSystem::Internal

namespace Dali::Toolkit::ParticleSystem
{
inline Internal::ParticleDomain& GetImplementation(ParticleSystem::ParticleDomain& source)
{
  DALI_ASSERT_ALWAYS(source && "ParticleDomain handle is empty");

  BaseObject& handle = source.GetBaseObject();

  return static_cast<Internal::ParticleDomain&>(handle);
}

inline const Internal::ParticleDomain& GetImplementation(const ParticleSystem::ParticleDomain& source)
{
  DALI_ASSERT_ALWAYS(source && "ParticleDomain handle is empty");

  const BaseObject& handle = source.GetBaseObject();

  return static_cast<const Internal::ParticleDomain&>(handle);
}

} // namespace Dali::Toolkit::ParticleSystem

#endif // DALI_TOOLKIT_PARTICLE_SYSTEM_INTERNAL_PARTICLE_DOMAIN_H
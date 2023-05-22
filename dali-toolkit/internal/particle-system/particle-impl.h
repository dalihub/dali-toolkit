#ifndef DALI_TOOLKIT_PARTICLE_SYSTEM_INTERNAL_PARTICLE_H
#define DALI_TOOLKIT_PARTICLE_SYSTEM_INTERNAL_PARTICLE_H
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
#include <dali-toolkit/internal/particle-system/particle-list-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/particle-system/particle-list.h>

namespace Dali::Toolkit::ParticleSystem::Internal
{
class Particle : public Dali::BaseObject
{
public:
  ~Particle() = default;

  Particle(Internal::ParticleList& ownerList, uint32_t index);

  void* Get(ParticleStreamTypeFlagBit streamBit);

  void* GetByIndex(uint32_t streamIndex);

  uint32_t GetIndex() const;

private:
  Internal::ParticleList& mOwnerList;
  uint32_t                mIndex;
};
} // namespace Dali::Toolkit::ParticleSystem::Internal

namespace Dali::Toolkit::ParticleSystem
{
inline Internal::Particle& GetImplementation(ParticleSystem::Particle& particle)
{
  DALI_ASSERT_ALWAYS(particle && "Particle handle is empty");

  BaseObject& handle = particle.GetBaseObject();

  return static_cast<Internal::Particle&>(handle);
}

inline const Internal::Particle& GetImplementation(const ParticleSystem::Particle& particle)
{
  DALI_ASSERT_ALWAYS(particle && "Particle handle is empty");

  const BaseObject& handle = particle.GetBaseObject();

  return static_cast<const Internal::Particle&>(handle);
}

} // namespace Dali::Toolkit::ParticleSystem

#endif // DALI_TOOLKIT_PARTICLE_SYSTEM_INTERNAL_PARTICLE_H
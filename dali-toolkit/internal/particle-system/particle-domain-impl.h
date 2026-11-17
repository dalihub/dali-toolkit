#ifndef DALI_TOOLKIT_PARTICLE_SYSTEM_INTERNAL_PARTICLE_DOMAIN_H
#define DALI_TOOLKIT_PARTICLE_SYSTEM_INTERNAL_PARTICLE_DOMAIN_H
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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/particle-system/particle-domain.h>
#include <dali/public-api/common/intrusive-ptr.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-object.h>
#include <memory>

namespace DALI_NAMESPACE::Toolkit::ParticleSystem::Internal
{
class ParticleDomain : public Dali::BaseObject
{
public:
  /**
   * @brief Creates an unreferenced implementation only for BaseHandle construction.
   * @warning Pass the result directly to a BaseHandle-derived handle constructor.
   * The constructor must acquire its first reference (the returned reference count is zero).
   * Use New() when an IntrusivePtr owner is required.
   */
  static DALI_INTERNAL ParticleDomain* NewForBaseHandle()
  {
    return new ParticleDomain();
  }

  /**
   * @brief Creates a reference-counted implementation.
   */
  static Dali::IntrusivePtr<ParticleDomain> New()
  {
    return Dali::IntrusivePtr<ParticleDomain>(new ParticleDomain());
  }

protected:
  ParticleDomain()           = default;
  ~ParticleDomain() override = default;
};

} //namespace DALI_NAMESPACE::Toolkit::ParticleSystem::Internal

namespace DALI_NAMESPACE::Toolkit::ParticleSystem
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

} //namespace DALI_NAMESPACE::Toolkit::ParticleSystem

#endif // DALI_TOOLKIT_PARTICLE_SYSTEM_INTERNAL_PARTICLE_DOMAIN_H
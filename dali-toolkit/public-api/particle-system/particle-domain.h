#ifndef DALI_TOOLKIT_PARTICLE_SYSTEM_PARTICLE_DOMAIN_H
#define DALI_TOOLKIT_PARTICLE_SYSTEM_PARTICLE_DOMAIN_H
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
#include <dali-toolkit/public-api/particle-system/particle-types.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>
#include <memory>

namespace Dali::Toolkit::ParticleSystem::Internal
{
class ParticleDomain;
}

namespace Dali::Toolkit::ParticleSystem
{

/**
 * @class ParticleDomain
 *
 * @brief ParticleDomain bounds simulation area to the specified region
 */
class DALI_TOOLKIT_API ParticleDomain : public Dali::BaseHandle
{
public:
  /**
   * @brief Constructor
   */
  ParticleDomain() = default;

  /**
   * @brief Creates new ParticleDomain object
   *
   * @return New ParticleDomain object
   */
  static ParticleDomain New();

  /**
   * @brief Downcasts a handle to ParticleDomain handle.
   *
   * If handle points to an ParticleDomain object, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle to An object
   * @return handle to a ParticleDomain object or an uninitialized handle
   */
  static ParticleDomain DownCast(BaseHandle handle);

private:
  /// @cond internal
  /**
   * @brief This constructor is used by ParticleDomain::New() methods.
   *
   * @param [in] impl A pointer to a newly allocated implementation
   */
  ParticleDomain(Internal::ParticleDomain* impl);
  /// @endcond
};

} // namespace Dali::Toolkit::ParticleSystem

#endif // DALI_TOOLKIT_PARTICLE_SYSTEM_PARTICLE_DOMAIN_H
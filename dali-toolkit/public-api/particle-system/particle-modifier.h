#ifndef DALI_TOOLKIT_PARTICLE_SYSTEM_PARTICLE_MODIFIER_H
#define DALI_TOOLKIT_PARTICLE_SYSTEM_PARTICLE_MODIFIER_H
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
#include <dali/public-api/signals/callback.h>
#include <memory>

namespace Dali::Toolkit::ParticleSystem::Internal
{
class ParticleModifier;
}

namespace Dali::Toolkit::ParticleSystem
{
class ParticleEmitter;

class ParticleList;

class DALI_TOOLKIT_API ParticleModifierInterface
{
public:
  /**
   * @brief Destructor
   */
  virtual ~ParticleModifierInterface() = default;

  /**
   * @brief Update function to update the modifier to alter the behavior of particles.
   * @param[in] particleList       List of particles
   * @param[in] firstParticleIndex Index of the first particle
   * @param[in] particleCount      Number of particles
   */
  virtual void Update(ParticleList& particleList, uint32_t firstParticleIndex, uint32_t particleCount) = 0;

  /**
   * @brief Will be called to check whether modifier supports multi-threading
   *
   * If modifier supports multi-threading then Update() function will be called
   * providing partial range of particles to process.
   *
   * It is important to make sure, the batch of particles has no dependencies
   * on particles from outside the batch. If this is the case, the function
   * must return false and single threaded process will proceed.
   *
   * @return True if modifier supports multi-threading
   */
  virtual bool IsMultiThreaded()
  {
    return false;
  }
};

/**
 * @class ParticleModifier
 *
 * @brief ParticleModifer allows altering particle behaviour during simulation.
 *
 * Multiple modifiers can be used in the modifier stack.
 *
 * Output of previous modifier becomes an input of the next one.
 *
 */
class DALI_TOOLKIT_API ParticleModifier : public Dali::BaseHandle
{
public:
  /**
   * @brief Constructor
   */
  ParticleModifier() = default;

  /**
   * @brief Destructor
   */
  ~ParticleModifier() = default;

  /**
   * @brief Creates new modifier with given functor object.
   * The modifier takes over the ownership over the functor object.
   *
   * @param[in] modifierUpdater Functor for the modifier
   * @return New ParticleModifier object
   */
  static ParticleModifier New(std::unique_ptr<ParticleModifierInterface>&& modifierUpdater);

  /**
   * @brief Creates new modifier with given class and construction arguments
   *
   * @return New ParticleModifier object
   */
  template<class T, class... Args>
  static ParticleModifier New(Args... args)
  {
    return New(std::move(std::make_unique<T>(args...)));
  }
  /**
   * @brief Returns associated particle modifier callback
   *
   * @return Valid reference to associated callback
   */
  ParticleModifierInterface& GetModifierCallback();

  /**
   * @brief Downcasts a handle to ParticleModifier handle.
   *
   * If handle points to an ParticleModifier object, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle to An object
   * @return handle to a ParticleModifier object or an uninitialized handle
   */
  static ParticleModifier DownCast(BaseHandle handle);

private:
  /// @cond internal
  /**
   * @brief This constructor is used by ParticleModifier::New() methods.
   *
   * @param [in] impl A pointer to a newly allocated implementation
   */
  ParticleModifier(Internal::ParticleModifier* impl);
  /// @endcond
};

} // namespace Dali::Toolkit::ParticleSystem

#endif

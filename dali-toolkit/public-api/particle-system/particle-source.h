#ifndef DALI_TOOLKIT_PARTICLE_SYSTEM_PARTICLE_SOURCE_H
#define DALI_TOOLKIT_PARTICLE_SYSTEM_PARTICLE_SOURCE_H
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

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/callback.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/particle-system/particle-types.h>

namespace Dali::Toolkit::ParticleSystem::Internal
{
class ParticleSource;
}

namespace Dali::Toolkit::ParticleSystem
{
class ParticleEmitter;

class ParticleList;

class ParticleDomain;

class ParticleModifier;

class ParticleRenderer;

/**
 * @brief Interface to override as a particle source
 *
 * ParticleSourceInterace allows altering behaviour for
 * generating new particles.
 */
class DALI_TOOLKIT_API ParticleSourceInterface
{
public:
  /**
   * @brief Adds new particles
   * @param[in] outList List to write back
   * @param[in] count Requested particles count
   *
   * @return Function should return a Number of emitted particles
   */
  virtual uint32_t Update(ParticleList& outList, uint32_t count) = 0;

  /**
   * @brief Called when source is added to the emitter
   */
  virtual void Init() = 0;
};

/**
 * @class ParticleSource
 *
 * ParticleSource defines a logic associated with particles emission.
 * The emitter will use ParticleSource in order to spawn new particles.
 *
 * ParticleSource manages how, where and how many particles to emit.
 *
 * ParticleSource uses an implementation of ParticleSourceInterface when
 * emitting new particles.
 */
class DALI_TOOLKIT_API ParticleSource : public Dali::BaseHandle
{
public:
  /**
   * @brief Constructor
   */
  ParticleSource() = default;

  /**
   * @brief Destructor
   */
  ~ParticleSource() = default;

  /**
   * @brief Creates a new instance of ParticleSource
   *
   * New instance is given an ownership over valid implementation
   * of a ParticleSourceInferface.
   *
   * @param[in] particleSourceCallback Valid implementation of ParticleSourceInterface
   *
   * @return New ParticleSource object handle
   */
  static ParticleSource New(std::unique_ptr<ParticleSourceInterface>&& particleSourceCallback);

  /**
   * @brief Creates new ParticleSource with given source callback and arguments
   *
   * The function creates a ParticleSource of a given class (T) and constructor
   * arguments.
   *
   * @return New ParticleSource object handle
   */
  template<class T, class... Args>
  static ParticleSource New(Args... args)
  {
    return New(std::move(std::make_unique<T>(args...)));
  }

  /**
   * @brief Downcasts a handle to ParticleSource handle.
   *
   * If handle points to an ParticleSource object, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle to An object
   * @return handle to a ParticleSource object or an uninitialized handle
   */
  static ParticleSource DownCast(BaseHandle handle);

  /**
   * @brief Returns associated particle source callback
   *
   * @return Valid reference to associated callback
   */
  [[nodiscard]] ParticleSourceInterface& GetSourceCallback() const;

private:
  /// @cond internal
  /**
   * @brief This constructor is used by ParticleSource::New() methods.
   *
   * @param [in] impl A pointer to a newly allocated implementation
   */
  ParticleSource(Internal::ParticleSource* impl);
  /// @endcond
};
} // namespace Dali::Toolkit::ParticleSystem

#endif // DALI_TOOLKIT_PARTICLE_SYSTEM_PARTICLE_SOURCE_H
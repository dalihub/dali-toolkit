#ifndef DALI_TOOLKIT_PARTICLE_SYSTEM_PARTICLE_EMITTER_H
#define DALI_TOOLKIT_PARTICLE_SYSTEM_PARTICLE_EMITTER_H
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
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/object/base-handle.h>
#include <memory>

namespace Dali::Toolkit::ParticleSystem::Internal
{
class ParticleEmitter;
}

namespace Dali::Toolkit::ParticleSystem
{
class ParticleSource;

class ParticleDomain;

class ParticleList;

class ParticleModifier;

class ParticleRenderer;

/**
 * @class ParticleEmitter
 *
 * ParticleEmitter manages a particle emission process. The primary function
 * of the particle emitter class is to emit particles into a simulated environment.
 *
 * ParticleEmitter is responsible for:

 * - Particle generation:
 *
 * The emitter generates particles with specific initial properties,
 * such as position, velocity, size etc. It can create particles various ways which can be
 * implemented by overriding ParticleSourceInterface. It may create particles in bursts, streams
 * or in response to specific events. The ParticleSource must be set in order to generate particles
 * in the system.
 *
 * - Particle simulation:
 *
 * The emitter updates state of each particle by invoking ParticleModifier stack. It updates particles
 * over time to simulate desired behaviour. Particle modifiers may apply modifications to the system like
 * applying forces (gravity, wind), integrating physics.
 *
 * The stack of modifiers is executed in order and output of previous modifier is an input of
 * next one.
 *
 * At least one ParticleModifier must be set in order to update particle system and run
 * the simulation.
 *
 * - Particle rendering
 *
 * ParticleRenderer must be set in order to render particles. The basic renderer renders only 2D billboard
 * projected (always facing the camera) particles however the behaviour can be altered in order to render
 * more complex systems.
 *
 * Rendering may be optimize for different graphics APIs.
 *
 * - Particle management
 *
 * The emitter manages the lifecycle of particles, including creation (via ParticleSource),
 * update (via ParticleModifier stack) and removal (modifiers and specified lifetime of particles). It
 * handles scenarios such as recycling particles that have reached the end of their lifespan, reusing them,
 * or dynamically adjusting their properties based on the emitter's parameters or external factors.
 *
 * The particles are stored as ParticleList object which is generated internally. The emitter controls
 * behaviour of the particle list.
 *
 * The basic components making the Particle System are:
 * - ParticleEmitter - responsible for controlling the emission
 * - ParticleSource - responsible for generating new particles in the system
 * - ParticleModifier - responsible for altering the behaviour of particles (updates) and controlling the lifetime
 * - ParticleRenderer - responsible for rendering the particle system
 * - ParticleList - storage for particle data
 * - Particle - view on a selected particle data
 * - ParticleDomain - the domain of particle system (area/volume) that particle system is bound within
 *
 */
class DALI_TOOLKIT_API ParticleEmitter : public Dali::BaseHandle
{
public:
  /**
   * @brief Enum describing status of emitter
   */
  enum class Status
  {
    INCOMPLETE, ///< Status set when not all data is set in order to run simulation
    READY,      ///< Emitter ready (fully set up)
    STARTED,    ///< Emitter started
    PAUSED,     ///< Emitter paused
    STOPPED     ///< Emitter stopped
  };

  /**
   * @brief Creates new ParticleEmitter
   *
   * Function creates new ParticleEmitter object with given initial specification.
   *
   * @return valid emitter object
   */
  static ParticleEmitter New();

  /**
   * @brief Downcasts a handle to ParticleEmitter handle.
   *
   * If handle points to an ParticleEmitter object, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle to An object
   * @return handle to a ParticleEmitter object or an uninitialized handle
   */
  static ParticleEmitter DownCast(BaseHandle handle);

  /**
   * @brief Enables parallel processing on the CPU
   *
   * This flag gives a hint to attached ParticleSystem objects to use
   * multiple threads if possible.
   *
   * Setting this hint may have no effect after the particle system started
   * simulation. It should be set before calling Start() function.
   *
   * @param[in] enabled if True it will try to use MT
   */
  void EnableParallelProcessing(bool enabled);

  /**
   * @brief Tests whether parallel processing is enabled
   *
   * @return True if parallel processing mode is enabled.
   */
  [[nodiscard]] bool IsParallelProcessingEnabled() const;

  /**
   * @brief Sets emitter source
   *
   * ParticleSource represents objects responsible for emitting
   * new particles as well as reusing expired particles.
   *
   * @param[in] particleSource valid particle emitter source object
   *
   */
  void SetSource(const ParticleSource& particleSource);

  /**
   * @brief Returns currently used particle emitter source
   *
   * @return Handle to the ParticleSource object
   */
  [[nodiscard]] ParticleSource GetSource() const;

  /**
   * @brief Sets emitter domain
   *
   * ParticleDomain encloses the area of particle emission.
   *
   * @param[in] particleDomain valid particle emitter domain
   */
  void SetDomain(const ParticleDomain& particleDomain);

  /**
   * @brief Sets emitter domain
   *
   * ParticleRenderer provides implementation for rendering set of particles
   *
   * @param[in] particleRenderer a valid instance of ParticleRenderer
   */
  void SetRenderer(const ParticleRenderer& particleRenderer);

  /**
   * @brief Sets maximum particle count in the system
   *
   * This value is mutable but changing number of particles will
   * force regenerating the whole system data!
   *
   * @param[in] maxParticleCount maximum number of particles in the system
   */
  void SetParticleCount(uint32_t maxParticleCount);

  /**
   * @brief Returns currently used particle emitter domain
   *
   * @return Handle to the ParticleDomain object
   */
  [[nodiscard]] ParticleDomain GetDomain() const;

  /**
   * @brief Returns currently used particle emitter renderer
   *
   * @return Handle to the ParticleRenderer object
   */
  [[nodiscard]] ParticleRenderer GetRenderer() const;


  /**
   * @brief Attaches particle system to an actor
   *
   * @param[in] actor actor to attach the particle system
   */
  void AttachTo(Actor actor);

  /**
   * @brief Adds new modifier
   * @param[in] particleMmodifier valid ParticleModifier object
   *
   * @return Index into the modifier stack associated with added modifier
   */
  uint32_t AddModifier(const ParticleModifier& particleMmodifier);

  /**
   * @brief Sets particle emission rate per second
   *
   * @param[in] ratePerSecond maximum number of particles emitted per second
   */
  void SetEmissionRate(uint32_t ratePerSecond);

  /**
   * @brief Returns emission rate per second
   *
   * @return Value of emission rate
   */
  [[nodiscard]] uint32_t GetEmissionRate() const;

  /**
   * @brief number of particles to be emitted on start of the emitter
   *
   * @param[in] count Initial count of particles in the system
   */
  void SetInitialParticleCount(uint32_t count);

  /**
   * @brief returns number of particles being emitted on start of the emitter
   *
   * @return number of particles
   */
  [[nodiscard]] uint32_t GetInitialParticleCount() const;

  /**
   * @brief Sets maximum number of particles alive
   *
   * This function limits number of active particles in the system.
   *
   * If set to 0, there is no limit.
   *
   * If set to non-zero, if the system reaches limit, no new particles
   * will spawn until some of them die.
   *
   * @param[in] count Limit of active particles
   */
  void SetActiveParticlesLimit(uint32_t count);

  /**
   * @brief Returns active particles limit
   *
   * @return Number of active particles system is limited to
   */
  [[nodiscard]] uint32_t GetActiveParticlesLimit() const;

  /**
   * @brief Returns modifier in stack
   *
   * @param[in] index Index of modifier
   * @return Returns valid pointer to the modifier or nullptr
   */
  ParticleModifier GetModifierAt(uint32_t index);

  /**
   * @brief Removes modifier at specified index
   *
   * @param[in] index of modifier to be removed
   */
  void RemoveModifierAt(uint32_t index);

  /**
   * @brief Returns ParticleList object
   *
   * @return Valid reference to the ParticleList
   */
  ParticleList& GetParticleList();

  /**
   * @brief Starts emitting particles
   */
  void Start();

  /**
   * @brief Stops emitting particles
   */
  void Stop();

  /**
   * @brief Returns current emitter status
   *
   * @return Current emitter status
   */
  [[nodiscard]] Status GetStatus() const;

  /**
   * @brief Constructor
   */
  ParticleEmitter() = default;

private:
  /// @cond internal
  /**
   * @brief This constructor is used by ParticleEmitter::New() methods.
   *
   * @param [in] impl A pointer to a newly allocated implementation
   */
  ParticleEmitter(Dali::Toolkit::ParticleSystem::Internal::ParticleEmitter* impl);
  /// @endcond
};
} // namespace Dali::Toolkit::ParticleSystem

#endif // DALI_TOOLKIT_PARTICLE_SYSTEM_PARTICLE_EMITTER_H

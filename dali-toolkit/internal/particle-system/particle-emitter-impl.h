#ifndef DALI_TOOLKIT_PARTICLE_SYSTEM_INTERNAL_PARTICLE_EMITTER_H
#define DALI_TOOLKIT_PARTICLE_SYSTEM_INTERNAL_PARTICLE_EMITTER_H
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
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/object/base-object.h>
#include <chrono>
#include <ctime>
#include <memory>

// For multithreading update
#include <dali/devel-api/threading/thread-pool.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/particle-system/particle-domain.h>
#include <dali-toolkit/public-api/particle-system/particle-emitter.h>
#include <dali-toolkit/public-api/particle-system/particle-list.h>
#include <dali-toolkit/public-api/particle-system/particle-modifier.h>
#include <dali-toolkit/public-api/particle-system/particle-renderer.h>
#include <dali-toolkit/public-api/particle-system/particle-source.h>

namespace Dali::Toolkit::ParticleSystem::Internal
{
class FrameCallback;
class ParticleEmitter : public Dali::BaseObject, public Dali::ConnectionTracker
{
public:
  /**
   * @brief Constructor
   */
  ParticleEmitter();

  /**
   * @brief Destructor
   */
  ~ParticleEmitter() override = default;

  /**
   * @brief Tests whether emitter is complete (ready for simulation)
   *
   * @return True if emitter is complete, false otherwise
   */
  [[nodiscard]] bool IsComplete() const
  {
    return (mParticleStatusBits & STATUS_COMPLETE_BITS) == STATUS_COMPLETE_BITS;
  }

  [[nodiscard]] ParticleSystem::ParticleSource GetSource() const;

  void SetSource(const ParticleSystem::ParticleSource& source);

  [[nodiscard]] ParticleSystem::ParticleDomain GetDomain() const;

  void SetDomain(const ParticleSystem::ParticleDomain& domain);

  [[nodiscard]] ParticleSystem::ParticleRenderer GetRenderer() const;

  [[nodiscard]] ParticleSystem::ParticleModifier GetModifierAt(uint32_t index);

  void RemoveModifierAt(uint32_t index);

  void SetRenderer(const ParticleSystem::ParticleRenderer& renderer);

  void SetParticleCount(uint32_t maxParticleCount);

  ParticleSystem::ParticleList& GetParticleList();

  uint32_t AddModifier(const ParticleSystem::ParticleModifier& modifier);

  void AttachTo(Actor actor);

  [[nodiscard]] Actor GetActor() const;

  void Update();

  void UpdateSource(uint32_t count);

  void UpdateModifierMT(Dali::Toolkit::ParticleSystem::ParticleModifier& modifier);

  void UpdateDomain();

  void SetEmissionRate(uint32_t ratePerSecond);

  [[nodiscard]] uint32_t GetEmissionRate() const;

  void SetInitialParticleCount(uint32_t count);

  [[nodiscard]] uint32_t GetInitialParticleCount() const;

  void Start();

  void Stop();

  void EnableParallelProcessing(bool enabled);

  [[nodiscard]] bool IsParallelProcessingEnabled() const;

  void SetActiveParticlesLimit(uint32_t count);

  [[nodiscard]] uint32_t GetActiveParticlesLimit() const;

  [[nodiscard]] ParticleSystem::ParticleEmitter::Status GetStatus() const;

  [[nodiscard]] std::chrono::milliseconds GetCurrentTimeMillis() const;

  // All these bits must be set in order to consider emitter COMPLETE
  const uint32_t SOURCE_SET_STATUS_BIT   = 1 << 0;
  const uint32_t RENDERER_SET_STATUS_BIT = 1 << 1;
  const uint32_t DOMAIN_SET_STATUS_BIT   = 1 << 2;

  // 1. Only one of these flags can be set at a time
  // 2. They are invalid as long as emitter is INCOMPLETE
  const uint32_t SIMULATION_STARTED_STATUS_BIT = 1 << 3;
  const uint32_t SIMULATION_PAUSED_STATUS_BIT  = 1 << 4;
  const uint32_t SIMULATION_STOPPED_STATUS_BIT = 1 << 5;

  const uint32_t STATUS_COMPLETE_BITS = SOURCE_SET_STATUS_BIT | RENDERER_SET_STATUS_BIT | DOMAIN_SET_STATUS_BIT;

  ParticleSystem::ParticleSource mParticleSource; ///< Current particle source object
  ParticleSystem::ParticleDomain mParticleDomain; ///< Current particle domain object

  uint8_t mParticleStatusBits{0u}; ///< Current status of the emitter

  // List of particles
  ParticleSystem::ParticleList mParticleList;

  std::vector<ParticleSystem::ParticleModifier> mModifiers;

  ParticleSystem::ParticleRenderer mParticleRenderer;

  Actor mActor;

  uint32_t                  mEmissionRatePerSecond{1u};
  std::atomic<uint32_t>     mEmissionCountOnStart{0u};
  std::atomic<uint32_t>     mActiveParticlesLimit{0u}; ///< 0 - unlimited
  std::atomic<bool>         mSystemStarted{false};
  std::chrono::milliseconds mCurrentMilliseconds{0};
  std::chrono::milliseconds mLastUpdateMs{0};

  bool                           mParallelProcessing{false};
  std::unique_ptr<FrameCallback> mFrameCallback;
};

} // namespace Dali::Toolkit::ParticleSystem::Internal

namespace Dali::Toolkit::ParticleSystem
{
// Returns thread pool shared by whole particle system
Dali::ThreadPool& GetThreadPool();

inline Internal::ParticleEmitter& GetImplementation(ParticleSystem::ParticleEmitter& source)
{
  DALI_ASSERT_ALWAYS(source && "ParticleEmitter handle is empty");

  BaseObject& handle = source.GetBaseObject();

  return static_cast<Internal::ParticleEmitter&>(handle);
}

inline const Internal::ParticleEmitter& GetImplementation(const ParticleSystem::ParticleEmitter& source)
{
  DALI_ASSERT_ALWAYS(source && "ParticleEmitter handle is empty");

  const BaseObject& handle = source.GetBaseObject();

  return static_cast<const Internal::ParticleEmitter&>(handle);
}

} // namespace Dali::Toolkit::ParticleSystem
#endif // DALI_TOOLKIT_PARTICLE_SYSTEM_INTERNAL_PARTICLE_EMITTER_H
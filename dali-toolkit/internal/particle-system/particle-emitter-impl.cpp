/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/particle-system/particle-emitter-impl.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/particle-system/particle-list-impl.h>
#include <dali-toolkit/internal/particle-system/particle-modifier-impl.h>
#include <dali-toolkit/internal/particle-system/particle-renderer-impl.h>
#include <dali-toolkit/internal/particle-system/particle-source-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/common/stage-devel.h>
#include <dali/devel-api/update/frame-callback-interface.h>
#include <memory>
#include <utility>

namespace Dali::Toolkit::ParticleSystem::Internal
{
constexpr uint32_t DEFAULT_PARTICLE_COUNT = 100u; ///< Default number of particles in system if not set by user

/**
 * Particle system frame callback to run modifiers and sources
 */
class FrameCallback : public Dali::FrameCallbackInterface
{
public:
  /**
   * @brief Constructor.
   */
  FrameCallback(Internal::ParticleEmitter* emitter)
  : mEmitter(emitter)
  {
  }

  ~FrameCallback() = default;

private:
  bool Update(Dali::UpdateProxy& updateProxy, float elapsedSeconds) override
  {
    mEmitter->Update();
    return true;
  }

  Internal::ParticleEmitter* mEmitter;
};

ParticleSystem::ParticleSource ParticleEmitter::GetSource() const
{
  return mParticleSource;
}

void ParticleEmitter::SetSource(const ParticleSystem::ParticleSource& source)
{
  mParticleStatusBits |= SOURCE_SET_STATUS_BIT;
  mParticleSource = source;

  // call the init function of source
  GetImplementation(mParticleSource).GetUpdater().Init();
}

void ParticleEmitter::SetDomain(const ParticleSystem::ParticleDomain& domain)
{
  mParticleStatusBits |= DOMAIN_SET_STATUS_BIT;
  mParticleDomain = domain;
}

void ParticleEmitter::SetRenderer(const ParticleSystem::ParticleRenderer& renderer)
{
  mParticleStatusBits |= RENDERER_SET_STATUS_BIT;
  mParticleRenderer = renderer;
  GetImplementation(mParticleRenderer).SetEmitter(this);
}

void ParticleEmitter::SetParticleCount(uint32_t maxParticleCount)
{
  if(!mParticleList || maxParticleCount != GetImplementation(mParticleList).GetParticleCount())
  {
    // Default particle list has no data streams, it will replace old list
    mParticleList = ParticleSystem::ParticleList::New(maxParticleCount,
                                                      ParticleStream::POSITION_STREAM_BIT |
                                                        ParticleStream::COLOR_STREAM_BIT |
                                                        ParticleStream::VELOCITY_STREAM_BIT |
                                                        ParticleStream::SCALE_STREAM_BIT |
                                                        ParticleStream::LIFETIME_STREAM_BIT);
  }
}

uint32_t ParticleEmitter::GetParticleCount()
{
  return GetImplementation(mParticleList).GetParticleCount();
}

ParticleSystem::ParticleList& ParticleEmitter::GetParticleList()
{
  return mParticleList;
}

uint32_t ParticleEmitter::AddModifier(const ParticleSystem::ParticleModifier& modifier)
{
  mModifiers.emplace_back(modifier);
  return mModifiers.size() - 1;
}

ParticleSystem::ParticleDomain ParticleEmitter::GetDomain() const
{
  return mParticleDomain;
}

ParticleSystem::ParticleRenderer ParticleEmitter::GetRenderer() const
{
  return mParticleRenderer;
}

ParticleSystem::ParticleModifier ParticleEmitter::GetModifierAt(uint32_t index)
{
  return index < mModifiers.size() ? mModifiers[index] : ParticleSystem::ParticleModifier();
}

void ParticleEmitter::RemoveModifierAt(uint32_t index)
{
  if(DALI_LIKELY(index < mModifiers.size()))
  {
    mModifiers.erase(mModifiers.begin() + index);
  }
}

void ParticleEmitter::Start()
{
  if(mActor && IsComplete() && !(mParticleStatusBits & SIMULATION_STARTED_STATUS_BIT))
  {
    if(mFrameCallback)
    {
      Stop();
    }

    GetImplementation(mParticleRenderer).Initialize();

    mSystemStarted.store(1u, std::memory_order_relaxed);
    mParticleStatusBits &= ~(SIMULATION_STOPPED_STATUS_BIT | SIMULATION_PAUSED_STATUS_BIT);
    mParticleStatusBits |= SIMULATION_STARTED_STATUS_BIT;
    mFrameCallback = std::make_unique<FrameCallback>(this);

    // Attach renderer to an actor
    auto renderer = GetImplementation(mParticleRenderer).GetRenderer();
    mActor.AddRenderer(renderer);
    DevelStage::AddFrameCallback(Stage::GetCurrent(), *mFrameCallback, mActor);
  }
}

void ParticleEmitter::Stop()
{
  if(mActor && IsComplete() && (mParticleStatusBits & SIMULATION_STARTED_STATUS_BIT))
  {
    mSystemStarted.store(0u, std::memory_order_relaxed);
    mParticleStatusBits &= ~(SIMULATION_STARTED_STATUS_BIT | SIMULATION_PAUSED_STATUS_BIT);
    mParticleStatusBits |= SIMULATION_STOPPED_STATUS_BIT;
    auto renderer = GetImplementation(mParticleRenderer).GetRenderer();
    mActor.RemoveRenderer(renderer);
    DevelStage::RemoveFrameCallback(Stage::GetCurrent(), *mFrameCallback);
  }
}

std::chrono::milliseconds ParticleEmitter::GetCurrentTimeMillis() const
{
  std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::system_clock::now().time_since_epoch());
  return ms;
}

void ParticleEmitter::Update()
{
  auto currentUpdateMs = GetCurrentTimeMillis();

  if(mCurrentMilliseconds.count() == 0)
  {
    mCurrentMilliseconds = currentUpdateMs;
  }

  if(mLastUpdateMs.count() == 0)
  {
    mLastUpdateMs = currentUpdateMs;
  }

  // TODO : Make below codes as thread safe! See mSystemStarted implementation

  float emissionDelta = 1.0f / float(mEmissionRatePerSecond); // time per one particle emission (TODO: add some randomness to it)

  auto diffTime = double((currentUpdateMs - mCurrentMilliseconds).count()) / 1000.0;

  uint32_t emissionCount = 0u;
  if(diffTime >= emissionDelta)
  {
    emissionCount        = round(diffTime / emissionDelta);
    mCurrentMilliseconds = mCurrentMilliseconds + std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<double>(emissionCount * emissionDelta));
  }

  // Update lifetimes and discard dead particles
  const auto deltaMs = currentUpdateMs - mLastUpdateMs;

  mLastUpdateMs = currentUpdateMs;

  const float deltaSeconds = float(deltaMs.count()) / 1000.0f;
  if(deltaMs.count())
  {
    auto& particles = mParticleList.GetActiveParticles();

    static std::vector<uint32_t> toEraseIndices;

    uint32_t n = 0;
    for(auto& p : particles)
    {
      auto& lifetime = p.Get<float>(ParticleStream::LIFETIME_STREAM_BIT);
      lifetime -= deltaSeconds;
      if(lifetime <= 0.0f)
      {
        toEraseIndices.emplace_back(n);
      }
      ++n;
    }

    if(!toEraseIndices.empty())
    {
      GetImplementation(mParticleList).ReleaseParticles(toEraseIndices);
      toEraseIndices.clear();
    }
  }

  // apply initial emission count
  const bool isFirstFrameAfterStart = mSystemStarted.fetch_and(0u, std::memory_order_relaxed);
  if(isFirstFrameAfterStart)
  {
    emissionCount = mEmissionCountOnStart;
  }

  // Update source if there are any particles to be emitted
  if(emissionCount)
  {
    // Apply active particles limiter
    if(mActiveParticlesLimit && mParticleList.GetActiveParticleCount() + emissionCount > mActiveParticlesLimit)
    {
      emissionCount = mActiveParticlesLimit - mParticleList.GetActiveParticleCount();
    }
    UpdateSource(emissionCount);
  }

  // Update modifier stack
  for(auto& modifier : mModifiers)
  {
    if(modifier)
    {
      // Parallel processing must be enabled in order to use MT mode
      bool mt = GetImplementation(modifier).GetUpdater().IsMultiThreaded() && mParallelProcessing;

      if(!mt) // single-threaded, update all particles in one go
      {
        GetImplementation(modifier).Update(mParticleList, 0, mParticleList.GetActiveParticleCount());
      }
      else
      {
        UpdateModifierMT(modifier);
      }
    }
  }

  UpdateDomain();
}

void ParticleEmitter::AttachTo(Actor actor)
{
  mActor = std::move(actor);
}

Actor ParticleEmitter::GetActor() const
{
  return mActor;
}

void ParticleEmitter::UpdateSource(uint32_t count)
{
  GetImplementation(mParticleSource).Update(mParticleList, count);
}

void ParticleEmitter::UpdateModifierMT(Dali::Toolkit::ParticleSystem::ParticleModifier& modifier)
{
  auto& threadPool = GetThreadPool();

  static const auto workerThreads = threadPool.GetWorkerCount();

  const auto activeCount = mParticleList.GetActiveParticleCount();

  // at least 10 particles per worker thread (should be parametrized)
  // If less, continue ST
  if(activeCount < workerThreads * 10)
  {
    GetImplementation(modifier).Update(mParticleList, 0, activeCount);
    return;
  }

  const auto partial = activeCount / workerThreads;

  // make tasks
  struct UpdateTask
  {
    UpdateTask(Internal::ParticleModifier& modifier, ParticleSystem::ParticleList& list, uint32_t first, uint32_t count)
    : mModifier(modifier),
      mList(list),
      mFirst(first),
      mCount(count)
    {
    }

    Internal::ParticleModifier&   mModifier;
    ParticleSystem::ParticleList& mList;
    const uint32_t                mFirst;
    const uint32_t                mCount;

    void Update()
    {
      mModifier.Update(mList, mFirst, mCount);
    }
  };

  static std::vector<UpdateTask> updateTasks;
  updateTasks.reserve(workerThreads);

  static std::vector<Task> tasks;
  tasks.reserve(workerThreads);

  for(auto i = 0u; i < workerThreads; ++i)
  {
    const auto index = i * partial;
    const auto count = (i == workerThreads - 1) ? activeCount - index : partial;

    updateTasks.emplace_back(GetImplementation(modifier), mParticleList, index, count);
    tasks.emplace_back([&task = updateTasks.back()](uint32_t n)
                       { task.Update(); });
  }

  auto future = threadPool.SubmitTasks(tasks, 0);
  future->Wait();

  // clear tasks
  updateTasks.clear();
  tasks.clear();
}

void ParticleEmitter::UpdateDomain()
{
  // TODO
}

void ParticleEmitter::SetEmissionRate(uint32_t ratePerSecond)
{
  mEmissionRatePerSecond = ratePerSecond;
}

uint32_t ParticleEmitter::GetEmissionRate() const
{
  return mEmissionRatePerSecond;
}

void ParticleEmitter::EnableParallelProcessing(bool enabled)
{
  mParallelProcessing = enabled;
}

bool ParticleEmitter::IsParallelProcessingEnabled() const
{
  return mParallelProcessing;
}

void ParticleEmitter::SetInitialParticleCount(uint32_t count)
{
  mEmissionCountOnStart = count;
}

uint32_t ParticleEmitter::GetInitialParticleCount() const
{
  return mEmissionCountOnStart;
}

void ParticleEmitter::SetActiveParticlesLimit(uint32_t count)
{
  mActiveParticlesLimit = count;
}

uint32_t ParticleEmitter::GetActiveParticlesLimit() const
{
  return mActiveParticlesLimit;
}

ParticleSystem::ParticleEmitter::Status ParticleEmitter::GetStatus() const
{
  auto statusMask = SIMULATION_STARTED_STATUS_BIT | SIMULATION_PAUSED_STATUS_BIT | SIMULATION_STOPPED_STATUS_BIT;
  auto status     = (mParticleStatusBits & statusMask);

  if(status & SIMULATION_PAUSED_STATUS_BIT)
  {
    return ParticleSystem::ParticleEmitter::Status::PAUSED;
  }
  else if(status & SIMULATION_STOPPED_STATUS_BIT)
  {
    return ParticleSystem::ParticleEmitter::Status::STOPPED;
  }
  else if(status & SIMULATION_STARTED_STATUS_BIT)
  {
    return ParticleSystem::ParticleEmitter::Status::STARTED;
  }
  else
  {
    return !IsComplete() ? ParticleSystem::ParticleEmitter::Status::INCOMPLETE : ParticleSystem::ParticleEmitter::Status::READY;
  }
}

ParticleEmitter::ParticleEmitter()
{
  // Necessary to be called to initialize internal ParticleList
  SetParticleCount(DEFAULT_PARTICLE_COUNT);
}

ParticleEmitter::~ParticleEmitter()
{
  if(mParticleRenderer)
  {
    GetImplementation(mParticleRenderer).PrepareToDie();
  }
}

} // namespace Dali::Toolkit::ParticleSystem::Internal
namespace Dali::Toolkit::ParticleSystem
{
Dali::ThreadPool& GetThreadPool()
{
  static std::unique_ptr<Dali::ThreadPool> gThreadPool{nullptr};
  static std::once_flag                    onceFlag;

  // Intialize thread pool if not there yet, make sure it happens once and it's synchronized!,
  // NOTE: this function shouldn't be called from multiple thread anyway
  if(!gThreadPool)
  {
    std::call_once(onceFlag, [&threadPool = gThreadPool]
                   { threadPool = std::make_unique<Dali::ThreadPool>();
                     threadPool->Initialize(4u); });
  }

  return *gThreadPool;
}
} // namespace Dali::Toolkit::ParticleSystem
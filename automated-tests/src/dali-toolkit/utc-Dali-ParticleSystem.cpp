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

#include <dali-test-suite-utils.h>
#include <dali-toolkit/public-api/particle-system/particle-domain.h>
#include <dali-toolkit/public-api/particle-system/particle-emitter.h>
#include <dali-toolkit/public-api/particle-system/particle-list.h>
#include <dali-toolkit/public-api/particle-system/particle-modifier.h>
#include <dali-toolkit/public-api/particle-system/particle-renderer.h>
#include <dali-toolkit/public-api/particle-system/particle-source.h>
#include <dali-toolkit/public-api/particle-system/particle.h>

#include <dlfcn.h>

#include <future>

using namespace Dali;
using namespace Dali::Toolkit::ParticleSystem;

/**
 * Helper function to invoke next function in the call chain
 */
template<class R, class T, class... Args>
R InvokeNext(T* pObj, Args... args)
{
  Dl_info info;
  dladdr(__builtin_return_address(0), &info);
  using Func = R (*)(T*, Args...);
  auto sym   = (Func)(dlsym(RTLD_NEXT, info.dli_sname));
  return sym(pObj, args...);
}

static std::chrono::milliseconds currentTime(1u);
static void                      AdvanceTimeByMs(uint32_t ms)
{
  currentTime += std::chrono::milliseconds(ms);
}

namespace Dali::Toolkit::ParticleSystem::Internal
{
// Create fake time getter
struct ParticleEmitter
{
  [[nodiscard]] std::chrono::milliseconds GetCurrentTimeMillis() const;
};
std::chrono::milliseconds ParticleEmitter::GetCurrentTimeMillis() const
{
  [[maybe_unused]] auto value = InvokeNext<std::chrono::milliseconds>(this);
  return std::chrono::milliseconds(currentTime);
}
} // namespace Dali::Toolkit::ParticleSystem::Internal

using ParticleEmitterWrapper = Dali::Toolkit::ParticleSystem::Internal::ParticleEmitter;

Texture CreateTexture()
{
  Texture   texture   = Texture::New(Dali::TextureType::TEXTURE_2D, Dali::Pixel::RGBA8888, 100, 100);
  uint8_t*  data      = reinterpret_cast<uint8_t*>(malloc(100 * 100 * 4));
  PixelData pixelData = PixelData::New(data, 100 * 100 * 4, 100, 100, Pixel::Format::RGBA8888, PixelData::FREE);
  texture.Upload(pixelData);
  return texture;
}

/**
 * Test particle source
 */
class TestSource : public ParticleSourceInterface
{
public:
  TestSource(ParticleEmitter* emitter)
  : mEmitter(*emitter)
  {
  }

  void NewFrame()
  {
    mPromise = std::promise<uint32_t>();
    mFuture  = mPromise.get_future();
  }

  uint32_t Update(ParticleList& outList, uint32_t count) override
  {
    mPromise.set_value(count);
    auto i = count;
    while(--i)
    {
      outList.NewParticle(1.0f);
    }
    return count;
  }

  void Init() override
  {
    // calls initialized
    mInitialized = true;
  }

  bool                   mInitialized{false};
  std::future<uint32_t>  mFuture;
  std::promise<uint32_t> mPromise;
  ParticleEmitter&       mEmitter;
};

class TestSource2 : public ParticleSourceInterface
{
public:
  TestSource2(ParticleEmitter* emitter)
  : mEmitter(*emitter)
  {
  }

  void NewFrame()
  {
    mPromise = std::promise<uint32_t>();
    mFuture  = mPromise.get_future();
  }

  uint32_t Update(ParticleList& outList, uint32_t count) override
  {
    mPromise.set_value(count);

    while(count--)
    {
      auto particle = outList.NewParticle(1.0);
      if(!particle)
      {
        return 0u;
      }

      [[maybe_unused]] auto& pos = particle.GetByIndex<Vector3>(mStreamBasePos);

      [[maybe_unused]] auto& gpos = particle.Get<Vector3>(ParticleStream::POSITION_STREAM_BIT);
      [[maybe_unused]] auto& col  = particle.Get<Vector4>(ParticleStream::COLOR_STREAM_BIT);
      [[maybe_unused]] auto& vel  = particle.Get<Vector3>(ParticleStream::VELOCITY_STREAM_BIT);
      [[maybe_unused]] auto& sca  = particle.Get<Vector3>(ParticleStream::SCALE_STREAM_BIT);
      //auto& basePos = particle.Get<Vector3>(ParticleStream::SCALE_STREAM_BIT);
    }

    return count;
  }

  void Init() override
  {
    // calls initialized
    mStreamBasePos = mEmitter.GetParticleList().AddLocalStream<Vector3>(Vector3::ZERO);
    mInitialized   = true;
  }

  bool                   mInitialized{false};
  std::future<uint32_t>  mFuture;
  std::promise<uint32_t> mPromise;
  uint32_t               mStreamBasePos{0u};
  ParticleEmitter&       mEmitter;
};
/**
 * Sample of FlameModifier
 */
struct TestModifier : public ParticleModifierInterface
{
  void Update(ParticleList& particleList, uint32_t firstParticleIndex, uint32_t particleCount) override
  {
  }
};

struct TestModifierMT : public ParticleModifierInterface
{
  void Update(ParticleList& particleList, uint32_t firstParticleIndex, uint32_t particleCount) override
  {
  }

  bool IsMultiThreaded() override
  {
    return true;
  }
};

/**
 * Another modifier to test modifier stack
 */
struct TestModifier2 : public ParticleModifierInterface
{
  void Update(ParticleList& particleList, uint32_t firstParticleIndex, uint32_t particleCount) override
  {
  }
};

struct EmitterGroup
{
  ParticleEmitter  emitter;
  ParticleRenderer renderer;
  ParticleModifier modifier;
  ParticleSource   source;
};

// Helper function to create emitter (every test will be doing that)
template<class SOURCE, class MODIFIER>
ParticleEmitter CreateEmitter(EmitterGroup* output = nullptr)
{
  auto emitter = ParticleEmitter::New();

  bool result = (emitter != nullptr);
  DALI_TEST_EQUALS(result, true, TEST_LOCATION);

  // Create test source
  auto source = ParticleSource::New<SOURCE>(&emitter);

  {
    BaseHandle handle(source);
    auto       newHandle = ParticleSource::DownCast(handle);
    DALI_TEST_EQUALS(newHandle, source, TEST_LOCATION);
  }

  // Create test renderer
  auto renderer = ParticleRenderer::New();

  {
    BaseHandle handle(renderer);
    auto       newHandle = ParticleRenderer::DownCast(handle);
    DALI_TEST_EQUALS(newHandle, renderer, TEST_LOCATION);
  }

  // Create modifier
  auto modifier = ParticleModifier::New<MODIFIER>();

  {
    BaseHandle handle(modifier);
    auto       newHandle = ParticleModifier::DownCast(handle);
    DALI_TEST_EQUALS(newHandle, modifier, TEST_LOCATION);
  }

  auto domain = ParticleDomain::New();

  {
    BaseHandle handle(domain);
    auto       newHandle = ParticleDomain::DownCast(handle);
    DALI_TEST_EQUALS(newHandle, domain, TEST_LOCATION);
  }

  // Test emitter readiness
  auto ready = emitter.GetStatus();

  // Emitter should return status incomplete
  DALI_TEST_EQUALS(ready, ParticleEmitter::Status::INCOMPLETE, TEST_LOCATION);

  // Attach all components to the emitter
  emitter.SetSource(source);
  emitter.SetRenderer(renderer);
  emitter.AddModifier(modifier);
  emitter.SetDomain(domain);

  auto domain0   = emitter.GetDomain();
  auto renderer0 = emitter.GetRenderer();

  DALI_TEST_EQUALS(renderer0, renderer, TEST_LOCATION);
  DALI_TEST_EQUALS(domain0, domain, TEST_LOCATION);

  if(output)
  {
    output->emitter  = emitter;
    output->renderer = renderer;
    output->modifier = modifier;
    output->source   = source;
  }

  return emitter;
}

int UtcDaliParticleSystemEmitterNew(void)
{
  // create particle emitter
  auto emitter = ParticleEmitter::New();

  bool result = (emitter != nullptr);
  DALI_TEST_EQUALS(result, true, TEST_LOCATION);

  // Create test source
  auto source = ParticleSource::New<TestSource>(&emitter);

  // Create test renderer
  auto renderer = ParticleRenderer::New();

  // Create modifier
  auto modifier = ParticleModifier::New<TestModifier>();

  // Create domain
  auto domain = ParticleDomain::New();

  // Test emitter readiness
  auto ready = emitter.GetStatus();

  // Emitter should return status incomplete
  DALI_TEST_EQUALS(ready, ParticleEmitter::Status::INCOMPLETE, TEST_LOCATION);

  // Attach all components to the emitter
  emitter.SetSource(source);
  emitter.SetRenderer(renderer);
  emitter.AddModifier(modifier);
  emitter.SetDomain(domain);

  // test status again (domain is optional);
  ready = emitter.GetStatus();

  // Emitter should return status incomplete
  DALI_TEST_EQUALS(ready, ParticleEmitter::Status::READY, TEST_LOCATION);

  END_TEST;
}

int UtcDaliParticleSystemEmitterNew2(void)
{
  // create particle emitter
  auto emitter = ParticleEmitter::New();

  bool result = (emitter != nullptr);
  DALI_TEST_EQUALS(result, true, TEST_LOCATION);

  // Create test source
  auto source = ParticleSource::New<TestSource>(&emitter);

  // Create test renderer
  auto renderer = ParticleRenderer::New();

  // Create modifier
  auto modifier = ParticleModifier::New<TestModifier>();

  // Create domain
  auto domain = ParticleDomain::New();

  // Test emitter readiness
  auto ready = emitter.GetStatus();

  // Emitter should return status incomplete
  DALI_TEST_EQUALS(ready, ParticleEmitter::Status::INCOMPLETE, TEST_LOCATION);

  // Attach all components to the emitter
  emitter.SetSource(source);
  emitter.SetRenderer(renderer);
  emitter.AddModifier(modifier);
  emitter.SetDomain(domain);

  // test blending mode
  DALI_TEST_EQUALS(renderer.GetBlendingMode(), Dali::Toolkit::ParticleSystem::BlendingMode::DEFAULT, TEST_LOCATION);

  emitter.SetParticleCount(10000);
  DALI_TEST_EQUALS(emitter.GetParticleCount(), 10000, TEST_LOCATION);

  auto m = emitter.GetModifierAt(0);
  DALI_TEST_EQUALS(m != nullptr, true, TEST_LOCATION);

  m.GetModifierCallback();

  // test status again (domain is optional);
  ready = emitter.GetStatus();

  // Emitter should return status incomplete
  DALI_TEST_EQUALS(ready, ParticleEmitter::Status::READY, TEST_LOCATION);

  END_TEST;
}

int UtcDaliParticleSystemEmitterDefaultStreams(void)
{
  // create particle emitter
  auto emitter = ParticleEmitter::New();

  bool result = (emitter != nullptr);
  DALI_TEST_EQUALS(result, true, TEST_LOCATION);

  // Create test source
  auto source = ParticleSource::New<TestSource>(&emitter);

  // Create test renderer
  auto renderer = ParticleRenderer::New();

  // Create modifier
  auto modifier = ParticleModifier::New<TestModifier>();

  // Create domain
  auto domain = ParticleDomain::New();

  // Test emitter readiness
  auto ready = emitter.GetStatus();

  // Emitter should return status incomplete
  DALI_TEST_EQUALS(ready, ParticleEmitter::Status::INCOMPLETE, TEST_LOCATION);

  // Attach all components to the emitter
  emitter.SetSource(source);
  emitter.SetRenderer(renderer);
  emitter.AddModifier(modifier);
  emitter.SetDomain(domain);

  auto                  particleList   = emitter.GetParticleList();
  std::vector<uint32_t> expectedValues = {
    ParticleStream::POSITION_STREAM_BIT, 0, ParticleStream::COLOR_STREAM_BIT, 3, ParticleStream::VELOCITY_STREAM_BIT, 2, ParticleStream::SCALE_STREAM_BIT, 1, ParticleStream::LIFETIME_STREAM_BIT, 4, ParticleStream::LIFETIME_BASE_STREAM_BIT, 5};

  for(auto i = 0u; i < expectedValues.size(); i += 2)
  {
    auto index = particleList.GetDefaultStreamIndex(expectedValues[i]);
    DALI_TEST_EQUALS(index, expectedValues[i + 1], TEST_LOCATION);
  }

  // test status again (domain is optional);
  ready = emitter.GetStatus();

  // Emitter should return status incomplete
  DALI_TEST_EQUALS(ready, ParticleEmitter::Status::READY, TEST_LOCATION);

  END_TEST;
}

int UtcDaliParticleSystemEmitterModifierStack(void)
{
  // create particle emitter
  auto emitter = ParticleEmitter::New();

  bool result = (emitter != nullptr);
  DALI_TEST_EQUALS(result, true, TEST_LOCATION);

  // Create test source
  auto source = ParticleSource::New<TestSource>(&emitter);

  // Create test renderer
  auto renderer = ParticleRenderer::New();

  // Create modifier
  auto modifier0 = ParticleModifier::New<TestModifier>();
  auto modifier1 = ParticleModifier::New<TestModifier>();
  auto modifier2 = ParticleModifier::New<TestModifier>();

  // Create domain
  auto domain = ParticleDomain::New();

  // Test emitter readiness
  auto ready = emitter.GetStatus();

  // Emitter should return status incomplete
  DALI_TEST_EQUALS(ready, ParticleEmitter::Status::INCOMPLETE, TEST_LOCATION);

  // Attach all components to the emitter
  emitter.SetSource(source);
  emitter.SetRenderer(renderer);
  emitter.AddModifier(modifier0);
  emitter.AddModifier(modifier1);
  emitter.AddModifier(modifier2);

  emitter.SetDomain(domain);

  // test status again (domain is optional);
  ready = emitter.GetStatus();

  // Emitter should return status incomplete
  DALI_TEST_EQUALS(ready, ParticleEmitter::Status::READY, TEST_LOCATION);

  auto modifier = emitter.GetModifierAt(1);
  DALI_TEST_EQUALS(modifier, modifier1, TEST_LOCATION);

  emitter.RemoveModifierAt(0);
  modifier = emitter.GetModifierAt(0);
  DALI_TEST_EQUALS(modifier, modifier1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliParticleSystemTest(void)
{
  TestApplication application;

  // Create actor to be used with emitter
  Actor actor = Actor::New();
  application.GetScene().Add(actor);
  actor.SetProperty(Actor::Property::SIZE, Vector2(100, 100));

  auto emitter = CreateEmitter<TestSource, TestModifier>();

  // test status again (domain is optional);
  auto ready = emitter.GetStatus();

  // Emitter should return status incomplete
  DALI_TEST_EQUALS(ready, ParticleEmitter::Status::READY, TEST_LOCATION);

  // Set initial parameters of system
  emitter.SetInitialParticleCount(1000);
  emitter.SetActiveParticlesLimit(5000);

  // Test getters
  auto initialParticleCount = emitter.GetInitialParticleCount();
  auto activeParticlesLimit = emitter.GetActiveParticlesLimit();

  DALI_TEST_EQUALS(initialParticleCount, 1000, TEST_LOCATION);
  DALI_TEST_EQUALS(activeParticlesLimit, 5000, TEST_LOCATION);

  // Attach emitter to actor
  emitter.AttachTo(actor);

  // Start emitter
  emitter.Start();

  auto status = emitter.GetStatus();
  DALI_TEST_EQUALS(status, ParticleEmitter::Status::STARTED, TEST_LOCATION);

  auto& sourceCallback = dynamic_cast<TestSource&>(emitter.GetSource().GetSourceCallback());

  // Run simulation
  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  // First call into source callback should emit initial number of particles
  auto emittedParticleCount = sourceCallback.mFuture.get();
  DALI_TEST_EQUALS(emittedParticleCount, 1000, TEST_LOCATION);

  // Run 3 more frames advancing by 1000ms which should
  // emit particles based on emission rate
  AdvanceTimeByMs(1000);

  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  AdvanceTimeByMs(1000);

  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  AdvanceTimeByMs(1000);

  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliParticleSystemTestWithTextureScreen(void)
{
  TestApplication application;

  // Create actor to be used with emitter
  Actor actor = Actor::New();
  application.GetScene().Add(actor);
  actor.SetProperty(Actor::Property::SIZE, Vector2(100, 100));

  EmitterGroup group;

  auto emitter = CreateEmitter<TestSource, TestModifier>(&group);

  // Blending mode with screen
  auto texture = CreateTexture();
  group.renderer.SetTexture(texture);
  group.renderer.SetBlendingMode(BlendingMode::SCREEN);

  // test status again (domain is optional);
  auto ready = emitter.GetStatus();

  // Emitter should return status incomplete
  DALI_TEST_EQUALS(ready, ParticleEmitter::Status::READY, TEST_LOCATION);

  // Set initial parameters of system
  emitter.SetInitialParticleCount(1000);
  emitter.SetActiveParticlesLimit(5000);

  // Test getters
  auto initialParticleCount = emitter.GetInitialParticleCount();
  auto activeParticlesLimit = emitter.GetActiveParticlesLimit();

  DALI_TEST_EQUALS(initialParticleCount, 1000, TEST_LOCATION);
  DALI_TEST_EQUALS(activeParticlesLimit, 5000, TEST_LOCATION);

  // Attach emitter to actor
  emitter.AttachTo(actor);

  // Start emitter
  emitter.Start();

  auto status = emitter.GetStatus();
  DALI_TEST_EQUALS(status, ParticleEmitter::Status::STARTED, TEST_LOCATION);

  auto& sourceCallback = dynamic_cast<TestSource&>(emitter.GetSource().GetSourceCallback());

  // Run simulation
  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  // First call into source callback should emit initial number of particles
  auto emittedParticleCount = sourceCallback.mFuture.get();
  DALI_TEST_EQUALS(emittedParticleCount, 1000, TEST_LOCATION);

  // Run 3 more frames advancing by 1000ms which should
  // emit particles based on emission rate
  AdvanceTimeByMs(1000);

  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  AdvanceTimeByMs(1000);

  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  AdvanceTimeByMs(1000);

  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliParticleSystemTestWithTextureAdd(void)
{
  TestApplication application;

  // Create actor to be used with emitter
  Actor actor = Actor::New();
  application.GetScene().Add(actor);
  actor.SetProperty(Actor::Property::SIZE, Vector2(100, 100));

  EmitterGroup group;

  auto emitter = CreateEmitter<TestSource, TestModifier>(&group);

  // Blending mode with screen
  auto texture = CreateTexture();
  group.renderer.SetTexture(texture);
  group.renderer.SetBlendingMode(BlendingMode::DEFAULT);

  // test status again (domain is optional);
  auto ready = emitter.GetStatus();

  // Emitter should return status incomplete
  DALI_TEST_EQUALS(ready, ParticleEmitter::Status::READY, TEST_LOCATION);

  // Set initial parameters of system
  emitter.SetInitialParticleCount(1000);
  emitter.SetActiveParticlesLimit(5000);

  // Test getters
  auto initialParticleCount = emitter.GetInitialParticleCount();
  auto activeParticlesLimit = emitter.GetActiveParticlesLimit();

  DALI_TEST_EQUALS(initialParticleCount, 1000, TEST_LOCATION);
  DALI_TEST_EQUALS(activeParticlesLimit, 5000, TEST_LOCATION);

  // Attach emitter to actor
  emitter.AttachTo(actor);

  // Start emitter
  emitter.Start();

  auto status = emitter.GetStatus();
  DALI_TEST_EQUALS(status, ParticleEmitter::Status::STARTED, TEST_LOCATION);

  auto& sourceCallback = dynamic_cast<TestSource&>(emitter.GetSource().GetSourceCallback());

  // Run simulation
  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  // First call into source callback should emit initial number of particles
  auto emittedParticleCount = sourceCallback.mFuture.get();
  DALI_TEST_EQUALS(emittedParticleCount, 1000, TEST_LOCATION);

  // Run 3 more frames advancing by 1000ms which should
  // emit particles based on emission rate
  AdvanceTimeByMs(1000);

  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  AdvanceTimeByMs(1000);

  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  AdvanceTimeByMs(1000);

  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliParticleSystemTestInitialSetup(void)
{
  TestApplication application;

  // Create actor to be used with emitter
  Actor actor = Actor::New();
  application.GetScene().Add(actor);
  actor.SetProperty(Actor::Property::SIZE, Vector2(100, 100));

  EmitterGroup group;

  auto emitter = CreateEmitter<TestSource, TestModifier>(&group);

  emitter.SetEmissionRate(1000);
  emitter.SetInitialParticleCount(1000);
  emitter.SetActiveParticlesLimit(10000);

  auto emissionRate = emitter.GetEmissionRate();
  auto initialCount = emitter.GetInitialParticleCount();
  auto activeCount  = emitter.GetActiveParticlesLimit();

  DALI_TEST_EQUALS(emissionRate, 1000, TEST_LOCATION);
  DALI_TEST_EQUALS(initialCount, 1000, TEST_LOCATION);
  DALI_TEST_EQUALS(activeCount, 10000, TEST_LOCATION);

  // Blending mode with screen
  auto texture = CreateTexture();
  group.renderer.SetTexture(texture);
  group.renderer.SetBlendingMode(BlendingMode::DEFAULT);

  // test status again (domain is optional);
  auto ready = emitter.GetStatus();

  // Emitter should return status incomplete
  DALI_TEST_EQUALS(ready, ParticleEmitter::Status::READY, TEST_LOCATION);

  // Set initial parameters of system
  emitter.SetInitialParticleCount(1000);
  emitter.SetActiveParticlesLimit(5000);

  // Test getters
  auto initialParticleCount = emitter.GetInitialParticleCount();
  auto activeParticlesLimit = emitter.GetActiveParticlesLimit();

  DALI_TEST_EQUALS(initialParticleCount, 1000, TEST_LOCATION);
  DALI_TEST_EQUALS(activeParticlesLimit, 5000, TEST_LOCATION);

  // Attach emitter to actor
  emitter.AttachTo(actor);

  // Start emitter
  emitter.Start();

  auto status = emitter.GetStatus();
  DALI_TEST_EQUALS(status, ParticleEmitter::Status::STARTED, TEST_LOCATION);

  auto& sourceCallback = dynamic_cast<TestSource&>(emitter.GetSource().GetSourceCallback());

  // Run simulation
  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  // First call into source callback should emit initial number of particles
  auto emittedParticleCount = sourceCallback.mFuture.get();
  DALI_TEST_EQUALS(emittedParticleCount, 1000, TEST_LOCATION);

  // Run 3 more frames advancing by 1000ms which should
  // emit particles based on emission rate
  AdvanceTimeByMs(1000);

  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  AdvanceTimeByMs(1000);

  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  AdvanceTimeByMs(1000);

  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliParticleSystemTestMT(void)
{
  TestApplication application;

  // Create actor to be used with emitter
  Actor actor = Actor::New();
  application.GetScene().Add(actor);
  actor.SetProperty(Actor::Property::SIZE, Vector2(100, 100));

  EmitterGroup group;

  auto emitter = CreateEmitter<TestSource, TestModifierMT>(&group);

  emitter.SetEmissionRate(10000);
  emitter.SetInitialParticleCount(10000);
  emitter.SetActiveParticlesLimit(20000);
  emitter.SetParticleCount(300000);

  auto emissionRate = emitter.GetEmissionRate();
  auto initialCount = emitter.GetInitialParticleCount();
  auto activeCount  = emitter.GetActiveParticlesLimit();

  DALI_TEST_EQUALS(emissionRate, 10000, TEST_LOCATION);
  DALI_TEST_EQUALS(initialCount, 10000, TEST_LOCATION);
  DALI_TEST_EQUALS(activeCount, 20000, TEST_LOCATION);

  emitter.EnableParallelProcessing(true);

  auto mtEnabled = emitter.IsParallelProcessingEnabled();
  DALI_TEST_EQUALS(mtEnabled, true, TEST_LOCATION);

  // Blending mode with screen
  auto texture = CreateTexture();
  group.renderer.SetTexture(texture);
  group.renderer.SetBlendingMode(BlendingMode::DEFAULT);

  // test status again (domain is optional);
  auto ready = emitter.GetStatus();

  // Emitter should return status incomplete
  DALI_TEST_EQUALS(ready, ParticleEmitter::Status::READY, TEST_LOCATION);

  // Attach emitter to actor
  emitter.AttachTo(actor);

  // Start emitter
  emitter.Start();

  auto status = emitter.GetStatus();
  DALI_TEST_EQUALS(status, ParticleEmitter::Status::STARTED, TEST_LOCATION);

  auto& sourceCallback = dynamic_cast<TestSource&>(emitter.GetSource().GetSourceCallback());

  // Run simulation
  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  // First call into source callback should emit initial number of particles
  auto emittedParticleCount = sourceCallback.mFuture.get();
  DALI_TEST_EQUALS(emittedParticleCount, 10000, TEST_LOCATION);

  // Run 3 more frames advancing by 1000ms which should
  // emit particles based on emission rate
  AdvanceTimeByMs(1000);

  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  AdvanceTimeByMs(1000);

  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  AdvanceTimeByMs(1000);

  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliParticleSystemTestParticleSource(void)
{
  TestApplication application;

  // Create actor to be used with emitter
  Actor actor = Actor::New();
  application.GetScene().Add(actor);
  actor.SetProperty(Actor::Property::SIZE, Vector2(100, 100));

  EmitterGroup group;

  auto emitter = CreateEmitter<TestSource2, TestModifier>(&group);

  emitter.SetEmissionRate(1000);
  emitter.SetInitialParticleCount(1000);
  emitter.SetActiveParticlesLimit(10000);

  auto emissionRate = emitter.GetEmissionRate();
  auto initialCount = emitter.GetInitialParticleCount();
  auto activeCount  = emitter.GetActiveParticlesLimit();

  DALI_TEST_EQUALS(emissionRate, 1000, TEST_LOCATION);
  DALI_TEST_EQUALS(initialCount, 1000, TEST_LOCATION);
  DALI_TEST_EQUALS(activeCount, 10000, TEST_LOCATION);

  emitter.EnableParallelProcessing(true);

  // Blending mode with screen
  auto texture = CreateTexture();
  group.renderer.SetTexture(texture);
  group.renderer.SetBlendingMode(BlendingMode::DEFAULT);

  // test status again (domain is optional);
  auto ready = emitter.GetStatus();

  // Emitter should return status incomplete
  DALI_TEST_EQUALS(ready, ParticleEmitter::Status::READY, TEST_LOCATION);

  // Set initial parameters of system
  emitter.SetInitialParticleCount(1000);
  emitter.SetActiveParticlesLimit(5000);

  // Test getters
  auto initialParticleCount = emitter.GetInitialParticleCount();
  auto activeParticlesLimit = emitter.GetActiveParticlesLimit();

  DALI_TEST_EQUALS(initialParticleCount, 1000, TEST_LOCATION);
  DALI_TEST_EQUALS(activeParticlesLimit, 5000, TEST_LOCATION);

  // Attach emitter to actor
  emitter.AttachTo(actor);

  // Start emitter
  emitter.Start();

  auto status = emitter.GetStatus();
  DALI_TEST_EQUALS(status, ParticleEmitter::Status::STARTED, TEST_LOCATION);

  auto& sourceCallback = static_cast<TestSource2&>(emitter.GetSource().GetSourceCallback());

  // Run simulation
  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  // First call into source callback should emit initial number of particles
  auto emittedParticleCount = sourceCallback.mFuture.get();
  DALI_TEST_EQUALS(emittedParticleCount, 1000, TEST_LOCATION);

  // Run 3 more frames advancing by 1000ms which should
  // emit particles based on emission rate
  AdvanceTimeByMs(1000);

  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  AdvanceTimeByMs(1000);

  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  AdvanceTimeByMs(1000);

  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  // Stop the emitter
  emitter.Stop();

  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliParticleSystemReplaceEmitter(void)
{
  TestApplication application;

  // Create actor to be used with emitter
  Actor actor = Actor::New();
  application.GetScene().Add(actor);
  actor.SetProperty(Actor::Property::SIZE, Vector2(100, 100));

  EmitterGroup group;

  auto emitter = CreateEmitter<TestSource, TestModifier>(&group);

  // Blending mode with screen
  auto texture = CreateTexture();
  group.renderer.SetTexture(texture);
  group.renderer.SetBlendingMode(BlendingMode::DEFAULT);

  // test status again (domain is optional);
  auto ready = emitter.GetStatus();

  // Emitter should return status incomplete
  DALI_TEST_EQUALS(ready, ParticleEmitter::Status::READY, TEST_LOCATION);

  // Set initial parameters of system
  emitter.SetInitialParticleCount(1000);
  emitter.SetActiveParticlesLimit(5000);

  // Test getters
  auto initialParticleCount = emitter.GetInitialParticleCount();
  auto activeParticlesLimit = emitter.GetActiveParticlesLimit();

  DALI_TEST_EQUALS(initialParticleCount, 1000, TEST_LOCATION);
  DALI_TEST_EQUALS(activeParticlesLimit, 5000, TEST_LOCATION);

  // Attach emitter to actor
  emitter.AttachTo(actor);

  // Start emitter
  emitter.Start();

  auto status = emitter.GetStatus();
  DALI_TEST_EQUALS(status, ParticleEmitter::Status::STARTED, TEST_LOCATION);

  auto& sourceCallback = dynamic_cast<TestSource&>(emitter.GetSource().GetSourceCallback());

  // Run simulation
  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  // First call into source callback should emit initial number of particles
  auto emittedParticleCount = sourceCallback.mFuture.get();
  DALI_TEST_EQUALS(emittedParticleCount, 1000, TEST_LOCATION);

  // Run 3 more frames advancing by 1000ms which should
  // emit particles based on emission rate
  AdvanceTimeByMs(1000);

  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  AdvanceTimeByMs(1000);

  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  AdvanceTimeByMs(1000);

  sourceCallback.NewFrame();
  application.SendNotification();
  application.Render();

  // replace emitter
  auto                  oldEmitter = emitter.GetObjectPtr(); // store old emitter
  [[maybe_unused]] auto i          = oldEmitter->ReferenceCount();
  // Reset group
  group = {};

  emitter = CreateEmitter<TestSource, TestModifier>(&group);
  DALI_TEST_EQUALS(bool(emitter.GetObjectPtr() != oldEmitter), true, TEST_LOCATION);

  END_TEST;
}
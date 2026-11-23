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

// EXTERNAL INCLUDES
#include <dali-toolkit-test-suite-utils.h>
#include <dali.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <toolkit-adaptor-impl.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/gradient/conic-gradient.h>
#include <dali-toolkit/internal/visuals/gradient/gradient-visual.h>
#include <dali-toolkit/internal/visuals/gradient/linear-gradient.h>
#include <dali-toolkit/internal/visuals/gradient/radial-gradient.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>

using namespace Dali;
namespace
{
namespace Impl = Dali::Toolkit::Internal;
using Cache    = Impl::GradientTextureCache;
/** Restores the mock adaptor flag even when a test assertion returns early. */
struct AdaptorUnavailableScope
{
  AdaptorUnavailableScope()
  {
    Test::ToolkitAdaptor::SetAdaptorAvailableForce(false);
  }
  ~AdaptorUnavailableScope()
  {
    Test::ToolkitAdaptor::SetAdaptorAvailableForce(true);
  }
};

IntrusivePtr<Impl::Gradient> MakeGradient(float first = 0.f, float last = 1.f, Vector4 color = Color::RED)
{
  IntrusivePtr<Impl::Gradient> gradient = Impl::LinearGradient::New(Vector2::ZERO, Vector2::ONE, nullptr);
  Vector<float>                offsets;
  offsets.PushBack(first);
  offsets.PushBack(last);
  Vector<Vector4> colors;
  colors.PushBack(color);
  colors.PushBack(Color::BLUE);
  gradient->ApplyStops(offsets, colors);
  return gradient;
}
Property::Map MakeProperties()
{
  Property::Map map;
  map.Insert(Toolkit::GradientVisual::Property::START_POSITION, Vector2::ZERO);
  map.Insert(Toolkit::GradientVisual::Property::END_POSITION, Vector2::ONE);
  Property::Array offsets;
  offsets.PushBack(0.f);
  offsets.PushBack(1.f);
  Property::Array colors;
  colors.PushBack(Color::RED);
  colors.PushBack(Color::BLUE);
  map.Insert(Toolkit::GradientVisual::Property::STOP_OFFSET, offsets);
  map.Insert(Toolkit::GradientVisual::Property::STOP_COLOR, colors);
  return map;
}
} //namespace

int UtcDaliGradientTextureCacheReferences(void)
{
  ToolkitTestApplication application;
  Cache                  cache;
  auto                   first  = MakeGradient();
  auto                   second = MakeGradient();
  const auto             id     = cache.Acquire(*first);
  DALI_TEST_CHECK(id != Cache::INVALID_ID);
  DALI_TEST_EQUALS(Cache::INVALID_ID, -1, TEST_LOCATION);
  DALI_TEST_EQUALS(id, cache.Acquire(*second), TEST_LOCATION);
  Texture original = cache.GetTexture(id);
  DALI_TEST_CHECK(original);
  DALI_TEST_EQUALS(original.GetWidth(), 512u, TEST_LOCATION);
  DALI_TEST_EQUALS(original.GetHeight(), 1u, TEST_LOCATION);
  cache.RequestRelease(id);
  DALI_TEST_CHECK(cache.GetTexture(id) == original);
  cache.RequestRelease(id);
  DALI_TEST_CHECK(cache.GetTexture(id));
  application.SendNotification();
  DALI_TEST_CHECK(!cache.GetTexture(id));
  cache.RequestRelease(id);
  cache.RequestRelease(Cache::INVALID_ID);
  const auto newId = cache.Acquire(*first);
  DALI_TEST_EQUALS(newId, id, TEST_LOCATION);
  DALI_TEST_CHECK(cache.GetTexture(newId) != original);
  cache.RequestRelease(newId);
  IntrusivePtr<Impl::Gradient> empty = Impl::LinearGradient::New(Vector2::ZERO, Vector2::ONE, nullptr);
  DALI_TEST_EQUALS(cache.Acquire(*empty), Cache::INVALID_ID, TEST_LOCATION);
  DALI_TEST_CHECK(!empty->GenerateLookupTexture());
  END_TEST;
}

int UtcDaliGradientTextureCacheDifferentGradientTypes(void)
{
  ToolkitTestApplication       application;
  Cache                        cache;
  IntrusivePtr<Impl::Gradient> linear = Impl::LinearGradient::New(Vector2::ZERO, Vector2::ONE, nullptr);
  IntrusivePtr<Impl::Gradient> radial = Impl::RadialGradient::New(Vector2(0.25f, 0.75f), 0.5f, nullptr);
  IntrusivePtr<Impl::Gradient> conic  = Impl::ConicGradient::New(Vector2(0.5f, 0.5f), Radian(0.75f), nullptr);

  Vector<float> offsets;
  offsets.PushBack(0.f);
  offsets.PushBack(0.4f);
  offsets.PushBack(1.f);
  Vector<Vector4> colors;
  colors.PushBack(Color::RED);
  colors.PushBack(Vector4(0.f, 1.f, 0.f, 0.5f));
  colors.PushBack(Color::BLUE);
  for(const auto& gradient : {linear, radial, conic})
  {
    gradient->ApplyStops(offsets, colors);
  }

  const auto linearId = cache.Acquire(*linear);
  const auto radialId = cache.Acquire(*radial);
  const auto conicId  = cache.Acquire(*conic);
  DALI_TEST_CHECK(linearId != Cache::INVALID_ID);
  DALI_TEST_EQUALS(radialId, linearId, TEST_LOCATION);
  DALI_TEST_EQUALS(conicId, linearId, TEST_LOCATION);
  const Texture texture = cache.GetTexture(linearId);
  DALI_TEST_CHECK(texture);
  DALI_TEST_CHECK(cache.GetTexture(radialId) == texture);
  DALI_TEST_CHECK(cache.GetTexture(conicId) == texture);

  // Each gradient owns one reference to the same lookup texture.
  cache.RequestRelease(linearId);
  application.SendNotification();
  DALI_TEST_CHECK(cache.GetTexture(radialId) == texture);
  cache.RequestRelease(radialId);
  application.SendNotification();
  DALI_TEST_CHECK(cache.GetTexture(conicId) == texture);
  cache.RequestRelease(conicId);
  application.SendNotification();
  DALI_TEST_CHECK(!cache.GetTexture(linearId));
  END_TEST;
}

int UtcDaliGradientTextureCacheCollisions(void)
{
  ToolkitTestApplication application;
  Cache                  cache;
  auto                   first           = MakeGradient(0.25f, 0.75f, Vector4(0.5f, 0.f, 0.f, 1.f));
  auto                   offsetCollision = MakeGradient(0.250001f, 0.75f, Vector4(0.5f, 0.f, 0.f, 1.f));
  auto                   colorCollision  = MakeGradient(0.25f, 0.75f, Vector4(0.50001f, 0.f, 0.f, 1.f));
  const auto             a               = cache.Acquire(*first);
  const auto             b               = cache.Acquire(*offsetCollision);
  const auto             c               = cache.Acquire(*colorCollision);
  DALI_TEST_CHECK(a != b && a != c && b != c);
  DALI_TEST_EQUALS(cache.Acquire(*first), a, TEST_LOCATION);
  DALI_TEST_EQUALS(cache.Acquire(*offsetCollision), b, TEST_LOCATION);
  DALI_TEST_EQUALS(cache.Acquire(*colorCollision), c, TEST_LOCATION);
  DALI_TEST_CHECK(cache.GetTexture(a) != cache.GetTexture(b));
  cache.RequestRelease(a);
  cache.RequestRelease(b);
  cache.RequestRelease(c);
  cache.RequestRelease(b);
  application.SendNotification();
  DALI_TEST_CHECK(!cache.GetTexture(b));
  DALI_TEST_CHECK(cache.GetTexture(a));
  DALI_TEST_CHECK(cache.GetTexture(c));
  cache.RequestRelease(a);
  cache.RequestRelease(c);
  END_TEST;
}

int UtcDaliGradientTextureCacheSpread(void)
{
  ToolkitTestApplication application;
  Cache                  cache;
  auto                   gradient = MakeGradient(0.2f, 0.8f, Vector4(1.f, 0.f, 0.f, 0.5f));
  const auto             pad      = cache.Acquire(*gradient);
  gradient->SetSpreadMethod(Toolkit::GradientVisual::SpreadMethod::REFLECT);
  DALI_TEST_EQUALS(cache.Acquire(*gradient), pad, TEST_LOCATION);
  gradient->SetSpreadMethod(Toolkit::GradientVisual::SpreadMethod::REPEAT);
  const auto repeat = cache.Acquire(*gradient);
  DALI_TEST_CHECK(repeat != pad);
  DALI_TEST_EQUALS(cache.GetTexture(repeat).GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION);
  cache.RequestRelease(pad);
  cache.RequestRelease(pad);
  cache.RequestRelease(repeat);
  gradient            = MakeGradient();
  const auto complete = cache.Acquire(*gradient);
  gradient->SetSpreadMethod(Toolkit::GradientVisual::SpreadMethod::REPEAT);
  DALI_TEST_EQUALS(cache.Acquire(*gradient), complete, TEST_LOCATION);
  cache.RequestRelease(complete);
  cache.RequestRelease(complete);
  END_TEST;
}

int UtcDaliGradientTextureCacheExplicitReferences(void)
{
  ToolkitTestApplication application;
  Cache                  cache;
  auto                   gradient = MakeGradient();
  const auto             id       = cache.Acquire(*gradient);
  cache.Reference(id);
  cache.Reference(Cache::INVALID_ID);
  cache.RequestRelease(id);
  application.SendNotification();
  DALI_TEST_CHECK(cache.GetTexture(id));
  cache.RequestRelease(id);
  application.SendNotification();
  DALI_TEST_CHECK(!cache.GetTexture(id));
  DALI_TEST_EQUALS(sizeof(Cache::Id), 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(sizeof(Cache), sizeof(void*), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGradientTextureCacheVisualUpdates(void)
{
  ToolkitTestApplication   application;
  Impl::VisualFactoryCache factory(false);
  auto                     properties = MakeProperties();
  auto                     first      = Impl::GradientVisual::New(factory, properties);
  auto                     second     = Impl::GradientVisual::New(factory, properties);
  Texture                  original   = first->GetRenderer().GetTextures().GetTexture(0u);
  DALI_TEST_CHECK(original == second->GetRenderer().GetTextures().GetTexture(0u));
  Property::Array colors;
  colors.PushBack(Color::GREEN);
  colors.PushBack(Color::BLUE);
  Property::Map update;
  update.Insert(Toolkit::GradientVisual::Property::STOP_COLOR, colors);
  first->SetProperties(update);
  DALI_TEST_CHECK(first->GetRenderer().GetTextures().GetTexture(0u) != original);
  DALI_TEST_CHECK(second->GetRenderer().GetTextures().GetTexture(0u) == original);
  first->SetProperties(properties);
  DALI_TEST_CHECK(first->GetRenderer().GetTextures().GetTexture(0u) == original);
  second.Reset();
  DALI_TEST_CHECK(first->GetRenderer().GetTextures().GetTexture(0u) == original);
  first.Reset();
  auto third = Impl::GradientVisual::New(factory, properties);
  DALI_TEST_CHECK(third->GetRenderer().GetTextures().GetTexture(0u) == original);
  application.SendNotification();
  third.Reset();
  application.SendNotification();
  third = Impl::GradientVisual::New(factory, properties);
  DALI_TEST_CHECK(third->GetRenderer().GetTextures().GetTexture(0u) != original);
  END_TEST;
}

int UtcDaliGradientTextureCacheAutomaticAndNarrowStops(void)
{
  ToolkitTestApplication       application;
  Cache                        cache;
  IntrusivePtr<Impl::Gradient> gradient = Impl::LinearGradient::New(Vector2::ZERO, Vector2::ONE, nullptr);
  Vector<float>                offsets;
  Vector<Vector4>              colors;
  colors.PushBack(Color::RED);
  colors.PushBack(Color::GREEN);
  colors.PushBack(Color::YELLOW);
  colors.PushBack(Color::BLUE);
  gradient->ApplyStops(offsets, colors);
  auto id = cache.Acquire(*gradient);
  DALI_TEST_CHECK(cache.GetTexture(id));
  DALI_TEST_EQUALS(gradient->GetStops()[1].mOffset, 1.f / 3.f, 0.00001f, TEST_LOCATION);
  DALI_TEST_EQUALS(gradient->GetStops()[2].mOffset, 2.f / 3.f, 0.00001f, TEST_LOCATION);
  cache.RequestRelease(id);
  offsets.PushBack(0.f);
  offsets.PushBack(1.f / 512.f);
  offsets.PushBack(1.f / 512.f);
  offsets.PushBack(1.f);
  gradient->ApplyStops(offsets, colors);
  id = cache.Acquire(*gradient);
  DALI_TEST_EQUALS(cache.GetTexture(id).GetPixelFormat(), Pixel::RGB888, TEST_LOCATION);
  cache.RequestRelease(id);
  for(auto pair : {Vector2(0.f, 0.8f), Vector2(0.2f, 1.f), Vector2(0.5f, 0.5f)})
  {
    gradient = MakeGradient(pair.x, pair.y);
    gradient->SetSpreadMethod(Toolkit::GradientVisual::SpreadMethod::REPEAT);
    id = cache.Acquire(*gradient);
    DALI_TEST_CHECK(cache.GetTexture(id));
    cache.RequestRelease(id);
  }
  END_TEST;
}

int UtcDaliGradientTextureCacheDestruction(void)
{
  ToolkitTestApplication application;
  Texture                texture;
  {
    Cache      cache;
    auto       gradient = MakeGradient();
    const auto id       = cache.Acquire(*gradient);
    texture             = cache.GetTexture(id);
    cache.RequestRelease(id);
  }
  // Destroying the cache cancels its pending post-processor. A Texture handle
  // independently retains its GPU resource; the cache itself is not retained.
  application.SendNotification();
  DALI_TEST_EQUALS(texture.GetWidth(), 512u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGradientTextureCacheDeferredRelease(void)
{
  ToolkitTestApplication application;
  Cache                  cache;
  auto                   gradient = MakeGradient();
  const auto             id       = cache.Acquire(*gradient);
  const Texture          original = cache.GetTexture(id);
  cache.RequestRelease(id);
  DALI_TEST_CHECK(cache.GetTexture(id) == original);
  DALI_TEST_EQUALS(cache.Acquire(*gradient), id, TEST_LOCATION);
  application.SendNotification();
  DALI_TEST_CHECK(cache.GetTexture(id) == original);
  cache.RequestRelease(id);
  application.SendNotification();
  DALI_TEST_CHECK(!cache.GetTexture(id));
  // Stale and invalid IDs must not register another removal processor.
  cache.RequestRelease(id);
  cache.RequestRelease(Cache::INVALID_ID);
  application.SendNotification();
  END_TEST;
}

int UtcDaliGradientNormalizeStopsInvalidation(void)
{
  ToolkitTestApplication application;
  auto                   gradient = MakeGradient(0.8f, 0.2f);
  gradient->NormalizeStops();
  DALI_TEST_EQUALS(gradient->GetStops()[0].mOffset, 0.2f, TEST_LOCATION);
  gradient->NormalizeStops();
  DALI_TEST_EQUALS(gradient->GetStops()[0].mStopColor, Color::BLUE, TEST_LOCATION);

  Vector<float> offsets;
  offsets.PushBack(1.f);
  offsets.PushBack(0.f);
  Vector<Vector4> colors;
  gradient->ApplyStops(offsets, colors);
  gradient->NormalizeStops();
  DALI_TEST_EQUALS(gradient->GetStops()[0].mOffset, 0.f, TEST_LOCATION);
  DALI_TEST_EQUALS(gradient->GetStops()[0].mStopColor, Color::RED, TEST_LOCATION);

  IntrusivePtr<Impl::Gradient> copy = Impl::LinearGradient::New(Vector2::ZERO, Vector2::ONE, gradient);
  copy->NormalizeStops();
  DALI_TEST_EQUALS(copy->GetStops()[0].mStopColor, Color::RED, TEST_LOCATION);
  copy->ClearStops();
  copy->NormalizeStops();
  copy->NormalizeStops();
  DALI_TEST_EQUALS(copy->GetStops().Count(), 0u, TEST_LOCATION);
  colors.PushBack(Color::GREEN);
  colors.PushBack(Color::BLUE);
  colors.PushBack(Color::WHITE);
  copy->ApplyStops(Vector<float>(), colors);
  copy->NormalizeStops();
  DALI_TEST_EQUALS(copy->GetStops()[1].mOffset, 0.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(copy->GetStops()[1].mStopColor, Color::BLUE, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGradientTextureCacheDenseRemoval(void)
{
  ToolkitTestApplication application;
  Cache                  cache;
  auto                   first         = MakeGradient(0.f, 1.f, Color::RED);
  auto                   middle        = MakeGradient(0.f, 1.f, Color::GREEN);
  auto                   last          = MakeGradient(0.f, 1.f, Color::WHITE);
  auto                   replacement   = MakeGradient(0.f, 1.f, Color::YELLOW);
  const auto             firstId       = cache.Acquire(*first);
  const auto             middleId      = cache.Acquire(*middle);
  const auto             lastId        = cache.Acquire(*last);
  Texture                middleTexture = cache.GetTexture(middleId);
  Texture                lastTexture   = cache.GetTexture(lastId);
  DALI_TEST_EQUALS(cache.Acquire(*middle), middleId, TEST_LOCATION);

  cache.RequestRelease(firstId);
  application.SendNotification();
  DALI_TEST_CHECK(!cache.GetTexture(firstId));
  DALI_TEST_CHECK(cache.GetTexture(lastId) == lastTexture);
  DALI_TEST_CHECK(cache.GetTexture(middleId) == middleTexture);
  DALI_TEST_EQUALS(cache.Acquire(*last), lastId, TEST_LOCATION);
  cache.Reference(lastId);

  cache.RequestRelease(middleId);
  cache.RequestRelease(middleId);
  application.SendNotification();
  DALI_TEST_CHECK(!cache.GetTexture(middleId));
  DALI_TEST_CHECK(cache.GetTexture(lastId) == lastTexture);
  cache.Reference(middleId);
  cache.RequestRelease(middleId);
  DALI_TEST_CHECK(!cache.GetTexture(123456));
  cache.Reference(123456);
  cache.RequestRelease(123456);

  const auto replacementId = cache.Acquire(*replacement);
  DALI_TEST_EQUALS(replacementId, middleId, TEST_LOCATION);
  Texture replacementTexture = cache.GetTexture(replacementId);
  DALI_TEST_CHECK(replacementTexture != middleTexture);
  // Removing the moved entry moves the replacement again, during one queue drain.
  cache.RequestRelease(lastId);
  cache.RequestRelease(lastId);
  cache.RequestRelease(lastId);
  cache.RequestRelease(replacementId);
  application.SendNotification();
  DALI_TEST_CHECK(!cache.GetTexture(lastId));
  DALI_TEST_CHECK(!cache.GetTexture(replacementId));
  const auto reusedId = cache.Acquire(*first);
  DALI_TEST_CHECK(cache.GetTexture(reusedId));
  DALI_TEST_EQUALS(cache.Acquire(*first), reusedId, TEST_LOCATION);
  cache.RequestRelease(reusedId);
  cache.RequestRelease(reusedId);
  application.SendNotification();
  DALI_TEST_CHECK(!cache.GetTexture(reusedId));
  END_TEST;
}

int UtcDaliGradientTextureCacheAdaptorUnavailable(void)
{
  ToolkitTestApplication application;
  Cache                  cache;
  auto                   first         = MakeGradient();
  auto                   second        = MakeGradient(0.f, 1.f, Color::GREEN);
  const auto             firstId       = cache.Acquire(*first);
  const auto             secondId      = cache.Acquire(*second);
  Texture                firstTexture  = cache.GetTexture(firstId);
  Texture                secondTexture = cache.GetTexture(secondId);
  {
    AdaptorUnavailableScope unavailable;
    DALI_TEST_CHECK(DALI_UNLIKELY(!Dali::Adaptor::IsAvailable()));
    DALI_TEST_EQUALS(cache.Acquire(*first), Cache::INVALID_ID, TEST_LOCATION);
    DALI_TEST_CHECK(!cache.GetTexture(firstId));
    DALI_TEST_CHECK(!Cache::GenerateTexture(*first));
    DALI_TEST_CHECK(!first->GenerateLookupTexture());
    cache.Reference(firstId);
    cache.RequestRelease(secondId);
  }
  application.SendNotification();
  DALI_TEST_CHECK(cache.GetTexture(firstId) == firstTexture);
  DALI_TEST_CHECK(cache.GetTexture(secondId) == secondTexture);
  cache.RequestRelease(firstId);
  application.SendNotification();
  DALI_TEST_CHECK(!cache.GetTexture(firstId));
  DALI_TEST_CHECK(cache.GetTexture(secondId));
  cache.RequestRelease(secondId);
  application.SendNotification();
  DALI_TEST_CHECK(!cache.GetTexture(secondId));
  END_TEST;
}

int UtcDaliGradientVisualAdaptorUnavailable(void)
{
  ToolkitTestApplication application;
  auto                   factory    = std::make_unique<Impl::VisualFactoryCache>(false);
  auto                   properties = MakeProperties();
  auto                   visual     = Impl::GradientVisual::New(*factory, properties);
  {
    AdaptorUnavailableScope unavailable;
    // A cached reference must not be dereferenced after its factory is destroyed.
    factory.reset();
    visual.Reset();
    DALI_TEST_CHECK(!visual);
  }
  END_TEST;
}

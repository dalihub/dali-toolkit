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
 */

// EXTERNAL INCLUDES
#include <type_traits>

// INTERNAL INCLUDES
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view.h>

using namespace Dali;
using namespace Dali::Toolkit;

static_assert(!std::is_default_constructible_v<DefaultRuler>);
static_assert(!std::is_copy_constructible_v<DefaultRuler>);
static_assert(!std::is_destructible_v<DefaultRuler>);
static_assert(!std::is_default_constructible_v<FixedRuler>);
static_assert(!std::is_constructible_v<FixedRuler, float>);
static_assert(!std::is_copy_constructible_v<FixedRuler>);
static_assert(!std::is_destructible_v<FixedRuler>);
static_assert(std::is_same_v<decltype(DefaultRuler::New()), IntrusivePtr<DefaultRuler>>);
static_assert(std::is_same_v<decltype(FixedRuler::New()), IntrusivePtr<FixedRuler>>);

int UtcDaliToolkitDefaultRulerNewOwnership(void)
{
  auto ruler = DefaultRuler::New();
  DALI_TEST_CHECK(ruler);
  DALI_TEST_EQUALS(ruler->ReferenceCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(ruler->Snap(27.0f, 0.5f), 27.0f, TEST_LOCATION);

  RulerPtr owner = ruler;
  DALI_TEST_EQUALS(ruler->ReferenceCount(), 2u, TEST_LOCATION);
  ruler.Reset();
  DALI_TEST_EQUALS(owner->ReferenceCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(owner->GetTotalPages(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliToolkitFixedRulerNewOwnership(void)
{
  auto ruler = FixedRuler::New(10.0f);
  DALI_TEST_CHECK(ruler);
  DALI_TEST_EQUALS(ruler->ReferenceCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(ruler->Snap(27.0f, 0.5f), 30.0f, TEST_LOCATION);

  RulerPtr owner = ruler;
  DALI_TEST_EQUALS(ruler->ReferenceCount(), 2u, TEST_LOCATION);
  ruler.Reset();
  DALI_TEST_EQUALS(owner->ReferenceCount(), 1u, TEST_LOCATION);

  auto defaultSpacing = FixedRuler::New();
  DALI_TEST_EQUALS(defaultSpacing->Snap(2.7f, 0.5f), 3.0f, TEST_LOCATION);
  END_TEST;
}

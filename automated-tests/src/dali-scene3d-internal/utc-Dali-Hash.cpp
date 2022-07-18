/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

// Enable debug log for test coverage
#define DEBUG_ENABLED 1

#include "dali-scene3d/internal/loader/hash.h"
#include <dali-test-suite-utils.h>
#include <string>

using namespace Dali;
using namespace Dali::Scene3D::Loader;

int UtcDaliHash(void)
{
  DALI_TEST_EQUAL(Hash(0).Add(true), 0);
  DALI_TEST_EQUAL(Hash(0).Add(false), 1);

  int32_t myInt32 = std::numeric_limits<int32_t>::min();
  DALI_TEST_EQUAL(Hash(0).Add(myInt32), myInt32);

  uint32_t myUint32 = std::numeric_limits<uint32_t>::max();
  DALI_TEST_EQUAL(Hash(0).Add(myUint32), myUint32);

  uint64_t myUint64 = std::numeric_limits<uint64_t>::max();
  DALI_TEST_EQUAL(uint64_t(Hash(0).Add(myUint64)), myUint64);

  constexpr uint32_t multiplier = 31;
  uint64_t expected = 0;
  float f = 1928.46852;
  for (auto i0 = reinterpret_cast<uint8_t const*>(&f), i1 = i0 + sizeof(f); i0 != i1; ++i0)
  {
    expected = expected * multiplier + *i0;
  }

  DALI_TEST_EQUAL(uint64_t(Hash(0).Add(f)), expected);

  END_TEST;
}


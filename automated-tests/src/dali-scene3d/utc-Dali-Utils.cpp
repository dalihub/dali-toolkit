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

#include "dali-scene3d/public-api/loader/utils.h"
#include <dali-test-suite-utils.h>
#include <string_view>

using namespace Dali;
using namespace Dali::Scene3D::Loader;

namespace
{
constexpr std::string_view TEST_STRING = "The quick brown fox jumped over a lazy dog.1337true";
}

template <size_t N>
void HelpTestStreamBuffer()
{
  char buffer[N];
  StreamBuffer streamBuffer(buffer, N); // note we aren't leaving space for a terminating 0 as we're using strNcmp.
  std::ostream stream(&streamBuffer);

  stream << "The quick brown fox jumped over a lazy dog.";
  stream << 1337;
  stream << std::boolalpha << true;

  const size_t checkLen = std::min(N, TEST_STRING.size());
  DALI_TEST_EQUAL(0, strncmp(buffer, TEST_STRING.data(), checkLen));
}

int UtcDaliUtilsStreamBuffer(void)
{
  HelpTestStreamBuffer<16>();
  HelpTestStreamBuffer<32>();
  HelpTestStreamBuffer<64>();
  END_TEST;
}

int UtcDaliUtilsFormatString(void)
{
  DALI_TEST_EQUAL(FormatString("%s", "hello"), "hello");
  DALI_TEST_EQUAL(FormatString("%d", 1667), "1667");

  DALI_TEST_EQUAL(FormatString("%s %d", "hello", 2778), "hello 2778");
  DALI_TEST_EQUAL(FormatString("%d %s", 3889, "hello"), "3889 hello");

  END_TEST;
}

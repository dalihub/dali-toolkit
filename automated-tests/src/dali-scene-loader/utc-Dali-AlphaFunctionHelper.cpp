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

#include "dali-scene-loader/public-api/alpha-function-helper.h"
#include <dali-test-suite-utils.h>

using namespace Dali;
using namespace Dali::SceneLoader;

#define ALPHA_FN_PAIR(x) { #x, AlphaFunction::x }

const std::pair<std::string, AlphaFunction::BuiltinFunction> BUILTIN_FUNCTIONS[] {
  ALPHA_FN_PAIR(DEFAULT),
  ALPHA_FN_PAIR(LINEAR),
  ALPHA_FN_PAIR(REVERSE),
  ALPHA_FN_PAIR(EASE_IN),
  ALPHA_FN_PAIR(EASE_OUT),
  ALPHA_FN_PAIR(EASE_IN_OUT),
  ALPHA_FN_PAIR(EASE_IN_SQUARE),
  ALPHA_FN_PAIR(EASE_OUT_SQUARE),
  ALPHA_FN_PAIR(EASE_IN_SINE),
  ALPHA_FN_PAIR(EASE_OUT_SINE),
  ALPHA_FN_PAIR(EASE_IN_OUT_SINE),
  ALPHA_FN_PAIR(BOUNCE),
  ALPHA_FN_PAIR(SIN),
  ALPHA_FN_PAIR(EASE_OUT_BACK),
};

int UtcDaliAlphaFunctionHelperGet(void)
{
  bool found;
  for (auto& a: BUILTIN_FUNCTIONS)
  {
    auto result = GetAlphaFunction(a.first, &found);
    DALI_TEST_EQUAL(result.GetBuiltinFunction(), a.second);
    DALI_TEST_EQUAL(result.GetMode(), AlphaFunction::BUILTIN_FUNCTION);
    DALI_TEST_CHECK(found);
  }

  auto result = GetAlphaFunction("made up function", &found);
  DALI_TEST_EQUAL(result.GetBuiltinFunction(), AlphaFunction::DEFAULT);
  DALI_TEST_EQUAL(result.GetMode(), AlphaFunction::BUILTIN_FUNCTION);
  DALI_TEST_CHECK(!found);

  END_TEST;
}

int UtcDaliAlphaFunctionHelperRegister(void)
{
  for (auto& a: BUILTIN_FUNCTIONS)
  {
    DALI_TEST_ASSERTION(RegisterAlphaFunction(a.first, AlphaFunction()), "given key already exists");
  }

  AlphaFunctionPrototype testFn = [](float f) {
    return f > .5f ? 1.f : 0.f;
  };
  RegisterAlphaFunction("step", AlphaFunction(testFn));

  bool found;
  auto result = GetAlphaFunction("step", &found);
  DALI_TEST_EQUAL(result.GetMode(), AlphaFunction::CUSTOM_FUNCTION);
  DALI_TEST_EQUAL(result.GetCustomFunction(), testFn);

  END_TEST;
}

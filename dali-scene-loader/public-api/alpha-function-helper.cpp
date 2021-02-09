/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include "dali-scene-loader/public-api/alpha-function-helper.h"
#include <unordered_map>

namespace Dali
{
namespace SceneLoader
{
namespace
{
// clang-format off
#define DALI_ALPHA_FUNCTION_ENTRY(x) { #x, AlphaFunction::x }
// clang-format on

std::unordered_map<std::string, AlphaFunction> sFunctions{
  DALI_ALPHA_FUNCTION_ENTRY(DEFAULT),
  DALI_ALPHA_FUNCTION_ENTRY(LINEAR),
  DALI_ALPHA_FUNCTION_ENTRY(REVERSE),
  DALI_ALPHA_FUNCTION_ENTRY(EASE_IN_SQUARE),
  DALI_ALPHA_FUNCTION_ENTRY(EASE_OUT_SQUARE),
  DALI_ALPHA_FUNCTION_ENTRY(EASE_IN),
  DALI_ALPHA_FUNCTION_ENTRY(EASE_OUT),
  DALI_ALPHA_FUNCTION_ENTRY(EASE_IN_OUT),
  DALI_ALPHA_FUNCTION_ENTRY(EASE_IN_SINE),
  DALI_ALPHA_FUNCTION_ENTRY(EASE_OUT_SINE),
  DALI_ALPHA_FUNCTION_ENTRY(EASE_IN_OUT_SINE),
  DALI_ALPHA_FUNCTION_ENTRY(BOUNCE),
  DALI_ALPHA_FUNCTION_ENTRY(SIN),
  DALI_ALPHA_FUNCTION_ENTRY(EASE_OUT_BACK),
};

#undef DALI_ALPHA_FUNCTION_ENTRY

} // namespace

AlphaFunction GetAlphaFunction(const std::string& name, bool* found)
{
  auto iFind   = sFunctions.find(name);
  bool success = iFind != sFunctions.end();
  if(found)
  {
    *found = success;
  }
  return success ? iFind->second : AlphaFunction(AlphaFunction::DEFAULT);
}

void RegisterAlphaFunction(const std::string& name, AlphaFunction alphaFn)
{
  DALI_ASSERT_ALWAYS(sFunctions.insert({name, alphaFn}).second &&
                     "Function with given key already exists.");
}

} // namespace SceneLoader
} // namespace Dali

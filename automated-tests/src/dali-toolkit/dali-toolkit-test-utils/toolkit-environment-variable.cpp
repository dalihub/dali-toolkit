/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include "toolkit-environment-variable.h"

// EXTERNAL INCLUDE
#include <map>

namespace Dali
{
namespace EnvironmentVariable
{
namespace
{
std::map<std::string, std::string> gEnvironmentVariables;
} // namespace

const char* GetEnvironmentVariable(const char* variable)
{
  auto value = gEnvironmentVariables.find(variable);
  if(value != gEnvironmentVariables.end())
  {
    return value->second.c_str();
  }
  return nullptr;
}

void SetTestEnvironmentVariable(const char* variable, const char* value)
{
  gEnvironmentVariables[variable] = value;
}

} // namespace EnvironmentVariable

} // namespace Dali

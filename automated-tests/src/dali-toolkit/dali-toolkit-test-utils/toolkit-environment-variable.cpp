/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <cstddef>
#include <string>

namespace Dali
{
namespace EnvironmentVariable
{
namespace
{
const char* gReturnValue = NULL;
std::string gEnvironmentVariableName;
std::string gEnvironmentVariableValue;
} // namespace

const char* GetEnvironmentVariable(const char* variable)
{
  if(gEnvironmentVariableName == variable)
  {
    return gEnvironmentVariableValue.c_str();
  }
  return gReturnValue;
}

void SetTestingEnvironmentVariable(bool testing)
{
  if(testing)
  {
    gReturnValue = "1";
  }
  else
  {
    gReturnValue = NULL;
  }
}

void SetTestEnvironmentVariable(const char* variable, const char* value)
{
  gEnvironmentVariableName  = variable;
  gEnvironmentVariableValue = value;
}

} // namespace EnvironmentVariable

} // namespace Dali

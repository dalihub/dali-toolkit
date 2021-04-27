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

#include <vector>

#include "dali-scene-loader/public-api/mesh-definition.h"
#include <dali-test-suite-utils.h>

using namespace Dali;
using namespace Dali::SceneLoader;

int UtcDaliMeshDefinitionBlobApplyMinMaxBothMinMaxApplied(void)
{
  using Container = std::vector<float>;

  Container buffer = { 4.0f, 6.0f, 8.0f, 10.0f, 12.0f };
  Container minValues = { 5.0f };
  Container maxValues = { 10.0f };
  Container result = {5.0f, 6.0f, 8.0f, 10.0f, 10.0f};

  MeshDefinition::Blob::ApplyMinMax(minValues, maxValues, 5, buffer.data());

  for( auto i = 0u; i < result.size(); ++i)
  {
    DALI_TEST_EQUALS(buffer[i], result[i], TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliMeshDefinitionBlobApplyMinMaxOnlyMin(void)
{
  using Container = std::vector<float>;

  Container buffer = { 4.0f, 6.0f, 8.0f, 10.0f, 12.0f };
  Container minValues = { 5.0f };
  Container maxValues = {};
  Container result = {5.0f, 6.0f, 8.0f, 10.0f, 12.0f};

  MeshDefinition::Blob::ApplyMinMax(minValues, maxValues, 5, buffer.data());

  for( auto i = 0u; i < result.size(); ++i)
  {
    DALI_TEST_EQUALS(buffer[i], result[i], TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliMeshDefinitionBlobApplyMinMaxOnlyMax(void)
{
  using Container = std::vector<float>;

  Container buffer = { 4.0f, 6.0f, 8.0f, 10.0f, 12.0f };
  Container minValues = { };
  Container maxValues = { 10.0f };
  Container result = {4.0f, 6.0f, 8.0f, 10.0f, 10.0f};

  MeshDefinition::Blob::ApplyMinMax(minValues, maxValues, 5, buffer.data());

  for( auto i = 0u; i < result.size(); ++i)
  {
    DALI_TEST_EQUALS(buffer[i], result[i], TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliMeshDefinitionBlobApplyMinMaxBothEmpty(void)
{
  using Container = std::vector<float>;

  Container buffer = { 4.0f, 6.0f, 8.0f, 10.0f, 12.0f };
  Container minValues;
  Container maxValues;
  Container result = {4.0f, 6.0f, 8.0f, 10.0f, 12.0f};

  MeshDefinition::Blob::ApplyMinMax(minValues, maxValues, 5, buffer.data());

  for( auto i = 0u; i < result.size(); ++i)
  {
    DALI_TEST_EQUALS(buffer[i], result[i], TEST_LOCATION);
  }

  END_TEST;
}


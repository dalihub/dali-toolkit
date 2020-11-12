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

#include "dali-scene-loader/public-api/resource-bundle.h"
#include "dali-scene-loader/public-api/utils.h"
#include <dali-test-suite-utils.h>
#include <string_view>

using namespace Dali;
using namespace Dali::SceneLoader;

int UtcDaliResourceRefCounts(void)
{
  ResourceBundle resourceBundle;
  resourceBundle.mEnvironmentMaps.resize(4);
  resourceBundle.mShaders.resize(13);
  resourceBundle.mMeshes.resize(17);
  resourceBundle.mMaterials.resize(19);

  int i = 0;
  std::vector<int> testEnvironmentReferences(resourceBundle.mEnvironmentMaps.size());
  for (auto& m : resourceBundle.mMaterials)
  {
    Index iEnv = 0;
    iEnv += (i % 3) == 0;
    iEnv += ((i % 4) == 0) * 2;
    m.first.mEnvironmentIdx = iEnv;

    printf("%d : %d, ", i, iEnv);

    ++testEnvironmentReferences[iEnv];
    ++i;
  }

  auto counter = resourceBundle.CreateRefCounter();
  DALI_TEST_EQUAL(counter[ResourceType::Environment].Size(), resourceBundle.mEnvironmentMaps.size());
  DALI_TEST_EQUAL(counter[ResourceType::Shader].Size(), resourceBundle.mShaders.size());
  DALI_TEST_EQUAL(counter[ResourceType::Mesh].Size(), resourceBundle.mMeshes.size());
  DALI_TEST_EQUAL(counter[ResourceType::Material].Size(), resourceBundle.mMaterials.size());

  std::fill(counter[ResourceType::Material].begin(), counter[ResourceType::Material].end(), 1u);
  resourceBundle.CountEnvironmentReferences(counter);
  i = 0;
  for (auto& er: counter[ResourceType::Environment])
  {
    DALI_TEST_EQUAL(er, testEnvironmentReferences[i]);
    ++i;
  }

  END_TEST;
}

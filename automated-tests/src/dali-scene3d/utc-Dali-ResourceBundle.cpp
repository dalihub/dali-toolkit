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

#include <dali-test-suite-utils.h>
#include <string_view>
#include "dali-scene3d/public-api/loader/resource-bundle.h"
#include "dali-scene3d/public-api/loader/utils.h"

using namespace Dali;
using namespace Dali::Scene3D::Loader;

int UtcDaliResourceRefCounts(void)
{
  ResourceBundle resourceBundle;
  
  // Use PushBack with correct Pair types
  for(int i = 0; i < 4; ++i)
  {
    resourceBundle.mEnvironmentMaps.PushBack({EnvironmentDefinition{}, EnvironmentDefinition::Textures{}});
  }
  for(int i = 0; i < 13; ++i)
  {
    resourceBundle.mShaders.PushBack({ShaderDefinition{}, Shader()});
  }
  for(int i = 0; i < 17; ++i)
  {
    resourceBundle.mMeshes.PushBack({MeshDefinition{}, MeshGeometry{}});
  }
  for(int i = 0; i < 19; ++i)
  {
    resourceBundle.mMaterials.PushBack({MaterialDefinition{}, TextureSet()});
  }

  int              i = 0;
  std::vector<int> testEnvironmentReferences(resourceBundle.mEnvironmentMaps.Count());
  for(auto& m : resourceBundle.mMaterials)
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
  DALI_TEST_EQUAL(counter[ResourceType::Environment].Size(), resourceBundle.mEnvironmentMaps.Count());
  DALI_TEST_EQUAL(counter[ResourceType::Shader].Size(), resourceBundle.mShaders.Count());
  DALI_TEST_EQUAL(counter[ResourceType::Mesh].Size(), resourceBundle.mMeshes.Count());
  DALI_TEST_EQUAL(counter[ResourceType::Material].Size(), resourceBundle.mMaterials.Count());

  std::fill(counter[ResourceType::Material].Begin(), counter[ResourceType::Material].End(), 1u);
  resourceBundle.mReferenceCounts = std::move(counter);
  resourceBundle.CountEnvironmentReferences();

  const ResourceRefCounts& referenceCounts = resourceBundle.mReferenceCounts;
  i                                        = 0;
  for(auto& er : referenceCounts[ResourceType::Environment])
  {
    DALI_TEST_EQUAL(er, testEnvironmentReferences[i]);
    ++i;
  }

  END_TEST;
}

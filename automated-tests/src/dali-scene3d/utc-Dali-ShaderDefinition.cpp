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

#include "dali-scene3d/public-api/loader/shader-definition.h"
#include <dali-test-suite-utils.h>
#include <string_view>

using namespace Dali;
using namespace Dali::Scene3D::Loader;

int UtcDaliShaderDefinitionFailedToLoad(void)
{
  ShaderDefinition shaderDef;
  shaderDef.mVertexShaderPath = "nonexistent.vsh";
  shaderDef.mFragmentShaderPath = "nonexistent.vsh";

  DALI_TEST_ASSERTION(shaderDef.LoadRaw(""), "Failed to load shader source");

  shaderDef.mVertexShaderPath = "dli_pbr.vsh";

  auto shaderPath = TEST_RESOURCE_DIR "/";
  DALI_TEST_ASSERTION(shaderDef.LoadRaw(shaderPath), "Failed to load shader source");

  shaderDef.mFragmentShaderPath = "dli_pbr.fsh";

  shaderDef.mHints.push_back("MODIFIES_GEOMETRY");
  shaderDef.mHints.push_back("OUTPUT_IS_TRANSPARENT");
  auto raw = shaderDef.LoadRaw(shaderPath);

  TestApplication app;
  auto shader = shaderDef.Load(std::move(raw));
  DALI_TEST_CHECK(shader);

  END_TEST;
}

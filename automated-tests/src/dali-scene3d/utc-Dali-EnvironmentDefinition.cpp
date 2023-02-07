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

#include "dali-scene3d/public-api/loader/environment-definition.h"
#include <dali-test-suite-utils.h>
#include <string_view>

using namespace Dali;
using namespace Dali::Scene3D::Loader;

int UtcDaliEnvironmentDefinitionLoadRawDefault(void)
{
  EnvironmentDefinition envDef;
  auto rawData = envDef.LoadRaw("");

  DALI_TEST_EQUAL(rawData.mDiffuse.mPixelData.size(), 6u);
  for (auto& face: rawData.mDiffuse.mPixelData)
  {
    DALI_TEST_EQUAL(face.size(), 1u);
  }

  DALI_TEST_EQUAL(rawData.mSpecular.mPixelData.size(), 6u);
  for (auto& face: rawData.mSpecular.mPixelData)
  {
    DALI_TEST_EQUAL(face.size(), 1u);
  }

  END_TEST;
}

int UtcDaliEnvironmentDefinitionLoadRawFail(void)
{
  for (auto name: { "nonexistent.ktx", TEST_RESOURCE_DIR "Cobe.obj" , TEST_RESOURCE_DIR "truncated.ktx" })
  {
    EnvironmentDefinition envDef { name, name };
    DALI_TEST_ASSERTION(envDef.LoadRaw(""), "Failed to load cubemap texture");

    envDef.mDiffuseMapPath = "";
    DALI_TEST_ASSERTION(envDef.LoadRaw(""), "Failed to load cubemap texture"); // specular map still invalid
  }

  END_TEST;
}

int UtcDaliEnvironmentDefinitionLoadRawSuccess(void)
{
  EnvironmentDefinition envDef { "forest_irradiance.ktx", "forest_radiance.ktx" };
  auto rawData = envDef.LoadRaw(TEST_RESOURCE_DIR "/");

  DALI_TEST_EQUAL(rawData.mDiffuse.mPixelData.size(), 6u);
  for (auto& face: rawData.mDiffuse.mPixelData)
  {
    DALI_TEST_EQUAL(face.size(), 1u);
    uint32_t size = 64u;
    for (auto& mipLevel : face)
    {
      DALI_TEST_EQUAL(mipLevel.GetPixelFormat(), Pixel::Format::RGB888);
      DALI_TEST_EQUAL(mipLevel.GetWidth(), size);
      DALI_TEST_EQUAL(mipLevel.GetHeight(), size);
      size /= 2;
    }
  }

  DALI_TEST_EQUAL(rawData.mSpecular.mPixelData.size(), 6u);
  for (auto& face: rawData.mSpecular.mPixelData)
  {
    DALI_TEST_EQUAL(face.size(), 5u);
    uint32_t size = 64u;
    for (auto& mipLevel : face)
    {
      DALI_TEST_EQUAL(mipLevel.GetPixelFormat(), Pixel::Format::RGB888);
      DALI_TEST_EQUAL(mipLevel.GetWidth(), size);
      DALI_TEST_EQUAL(mipLevel.GetHeight(), size);
      size /= 2;
    }
  }

  END_TEST;
}

int UtcDaliEnvironmentDefinitionLoadEmptyRaw(void)
{
  EnvironmentDefinition::RawData rawData;
  EnvironmentDefinition envDef;
  auto textures = envDef.Load(std::move(rawData));
  DALI_TEST_CHECK(!textures.mDiffuse);
  DALI_TEST_CHECK(!textures.mSpecular);

  END_TEST;
}

namespace
{

void CheckTextureDefault(Texture texture)
{
  DALI_TEST_CHECK(texture);
  DALI_TEST_EQUAL(texture.GetWidth(), 1u);
  DALI_TEST_EQUAL(texture.GetHeight(), 1u);
}

void CheckTextureNotDefault(Texture texture)
{
  DALI_TEST_CHECK(texture);
  DALI_TEST_CHECK(texture.GetWidth() > 1u);
  DALI_TEST_CHECK(texture.GetHeight() > 1u);
}

}

int UtcDaliEnvironmentDefinitionLoadDefault(void)
{
  EnvironmentDefinition envDef{};
  auto rawData = envDef.LoadRaw(TEST_RESOURCE_DIR "/");;

  TestApplication app;
  auto textures = envDef.Load(std::move(rawData));

  CheckTextureDefault(textures.mSpecular);
  CheckTextureDefault(textures.mDiffuse);

  END_TEST;
}

int UtcDaliEnvironmentDefinitionLoadDiffuse(void)
{
  EnvironmentDefinition envDef{ "forest_irradiance.ktx" };
  auto rawData = envDef.LoadRaw(TEST_RESOURCE_DIR "/");;

  TestApplication app;
  auto textures = envDef.Load(std::move(rawData));

  CheckTextureNotDefault(textures.mDiffuse);
  CheckTextureDefault(textures.mSpecular);

  END_TEST;
}

int UtcDaliEnvironmentDefinitionLoadSpecular(void)
{
  EnvironmentDefinition envDef{ "", "forest_radiance.ktx" };
  auto rawData = envDef.LoadRaw(TEST_RESOURCE_DIR "/");;

  TestApplication app;
  auto textures = envDef.Load(std::move(rawData));

  CheckTextureDefault(textures.mDiffuse);
  CheckTextureNotDefault(textures.mSpecular);

  END_TEST;
}

int UtcDaliEnvironmentDefinitionLoadBoth(void)
{
  EnvironmentDefinition envDef{ "forest_irradiance.ktx", "forest_radiance.ktx" };
  auto rawData = envDef.LoadRaw(TEST_RESOURCE_DIR "/");;

  TestApplication app;
  auto textures = envDef.Load(std::move(rawData));

  CheckTextureNotDefault(textures.mDiffuse);
  CheckTextureNotDefault(textures.mSpecular);

  END_TEST;
}

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
 *
 */

// Enable debug log for test coverage
#define DEBUG_ENABLED 1

#include <dali-test-suite-utils.h>
#include <string_view>
#include "dali-scene3d/public-api/loader/environment-map-loader.h"

#include <fstream>

using namespace Dali;
using namespace Dali::Scene3D::Loader;

int UtcDaliEnvironmentMapLoaderFailNonexistent(void)
{
  EnvironmentMapData environmentMapData;
  DALI_TEST_CHECK(!LoadEnvironmentMap("non-existent.jpg", environmentMapData));
  END_TEST;
}

int UtcDaliEnvironmentMapLoaderSuccess01(void)
{
  EnvironmentMapData environmentMapData;
  auto               path = TEST_RESOURCE_DIR "/forest_radiance.ktx";
  DALI_TEST_CHECK(LoadEnvironmentMap(path, environmentMapData));

  DALI_TEST_EQUAL(6u, environmentMapData.mPixelData.size());
  for(auto& face : environmentMapData.mPixelData)
  {
    uint32_t size = 64;
    for(auto& mipData : face)
    {
      DALI_TEST_EQUAL(size, mipData.GetWidth());
      DALI_TEST_EQUAL(size, mipData.GetHeight());
      DALI_TEST_EQUAL(Pixel::Format::RGB888, mipData.GetPixelFormat());
      size /= 2;
    }
  }

  END_TEST;
}

int UtcDaliEnvironmentMapLoaderAutoType01(void)
{
  EnvironmentMapData environmentMapData;
  auto               path = TEST_RESOURCE_DIR "/forest_diffuse_cubemap.png"; // cross horizontal
  DALI_TEST_EQUAL(environmentMapData.GetEnvironmentMapType(), Scene3D::EnvironmentMapType::AUTO);
  DALI_TEST_CHECK(LoadEnvironmentMap(path, environmentMapData));

  DALI_TEST_EQUAL(environmentMapData.GetEnvironmentMapType(), Scene3D::EnvironmentMapType::CUBEMAP);
  DALI_TEST_EQUAL(6u, environmentMapData.mPixelData.size());
  for(auto& face : environmentMapData.mPixelData)
  {
    uint32_t size = 512;
    DALI_TEST_EQUAL(size, face[0].GetWidth());
    DALI_TEST_EQUAL(size, face[0].GetHeight());
    DALI_TEST_EQUAL(Pixel::Format::RGBA8888, face[0].GetPixelFormat());
  }

  END_TEST;
}

int UtcDaliEnvironmentMapLoaderAutoType02(void)
{
  EnvironmentMapData environmentMapData;
  auto               path = TEST_RESOURCE_DIR "/forest_diffuse_cubemap_cross_vertical.png"; // cross vertical
  DALI_TEST_EQUAL(environmentMapData.GetEnvironmentMapType(), Scene3D::EnvironmentMapType::AUTO);
  DALI_TEST_CHECK(LoadEnvironmentMap(path, environmentMapData));

  DALI_TEST_EQUAL(environmentMapData.GetEnvironmentMapType(), Scene3D::EnvironmentMapType::CUBEMAP);
  DALI_TEST_EQUAL(6u, environmentMapData.mPixelData.size());
  for(auto& face : environmentMapData.mPixelData)
  {
    uint32_t size = 256;
    DALI_TEST_EQUAL(size, face[0].GetWidth());
    DALI_TEST_EQUAL(size, face[0].GetHeight());
    DALI_TEST_EQUAL(Pixel::Format::RGBA8888, face[0].GetPixelFormat());
  }

  END_TEST;
}

int UtcDaliEnvironmentMapLoaderAutoType03(void)
{
  EnvironmentMapData environmentMapData;
  auto               path = TEST_RESOURCE_DIR "/cubemap_array_horizontal.png"; // array horizontal
  DALI_TEST_EQUAL(environmentMapData.GetEnvironmentMapType(), Scene3D::EnvironmentMapType::AUTO);
  DALI_TEST_CHECK(LoadEnvironmentMap(path, environmentMapData));

  DALI_TEST_EQUAL(environmentMapData.GetEnvironmentMapType(), Scene3D::EnvironmentMapType::CUBEMAP);
  DALI_TEST_EQUAL(6u, environmentMapData.mPixelData.size());
  for(auto& face : environmentMapData.mPixelData)
  {
    uint32_t size = 100;
    DALI_TEST_EQUAL(size, face[0].GetWidth());
    DALI_TEST_EQUAL(size, face[0].GetHeight());
    DALI_TEST_EQUAL(Pixel::Format::RGB888, face[0].GetPixelFormat());
  }

  END_TEST;
}

int UtcDaliEnvironmentMapLoaderAutoType04(void)
{
  EnvironmentMapData environmentMapData;
  auto               path = TEST_RESOURCE_DIR "/cubemap_array_vertical.png"; // array horizontal
  DALI_TEST_EQUAL(environmentMapData.GetEnvironmentMapType(), Scene3D::EnvironmentMapType::AUTO);
  DALI_TEST_CHECK(LoadEnvironmentMap(path, environmentMapData));

  DALI_TEST_EQUAL(environmentMapData.GetEnvironmentMapType(), Scene3D::EnvironmentMapType::CUBEMAP);
  DALI_TEST_EQUAL(6u, environmentMapData.mPixelData.size());
  for(auto& face : environmentMapData.mPixelData)
  {
    uint32_t size = 100;
    DALI_TEST_EQUAL(size, face[0].GetWidth());
    DALI_TEST_EQUAL(size, face[0].GetHeight());
    DALI_TEST_EQUAL(Pixel::Format::RGB888, face[0].GetPixelFormat());
  }

  END_TEST;
}

int UtcDaliEnvironmentMapLoaderAutoType05(void)
{
  EnvironmentMapData environmentMapData;
  auto               path = TEST_RESOURCE_DIR "/equirectangular.png"; // no cube map
  DALI_TEST_EQUAL(environmentMapData.GetEnvironmentMapType(), Scene3D::EnvironmentMapType::AUTO);
  DALI_TEST_CHECK(LoadEnvironmentMap(path, environmentMapData));

  DALI_TEST_EQUAL(environmentMapData.GetEnvironmentMapType(), Scene3D::EnvironmentMapType::EQUIRECTANGULAR);
  DALI_TEST_EQUAL(1u, environmentMapData.mPixelData.size());
  for(auto& face : environmentMapData.mPixelData)
  {
    DALI_TEST_EQUAL(20, face[0].GetWidth());
    DALI_TEST_EQUAL(10, face[0].GetHeight());
    DALI_TEST_EQUAL(Pixel::Format::RGB888, face[0].GetPixelFormat());
  }

  END_TEST;
}

int UtcDaliEnvironmentMapLoaderEquirectangularMapType01(void)
{
  EnvironmentMapData environmentMapData;
  auto               path = TEST_RESOURCE_DIR "/forest_diffuse_cubemap.png"; // no cube map
  DALI_TEST_EQUAL(environmentMapData.GetEnvironmentMapType(), Scene3D::EnvironmentMapType::AUTO);
  environmentMapData.SetEnvironmentMapType(Scene3D::EnvironmentMapType::EQUIRECTANGULAR);
  DALI_TEST_CHECK(LoadEnvironmentMap(path, environmentMapData));

  DALI_TEST_EQUAL(environmentMapData.GetEnvironmentMapType(), Scene3D::EnvironmentMapType::EQUIRECTANGULAR);
  DALI_TEST_EQUAL(1u, environmentMapData.mPixelData.size());
  for(auto& face : environmentMapData.mPixelData)
  {
    DALI_TEST_EQUAL(2048, face[0].GetWidth());
    DALI_TEST_EQUAL(1536, face[0].GetHeight());
    DALI_TEST_EQUAL(Pixel::Format::RGBA8888, face[0].GetPixelFormat());
  }

  END_TEST;
}

int UtcDaliEnvironmentMapLoaderEnvironmentMapDataCreateTexture01(void)
{
  EnvironmentMapData environmentMapData;
  auto               path = TEST_RESOURCE_DIR "/forest_radiance.ktx";
  DALI_TEST_CHECK(LoadEnvironmentMap(path, environmentMapData));

  TestApplication app;
  auto            texture = environmentMapData.GetTexture();

  DALI_TEST_CHECK(texture);
  DALI_TEST_EQUAL(64u, texture.GetWidth());
  DALI_TEST_EQUAL(64u, texture.GetHeight());

  END_TEST;
}

int UtcDaliEnvironmentMapLoaderEnvironmentMapDataCreateTexture02(void)
{
  EnvironmentMapData environmentMapData;
  auto               path = TEST_RESOURCE_DIR "/forest_diffuse_cubemap.png";
  DALI_TEST_CHECK(LoadEnvironmentMap(path, environmentMapData));

  TestApplication app;
  auto            texture = environmentMapData.GetTexture();

  DALI_TEST_CHECK(texture);
  DALI_TEST_EQUAL(512u, texture.GetWidth());
  DALI_TEST_EQUAL(512u, texture.GetHeight());

  END_TEST;
}

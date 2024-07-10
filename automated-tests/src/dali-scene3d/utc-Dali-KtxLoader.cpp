/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

#include "dali-scene3d/public-api/loader/ktx-loader.h"
#include <dali-test-suite-utils.h>
#include <string_view>

#include <fstream>

using namespace Dali;
using namespace Dali::Scene3D::Loader;

int UtcDaliKtxLoaderFailNonexistent(void)
{
  EnvironmentMapData environmentMapData;
  DALI_TEST_CHECK(!LoadKtxData("non-existent.ktx", environmentMapData));
  END_TEST;
}

int UtcDaliKtxLoaderFailInvalid1(void)
{
  EnvironmentMapData environmentMapData;
  DALI_TEST_CHECK(!LoadKtxData(TEST_RESOURCE_DIR "/invalid.svg", environmentMapData)); // file smaller than KTX header
  END_TEST;
}

int UtcDaliKtxLoaderFailInvalid2(void)
{
  EnvironmentMapData environmentMapData;
  DALI_TEST_CHECK(!LoadKtxData(TEST_RESOURCE_DIR "/anim.gif", environmentMapData)); // not a KTX
  END_TEST;
}

int UtcDaliKtxLoaderFailTruncated(void)
{
  EnvironmentMapData environmentMapData;
  DALI_TEST_CHECK(!LoadKtxData(TEST_RESOURCE_DIR "/truncated.ktx", environmentMapData));
  END_TEST;
}

int UtcDaliKtxLoaderFailValidHeaderInvalidArray(void)
{
  EnvironmentMapData environmentMapData;
  DALI_TEST_CHECK(!LoadKtxData(TEST_RESOURCE_DIR "/headerOK-invalidArray.ktx", environmentMapData));
  END_TEST;
}

int UtcDaliKtxLoaderFailValidHeaderOnly(void)
{
  EnvironmentMapData environmentMapData;
  DALI_TEST_CHECK(!LoadKtxData(TEST_RESOURCE_DIR "/headerOnly.ktx", environmentMapData));
  END_TEST;
}

int UtcDaliKtxLoaderSuccess(void)
{
  EnvironmentMapData environmentMapData;
  auto path = TEST_RESOURCE_DIR "/forest_radiance.ktx";
  DALI_TEST_CHECK(LoadKtxData(path, environmentMapData));

  DALI_TEST_EQUAL(6u, environmentMapData.mPixelData.size());
  for (auto& face: environmentMapData.mPixelData)
  {
    uint32_t size = 64;
    for (auto& mipData: face)
    {
      DALI_TEST_EQUAL(size, mipData.GetWidth());
      DALI_TEST_EQUAL(size, mipData.GetHeight());
      DALI_TEST_EQUAL(Pixel::Format::RGB888, mipData.GetPixelFormat());
      size /= 2;
    }
  }

  END_TEST;
}

int UtcDaliKtxLoaderFormats(void)
{
  auto resPath = TEST_RESOURCE_DIR "/";
#define FORMAT_PATH(x) { #x, Pixel::x }
#define FORMAT_COMPRESSED_PATH(x) { #x, Pixel::COMPRESSED_ ## x ## _KHR }
  std::pair<std::string, Pixel::Format> pathFormats[] {
    FORMAT_COMPRESSED_PATH(RGBA_ASTC_4x4),
    FORMAT_COMPRESSED_PATH(RGBA_ASTC_5x4),
    FORMAT_COMPRESSED_PATH(RGBA_ASTC_5x5),
    FORMAT_COMPRESSED_PATH(RGBA_ASTC_6x5),
    FORMAT_COMPRESSED_PATH(RGBA_ASTC_6x6),
    FORMAT_COMPRESSED_PATH(RGBA_ASTC_8x5),
    FORMAT_COMPRESSED_PATH(RGBA_ASTC_8x6),
    FORMAT_COMPRESSED_PATH(RGBA_ASTC_8x8),
    FORMAT_COMPRESSED_PATH(RGBA_ASTC_10x5),
    FORMAT_COMPRESSED_PATH(RGBA_ASTC_10x6),
    FORMAT_COMPRESSED_PATH(RGBA_ASTC_10x10),
    FORMAT_COMPRESSED_PATH(RGBA_ASTC_12x10),
    FORMAT_COMPRESSED_PATH(RGBA_ASTC_12x12),
    FORMAT_PATH(RGB16F),
    FORMAT_PATH(RGB32F),
    FORMAT_PATH(RGBA8888),
  };
  for (auto i : pathFormats)
  {
    EnvironmentMapData environmentMapData;
    DALI_TEST_CHECK(LoadKtxData(resPath + i.first + ".ktx", environmentMapData));
    DALI_TEST_EQUAL(environmentMapData.mPixelData[0][0].GetPixelFormat(), i.second);
  }

  END_TEST;
}

int UtcDaliKtxLoaderEnvironmentMApDataCreateTexture1(void)
{
  uint32_t pixelBufferSize = 3;
  uint8_t* pixelBuffer = new uint8_t[pixelBufferSize];

  EnvironmentMapData environmentMapData;
  environmentMapData.mPixelData.push_back({});

  auto pixelData = PixelData::New(pixelBuffer, pixelBufferSize, 1, 1, Pixel::Format::RGB888, PixelData::DELETE_ARRAY);
  environmentMapData.mPixelData[0].push_back(pixelData);

  TestApplication app;
  auto texture = environmentMapData.GetTexture();

  DALI_TEST_CHECK(texture);
  DALI_TEST_EQUAL(1u, texture.GetWidth());
  DALI_TEST_EQUAL(1u, texture.GetHeight());

  END_TEST;
}

int UtcDaliKtxLoaderEnvironmentMApDataCreateTexture2(void)
{
  EnvironmentMapData environmentMapData;
  auto path = TEST_RESOURCE_DIR "/forest_radiance.ktx";
  DALI_TEST_CHECK(LoadKtxData(path, environmentMapData));

  TestApplication app;
  auto texture = environmentMapData.GetTexture();

  DALI_TEST_CHECK(texture);
  DALI_TEST_EQUAL(64u, texture.GetWidth());
  DALI_TEST_EQUAL(64u, texture.GetHeight());

  END_TEST;
}

int UtcDaliKtxLoaderEnvironmentMApDataCreateTexture3(void)
{
  EnvironmentMapData environmentMapData;
  auto path = TEST_RESOURCE_DIR "/papermill_E_diffuse-64.ktx";
  DALI_TEST_CHECK(LoadKtxData(path, environmentMapData));

  TestApplication app;
  auto texture = environmentMapData.GetTexture();

  DALI_TEST_CHECK(texture);
  DALI_TEST_EQUAL(64u, texture.GetWidth());
  DALI_TEST_EQUAL(64u, texture.GetHeight());

  END_TEST;

}

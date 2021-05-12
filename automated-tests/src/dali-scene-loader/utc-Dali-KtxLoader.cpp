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

// Enable debug log for test coverage
#define DEBUG_ENABLED 1

#include "dali-scene-loader/public-api/ktx-loader.h"
#include <dali-test-suite-utils.h>
#include <string_view>

#include <fstream>

using namespace Dali;
using namespace Dali::SceneLoader;

int UtcDaliKtxLoaderFailNonexistent(void)
{
  CubeData data;
  DALI_TEST_CHECK(!LoadCubeMapData("non-existent.ktx", data));
  END_TEST;
}

int UtcDaliKtxLoaderFailInvalid1(void)
{
  CubeData data;
  DALI_TEST_CHECK(!LoadCubeMapData(TEST_RESOURCE_DIR "/invalid.svg", data)); // file smaller than KTX header
  END_TEST;
}

int UtcDaliKtxLoaderFailInvalid2(void)
{
  CubeData data;
  DALI_TEST_CHECK(!LoadCubeMapData(TEST_RESOURCE_DIR "/anim.gif", data)); // not a KTX
  END_TEST;
}

int UtcDaliKtxLoaderFailTruncated(void)
{
  CubeData data;
  DALI_TEST_CHECK(!LoadCubeMapData(TEST_RESOURCE_DIR "/truncated.ktx", data));
  END_TEST;
}

int UtcDaliKtxLoaderSuccess(void)
{
  CubeData cubeData;
  auto path = TEST_RESOURCE_DIR "/forest_radiance.ktx";
  DALI_TEST_CHECK(LoadCubeMapData(path, cubeData));

  DALI_TEST_EQUAL(6u, cubeData.data.size());
  for (auto& face: cubeData.data)
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
    CubeData cubeData;
    DALI_TEST_CHECK(LoadCubeMapData(resPath + i.first + ".ktx", cubeData));
    DALI_TEST_EQUAL(cubeData.data[0][0].GetPixelFormat(), i.second);
  }

  END_TEST;
}

int UtcDaliKtxLoaderCubeDataCreateTexture1(void)
{
  uint32_t pixelBufferSize = 3;
  uint8_t* pixelBuffer = new uint8_t[pixelBufferSize];

  CubeData cubeData;
  cubeData.data.push_back({});

  auto pixelData = PixelData::New(pixelBuffer, pixelBufferSize, 1, 1, Pixel::Format::RGB888, PixelData::DELETE_ARRAY);
  cubeData.data[0].push_back(pixelData);

  TestApplication app;
  auto texture = cubeData.CreateTexture();

  DALI_TEST_CHECK(texture);
  DALI_TEST_EQUAL(1u, texture.GetWidth());
  DALI_TEST_EQUAL(1u, texture.GetHeight());

  END_TEST;
}

int UtcDaliKtxLoaderCubeDataCreateTexture2(void)
{
  CubeData cubeData;
  auto path = TEST_RESOURCE_DIR "/forest_radiance.ktx";
  DALI_TEST_CHECK(LoadCubeMapData(path, cubeData));

  TestApplication app;
  auto texture = cubeData.CreateTexture();

  DALI_TEST_CHECK(texture);
  DALI_TEST_EQUAL(64u, texture.GetWidth());
  DALI_TEST_EQUAL(64u, texture.GetHeight());

  END_TEST;
}

int UtcDaliKtxLoaderCubeDataCreateTexture3(void)
{
  CubeData cubeData;
  auto path = TEST_RESOURCE_DIR "/papermill_E_diffuse-64.ktx";
  DALI_TEST_CHECK(LoadCubeMapData(path, cubeData));

  TestApplication app;
  auto texture = cubeData.CreateTexture();

  DALI_TEST_CHECK(texture);
  DALI_TEST_EQUAL(64u, texture.GetWidth());
  DALI_TEST_EQUAL(64u, texture.GetHeight());

  END_TEST;

}

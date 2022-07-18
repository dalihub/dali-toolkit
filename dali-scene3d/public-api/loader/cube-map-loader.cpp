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

// FILE HEADER
#include <dali-scene3d/public-api/loader/cube-map-loader.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/loader/cube-loader.h>
#include <dali-scene3d/public-api/loader/ktx-loader.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <filesystem>

namespace Dali
{
namespace
{
const std::string_view KTX_EXTENSION = ".ktx";
}

namespace Scene3D
{
namespace Loader
{
bool LoadCubeMapData(const std::string& cubeMapUrl, CubeData& cubedata)
{
  std::filesystem::path modelPath(cubeMapUrl);
  std::string           extension = modelPath.extension();
  std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

  return (extension == KTX_EXTENSION) ? Dali::Scene3D::Loader::LoadKtxData(cubeMapUrl, cubedata) : Dali::Scene3D::Loader::LoadCubeData(cubeMapUrl, cubedata);
}

Texture LoadCubeMap(const std::string& cubeMapUrl)
{
  Texture                         cubeTexture;
  Dali::Scene3D::Loader::CubeData cubeData;
  if(Dali::Scene3D::Loader::LoadCubeMapData(cubeMapUrl, cubeData))
  {
    cubeTexture = cubeData.CreateTexture();
  }
  else
  {
    DALI_LOG_ERROR("Fail to load cube map, %s\n", cubeMapUrl.c_str());
  }

  return cubeTexture;
}

} // namespace Loader
} // namespace Scene3D
} // namespace Dali

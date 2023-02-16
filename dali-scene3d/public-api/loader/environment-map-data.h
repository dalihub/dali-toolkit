#ifndef DALI_SCENE3D_LOADER_ENVIRONMENT_MAP_DATA_H
#define DALI_SCENE3D_LOADER_ENVIRONMENT_MAP_DATA_H
/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>
#include <dali-scene3d/public-api/common/environment-map.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/rendering/texture.h>
#include <dali/public-api/rendering/shader.h>

namespace Dali
{
namespace Scene3D
{
namespace Loader
{
/**
 * @brief Stores the pixel data objects for environment map texture.
 * EnvironmentMapData supports cube map that contains textures for 6 faces, or
 * an equirectangular image.
 */
class DALI_SCENE3D_API EnvironmentMapData
{
public:
  /**
   * @brief Retrieves environment map texture from image file
   * @return texture for loaded environment map.
   */
  Texture GetTexture();

  /**
   * @brief Sets environment map type
   * @param[in] environmentMapType environment map type
   */
  void SetEnvironmentMapType(Dali::Scene3D::EnvironmentMapType environmentMapType)
  {
    if(mEnvironmentMapType != environmentMapType)
    {
      mEnvironmentMapTexture.Reset();
      mEnvironmentMapType = environmentMapType;
    }
  }

  /**
   * @brief Retrieves environment map type.
   * @return environment map type
   */
  Dali::Scene3D::EnvironmentMapType GetEnvironmentMapType() const
  {
    return mEnvironmentMapType;
  }

public:
  std::vector<std::vector<PixelData> > mPixelData;

private:
  Dali::Texture                     mEnvironmentMapTexture;
  Dali::Shader                      mEnvironmentMapShader;
  Dali::Scene3D::EnvironmentMapType mEnvironmentMapType{Dali::Scene3D::EnvironmentMapType::AUTO};
};

} // namespace Loader
} // namespace Scene3D
} // namespace Dali

#endif // DALI_SCENE3D_LOADER_ENVIRONMENT_MAP_DATA_H

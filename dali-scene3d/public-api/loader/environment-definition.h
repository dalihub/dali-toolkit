#ifndef DALI_SCENE3D_LOADER_ENVIRONMENT_DEFINITION_H
#define DALI_SCENE3D_LOADER_ENVIRONMENT_DEFINITION_H
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

// EXTERNAL INCLUDES
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/rendering/texture.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>
#include <dali-scene3d/public-api/loader/environment-map-data.h>

namespace Dali::Scene3D::Loader
{
/**
 * @brief Defines an environment map with either or both of radiance and irradiance maps.
 * @SINCE_2_0.7
 */
struct DALI_SCENE3D_API EnvironmentDefinition
{
  struct Textures
  {
    Texture  mDiffuse;  // irradiance
    Texture  mSpecular; // radiance
    Texture  mBrdf;     // pre-computed brdf
    uint32_t mSpecularMipmapLevels{1u};

    bool IsLoaded() const
    {
      return mDiffuse || mSpecular;
    }
  };

  struct RawData
  {
    EnvironmentMapData mDiffuse;
    EnvironmentMapData mSpecular;
    PixelData          mBrdf;
  };

  using EnvironmentData = std::pair<EnvironmentDefinition, Textures>;
  using Vector          = std::vector<EnvironmentData>;

  EnvironmentDefinition() = default;

  EnvironmentDefinition(const EnvironmentDefinition&) = delete;
  EnvironmentDefinition& operator=(const EnvironmentDefinition&) = delete;

  EnvironmentDefinition(EnvironmentDefinition&&) = default;
  EnvironmentDefinition& operator=(EnvironmentDefinition&&) = default;

  static Dali::Texture GetBrdfTexture();

  /**
   * @brief Loads raw pixel data for the given diffuse and specular maps.
   * @SINCE_2_0.7
   * @return The raw data for the given diffuse and specular maps
   * @note This can be done on any thread.
   */
  RawData LoadRaw(const std::string& environmentsPath);

  /**
   * @brief Creates DALi cubemap Textures from the pixel data in @a raw, then returns them in a Textures object.
   * @SINCE_2_0.7
   * @return DALi cubemap Textures
   * @note This must only be called from the event thread.
   */
  Textures Load(RawData&& raw);

  /**
   * @brief Get default intensity value.
   * @SINCE_2_2.1
   * @return Default intensity. (1.0f)
   */
  static float GetDefaultIntensity();

private:
  /// @cond internal
  static void LoadBrdfTexture();
  /// @endcond internal

public: // DATA
  std::string              mDiffuseMapPath;
  std::string              mSpecularMapPath;
  std::shared_ptr<RawData> mRawData;
  Quaternion               mCubeOrientation = Quaternion::IDENTITY;
  Vector3                  mYDirection      = Vector3::ONE;
  float                    mIblIntensity    = 1.0f;
  bool                     mUseBrdfTexture  = false;

private:
  static PixelData mBrdfPixelData;
  static Texture   mBrdfTexture;
  static bool      mIsBrdfLoaded;
};

} // namespace Dali::Scene3D::Loader

#endif // DALI_SCENE3D_LOADER_ENVIRONMENT_DEFINITION_H

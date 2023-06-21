#ifndef DALI_SCENE3D_INTERNAL_LIGHT_OBSERVER_H
#define DALI_SCENE3D_INTERNAL_LIGHT_OBSERVER_H

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

// EXTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>
#include <dali/public-api/rendering/texture.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/light/light.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
class LightObserver
{
public:
  /**
   * @brief Constructor.
   */
  LightObserver() = default;

  /**
   * @brief Virtual destructor.
   */
  virtual ~LightObserver() = default;

  /**
   * @brief Notifies Image Based Light Textures are changed by parent SceneView.
   *
   * @param[in] diffuseTexture cube map texture that can be used as a diffuse IBL source.
   * @param[in] specularTexture cube map texture that can be used as a specular IBL source.
   * @param[in] scaleFactor scale factor that controls light source intensity in [0.0f, 1.0f]. Default value is 1.0f.
   * @param[in] specularMipmapLevels mipmap levels of specular texture
   */
  virtual void NotifyImageBasedLightTexture(Dali::Texture diffuseTexture, Dali::Texture specularTexture, float scaleFactor, uint32_t specularMipmapLevels) = 0;

  /**
   * @brief Notifies Scale Factor of Image Based Light is changed by parent SceneView.
   *
   * @param[in] scaleFactor scale factor that controls light source intensity in [0.0f, 1.0f].
   */
  virtual void NotifyImageBasedLightScaleFactor(float scaleFactor) = 0;

  /**
   * @brief Notifies new light is added on SceneView
   *
   * @param[in] lightIndex Index of added light.
   * @param[in] light Added light.
   */
  virtual void NotifyLightAdded(uint32_t lightIndex, Scene3D::Light light) = 0;

  /**
   * @brief Notifies a light is removed from SceneView
   *
   * @param[in] lightIndex Index of light to be removed.
   */
  virtual void NotifyLightRemoved(uint32_t lightIndex) = 0;
};

} // namespace Internal

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_INTERNAL_LIGHT_OBSERVER_H

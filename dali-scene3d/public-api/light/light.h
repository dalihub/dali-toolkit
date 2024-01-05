#ifndef DALI_SCENE3D_LIGHT_H
#define DALI_SCENE3D_LIGHT_H

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

// EXTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>
#include <dali/public-api/common/dali-common.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal DALI_INTERNAL
{
class Light;
}

/**
 * @addtogroup dali_toolkit_controls_light
 * @{
 */

/**
 * @brief This class is to define 3D Light source.
 * Currently this Light class supports Directional Light that lits every position from the same direction. (e.g, Sun light)
 * If a Light object is added on SceneView, the 3D objects in the SceneView are shined the Light.
 * DALi Scene3D limits the maximum enabled light count per each SceneView.
 * Currently the maximum number is set to 5, and it can be retrieved by using GetMaximumEnabledLightCount().
 * If more than 5 enabled Light objects are added on SceneView, SceneView turns on only 5 lights in the order the lights were added.
 * This Light can be added to SceneView directly but also it can be added on other Actor.
 * When a parent actor is added to a SceneView, its Light behaves in the SceneView the same as if it were added directly to the SceneView.
 * @note Light inherits Actor, so Light color and direction can be controlled by setting Actor's COLOR and ORIENTATION Property.
 * Dali::DevelActor::LookAt() method can be used to set light direction easily.
 * @note Default light direction is to Z-axis
 * @code
 * Scene3D::SceneView sceneView = Scene3D::SceneView::New();
 * Scene3D::Lightlight = Scene3D::Light::New();
 * light.SetProperty(Dali::Actor::Property::COLOR, Color::BROWN);
 * Dali::DevelActor::LookAt(light, Vector3(1.0f, 1.0f, 1.0f));
 * sceneView.Add(light);
 * @endcode
 * @SINCE_2_2.32
 */
class DALI_SCENE3D_API Light : public Dali::Toolkit::Control
{
public:
  /**
   * @brief Create an initialized Light.
   *
   * @SINCE_2_2.32
   * @return A handle to a newly allocated Dali resource
   */
  static Light New();

  /**
   * @brief Creates an uninitialized Light.
   *
   * Only derived versions can be instantiated. Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   *
   * @SINCE_2_2.32
   */
  Light();

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   *
   * @SINCE_2_2.32
   */
  ~Light();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_2_2.32
   * @param[in] light Handle to an object
   */
  Light(const Light& light);

  /**
   * @brief Move constructor
   *
   * @SINCE_2_2.32
   * @param[in] rhs A reference to the moved handle
   */
  Light(Light&& rhs);

  /**
   * @brief Assignment operator.
   *
   * @SINCE_2_2.32
   * @param[in] light Handle to an object
   * @return reference to this
   */
  Light& operator=(const Light& light);

  /**
   * @brief Move assignment
   *
   * @SINCE_2_2.32
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  Light& operator=(Light&& rhs);

  /**
   * @brief Downcasts an Object handle to Light.
   *
   * If handle points to a Light, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @SINCE_2_2.32
   * @param[in] handle Handle to an object
   * @return Handle to a Light or an uninitialized handle
   */
  static Light DownCast(BaseHandle handle);

  /**
   * @brief Enables this light.
   * The Light is turned on when the Light object is added on SceneView and enabled.
   * @SINCE_2_2.32
   * @note SceneView can turn on only up to maximum enabled light count that can be retrieved by GetMaximumEnabledLightCount().
   * @param[in] enable True to make this Light enable.
   */
  void Enable(bool enable);

  /**
   * @brief Checks whether this light is enabled or not.
   * @SINCE_2_2.32
   * @return True if this light is enabled.
   */
  bool IsEnabled() const;

  /**
   * @brief Retrieves maximum enabled light count.
   * @SINCE_2_2.32
   * @return The number of maximum enabled light count.
   */
  static uint32_t GetMaximumEnabledLightCount();

  /**
   * @brief Enables shadow for this light.
   * Dali::Scene3D generates shadow by using shadow map.
   * For the Directional Light, the shadow map is created to cover view frustum of current selected camera.
   * This means that if the distance between the near and far planes is too large,
   * the shadow map has to cover an unnecessarily large area. This results in lower shadow quality.
   * @SINCE_2_2.43
   * @note This light should be already turned on in the SceneView.
   * @note (When enable is true) If there is previous light already enabled shadow in the SceneView, this function call is ignored.
   * @note (When enable is false, and this light is currently used for shader)
   * If there are other lights those are turned on and shadow enabled, one of the light will be used for shadow automatically.
   * @param[in] enable True to make this Light's shadow enable.
   */
  void EnableShadow(bool enable);

  /**
   * @brief Checks whether the shadow of this light is enabled or not.
   * @SINCE_2_2.43
   * @return True if the shadow of this light is enabled.
   */
  bool IsShadowEnabled() const;

  /**
   * @brief Enables filtering to soften the edge of shadow.
   * Basically the shadow is hard shadow that has sharp edge.
   * This method enables soft filtering to make the sharp edge to smoothing.
   * @SINCE_2_2.43
   * @note This soft filtering requires expensive computation power.
   * @param[in] useSoftFiltering True to soften the shadow edge.
   */
  void EnableShadowSoftFiltering(bool useSoftFiltering);

  /**
   * @brief Checks whether the shadow uses soft filtering.
   * @SINCE_2_2.43
   * @return True if the shadow edge is soften.
   */
  bool IsShadowSoftFilteringEnabled() const;

  /**
   * @brief Sets shadow intensity.
   * If the intensity is larger, the shadow area will be darker.
   * The intensity value is between [0, 1].
   * Default value is 0.5
   * @SINCE_2_2.43
   * @param[in] shadowIntensity Shadow intensity value
   */
  void SetShadowIntensity(float shadowIntensity);

  /**
   * @brief Retrieve shadow intensity.
   * @SINCE_2_2.43
   * @return Current shadow intensity value.
   */
  float GetShadowIntensity() const;

  /**
   * @brief Sets shadow bias.
   * Shadow bias is an offset value to remove shadow acne that is a visual artifact can be shown on the Shadow
   * Default value is 0.001
   * @SINCE_2_2.43
   * @param[in] shadowBias bias value to remove shadow acne.
   * @note If the shadow bias is too large, the object will appear detached from the shadow.
   */
  void SetShadowBias(float shadowBias);

  /**
   * @brief Retrieves shadow bias value.
   * @SINCE_2_2.43
   * @return Shadow bias value.
   */
  float GetShadowBias() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in] implementation The Control implementation
   */
  DALI_INTERNAL Light(Internal::Light& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  DALI_INTERNAL Light(Dali::Internal::CustomActor* internal);
  /// @endcond
};

/**
 * @}
 */
} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_LIGHT_H

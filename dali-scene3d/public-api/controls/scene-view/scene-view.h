#ifndef DALI_SCENE3D_SCENE_VIEW_H
#define DALI_SCENE3D_SCENE_VIEW_H

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
#include <dali-toolkit/public-api/controls/control.h>
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/common/dali-common.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>
#include <dali-scene3d/public-api/common/environment-map.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal DALI_INTERNAL
{
class SceneView;
}

/**
 * @addtogroup dali_toolkit_controls_scene_view
 * @{
 */

/**
 * @brief SceneView is a Dali::Toolkit::Control to show multiple 3D objects in a single 3D scene.
 * Each SceneView has its own 3D space, and 3D objects added to SceneView are positioned in the space.
 * SceneView has a 3D root layer internally to trigger the depth test in the rendering process.
 * When an Actor is added to the SceneView with Add() method,
 * the Actor and its children are actually become child of the 3D layer automatically.
 *
 *  SceneView
 *      |
 * Dali::Layer(Layer::LAYER_3D)
 *      |
 *    Actor
 *
 * The children of the 3D root layer will be rendered with the SceneView's own CameraActor.
 *
 * SceneView can have multiple CameraActor and one of them is used to render the multiple objects as a Scene.
 * AddCamera(), RemoveCamera(), GetCamera(), and SelectCamera() are methods to manage CameraActors of the SceneView.
 * Users can place multiple cameras in a scene, either to show the entire scene or to show individual objects.
 * And the user can select the currently needed camera by using the SelectCamera() method.
 *
 * SceneView makes one built-in CameraActor by default.
 * The default CameraActor has index 0 and is not removed by using RemoveCamera() method.
 * Therefore, the minimum value returned by GetCameraCount() method is 1.
 *
 * If the size of SceneView is changed, Some properties of CameraActor that depend on the size can be changed too.
 * The changing properties are as follows: projectionMode, aspectRatio, leftPlaneDistance, rightPlaneDistance, topPlaneDistance, and bottomPlaneDistance.
 * Position, near/farPlaneDistance, and FieldOfView are maintained even if the size of the SceneView is changed.
 * The FieldOfView of Dali::CameraActor is for vertical fov. The horizontal fov is internally updated according to the SceneView size.
 *
 * The same light source is set for all Models added to SceneView, if SceneView has light source.
 * The SetImageBasedLightSource() method sets the same IBL to all Models added to the SceneView.
 * If any Model already has an IBL, it is replaced with the SceneView's IBL.
 *
 * SceneView provides an option to use FBO for rendering result with UseFramebuffer() method.
 * If it is false, SceneView is always drawn in the form of a rectangle on the default window surface directly.
 * It improves performance, but the SceneView is always drawn on top of other 2D objects regardless of Actor tree order.
 * And it will show wrong result in case the window's default CameraActor is transformed.
 * So, it is recommended not to change window's CameraActor.
 *
 * If FBO is used, the rendering result of SceneView is drawn on the FBO and it is mapped on the plane of the SceneView.
 * It decreases performance, but it is useful to show SceneView according to the rendering order with other Actors.
 * And it can be used in case window's CameraActor is transformed.
 *
 * And since SceneView is a Control, it can be placed together with other 2D UI components in the DALi window.
 *
 * @SINCE_2_1.38
 * @code
 *
 * Dali::Scene3D::SceneView sceneView = Dali::Scene3D::SceneView::New();
 * sceneView.SetProperty(Dali::Actor::Property::SIZE, Vector2(400, 400));
 * mWindow.Add(sceneView);
 *
 * Dali::Scene3D::Model model = Dali::Scene3D::Model::New(...);
 * sceneView.Add(model);
 *
 * CameraActor cameraActor = CameraActor::New();
 * sceneView.AddCamera(cameraActor);
 *
 * @endcode
 */
class DALI_SCENE3D_API SceneView : public Dali::Toolkit::Control
{
public:
  /**
   * @brief Create an initialized SceneView.
   *
   * @SINCE_2_1.38
   * @return A handle to a newly allocated Dali resource
   */
  static SceneView New();

  /**
   * @brief Creates an uninitialized SceneView.
   *
   * Only derived versions can be instantiated. Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   *
   * @SINCE_2_1.38
   */
  SceneView();

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   *
   * @SINCE_2_1.38
   */
  ~SceneView();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_2_1.38
   * @param[in] sceneView Handle to an object
   */
  SceneView(const SceneView& sceneView);

  /**
   * @brief Move constructor
   *
   * @SINCE_2_1.38
   * @param[in] rhs A reference to the moved handle
   */
  SceneView(SceneView&& rhs) noexcept;

  /**
   * @brief Assignment operator.
   *
   * @SINCE_2_1.38
   * @param[in] sceneView Handle to an object
   * @return reference to this
   */
  SceneView& operator=(const SceneView& sceneView);

  /**
   * @brief Move assignment
   *
   * @SINCE_2_1.38
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  SceneView& operator=(SceneView&& rhs) noexcept;

  /**
   * @brief Downcasts an Object handle to SceneView.
   *
   * If handle points to a SceneView, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @SINCE_2_1.38
   * @param[in] handle Handle to an object
   * @return Handle to a SceneView or an uninitialized handle
   */
  static SceneView DownCast(BaseHandle handle);

  /**
   * @brief Adds a CameraActor to the SceneView
   * The CameraActor can be used as a selected camera to render the scene by using SelectCamera(uint32_t) or SelectCamera(std::string)
   *
   * @SINCE_2_1.38
   * @note
   * AspectRatio property of CameraActor will be changed depending on the Size of this SceneView.
   *
   * For Perspective camera
   * The FieldOfView of Dali::CameraActor is for vertical fov.
   * When the size of the SceneView is changed, the vertical fov is maintained
   * and the horizontal fov is automatically calculated according to the SceneView's aspect ratio.
   *
   * For Orthographic camera
   * leftPlaneDistance, rightPlaneDistance, and bottomPlaneDistance properties are defined according to the topPlaneDistance and aspectRatio.
   *
   * @param[in] camera CameraActor added on this scene view.
   */
  void AddCamera(Dali::CameraActor camera);

  /**
   * @brief Removes a CameraActor from this SceneView.
   *
   * @SINCE_2_1.38
   * @note If removed CameraActor is selected CameraActor,
   * first camera in the list is set to selected CameraActor.
   *
   * @param[in] camera CameraActor to be removed from this SceneView
   */
  void RemoveCamera(CameraActor camera);

  /**
   * @brief Retrieves the number of cameras.
   *
   * @SINCE_2_1.38
   * @return Number of cameras those currently the SceneView contains.
   */
  uint32_t GetCameraCount() const;

  /**
   * @brief Retrieves selected CameraActor.
   *
   * @SINCE_2_1.38
   * @return CameraActor currently used in SceneView as a selected CameraActor
   */
  CameraActor GetSelectedCamera() const;

  /**
   * @brief Retrieves a CameraActor of the index.
   *
   * @SINCE_2_1.38
   * @param[in] index Index of CameraActor to be retrieved.
   *
   * @return CameraActor of the index
   */
  CameraActor GetCamera(uint32_t index) const;

  /**
   * @brief Retrieves a CameraActor of the name.
   *
   * @SINCE_2_1.38
   * @param[in] name string keyword of CameraActor to be retrieved.
   *
   * @return CameraActor that has the name as a Dali::Actor::Property::NAME
   */
  CameraActor GetCamera(const std::string& name) const;

  /**
   * @brief Makes SceneView use a CameraActor of index as a selected camera.
   *
   * @SINCE_2_1.38
   * @param[in] index Index of CameraActor to be used as a selected camera.
   */
  void SelectCamera(uint32_t index);

  /**
   * @brief Makes SceneView use a CameraActor of a name as a selected camera.
   *
   * @SINCE_2_1.38
   * @param[in] name string keyword of CameraActor to be used as a selected camera.
   */
  void SelectCamera(const std::string& name);

  /**
   * @brief Sets Image Based Light Source to apply it on the all Models those added on this SceneView.
   *
   * @SINCE_2_1.38
   * @note If any Models already have IBL, they are batch-overridden with the SceneView's IBL.
   * If SceneView has IBL, IBL of newly added Model is also overridden.
   * To set indivisual IBL for each Model, the Model's IBL should be set after the SceneView's IBL.
   *
   * @param[in] diffuseUrl cube map that can be used as a diffuse IBL source.
   * @param[in] specularUrl cube map that can be used as a specular IBL source.
   * @param[in] scaleFactor scale factor that controls light source intensity in [0.0f, 1.0f]. Default value is 1.0f.
   */
  void SetImageBasedLightSource(const std::string& diffuseUrl, const std::string& specularUrl, float scaleFactor = 1.0f);

  /**
   * @brief Sets Scale Factor of Image Based Light Source.
   *
   * @SINCE_2_1.41
   * @note If SetImageBasedLightSource() method is called after this method, scaleFactor is overriden.
   * @note Default value is 1.0f.
   *
   * @param[in] scaleFactor scale factor that controls light source intensity in [0.0f, 1.0f].
   */
  void SetImageBasedLightScaleFactor(float scaleFactor);

  /**
   * @brief Gets Scale Factor of Image Based Light Source.
   * Default value is 1.0f.
   *
   * @SINCE_2_1.41
   * @return scale factor that controls light source intensity.
   */
  float GetImageBasedLightScaleFactor() const;

  /**
   * @brief Sets whether to use FBO or not for the SceneView.
   * If useFramebuffer is true, rendering result of SceneView is drawn on FBO and it is mapping on this SceneView plane.
   * If useFramebuffer is false, each item in SceneView is rendered on window directly.
   * Default is false.
   *
   * @SINCE_2_1.38
   * @note If useFramebuffer is true, it could decrease performance but entire rendering order is satisfied.
   * If useFramebuffer is false, performance is become better but SceneView is rendered on top of the other 2D Actors regardless tree order.
   *
   * @param[in] useFramebuffer True to use FBO for SceneView.
   */
  void UseFramebuffer(bool useFramebuffer);

  /**
   * @brief Gets whether this SceneView uses Framebuffer or not.
   *
   * @SINCE_2_1.38
   * @return bool True if this SceneView uses Framebuffer.
   */
  bool IsUsingFramebuffer() const;

  /**
   * @brief Sets Multisampling level when we use Framebuffer.
   * Default is 0.
   *
   * @SINCE_2_2.12
   * @note Only applied if SceneView is using Framebuffer and Framebuffer Multisampling extension is supported.
   *
   * @param[in] multiSamplingLevel Level of multisampling if we use Framebuffer.
   */
  void SetFramebufferMultiSamplingLevel(uint8_t multiSamplingLevel);

  /**
   * @brief Gets Multisampling level that user set.
   * Default is 0.
   *
   * @SINCE_2_2.12
   * @note This API doesn't check whether Multisampling extension is supported or not.
   *
   * @return MultisamplingLevel that user set.
   */
  uint8_t GetFramebufferMultiSamplingLevel() const;

  /**
   * @brief Sets Skybox for this scene.
   * Skybox texture starts to be loaded when SceneView is onScene.
   * And Skybox texture is asynchronously loaded. When loading is finished, ResourceReady is emitted.
   *
   * @SINCE_2_2.0
   * @param[in] skyboxUrl image url for skybox.
   * @note Default SkyboxEnvironmentMapType is Cube Map. Use SetSkyboxEnvironmentMapType method to set type explicitly.
   */
  void SetSkybox(const std::string& skyboxUrl);

  /**
   * @brief Sets Skybox environment map type for this skybox.
   * If skybox texture already starts to be loaded, when the type is changed, the load request is canceled and re-starts to load with new type.
   *
   * @SINCE_2_2.11
   * @param[in] skyboxEnvironmentMapType The environment type of skybox (by default it is cubemap).
   */
  void SetSkyboxEnvironmentMapType(Scene3D::EnvironmentMapType skyboxEnvironmentMapType);

  /**
   * @brief Sets Skybox intensity.
   * The skybox intensity is multiplied to the color of skybox texture.
   * Default value is 1.0f.
   *
   * @SINCE_2_2.0
   * @note Intensity should be positive value.
   * @param[in] intensity Intensity value to be multiplied to the cube map color
   */
  void SetSkyboxIntensity(float intensity);

  /**
   * @brief Gets Skybox intensity.
   * Default value is 1.0f.
   *
   * @SINCE_2_2.0
   * @return skybox intensity
   */
  float GetSkyboxIntensity() const;

  /**
   * @brief Sets Orientation of Skybox.
   *
   * @SINCE_2_2.0
   * @param[in] orientation Quaternion for orientation of Skybox.
   */
  void SetSkyboxOrientation(const Quaternion& orientation);

  /**
   * @brief Gets Skybox orientation.
   *
   * @SINCE_2_2.0
   * @return skybox orientation
   */
  Quaternion GetSkyboxOrientation() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in] implementation The Control implementation
   */
  DALI_INTERNAL SceneView(Internal::SceneView& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  DALI_INTERNAL SceneView(Dali::Internal::CustomActor* internal);
  /// @endcond
};

/**
 * @}
 */
} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_SCENE_VIEW_H

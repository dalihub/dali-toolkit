#ifndef DALI_TOOLKIT_SCENE3D_VIEW_H
#define DALI_TOOLKIT_SCENE3D_VIEW_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/actors/camera-actor.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal DALI_INTERNAL
{
/**
 * Scene3dView implementation class
 */
class Scene3dView;

} // namespace DALI_INTERNAL

/**
 *
 * Scene3dView is a class for containing scene elements loaded from scene format file(e.g., glTF). Scene elements mean scene graph, cameras, and animations.
 *
 * Basic idea:-
 *
 * 1) The Scene3dView is initialized with diffuse and specular cube map for the Image Based Lighting.\n
 *    If the Scene3dView initialized without cube map, the objects of the Scene3dView cannot be rendered with IBL.\n
 * 2) The Scene3dView is loaded from each scene format file(e.g., glTF).\n
 * 3) The Scene3dView can have a point light or a directional light.(optional)\n
 * 4) The Scene3dView playes each actor's animation.\n
 *
 *
 * Usage example: -
 *
 * @code
 *
 * void Scene3dViewExample::Create( Application& application )
 * {
 *   // Use 'Scene3dView::New( URL_SCENE_FILE )', if you don't want to render with IBL.
 *   Scene3dView scene3dView = Scene3dView::New( URL_SCENE_FILE, URL_DIFFUSE_TEXTURE, URL_SPECULAR_TEXTURE );
 *
 *   Stage::GetCurrent().Add( scene3dView );
 *   scene3dView.PlayAnimations();
 *
 *   scene3dView.SetLight( Scene3dView::LightType::DIRECTIONAL_LIGHT, Vector3( 1.0, 1.0, -1.0 ), Vector3( 0.3, 0.3, 0.3 ) );
 * }
 *
 * @endcode
 *
 * @remarks This control makes 3D Layer internally. Therefore, if any 2D UI
 * control is added as a child of this Scene3dView, the functionality of the 2D UI
 * may not work well.
 */

class DALI_TOOLKIT_API Scene3dView : public Control
{
public:
  enum LightType
  {
    // Scene doesn't use both of point and directional light
    NONE = 0,
    // Scene use point light
    POINT_LIGHT,
    // Scene use directional light
    DIRECTIONAL_LIGHT,
    // Scene use Image Based Lighting
    IMAGE_BASED_LIGHT,
    // Scene use Image Based Lighting and point light
    IMAGE_BASED_LIGHT_AND_POINT_LIGHT,
    // Scene use Image Based Lighting and directional light
    IMAGE_BASED_LIGHT_AND_DIRECTIONAL_LIGHT
  };

  /**
   * @brief Create an uninitialized Scene3dView; this can be initialized with Scene3dView::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  Scene3dView();

  /**
   * @brief Copy constructor. Creates another handle that points to the same real object
   */
  Scene3dView(const Scene3dView& handle);

  /**
   * @brief Assignment operator. Changes this handle to point to another real object
   */
  Scene3dView& operator=(const Scene3dView& handle);

  /**
   * @brief Destructor
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Scene3dView();

  /**
   * @brief Downcast an Object handle to Scene3dView. If handle points to a Scene3dView the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a Scene3dView or an uninitialized handle
   */
  static Scene3dView DownCast(BaseHandle handle);

  /**
   * @brief Create an initialized Scene3dView.
   * @param[in] filePath File path of scene format file (e.g., glTF).
   * @return A handle to a newly allocated Dali resource
   */
  static Scene3dView New(const std::string& filePath);

  /**
   * @brief Create an initialized Scene3dView.
   * @param[in] filePath File path of scene format file (e.g., glTF).
   * @param[in] diffuseTexturePath The texture path of diffuse cube map that used to render with Image Based Lighting.
   * @param[in] specularTexturePath The texture path of specular cube map that used to render with Image Based Lighting.
   * @param[in] scaleFactor Scaling factor for the Image Based Lighting.
   * @return A handle to a newly allocated Dali resource
   */
  static Scene3dView New(const std::string& filePath, const std::string& diffuseTexturePath, const std::string& specularTexturePath, Vector4 scaleFactor);

  /**
   * @brief Get animation count.
   * @return number of animations.
   */
  uint32_t GetAnimationCount();

  /**
   * @brief Play an animation.
   * @param[in] index Animation index
   * @return true if animation is played.
   */
  bool PlayAnimation(uint32_t index);

  /**
   * @brief Play all animations.
   * @return true if animations are played.
   */
  bool PlayAnimations();

  /**
   * @brief Set point light or directional light. If SetLight is not called, this scene doesn't use these kind of light.
   * @param[in] type The light type. If the light is point light set this LightType::POINT_LIGHT,
   * or if the light is directional light set this LightType::DIRECTIONAL_LIGHT.
   * @param[in] lightVector The point light position when light type is LightType::POINT_LIGHT.
   * The light direction when light type is LightType::DIRECTIONAL_LIGHT.
   * @param[in] lightColor Vector3 value that denotes the light color of point light or directional light. Since this is the light color, we don't need to use alpha value.
   * @return true if point light or directional light is set.
   */
  bool SetLight(LightType type, Vector3 lightVector, Vector3 lightColor);

  /**
   * @brief Get default CameraActor. Dali::Camera::Type = Dali::Camera::LOOK_AT_TARGET , near clipping plane = 0.1, and camera position = Vector3( 0.0, 0.0, 0.0 ).
   * @return CameraActor.
   */
  CameraActor GetDefaultCamera();

  /**
   * @brief Get camera count.
   * @return number of cameras.
   */
  uint32_t GetCameraCount();

  /**
   * @brief Get CameraActor. If there is no CameraActor in the list, then returns default CameraActor.
   * @param[in] cameraIndex Index of CameraActor list.
   * @return CameraActor.
   */
  CameraActor GetCamera(uint32_t cameraIndex);

  // Not intended for developer use
public:
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The UI Control implementation.
   */
  DALI_INTERNAL Scene3dView(Toolkit::Internal::Scene3dView& implementation);

  explicit DALI_INTERNAL Scene3dView(Dali::Internal::CustomActor* internal);
};

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_SCENE3D_VIEW_H

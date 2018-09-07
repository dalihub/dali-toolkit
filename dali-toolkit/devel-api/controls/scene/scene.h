#ifndef DALI_TOOLKIT_SCENE_H
#define DALI_TOOLKIT_SCENE_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/camera-actor.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali/public-api/rendering/texture.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{

/**
 * Scene implementation class
 */
class Scene;

}

/**
 *
 * Scene is a class for containing scene elements loaded from scene format file(e.g., glTF). Scene elements mean scene graph, cameras, and animations.
 *
 * Basic idea:-
 *
 * 1) The Scene is initialized with diffuse and specular cube map for the Image Based Lighting.\n
 *    If the Scene initialized without cube map, the objects of the Scene cannot be rendered with IBL.\n
 * 2) The Scene is loaded from each scene format file(e.g., glTF).\n
 * 3) The Scene can have a point light or a directional light.(optional)\n
 * 4) The Scene playes each actor's animation.\n
 *
 *
 * Usage example: -
 *
 * @code
 *
 * void SceneExample::Create( Application& application )
 * {
 *   // Use 'Scene::New( URL_SCENE_FILE )', if you don't want to render with IBL.
 *   Scene scene = Scene::New( URL_SCENE_FILE, URL_DIFFUSE_TEXTURE, URL_SPECULAR_TEXTURE );
 *
 *   Stage::GetCurrent().Add( scene );
 *   scene.PlayAnimations();
 *
 *   scene.SetLight( Scene::LightType::DIRECTIONAL_LIGHT, Vector3( 1.0, 1.0, -1.0 ), Vector3( 0.3, 0.3, 0.3 ) );
 * }
 *
 * @endcode
 *
 * @remarks This control makes 3D Layer internally. Therefore, if any 2D UI
 * control is added as a child of this Scene, the functionality of the 2D UI
 * may not work well.
 */

class DALI_TOOLKIT_API Scene : public Control
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
   * @brief Create an uninitialized Scene; this can be initialized with Scene::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  Scene();

  /**
   * @brief Copy constructor. Creates another handle that points to the same real object
   */
  Scene( const Scene& handle );

  /**
   * @brief Assignment operator. Changes this handle to point to another real object
   */
  Scene& operator=( const Scene& handle );

  /**
   * @brief Destructor
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Scene();

  /**
   * @brief Downcast an Object handle to Scene. If handle points to a Scene the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a Scene or an uninitialized handle
   */
  static Scene DownCast( BaseHandle handle );

  /**
   * @brief Create an initialized Scene.
   * @param[in] filePath File path of scene format file (e.g., glTF).
   * @return A handle to a newly allocated Dali resource
   */
  static Scene New( const std::string& filePath );

  /**
   * @brief Create an initialized Scene.
   * @param[in] filePath File path of scene format file (e.g., glTF).
   * @param[in] diffuseTexturePath The texture path of diffuse cube map that used to render with Image Based Lighting.
   * @param[in] specularTexturePath The texture path of specular cube map that used to render with Image Based Lighting.
   * @param[in] ScaleFactor Scaling factor for the Image Based Lighting. Default value is initialized with Vector4( 1.0, 1.0, 1.0, 1.0 ).
   * @return A handle to a newly allocated Dali resource
   */
  static Scene New( const std::string& filePath, const std::string& diffuseTexturePath, const std::string& specularTexturePath, Vector4 ScaleFactor = Vector4( 1.0, 1.0, 1.0, 1.0 ) );

  /**
   * @brief Get animation count.
   * @return number of animations.
   */
  uint32_t GetAnimationCount();

  /**
   * @brief Play an animations.
   * @param[in] index Animation index
   * @return true if animation is played.
   */
  bool PlayAnimation( uint32_t index );

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
  bool SetLight( LightType type, Vector3 lightVector, Vector3 lightColor = Vector3( 1.0, 1.0, 1.0 ) );

  /**
   * @brief Get default CameraActor. Dali::Camera::Type = Dali::Camera::LOOK_AT_TARGET , near clipping plane = 0.1, and camera position = Vector3( 0.0, 0.0, 0.0 ).
   * @return CameraActor.
   */
  CameraActor GetDefaultCamera();

  /**
   * @brief Get CameraActor. If there is no CameraActor in the list, then returns default CameraActor.
   * @return CameraActor.
   */
  CameraActor GetCamera( int cameraIndex = -1 );

  // Not intended for developer use
public:

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The UI Control implementation.
   */
  DALI_INTERNAL Scene( Toolkit::Internal::Scene& implementation );

  explicit DALI_INTERNAL Scene( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_SCENE_H

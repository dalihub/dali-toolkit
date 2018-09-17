#ifndef DALI_TOOLKIT_INTERNAL_SCENE_H
#define DALI_TOOLKIT_INTERNAL_SCENE_H

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

// EXTERNAL INCLUDES
#include <cstring>
#include <dali/public-api/object/base-object.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/adaptor-framework/file-loader.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/scene/scene.h>
#include <dali-toolkit/internal/controls/scene/gltf-loader.h>
#include <dali-toolkit/public-api/controls/control-impl.h>

namespace Dali
{

namespace Toolkit
{

class Scene;

namespace Internal
{

class GltfLoader;

namespace
{

// glTF file extension
const std::string GLTF_EXT( ".gltf" );

/**
 * cube map face index
 */
const int cubeMap_index_x[2][6] = { { 2, 0, 1, 1, 1, 3 },{ 0, 1, 2, 3, 4, 5 } };
const int cubeMap_index_y[2][6] = { { 1, 1, 0, 2, 1, 1 },{ 0, 0, 0, 0, 0, 0 } };

} // namespace

/**
 * Scene implementation class
 */
class Scene : public Control
{
public:

  enum CubeType
  {
    CROSS_HORIZENTAL = 0,                 // Cross horizental style cube map
    ARRAY_HORIZENTAL,                     // array horizental style cube map
    NONE
  };


  /**
   * @copydoc Dali::Toolkit::Scene::Scene
   */
  Scene();

  /**
   * @copydoc Dali::Toolkit::Scene::~Scene
   */
  virtual ~Scene();

  /**
   * @copydoc Dali::Toolkit::Scene::New( const std::string& filePath )
   */
  static Dali::Toolkit::Scene New( const std::string& filePath );

  /**
   * @copydoc Dali::Toolkit::Scene::New( const std::string& filePath, const std::string& diffuseTexturePath, const std::string& specularTexturePath, Vector4 ScaleFactor = Vector4( 1.0, 1.0, 1.0, 1.0 ) )
   */
  static Dali::Toolkit::Scene New( const std::string& filePath, const std::string& diffuseTexturePath, const std::string& specularTexturePath, Vector4 ScaleFactor = Vector4( 1.0, 1.0, 1.0, 1.0 ) );

  /**
   * @copydoc Dali::Toolkit::Scene::CreateScene( const std::string& filePath )
   */
  bool CreateScene( const std::string& filePath );

  /**
   * @copydoc Dali::Toolkit::Scene::CreateScene( const std::string& filePath )
   */
  uint32_t GetAnimationCount();

  /**
   * @copydoc Dali::Toolkit::Scene::PlayAnimation()
   */
  bool PlayAnimation( uint32_t index );

  /**
   * @copydoc Dali::Toolkit::Scene::PlayAnimations()
   */
  bool PlayAnimations();

  /**
   * @copydoc Dali::Toolkit::Scene::SetLight( Toolkit::Scene::LightType type, Vector3 lightVector, Vector3 lightColor = Vector3( 1.0, 1.0, 1.0 ) )
   */
  bool SetLight( Toolkit::Scene::LightType type, Vector3 lightVector, Vector3 lightColor = Vector3( 1.0, 1.0, 1.0 ) );

  /**
   * @brief Set default CameraActor.
   * with Dali::Camera::Type = Dali::Camera::LOOK_AT_TARGET,
   * near clipping plane = 0.1,
   * and camera position = Vector3( 0.0, 0.0, 0.0 ).
   */
  bool SetDefaultCamera( const Dali::Camera::Type type = Dali::Camera::LOOK_AT_TARGET, const float nearPlane = 0.1, const Vector3 cameraPosition = Vector3( 0.0, 0.0, 0.0 ) );

  /**
   * @brief Add CameraActor loaded from scene format file.
   */
  void AddCamera( CameraActor& cameraActor );

  /**
   * @brief Add Animation loaded from scene format file.
   */
  void AddAnimation( Animation& animation );

  /**
   * @brief Add new Shader.
   * Actors can share same Shader if they use same properties.
   * If a property changes in a shader, then the property of all actors that use the shader change.
   */
  void AddShader( Shader shader );

  /**
   * @brief Get Root Actor.
   */
  Actor GetRoot();

  /**
   * @copydoc Dali::Toolkit::Scene::GetDefaultCamera()
   */
  CameraActor GetDefaultCamera();

  /**
   * @copydoc Dali::Toolkit::Scene::GetCamera( int cameraIndex = -1 )
   */
  CameraActor GetCamera( int cameraIndex = -1 );

  /**
   * @brief Get light type.
   */
  Toolkit::Scene::LightType GetLightType();

  /**
   * @brief Get light vector.
   * Return light position when light type is LightType::POINT_LIGHT
   * Return light direction when light type is LightType::DIRECTIONAL_LIGHT
   */
  Vector3 GetLightVector();

  /**
   * @brief Get light color.
   */
  Vector3 GetLightColor();

  /**
   * @brief Get Scaling factor of IBL.
   */
  Vector4 GetIBLScaleFactor();

  /**
   * @brief Get BRDF Texture.
   */
  Texture GetBRDFTexture();

  /**
   * @brief Get diffuse cube map texture.
   */
  Texture GetDiffuseTexture();

  /**
   * @brief Get specular cube map texture.
   */
  Texture GetSpecularTexture();

private:
  /**
   * @brief Crop image buffer.
   * For each direction, Offset + faceSize must be width or height or less then them.
   */
  unsigned char* CropBuffer( unsigned char* sourceBuffer, unsigned int bytesPerPixel, unsigned int width, unsigned int height, unsigned int xOffset, unsigned int yOffset, unsigned int xFaceSize, unsigned int yFaceSize );

  /**
   * @brief Upload cube map texture.
   */
  void UploadTextureFace( Texture& texture, Devel::PixelBuffer pixelBuffer, int faceIndex );

  /**
   * @brief Set diffuse and specular cube map textures.
   */
  void SetCubeMap( const std::string& diffuseTexturePath, const std::string& specularTexturePath, Vector4 ScaleFactor = Vector4( 1.0, 1.0, 1.0, 1.0 ) );

  virtual void OnInitialize();


  /**
   * @brief Load 2D texture.
   * @param[in] imageUrl Image URL of the texture.
   * @param[in] generateMipmaps If generateMipmaps is true, then generate mipmap of this texture.
   * @return Texture loaded from imageUrl.
   */
  Texture LoadTexture( const char *imageUrl, bool generateMipmaps );

private:
  Actor mRoot; // Root actor that contains scene graph

  std::vector<Shader> mShaderArray; // Shader Array to change properties of scene such as lighting.

  std::vector<CameraActor> mCameraActorArray; // CameraActer array loaded from scene format file.
  CameraActor mDefaultCamera; // Default CameraActor for the empty mCameraActorArray.

  std::vector<Animation> mAnimationArray; // Animation array loaded from scene format file.

  Toolkit::Scene::LightType mLightType; // Light type
  Vector3 mLightVector; // Light position when mLightType is LightType::POINT_LIGHT
                        // Light direction when mLightType is LightType::DIRECTIONAL_LIGHT
  Vector3 mLightColor; // Light color

  Vector4 mIBLScaleFactor; // IBL scaling factor for the IBL rendering
  Texture mBRDFTexture; // BRDF texture for the PBR rendering
  Texture mSpecularTexture; // Specular cube map texture
  Texture mDiffuseTexture; // Diffuse cube map texture

private:

  // Undefined copy constructor.
  Scene( const Scene& );

  // Undefined assignment operator.
  Scene& operator=( const Scene& );
};

} // namespace Internal

  // Helpers for public-api forwarding methods
inline const Internal::Scene& GetImpl( const Toolkit::Scene& scene )
{
  DALI_ASSERT_ALWAYS( scene && "Scene handle is empty" );
  const Dali::RefObject& handle = scene.GetImplementation();

  return static_cast<const Toolkit::Internal::Scene&>( handle );
}

inline Internal::Scene& GetImpl( Toolkit::Scene& scene )
{
  DALI_ASSERT_ALWAYS( scene && "Scene handle is empty" );

  Dali::RefObject& handle = scene.GetImplementation();

  return static_cast<Toolkit::Internal::Scene&>( handle );
}

}//namespace Toolkit

}//namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_SCENE_H

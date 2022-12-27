#ifndef DALI_TOOLKIT_INTERNAL_SCENE3D_VIEW_H
#define DALI_TOOLKIT_INTERNAL_SCENE3D_VIEW_H

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
#include <dali/devel-api/adaptor-framework/file-loader.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/rendering/shader.h>
#include <dali/public-api/rendering/texture.h>
#include <cstring>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/scene3d-view/scene3d-view.h>
#include <dali-toolkit/public-api/controls/control-impl.h>

namespace Dali
{
namespace Toolkit
{
class Scene3dView;

namespace Internal
{
namespace Gltf
{
class Loader;

}

/**
 * Scene3dView implementation class
 */
class Scene3dView : public Control
{
public:
  enum CubeType
  {
    CROSS_HORIZONTAL = 0, // Cross horizontal style cube map
    ARRAY_HORIZONTAL,     // array horizontal style cube map
    NONE
  };

  /**
   * @copydoc Dali::Toolkit::Scene3dView::Scene3dView
   */
  Scene3dView();

  /**
   * @copydoc Dali::Toolkit::Scene3dView::~Scene3dView
   */
  virtual ~Scene3dView();

  /**
   * @copydoc Dali::Toolkit::Scene3dView::New( const std::string& filePath )
   */
  static Dali::Toolkit::Scene3dView New(const std::string& filePath);

  /**
   * @copydoc Dali::Toolkit::Scene3dView::New( const std::string& filePath, const std::string& diffuseTexturePath, const std::string& specularTexturePath, Vector4 scaleFactor )
   */
  static Dali::Toolkit::Scene3dView New(const std::string& filePath, const std::string& diffuseTexturePath, const std::string& specularTexturePath, Vector4 scaleFactor);

  /**
   * @copydoc Dali::Toolkit::Scene3dView::CreateScene()
   */
  bool CreateScene();

  /**
   * @copydoc Dali::Toolkit::Scene3dView::GetAnimationCount()
   */
  uint32_t GetAnimationCount();

  /**
   * @copydoc Dali::Toolkit::Scene3dView::PlayAnimation()
   */
  bool PlayAnimation(uint32_t index);

  /**
   * @copydoc Dali::Toolkit::Scene3dView::PlayAnimations()
   */
  bool PlayAnimations();

  /**
   * @copydoc Dali::Toolkit::Scene3dView::SetLight( Toolkit::Scene3dView::LightType type, Vector3 lightVector, Vector3 lightColor )
   */
  bool SetLight(Toolkit::Scene3dView::LightType type, Vector3 lightVector, Vector3 lightColor);

  /**
   * @brief Set default CameraActor specified in the each scene format specification.
   * Default input values are derived from glTF default camera format.
   * with Dali::Camera::Type = Dali::Camera::LOOK_AT_TARGET,
   * near clipping plane = 0.1,
   * and camera position = Vector3( 0.0, 0.0, 0.0 ).
   */
  bool SetDefaultCamera(const Dali::Camera::Type type = Dali::Camera::LOOK_AT_TARGET, const float nearPlane = 0.1, const Vector3 cameraPosition = Vector3(0.0, 0.0, 0.0));

  /**
   * @brief Add CameraActor loaded from scene format file.
   */
  void AddCamera(CameraActor cameraActor);

  /**
   * @brief Add Animation loaded from scene format file.
   */
  void AddAnimation(Animation animation);

  /**
   * @brief Add new Shader.
   * Actors can share same Shader if they use same properties.
   * If a property changes in a shader, then the property of all actors that use the shader change.
   */
  void AddShader(Shader shader);

  /**
   * @brief Get Root Actor.
   */
  Actor GetRoot();

  /**
   * @copydoc Dali::Toolkit::Scene3dView::GetDefaultCamera()
   */
  CameraActor GetDefaultCamera();

  /**
   * @copydoc Dali::Toolkit::Scene3dView::GetCameraCount()
   */
  uint32_t GetCameraCount();

  /**
   * @copydoc Dali::Toolkit::Scene3dView::GetCamera( uint32_t cameraIndex )
   */
  CameraActor GetCamera(uint32_t cameraIndex);

  /**
   * @brief Get light type.
   */
  Toolkit::Scene3dView::LightType GetLightType();

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

  /**
   * @brief Get whether the scene has image based rendering or not.
   */
  bool HasImageBasedLighting();

private:
  /**
   * @brief Get Cropped image buffer.
   * For each direction, Offset + faceSize must be width or height or less then them.
   */
  uint8_t* GetCroppedBuffer(uint8_t* sourceBuffer, uint32_t bytesPerPixel, uint32_t width, uint32_t height, uint32_t xOffset, uint32_t yOffset, uint32_t xFaceSize, uint32_t yFaceSize);

  /**
   * @brief Upload cube map texture.
   */
  void UploadTextureFace(Texture& texture, Devel::PixelBuffer pixelBuffer, uint32_t faceIndex);

  /**
   * @brief Set diffuse and specular cube map textures.
   */
  void SetCubeMap(const std::string& diffuseTexturePath, const std::string& specularTexturePath, Vector4 scaleFactor = Vector4(1.0, 1.0, 1.0, 1.0));

  virtual void OnInitialize();

  /**
   * @brief Load 2D texture.
   * @param[in] imageUrl Image URL of the texture.
   * @param[in] generateMipmaps If generateMipmaps is true, then generate mipmap of this texture.
   * @return Texture loaded from imageUrl.
   */
  Texture LoadTexture(const char* imageUrl, bool generateMipmaps);

private:
  Actor       mRoot;     // Root actor that contains scene graph
  std::string mFilePath; // Full file path of scene file

  std::vector<Shader> mShaderArray; // Shader Array to change properties of scene such as lighting.

  std::vector<CameraActor> mCameraActorArray; // CameraActer array loaded from scene format file.
  CameraActor              mDefaultCamera;    // Default CameraActor for the empty mCameraActorArray.

  std::vector<Animation> mAnimationArray; // Animation array loaded from scene format file.

  Toolkit::Scene3dView::LightType mLightType;   // Light type
  Vector3                         mLightVector; // Light position when mLightType is LightType::POINT_LIGHT
                                                // Light direction when mLightType is LightType::DIRECTIONAL_LIGHT
  Vector3 mLightColor;                          // Light color

  Vector4 mIBLScaleFactor;  // IBL scaling factor for the IBL rendering
  Texture mBRDFTexture;     // BRDF texture for the PBR rendering
  Texture mSpecularTexture; // Specular cube map texture
  Texture mDiffuseTexture;  // Diffuse cube map texture
  bool    mUseIBL;

private:
  // Undefined copy constructor.
  Scene3dView(const Scene3dView&);

  // Undefined assignment operator.
  Scene3dView& operator=(const Scene3dView&);
};

} // namespace Internal

// Helpers for public-api forwarding methods
inline const Internal::Scene3dView& GetImpl(const Toolkit::Scene3dView& scene3dView)
{
  DALI_ASSERT_ALWAYS(scene3dView && "Scene3dView handle is empty");
  const Dali::RefObject& handle = scene3dView.GetImplementation();

  return static_cast<const Toolkit::Internal::Scene3dView&>(handle);
}

inline Internal::Scene3dView& GetImpl(Toolkit::Scene3dView& scene3dView)
{
  DALI_ASSERT_ALWAYS(scene3dView && "Scene3dView handle is empty");

  Dali::RefObject& handle = scene3dView.GetImplementation();

  return static_cast<Toolkit::Internal::Scene3dView&>(handle);
}

} //namespace Toolkit

} //namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_SCENE3D_VIEW_H

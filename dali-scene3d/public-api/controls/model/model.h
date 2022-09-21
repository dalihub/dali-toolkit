#ifndef DALI_SCENE3D_MODEL_H
#define DALI_SCENE3D_MODEL_H

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

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>

// EXTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>
#include <dali/public-api/rendering/texture.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal DALI_INTERNAL
{
class Model;
}

/**
 * @addtogroup dali_toolkit_controls_model
 * @{
 */

/**
 * @brief Model is a control to show 3D model objects.
 * Model supports to load glTF 2.0 and DLI models for the input format
 * and also supports Physically Based Rendering with Image Based Lighting.
 *
 * The Animations defined in the glTF or DLI models are also loaded and can be retrieved by using GetAnimation() method.
 * The number of animation is also retrieved by GetAnimationCount() method.
 *
 * By default, The loaded model has its own position and size which are defined in vertex buffer regardless of the Control size.
 *
 * @code
 *
 * Model model = Model::New(modelUrl);
 * model.SetProperty(Dali::Actor::Property::SIZE, Vector2(width, height));
 * model.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
 * model.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
 * model.SetImageBasedLightSource(diffuseUrl, specularUrl, scaleFactor);
 * window.Add(model);
 * uint32_t animationCount = model.GetAnimationCount();
 * Dali::Animation animation = model.GetAnimation(0);
 * animation.Play();
 *
 * @endcode
 */
class DALI_SCENE3D_API Model : public Dali::Toolkit::Control
{
public:
  /**
   * @brief Create an initialized Model.
   * @param[in] modelUrl model file path.(e.g., glTF, and DLI).
   * @param[in] resourceDirectoryUrl resource file path that includes binary, image etc.
   * @note If resourceDirectoryUrl is empty, the parent directory path of modelUrl is used for resource path.
   * @return A handle to a newly allocated Dali resource
   */
  static Model New(const std::string& modelUrl, const std::string& resourceDirectoryUrl = std::string());

  /**
   * @brief Creates an uninitialized Model.
   *
   * Only derived versions can be instantiated. Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   */
  Model();

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Model();

  /**
   * @brief Copy constructor.
   * @param[in] model Handle to an object
   */
  Model(const Model& model);

  /**
   * @brief Move constructor
   *
   * @param[in] rhs A reference to the moved handle
   */
  Model(Model&& rhs);

  /**
   * @brief Assignment operator.
   * @param[in] model Handle to an object
   * @return reference to this
   */
  Model& operator=(const Model& model);

  /**
   * @brief Move assignment
   *
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  Model& operator=(Model&& rhs);

  /**
   * @brief Downcasts an Object handle to Model.
   *
   * If handle points to a Model, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return Handle to a Model or an uninitialized handle
   */
  static Model DownCast(BaseHandle handle);

  /**
   * @brief Retrieves model root Actor.
   * @return Root Actor of the model.
   */
  const Actor GetModelRoot() const;

  /**
   * @brief Changes Image Based Light as the input textures.
   * @param[in] diffuseUrl cube map that can be used as a diffuse IBL source.
   * @param[in] specularUrl cube map that can be used as a specular IBL source.
   * @param[in] scaleFactor scale factor that controls light source intensity in [0.0f, 1.0f]. Default value is 1.0f.
   */
  void SetImageBasedLightSource(const std::string& diffuseUrl, const std::string& specularUrl, float scaleFactor = 1.0f);

  /**
   * @brief Sets Image Based Light Texture.
   * @note This method doesn't load texture from file, so this work is performed synchronously.
   *
   * @param[in] diffuseTexture cube map texture that can be used as a diffuse IBL source.
   * @param[in] specularTexture cube map texture that can be used as a specular IBL source.
   * @param[in] scaleFactor scale factor that controls light source intensity in [0.0f, 1.0f]. Default value is 1.0f.
   *
   * @note Both of diffuse texture and specular texture should be available. If not, nothing applied.
   */
  void SetImageBasedLightTexture(Texture diffuseTexture, Texture specularTexture, float scaleFactor = 1.0f);

  /**
   * @brief Sets Scale Factor of Image Based Light Source.
   *
   * @note If SetImageBasedLightSource() or SetImageBasedLightTexture() method is called after this method, scaleFactor is overrided.
   *
   * @param[in] scaleFactor scale factor that controls light source intensity in [0.0f, 1.0f].
   */
  void SetImageBasedLightScaleFactor(float scaleFactor);

  /**
   * @brief Gets Scale Factor of Image Based Light Source.
   * Default value is 1.0f.
   *
   * @return scale factor that controls light source intensity.
   */
  float GetImageBasedLightScaleFactor() const;

  /**
   * @brief Gets number of animations those loaded from model file.
   * @return The number of loaded animations.
   * @note This method should be called after Model load finished.
   */
  uint32_t GetAnimationCount() const;

  /**
   * @brief Gets animation at the index.
   * @param[in] index Index of animation to be retrieved.
   * @return Animation at the index.
   * @note This method should be called after Model load finished.
   */
  Dali::Animation GetAnimation(uint32_t index) const;

  /**
   * @brief Retrieves animation with the given name.
   * @param[in] name string name of animation to be retrieved.
   * @return Animation that has the given name.
   * @note This method should be called after Model load finished.
   */
  Dali::Animation GetAnimation(const std::string& name) const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in] implementation The Control implementation
   */
  DALI_INTERNAL Model(Internal::Model& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  DALI_INTERNAL Model(Dali::Internal::CustomActor* internal);
  /// @endcond
};

/**
 * @}
 */
} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_H

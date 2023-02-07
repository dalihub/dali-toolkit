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
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/rendering/texture.h>

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
 * @SINCE_2_1.41
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
   *
   * @SINCE_2_1.41
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
   *
   * @SINCE_2_1.41
   */
  Model();

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   *
   * @SINCE_2_1.41
   */
  ~Model();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_2_1.41
   * @param[in] model Handle to an object
   */
  Model(const Model& model);

  /**
   * @brief Move constructor
   *
   * @SINCE_2_1.41
   * @param[in] rhs A reference to the moved handle
   */
  Model(Model&& rhs);

  /**
   * @brief Assignment operator.
   *
   * @SINCE_2_1.41
   * @param[in] model Handle to an object
   * @return reference to this
   */
  Model& operator=(const Model& model);

  /**
   * @brief Move assignment
   *
   * @SINCE_2_1.41
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
   * @SINCE_2_1.41
   * @param[in] handle Handle to an object
   * @return Handle to a Model or an uninitialized handle
   */
  static Model DownCast(BaseHandle handle);

  /**
   * @brief Retrieves model root Actor.
   *
   * @SINCE_2_1.41
   * @return Root Actor of the model.
   */
  const Actor GetModelRoot() const;

  /**
   * @brief Whether allow this model's children actor to use events.
   *
   * Usually, 3D Model might have a lot of actors. And most of them don't need to check events.
   * To optimize traversal, we need to setup some flag that this model don't allow (or allow) to traversal
   * children so that child can use events.
   *
   * @SINCE_2_1.43
   * @note Even if we set children sensitive as false, model itself's sensitive
   * is depend on it's property.
   * @note If we don't call this API, default is false. (Don't allow to traversal model's children during hit-test)
   *
   * @param[in] enable True to enable model's children can use events.
   */
  void SetChildrenSensitive(bool enable);

  /**
   * @brief Gets whether this Model allow model's children actor to use events or not.
   *
   * @SINCE_2_1.43
   * @return bool True if this Model allow model children sensitive.
   */
  bool GetChildrenSensitive() const;

  /**
   * @brief Whether allow this model's children actor to be keyboard focusable.
   *
   * Usually, 3D Model might have a lot of actors. And most of them don't need to check focusable.
   * To optimize traversal, we need to setup some flag that this model don't allow (or allow) to traversal
   * children so that child can be keyboard focusable.
   *
   * @SINCE_2_2.2
   * @note Even if we set children focusable as false, model itself's focusable
   * is depend on it's property.
   * @note If we don't call this API, default is false. (Don't allow to traversal model's children during focusable test)
   *
   * @param[in] enable True to enable model's children can be focusable.
   */
  void SetChildrenFocusable(bool enable);

  /**
   * @brief Gets whether this Model allow model's children actor to be keyboard focusable or not.
   *
   * @SINCE_2_2.2
   * @return bool True if this Model allow model children are focusable.
   */
  bool GetChildrenFocusable() const;

  /**
   * @brief Changes Image Based Light as the input textures.
   *
   * @SINCE_2_1.41
   * @param[in] diffuseUrl cube map that can be used as a diffuse IBL source.
   * @param[in] specularUrl cube map that can be used as a specular IBL source.
   * @param[in] scaleFactor scale factor that controls light source intensity in [0.0f, 1.0f]. Default value is 1.0f.
   */
  void SetImageBasedLightSource(const std::string& diffuseUrl, const std::string& specularUrl, float scaleFactor = 1.0f);

  /**
   * @brief Sets Image Based Light Texture.
   *
   * @SINCE_2_1.41
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
   * @SINCE_2_1.41
   * @note If SetImageBasedLightSource() or SetImageBasedLightTexture() method is called after this method, scaleFactor is overrided.
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
   * @brief Gets number of animations those loaded from model file.
   *
   * @SINCE_2_1.41
   * @return The number of loaded animations.
   * @note This method should be called after Model load finished.
   */
  uint32_t GetAnimationCount() const;

  /**
   * @brief Gets animation at the index.
   *
   * @SINCE_2_1.41
   * @param[in] index Index of animation to be retrieved.
   * @return Animation at the index.
   * @note This method should be called after Model load finished.
   */
  Dali::Animation GetAnimation(uint32_t index) const;

  /**
   * @brief Retrieves animation with the given name.
   *
   * @SINCE_2_1.41
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

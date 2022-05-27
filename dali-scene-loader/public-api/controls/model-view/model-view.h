#ifndef DALI_SCENE_LOADER_MODEL_VIEW_H
#define DALI_SCENE_LOADER_MODEL_VIEW_H

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
#include <dali-scene-loader/public-api/api.h>

// EXTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal DALI_INTERNAL
{
class ModelView;
}

/**
 * @addtogroup dali_toolkit_controls_model_view
 * @{
 */

/**
 * @brief ModelView is a control to show 3D model objects.
 * ModelView supports to load glTF 2.0 and DLI models for the input format
 * and also supports Physically Based Rendering with Image Based Lighting.
 *
 * The Animations defined in the glTF or DLI models are also loaded and can be retrieved by using GetAnimation() method.
 * The number of animation is also retrieved by GetAnimationCount() method.
 *
 * By default, The loaded model has it's own position and size which are defined in vertex buffer regardless of the Control size.
 * The model can be resized and repositioned to fit to the ModelView Control with FitSize() and FitCenter() methods.
 *
 * @code
 *
 * ModelView modelView = ModelView::New(modelUrl);
 * modelView.SetProperty(Dali::Actor::Property::SIZE, Vector2(width, height));
 * modelView.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
 * modelView.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
 * modelView.SetImageBasedLightSource(diffuseUrl, specularUrl, scaleFactor);
 * modelView.FitSize(true);
 * modelView.FitCenter(true);
 * window.Add(modelView);
 * uint32_t animationCount = modelView.GetAnimationCount();
 * Dali::Animation animation = modelView.GetAnimation(0);
 * animation.Play();
 *
 * @endcode
 */
class DALI_SCENE_LOADER_API ModelView : public Dali::Toolkit::Control
{
public:
  /**
   * @brief Create an initialized ModelView.
   * @param[in] modelPath model file path.(e.g., glTF, and DLI).
   * @param[in] resourcePath resource file path that includes binary, image etc.
   * @note If resourcePath is empty, the parent directory path of modelPath is used for resource path.
   * @return A handle to a newly allocated Dali resource
   */
  static ModelView New(const std::string& modelPath, const std::string& resourcePath = std::string());

  /**
   * @brief Creates an uninitialized ModelView.
   *
   * Only derived versions can be instantiated. Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   */
  ModelView();

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~ModelView();

  /**
   * @brief Copy constructor.
   * @param[in] modelView Handle to an object
   */
  ModelView(const ModelView& modelView);

  /**
   * @brief Move constructor
   *
   * @param[in] rhs A reference to the moved handle
   */
  ModelView(ModelView&& rhs);

  /**
   * @brief Assignment operator.
   * @param[in] modelView Handle to an object
   * @return reference to this
   */
  ModelView& operator=(const ModelView& modelView);

  /**
   * @brief Move assignment
   *
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  ModelView& operator=(ModelView&& rhs);

  /**
   * @brief Downcasts an Object handle to ModelView.
   *
   * If handle points to a ModelView, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return Handle to a ModelView or an uninitialized handle
   */
  static ModelView DownCast(BaseHandle handle);

  /**
   * @brief Retrieves model root Actor.
   * @return Root Actor of the model.
   */
  const Actor GetModelRoot();

  /**
   * @brief Fits the model to the Control size.
   * @param[in] fit true to fit model size to control.
   * @note This method makes model fit to the Control size by keeping original model ratio.
   * It means If model size is (2, 2, 2) and ModelView size is (10, 8), then the model become looks (8, 8, 8).
   * If ModelView Size x or y is 0, this method don't work anything.
   * If ModelView Size z is 0, this method considers only x and y values of ModelView Size.
   */
  void FitSize(bool fit);

  /**
   * @brief Moves the model to the center of control.
   * @param[in] fit true to fit model to center of control.
   * @note This method doesn't changes size of model.
   */
  void FitCenter(bool fit);

  /**
   * @brief Changes Image Based Light as the input textures.
   * @param[in] diffuse cube map that can be used as a diffuse IBL source.
   * @param[in] specular cube map that can be used as a specular IBL source.
   * @param[in] scaleFactor scale factor that controls light source intensity in [0.0f, 1.0f]. Default value is 1.0f.
   */
  void SetImageBasedLightSource(const std::string& diffuse, const std::string& specular, float scaleFactor = 1.0f);

  /**
   * @brief Gets number of animations those loaded from model file.
   * @return The number of loaded animations.
   * @note This method should be called after Model load finished.
   */
  uint32_t GetAnimationCount();

  /**
   * @brief Gets animation at the index.
   * @param[in] index Index of animation to be retrieved.
   * @return Animation at the index.
   * @note This method should be called after Model load finished.
   */
  Dali::Animation GetAnimation(uint32_t index);

  /**
   * @brief Retrieves animation with the given name.
   * @param[in] name string name of animation to be retrieved.
   * @return Animation that has the given name.
   * @note This method should be called after Model load finished.
   */
  Dali::Animation GetAnimation(const std::string& name);

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in] implementation The Control implementation
   */
  DALI_INTERNAL ModelView(Internal::ModelView& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  DALI_INTERNAL ModelView(Dali::Internal::CustomActor* internal);
  /// @endcond
};

/**
 * @}
 */
} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE_LOADER_MODEL_VIEW_H

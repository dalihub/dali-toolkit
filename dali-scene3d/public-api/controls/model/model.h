#ifndef DALI_SCENE3D_MODEL_H
#define DALI_SCENE3D_MODEL_H

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
#include <dali/public-api/rendering/texture.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>
#include <dali-scene3d/public-api/model-components/model-node.h>

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
   * @note If modelUrl is empty, it will not load resouces. Only ModelRoot will be created.
   * @note If resourceDirectoryUrl is empty, the parent directory path of modelUrl is used for resource path.
   * @return A handle to a newly allocated Dali resource
   */
  static Model New(const std::string& modelUrl = std::string(), const std::string& resourceDirectoryUrl = std::string());

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
   * @brief Retrieves model root Node.
   *
   * @SINCE_2_1.41
   * @return Root Node of the model.
   */
  const ModelNode GetModelRoot() const;

  /**
   * @brief Add new ModelNode to this Model.
   * This modelNode will become child of ModelRoot.
   *
   * @SINCE_2_2.99
   * @param[in] modelNode the root of ModelNode tree to be added.
   */
  void AddModelNode(ModelNode modelNode);

  /**
   * @brief Remove ModelNode from this Model.
   *
   * @SINCE_2_2.99
   * @param[in] modelNode the root of ModelNode tree to be removed.
   */
  void RemoveModelNode(ModelNode modelNode);

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
   * @brief Sets Scale Factor of Image Based Light Source.
   *
   * @SINCE_2_1.41
   * @note If SetImageBasedLightSource() method is called after this method, scaleFactor is overrided.
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

  /**
   * @brief Gets number of camera parameters those loaded from model file.
   *
   * @SINCE_2_2.15
   * @return The number of loaded camera parameters.
   * @note This method should be called after Model load finished.
   */
  uint32_t GetCameraCount() const;

  /**
   * @brief Generate camera actor using camera parameters at the index.
   * If camera parameter is valid, create new CameraActor.
   * Camera parameter decide at initialized time and
   * didn't apply model node's current position (like Animation).
   *
   * @SINCE_2_2.15
   * @param[in] index Index of camera to be used for generation camera.
   * @return Generated CameraActor by the index, or empty Handle if generation failed.
   * @note This method should be called after Model load finished.
   */
  Dali::CameraActor GenerateCamera(uint32_t index) const;

  /**
   * @brief Apply camera parameters at the index to inputed camera actor.
   * If camera parameter is valid and camera actor is not empty, apply parameters.
   * It will change camera's transform and near / far / fov or orthographic size / aspect ratio (if defined)
   * Camera parameter decide at initialized time and
   * didn't apply model node's current position (like Animation).
   *
   * @SINCE_2_2.15
   * @param[in] index Index of camera to be used for generation camera.
   * @param[in,out] camera Index of camera to be used for generation camera.
   * @return True if apply successed. False otherwise.
   * @note This method should be called after Model load finished.
   */
  bool ApplyCamera(uint32_t index, Dali::CameraActor camera) const;

  /**
   * @brief Returns a child ModelNode object with a name that matches nodeName.
   *
   * @SINCE_2_2.34
   * @param[in] nodeName The name of the child ModelNode object you want to find.
   * @return Returns a child ModelNode object with a name that matches nodeName. If there is no corresponding child ModelNode object, it returns an empty ModelNode object.
   */
  ModelNode FindChildModelNodeByName(std::string_view nodeName);

  /**
   * @brief Retrieve the list of blendshape name that current Model hold.
   * The name will be appended end of input list.
   *
   * @SINCE_2_2.34
   * @param[in, out] blendShapeNames The name of blendShape list collected.
   * @note This method should be called after Model load finished.
   */
  void RetrieveBlendShapeNames(std::vector<std::string>& blendShapeNames) const;

  /**
   * @brief Retrieve the list of ModelNode that contains given blend shape name.
   * The ModelNode will be appended end of input list.
   *
   * @SINCE_2_2.34
   * @param[in] blendShapeName The name of blendShape that want to collect.
   * @param[in, out] modelNodes The ModelNode list collected.
   * @note This method should be called after Model load finished.
   */
  void RetrieveModelNodesByBlendShapeName(std::string_view blendShapeName, std::vector<ModelNode>& modelNodes) const;

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

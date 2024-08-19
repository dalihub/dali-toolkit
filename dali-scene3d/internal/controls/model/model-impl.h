#ifndef DALI_SCENE3D_INTERNAL_MODEL_H
#define DALI_SCENE3D_INTERNAL_MODEL_H

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
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali/devel-api/common/map-wrapper.h>
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/object/property-notification.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/rendering/texture.h>
#include <unordered_map>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/common/environment-map-load-task.h>
#include <dali-scene3d/internal/common/light-observer.h>
#include <dali-scene3d/internal/common/model-load-task.h>
#include <dali-scene3d/internal/event/collider-mesh-processor.h>
#include <dali-scene3d/internal/model-components/model-node-impl.h>
#include <dali-scene3d/public-api/controls/model/model.h>
#include <dali-scene3d/public-api/controls/scene-view/scene-view.h>
#include <dali-scene3d/public-api/light/light.h>
#include <dali-scene3d/public-api/loader/load-result.h>
#include <dali-scene3d/public-api/model-components/model-node.h>

namespace Dali
{
namespace Scene3D
{
class Model;

namespace Internal
{
/**
 * @brief Impl class for Model.
 */
class Model : public Dali::Toolkit::Internal::Control, public LightObserver, public Dali::Scene3D::Collidable
{
public:
  using AnimationData          = std::pair<std::string, Dali::Animation>;
  using CameraData             = Loader::CameraParameters;
  using BlendShapeModelNodeMap = std::map<std::string, std::vector<Scene3D::ModelNode>>;

  // ColliderMeshContainer doesn't hold actual collider meshes
  // but pairs unique ModelNode id with ModelNode for lookup purposes.
  // All model nodes in the container have collider mesh attached.
  using ColliderMeshContainer = std::unordered_map<int, Scene3D::ModelNode>;

  /**
   * @copydoc Model::New()
   */
  static Dali::Scene3D::Model New(const std::string& modelUrl, const std::string& resourceDirectoryUrl);

  /**
   * @copydoc Model::GetModelRoot()
   */
  const Scene3D::ModelNode GetModelRoot() const;

  /**
   * @copydoc Model::AddModelNode()
   */
  void AddModelNode(Scene3D::ModelNode modelNode);

  /**
   * @copydoc Model::RemoveModelNode()
   */
  void RemoveModelNode(Scene3D::ModelNode modelNode);

  /**
   * @copydoc Model::SetChildrenSensitive()
   */
  void SetChildrenSensitive(bool enable);

  /**
   * @copydoc Model::GetChildrenSensitive()
   */
  bool GetChildrenSensitive() const;

  /**
   * @copydoc Model::SetChildrenFocusable()
   */
  void SetChildrenFocusable(bool enable);

  /**
   * @copydoc Model::GetChildrenFocusable()
   */
  bool GetChildrenFocusable() const;

  /**
   * @copydoc Model::SetImageBasedLightSource()
   */
  void SetImageBasedLightSource(const std::string& diffuseUrl, const std::string& specularUrl, float scaleFactor);

  /**
   * @copydoc Model::SetImageBasedLightScaleFactor()
   */
  void SetImageBasedLightScaleFactor(float scaleFactor);

  /**
   * @copydoc Model::GetImageBasedLightScaleFactor()
   */
  float GetImageBasedLightScaleFactor() const;

  /**
   * @copydoc Model::GetAnimationCount()
   */
  uint32_t GetAnimationCount() const;

  /**
   * @copydoc Model::GetAnimation()
   */
  Dali::Animation GetAnimation(uint32_t index) const;

  /**
   * @copydoc Model::GetAnimation()
   */
  Dali::Animation GetAnimation(const std::string& name) const;

  /**
   * @copydoc Model::GetCameraCount()
   */
  uint32_t GetCameraCount() const;

  /**
   * @copydoc Model::GenerateCamera()
   */
  Dali::CameraActor GenerateCamera(uint32_t index) const;

  /**
   * @copydoc Model::ApplyCamera()
   */
  bool ApplyCamera(uint32_t index, Dali::CameraActor camera) const;

  /**
   * @copydoc Model::FindChildModelNodeByName()
   */
  Scene3D::ModelNode FindChildModelNodeByName(std::string_view nodeName);

  /**
   * @copydoc Model::RetrieveBlendShapeNames()
   */
  void RetrieveBlendShapeNames(std::vector<std::string>& blendShapeNames) const;

  /**
   * @copydoc Model::RetrieveModelNodesByBlendShapeName()
   */
  void RetrieveModelNodesByBlendShapeName(std::string_view blendShapeName, std::vector<Scene3D::ModelNode>& modelNodes) const;

  /**
   * @copydoc Model::GenerateMotionDataAnimation()
   */
  Dali::Animation GenerateMotionDataAnimation(Scene3D::MotionData motionData);

  /**
   * @copydoc Model::SetMotionData()
   */
  void SetMotionData(Scene3D::MotionData motionData);

  /**
   * @copydoc Model::CastShadow()
   */
  void CastShadow(bool castShadow);

  /**
   * @copydoc Model::IsShadowCasting()
   */
  bool IsShadowCasting() const;

  /**
   * @copydoc Model::ReceiveShadow()
   */
  void ReceiveShadow(bool receiveShadow);

  /**
   * @copydoc Model::IsShadowReceiving()
   */
  bool IsShadowReceiving() const;

  /**
   * @copydoc Scene3D::Model::MeshHitSignal()
   */
  Scene3D::Model::MeshHitSignalType& MeshHitSignal()
  {
    return mMeshHitSignal;
  }

  /**
   * @brief Emits MeshHitSignal
   * @param[in] modelNode ModelNode that has been hit
   * @return Result of hit handling.
   */
  bool EmitMeshHitSignal(Scene3D::ModelNode modelNode)
  {
    bool retVal = false;
    if(!mMeshHitSignal.Empty())
    {
      Scene3D::Model handle(GetOwner());
      retVal = mMeshHitSignal.Emit(handle, modelNode);
    }
    return retVal;
  }

  /**
   * @brief Returns collider mesh container
   * @return Returns valid container
   */
  const ColliderMeshContainer& GetNodeColliderMeshContainer() const
  {
    return mColliderMeshes;
  }

  /**
   * @brief Registers child node with collidier mesh
   *
   * @param[in] node ModelNode to register
   */
  void RegisterColliderMesh(Scene3D::ModelNode& node);

  /**
   * @brief Removes node/collider mesh from the register
   * @param[in] node Child node to remove from the register
   */
  void RemoveColliderMesh(Scene3D::ModelNode& node);

protected:
  /**
   * @brief Constructs a new Model.
   * @param[in] modelUrl model file path.(e.g., glTF, and DLI).
   * @param[in] resourceDirectoryUrl resource file path that includes binary, image etc.
   */
  Model(const std::string& modelUrl, const std::string& resourceDirectoryUrl);

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Model();

private:
  /**
   * @copydoc Toolkit::Control::OnInitialize
   */
  void OnInitialize();

  /**
   * @copydoc CustomActorImpl::OnSceneConnection()
   */
  void OnSceneConnection(int depth) override;

  /**
   * @copydoc CustomActorImpl::OnSceneDisconnection()
   */
  void OnSceneDisconnection() override;

  /**
   * @copydoc CustomActorImpl::OnSizeSet( const Vector3& size )
   */
  void OnSizeSet(const Vector3& size) override;

  /**
   * @copydoc Toolkit::Control::GetNaturalSize
   */
  Vector3 GetNaturalSize() override;

  /**
   * @copydoc Toolkit::Control::GetHeightForWidth()
   */
  float GetHeightForWidth(float width) override;

  /**
   * @copydoc Toolkit::Control::GetWidthForHeight()
   */
  float GetWidthForHeight(float height) override;

  /**
   * @copydoc Toolkit::Control::OnRelayout()
   */
  void OnRelayout(const Vector2& size, RelayoutContainer& container) override;

  /**
   * @copydoc Toolkit::Control::IsResourceReady()
   */
  bool IsResourceReady() const override;

private:
  /**
   * @brief Create Model Root of this Model class.
   */
  void CreateModelRoot();

  /**
   * @brief Scales the model to fit the control or to return to original size.
   */
  void ScaleModel(bool useCurrentSize);

  /**
   * @brief Changes model anchor point to set the model at center or returns to the original model pivot.
   */
  void FitModelPosition();

  /**
   * @brief Changes IBL textures of the input node.
   */
  void UpdateImageBasedLightTexture();

  /**
   * @brief Changes IBL scale factor of the input node.
   */
  void UpdateImageBasedLightScaleFactor();

  /**
   * @brief Apply self transform into inputed camera.
   * Inputed camera must be configured by CameraParameter. Mean, inputed camera coordinate depend on Model.
   * After this API finished, CameraActor coordinate system converted as DALi coordinate system.
   *
   * @param[in,out] camera CameraActor who need to apply model itself's transform
   */
  void ApplyCameraTransform(Dali::CameraActor camera) const;

public: // Overrides LightObserver Methods.
  /**
   * @copydoc Dali::Scene3D::Internal::LightObserver::NotifyShadowMapTexture()
   */
  void NotifyShadowMapTexture(Dali::Texture shadowMapTexture) override;

  /**
   * @copydoc Dali::Scene3D::Internal::LightObserver::NotifyImageBasedLightTexture()
   */
  void NotifyImageBasedLightTexture(Dali::Texture diffuseTexture, Dali::Texture specularTexture, float scaleFactor, uint32_t specularMipmapLevels) override;

  /**
   * @copydoc Dali::Scene3D::Internal::LightObserver::NotifyImageBasedLightScaleFactor()
   */
  void NotifyImageBasedLightScaleFactor(float scaleFactor) override;

private:
  /**
   * @brief Asynchronously model loading finished.
   */
  void OnModelLoadComplete();

  /**
   * @brief Asynchronously ibl diffusel image loading finished.
   */
  void OnIblDiffuseLoadComplete();

  /**
   * @brief Asynchronously ibl specular image loading finished.
   */
  void OnIblSpecularLoadComplete();

  /**
   * @brief Asynchronously ibl loading finished.
   */
  void OnIblLoadComplete();

  /**
   * @brief Update model root scale when Model size property is updated.
   */
  void OnSizeNotification(Dali::PropertyNotification& source);

  /**
   * @brief Reset Resource loading tasks.
   */
  void ResetResourceTasks();

  /**
   * @brief Notify Resource Ready signal.
   */
  void NotifyResourceReady();

  /**
   * @brief Create Model from loaded SceneDefinition.
   */
  void CreateModel();

  /**
   * @brief Create Dali::Animation from loaded AnimationDefinitions.
   */
  void CreateAnimations(Dali::Scene3D::Loader::SceneDefinition& scene);

  /**
   * @brief Reset CameraData from loaded CameraParameters.
   */
  void ResetCameraParameters();

  /**
   * @brief Collect ModelNode list by blendshape name
   */
  void UpdateBlendShapeNodeMap();

private:
  std::string                    mModelUrl;
  std::string                    mResourceDirectoryUrl;
  Scene3D::ModelNode             mModelRoot;
  std::vector<AnimationData>     mAnimations;
  std::vector<CameraData>        mCameraParameters;
  WeakHandle<Scene3D::SceneView> mParentSceneView;
  Dali::PropertyNotification     mSizeNotification;

  // Signals
  Scene3D::Model::MeshHitSignalType mMeshHitSignal;

  Dali::Scene3D::Loader::ShaderManagerPtr mShaderManager;

  ColliderMeshContainer mColliderMeshes;

  // List of ModelNode for name of blend shape.
  BlendShapeModelNodeMap mBlendShapeModelNodeMap;

  // Asynchronous loading variable
  ModelLoadTaskPtr          mModelLoadTask;
  EnvironmentMapLoadTaskPtr mIblDiffuseLoadTask;
  EnvironmentMapLoadTaskPtr mIblSpecularLoadTask;

  // Shadow
  Dali::Texture mShadowMapTexture;

  std::string mDiffuseIblUrl;
  std::string mSpecularIblUrl;

  Dali::Texture mDefaultSpecularTexture;
  Dali::Texture mDefaultDiffuseTexture;
  Dali::Texture mSceneSpecularTexture;
  Dali::Texture mSceneDiffuseTexture;
  Dali::Texture mSpecularTexture;
  Dali::Texture mDiffuseTexture;
  Vector3       mNaturalSize;
  Vector3       mModelPivot;
  float         mSceneIblScaleFactor;
  float         mIblScaleFactor;
  uint32_t      mSceneSpecularMipmapLevels;
  uint32_t      mSpecularMipmapLevels;
  bool          mModelChildrenSensitive;
  bool          mModelChildrenFocusable;
  bool          mModelResourceReady;
  bool          mIblDiffuseResourceReady;
  bool          mIblSpecularResourceReady;
  bool          mIblDiffuseDirty;
  bool          mIblSpecularDirty;
  bool          mIsShadowCasting;
  bool          mIsShadowReceiving;
};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Dali::Scene3D::Internal::Model& GetImpl(Dali::Scene3D::Model& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<Dali::Scene3D::Internal::Model&>(handle);
}

inline const Dali::Scene3D::Internal::Model& GetImpl(const Dali::Scene3D::Model& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  const Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<const Dali::Scene3D::Internal::Model&>(handle);
}

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_INTERNAL_MODEL_H

#ifndef DALI_SCENE3D_INTERNAL_MODEL_VIEW_H
#define DALI_SCENE3D_INTERNAL_MODEL_VIEW_H

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
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/rendering/texture.h>
#include <dali/public-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/controls/model-view/model-view.h>
#include <dali-scene3d/public-api/controls/scene-view/scene-view.h>

namespace Dali
{
namespace Scene3D
{
class ModelView;

namespace Internal
{
/**
 * @brief Impl class for ModelView.
 */
class ModelView : public Dali::Toolkit::Internal::Control
{
public:
  using AnimationData = std::pair<std::string, Dali::Animation>;

  /**
   * @brief Creates a new ModelView.
   *
   * @return A public handle to the newly allocated ModelView.
   */
  static Dali::Scene3D::ModelView New(const std::string& modelPath, const std::string& resourcePath);

  /**
   * @copydoc ModelView::GetModelRoot()
   */
  const Actor GetModelRoot();

  /**
   * @copydoc ModelView::FitModel()
   */
  void FitSize(bool fit);

  /**
   * @copydoc ModelView::FitCenter()
   */
  void FitCenter(bool fit);

  /**
   * @copydoc ModelView::SetImageBasedLightSource()
   */
  void SetImageBasedLightSource(const std::string& diffuse, const std::string& specular, float scaleFactor);

  /**
   * @copydoc ModelView::SetImageBasedLightTexture()
   */
  void SetImageBasedLightTexture(Dali::Texture diffuse, Dali::Texture specular, float scaleFactor);

  /**
   * @copydoc ModelView::GetAnimationCount()
   */
  uint32_t GetAnimationCount();

  /**
   * @copydoc ModelView::GetAnimation()
   */
  Dali::Animation GetAnimation(uint32_t index);

  /**
   * @copydoc ModelView::GetAnimation()
   */
  Dali::Animation GetAnimation(const std::string& name);

protected:
  /**
   * @brief Constructs a new ModelView.
   */
  ModelView(const std::string& modelPath, const std::string& resourcePath);

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ModelView();

private:
  /**
   * @copydoc CustomActorImpl::OnSceneConnection()
   */
  void OnSceneConnection(int depth) override;

  /**
   * @copydoc CustomActorImpl::OnSceneDisconnection()
   */
  void OnSceneDisconnection() override;

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

  /**
   * @brief Loads a model from file
   */
  void LoadModel();

  /**
   * @brief Scales the model to fit the control or to return to original size.
   */
  void ScaleModel();

  /**
   * @brief Changes model anchor point to set the model at center or returns to the original model pivot.
   */
  void FitModelPosition();

  /**
   * @brief Changes IBL information of the input node.
   */
  void CollectRenderableActor(Actor actor);

  /**
   * @brief Changes IBL information of the input node.
   */
  void UpdateImageBasedLight();

private:
  std::string                    mModelPath;
  std::string                    mResourcePath;
  Dali::Actor                    mModelRoot;
  std::vector<AnimationData>     mAnimations;
  std::vector<WeakHandle<Actor>> mRenderableActors;
  WeakHandle<Scene3D::SceneView> mParentSceneView;

  Dali::Texture mSpecularTexture;
  Dali::Texture mDiffuseTexture;
  Vector3       mNaturalSize;
  Vector3       mModelPivot;
  float         mIblScaleFactor;
  bool          mFitSize;
  bool          mFitCenter;
  bool          mModelResourceReady;
  bool          mIBLResourceReady;
};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Dali::Scene3D::Internal::ModelView& GetImpl(Dali::Scene3D::ModelView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<Dali::Scene3D::Internal::ModelView&>(handle);
}

inline const Dali::Scene3D::Internal::ModelView& GetImpl(const Dali::Scene3D::ModelView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  const Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<const Dali::Scene3D::Internal::ModelView&>(handle);
}

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_INTERNAL_MODEL_VIEW_H

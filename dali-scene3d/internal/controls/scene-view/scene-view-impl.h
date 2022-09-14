#ifndef DALI_SCENE3D_INTERNAL_SCENE_VIEW_H
#define DALI_SCENE3D_INTERNAL_SCENE_VIEW_H

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
#include <dali-toolkit/internal/visuals/image/image-visual.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/rendering/frame-buffer.h>
#include <dali/public-api/rendering/texture.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/controls/model-view/model-view.h>
#include <dali-scene3d/public-api/controls/scene-view/scene-view.h>

namespace Dali
{
namespace Scene3D
{
class SceneView;

namespace Internal
{
/**
 * @brief Impl class for SceneView.
 */
class SceneView : public Dali::Toolkit::Internal::Control
{
public:
  /**
   * @brief Creates a new SceneView.
   *
   * @return A public handle to the newly allocated SceneView.
   */
  static Dali::Scene3D::SceneView New();

  /**
   * @copydoc SceneView::AddCamera()
   */
  void AddCamera(Dali::CameraActor camera);

  /**
   * @copydoc SceneView::RemoveCamera()
   */
  void RemoveCamera(CameraActor camera);

  /**
   * @copydoc SceneView::GetCameraCount()
   */
  uint32_t GetCameraCount();

  /**
   * @copydoc SceneView::GetSelectedCamera()
   */
  CameraActor GetSelectedCamera();

  /**
   * @copydoc SceneView::GetCamera()
   */
  CameraActor GetCamera(uint32_t index);

  /**
   * @copydoc SceneView::GetCamera()
   */
  CameraActor GetCamera(const std::string& name);

  /**
   * @copydoc SceneView::SelectCamera()
   */
  void SelectCamera(uint32_t index);

  /**
   * @copydoc SceneView::SelectCamera()
   */
  void SelectCamera(const std::string& name);

  /**
   * @brief Register a ModelView.
   * Some works like ibl setting should be applied on the only ModelView not the all child actors.
   * SceneView contains child ModelView list to apply the works effectively.
   *
   * @param[in] modelView ModelView to be registered.
   */
  void RegisterModelView(Scene3D::ModelView modelView);

  /**
   * @brief Unregister a ModelView
   *
   * @param[in] modelView ModelView to be unregistered.
   */
  void UnregisterModelView(Scene3D::ModelView modelView);

  /**
   * @copydoc SceneView::SetImageBasedLightSource()
   */
  void SetImageBasedLightSource(const std::string& diffuse, const std::string& specular, float scaleFactor);

  /**
   * @copydoc SceneView::UseFramebuffer()
   */
  void UseFramebuffer(bool useFramebuffer);

  /**
   * @copydoc SceneView::IsUsingFramebuffer()
   */
  bool IsUsingFramebuffer();

protected:
  /**
   * @brief Constructs a new SceneView.
   */
  SceneView();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~SceneView();

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
   * @copydoc Toolkit::Control::OnInitialize()
   */
  void OnInitialize() override;

  /**
   * @copydoc Toolkit::Control::OnChildAdd()
   */
  void OnChildAdd(Actor& child) override;

  /**
   * @copydoc Toolkit::Control::OnChildRemove()
   */
  void OnChildRemove(Actor& child) override;

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
   * @brief Changes main camera as a input camera
   *
   * @param camera CameraActor that will be a main camera of the SceneView
   */
  void UpdateCamera(CameraActor camera);

  /**
   * @brief Updates RenderTask to use selected camera and to make framebuffer
   */
  void UpdateRenderTask();

private:
  Toolkit::Visual::Base mVisual;

  /////////////////////////////////////////////////////////////
  // FrameBuffer and Rendertask to render child objects as a 3D Scene
  CameraActor                     mDefaultCamera;
  CameraActor                     mSelectedCamera;
  std::vector<CameraActor>        mCameras;
  std::vector<Scene3D::ModelView> mModels;
  Dali::FrameBuffer               mRenderTarget;
  Dali::Texture                   mTexture;
  Dali::RenderTask                mRenderTask;

  Layer mRootLayer;

  Dali::Texture mSpecularTexture;
  Dali::Texture mDiffuseTexture;
  float         mIblScaleFactor{1.0f};
  bool          mUseFrameBuffer{false};

  // TODO : Light Source
};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Dali::Scene3D::Internal::SceneView& GetImpl(Dali::Scene3D::SceneView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<Dali::Scene3D::Internal::SceneView&>(handle);
}

inline const Dali::Scene3D::Internal::SceneView& GetImpl(const Dali::Scene3D::SceneView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  const Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<const Dali::Scene3D::Internal::SceneView&>(handle);
}

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_INTERNAL_SCENE_VIEW_H

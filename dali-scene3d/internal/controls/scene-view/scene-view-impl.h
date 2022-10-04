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
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/rendering/frame-buffer.h>
#include <dali/public-api/rendering/texture.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/controls/model/model.h>
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
  uint32_t GetCameraCount() const;

  /**
   * @copydoc SceneView::GetSelectedCamera()
   */
  CameraActor GetSelectedCamera() const;

  /**
   * @copydoc SceneView::GetCamera()
   */
  CameraActor GetCamera(uint32_t index) const;

  /**
   * @copydoc SceneView::GetCamera()
   */
  CameraActor GetCamera(const std::string& name) const;

  /**
   * @copydoc SceneView::SelectCamera()
   */
  void SelectCamera(uint32_t index);

  /**
   * @copydoc SceneView::SelectCamera()
   */
  void SelectCamera(const std::string& name);

  /**
   * @brief Register a Model.
   * Some works like ibl setting should be applied on the only Model not the all child actors.
   * SceneView contains child Model list to apply the works effectively.
   *
   * @param[in] model Model to be registered.
   */
  void RegisterModel(Scene3D::Model model);

  /**
   * @brief Unregister a Model
   *
   * @param[in] model Model to be unregistered.
   */
  void UnregisterModel(Scene3D::Model model);

  /**
   * @copydoc SceneView::SetImageBasedLightSource()
   */
  void SetImageBasedLightSource(const std::string& diffuseUrl, const std::string& specularUrl, float scaleFactor);

  /**
   * @copydoc SceneView::SetImageBasedLightScaleFactor()
   */
  void SetImageBasedLightScaleFactor(float scaleFactor);

  /**
   * @copydoc SceneView::GetImageBasedLightScaleFactor()
   */
  float GetImageBasedLightScaleFactor() const;

  /**
   * @copydoc SceneView::UseFramebuffer()
   */
  void UseFramebuffer(bool useFramebuffer);

  /**
   * @copydoc SceneView::IsUsingFramebuffer()
   */
  bool IsUsingFramebuffer() const;

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
   * @copydoc Toolkit::Control::IsResourceReady()
   */
  bool IsResourceReady() const override;

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

  /**
   * @brief Loads image based light from file.
   */
  void LoadImageBasedLight();

  /**
   * @brief Asynchronously loading finished.
   */
  void OnLoadComplete();

  /*
   * @brief Callback that will be called when window is resized.
   */
  void OnWindowResized(Window window, Window::WindowSize size);

  /**
   * @brief Update camera's projection orientation according to the screen orientation.
   */
  void RotateCamera();

private:
  Toolkit::Visual::Base mVisual;

  /////////////////////////////////////////////////////////////
  // FrameBuffer and Rendertask to render child objects as a 3D Scene
  CameraActor                 mDefaultCamera;
  CameraActor                 mSelectedCamera;
  std::vector<CameraActor>    mCameras;
  std::vector<Scene3D::Model> mModels;
  Dali::FrameBuffer           mRenderTarget;
  Dali::Texture               mTexture;
  Dali::RenderTask            mRenderTask;

  CallbackBase* mIblLoadedCallback;
  std::string   mDiffuseIblUrl;
  std::string   mSpecularIblUrl;
  int32_t       mScreenOrientation;

  Layer         mRootLayer;
  Dali::Texture mSpecularTexture;
  Dali::Texture mDiffuseTexture;
  float         mIblScaleFactor{1.0f};
  bool          mUseFrameBuffer{false};
  bool          mIBLResourceReady{true};

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

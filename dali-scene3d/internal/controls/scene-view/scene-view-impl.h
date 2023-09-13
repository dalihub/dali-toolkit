#ifndef DALI_SCENE3D_INTERNAL_SCENE_VIEW_H
#define DALI_SCENE3D_INTERNAL_SCENE_VIEW_H

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
#include <dali-toolkit/internal/visuals/image/image-visual.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/rendering/frame-buffer.h>
#include <dali/public-api/rendering/texture.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/common/environment-map-load-task.h>
#include <dali-scene3d/internal/common/light-observer.h>
#include <dali-scene3d/public-api/controls/scene-view/scene-view.h>
#include <dali-scene3d/public-api/loader/shader-manager.h>

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
   * @brief Register an item.
   *
   * Some works(e.g, lighting) of SceneView should be propagated to the child 3D items.
   * SceneView can avoid unnecessary tree traversal to find 3D items by storing only 3D items as a list.
   *
   * @param[in] item scene observer to be registered.
   */
  void RegisterSceneItem(Scene3D::Internal::LightObserver* item);

  /**
   * @brief Unregister an item
   *
   * @param[in] item scene observer to be unregistered.
   */
  void UnregisterSceneItem(Scene3D::Internal::LightObserver* item);

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
   * @brief Adds a Light object to this SceneView.
   * Multiple light object can be added on this SceneView
   * But the number of lights those actually turned on has limitation.
   * Scene3D::Light::GetMaximumEnabledLightCount() method can be used to know the maximum namber.
   *
   * @param[in] light Light object to be added.
   */
  void AddLight(Scene3D::Light light);

  /**
   * @brief Removes a Light object to this SceneView.
   * If the light is currently turned on, and SceneView has lights more than maximum number of enabled light.
   * One of light in queue is turned on after this light is removed.
   *
   * @param[in] light Light object to be removed.
   */
  void RemoveLight(Scene3D::Light light);

  /**
   * @brief Set a shadow to this scene by input light.
   * Currently, SceneView supports only one shadow.
   *
   * @param[in] light Light object to make shadow.
   * @note The shadow will be drawn if the input light is turn on in current scene.
   */
  void SetShadow(Scene3D::Light light);

  /**
   * @brief Removes Shadow from this SceneView.
   *
   * @param[in] light Light object to be removed.
   */
  void RemoveShadow(Scene3D::Light light);

  /**
   * @copydoc SceneView::GetActivatedLightCount()
   */
  uint32_t GetActivatedLightCount() const;

  /**
   * @copydoc SceneView::UseFramebuffer()
   */
  void UseFramebuffer(bool useFramebuffer);

  /**
   * @copydoc SceneView::IsUsingFramebuffer()
   */
  bool IsUsingFramebuffer() const;

  /**
   * @copydoc SceneView::SetFramebufferMultiSamplingLevel()
   */
  void SetFramebufferMultiSamplingLevel(uint8_t multiSamplingLevel);

  /**
   * @copydoc SceneView::GetFramebufferMultiSamplingLevel()
   */
  uint8_t GetFramebufferMultiSamplingLevel() const;

  /**
   * @copydoc SceneView::SetSkybox()
   */
  void SetSkybox(const std::string& skyboxUrl);

  /**
   * @copydoc SceneView::SetSkyboxEnvironmentMapType()
   */
  void SetSkyboxEnvironmentMapType(Scene3D::EnvironmentMapType skyboxEnvironmentMapType);

  /**
   * @copydoc SceneView::SetSkyboxIntensity()
   */
  void SetSkyboxIntensity(float intensity);

  /**
   * @copydoc SceneView::GetSkyboxIntensity()
   */
  float GetSkyboxIntensity() const;

  /**
   * @copydoc SceneView::SetSkyboxOrientation()
   */
  void SetSkyboxOrientation(const Quaternion& orientation);

  /**
   * @copydoc SceneView::GetSkyboxOrientation()
   */
  Quaternion GetSkyboxOrientation() const;

  /**
   * @brief Retrieves ShaderManager of this SceneView.
   * @return ShaderManager of this SceneView.
   */
  Dali::Scene3D::Loader::ShaderManagerPtr GetShaderManager() const;

  /**
   * @brief Update shader uniforms about shadow.
   * @param[in] light Light that makes shadow.
   */
  void UpdateShadowUniform(Scene3D::Light light);

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
   * @brief Callback that will be called when window is resized.
   */
  void OnWindowResized(Window window, Window::WindowSize size);

  /**
   * @brief Updates camera's projection orientation according to the screen orientation.
   */
  void RotateCamera();

  /**
   * @brief UpdateSkybox with skybox url and skybox environment map type.
   *
   * @param[in] skyboxUrl image url for skybox.
   * @param[in] skyboxEnvironmentMapType The environment type of skybox.
   */
  void UpdateSkybox(const std::string& skyboxUrl, Scene3D::EnvironmentMapType skyboxEnvironmentMapType);

  /**
   * @brief Asynchronously skybox loading finished.
   */
  void OnSkyboxLoadComplete();

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
   * @brief Notify the changes of Ibl textures to the child items.
   */
  void NotifyImageBasedLightTextureChange();

  /**
   * @brief Update shadowMap framebuffer when the size should be changed.
   * @param[in] shadowMapSize The size of shadowMap texture. The texture's width and hight is equal.
   */
  void UpdateShadowMapBuffer(uint32_t shadowMapSize);

private:
  Toolkit::Visual::Base mVisual;

  /////////////////////////////////////////////////////////////
  // FrameBuffer and Rendertask to render child objects as a 3D Scene
  Dali::WeakHandle<Dali::Window>                 mWindow;
  Integration::SceneHolder                       mSceneHolder;
  CameraActor                                    mDefaultCamera;
  CameraActor                                    mSelectedCamera;
  std::vector<CameraActor>                       mCameras;
  std::vector<Scene3D::Internal::LightObserver*> mItems;
  Dali::FrameBuffer                              mFrameBuffer;
  Dali::Texture                                  mTexture;
  Dali::RenderTask                               mRenderTask;
  Layer                                          mRootLayer;
  int32_t                                        mWindowOrientation;
  Dali::Actor                                    mSkybox;
  Quaternion                                     mSkyboxOrientation;
  float                                          mSkyboxIntensity{1.0f};
  uint8_t                                        mFrameBufferMultiSamplingLevel{0u};

  // Shader Factory
  Dali::Scene3D::Loader::ShaderManagerPtr mShaderManager;

  // Light
  std::vector<std::pair<Scene3D::Light, bool>> mLights; // Pair of Light object and flag that denotes the light is currently activated or not.
  Dali::FrameBuffer                            mShadowFrameBuffer;
  Dali::RenderTask                             mShadowMapRenderTask;
  Scene3D::Light                               mShadowLight;
  Dali::Texture                                mShadowTexture;

  // Asynchronous Loading.
  EnvironmentMapLoadTaskPtr mSkyboxLoadTask;
  EnvironmentMapLoadTaskPtr mIblDiffuseLoadTask;
  EnvironmentMapLoadTaskPtr mIblSpecularLoadTask;
  std::string               mSkyboxUrl;
  std::string               mDiffuseIblUrl;
  std::string               mSpecularIblUrl;

  Scene3D::EnvironmentMapType mSkyboxEnvironmentMapType{Scene3D::EnvironmentMapType::AUTO};
  Dali::Texture               mSkyboxTexture;
  Dali::Texture               mDiffuseTexture;
  Dali::Texture               mSpecularTexture;
  float                       mIblScaleFactor{1.0f};
  uint32_t                    mSpecularMipmapLevels{1u};
  bool                        mUseFrameBuffer{false};
  bool                        mSkyboxResourceReady{true};
  bool                        mIblDiffuseResourceReady{true};
  bool                        mIblSpecularResourceReady{true};
  bool                        mSkyboxDirty{false};
  bool                        mIblDiffuseDirty{false};
  bool                        mIblSpecularDirty{false};
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

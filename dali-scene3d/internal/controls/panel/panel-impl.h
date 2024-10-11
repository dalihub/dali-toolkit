#ifndef DALI_SCENE3D_INTERNAL_PANEL_H
#define DALI_SCENE3D_INTERNAL_PANEL_H

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
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/rendering/frame-buffer.h>
#include <dali/public-api/rendering/texture.h>
#include <unordered_map>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/common/environment-map-load-task.h>
#include <dali-scene3d/internal/common/light-observer.h>
#include <dali-scene3d/internal/event/collider-mesh-processor.h>
#include <dali-scene3d/internal/model-components/model-node-impl.h>
#include <dali-scene3d/public-api/controls/panel/panel.h>
#include <dali-scene3d/public-api/controls/scene-view/scene-view.h>
#include <dali-scene3d/public-api/light/light.h>
#include <dali-scene3d/public-api/model-components/model-node.h>

namespace Dali
{
namespace Scene3D
{
class Panel;

namespace Internal
{
/**
 * @brief Impl class for Panel.
 */
class Panel : public Dali::Toolkit::Internal::Control, public LightObserver
{
public:

  /**
   * @copydoc Panel::New()
   */
  static Dali::Scene3D::Panel New();

  /**
   * @copydoc Panel::SetPanelResolution()
   */
  void SetPanelResolution(const Vector2& resolution);

  /**
   * @copydoc Panel::GetPanelResolution()
   */
  Vector2 GetPanelResolution() const;

  /**
   * @copydoc Panel::SetContent()
   */
  void SetContent(Dali::Actor rootActor);

  /**
   * @copydoc Panel::GetContent()
   */
  Dali::Actor GetContent() const;

  /**
   * @copydoc Panel::ClearPanel()
   */
  void ClearPanel();

  /**
   * @copydoc Panel::CastShadow()
   */
  void CastShadow(bool castShadow);

  /**
   * @copydoc Panel::IsShadowCasting()
   */
  bool IsShadowCasting() const;

  /**
   * @copydoc Panel::ReceiveShadow()
   */
  void ReceiveShadow(bool receiveShadow);

  /**
   * @copydoc Panel::IsShadowReceiving()
   */
  bool IsShadowReceiving() const;

  // Properties

  /**
   * Called when a property of an object of this type is set.
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty(BaseObject* object, Property::Index index, const Property::Value& value);

  /**
   * Called to retrieve a property of an object of this type.
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty(BaseObject* object, Property::Index index);

protected:
  /**
   * @brief Constructs a new Panel.
   */
  Panel();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Panel();

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

private:
  /**
   * @brief Sets whether the plane is transparent or not.
   * @param[in] transparent True to make plane transparent.
   */
  void SetTransparent(bool transparent);

  /**
   * @brief Retrieves whether the plane is transparent or not.
   * @return Ture if the plane is transparent.
   */
  bool IsTransparent() const;

  /**
   * @brief Sets whether the plane is double sided or not.
   * @param[in] doubleSided True to make plane be rendered as double side.
   */
  void SetDoubleSided(bool doubleSided);

  /**
   * @brief Retrieves whether the plane is double sided or not.
   * @return Ture if the plane is rendered as double side.
   */
  bool IsDoubleSided() const;

  /**
   * @brief Sets whether the Panel has back face plane or not.
   * @param[in] useBackFacePlane True to use back face plane.
   */
  void SetUseBackFacePlane(bool useBackFacePlane);

  /**
   * @brief Retrieves whether the Panel has back face plane or not.
   * @return Ture if the Panel has back face plane.
   */
  bool IsUsingBackFacePlane() const;

  /**
   * @brief Sets color of back face plane.
   * @param[in] backFacePlaneColor Vector3 color of the back face plane.
   */
  void SetBackFacePlaneColor(Vector3 backFacePlaneColor);

  /**
   * @brief Retrieves color of back face plane.
   * @return back face plane color.
   */
  Vector3 GetBackFacePlaneColor() const;

  /**
   * @brief Update plane's properties.
   */
  void UpdateProperties();

  /**
   * @brief Changes IBL textures of the input node.
   */
  void UpdateImageBasedLightTexture();

  /**
   * @brief Changes IBL scale factor of the input node.
   */
  void UpdateImageBasedLightScaleFactor();

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
   * @brief Update model root scale when Panel size property is updated.
   */
  void OnSizeNotification(Dali::PropertyNotification& source);

  /**
   * @brief Updates RenderTask apply new panel size
   */
  void UpdateRenderTask();

private:
  Vector2                        mPanelResolution;
  Scene3D::ModelNode             mPanelNode;
  Scene3D::ModelNode             mContentPlaneNode;
  Scene3D::ModelNode             mBackPlaneNode;
  Scene3D::ModelNode             mDoubleSidedPlaneNode;
  Dali::Layer                    mRootLayer;
  Integration::SceneHolder       mSceneHolder;
  CameraActor                    mCamera;
  Dali::FrameBuffer              mFrameBuffer;
  Dali::Texture                  mTexture;
  Scene3D::Material              mContentPlaneMaterial;
  Scene3D::Material              mBackPlaneMaterial;
  Scene3D::Material              mDoubleSidedPlaneMaterial;
  Dali::Geometry                 mGeometry;
  Dali::Geometry                 mDoubleSidedGeometry;
  Dali::RenderTask               mRenderTask;
  WeakHandle<Scene3D::SceneView> mParentSceneView;

  Property::Index mResolutionPropertyIndex;

  bool    mIsTransparent : 1;        /// Checks whether the panel background is transparent or not. defalut is {false}
  bool    mIsDoubleSided : 1;        /// Checkes whether the panel rendered on double sided or not. default is {false}.
  bool    mIsUsingBackFacePlane : 1; /// Checked whether the panel uses opaque back face plane or not. default is {true}.
  Vector3 mBackFacePlaneColor;

  Dali::Scene3D::Loader::ShaderManagerPtr mShaderManager;

  // IBL
  Dali::Texture mDefaultSpecularTexture;
  Dali::Texture mDefaultDiffuseTexture;
  Dali::Texture mSceneSpecularTexture;
  Dali::Texture mSceneDiffuseTexture;
  float         mSceneIblScaleFactor;
  uint32_t      mSceneSpecularMipmapLevels;

  // Shadow
  Dali::Texture mShadowMapTexture;
  bool          mIsShadowCasting;
  bool          mIsShadowReceiving;
};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Dali::Scene3D::Internal::Panel& GetImpl(Dali::Scene3D::Panel& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<Dali::Scene3D::Internal::Panel&>(handle);
}

inline const Dali::Scene3D::Internal::Panel& GetImpl(const Dali::Scene3D::Panel& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  const Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<const Dali::Scene3D::Internal::Panel&>(handle);
}

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_INTERNAL_PANEL_H

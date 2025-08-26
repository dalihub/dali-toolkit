#ifndef DALI_SCENE3D_LIGHT_IMPL_H
#define DALI_SCENE3D_LIGHT_IMPL_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/weak-handle.h>
#include <string_view>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/controls/scene-view/scene-view.h>
#include <dali-scene3d/public-api/light/light.h>

namespace Dali
{
namespace Scene3D
{
/**
 * @addtogroup dali_scene3d_light
 * @{
 */

namespace Internal
{
/**
 * @brief This is the internal base class for custom node of Light.
 *
 * @SINCE_2_2.32
 */
class DALI_SCENE3D_API Light : public Toolkit::Internal::Control
{
public:
  // Creation & Destruction
  /**
   * @brief Creates a new LightImpl instance that does not require touch by default.
   *
   * If touch is required, then the user can connect to this class' touch signal.
   * @SINCE_2_2.32
   * @return A handle to the Light instance
   */
  static Scene3D::Light New();

protected:
  /**
   * @brief Virtual destructor.
   * @SINCE_2_2.32
   */
  virtual ~Light();

public: // From Internal::Control.
  /**
   * @copydoc Dali::Toolkit::Internal::Control::OnInitialize()
   */
  void OnInitialize() override;

protected: // From CustomActorImpl. Note that it will make we can ignore many codes what Internal::Control using now.
  /**
   * @copydoc CustomActorImpl::OnSceneConnection()
   * @note If overridden, then an up-call to Light::OnSceneConnection MUST be made at the end.
   */
  void OnSceneConnection(int depth) override;

  /**
   * @copydoc CustomActorImpl::OnSceneDisconnection()
   * @note If overridden, then an up-call to Light::OnSceneDisconnection MUST be made at the end.
   */
  void OnSceneDisconnection() override;

  /**
   * @copydoc CustomActorImpl::OnChildAdd()
   * @note If overridden, then an up-call to Light::OnChildAdd MUST be made at the end.
   */
  void OnChildAdd(Actor& child) override;

  /**
   * @copydoc CustomActorImpl::OnChildRemove()
   * @note If overridden, then an up-call to Light::OnChildRemove MUST be made at the end.
   */
  void OnChildRemove(Actor& child) override;

  /**
   * @copydoc CustomActorImpl::OnSizeSet()
   * @note If overridden, then an up-call to Light::OnSizeSet MUST be made at the end.
   */
  void OnSizeSet(const Vector3& targetSize) override;

  /**
   * @copydoc CustomActorImpl::OnSizeAnimation()
   * @note If overridden, then an up-call to Light::OnSizeAnimation MUST be made at the end.
   */
  void OnSizeAnimation(Animation& animation, const Vector3& targetSize) override;

  /**
   * @copydoc CustomActorImpl::OnRelayout()
   */
  void OnRelayout(const Vector2& size, RelayoutContainer& container) override;

  /**
   * @copydoc CustomActorImpl::OnSetResizePolicy()
   */
  void OnSetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension) override;

  /**
   * @copydoc CustomActorImpl::GetNaturalSize()
   */
  Vector3 GetNaturalSize() override;

  /**
   * @copydoc CustomActorImpl::CalculateChildSize()
   */
  float CalculateChildSize(const Dali::Actor& child, Dimension::Type dimension) override;

  /**
   * @copydoc CustomActorImpl::GetHeightForWidth()
   */
  float GetHeightForWidth(float width) override;

  /**
   * @copydoc CustomActorImpl::GetWidthForHeight()
   */
  float GetWidthForHeight(float height) override;

  /**
   * @copydoc CustomActorImpl::RelayoutDependentOnChildren()
   */
  bool RelayoutDependentOnChildren(Dimension::Type dimension = Dimension::ALL_DIMENSIONS) override;

  /**
   * @copydoc CustomActorImpl::OnCalculateRelayoutSize()
   */
  void OnCalculateRelayoutSize(Dimension::Type dimension) override;

  /**
   * @copydoc CustomActorImpl::OnLayoutNegotiated()
   */
  void OnLayoutNegotiated(float size, Dimension::Type dimension) override;

protected:
  // Construction

  /**
   * @brief Light constructor.
   */
  Light();

public: // Public Method
  /**
   * @copydoc Scene3D::Light::Enable()
   */
  void Enable(bool enable);

  /**
   * @copydoc Scene3D::Light::IsEnabled()
   */
  bool IsEnabled() const;

  /**
   * @copydoc Scene3D::Light::EnableShadow()
   */
  void EnableShadow(bool enable);

  /**
   * @copydoc Scene3D::Light::IsShadowEnabled()
   */
  bool IsShadowEnabled() const;

  /**
   * @copydoc Scene3D::Light::GetCamera()
   */
  CameraActor GetCamera() const;

  /**
   * @copydoc Scene3D::Light::EnableShadowSoftFiltering()
   */
  void EnableShadowSoftFiltering(bool useSoftFiltering);

  /**
   * @copydoc Scene3D::Light::IsShadowSoftFilteringEnabled()
   */
  bool IsShadowSoftFilteringEnabled() const;

  /**
   * @copydoc Scene3D::Light::SetShadowIntensity()
   */
  void SetShadowIntensity(float shadowIntensity);

  /**
   * @copydoc Scene3D::Light::GetShadowIntensity()
   */
  float GetShadowIntensity() const;

  /**
   * @copydoc Scene3D::Light::SetShadowBias()
   */
  void SetShadowBias(float shadowBias);

  /**
   * @copydoc Scene3D::Light::GetShadowBias()
   */
  float GetShadowBias() const;

public: // Public Static Method
  /**
   * @copydoc Scene3D::Light::GetMaximumEnabledLightCount()
   */
  static uint32_t GetMaximumEnabledLightCount();

  // Shadow uniforms for vertex shader
  /**
   * @brief Retrieves Shadow Uniform Block Name for vertex shader
   * @return string_view for ShadowVertexUniformBlockName
   */
  static std::string_view GetShadowVertexUniformBlockName();

  /**
   * @brief Retrieves Uniform Name to define shadow is enabled or not.
   * @return string_view for ShadowEnabledUniformName
   */
  static std::string_view GetShadowEnabledUniformName();

  /**
   * @brief Retrieves Uniform Name for View/Projection matrix of the Shadow.
   * @return string_view for ShadowViewProjectionMatrixUniformName
   */
  static std::string_view GetShadowViewProjectionMatrixUniformName();

  // Light uniforms for fragment shader
  /**
   * @brief Retrieves Light Uniform Block Name for fragment shader
   * @return string_view for LightUniformBlockName
   */
  static std::string_view GetLightUniformBlockName();

  /**
   * @brief Retrieves Light Enabled Uniform Name for LightUniformBlock()
   * @return string_view for LightEnabledUniformName
   */
  static std::string_view GetLightCountUniformName();

  /**
   * @brief Retrieves Light Direction Uniform Name for LightUniformBlock()
   * @return string_view for LightDirectionUniformName
   */
  static std::string_view GetLightDirectionUniformName();

  /**
   * @brief Retrieves Light Color Uniform Name for LightUniformBlock()
   * @return string_view for LightColorUniformName
   */
  static std::string_view GetLightColorUniformName();

  // Shadow uniforms for fragment shader
  /**
   * @brief Retrieves Shadow Light Index Uniform Name for ShadowFragmentUniformBlock()
   * @return string_view for ShadowLightIndexUniformName
   */
  static std::string_view GetShadowLightIndexUniformName();

  /**
   * @brief Retrieves Shadow Soft Filtering Enable Uniform Name for ShadowFragmentUniformBlock()
   * @return string_view for ShadowSoftFilteringEnableUniformName
   */
  static std::string_view GetShadowSoftFilteringEnableUniformName();

  /**
   * @brief Retrieves Shadow Intensity Uniform Name for ShadowFragmentUniformBlock()
   * @return string_view for ShadowIntensityUniformName
   */
  static std::string_view GetShadowIntensityUniformName();

  /**
   * @brief Retrieves Shadow Bias Uniform Name for ShadowFragmentUniformBlock()
   * @return string_view for ShadowBiasUniformName
   */
  static std::string_view GetShadowBiasUniformName();

private:
  void UpdateShadowUniforms();

private:
  /// @cond internal

  // Not copyable or movable
  DALI_INTERNAL        Light(const Light&)     = delete; ///< Deleted copy constructor.
  DALI_INTERNAL        Light(Light&&)          = delete; ///< Deleted move constructor.
  DALI_INTERNAL Light& operator=(const Light&) = delete; ///< Deleted copy assignment operator.
  DALI_INTERNAL Light& operator=(Light&&)      = delete; ///< Deleted move assignment operator.

private:
  Dali::CameraActor              mLightSourceActor;
  WeakHandle<Scene3D::SceneView> mParentSceneView;
  bool                           mIsEnabled{true};
  bool                           mIsShadowEnabled{false};
  bool                           mUseSoftFiltering{false};
  float                          mShadowIntensity{0.5f};
  float                          mShadowBias{0.001f};
  /// @endcond
};

/**
 * @brief Gets implementation from the handle.
 *
 * @param handle
 * @return Implementation
 * @pre handle is initialized and points to a node
 */
DALI_SCENE3D_API Internal::Light& GetImplementation(Dali::Scene3D::Light& handle);

/**
 * @brief Gets implementation from the handle.
 *
 * @param handle
 * @return Implementation
 * @pre Handle is initialized and points to a node.
 */
DALI_SCENE3D_API const Internal::Light& GetImplementation(const Dali::Scene3D::Light& handle);

} // namespace Internal

/**
 * @}
 */
} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_LIGHT_IMPL_H

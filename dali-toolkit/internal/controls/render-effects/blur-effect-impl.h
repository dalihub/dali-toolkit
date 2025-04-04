#ifndef DALI_TOOLKIT_INTERNAL_BLUR_EFFECT_H
#define DALI_TOOLKIT_INTERNAL_BLUR_EFFECT_H

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
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/rendering/frame-buffer.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/render-effects/render-effect-impl.h>
#include <dali-toolkit/public-api/controls/render-effects/background-blur-effect.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class BlurEffectImpl;
using BlurEffectImplPtr = IntrusivePtr<BlurEffectImpl>;

class BlurEffectImpl : public RenderEffectImpl
{
public:
  /**
   * @brief Creates an initialized BlurEffect implementation, using default settings. The default settings are:
   *
   * downscaleFactor = 0.4f
   * pixelRadius = 5u
   *
   * This blur algorithm is used for both foreground and background blurs.
   *
   * @param[in] isBackground True when blurring background, False otherwise
   * @return A handle to a newly allocated Dali resource
   */

  static BlurEffectImplPtr New(bool isBackground);

  /**
   * @brief Creates an initialized BlurEffect implementation.
   * This blur algorithm is used for both foreground and background blurs.
   *
   * @param[in] downscaleFactor This value should reside in the range [0.0, 1.0].
   * @param[in] blurRadius The radius of Gaussian kernel.
   * @param[in] blurOnce Whether to blur once or always. Default is false(always).
   * @param[in] isBackground True when blurring background, False otherwise
   * @return A handle to a newly allocated Dali resource
   */
  static BlurEffectImplPtr New(float downscaleFactor, uint32_t blurRadius, bool blurOnce, bool isBackground);

  /**
   * @copydoc Toolkit::Internal::RenderEffectImpl::GetOffScreenRenderableType
   */
  OffScreenRenderable::Type GetOffScreenRenderableType() override;

  /**
   * @copydoc Toolkit::Internal::RenderEffectImpl::GetOffScreenRenderTasks
   */
  void GetOffScreenRenderTasks(std::vector<Dali::RenderTask>& tasks, bool isForward) override;

protected:
  /**
   * @brief Creates an uninitialized blur effect implementation
   * @param[in] isBackground True when blurring background, False otherwise
   */
  BlurEffectImpl(bool isBackground);

  /**
   * @brief Creates an uninitialized blur effect implementation
   * @param[in] downscaleFactor This value should reside in the range [0.0, 1.0].
   * @param[in] blurRadius The radius of Gaussian kernel.
   * @param[in] blurOnce Whether to blur once or always. Default is false(always).
   * @param[in] isBackground True when blurring background, False otherwise
   */
  BlurEffectImpl(float downscaleFactor, uint32_t blurRadius, bool blurOnce, bool isBackground);

  /**
   * @brief Destructor
   */
  virtual ~BlurEffectImpl();

  /**
   * @brief Initializes blur effect
   */
  void OnInitialize() override;

  /**
   * @brief Activates blur effect
   */
  void OnActivate() override;

  /**
   * @brief Dectivates blur effect
   */
  void OnDeactivate() override;

  /**
   * @brief Redraw effect without deactivation
   */
  void OnRefresh() override;

private:
  // Inner functions
  /**
   * @brief Sets frame buffers to draw blurred output.
   * @param[in] downsampledSize Downsampled size for performance.
   */
  void CreateFrameBuffers(const ImageDimensions downsampledSize);

  /**
   * @brief Removes and destroys local frame buffers.
   */
  void DestroyFrameBuffers();

  /**
   * @brief Sets blur render tasks.
   * Requires initialized buffers, source actors, and source cameras.
   * @param[in] sceneHolder SceneHolder of source control
   * @param[in] sourceControl Input source control
   */
  void CreateRenderTasks(Integration::SceneHolder sceneHolder, const Toolkit::Control sourceControl);

  /**
   * @brief Removes and destroys local render tasks.
   */
  void DestroyRenderTasks();

  /**
   * @brief Apply render tasks source actor, and some other options.
   * @param[in] renderTask Target render task to change source actor and exclusiveness
   * @param[in] sourceControl Input source control
   */
  void ApplyRenderTaskSourceActor(RenderTask sourceRenderTask, const Toolkit::Control sourceControl);

  /**
   * @brief Sets shader constants, gaussian kernel weights and pixel offsets.
   * @param[in] downsampledWidth Downsized width of input texture.
   * @param[in] downsampledHeight Downsized height of input texture.
   */
  void SetShaderConstants(uint32_t downsampledWidth, uint32_t downsampledHeight);

  /**
   * @brief Get an offset property in std::string format
   * @param[in] index Property's index
   * @return A string for shader
   */
  std::string GetSampleOffsetsPropertyName(unsigned int index) const;

  /**
   * @brief Get a weight property in std::string format
   * @param[in] index Property's index
   * @return A string for shader
   */
  std::string GetSampleWeightsPropertyName(unsigned int index) const;

  BlurEffectImpl(const BlurEffectImpl&) = delete;
  BlurEffectImpl(BlurEffectImpl&&)      = delete;
  BlurEffectImpl& operator=(BlurEffectImpl&&) = delete;      // no move()
  BlurEffectImpl& operator=(const BlurEffectImpl&) = delete; // no copy()

private:
  // Camera actors
  CameraActor mRenderDownsampledCamera;

  // Resource
  FrameBuffer mInputBackgroundFrameBuffer; // Input. Background. What to blur.

  Actor       mInternalRoot;
  Actor       mHorizontalBlurActor;
  RenderTask  mHorizontalBlurTask;
  FrameBuffer mTemporaryFrameBuffer;
  Actor       mVerticalBlurActor;
  RenderTask  mVerticalBlurTask;

  FrameBuffer mBlurredOutputFrameBuffer;
  RenderTask  mSourceRenderTask;

  // Variables
  float    mDownscaleFactor;
  uint32_t mPixelRadius;
  uint32_t mDownscaledPixelRadius;
  float    mBellCurveWidth;

  bool mSkipBlur : 1;
  bool mBlurOnce : 1;
  bool mIsBackground : 1;
};
} // namespace Internal

inline Toolkit::Internal::BlurEffectImpl& GetImplementation(Toolkit::BackgroundBlurEffect& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Toolkit::Internal::BlurEffectImpl&>(handle);
}

inline const Toolkit::Internal::BlurEffectImpl& GetImplementation(const Toolkit::BackgroundBlurEffect& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Toolkit::Internal::BlurEffectImpl&>(handle);
}

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_BACKGROUND_BLUR_EFFECT_H

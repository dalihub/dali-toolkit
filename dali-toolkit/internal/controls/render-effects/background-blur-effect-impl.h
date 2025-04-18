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
class BackgroundBlurEffectImpl;
using BackgroundBlurEffectImplPtr = IntrusivePtr<BackgroundBlurEffectImpl>;

class BackgroundBlurEffectImpl : public RenderEffectImpl
{
public:
  /**
   * @brief Creates an initialized BlurEffect implementation, using default settings. As default, blur radius is set to 10u.
   * @return A handle to a newly allocated Dali resource
   */

  static BackgroundBlurEffectImplPtr New();

  /**
   * @brief Creates an initialized BlurEffect implementation.
   *
   * @param[in] blurRadius The radius of Gaussian kernel.
   * @return A handle to a newly allocated Dali resource
   */
  static BackgroundBlurEffectImplPtr New(uint32_t blurRadius);

  /**
   * @copydoc Toolkit::Internal::RenderEffectImpl::GetOffScreenRenderableType
   */
  OffScreenRenderable::Type GetOffScreenRenderableType() override;

  /**
   * @copydoc Toolkit::Internal::RenderEffectImpl::GetOffScreenRenderTasks
   */
  void GetOffScreenRenderTasks(std::vector<Dali::RenderTask>& tasks, bool isForward) override;

  /**
   * @copydoc Toolkit::BackgroundBlurEffect::SetBlurOnce
   */
  void SetBlurOnce(bool blurOnce);

  /**
   * @copydoc Toolkit::BackgroundBlurEffect::GetBlurOnce
   */
  bool GetBlurOnce() const;

  /**
   * @copydoc Toolkit::BackgroundBlurEffect::SetBlurRadius
   */
  void SetBlurRadius(uint32_t blurRadius);

  /**
   * @copydoc Toolkit::BackgroundBlurEffect::GetBlurRadius
   */
  uint32_t GetBlurRadius() const;

  /**
   * @copydoc Toolkit::BackgroundBlurEffect::AddBlurStrengthAnimation
   */
  void AddBlurStrengthAnimation(Animation& animation, AlphaFunction alphaFunction, TimePeriod timePeriod, float fromValue, float toValue);

  /**
   * @copydoc Toolkit::BackgroundBlurEffect::AddBlurOpacityAnimation
   */
  void AddBlurOpacityAnimation(Animation& animation, AlphaFunction alphaFunction, TimePeriod timePeriod, float fromValue, float toValue);

  /**
   * @copydoc Toolkit::BackgroundBlurEffect::FinishedSignal
   */
  Dali::Toolkit::BackgroundBlurEffect::FinishedSignalType& FinishedSignal();

protected:
  /**
   * @brief Creates an uninitialized blur effect implementation
   */
  BackgroundBlurEffectImpl();

  /**
   * @brief Creates an uninitialized blur effect implementation
   * @param[in] blurRadius The radius of Gaussian kernel.
   */
  BackgroundBlurEffectImpl(uint32_t blurRadius);

  /**
   * @brief Destructor
   */
  virtual ~BackgroundBlurEffectImpl();

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

  /**
   * @brief Emits render finished signal of the effect,
   * when mBlurOnce is true and finished signal of the last render task(mVerticalBlurTask) is emitted.
   * @param[in] renderTask that emits source signal.
   */
  void OnRenderFinished(Dali::RenderTask& renderTask);

  BackgroundBlurEffectImpl(const BackgroundBlurEffectImpl&) = delete;
  BackgroundBlurEffectImpl(BackgroundBlurEffectImpl&&)      = delete;
  BackgroundBlurEffectImpl& operator=(BackgroundBlurEffectImpl&&) = delete;      // no move()
  BackgroundBlurEffectImpl& operator=(const BackgroundBlurEffectImpl&) = delete; // no copy()

public:
  Dali::Toolkit::BackgroundBlurEffect::FinishedSignalType mFinishedSignal; // Emits when blur once is enabled

private:
  // Camera actors
  CameraActor mCamera;
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
  uint32_t mBlurRadius;
  uint32_t mDownscaledBlurRadius;
  float    mBellCurveWidth;

  bool mSkipBlur : 1;
  bool mBlurOnce : 1;
};
} // namespace Internal

inline Toolkit::Internal::BackgroundBlurEffectImpl& GetImplementation(Toolkit::BackgroundBlurEffect& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Toolkit::Internal::BackgroundBlurEffectImpl&>(handle);
}

inline const Toolkit::Internal::BackgroundBlurEffectImpl& GetImplementation(const Toolkit::BackgroundBlurEffect& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Toolkit::Internal::BackgroundBlurEffectImpl&>(handle);
}

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_BACKGROUND_BLUR_EFFECT_H

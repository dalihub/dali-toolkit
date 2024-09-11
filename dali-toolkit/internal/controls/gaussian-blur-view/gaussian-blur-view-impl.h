#ifndef DALI_TOOLKIT_INTERNAL_GAUSSIAN_BLUR_EFFECT_H
#define DALI_TOOLKIT_INTERNAL_GAUSSIAN_BLUR_EFFECT_H

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
#include <dali/public-api/object/property-map.h>
#include <cmath>
#include <sstream>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/gaussian-blur-view/gaussian-blur-view.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>

namespace Dali
{
namespace Toolkit
{
class GaussianBlurView;

namespace Internal
{
/**
 * GaussianBlurView implementation class
 */
class GaussianBlurView : public Control
{
public:
  /**
   * @copydoc Dali::Toolkit::GaussianBlurView::GaussianBlurView
   */
  GaussianBlurView();

  /**
   * @copydoc Dali::Toolkit::GaussianBlurView::GaussianBlurView
   */
  GaussianBlurView(const unsigned int numSamples, const float blurBellCurveWidth, const Pixel::Format renderTargetPixelFormat, const float downsampleWidthScale, const float downsampleHeightScale, bool blurUserImage);

  /**
   * @copydoc Dali::Toolkit::GaussianBlurView::~GaussianBlurView
   */
  virtual ~GaussianBlurView();

  /**
   * @copydoc Dali::Toolkit::GaussianBlurView::New
   */
  static Dali::Toolkit::GaussianBlurView New();
  static Dali::Toolkit::GaussianBlurView New(const unsigned int numSamples, const float blurBellCurveWidth, const Pixel::Format renderTargetPixelFormat, const float downsampleWidthScale, const float downsampleHeightScale, bool blurUserImage);

  void Activate();
  void ActivateOnce();
  void Deactivate();

  void SetUserImageAndOutputRenderTarget(Texture inputImage, FrameBuffer outputRenderTarget);

  Property::Index GetBlurStrengthPropertyIndex() const
  {
    return mBlurStrengthPropertyIndex;
  }
  FrameBuffer GetBlurredRenderTarget() const;

  /// @copydoc Dali::Toolkit::GaussianBlurView::SetBackgroundColor(const Vector4&)
  void SetBackgroundColor(const Vector4& color);

  /// @copydoc Dali::Toolkit::GaussianBlurView::GetBackgroundColor
  Vector4 GetBackgroundColor() const;

  void                                                     AllocateResources();
  void                                                     CreateRenderTasks();
  void                                                     RemoveRenderTasks();
  Dali::Toolkit::GaussianBlurView::GaussianBlurViewSignal& FinishedSignal();

private:
  void OnInitialize() override;
  void OnSizeSet(const Vector3& targetSize) override;

  /**
   * @copydoc Control::OnChildAdd()
   */
  void OnChildAdd(Actor& child) override;

  /**
   * @copydoc Control::OnChildRemove()
   */
  void OnChildRemove(Actor& child) override;

  void        SetShaderConstants();
  std::string GetSampleOffsetsPropertyName(unsigned int index) const;
  std::string GetSampleWeightsPropertyName(unsigned int index) const;

  void OnRenderTaskFinished(Dali::RenderTask& renderTask);

  /////////////////////////////////////////////////////////////
  unsigned int  mPixelRadius;    // number of blur samples in each of horiz/vert directions
  float         mBellCurveWidth; // constant used when calculating the gaussian weights
  Pixel::Format mPixelFormat;    // pixel format used by render targets

  /////////////////////////////////////////////////////////////
  // downsampling is used for the separated blur passes to get increased blur with the same number of samples and also to make rendering quicker
  float mDownsampleWidthScale;
  float mDownsampleHeightScale;
  float mDownsampledWidth;
  float mDownsampledHeight;

  /////////////////////////////////////////////////////////////
  // if this is set to true, we blur a user supplied image rather than rendering and blurring children
  bool mBlurUserImage : 1;

  /////////////////////////////////////////////////////////////
  // if this is set to true, set the render tasks to refresh once
  bool mRenderOnce : 1;

  /////////////////////////////////////////////////////////////
  // background fill color
  Vector4 mBackgroundColor;

  /////////////////////////////////////////////////////////////
  // for checking if we need to reallocate render targets
  Vector2 mTargetSize;
  Vector2 mLastSize;

  /////////////////////////////////////////////////////////////
  // for creating a subtree for all user added child actors, so that we can have them exclusive to the mRenderChildrenTask and our other actors exclusive to our other tasks
  Actor mChildrenRoot;
  // for creating a subtree for the internal actors
  Actor mInternalRoot;

  /////////////////////////////////////////////////////////////
  // for mapping offscreen renders to render target sizes
  CameraActor mRenderFullSizeCamera;
  CameraActor mRenderDownsampledCamera;

  /////////////////////////////////////////////////////////////
  // for rendering all user added children to offscreen target
  FrameBuffer mRenderTargetForRenderingChildren;
  RenderTask  mRenderChildrenTask;
  FrameBuffer mBlurResultFrameBuffer;

  /////////////////////////////////////////////////////////////
  // for rendering separated blur passes to offscreen targets
  FrameBuffer mRenderTarget1;
  FrameBuffer mRenderTarget2;

  Actor mHorizontalBlurActor;
  Actor mVerticalBlurActor;

  RenderTask mHorizontalBlurTask;
  RenderTask mVerticalBlurTask;

  /////////////////////////////////////////////////////////////
  // for compositing blur and children renders to offscreen target
  Actor      mCompositingActor;
  RenderTask mCompositeTask;

  /////////////////////////////////////////////////////////////
  // for holding blurred result
  Actor mTargetActor;

  /////////////////////////////////////////////////////////////
  // for animating fade in / out of blur, hiding internal implementation but allowing user to set via GaussianBlurView interface
  Property::Index mBlurStrengthPropertyIndex;

  /////////////////////////////////////////////////////////////
  // User can specify image to blur and output target, so we can use GaussianBlurView for arbitrary blur processes
  Texture     mUserInputImage;
  FrameBuffer mUserOutputRenderTarget;

  Dali::Toolkit::GaussianBlurView::GaussianBlurViewSignal mFinishedSignal; ///< Signal emitted when blur has completed.

  bool mActivated : 1;

private:
  // Undefined copy constructor.
  GaussianBlurView(const GaussianBlurView&);

  // Undefined assignment operator.
  GaussianBlurView& operator=(const GaussianBlurView&);
};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Toolkit::Internal::GaussianBlurView& GetImpl(Toolkit::GaussianBlurView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<Toolkit::Internal::GaussianBlurView&>(handle);
}

inline const Toolkit::Internal::GaussianBlurView& GetImpl(const Toolkit::GaussianBlurView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  const Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<const Toolkit::Internal::GaussianBlurView&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_GAUSSIAN_BLUR_EFFECT_H

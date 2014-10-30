#ifndef __DALI_TOOLKIT_INTERNAL_GAUSSIAN_BLUR_EFFECT_H__
#define __DALI_TOOLKIT_INTERNAL_GAUSSIAN_BLUR_EFFECT_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <sstream>
#include <cmath>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/gaussian-blur-view/gaussian-blur-view.h>

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
  GaussianBlurView(const unsigned int numSamples, const float blurBellCurveWidth, const Pixel::Format renderTargetPixelFormat,
                   const float downsampleWidthScale, const float downsampleHeightScale,
                   bool blurUserImage);

  /**
   * @copydoc Dali::Toolkit::GaussianBlurView::~GaussianBlurView
   */
  virtual ~GaussianBlurView();

  /**
   * @copydoc Dali::Toolkit::GaussianBlurView::New
   */
  static Dali::Toolkit::GaussianBlurView New();
  static Dali::Toolkit::GaussianBlurView New( const unsigned int numSamples, const float blurBellCurveWidth, const Pixel::Format renderTargetPixelFormat,
                                              const float downsampleWidthScale, const float downsampleHeightScale,
                                              bool blurUserImage);

  void Add(Actor child);
  void Remove(Actor child);

  void Activate();
  void ActivateOnce();
  void Deactivate();

  void SetUserImageAndOutputRenderTarget(Image inputImage, FrameBufferImage outputRenderTarget);

  Property::Index GetBlurStrengthPropertyIndex() const {return mBlurStrengthPropertyIndex;}
  FrameBufferImage GetBlurredRenderTarget() const;

  /// @copydoc Dali::Toolkit::GaussianBlurView::SetBackgroundColor(const Vector4&)
  void SetBackgroundColor( const Vector4& color );

  /// @copydoc Dali::Toolkit::GaussianBlurView::GetBackgroundColor
  Vector4 GetBackgroundColor() const;

  void AllocateResources();
  void CreateRenderTasks();
  void RemoveRenderTasks();
  Dali::Toolkit::GaussianBlurView::GaussianBlurViewSignal& FinishedSignal();

private:

  virtual void OnInitialize();
  virtual void OnControlSizeSet(const Vector3& targetSize);
  virtual void OnStageDisconnection();

  virtual void OnControlStageConnection();

  void SetBlurBellCurveWidth(float blurBellCurveWidth);
  float CalcGaussianWeight(float x);
  void SetShaderConstants();
  std::string GetSampleOffsetsPropertyName( unsigned int index ) const;
  std::string GetSampleWeightsPropertyName( unsigned int index ) const;

  void OnRenderTaskFinished(Dali::RenderTask& renderTask);

  /////////////////////////////////////////////////////////////
  unsigned int mNumSamples;       // number of blur samples in each of horiz/vert directions
  float mBlurBellCurveWidth;      // constant used when calculating the gaussian weights
  Pixel::Format mPixelFormat;     // pixel format used by render targets

  /////////////////////////////////////////////////////////////
  // downsampling is used for the separated blur passes to get increased blur with the same number of samples and also to make rendering quicker
  float mDownsampleWidthScale;
  float mDownsampleHeightScale;
  float mDownsampledWidth;
  float mDownsampledHeight;

  /////////////////////////////////////////////////////////////
  // if this is set to true, we blur a user supplied image rather than rendering and blurring children
  bool mBlurUserImage:1;

  /////////////////////////////////////////////////////////////
  // if this is set to true, set the render tasks to refresh once
  bool mRenderOnce:1;

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

  /////////////////////////////////////////////////////////////
  // for mapping offscreen renders to render target sizes
  CameraActor mRenderFullSizeCamera;
  CameraActor mRenderDownsampledCamera;

  /////////////////////////////////////////////////////////////
  // for rendering all user added children to offscreen target
  FrameBufferImage mRenderTargetForRenderingChildren;
  RenderTask mRenderChildrenTask;

  /////////////////////////////////////////////////////////////
  // for rendering separated blur passes to offscreen targets
  FrameBufferImage mRenderTarget1;
  FrameBufferImage mRenderTarget2;

  ShaderEffect mHorizBlurShader;
  ShaderEffect mVertBlurShader;

  ImageActor mImageActorHorizBlur;
  ImageActor mImageActorVertBlur;

  RenderTask mHorizBlurTask;
  RenderTask mVertBlurTask;

  /////////////////////////////////////////////////////////////
  // for compositing blur and children renders to offscreen target
  ImageActor mImageActorComposite;
  RenderTask mCompositeTask;

  /////////////////////////////////////////////////////////////
  // for holding blurred result
  ImageActor mTargetActor;

  /////////////////////////////////////////////////////////////
  // for animating fade in / out of blur, hiding internal implementation but allowing user to set via GaussianBlurView interface
  Property::Index mBlurStrengthPropertyIndex;

  /////////////////////////////////////////////////////////////
  // User can specify image to blur and output target, so we can use GaussianBlurView for arbitrary blur processes
  Image mUserInputImage;
  FrameBufferImage mUserOutputRenderTarget;

  Dali::Toolkit::GaussianBlurView::GaussianBlurViewSignal mFinishedSignal; ///< Signal emitted when blur has completed.
private:

  // Undefined copy constructor.
  GaussianBlurView( const GaussianBlurView& );

  // Undefined assignment operator.
  GaussianBlurView& operator=( const GaussianBlurView& );
};

} // namespace Internal


// Helpers for public-api forwarding methods
inline Toolkit::Internal::GaussianBlurView& GetImpl( Toolkit::GaussianBlurView& obj )
{
  DALI_ASSERT_ALWAYS(obj);
  Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<Toolkit::Internal::GaussianBlurView&>(handle);
}

inline const Toolkit::Internal::GaussianBlurView& GetImpl( const Toolkit::GaussianBlurView& obj )
{
  DALI_ASSERT_ALWAYS(obj);
  const Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<const Toolkit::Internal::GaussianBlurView&>(handle);
}


} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_GAUSSIAN_BLUR_EFFECT_H__

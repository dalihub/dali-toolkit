#ifndef __DALI_TOOLKIT_INTERNAL_BLOOM_VIEW_H__
#define __DALI_TOOLKIT_INTERNAL_BLOOM_VIEW_H__

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
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/render-tasks/render-task.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/gaussian-blur-view/gaussian-blur-view.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/devel-api/controls/bloom-view/bloom-view.h>

namespace Dali
{

namespace Toolkit
{

class BloomView;

namespace Internal
{

/**
 * BloomEffect implementation class
 */
class BloomView : public Control
{
public:
  /**
   * @copydoc Dali::Toolkit::BloomView::BloomView
   */
  BloomView();

  /**
   * @copydoc Dali::Toolkit::BloomView::BloomView
   */
  BloomView(const unsigned int numSamples, const float blurBellCurveWidth, const Pixel::Format renderTargetPixelFormat,
                   const float downsampleWidthScale, const float downsampleHeightScale);

  /**
   * @copydoc Dali::Toolkit::BloomView::~BloomView
   */
  virtual ~BloomView();

  /**
   * @copydoc Dali::Toolkit::BloomView::New
   */
  static Dali::Toolkit::BloomView New();
  static Dali::Toolkit::BloomView New( const unsigned int numSamples, const float blurBellCurveWidth, const Pixel::Format renderTargetPixelFormat,
                                              const float downsampleWidthScale, const float downsampleHeightScale);
  void Activate();
  void Deactivate();

  Property::Index GetBloomThresholdPropertyIndex() const {return mBloomThresholdPropertyIndex;}
  Property::Index GetBlurStrengthPropertyIndex() const {return mBlurStrengthPropertyIndex;}
  Property::Index GetBloomIntensityPropertyIndex() const {return mBloomIntensityPropertyIndex;}
  Property::Index GetBloomSaturationPropertyIndex() const {return mBloomSaturationPropertyIndex;}
  Property::Index GetImageIntensityPropertyIndex() const {return mImageIntensityPropertyIndex;}
  Property::Index GetImageSaturationPropertyIndex() const {return mImageSaturationPropertyIndex;}

private:

  virtual void OnInitialize();
  virtual void OnSizeSet(const Vector3& targetSize);

  /**
   * @copydoc Control::OnChildAdd()
   */
  virtual void OnChildAdd( Actor& child );

  /**
   * @copydoc Control::OnChildRemove()
   */
  virtual void OnChildRemove( Actor& child );

  void AllocateResources();
  void CreateRenderTasks();
  void RemoveRenderTasks();

  void SetupProperties();


  /////////////////////////////////////////////////////////////
  unsigned int mBlurNumSamples;   // number of blur samples in each of horiz/vert directions
  float mBlurBellCurveWidth;      // constant used when calculating the gaussian weights
  Pixel::Format mPixelFormat;     // pixel format used by render targets

  /////////////////////////////////////////////////////////////
  // downsampling is used for the separated blur passes to get increased blur with the same number of samples and also to make rendering quicker
  float mDownsampleWidthScale;
  float mDownsampleHeightScale;
  float mDownsampledWidth;
  float mDownsampledHeight;


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
  FrameBufferImage mRenderTargetForRenderingChildren;
  RenderTask mRenderChildrenTask;

  /////////////////////////////////////////////////////////////
  // for extracting bright parts of image to an offscreen target
  FrameBufferImage mBloomExtractTarget; // for rendering bright parts of image into separate texture, also used as target for gaussian blur
  RenderTask mBloomExtractTask;
  Toolkit::ImageView mBloomExtractImageActor;

  /////////////////////////////////////////////////////////////
  // for blurring extracted bloom
  Dali::Toolkit::GaussianBlurView mGaussianBlurView;

  /////////////////////////////////////////////////////////////
  // for compositing bloom and children renders to offscreen target
  RenderTask mCompositeTask;

  Toolkit::ImageView mCompositeImageActor;

  /////////////////////////////////////////////////////////////
  // for holding blurred result
  FrameBufferImage mOutputRenderTarget;
  Toolkit::ImageView mTargetImageActor;

  /////////////////////////////////////////////////////////////
  // Properties for setting by user, e.g. by animations
  Property::Index mBloomThresholdPropertyIndex;
  Property::Index mBlurStrengthPropertyIndex;
  Property::Index mBloomIntensityPropertyIndex;
  Property::Index mBloomSaturationPropertyIndex;
  Property::Index mImageIntensityPropertyIndex;
  Property::Index mImageSaturationPropertyIndex;

  bool mActivated:1;

private:

  // Undefined copy constructor.
  BloomView( const BloomView& );

  // Undefined assignment operator.
  BloomView& operator=( const BloomView& );
};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Toolkit::Internal::BloomView& GetImpl( Toolkit::BloomView& obj )
{
  DALI_ASSERT_ALWAYS(obj);
  Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<Toolkit::Internal::BloomView&>(handle);
}

inline const Toolkit::Internal::BloomView& GetImpl( const Toolkit::BloomView& obj )
{
  DALI_ASSERT_ALWAYS(obj);
  const Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<const Toolkit::Internal::BloomView&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_BLOOM_VIEW_H__

#ifndef __DALI_TOOLKIT_INTERNAL_BLUR_TWO_PASS_FILTER_H__
#define __DALI_TOOLKIT_INTERNAL_BLUR_TWO_PASS_FILTER_H__

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
#include <dali/public-api/render-tasks/render-task.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali/public-api/actors/image-actor.h>

// INTERNAL INCLUDES
#include "image-filter.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * A two pass blur filter, pass one performs a horizontal blur and pass two performs a
 * vertical blur on the result of pass one.
 */
class BlurTwoPassFilter : public ImageFilter
{
public:
  /**
   * Default constructor
   */
  BlurTwoPassFilter();

  /**
   * Destructor
   */
  virtual ~BlurTwoPassFilter();

public: // From ImageFilter
  /// @copydoc Dali::Toolkit::Internal::ImageFilter::Enable
  virtual void Enable();

  /// @copydoc Dali::Toolkit::Internal::ImageFilter::Disable
  virtual void Disable();

  /// @copydoc Dali::Toolkit::Internal::ImageFilter::Refresh
  virtual void Refresh();

  /// @copydoc Dali::Toolkit::Internal::ImageFilter::SetSize
  virtual void SetSize( const Vector2& size );

  /**
   * Get the property index that controls the strength of the blur applied to the image. Useful for animating this property.
   * This property represents a value in the range [0.0 - 1.0] where 0.0 is no blur and 1.0 is full blur.
   */
  Property::Index GetBlurStrengthPropertyIndex() const {return mBlurStrengthPropertyIndex;}

  /**
   * Retrieve the handle to the object in order to animate or constrain the blur strength property
   * @return The hadnle to the object which blends the output image according to the blur strength
   */
  Handle GetHandleForAnimateBlurStrength();

private:

  /**
   * Setup render tasks for blur
   */
  void CreateRenderTasks();

private:
  BlurTwoPassFilter( const BlurTwoPassFilter& );
  BlurTwoPassFilter& operator=( const BlurTwoPassFilter& );

private: // Attributes

  // To perform horizontal blur from mInputImage to mImageForHorz
  RenderTask         mRenderTaskForHorz;
  Toolkit::ImageView mActorForInput;
  FrameBufferImage   mImageForHorz;

  // To perform vertical blur from mImageForHorz to mOutputImage
  RenderTask         mRenderTaskForVert;
  Toolkit::ImageView mActorForHorz;
  FrameBufferImage   mBlurredImage;

  // To blend the blurred image and input image according to the blur strength
  RenderTask         mRenderTaskForBlending;
  Toolkit::ImageView mActorForBlending;
  Actor              mRootActorForBlending;
  Property::Index    mBlurStrengthPropertyIndex;

}; // class BlurTwoPassFilter

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_BLUR_TWO_PASS_FILTER_H__


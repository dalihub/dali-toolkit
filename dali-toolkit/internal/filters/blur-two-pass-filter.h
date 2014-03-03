#ifndef __DALI_TOOLKIT_INTERNAL_BLUR_TWO_PASS_FILTER_H__
#define __DALI_TOOLKIT_INTERNAL_BLUR_TWO_PASS_FILTER_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali/dali.h>
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

  /**
   * Get the property index that controls the strength of the blur applied to the image. Useful for animating this property.
   * This property represents a value in the range [0.0 - 1.0] where 0.0 is no blur and 1.0 is full blur.
   */
  Property::Index GetBlurStrengthPropertyIndex() const {return mBlurStrengthPropertyIndex;}

  /**
   * Retrieve the constrainable object to animate or constrain the blur strength property
   * @return the constrainable object which blend the output image according to the blur strength
   */
  Constrainable GetHandleForAnimateBlurStrength();

private:
  /**
   * Setup position and parameters for camera
   */
  void SetupCamera();

  /**
   * Setup render tasks for blur
   */
  void CreateRenderTasks();

private:
  BlurTwoPassFilter( const BlurTwoPassFilter& );
  BlurTwoPassFilter& operator=( const BlurTwoPassFilter& );

private: // Attributes

  CameraActor      mCameraForBlur;

  // To perform horizontal blur from mInputImage to mImageForHorz
  RenderTask       mRenderTaskForHorz;
  ImageActor       mActorForInput;
  FrameBufferImage mImageForHorz;
  ShaderEffect     mShaderForHorz;

  // To perform vertical blur from mImageForHorz to mOutputImage
  RenderTask       mRenderTaskForVert;
  ImageActor       mActorForHorz;
  ShaderEffect     mShaderForVert;
  FrameBufferImage mBlurredImage;

  // To blend the blurred image and input image according to the blur strength
  RenderTask       mRenderTaskForBlending;
  ImageActor       mActorForBlending;
  Actor            mRootActorForBlending;
  ShaderEffect     mShaderForBlending;
  Property::Index  mBlurStrengthPropertyIndex;

}; // class BlurTwoPassFilter

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_BLUR_TWO_PASS_FILTER_H__


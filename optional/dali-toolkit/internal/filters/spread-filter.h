#ifndef __DALI_TOOLKIT_INTERNAL_SPREAD_FILTER_H__
#define __DALI_TOOLKIT_INTERNAL_SPREAD_FILTER_H__

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
 * A spread/thicken filter. Expands an image into transparent areas.
 */
class SpreadFilter : public ImageFilter
{
public:
  /**
   * Default constructor
   */
  SpreadFilter();

  /**
   * Destructor
   */
  virtual ~SpreadFilter();

  /**
   * Set the amount of spread in pixels.
   * @param[in] spread The amount of spread
   */
  void SetSpread( float spread );

public: // From ImageFilter
  /// @copydoc Dali::Toolkit::Internal::ImageFilter::Enable
  virtual void Enable();

  /// @copydoc Dali::Toolkit::Internal::ImageFilter::Disable
  virtual void Disable();

  /// @copydoc Dali::Toolkit::Internal::ImageFilter::Refresh
  virtual void Refresh();

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
  SpreadFilter( const SpreadFilter& );
  SpreadFilter& operator=( const SpreadFilter& );

private: // Attributes
  int              mSpread;
  CameraActor      mCameraActor;

  // To perform horizontal spread from mInputImage to mImageForHorz
  RenderTask       mRenderTaskForHorz;
  ImageActor       mActorForInput;
  FrameBufferImage mImageForHorz;
  ShaderEffect     mShaderForHorz;

  // To perform vertical spread from mImageForHorz to mOutputImage
  RenderTask       mRenderTaskForVert;
  ImageActor       mActorForHorz;
  ShaderEffect     mShaderForVert;
}; // class SpreadFilter

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_SPREAD_FILTER_H__


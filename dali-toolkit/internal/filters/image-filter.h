#ifndef __DALI_TOOLKIT_INTERNAL_IMAGE_FILTER_H__
#define __DALI_TOOLKIT_INTERNAL_IMAGE_FILTER_H__

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
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/effects-view/effects-view.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * An interface class that provides a interface for image filters that perform
 * a simple shader effect on an input image, rendering the output to a FrameBufferImage.
 */
class ImageFilter
{
public:
  typedef std::vector< Vector3 > FilterKernel;

public:

  /**
   * Default constructor
   */
  ImageFilter();

  /**
   * Destructor
   */
  virtual ~ImageFilter();

  /**
   * Enable effect, allocates any necessary resources
   */
  virtual void Enable() = 0;

  /**
   * Disable effect, releases any allocated resources
   */
  virtual void Disable() = 0;

  /**
   * Refresh the filter output
   */
  virtual void Refresh() = 0;

  /**
   * @copydoc Dali::Toolkit::EffectsView::SetRefreshOnDemand
   */
  void SetRefreshOnDemand( bool onDemand );

  /**
   * Set the input image
   * @param[in] The input/original image.
   */
  void SetInputImage( Image image );

  /**
   * Set the output image
   * @return The output image.
   */
  void SetOutputImage( FrameBufferImage image );

  /**
   * Set size of ImageFilter. Used to create internal offscreen buffers
   * @param[in] size  THe size.
   */
  void SetSize( const Vector2& size );

  /**
   * Set the pixel format for internal offscreen buffers
   * @param[in] pixelFormat The pixel format.
   */
  void SetPixelFormat( Pixel::Format pixelFormat );

  /**
   * Set the filter kernel
   * @param[in] The filter kernel
   */
  void SetKernel( const FilterKernel& kernel );

  /**
   * Get a const reference to the internal filter kernel
   * @Return A a const reference to the internal filter kernel
   */
  const FilterKernel& GetKernel() const;

  /**
   * Get the number of steps/elements in the kernel
   * @return The number of steps/elements in the kernel
   */
  size_t GetKernelSize() const;

  /**
   * Create a kernel from an array of weights
   * @param[in] weights
   * @param[in] count
   */
  void CreateKernel( const float* weights, size_t count);

  /**
   * Set the actor which acts as the root actor for all internal actors for connection to stage
   * @param[in] rootActor   An actor which acts as the root actor for any internal actors that need
   *                        to be created
   */
  void SetRootActor( Actor rootActor );

  /**
   * Set the background / clear color
   * @param[in] color The background / clear color
   */
  void SetBackgroundColor( const Vector4& color );

  /**
   * Enable optional debug output in the shader
   * @param[in] flag Set true to enable, dalse to disable.
   */
  void RenderDebug( bool flag );

protected:
  Image            mInputImage;
  FrameBufferImage mOutputImage;
  FilterKernel     mKernel;
  Actor            mRootActor;
  Vector4          mBackgroundColor;
  Vector2          mTargetSize;
  Pixel::Format    mPixelFormat;
  bool             mRefreshOnDemand;
  bool             mDebugRender;
}; // class Imagefilter

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_IMAGE_FILTER_H__


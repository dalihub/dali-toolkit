#ifndef __DALI_TOOLKIT_MASKED_IMAGE_VIEW_H__
#define __DALI_TOOLKIT_MASKED_IMAGE_VIEW_H__

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

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class MaskedImageView;
}

/**
 * MaskedImageView displays the result of an image created from a masking operation, which is described below:
 *
 *  - Firstly a target image size is chosen. The MaskedImageView handles the creation of this image internally. Initially the
 *    target image will be filled according to the BACKGROUND_COLOR property.
 *  - A source image is provided and positioned with the target image area. The position of the source image (in pixels), can
 *    be controlled using the SOURCE_OFFSET and SOURCE_SIZE properties. By default the source image is centered within the target
 *    image, and stretched to fill. Note that by default, no attempt is made to maintain the aspect ratio of the source image.
 *  - A mask image is provided and positioned in the same way as the source image, using the MASK_OFFSET and MASK_SIZE properties.
 *  - Conceptually the source image is then painted using the mask image as a stencil. Areas of the source which overlap with opaque
 *    areas of the mask, will be painted into the target image. However where the mask is transparent, the source will be faded away.
 *    Note that the edge of the mask image will be stretched to cover the entire target area.
 *
 * Initially MaskedImageView will perform the masking operation on a per-frame basis. This can impact performance, and may be
 * avoided by calling Pause() e.g. when the source & mask positions are not being modified. The Resume() method can then be called
 * to continue the masking operation when required.
 */
class MaskedImageView : public Control
{
public:

  /**
   * The custom properties installed by this control
   */
  enum CustomProperty
  {
    BACKGROUND_COLOR,    ///< Name "background-color", type VECTOR4
    SOURCE_SIZE,         ///< Name "source-size",      type VECTOR2
    SOURCE_OFFSET,       ///< Name "source-offset",    type VECTOR2
    MASK_SIZE,           ///< Name "mask-size",        type VECTOR2
    MASK_OFFSET,         ///< Name "mask-offset",      type VECTOR2

    CUSTOM_PROPERTY_COUNT
  };

  enum EditMode
  {
    EDIT_DISABLED,
    EDIT_SOURCE,
    EDIT_MASK
  };

  enum ImageRotation
  {
    ROTATE_0,   ///< No rotation
    ROTATE_90,  ///< Image is rotated clockwise by 90 degrees
    ROTATE_180, ///< Image is rotated clockwise by 180 degrees
    ROTATE_270  ///< Image is rotated clockwise by 270 degrees
  };

  static const float DEFAULT_MAXIMUM_SOURCE_SCALE; ///< Default SetMaximumSourceScale() value

  /**
   * Creates an empty MaskedImageView handle
   */
  MaskedImageView();

  /**
   * Copy constructor. Creates another handle that points to the same real object
   * @param handle to copy from
   */
  MaskedImageView( const MaskedImageView& handle );

  /**
   * Assignment operator. Changes this handle to point to another real object
   */
  MaskedImageView& operator=( const MaskedImageView& handle );

  /**
   * Virtual destructor.
   * Dali::Object derived classes typically do not contain member data.
   */
  virtual ~MaskedImageView();

  /**
   * Create the MaskedImageView control
   * @param[in] targetWidth The width of the target image
   * @param[in] targetHeight The height of the target image
   * @param[in] sourceImage The source image
   * @param[in] maskImage The mask image
   * @return A handle to the MaskedImageView control.
   */
  static MaskedImageView New( unsigned int targetWidth,
                              unsigned int targetHeight,
                              Image sourceImage,
                              Image maskImage );

  /**
   * Downcast an Object handle to MaskedImageView. If handle points to an MaskedImageView the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a MaskedImageView or an uninitialized handle
   */
  static MaskedImageView DownCast( BaseHandle handle );

  /**
   * Set the image used as a source in the masking operation
   * @param[in] sourceImage The source image
   */
  void SetSourceImage( Image sourceImage );

  /**
   * Retrieve the image used as a source in the masking operation
   * @return sourceImage The source image
   */
  Image GetSourceImage();

  /**
   * Set the image used as a mask in the masking operation
   * @param[in] maskImage The mask image
   */
  void SetMaskImage( Image maskImage );

  /**
   * Retrieve the image used as a mask in the masking operation
   * @return sourceImage The mask image
   */
  Image GetMaskImage();

  /**
   * Get the property index for a custom MaskedImageView property.
   * @param[in] customProperty A custom property enum defined in this class.
   * @return The property index e.g. for use with Animation::AnimateTo()
   */
  Property::Index GetPropertyIndex( CustomProperty customProperty ) const;

  /**
   * Pause the masking operation to improve performance.
   * Call this when the source & mask positions etc. are not being modified.
   */
  void Pause();

  /**
   * Resume the masking operation.
   */
  void Resume();

  /**
   * Query whether the masking operation has been paused.
   * @return True if the masking operation has been paused.
   */
  bool IsPaused() const;

  /**
   * Enable or disable an edit mode. The default is EDIT_DISABLED.
   * @param[in] editMode The edit mode required.
   */
  void SetEditMode( EditMode editMode );

  /**
   * Query which edit mode is enabled.
   */
  EditMode GetEditMode() const;

  /**
   * Set the aspect ratio to be preserved when editing the source image.
   * @param[in] widthOverHeight The aspect ratio i.e. width divided by height. If a value
   * of zero or less is set, then the aspect ratio of the source image will be ignored.
   */
  void SetSourceAspectRatio( float widthOverHeight );

  /**
   * Query the aspect ratio preserved when editing the source image.
   * @return The aspect ratio (width divided by height) or zero if no aspect ratio is set.
   */
  float GetSourceAspectRatio() const;

  /**
   * Set the maximum scale applied when editing the source image.
   * The minimum scale is implied by the target width/height i.e. the source image will
   * always fill that area when edit mode is enabled.
   * @param[in] scale The maximum scale.
   */
  void SetMaximumSourceScale( float scale );

  /**
   * Query the maximum scale applied when editing the source image.
   * @return The maximum scale.
   */
  float GetMaximumSourceScale() const;

  /**
   * Set the rotation applied to the source image.
   * @param[in] rotation The new rotation; by default the source image is not rotated (ROTATE_0).
   */
  void SetSourceRotation( ImageRotation rotation );

  /**
   * Query the rotation applied to the source image.
   * @return The current rotation.
   */
  ImageRotation GetSourceRotation() const;

public: /* Signals */

  typedef SignalV2< void (MaskedImageView& source) > MaskedImageViewSignal;

  /**
   * Signal emitted when the render task which targets the frame buffer of the masked image has finished.
   * This signal carries information of the control handle to the callback function.
   */
  MaskedImageViewSignal& MaskFinishedSignal();

  /**
   * @deprecated Use MaskFinishedSignal() instead.
   * Signal emitted when the render task which targets the frame buffer of the masked image has finished.
   */
  Dali::RenderTask::RenderTaskSignalV2& RenderFinishedSignal();

public: // Not intended for application developers

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  MaskedImageView(Internal::MaskedImageView& implementation);

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  MaskedImageView(Dali::Internal::CustomActor* internal);
};

} // namespace Toolkit

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TOOLKIT_MASKED_IMAGE_VIEW_H__

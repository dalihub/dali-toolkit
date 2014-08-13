#ifndef __DALI_TOOLKIT_MASKED_IMAGE_VIEW_H__
#define __DALI_TOOLKIT_MASKED_IMAGE_VIEW_H__

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
 * @brief MaskedImageView displays the result of an image created from a masking operation.
 *
 * Masking operations:
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
   * @brief The custom properties installed by this control.
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

  /**
   * @brief Edit mode for this control.
   *
   * @see SetEditMode()
   */
  enum EditMode
  {
    EDIT_DISABLED, ///< Editting is disabled
    EDIT_SOURCE,   ///< Editting affects the source image
    EDIT_MASK      ///< Editting affects the mask
  };

  /**
   * @brief The rotation of the image.
   *
   * @see SetSourceRotation()
   */
  enum ImageRotation
  {
    ROTATE_0,   ///< No rotation
    ROTATE_90,  ///< Image is rotated clockwise by 90 degrees
    ROTATE_180, ///< Image is rotated clockwise by 180 degrees
    ROTATE_270  ///< Image is rotated clockwise by 270 degrees
  };

  static const float DEFAULT_MAXIMUM_SOURCE_SCALE; ///< Default SetMaximumSourceScale() value

  /**
   * @brief Creates an empty MaskedImageView handle.
   */
  MaskedImageView();

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object
   * @param handle to copy from
   */
  MaskedImageView( const MaskedImageView& handle );

  /**
   * @brief Assignment operator.
   *
   * Changes this handle to point to another real object
   * @param[in] handle the handle of the object to re-assign this to
   * @return a reference to this
   */
  MaskedImageView& operator=( const MaskedImageView& handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~MaskedImageView();

  /**
   * @brief Create the MaskedImageView control.
   *
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
   * @brief Downcast an Object handle to MaskedImageView.
   *
   * If handle points to an MaskedImageView the downcast produces
   * valid handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return handle to a MaskedImageView or an uninitialized handle
   */
  static MaskedImageView DownCast( BaseHandle handle );

  /**
   * @brief Set the image used as a source in the masking operation.
   *
   * @param[in] sourceImage The source image
   */
  void SetSourceImage( Image sourceImage );

  /**
   * @brief Retrieve the image used as a source in the masking operation.
   *
   * @return sourceImage The source image
   */
  Image GetSourceImage();

  /**
   * @brief Set the image used as a mask in the masking operation.
   *
   * @param[in] maskImage The mask image
   */
  void SetMaskImage( Image maskImage );

  /**
   * @brief Retrieve the image used as a mask in the masking operation.
   *
   * @return sourceImage The mask image
   */
  Image GetMaskImage();

  /**
   * @brief Get the property index for a custom MaskedImageView property.
   *
   * @param[in] customProperty A custom property enum defined in this class.
   * @return The property index e.g. for use with Animation::AnimateTo()
   */
  Property::Index GetPropertyIndex( CustomProperty customProperty ) const;

  /**
   * @brief Pause the masking operation to improve performance.
   *
   * Call this when the source & mask positions etc. are not being modified.
   */
  void Pause();

  /**
   * @brief Resume the masking operation.
   *
   */
  void Resume();

  /**
   * @brief Query whether the masking operation has been paused.
   *
   * @return True if the masking operation has been paused.
   */
  bool IsPaused() const;

  /**
   * @brief Enable or disable an edit mode.
   *
   * The default is EDIT_DISABLED.
   * @param[in] editMode The edit mode required.
   */
  void SetEditMode( EditMode editMode );

  /**
   * @brief Query which edit mode is enabled.
   */
  EditMode GetEditMode() const;

  /**
   * @brief Set the aspect ratio to be preserved when editing the source image.
   *
   * @param[in] widthOverHeight The aspect ratio i.e. width divided by height. If a value
   * of zero or less is set, then the aspect ratio of the source image will be ignored.
   */
  void SetSourceAspectRatio( float widthOverHeight );

  /**
   * @brief Query the aspect ratio preserved when editing the source image.
   *
   * @return The aspect ratio (width divided by height) or zero if no aspect ratio is set.
   */
  float GetSourceAspectRatio() const;

  /**
   * @brief Set the maximum scale applied when editing the source image.
   *
   * The minimum scale is implied by the target width/height i.e. the source image will
   * always fill that area when edit mode is enabled.
   * @param[in] scale The maximum scale.
   */
  void SetMaximumSourceScale( float scale );

  /**
   * @brief Query the maximum scale applied when editing the source image.
   *
   * @return The maximum scale.
   */
  float GetMaximumSourceScale() const;

  /**
   * @brief Set the rotation applied to the source image.
   *
   * @param[in] rotation The new rotation; by default the source image is not rotated (ROTATE_0).
   */
  void SetSourceRotation( ImageRotation rotation );

  /**
   * @brief Query the rotation applied to the source image.
   *
   * @return The current rotation.
   */
  ImageRotation GetSourceRotation() const;

public: /* Signals */

  /// @brief Finished signal type.
  typedef SignalV2< void (MaskedImageView& source) > MaskedImageViewSignal;

  /**
   * @brief Signal emitted when the render task which targets the
   * frame buffer of the masked image has finished.
   *
   * This signal carries information of the control handle to the callback function.
   * @return the signal
   */
  MaskedImageViewSignal& MaskFinishedSignal();

public: // Not intended for application developers

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in]  implementation  The Control implementation.
   */
  MaskedImageView(Internal::MaskedImageView& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  MaskedImageView(Dali::Internal::CustomActor* internal);
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_MASKED_IMAGE_VIEW_H__

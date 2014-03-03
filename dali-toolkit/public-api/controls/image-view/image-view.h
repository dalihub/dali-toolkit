#ifndef __DALI_TOOLKIT_IMAGE_VIEW_H__
#define __DALI_TOOLKIT_IMAGE_VIEW_H__

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class ImageView;
}

class Button;

/**
 * ImageView control loads and displays the correct
 * image for the current level of detail (LOD) required.
 * LOD is typically calculated from the Camera distance.
 *
 * Example:
 *
 * ImageView imageView = ImageView::New();
 * imageView.SetCameraActor( mCamera );
 * imageView.SetSize( Vector2(64.0f, 64.0f) );
 * imageView.SetImage( "my-image.png", ImageView::BitmapType, 0.125f, 4.0f );
 * layer.Add(imageView);
 *
 * The above creates an ImageView at 64x64 in size. Images of 12.5% the size up
 * to 400% the size of imageView are created
 * i.e. 8x8, 16x16, 32x32, 64x64, 128x128, and 256x256
 *
 * based on the distance imageView is from mCamera an appropriate, different
 * image will be loaded and dispayed.
 */
class ImageView : public Control
{
public:

  /**
   * Image Types, determines how image should be rendered.
   */
  enum ImageType
  {
    BitmapType,                            ///< Standard Bitmap image
    DistanceFieldType                      ///< Distance Field encoded image
  };

  static const std::string DETAIL_PROPERTY_NAME;                          ///< The level of detail property

public:

  /**
   * Creates an empty ImageView handle
   */
  ImageView();

  /**
   * Copy constructor. Creates another handle that points to the same real object
   * @param handle to copy from
   */
  ImageView( const ImageView& handle );

  /**
   * Assignment operator. Changes this handle to point to another real object
   */
  ImageView& operator=( const ImageView& handle );

  /**
   * Virtual destructor.
   * Dali::Object derived classes typically do not contain member data.
   */
  virtual ~ImageView();

  /**
   * Create the Poup control
   * @return A handle to the ImageView control.
   */
  static ImageView New();

  /**
   * Downcast an Object handle to ImageView. If handle points to an ImageView the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a ImageView or an uninitialized handle
   */
  static ImageView DownCast( BaseHandle handle );

public:

  /**
   * Load image into ImageView for level of detail scaling.
   * Will automatically create different sized versions
   * of the source image.
   *
   * @param[in] filename The image path to load
   * @param[in] type The type of image e.g. BitmapType or DistanceFieldType
   */
  void SetImage(const std::string& filename, ImageType type);

  /**
   * Load image into ImageView for level of detail scaling.
   * The minimum scale is a percentage of the size of the
   * image view, and represents the smallest version of the
   * source image to display e.g. 0.125 for 12.5%
   * While the maximum scale represents the largest version of
   * the source image to display e.g. 1.00 for 100% (original
   * image view size)
   *
   * @note ImageView SetSize must be set specified prior to
   * calling this.
   *
   * @param[in] filename The image path to load
   * @param[in] type The type of image e.g. BitmapImage or DistanceFieldImage
   * @param[in] min The minimum scale detail to load.
   * @param[in] max The maximum scale detail to load.
   */
  void SetImage(const std::string& filename, ImageType type, float min, float max);

  /**
   * Sets an image to displayed for the entire detail range.
   * Regardless of the detail level this image will be displayed.
   *
   * @param[in] image The image to display
   */
  void SetImage(Image image);

  /**
   * Sets the camera to use for determining level of detail.
   * Which is based on distance from camera to this ImageView.
   * The detailFactor is the distance at which the ImageView
   * should appear at 100% scale. Which may differ based on
   * Projection, and ShaderEffect settings.
   * @param[in] camera The camera
   */
  void SetCameraActor(CameraActor camera);

  /**
   * Sets the camera to use for determining level of detail.
   * Which is based on distance from camera to this ImageView.
   * The detailFactor is the distance at which the ImageView
   * should appear at 100% scale. Which may differ based on
   * Projection, and ShaderEffect settings.
   * @param[in] camera The camera
   * @param[in] detailFactor The Camera distance where detail should be 1.0
   * (ImageView should appear at 100% scale)
   */
  void SetCameraActor(CameraActor camera, float detailFactor);

  /**
   * Sets the current detail level.
   * @note This sets the detail property value.
   * @param[in] detail The level of detail to be viewed at.
   */
  void SetDetail(float detail);

public: // Not intended for application developers

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  ImageView(Internal::ImageView& implementation);

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  ImageView(Dali::Internal::CustomActor* internal);
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_IMAGE_VIEW_H__

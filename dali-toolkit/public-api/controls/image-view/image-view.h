#ifndef __DALI_TOOLKIT_IMAGE_VIEW_H__
#define __DALI_TOOLKIT_IMAGE_VIEW_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/public-api/images/image-operations.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class ImageView;
}
/**
 * @addtogroup dali_toolkit_controls_image_view
 * @{
 */

/**
 * @brief ImageView is a class for displaying an image resource.
 *
 * An instance of ImageView can be created using a URL or an Image instance.
 *
 * @SINCE_1_0.0
 *
 */
class DALI_IMPORT_API ImageView : public Control
{
public:

  /**
   * @brief The start and end property ranges for this control.
   * @SINCE_1_0.0
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,  ///< @SINCE_1_0.0
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000,              ///< Reserve property indices @SINCE_1_0.0

    ANIMATABLE_PROPERTY_START_INDEX = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX,        ///< @SINCE_1_1.18
    ANIMATABLE_PROPERTY_END_INDEX =   ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 1000  ///< Reserve animatable property indices, @SINCE_1_1.18
  };

  /**
   * @brief An enumeration of properties belonging to the ImageView class.
   * @SINCE_1_0.0
   */
  struct Property
  {
    enum
    {
      // Event side properties

      /**
       * @DEPRECATED_1_1.16. Use IMAGE instead.
       * @brief name "resourceUrl", type string
       * @SINCE_1_0.0
       */
      RESOURCE_URL = PROPERTY_START_INDEX,

      /**
       * @brief name "image", type string if it is a url, map otherwise
       * @SINCE_1_0.0
       */
      IMAGE,

      /**
       * @brief name "preMultipliedAlpha", type Boolean
       * @SINCE_1_1.18
       * @pre image must be initialized.
       */
      PRE_MULTIPLIED_ALPHA,


      // Animatable properties

      /**
       * @brief name "pixelArea", type Vector4
       * @details Pixel area is a relative value with the whole image area as [0.0, 0.0, 1.0, 1.0].
       * @SINCE_1_1.18
       */
      PIXEL_AREA = ANIMATABLE_PROPERTY_START_INDEX,
    };
  };

public:

  /**
   * @brief Create an uninitialized ImageView.
   * @SINCE_1_0.0
   */
  ImageView();

  /**
   * @brief Create an initialized ImageView.
   *
   * @SINCE_1_0.0
   * @return A handle to a newly allocated Dali ImageView.
   *
   * @note ImageView will not display anything.
   */
  static ImageView New();

  /**
   * @brief Create an initialized ImageView from an Image instance.
   *
   * If the handle is empty, ImageView will not display anything.
   *
   * @SINCE_1_0.0
   * @param[in] image The Image instance to display.
   * @return A handle to a newly allocated ImageView.
   */
  static ImageView New( Image image );

  /**
   * @brief Create an initialized ImageView from an URL to an image resource.
   *
   * If the string is empty, ImageView will not display anything.
   *
   * @SINCE_1_0.0
   * @param[in] url The url of the image resource to display.
   * @return A handle to a newly allocated ImageView.
   */
  static ImageView New( const std::string& url );

  /**
   * @brief Create an initialized ImageView from a URL to an image resource.
   *
   * If the string is empty, ImageView will not display anything.
   *
   * @SINCE_1_1.10
   * @param[in] url The url of the image resource to display.
   * @param [in] size The width and height to which to fit the loaded image.
   * @return A handle to a newly allocated ImageView.
   *
   * @note A valid size is preferable for efficiency.
   *       However, do not set a size that is bigger than the actual image size, as up-scaling is not available.
   *       The content of the area not covered by the actual image is undefined and will not be cleared.
   */
  static ImageView New( const std::string& url, ImageDimensions size );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~ImageView();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_1_0.0
   * @param[in] imageView ImageView to copy. The copied ImageView will point at the same implementation
   */
  ImageView( const ImageView& imageView );

  /**
   * @brief Assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] imageView The ImageView to assign from.
   * @return The updated ImageView.
   */
  ImageView& operator=( const ImageView& imageView );

  /**
   * @brief Downcast a handle to ImageView handle.
   *
   * If handle points to a ImageView the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return handle to a ImageView or an uninitialized handle
   */
  static ImageView DownCast( BaseHandle handle );

  /**
   * @brief Sets this ImageView from an Image instance.
   *
   * If the handle is empty, ImageView will display nothing
   * @SINCE_1_0.0
   * @param[in] image The Image instance to display.
   */
  void SetImage( Image image );

  /**
   * @brief Sets this ImageView from the given URL.
   *
   * If the URL is empty, ImageView will not display anything.
   *
   * @SINCE_1_1.4
   * @param[in] url The URL to the image resource to display.
   */
  void SetImage( const std::string& url );

  /**
   * @brief Sets this ImageView from the given URL.
   *
   * If the URL is empty, ImageView will not display anything.
   *
   * @SINCE_1_1.10
   * @param[in] url The URL to the image resource to display.
   * @param [in] size The width and height to fit the loaded image to.
   */
  void SetImage( const std::string& url, ImageDimensions size );

  /**
   * @DEPRECATED_1_1.4
   * @brief Gets the Image instance handle used by the ImageView.
   *
   * A valid handle will be returned only if this instance was created with New(Image) or SetImage(Image) was called.
   *
   * @SINCE_1_0.0
   * @return The Image instance currently used by the ImageView.
   */
  Image GetImage() const;

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @SINCE_1_0.0
   * @param[in]  implementation  The ImageView implementation.
   */
  DALI_INTERNAL ImageView( Internal::ImageView& implementation );

  /**
   * @brief Allows the creation of this ImageView from an Internal::CustomActor pointer.
   *
   * @SINCE_1_0.0
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  DALI_INTERNAL ImageView( Dali::Internal::CustomActor* internal );
  /// @endcond

};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_IMAGE_VIEW_H__

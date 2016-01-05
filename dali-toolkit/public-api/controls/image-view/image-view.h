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
 *
 * @brief ImageView is a class for displaying an Image.
 */
class DALI_IMPORT_API ImageView : public Control
{
public:
  /**
   * @brief The start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000              ///< Reserve property indices
  };

  /**
   * @brief An enumeration of properties belonging to the ImageView class.
   */
  struct Property
  {
    enum
    {
      RESOURCE_URL = PROPERTY_START_INDEX, ///< name "resourceUrl",  @deprecated DALi 1.1.16 Use IMAGE instead.  type string
      IMAGE,                               ///< name "image",        @see SetImage(),                            type string if it is a url, map otherwise
    };
  };

public:

  /**
   * @brief Create an uninitialized ImageView.
   */
  ImageView();

  /**
   * @brief Create an initialized ImageView.
   *
   * @return A handle to a newly allocated Dali ImageView.
   *
   * @note ImageView will not display anything.
   */
  static ImageView New();

  /**
   * @brief Create an initialized ImageView from an Image.
   *
   * If the handle is empty, ImageView will not display anything.
   *
   * @param[in] image The Image to display.
   * @return A handle to a newly allocated ImageView.
   */
  static ImageView New( Image image );

  /**
   * @brief Create an initialized ImageView from an Image resource URL
   *
   * If the string is empty, ImageView will not display anything.
   *
   * @param[in] url The url of the image resource to display.
   * @return A handle to a newly allocated ImageView.
   */
  static ImageView New( const std::string& url );

  /**
   * @brief Create an initialized ImageView from an Image resource URL
   *
   * If the string is empty, ImageView will not display anything.
   *
   * @since DALi 1.1.10
   *
   * @param[in] url The url of the image resource to display.
   * @param [in] size The width and height to fit the loaded image to.
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
   */
  ~ImageView();

  /**
   * @brief Copy constructor.
   *
   * @param[in] imageView ImageView to copy. The copied ImageView will point at the same implementation
   */
  ImageView( const ImageView& imageView );

  /**
   * @brief Assignment operator.
   *
   * @param[in] imageView The ImageView to assign from.
   * @return The updated ImageView.
   */
  ImageView& operator=( const ImageView& imageView );

  /**
   * @brief Downcast an Object handle to ImageView.
   *
   * If handle points to a ImageView the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return handle to a ImageView or an uninitialized handle
   */
  static ImageView DownCast( BaseHandle handle );

  /**
   * @brief Sets this ImageView from an Image
   *
   * If the handle is empty, ImageView will display nothing
   * @param[in] image The Image to display.
   */
  void SetImage( Image image );

  /**
   * @brief Sets this ImageView from an Image URL
   *
   * If the URL is empty, ImageView will not display anything.
   *
   * @since DALi 1.1.4
   *
   * @param[in] url The Image resource to display.
   */
  void SetImage( const std::string& url );

  /**
   * @brief Sets this ImageView from an Image URL
   *
   * If the URL is empty, ImageView will not display anything.
   *
   * @since DALi 1.1.10
   *
   * @param[in] url A URL to the image resource to display.
   * @param [in] size The width and height to fit the loaded image to.
   */
  void SetImage( const std::string& url, ImageDimensions size );

  /**
   * @deprecated Gets the Image
   *
   * @return The Image currently set to this ImageView
   */
  Image GetImage() const;

public: // Not intended for application developers

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in]  implementation  The ImageView implementation.
   */
  DALI_INTERNAL ImageView( Internal::ImageView& implementation );

  /**
   * @brief Allows the creation of this ImageView from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  DALI_INTERNAL ImageView( Dali::Internal::CustomActor* internal );

};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_IMAGE_VIEW_H__

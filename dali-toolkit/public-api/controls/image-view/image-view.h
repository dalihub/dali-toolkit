#ifndef DALI_TOOLKIT_IMAGE_VIEW_H
#define DALI_TOOLKIT_IMAGE_VIEW_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
 * Some resources can be loaded before the ImageView is staged ( already cached ), in these cases if the connection to
 * ResouceReadySignal is done after the resource is set then signal will be missed.
 *
 * To protect against this, IsResourceReady() can be checked before connecting to ResourceReadySignal,
 * or the signal connection can be done before setting the resource.
 *
 * @code
 *    auto myImageView = ImageView::New( resourceUrl );
 *    if ( myImageView.IsResourceReady() )
 *    {
 *       // do something
 *    }
 *    else
 *    {
 *      myImageView.ResourceReadySignal.Connect( .... )
 *    }
 * @endcode
 *
 * OR Connect to signal before setting resource
 *
 * @code
 *    auto myImageView = ImageView::New();
 *    myImageView.ResourceReadySignal.Connect( .... )
 *    myImageView.SetProperty( ImageView::Property::IMAGE, resourceUrl );
 * @endcode
 *
 * @SINCE_1_0.0
 *
 */
class DALI_TOOLKIT_API ImageView : public Control
{
public:
  /**
   * @brief Enumeration for the start and end property ranges for this control.
   * @SINCE_1_0.0
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1, ///< @SINCE_1_0.0
    PROPERTY_END_INDEX   = PROPERTY_START_INDEX + 1000,             ///< Reserve property indices @SINCE_1_0.0

    ANIMATABLE_PROPERTY_START_INDEX = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX,       ///< @SINCE_1_1.18
    ANIMATABLE_PROPERTY_END_INDEX   = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 1000 ///< Reserve animatable property indices, @SINCE_1_1.18
  };

  /**
   * @brief Enumeration for the instance of properties belonging to the ImageView class.
   * @SINCE_1_0.0
   */
  struct Property
  {
    /**
     * @brief Enumeration for the instance of properties belonging to the ImageView class.
     * @SINCE_1_0.0
     */
    enum
    {
      // Event side properties

      /**
       * @brief name "image", type string if it is a url, map otherwise.
       * @SINCE_1_0.0
       */
      IMAGE = PROPERTY_START_INDEX,

      /**
       * @brief name "preMultipliedAlpha", type Boolean.
       * @SINCE_1_1.18
       * @pre image must be initialized.
       */
      PRE_MULTIPLIED_ALPHA,

      /**
       * @brief name "placeholderImage", type string.
       *
       * placeholder image is shown when image view is waiting for the image to load.
       * @SINCE_2_2.24
       */
      PLACEHOLDER_IMAGE,

      /**
       * @brief name "enableTransitionEffect", type Boolean
       *
       * This effect is a crossfade effect when the image is replaced.
       * the default duration of the crossfade effect is 0.3 seconds.
       * if the placeholder is enabled, the cross effect applies when the image is changed from a placeholder image to a new image.
       * if not, the cross effect applies when a new image is shown or is changed from the previous image to a new image.
       * @SINCE_2_2.24
       */
      ENABLE_TRANSITION_EFFECT,

      /**
       * @brief name "transitionEffectOption", type Map
       *
       * This option is for transition effect when the image is replaced.
       * if not, the cross effect use default option.
       * @SINCE_2_2.24
       */
      TRANSITION_EFFECT_OPTION,

      // Animatable properties

      /**
       * @brief name "pixelArea", type Vector4.
       * @details Pixel area is a relative value with the whole image area as [0.0, 0.0, 1.0, 1.0].
       * @SINCE_1_1.18
       */
      PIXEL_AREA = ANIMATABLE_PROPERTY_START_INDEX,
    };
  };

public:
  /**
   * @brief Creates an uninitialized ImageView.
   * @SINCE_1_0.0
   */
  ImageView();

  /**
   * @brief Create an initialized ImageView.
   *
   * @SINCE_1_0.0
   * @return A handle to a newly allocated Dali ImageView
   *
   * @note ImageView will not display anything.
   */
  static ImageView New();

  /**
   * @brief Creates an initialized ImageView from an URL to an image resource.
   *
   * If the string is empty, ImageView will not display anything.
   *
   * @SINCE_1_0.0
   * @REMARK_INTERNET
   * @REMARK_STORAGE
   * @param[in] url The url of the image resource to display
   * @return A handle to a newly allocated ImageView
   */
  static ImageView New(const std::string& url);

  /**
   * @brief Creates an initialized ImageView from a URL to an image resource.
   *
   * If the string is empty, ImageView will not display anything.
   *
   * @SINCE_1_1.10
   * @REMARK_INTERNET
   * @REMARK_STORAGE
   * @param[in] url The url of the image resource to display
   * @param [in] size The width and height to which to fit the loaded image
   * @return A handle to a newly allocated ImageView
   * @note A valid size is preferable for efficiency.
   *       However, do not set a size that is bigger than the actual image size, as up-scaling is not available.
   *       The content of the area not covered by the actual image is undefined and will not be cleared.
   */
  static ImageView New(const std::string& url, ImageDimensions size);

  /**
   * @brief Create an initialized ImageView with additional behaviour.
   *
   * @SINCE_2_1.8
   * @param[in] additionalBehaviour Additional control behaviour
   * @return A handle to a newly allocated Dali ImageView
   *
   * @note ImageView will not display anything.
   */
  static ImageView New(ControlBehaviour additionalBehaviour);

  /**
   * @brief Creates an initialized ImageView from an URL to an image resource with additional behaviour.
   *
   * If the string is empty, ImageView will not display anything.
   *
   * @SINCE_2_1.8
   * @REMARK_INTERNET
   * @REMARK_STORAGE
   * @param[in] additionalBehaviour Additional control behaviour
   * @param[in] url The url of the image resource to display
   * @return A handle to a newly allocated ImageView
   */
  static ImageView New(ControlBehaviour additionalBehaviour, const std::string& url);

  /**
   * @brief Creates an initialized ImageView from a URL to an image resource with additional behaviour.
   *
   * If the string is empty, ImageView will not display anything.
   *
   * @SINCE_2_1.8
   * @REMARK_INTERNET
   * @REMARK_STORAGE
   * @param[in] additionalBehaviour Additional control behaviour
   * @param[in] url The url of the image resource to display
   * @param[in] size The width and height to which to fit the loaded image
   * @return A handle to a newly allocated ImageView
   * @note A valid size is preferable for efficiency.
   *       However, do not set a size that is bigger than the actual image size, as up-scaling is not available.
   *       The content of the area not covered by the actual image is undefined and will not be cleared.
   */
  static ImageView New(ControlBehaviour additionalBehaviour, const std::string& url, ImageDimensions size);

  /**
   * @brief Destructor.
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
  ImageView(const ImageView& imageView);

  /**
   * @brief Move constructor
   * @SINCE_1_9.23
   *
   * @param[in] rhs A reference to the moved handle
   */
  ImageView(ImageView&& rhs) noexcept;

  /**
   * @brief Assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] imageView The ImageView to assign from
   * @return The updated ImageView
   */
  ImageView& operator=(const ImageView& imageView);

  /**
   * @brief Move assignment
   * @SINCE_1_9.23
   *
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  ImageView& operator=(ImageView&& rhs) noexcept;

  /**
   * @brief Downcasts a handle to ImageView handle.
   *
   * If handle points to a ImageView, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return Handle to a ImageView or an uninitialized handle
   */
  static ImageView DownCast(BaseHandle handle);

  /**
   * @brief Sets this ImageView from the given URL.
   *
   * If the URL is empty, ImageView will not display anything.
   *
   * @SINCE_1_1.4
   * @REMARK_INTERNET
   * @REMARK_STORAGE
   * @param[in] url The URL to the image resource to display
   */
  void SetImage(const std::string& url);

  /**
   * @brief Sets this ImageView from the given URL.
   *
   * If the URL is empty, ImageView will not display anything.
   *
   * @SINCE_1_1.10
   * @REMARK_INTERNET
   * @REMARK_STORAGE
   * @param[in] url The URL to the image resource to display
   * @param [in] size The width and height to fit the loaded image to
   */
  void SetImage(const std::string& url, ImageDimensions size);

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @SINCE_1_0.0
   * @param[in] implementation The ImageView implementation
   */
  DALI_INTERNAL ImageView(Internal::ImageView& implementation);

  /**
   * @brief Allows the creation of this ImageView from an Internal::CustomActor pointer.
   *
   * @SINCE_1_0.0
   * @param[in] internal A pointer to the internal CustomActor
   */
  DALI_INTERNAL ImageView(Dali::Internal::CustomActor* internal);
  /// @endcond
};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_IMAGE_VIEW_H

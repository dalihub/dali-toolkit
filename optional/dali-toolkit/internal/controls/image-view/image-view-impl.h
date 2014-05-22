#ifndef __DALI_TOOLKIT_INTERNAL_ImageView_H__
#define __DALI_TOOLKIT_INTERNAL_ImageView_H__

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
#include <boost/bind.hpp>

// INTERNAL INCLUDES
#include <dali/dali.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class ImageView;

typedef IntrusivePtr<ImageView>    ImageViewPtr;

/**
 * @copydoc Toolkit::ImageView
 */
class ImageView : public ControlImpl
{
public:

  typedef Toolkit::ImageView::ImageType ImageType;

  /**
   * ImageRequest element
   * represents an image to be loaded and displayed
   * with given attributes.
   */
  struct ImageRequest
  {
    /**
     * Default constructor
     */
    ImageRequest()
    {
    }

    /**
     * @param[in] filename to load
     * @param[in] width Width of image.
     * @param[in] height Height of image.
     */
    ImageRequest( const std::string& filename, unsigned int width, unsigned int height )
    : mFilename( filename )
    {
      mAttributes.SetSize( width, height );
    }

    std::string mFilename;                  ///< filename of image
    ImageAttributes mAttributes;            ///< attributes of image
  };

public:

  /**
   * Create a new ImageView.
   * @return A public handle to the newly allocated ImageView.
   */
  static Dali::Toolkit::ImageView New();

public:

  /**
   * @copydoc Toolkit::ImageView::SetImage(const std::string& filename, ImageType type, float min, float max)
   */
  void SetImage(const std::string& filename, ImageType type, float min, float max);

  /**
   * @copydoc Toolkit::ImageView::SetImage(Image& image);
   */
  void SetImage(Image image);

  /**
   * Adds an image to displayed at a detail range.
   *
   * @note If two or more images are specified to be displayed at
   * the same overlapping range. Then the last image that was added
   * will be displayed.
   *
   * @param[in] req The image to load and display
   * @param[in] condition The detail condition to be satisified for the image to display
   */
  void AddImage(ImageRequest& req, PropertyCondition condition);

  /**
   * @copydoc Toolkit::ImageView::SetCameraActor
   */
  void SetCameraActor(CameraActor camera, float detailFactor);

  /**
   * @copydoc Toolkit::ImageView::SetDetail
   */
  void SetDetail(float detail);

protected:

  /**
   * Construct a new ImageView.
   */
  ImageView();

  /**
   * 2nd-phase initialization.
   */
  void Initialize();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ImageView();

private:

  /**
   * Sets a Bitmap Image as the image to display for this ImageView
   * min and max represent the minimum and maximum sizes to load.
   * sizes will be created at 2^n scale factor. where n goes from
   * ceil(log2(min)) to ceil(log2(max))
   *
   * @param[in] filename the image path to load
   * @param[in] min the minimum size to load
   * @param[in] max the maximum size to load
   */
  void SetImageBitmap(const std::string& filename, float min, float max);

  /**
   * Sets a Distance Field Image as the image to display for this ImageView
   *
   * @param[in] filename the image path to load
   */
  void SetImageDistanceField(const std::string& filename);

  /**
   * Invoked whenever the detail property passes a notification point.
   * @param[in] notification The notification instance.
   */
  virtual void OnDetailChange(PropertyNotification& notification );

private:

  // Undefined
  ImageView(const ImageView&);

  // Undefined
  ImageView& operator=(const ImageView& rhs);

private:

  Property::Index mPropertyDetail;                              ///< Detail property, changing this affects the level of detail of the content.
  ImageActor mImageActor;                                       ///< Holding image actor for the various images at differing levels of detail.
  std::map<PropertyNotification, ImageRequest> mNotifications;  ///< Property Notification -> Image map table.

  PropertyNotification mPropertyNotification;   ///< Property notification
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::ImageView& GetImpl(Toolkit::ImageView& pub)
{
  DALI_ASSERT_ALWAYS(pub);

  Dali::RefObject& handle = pub.GetImplementation();

  return static_cast<Toolkit::Internal::ImageView&>(handle);
}

inline const Toolkit::Internal::ImageView& GetImpl(const Toolkit::ImageView& pub)
{
  DALI_ASSERT_ALWAYS(pub);

  const Dali::RefObject& handle = pub.GetImplementation();

  return static_cast<const Toolkit::Internal::ImageView&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_ImageView_H__

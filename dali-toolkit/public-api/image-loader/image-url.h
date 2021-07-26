#ifndef DALI_TOOLKIT_IMAGE_URL_H
#define DALI_TOOLKIT_IMAGE_URL_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
 */

// EXTERNAL INCLUDES
#include <string>
#include <dali/public-api/rendering/texture.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal DALI_INTERNAL
{
class ImageUrl;
}
/**
 * @brief ImageUrl can be used to wrap an external buffer.
 *
 * An instance of ImageUrl can be created from Image::GenerateUrl().
 * Application can get url from ImageUrl.
 * When application does not use this anymore, the destructor of the ImageUrl is called.
 * At this time, the buffer is deleted from the texture manager.
 */
class DALI_TOOLKIT_API ImageUrl : public BaseHandle
{
public:
  /**
   * Default Constructor.
   * Resulting URL is not valid
   */
  ImageUrl();

  /**
   * Destructor
   */
  ~ImageUrl();

  /**
   * @brief Create an initialized ImageUrl.
   *
   * @param[in] texture The texture url is got from external buffer.
   * @return A handle to a newly allocated Dali resource.
   */
  static ImageUrl New(Texture& texture);

  /**
   * @brief Downcast an Object handle to ImageUrl handle.
   *
   * If handle points to a ImageUrl object the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle to An object.
   * @return handle to a ImageUrl object or an uninitialized handle.
   */
  static ImageUrl DownCast(BaseHandle handle);

  /**
   * Copy constructor
   * @param[in] url The url to copy
   */
  ImageUrl(const ImageUrl& url);

  /**
   * Assignment operator
   * @param[in] url The url to copy
   */
  ImageUrl& operator=(const ImageUrl& url);

  /**
   * @brief Move constructor.
   * @param[in] rhs A reference to the moved handle
   */
  ImageUrl(ImageUrl&& rhs);

  /**
   * @brief Move assignment operator.
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this handle
   */
  ImageUrl& operator=(ImageUrl&& rhs);

  /**
   * Get the url
   * @return Returns url's string
   */
  const std::string& GetUrl() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief This constructor is used by New() methods.
   *
   * @param[in] internal A pointer to a newly allocated Dali resource.
   */
  explicit DALI_INTERNAL ImageUrl(Toolkit::Internal::ImageUrl* internal);
  /// @endcond
};

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_IMAGE_URL_H */

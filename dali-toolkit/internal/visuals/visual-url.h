#ifndef DALI_TOOLKIT_INTERNAL_VISUAL_URL_H
#define DALI_TOOLKIT_INTERNAL_VISUAL_URL_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#include <string>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class VisualUrl
{
public:

  /**
   * The type of the URL based on the string contents
   */
  enum Type
  {
    REGULAR_IMAGE,
    N_PATCH,
    SVG,
    GIF
  };

  enum Location
  {
    LOCAL,
    REMOTE
  };

  /**
   * Default Constructor.
   * Resulting URL is not valid
   */
  VisualUrl();

  /**
   * Constructor.
   * Determines type of visual and whether the url is local or remote
   * @param[in] url The URL to store and resolve
   */
  VisualUrl( const std::string& url );

  /**
   * Copy constructor
   * @param[in] url The VisualUrl to copy
   */
  VisualUrl( const VisualUrl& url );

  /**
   * Assignment operator
   * @param[in] url The VisualUrl to copy
   */
  VisualUrl& operator=( const VisualUrl& url );

  /**
   * Get the full URL
   * @return The url
   */
  const std::string& GetUrl() const;

  /**
   * Get the visual type of the URL
   * @return The visual type of the URL
   */
  Type GetType() const;

  /**
   * Is the URL is local to the device, or remote?
   * @return the location of the resource
   */
  Location GetLocation() const;

  /**
   * Is the URL valid?
   * @return true if the URL has length
   */
  bool IsValid() const;

  /**
   * @return true if the location is LOCAL
   */
  bool IsLocal() const;

private:
  std::string mUrl;
  Type mType;
  Location mLocation;
};


} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_INTERNAL_VISUAL_URL_H */

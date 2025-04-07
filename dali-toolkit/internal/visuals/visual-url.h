#ifndef DALI_TOOLKIT_INTERNAL_VISUAL_URL_H
#define DALI_TOOLKIT_INTERNAL_VISUAL_URL_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <cstdint>
#include <string>
#include <string_view>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class TextureManager;

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
    GIF,
    WEBP,
    JSON,
    TVG
  };

  enum ProtocolType
  {
    LOCAL,   ///< file in local file system
    TEXTURE, ///< texture uploaded to texture manager
    REMOTE,  ///< remote image
    BUFFER   ///< encoded image buffer
  };

  /**
   * Default Constructor.
   * Resulting URL is not valid
   */
  VisualUrl();

  /**
   * Default Destructor.
   * Delete an external texture if if protocolType is TEXTURE.
   */
  ~VisualUrl();

  /**
   * Constructor.
   * Determines type of visual and whether the url is local or remote
   * Notify that it is using an external texture if if protocolType is TEXTURE.
   *
   * @param[in] url The URL to store and resolve
   */
  VisualUrl(const std::string& url);

  /**
   * Copy constructor
   * @param[in] url The VisualUrl to copy
   */
  VisualUrl(const VisualUrl& url);

  /**
   * Assignment operator
   * @param[in] url The VisualUrl to copy
   */
  VisualUrl& operator=(const VisualUrl& url);

  /**
   * Move constructor
   * @param[in] url The VisualUrl to move
   */
  VisualUrl(VisualUrl&& url) noexcept;

  /**
   * Move assignment operator
   * @param[in] url The VisualUrl to move
   */
  VisualUrl& operator=(VisualUrl&& url) noexcept;

  /**
   * Get the full URL
   * @return The url
   */
  const std::string& GetUrl() const;

  /**
   * Get the ellipsed URL.
   * @return The ellipsed url
   */
  std::string GetEllipsedUrl() const;

  /**
   * Get the hash value of full URL
   * @return The hash value of url
   */
  std::size_t GetUrlHash() const;

  /**
   * Get the visual type of the URL
   * @return The visual type of the URL
   */
  Type GetType() const;

  /**
   * Is the URL is local to the device, or remote?
   * @return the location of the resource
   */
  ProtocolType GetProtocolType() const;

  /**
   * Is the URL valid?
   * @return true if the URL has length
   */
  bool IsValid() const;

  /**
   * @return true if the location is LOCAL, i.e. is loadable from local file system
   */
  bool IsLocalResource() const;

  /**
   * @return true if the location is BUFFER, i.e. may contain EncodedImageBuffer
   */
  bool IsBufferResource() const;

  /**
   * @return the location part of the url
   */
  std::string GetLocation() const;

  /**
   * @return the location part of the url without extension
   */
  std::string GetLocationWithoutExtension() const;

  /**
   * @brief Convert the location part of the url as single integer
   *
   * @param[out] integerLocation The result of conversion.
   * @return True if the location can be converted to integer. Otherwise false.
   */
  bool GetLocationAsInteger(int32_t& integerLocation) const;

  /**
   * @brief Increase the external resource reference count.
   * Only have effort if ProtocolType is TEXTURE or BUFFER.
   * 
   * @post DecreaseExternalResourceReference Should be called when the resource is no longer needed.
   *
   * @param[in] textureManager The texture manager who owns the external texture.
   */
  void IncreaseExternalResourceReference(TextureManager& textureManager) const;

  /**
   * @brief Decrease the external resource reference count.
   * Only have effort if ProtocolType is TEXTURE or BUFFER.
   *
   * @pre IncreaseExternalResourceReference Should be called before calling this function.
   *
   * @param[in] textureManager The texture manager who owns the external texture.
   */
  void DecreaseExternalResourceReference(TextureManager& textureManager) const;

  /**
   * Helper to create a URL of type TEXTURE
   * @param location the location of the texture
   * @return the Url
   */
  static std::string CreateTextureUrl(const std::string& location);

  /**
   * Helper to create a URL of type BUFFER
   * @param[in] location the location of the texture
   * @param[in] extension the extension of url
   * @return the Url
   */
  static std::string CreateBufferUrl(const std::string& location, const std::string_view& extension);

  /**
   * Helper to get a ProtocolType from url
   * @param url the url of the texture
   * @return the protocol type
   */
  static VisualUrl::ProtocolType GetProtocolType(const std::string& url);

  /**
   * Helper to get a location from url
   * @param url the location of the texture
   * @return the location
   */
  static std::string GetLocation(const std::string& url);

  /**
   * Helper to get a location from url without extension
   * @param[in] url the location of the texture
   * @return the location without extension
   */
  static std::string GetLocationWithoutExtension(const std::string& url);

private:
  std::string  mUrl;
  Type         mType;
  ProtocolType mLocation;

  mutable std::size_t mUrlHash;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_INTERNAL_VISUAL_URL_H */

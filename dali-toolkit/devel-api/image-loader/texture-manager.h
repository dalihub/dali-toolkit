#ifndef DALI_TOOLKIT_DEVEL_API_TEXTURE_MANAGER_H
#define DALI_TOOLKIT_DEVEL_API_TEXTURE_MANAGER_H

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
#include <dali/public-api/rendering/texture-set.h>
#include <dali/public-api/adaptor-framework/encoded-image-buffer.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>

namespace Dali
{
namespace Toolkit
{
/**
 * API to interface with the toolkit texture manager
 * Allows developers to add Textures through TextureSets to toolkit so that visuals can use them to render
 */
namespace TextureManager
{
/**
 * @brief Add a Texture to texture manager
 * Toolkit keeps the Texture handle until RemoveTexture is called.
 * @note this method does not check for duplicates,
 *       if same Texture is added multiple times, a different URL is returned each time
 * @param[in] texture the Texture to add
 * @return the Url string representing this texture
 */
DALI_TOOLKIT_API std::string AddTexture(Texture& texture);

/**
 * @brief Add a TextureSet to texture manager
 * Toolkit keeps the TextureSet handle until RemoveTexture is called.
 * @note this method does not check for duplicates,
 *       if same TextureSet is added multiple times, a different URL is returned each time
 * @param[in] textureSet the TextureSet to add
 * @return the Url string representing this texture
 */
DALI_TOOLKIT_API std::string AddTexture(TextureSet& textureSet);

/**
 * @brief Add a EncodedImageBuffer to texture manager
 * Toolkit keeps the EncodedImageBuffer handle until RemoveTexture is called.
 * @note this method check for duplicates reference,
 *       if same EncodedImageBuffer is added multiple times, a same URL is returned
 * @param[in] encodedImageBuffer the image buffer to add
 * @return the Url string representing this texture
 */
DALI_TOOLKIT_API std::string AddTexture(const Dali::EncodedImageBuffer& encodedImageBuffer);

/**
 * @brief Removes a TextureSet from toolkit
 * @note TextureSet may still be used by visuals and kept alive by them
 * @param[in] textureUrl to remove
 * @return the handle to the TextureSet or empty handle in case TextureSet is not found
 */
DALI_TOOLKIT_API TextureSet RemoveTexture(const std::string& textureUrl);

} // namespace TextureManager

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_DEVEL_API_TEXTURE_MANAGER_H

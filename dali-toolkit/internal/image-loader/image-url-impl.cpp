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
 *
 */

// CLASS HEADER
#include <dali-toolkit/devel-api/image-loader/texture-manager.h>
#include <dali-toolkit/internal/image-loader/image-url-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
ImageUrl::ImageUrl(Texture& texture)
: mUrl("")
{
  mUrl = Dali::Toolkit::TextureManager::AddTexture(texture);
}

ImageUrl::~ImageUrl()
{
  Dali::Toolkit::TextureManager::RemoveTexture(mUrl);
}

ImageUrlPtr ImageUrl::New(Texture& texture)
{
  ImageUrlPtr imageUrlPtr = new ImageUrl(texture);
  return imageUrlPtr;
}

const std::string& ImageUrl::GetUrl() const
{
  return mUrl;
}

} // End of namespace Internal

} // End of namespace Toolkit

} // End of namespace Dali

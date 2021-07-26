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

// CLASS HEADER
#include <dali-toolkit/public-api/image-loader/image-url.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/image-loader/image-url-impl.h>

namespace Dali
{
namespace Toolkit
{
ImageUrl::ImageUrl()
{
}

ImageUrl::~ImageUrl()
{
}

ImageUrl ImageUrl::New(Texture& texture)
{
  Toolkit::Internal::ImageUrlPtr internal = Toolkit::Internal::ImageUrl::New(texture);
  return ImageUrl(internal.Get());
}

ImageUrl ImageUrl::New(const EncodedImageBuffer& encodedImageBuffer)
{
  Toolkit::Internal::ImageUrlPtr internal = Toolkit::Internal::ImageUrl::New(encodedImageBuffer);
  return ImageUrl(internal.Get());
}

ImageUrl ImageUrl::DownCast(BaseHandle handle)
{
  return ImageUrl(dynamic_cast<Toolkit::Internal::ImageUrl*>(handle.GetObjectPtr()));
}

ImageUrl::ImageUrl(const ImageUrl& rhs) = default;

ImageUrl& ImageUrl::operator=(const ImageUrl& url) = default;

ImageUrl::ImageUrl(ImageUrl&& rhs) = default;

ImageUrl& ImageUrl::operator=(ImageUrl&& rhs) = default;

const std::string& ImageUrl::GetUrl() const
{
  return GetImpl(*this).GetUrl();
}

ImageUrl::ImageUrl(Toolkit::Internal::ImageUrl* internal)
: BaseHandle(internal)
{
}

} // namespace Toolkit

} // namespace Dali

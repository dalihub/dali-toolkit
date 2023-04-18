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
 */

// CLASS HEADER
#include <dali-toolkit/public-api/image-loader/image.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/image-loader/texture-manager.h>
#include <dali-toolkit/public-api/image-loader/image-url.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/rendering/frame-buffer-devel.h>

namespace Dali
{
namespace Toolkit
{
namespace Image
{
Dali::Toolkit::ImageUrl GenerateUrl(Dali::FrameBuffer frameBuffer, Pixel::Format pixelFormat, uint32_t width, uint32_t height)
{
  Texture texture = Texture::New(Dali::TextureType::TEXTURE_2D, pixelFormat, width, height);
  frameBuffer.AttachColorTexture(texture, 0u, 0u);
  // TODO : Need to check frameBuffer result use preMultiplied color or not. By default, we use premultiplied
  Dali::Toolkit::ImageUrl imageUrl = Dali::Toolkit::ImageUrl::New(texture, true);
  return imageUrl;
}

Dali::Toolkit::ImageUrl GenerateUrl(const Dali::FrameBuffer frameBuffer, uint8_t index)
{
  Texture texture = Dali::DevelFrameBuffer::GetColorTexture(frameBuffer, index);
  // TODO : Need to check frameBuffer result use preMultiplied color or not. By default, we use premultiplied
  Dali::Toolkit::ImageUrl imageUrl = Dali::Toolkit::ImageUrl::New(texture, true);
  return imageUrl;
}

Dali::Toolkit::ImageUrl GenerateUrl(const Dali::PixelData pixelData, bool preMultiplied)
{
  Texture texture = Texture::New(TextureType::TEXTURE_2D, pixelData.GetPixelFormat(), pixelData.GetWidth(), pixelData.GetHeight());
  texture.Upload(pixelData);
  Dali::Toolkit::ImageUrl imageUrl = Dali::Toolkit::ImageUrl::New(texture, preMultiplied);
  return imageUrl;
}

Dali::Toolkit::ImageUrl GenerateUrl(const Dali::NativeImageSourcePtr nativeImageSource, bool preMultiplied)
{
  Texture texture = Dali::Texture::New(*nativeImageSource);
  Dali::Toolkit::ImageUrl imageUrl = Dali::Toolkit::ImageUrl::New(texture, preMultiplied);
  return imageUrl;
}

Dali::Toolkit::ImageUrl GenerateUrl(const Dali::EncodedImageBuffer encodedImageBuffer)
{
  return Dali::Toolkit::ImageUrl::New(encodedImageBuffer);
}

} // namespace Image

} // namespace Toolkit

} // namespace Dali

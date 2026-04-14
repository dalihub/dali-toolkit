/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/image-loader/image-url-utils.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/image-loader/texture-manager.h>
#include <dali-toolkit/public-api/image-loader/image-url.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/rendering/frame-buffer-devel.h>
#include <dali/integration-api/string-utils.h>
#include <dali/integration-api/texture-integ.h>

using Dali::Integration::ToDaliString;

namespace Dali
{
namespace Toolkit
{
namespace ImageUrlUtils
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
  Texture                 texture  = Texture::New(TextureType::TEXTURE_2D, pixelData.GetPixelFormat(), pixelData.GetWidth(), pixelData.GetHeight());
  Dali::Toolkit::ImageUrl imageUrl = Dali::Toolkit::ImageUrl::New(texture, preMultiplied);
#if defined(GPU_MEMORY_PROFILE_ENABLED)
  Dali::Integration::TextureUploadWithContent(texture, pixelData, imageUrl.GetUrl(), Dali::Integration::TextureContextTypeHint::EXTERNAL_IMAGE);
#else
  texture.Upload(pixelData);
#endif
  return imageUrl;
}

Dali::Toolkit::ImageUrl GenerateUrl(const Dali::NativeImageInterfacePtr nativeImageInterface, bool preMultiplied)
{
  Texture                 texture  = Dali::Texture::New(*nativeImageInterface);
  Dali::Toolkit::ImageUrl imageUrl = Dali::Toolkit::ImageUrl::New(texture, preMultiplied);
  return imageUrl;
}

Dali::Toolkit::ImageUrl GenerateUrl(const Dali::EncodedImageBuffer encodedImageBuffer)
{
  return Dali::Toolkit::ImageUrl::New(encodedImageBuffer);
}

Dali::Toolkit::ImageUrl GenerateDepthUrl(const Dali::FrameBuffer frameBuffer)
{
  Texture                 texture  = Dali::DevelFrameBuffer::GetDepthTexture(frameBuffer);
  Dali::Toolkit::ImageUrl imageUrl = Dali::Toolkit::ImageUrl::New(texture, false);
  return imageUrl;
}

} //namespace ImageUrlUtils

} // namespace Toolkit

} // namespace Dali

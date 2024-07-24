/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/image-loader/image-url-impl.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/image-loader/texture-manager.h>
#include <dali-toolkit/internal/texture-manager/texture-manager-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>
#include <dali-toolkit/internal/visuals/visual-url.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
ImageUrl::ImageUrl(Texture& texture, bool preMultiplied)
{
  mUrl = Dali::Toolkit::TextureManager::AddTexture(texture, preMultiplied);
}

ImageUrl::ImageUrl(const EncodedImageBuffer& encodedImageBuffer)
: mUrl("")
{
  auto visualFactory = Dali::Toolkit::VisualFactory::Get();
  if(visualFactory)
  {
    auto& textureManager = GetImplementation(visualFactory).GetTextureManager();
    mUrl                 = textureManager.AddEncodedImageBuffer(encodedImageBuffer);
  }
}

ImageUrl::~ImageUrl()
{
  if(mUrl.size() > 0)
  {
    auto visualFactory = Dali::Toolkit::VisualFactory::Get();
    if(visualFactory)
    {
      auto& textureManager = GetImplementation(visualFactory).GetTextureManager();
      if(VisualUrl::TEXTURE == VisualUrl::GetProtocolType(mUrl))
      {
        textureManager.RemoveExternalTextureByUrl(mUrl);
      }
      else if(VisualUrl::BUFFER == VisualUrl::GetProtocolType(mUrl))
      {
        textureManager.RemoveEncodedImageBufferByUrl(mUrl);
      }
    }
  }
}

ImageUrlPtr ImageUrl::New(Texture& texture, bool preMultiplied)
{
  ImageUrlPtr imageUrlPtr = new ImageUrl(texture, preMultiplied);
  return imageUrlPtr;
}

ImageUrlPtr ImageUrl::New(const EncodedImageBuffer& encodedImageBuffer)
{
  ImageUrlPtr imageUrlPtr = new ImageUrl(encodedImageBuffer);
  return imageUrlPtr;
}

const std::string& ImageUrl::GetUrl() const
{
  return mUrl;
}

} // End of namespace Internal

} // End of namespace Toolkit

} // End of namespace Dali

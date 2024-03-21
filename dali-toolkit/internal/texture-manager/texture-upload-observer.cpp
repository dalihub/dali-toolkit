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
 *
 */

// CLASS HEADER
#include <dali-toolkit/internal/texture-manager/texture-upload-observer.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/texture-manager/texture-manager-type.h> // for INVALUD_TEXTURE_ID

namespace Dali
{
namespace Toolkit
{
TextureUploadObserver::TextureInformation::TextureInformation(const ReturnType returnType, const int32_t textureId, TextureSet textureSet, const bool preMultiplied)
: returnType(returnType),
  textureId(textureId),
  textureSet(textureSet),
  preMultiplied(preMultiplied),
  pixelBuffer(),
  url()
{
}

TextureUploadObserver::TextureInformation::TextureInformation(const ReturnType returnType, const int32_t textureId, TextureSet textureSet, const std::string& url, const bool preMultiplied)
: returnType(returnType),
  textureId(textureId),
  textureSet(textureSet),
  preMultiplied(preMultiplied),
  pixelBuffer(),
  url(url)
{
}

TextureUploadObserver::TextureInformation::TextureInformation(const ReturnType returnType, Devel::PixelBuffer pixelBuffer, const std::string& url, const bool preMultiplied)
: returnType(returnType),
  textureId(Internal::TextureManagerType::INVALID_TEXTURE_ID),
  textureSet(),
  preMultiplied(preMultiplied),
  pixelBuffer(pixelBuffer),
  url(url)
{
}

TextureUploadObserver::TextureInformation::TextureInformation(const ReturnType returnType, const int32_t textureId, TextureSet textureSet, const uint32_t frameCount, const uint32_t interval, const bool preMultiplied)
: returnType(returnType),
  textureId(textureId),
  textureSet(textureSet),
  preMultiplied(preMultiplied),
  pixelBuffer(),
  url(),
  frameCount(frameCount),
  interval(interval)
{
}

TextureUploadObserver::TextureUploadObserver()
{
}

TextureUploadObserver::~TextureUploadObserver()
{
  if(!mDestructionSignal.Empty())
  {
    mDestructionSignal.Emit(this);
  }
}

TextureUploadObserver::DestructionSignalType& TextureUploadObserver::DestructionSignal()
{
  return mDestructionSignal;
}

} // namespace Toolkit

} // namespace Dali

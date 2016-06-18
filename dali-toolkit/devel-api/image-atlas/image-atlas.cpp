 /*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include "image-atlas.h"

// INTERNAL INCLUDES
#include <dali-toolkit/internal/image-atlas/image-atlas-impl.h>

namespace Dali
{

namespace Toolkit
{

ImageAtlas::ImageAtlas()
{
}

ImageAtlas::~ImageAtlas()
{
}

ImageAtlas::ImageAtlas(Internal::ImageAtlas* internal)
: BaseHandle( internal )
{
}

ImageAtlas::ImageAtlas( const ImageAtlas& handle )
: BaseHandle( handle )
{
}

ImageAtlas& ImageAtlas::operator=( const ImageAtlas& handle )
{
  BaseHandle::operator=(handle);
  return *this;
}

ImageAtlas ImageAtlas::New(SizeType width, SizeType height,
                           Pixel::Format pixelFormat)
{
  IntrusivePtr<Internal::ImageAtlas> internal = Internal::ImageAtlas::New( width, height, pixelFormat);
  return ImageAtlas( internal.Get() );
}

Image ImageAtlas::GetAtlas()
{
  return GetImplementation( *this ).GetAtlas();
}

void ImageAtlas::SetBrokenImage( const std::string& brokenImageUrl )
{
  GetImplementation( *this ).SetBrokenImage( brokenImageUrl );
}

bool ImageAtlas::Upload( Vector4& textureRect,
                         const std::string& url,
                         ImageDimensions size,
                         FittingMode::Type fittingMode,
                         bool orientationCorrection )
{
  return GetImplementation(*this).Upload( textureRect, url, size, fittingMode, orientationCorrection );
}

bool ImageAtlas::Upload( Vector4& textureRect, PixelData pixelData )
{
  return GetImplementation(*this).Upload( textureRect, pixelData );
}

void ImageAtlas::Remove(const Vector4& textureRect)
{
  GetImplementation(*this).Remove( textureRect );
}

} // namespace Toolkit

} // namespace Dali

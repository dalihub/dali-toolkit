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
#include <dali/public-api/adaptor-framework/native-image-source.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/any.h>

namespace Dali
{

NativeImageSourcePtr NativeImageSource::New( unsigned int width, unsigned int height, ColorDepth depth )
{
  Any empty;
  NativeImageSourcePtr image = new NativeImageSource( width, height, depth, empty );
  return image;
}

Any NativeImageSource::GetNativeImageSource()
{
  Any source;
  return source;
}

NativeImageSourcePtr NativeImageSource::New( Any nativeImageSource )
{
  NativeImageSourcePtr image = new NativeImageSource(0, 0, COLOR_DEPTH_DEFAULT, nativeImageSource);
  return image;
}

bool NativeImageSource::GetPixels( std::vector<unsigned char> &pixbuf, unsigned int &width, unsigned int &height, Pixel::Format& pixelFormat ) const
{
  return false;
}

bool NativeImageSource::EncodeToFile(const std::string& filename) const
{
  return false;
}

void NativeImageSource::SetSource( Any source )
{
}

bool NativeImageSource::IsColorDepthSupported( ColorDepth colorDepth )
{
  return false;
}

bool NativeImageSource::GlExtensionCreate()
{
  return false;
}

void NativeImageSource::GlExtensionDestroy()
{
}

unsigned int NativeImageSource::TargetTexture()
{
  return 0;
}

void NativeImageSource::PrepareTexture()
{
}

unsigned int NativeImageSource::GetWidth() const
{
  return 0;
}

unsigned int NativeImageSource::GetHeight() const
{
  return 0;
}

bool NativeImageSource::RequiresBlending() const
{
  return false;
}

NativeImageInterface::Extension* NativeImageSource::GetExtension()
{
  return NULL;
}

NativeImageSource::NativeImageSource( unsigned int width, unsigned int height, ColorDepth depth, Any nativeImageSource )
{
}

NativeImageSource::~NativeImageSource()
{
}

} // namespace Dali

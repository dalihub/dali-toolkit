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
#include <dali/public-api/adaptor-framework/native-image.h>

// INTERNAL INCLUDES
#include "toolkit-native-image.h"

// EXTERNAL INCLUDES
#include <dali/public-api/object/any.h>

namespace Dali
{

const char* NativeImageTest::GetCustomFragmentPrefix()
{
  return "#extension GL_OES_EGL_image_external:require\n";
}

NativeImagePtr NativeImage::New(unsigned int width, unsigned int height, ColorDepth depth)
{
  Any            empty;
  NativeImagePtr image = new NativeImage(width, height, depth, empty);
  return image;
}

Any NativeImage::GetNativeImage()
{
  Any source;
  return source;
}

NativeImagePtr NativeImage::New(Any nativeImage)
{
  NativeImagePtr image = new NativeImage(0, 0, COLOR_DEPTH_DEFAULT, nativeImage);
  return image;
}

bool NativeImage::GetPixels(Dali::Vector<unsigned char>& pixbuf, unsigned int& width, unsigned int& height, Pixel::Format& pixelFormat) const
{
  return false;
}

bool NativeImage::EncodeToFile(const std::string& filename) const
{
  return false;
}

void NativeImage::SetSource(Any source)
{
}

bool NativeImage::IsColorDepthSupported(ColorDepth colorDepth)
{
  return false;
}

bool NativeImage::CreateResource()
{
  return false;
}

void NativeImage::DestroyResource()
{
}

unsigned int NativeImage::TargetTexture()
{
  return 0;
}

Dali::NativeImageInterface::PrepareTextureResult NativeImage::PrepareTexture()
{
  return Dali::NativeImageInterface::PrepareTextureResult::NO_ERROR;
}

unsigned int NativeImage::GetWidth() const
{
  return 0;
}

unsigned int NativeImage::GetHeight() const
{
  return 0;
}

bool NativeImage::RequiresBlending() const
{
  return false;
}

int NativeImage::GetTextureTarget() const
{
  return 0;
}

bool NativeImage::ApplyNativeFragmentShader(std::string& shader)
{
  return ApplyNativeFragmentShader(shader, 1);
}

bool NativeImage::ApplyNativeFragmentShader(std::string& shader, int mask)
{
  shader = Dali::NativeImageTest::GetCustomFragmentPrefix() + shader;
  return true;
}

const char* NativeImage::GetCustomSamplerTypename() const
{
  return "";
}

Any NativeImage::GetNativeImageHandle() const
{
  return nullptr;
}

bool NativeImage::SourceChanged() const
{
  return false;
}

NativeImageInterface::Extension* NativeImage::GetExtension()
{
  return NULL;
}

NativeImage::NativeImage(unsigned int width, unsigned int height, ColorDepth depth, Any nativeImage)
{
}

NativeImage::~NativeImage()
{
}

} // namespace Dali

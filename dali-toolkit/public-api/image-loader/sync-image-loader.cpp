/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include "sync-image-loader.h"
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/integration-api/string-utils.h>

namespace Dali
{
namespace Toolkit
{
namespace SyncImageLoader
{
PixelData Load(const Dali::String& url)
{
  return Load(url, ImageDimensions(), SamplingMode::BOX_THEN_LINEAR, true);
}

PixelData Load(const Dali::String& url, ImageDimensions dimensions)
{
  return Load(url, dimensions, SamplingMode::BOX_THEN_LINEAR, true);
}

PixelData Load(const Dali::String& url,
               ImageDimensions     dimensions,
               SamplingMode::Type  samplingMode,
               bool                orientationCorrection)
{
  // Load the image synchronously (block the thread here).
  Devel::PixelBuffer pixelBuffer = Dali::LoadImageFromFile(Dali::Integration::ToStdString(url), dimensions, samplingMode, orientationCorrection);
  if(pixelBuffer)
  {
    return Devel::PixelBuffer::Convert(pixelBuffer);
  }
  return Dali::PixelData(); // return empty handle
}

} // namespace SyncImageLoader

} // namespace Toolkit

} // namespace Dali

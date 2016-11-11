/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/bitmap-loader.h>


namespace Dali
{

namespace Toolkit
{

namespace SyncImageLoader
{


PixelData Load( const std::string& url )
{
  return Load( url, ImageDimensions(), FittingMode::DEFAULT, SamplingMode::BOX_THEN_LINEAR, true );
}

PixelData Load( const std::string& url, ImageDimensions dimensions )
{
  return Load( url, dimensions, FittingMode::DEFAULT, SamplingMode::BOX_THEN_LINEAR, true );
}

PixelData Load( const std::string& url,
                ImageDimensions dimensions,
                FittingMode::Type fittingMode,
                SamplingMode::Type samplingMode,
                bool orientationCorrection )
{
  BitmapLoader loader = BitmapLoader::New( url, dimensions, fittingMode, samplingMode, orientationCorrection );

  // Load the image synchronously (block the thread here).
  loader.Load();

  return loader.GetPixelData();
}


} // namespace SyncImageLoader

} // namespace Toolkit

} // namespace Dali

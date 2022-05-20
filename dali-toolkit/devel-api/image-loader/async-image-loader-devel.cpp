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

#include <dali-toolkit/devel-api/image-loader/async-image-loader-devel.h>
#include <dali-toolkit/internal/image-loader/async-image-loader-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace DevelAsyncImageLoader
{
uint32_t LoadAnimatedImage(AsyncImageLoader                         asyncImageLoader,
                           Dali::AnimatedImageLoading               animatedImageLoading,
                           uint32_t                                 frameIndex,
                           DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad)
{
  return GetImplementation(asyncImageLoader).LoadAnimatedImage(animatedImageLoading, frameIndex, preMultiplyOnLoad);
}

uint32_t Load(AsyncImageLoader                         asyncImageLoader,
              const std::string&                       url,
              ImageDimensions                          dimensions,
              FittingMode::Type                        fittingMode,
              SamplingMode::Type                       samplingMode,
              bool                                     orientationCorrection,
              DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad)
{
  return GetImplementation(asyncImageLoader).Load(Toolkit::Internal::VisualUrl(url), dimensions, fittingMode, samplingMode, orientationCorrection, preMultiplyOnLoad);
}

uint32_t ApplyMask(AsyncImageLoader                         asyncImageLoader,
                   Devel::PixelBuffer                       pixelBuffer,
                   Devel::PixelBuffer                       maskPixelBuffer,
                   float                                    contentScale,
                   bool                                     cropToMask,
                   DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad)
{
  return GetImplementation(asyncImageLoader).ApplyMask(pixelBuffer, maskPixelBuffer, contentScale, cropToMask, preMultiplyOnLoad);
}

PixelBufferLoadedSignalType& PixelBufferLoadedSignal(AsyncImageLoader asyncImageLoader)
{
  return GetImplementation(asyncImageLoader).PixelBufferLoadedSignal();
}

} // namespace DevelAsyncImageLoader
} // namespace Toolkit
} // namespace Dali

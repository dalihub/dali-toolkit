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
#include "async-image-loader.h"

// INTERNAL INCLUDES
#include <dali-toolkit/internal/image-loader/async-image-loader-impl.h>

namespace Dali
{

namespace Toolkit
{

AsyncImageLoader::AsyncImageLoader()
{
}

AsyncImageLoader::~AsyncImageLoader()
{
}

AsyncImageLoader::AsyncImageLoader( Internal::AsyncImageLoader* impl )
: BaseHandle( impl )
{}

AsyncImageLoader::AsyncImageLoader( const AsyncImageLoader& handle )
: BaseHandle( handle )
{
}

AsyncImageLoader& AsyncImageLoader::operator=( const AsyncImageLoader& handle )
{
  BaseHandle::operator=(handle);
  return *this;
}

AsyncImageLoader AsyncImageLoader::New()
{
  IntrusivePtr<Internal::AsyncImageLoader> internal = Internal::AsyncImageLoader::New();
   return AsyncImageLoader( internal.Get() );
}

uint32_t AsyncImageLoader::Load( const std::string& url )
{
  return GetImplementation(*this).Load( url, ImageDimensions(), FittingMode::DEFAULT, SamplingMode::BOX_THEN_LINEAR, true );
}

uint32_t AsyncImageLoader::Load( const std::string& url, ImageDimensions size )
{
  return GetImplementation(*this).Load( url, size, FittingMode::DEFAULT, SamplingMode::BOX_THEN_LINEAR, true );
}

uint32_t AsyncImageLoader::Load( const std::string& url,
                                 ImageDimensions size,
                                 FittingMode::Type fittingMode,
                                 SamplingMode::Type samplingMode,
                                 bool orientationCorrection )
{
  return GetImplementation(*this).Load( url, size, fittingMode, samplingMode, orientationCorrection );
}

bool AsyncImageLoader::Cancel( uint32_t loadingTaskId)
{
  return GetImplementation(*this).Cancel( loadingTaskId );
}

void AsyncImageLoader::CancelAll()
{
  GetImplementation(*this).CancelAll();
}

AsyncImageLoader::ImageLoadedSignalType& AsyncImageLoader::ImageLoadedSignal()
{
  return GetImplementation(*this).ImageLoadedSignal();
}
} // namespace Toolkit

} // namespace Dali

#ifndef DALI_TOOLKIT_DEVEL_API_IMAGE_LOADER_ASYNC_IMAGE_LOADER_DEVEL_H
#define DALI_TOOLKIT_DEVEL_API_IMAGE_LOADER_ASYNC_IMAGE_LOADER_DEVEL_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/signals/dali-signal.h>
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali-toolkit/public-api/image-loader/async-image-loader.h>

namespace Dali
{
namespace Toolkit
{
namespace DevelAsyncImageLoader
{

typedef Signal< void ( uint32_t, Devel::PixelBuffer ) > PixelBufferLoadedSignalType;

/**
 * Connect to this signal if you want to load a PixelBuffer instead of a PixelData.
 * @note Connecting to this signal prevents the emission of the ImageLoadedSignal.
 */
DALI_TOOLKIT_API PixelBufferLoadedSignalType&  PixelBufferLoadedSignal( AsyncImageLoader asyncImageLoader );

}
} // Toolkit
} // Dali

#endif

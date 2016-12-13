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
 *
 */

// CLASS HEADER
#include "async-image-loader-impl.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/adaptors/adaptor.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

AsyncImageLoader::AsyncImageLoader()
: mLoadedSignal(),
  mLoadThread( new EventThreadCallback( MakeCallback( this, &AsyncImageLoader::ProcessLoadedImage ) ) ),
  mLoadTaskId( 0u ),
  mIsLoadThreadStarted( false )
{
}

AsyncImageLoader::~AsyncImageLoader()
{
  mLoadThread.CancelAll();
}

IntrusivePtr<AsyncImageLoader> AsyncImageLoader::New()
{
  IntrusivePtr<AsyncImageLoader> internal = new AsyncImageLoader();
  return internal;
}

uint32_t AsyncImageLoader::Load( const std::string& url,
                                 ImageDimensions dimensions,
                                 FittingMode::Type fittingMode,
                                 SamplingMode::Type samplingMode,
                                 bool orientationCorrection )
{
  if( !mIsLoadThreadStarted )
  {
    mLoadThread.Start();
    mIsLoadThreadStarted = true;
  }

  mLoadThread.AddTask( new LoadingTask( ++mLoadTaskId, url, dimensions, fittingMode, samplingMode, orientationCorrection ) );

  return mLoadTaskId;
}

Toolkit::AsyncImageLoader::ImageLoadedSignalType& AsyncImageLoader::ImageLoadedSignal()
{
  return mLoadedSignal;
}

bool AsyncImageLoader::Cancel( uint32_t loadingTaskId )
{
  return mLoadThread.CancelTask( loadingTaskId );
}

void AsyncImageLoader::CancelAll()
{
  mLoadThread.CancelAll();
}

void AsyncImageLoader::ProcessLoadedImage()
{
  while( LoadingTask *next =  mLoadThread.NextCompletedTask() )
  {
    mLoadedSignal.Emit( next->id, next->pixelData );
    delete next;
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

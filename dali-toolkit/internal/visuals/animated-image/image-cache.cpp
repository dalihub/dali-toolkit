/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#include "image-cache.h"

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

ImageCache::ImageCache( TextureManager&                 textureManager,
                        ImageCache::FrameReadyObserver& observer,
                        unsigned int                    batchSize )
: mTextureManager( textureManager ),
  mObserver( observer ),
  mBatchSize( batchSize ),
  mUrlIndex(0u),
  mWaitingForReadyFrame(false),
  mRequestingLoad(false),
  mTextureManagerAlive(true)
{
  mTextureManager.AddObserver( *this );
}

ImageCache::~ImageCache()
{
  if( mTextureManagerAlive )
  {
    mTextureManager.RemoveObserver( *this );
  }
}

void ImageCache::TextureManagerDestroyed()
{
  mTextureManagerAlive = false;
}

} //namespace Internal
} //namespace Toolkit
} //namespace Dali

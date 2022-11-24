/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
ImageCache::ImageCache(TextureManager&                     textureManager,
                       ImageDimensions                     size,
                       Dali::FittingMode::Type             fittingMode,
                       Dali::SamplingMode::Type            samplingMode,
                       TextureManager::MaskingDataPointer& maskingData,
                       ImageCache::FrameReadyObserver&     observer,
                       uint32_t                            batchSize,
                       uint32_t                            interval,
                       bool                                preMultiplyOnLoad)
: mTextureManager(textureManager),
  mObserver(observer),
  mMaskingData(maskingData),
  mDesiredSize(size),
  mFittingMode(fittingMode),
  mSamplingMode(samplingMode),
  mBatchSize(batchSize),
  mInterval(interval),
  mLoadState(TextureManager::LoadState::NOT_STARTED),
  mRequestingLoad(false),
  mPreMultiplyOnLoad(preMultiplyOnLoad),
  mTextureManagerAlive(true)
{
  mTextureManager.AddObserver(*this);
}

ImageCache::~ImageCache()
{
  if(mTextureManagerAlive)
  {
    mTextureManager.RemoveObserver(*this);
  }
}

void ImageCache::TextureManagerDestroyed()
{
  mTextureManagerAlive = false;
}

void ImageCache::SetInterval(uint32_t interval)
{
  mInterval = interval;
}

} //namespace Internal
} //namespace Toolkit
} //namespace Dali

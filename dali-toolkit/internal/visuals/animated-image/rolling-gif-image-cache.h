#ifndef DALI_TOOLKIT_INTERNAL_ROLLING_GIF_IMAGE_CACHE_H
#define DALI_TOOLKIT_INTERNAL_ROLLING_GIF_IMAGE_CACHE_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/gif-loading.h>
#include <dali/devel-api/common/circular-queue.h>
#include <dali-toolkit/internal/visuals/animated-image/image-cache.h>
#include <dali-toolkit/internal/visuals/texture-manager-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

/**
 * Class to manage a rolling cache of GIF images, where the cache size
 * is smaller than the total number of images.
 *
 * Frames are always ready, so the observer.FrameReady callback is never triggered;
 * the FirstFrame and NextFrame APIs will always return a texture.
 */
class RollingGifImageCache : public ImageCache
{
public:
  /**
   * Constructor.
   * @param[in] textureManager The texture manager
   * @param[in] gifLoader The loaded gif image
   * @param[in] frameCount The number of frames in the gif
   * @param[in] observer FrameReady observer
   * @param[in] cacheSize The size of the cache
   * @param[in] batchSize The size of a batch to load
   *
   * This will start loading textures immediately, according to the
   * batch and cache sizes.
   */
  RollingGifImageCache( TextureManager&                 textureManager,
                        GifLoading&                     gifLoader,
                        uint32_t                        frameCount,
                        ImageCache::FrameReadyObserver& observer,
                        uint16_t                        cacheSize,
                        uint16_t                        batchSize );

  /**
   * Destructor
   */
  virtual ~RollingGifImageCache();

  /**
   * Get the first frame. If it's not ready, this will trigger the
   * sending of FrameReady() when the image becomes ready.
   */
  TextureSet FirstFrame() override;

  /**
   * Get the next frame. If it's not ready, this will trigger the
   * sending of FrameReady() when the image becomes ready.
   * This will trigger the loading of the next batch.
   */
  TextureSet NextFrame() override;

private:
  /**
   * @return true if the front frame is ready
   */
  bool IsFrontReady() const;

  /**
   * Load the next batch of images
   */
  void LoadBatch();

  /**
   * Get the texture set of the front frame.
   * @return the texture set
   */
  TextureSet GetFrontTextureSet() const;

  /**
   * Get the texture id of the given index
   */
  TextureManager::TextureId GetCachedTextureId( int index ) const;

private:
  /**
   * Secondary class to hold readiness and index into url
   */
  struct ImageFrame
  {
    unsigned int mFrameNumber = 0u;
  };

  GifLoading&               mGifLoading;
  uint32_t                  mFrameCount;
  int                       mFrameIndex;
  std::vector<UrlStore>     mImageUrls;
  uint16_t                  mCacheSize;
  CircularQueue<ImageFrame> mQueue;
};

} // namespace Internal
} // namespace Toolkit
} // namespace Dali

#endif

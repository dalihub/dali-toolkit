#ifndef DALI_TOOLKIT_INTERNAL_ROLLING_IMAGE_CACHE_H
#define DALI_TOOLKIT_INTERNAL_ROLLING_IMAGE_CACHE_H

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

// EXTERNAL INCLUDES
#include <dali-toolkit/internal/texture-manager/texture-manager-impl.h>
#include <dali-toolkit/internal/visuals/animated-image/image-cache.h>
#include <dali/devel-api/common/circular-queue.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
/**
 * Class to manage a rolling cache of images, where the cache size
 * is smaller than the total number of images.
 */
class RollingImageCache : public ImageCache, public TextureUploadObserver
{
public:
  /**
   * Constructor.
   * @param[in] textureManager The texture manager
   * @param[in] urlList List of urls to cache
   * @param[in] observer FrameReady observer
   * @param[in] cacheSize The size of the cache
   * @param[in] batchSize The size of a batch to load
   *
   * This will start loading textures immediately, according to the
   * batch and cache sizes.
   */
  RollingImageCache(TextureManager&                 textureManager,
                    UrlList&                        urlList,
                    ImageCache::FrameReadyObserver& observer,
                    uint16_t                        cacheSize,
                    uint16_t                        batchSize);

  /**
   * Destructor
   */
  ~RollingImageCache() override;

  /**
   * Get the Nth frame. If it's not ready, this will trigger the
   * sending of FrameReady() when the image becomes ready.
   */
  TextureSet Frame(uint32_t frameIndex) override;

  /**
   * Get the first frame. If it's not ready, this will trigger the
   * sending of FrameReady() when the image becomes ready.
   */
  TextureSet FirstFrame() override;

  /**
   * Get the next frame. If it's not ready, this will trigger the
   * sending of FrameReady() when the image becomes ready.
   */
  TextureSet NextFrame() override;

  /**
   * Get the interval of Nth frame.
   */
  uint32_t GetFrameInterval(uint32_t frameIndex) const override;

  /**
   * Get the current rendered frame index.
   * If there isn't any loaded frame, returns -1.
   */
  int32_t GetCurrentFrameIndex() const override;

  /**
   * Get total frame count of the animated image file.
   */
  int32_t GetTotalFrameCount() const override;

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
   * Find the matching image frame, and set it to ready
   */
  void SetImageFrameReady(TextureManager::TextureId textureId);

  /**
   * Get the texture set of the front frame.
   * @return the texture set
   */
  TextureSet GetFrontTextureSet() const;

  /**
   * Get the texture id of the given index
   */
  TextureManager::TextureId GetCachedTextureId(int index) const;

  /**
   * Check if the front frame has become ready - if so, inform observer
   * @param[in] wasReady Readiness before call.
   */
  void CheckFrontFrame(bool wasReady);

protected:
  void LoadComplete(bool loadSuccess, TextureInformation textureInformation) override;

private:
  /**
   * Secondary class to hold readiness and index into url
   */
  struct ImageFrame
  {
    unsigned int mUrlIndex = 0u;
    bool         mReady    = false;
  };

  std::vector<UrlStore>&    mImageUrls;
  CircularQueue<ImageFrame> mQueue;
};

} // namespace Internal
} // namespace Toolkit
} // namespace Dali

#endif

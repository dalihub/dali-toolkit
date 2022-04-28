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
   * @param[in] urlList        List of urls to cache
   * @param[in] maskingData    Masking data to be applied.
   * @param[in] observer       FrameReady observer
   * @param[in] cacheSize      The size of the cache
   * @param[in] batchSize      The size of a batch to load
   * @param[in] interval       Time interval between each frame
   *
   * This will start loading textures immediately, according to the
   * batch and cache sizes.
   */
  RollingImageCache(TextureManager&                     textureManager,
                    UrlList&                            urlList,
                    TextureManager::MaskingDataPointer& maskingData,
                    ImageCache::FrameReadyObserver&     observer,
                    uint16_t                            cacheSize,
                    uint16_t                            batchSize,
                    uint32_t                            interval);

  /**
   * Destructor
   */
  ~RollingImageCache() override;

  /**
   * @copydoc Internal::ImageCache::Frame()
   */
  TextureSet Frame(uint32_t frameIndex) override;

  /**
   * @copydoc Internal::ImageCache::FirstFrame()
   */
  TextureSet FirstFrame() override;

  /**
   * @copydoc Internal::ImageCache::GetFrameInterval()
   */
  uint32_t GetFrameInterval(uint32_t frameIndex) const override;

  /**
   * @copydoc Internal::ImageCache::GetCurrentFrameIndex()
   */
  int32_t GetCurrentFrameIndex() const override;

  /**
   * @copydoc Internal::ImageCache::GetTotalFrameCount()
   */
  int32_t GetTotalFrameCount() const override;

  /**
   * @copydoc Internal::ImageCache::ClearCache()
   */
  void ClearCache() override;

private:
  /**
   * @brief Check whether the front frame is ready or not.
   *
   * @return true if the front frame is ready
   */
  bool IsFrontReady() const;

  /**
   * @brief Load the next batch of images
   *
   * @param[in] frameIndex starting frame index of batch to be loaded.
   */
  void LoadBatch(uint32_t frameIndex);

  /**
   * @brief Get the texture set of the front frame.
   *
   * @return the texture set of the front of Cache.
   */
  TextureSet GetFrontTextureSet() const;

  /**
   * @brief Get the texture id of the given index
   *
   * @param[in] index index of the queue.
   */
  TextureManager::TextureId GetCachedTextureId(int index) const;

  /**
   * @brief Pop front entity of Cache.
   */
  void PopFrontCache();

protected:
  /**
   * @copydoc Toolkit::TextureUploadObserver::LoadComplete()
   */
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

  std::vector<UrlStore>&                 mImageUrls;
  CircularQueue<ImageFrame>              mQueue;
};

} // namespace Internal
} // namespace Toolkit
} // namespace Dali

#endif

#ifndef DALI_TOOLKIT_INTERNAL_ROLLING_ANIMATED_IMAGE_CACHE_H
#define DALI_TOOLKIT_INTERNAL_ROLLING_ANIMATED_IMAGE_CACHE_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/visuals/animated-image/image-cache.h>
#include <dali-toolkit/internal/visuals/texture-manager-impl.h>
#include <dali/devel-api/adaptor-framework/animated-image-loading.h>
#include <dali/devel-api/common/circular-queue.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
/**
 * Class to manage a rolling cache of Animated images, where the cache size
 * is smaller than the total number of images.
 *
 * Frames are always ready, so the observer.FrameReady callback is never triggered;
 * the FirstFrame and NextFrame APIs will always return a texture.
 */
class RollingAnimatedImageCache : public ImageCache, public TextureUploadObserver
{
public:
  /**
   * @brief Constructor.
   * @param[in] textureManager The texture manager
   * @param[in] animatedImageLoader The loaded animated image
   * @param[in] observer FrameReady observer
   * @param[in] cacheSize The size of the cache
   * @param[in] batchSize The size of a batch to load
   * @param[in] isSynchronousLoading The flag to define whether to load first frame synchronously
   *
   * This will start loading textures immediately, according to the
   * batch and cache sizes.
   */
  RollingAnimatedImageCache(TextureManager&                 textureManager,
                            AnimatedImageLoading&           animatedImageLoader,
                            ImageCache::FrameReadyObserver& observer,
                            uint16_t                        cacheSize,
                            uint16_t                        batchSize,
                            bool                            isSynchronousLoading);

  /**
   * @brief Destructor
   */
  ~RollingAnimatedImageCache() override;

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
   * @copydoc Internal::ImageCache::GetLoadState()
   */
  TextureManager::LoadState GetLoadState() const override;

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
   * @brief Request to Load a frame
   *
   * @param[in] frameIndex          index of frame to be loaded.
   * @param[in] useCache            true if this frame loading uses cache.
   * @param[in] synchronousLoading  true if the frame should be loaded synchronously
   */
  TextureSet RequestFrameLoading(uint32_t frameIndex, bool useCache, bool synchronousLoading);

  /**
   * @brief Load the next batch of images
   *
   * @param[in] frameIndex starting frame index of batch to be loaded.
   */
  void LoadBatch(uint32_t frameIndex);

  /**
   * @brief Find the matching image frame, and set it to ready
   *
   * @param[in] textureId texture id to be marked as ready.
   */
  void SetImageFrameReady(TextureManager::TextureId textureId);

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

protected:

  /**
   * @copydoc Toolkit::TextureUploadObserver::AnimatedImageUploadComplete()
   */
  void AnimatedImageUploadComplete(bool loadSuccess, int32_t textureId, uint32_t frameCount, uint32_t interval) override;

private:

  /**
   * Secondary class to hold readiness and index into url
   */
  struct ImageFrame
  {
    uint32_t mFrameNumber = 0u;
    bool     mReady       = false;
  };

  Dali::AnimatedImageLoading mAnimatedImageLoading;
  uint32_t                   mFrameCount;
  uint32_t                   mFrameIndex;
  uint32_t                   mCacheSize;
  std::vector<UrlStore>      mImageUrls;
  std::vector<int32_t>       mIntervals;
  std::vector<uint32_t>      mLoadWaitingQueue;
  CircularQueue<ImageFrame>  mQueue;
  TextureManager::LoadState  mLoadState; ///< The texture loading state
  bool                       mIsSynchronousLoading;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif //DALI_TOOLKIT_INTERNAL_ROLLING_ANIMATED_IMAGE_CACHE_H

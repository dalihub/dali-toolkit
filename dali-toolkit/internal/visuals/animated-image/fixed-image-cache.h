#ifndef DALI_TOOLKIT_INTERNAL_FIXED_IMAGE_CACHE_H
#define DALI_TOOLKIT_INTERNAL_FIXED_IMAGE_CACHE_H

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

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class FixedImageCache : public ImageCache, public TextureUploadObserver
{
public:
  /**
   * Constructor.
   * @param[in] textureManager The texture manager
   * @param[in] urlList List of urls to cache
   * @param[in] observer FrameReady observer
   * @param[in] batchSize The size of a batch to load
   *
   * This will start loading textures immediately, according to the
   * batch and cache sizes. The cache is as large as the number of urls.
   */
  FixedImageCache(TextureManager&                 textureManager,
                  UrlList&                        urlList,
                  ImageCache::FrameReadyObserver& observer,
                  unsigned int                    batchSize);

  ~FixedImageCache() override;

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
   * Check if the front frame has become ready - if so, inform observer
   * @param[in] wasReady Readiness before call.
   */
  void CheckFrontFrame(bool wasReady);

protected:
  void LoadComplete(bool loadSuccess, TextureInformation textureInformation) override;

private:
  std::vector<UrlStore>& mImageUrls;
  std::vector<bool>      mReadyFlags;
  unsigned int           mFront;
};

} //namespace Internal
} //namespace Toolkit
} //namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_FIXED_IMAGE_CACHE_H

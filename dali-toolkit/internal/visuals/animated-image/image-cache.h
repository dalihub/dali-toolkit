#ifndef DALI_TOOLKIT_INTERNAL_IMAGE_CACHE_H
#define DALI_TOOLKIT_INTERNAL_IMAGE_CACHE_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/visuals/texture-upload-observer.h>
#include <dali-toolkit/internal/visuals/texture-manager-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

class ImageCache : public TextureManager::LifecycleObserver
{
public:
  /**
   * Observer class to inform when the next image is ready
   */
  class FrameReadyObserver
  {
  public:
    /**
     * Informs observer when the next texture set is ready to display
     * @param[in] textureSet The ready texture set
     */
    virtual void FrameReady( TextureSet textureSet ) = 0;
  };

  struct UrlStore
  {
    TextureManager::TextureId mTextureId = TextureManager::INVALID_TEXTURE_ID;
    std::string mUrl;
  };

  /**
   * List of URLs
   */
  typedef std::vector<UrlStore> UrlList;

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
  ImageCache( TextureManager&                 textureManager,
              ImageCache::FrameReadyObserver& observer,
              unsigned int                    batchSize );

  virtual ~ImageCache();

  /**
   * Get the first frame. If it's not ready, this will trigger the
   * sending of FrameReady() when the image becomes ready.
   */
  virtual TextureSet FirstFrame() = 0;

  /**
   * Get the next frame. If it's not ready, this will trigger the
   * sending of FrameReady() when the image becomes ready.
   */
  virtual TextureSet NextFrame() = 0;

  /**
   * Get the Nth frame. If it's not ready, this will trigger the
   * sending of FrameReady() when the image becomes ready.
   */
  virtual TextureSet Frame( uint32_t frameIndex ) = 0;

  /**
   * Get the interval of Nth frame.
   */
  virtual uint32_t GetFrameInterval( uint32_t frameIndex ) const = 0;

  /**
   * Get the current rendered frame index.
   * If there isn't any loaded frame, returns -1.
   */
  virtual int32_t GetCurrentFrameIndex() const = 0;

private:

  /**
   * Called before the texture manager is destroyed.
   */
  void TextureManagerDestroyed() final;

protected:
  TextureManager&        mTextureManager;
  FrameReadyObserver&    mObserver;
  unsigned int           mBatchSize;
  unsigned int           mUrlIndex;
  bool                   mWaitingForReadyFrame:1;
  bool                   mRequestingLoad:1;
  bool                   mTextureManagerAlive:1;
};

} //namespace Internal
} //namespace Toolkit
} //namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_IMAGE_CACHE_H

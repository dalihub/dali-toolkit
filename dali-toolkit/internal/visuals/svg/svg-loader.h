#ifndef DALI_TOOLKIT_SVG_LOADER_H
#define DALI_TOOLKIT_SVG_LOADER_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/vector-image-renderer.h>
#include <dali/integration-api/processor-interface.h>
#include <dali/public-api/adaptor-framework/async-task-manager.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/rendering/texture-set.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/svg/svg-loader-observer.h>
#include <dali-toolkit/internal/visuals/svg/svg-task.h>
#include <dali-toolkit/internal/visuals/visual-url.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class VisualFactoryCache;

/**
 * The manager for loading Svg textures.
 * It caches them internally for better performance; i.e. to avoid loading and
 * parsing the files over and over.
 *
 * @note To use EncodedImageBuffer and AtlasManager, we need to set VisualFactoryCache.
 */
class SvgLoader : public ConnectionTracker, public Integration::Processor
{
public:
  typedef int32_t SvgLoadId;      ///< The SvgLoadId type. This is used as a handle to refer to a particular SvgLoader Data.
  typedef int32_t SvgRasterizeId; ///< The SvgRasterizeId type. This is used as a handle to refer to a particular SvgLoader Data.

  static constexpr SvgLoadId      INVALID_SVG_LOAD_ID      = -1; ///< Used to represent a null SvgLoadId or error
  static constexpr SvgRasterizeId INVALID_SVG_RASTERIZE_ID = -1; ///< Used to represent a null SvgRasterizeId or error

  /**
   * @brief The LoadState Enumeration represents the current state of a particular Svg data's life-cycle.
   */
  enum class LoadState
  {
    NOT_STARTED,   ///< Default
    LOADING,       ///< Loading has been started, but not finished.
    LOAD_FINISHED, ///< Loading has finished.
    CANCELLED,     ///< Removed before loading completed
    LOAD_FAILED    ///< Async loading failed, e.g. connection problem
  };

  /**
   * @brief The RasterizeState Enumeration represents the current state of a particular Svg data's life-cycle.
   */
  enum class RasterizeState
  {
    NOT_STARTED,  ///< Default
    RASTERIZING,  ///< Rasterizing has been started, but not finished.
    UPLOADED,     ///< Upload has finished.
    UPLOAD_FAILED ///< Async rasterizing failed, e.g. connection problem
  };

private:
  typedef uint32_t               SvgCacheIndex; ///< The Cache index type. Only be used internally
  static constexpr SvgCacheIndex INVALID_SVG_CACHE_INDEX = static_cast<SvgCacheIndex>(std::numeric_limits<SvgCacheIndex>::max());

  using ObserverContainer = Dali::Vector<SvgLoaderObserver*>;

public:
  /**
   * Constructor
   */
  SvgLoader();

  /**
   * Destructor, non-virtual as not a base class
   */
  ~SvgLoader();

  /**
   * @brief Request to load an SVG file.
   * It will notify SvgLoaderObserver->LoadComplete before we call RequestLoadRemove.
   * @note Even if synchronousLoading is true, we notify observer always.
   * @note If we meat cached svg load, notify LoadComplete first, and then API function finished.
   *
   * @param[in] url to retrieve
   * @param[in] dpi The DPI of the screen.
   * @param[in] svgObserver The SvgVisual that requested loading.
   * @param[in] synchronousLoading True if the image will be loaded in synchronous time.
   * @return id of the load request.
   */
  SvgLoadId Load(const VisualUrl& url, float dpi, SvgLoaderObserver* svgObserver, bool synchronousLoading);

  /**
   * @brief Request to rasterize an SVG file.
   * It will notify SvgLoaderObserver->RasterizeComplete before we call RequestRasterizeRemove.
   * @note Even if synchronousLoading is true, we notify observer always.
   * @note If we meat cached svg rasterize, notify RasterizeComplete first, and then API function finished.
   *
   * @todo RasterizeComplete will not be called forever if loading failed. Should we need to support it?
   *
   * @param[in] loaderId to retrieve
   * @param[in] width The rasterization width.
   * @param[in] height The rasterization height.
   * @param[in] attemptAtlasing True if we need to attempt atlas the image.
   * @param[in] svgObserver The SvgVisual that requested loading.
   * @param[in] synchronousLoading True if the image will be loaded in synchronous time.
   * @return id of the rasterize request.
   */
  SvgRasterizeId Rasterize(SvgLoadId loaderId, uint32_t width, uint32_t height, bool attemptAtlasing, SvgLoaderObserver* svgObserver, bool synchronousLoading);

  /**
   * @brief Request to remove a texture matching id.
   * Erase the observer from the observer list of cache if we need.
   *
   * @param[in] loadId cache data id
   * @param[in] svgObserver The SvgVisual that requested loading.
   */
  void RequestLoadRemove(SvgLoadId loadId, SvgLoaderObserver* svgObserver);

  /**
   * @brief Request to remove a texture matching id.
   * Erase the observer from the observer list of cache if we need.
   *
   * @param[in] rasterizeId cache data id
   * @param[in] svgObserver The SvgVisual that requested loading.
   * @param[in] removalSynchronously Whether the removal should be done synchronously, or not.
   *                                 It will be true when we want to ignore unused rasterize task.
   */
  void RequestRasterizeRemove(SvgRasterizeId rasterizeId, SvgLoaderObserver* svgObserver, bool removalSynchronously);

  /**
   * @brief Get the VectorImageRenderer of matching loadId.
   *
   * @param[in] loadId cache data id
   * @return The vector image renderer of matching loadId.
   */
  VectorImageRenderer GetVectorImageRenderer(SvgLoadId loadId) const;

protected: // Implementation of Processor
  /**
   * @copydoc Dali::Integration::Processor::Process()
   */
  void Process(bool postProcessor) override;

  /**
   * @copydoc Dali::Integration::Processor::GetProcessorName()
   */
  std::string_view GetProcessorName() const override
  {
    return "SvgLoader";
  }

private:
  SvgLoadId GenerateUniqueSvgLoadId();

  SvgRasterizeId GenerateUniqueSvgRasterizeId();

  SvgCacheIndex GetCacheIndexFromLoadCacheById(const SvgLoadId loadId) const;

  SvgCacheIndex GetCacheIndexFromRasterizeCacheById(const SvgRasterizeId rasterizeId) const;

  SvgCacheIndex FindCacheIndexFromLoadCache(const VisualUrl& imageUrl, float dpi) const;

  SvgCacheIndex FindCacheIndexFromRasterizeCache(const SvgLoadId loadId, uint32_t width, uint32_t height, bool attemptAtlasing) const;

  /**
   * @brief Remove a texture matching id.
   * Erase the observer from the observer list of cache if we need.
   * This API decrease cached SvgLoadInfo reference.
   * If the SvgLoadInfo reference become 0, the VectorImageRenderer will be reset.
   *
   * @param[in] loadId cache data id
   */
  void RemoveLoad(SvgLoadId loadId);

  /**
   * @brief Remove a texture matching id.
   * Erase the observer from the observer list of cache if we need.
   * This API decrease cached SvgRasterizeInfo reference.
   * If the SvgRasterizeInfo reference become 0, the textureSet will be reset.
   *
   * @param[in] rasterizeId cache data id
   */
  void RemoveRasterize(SvgRasterizeId rasterizeId);

public:
  /**
   * @brief Information of Svg image load data
   */
  struct SvgLoadInfo
  {
    SvgLoadInfo(SvgLoadId loadId, const VisualUrl& url, float dpi)
    : mId(loadId),
      mTask(),
      mImageUrl(url),
      mDpi(dpi),
      mLoadState(LoadState::NOT_STARTED),
      mVectorImageRenderer(Dali::VectorImageRenderer::New()),
      mObservers(),
      mReferenceCount(1u)
    {
    }
    ~SvgLoadInfo()
    {
    }

    SvgLoadInfo(SvgLoadInfo&& info) noexcept // move constructor
    : mId(info.mId),
      mTask(std::move(info.mTask)),
      mImageUrl(std::move(info.mImageUrl)),
      mDpi(info.mDpi),
      mLoadState(info.mLoadState),
      mVectorImageRenderer(std::move(info.mVectorImageRenderer)),
      mObservers(std::move(info.mObservers)),
      mReferenceCount(info.mReferenceCount)
    {
      info.mTask.Reset();
      info.mVectorImageRenderer.Reset();
      info.mReferenceCount = 0;
    }
    SvgLoadInfo& operator=(SvgLoadInfo&& info) noexcept // move operator
    {
      if(this != &info)
      {
        mId   = info.mId;
        mTask = std::move(info.mTask);

        mImageUrl = std::move(info.mImageUrl);

        mDpi = info.mDpi;

        mLoadState           = info.mLoadState;
        mVectorImageRenderer = std::move(info.mVectorImageRenderer);

        mObservers = std::move(info.mObservers);

        mReferenceCount = info.mReferenceCount;

        info.mTask.Reset();
        info.mVectorImageRenderer.Reset();
        info.mReferenceCount = 0;
      }
      return *this;
    }

  private:
    SvgLoadInfo()                                   = delete; // Do not use empty constructor
    SvgLoadInfo(const SvgLoadInfo& info)            = delete; // Do not use copy constructor
    SvgLoadInfo& operator=(const SvgLoadInfo& info) = delete; // Do not use copy assign

  public:
    SvgLoadId         mId;
    SvgLoadingTaskPtr mTask; ///< Async task. It would be deleted when loading completed.

    VisualUrl mImageUrl;
    float     mDpi;

    LoadState           mLoadState;
    VectorImageRenderer mVectorImageRenderer;

    ObserverContainer mObservers;

    int32_t mReferenceCount; ///< The number of Svg visuals that use this data.
  };

  /**
   * @brief Information of Svg image rasterize data
   */
  struct SvgRasterizeInfo
  {
    SvgRasterizeInfo(SvgRasterizeId rasterizeId, SvgLoadId loadId, uint32_t width, uint32_t height, bool attemptAtlasing)
    : mId(rasterizeId),
      mTask(),
      mLoadId(loadId),
      mWidth(width),
      mHeight(height),
      mAttemptAtlasing(attemptAtlasing),
      mRasterizeState(RasterizeState::NOT_STARTED),
      mTextureSet(),
      mAtlasRect(Vector4(0.0f, 0.0f, 1.0f, 1.0f)),
      mAtlasAttempted(attemptAtlasing), ///< Let we assume that atlas attempted until rasterize completed.
      mObservers(),
      mReferenceCount(1u)
    {
    }
    ~SvgRasterizeInfo()
    {
    }
    SvgRasterizeInfo(SvgRasterizeInfo&& info) noexcept // move constructor
    : mId(info.mId),
      mTask(std::move(info.mTask)),
      mLoadId(info.mLoadId),
      mWidth(info.mWidth),
      mHeight(info.mHeight),
      mAttemptAtlasing(info.mAttemptAtlasing),
      mRasterizeState(info.mRasterizeState),
      mTextureSet(std::move(info.mTextureSet)),
      mAtlasRect(std::move(info.mAtlasRect)),
      mAtlasAttempted(info.mAtlasAttempted),
      mObservers(std::move(info.mObservers)),
      mReferenceCount(info.mReferenceCount)
    {
      info.mTask.Reset();
      info.mTextureSet.Reset();
      info.mReferenceCount = 0;
    }
    SvgRasterizeInfo& operator=(SvgRasterizeInfo&& info) noexcept // move operator
    {
      if(this != &info)
      {
        mId   = info.mId;
        mTask = std::move(info.mTask);

        mLoadId          = info.mLoadId;
        mWidth           = info.mWidth;
        mHeight          = info.mHeight;
        mAttemptAtlasing = info.mAttemptAtlasing;

        mRasterizeState = info.mRasterizeState;
        mTextureSet     = std::move(info.mTextureSet);
        mAtlasRect      = std::move(info.mAtlasRect);
        mAtlasAttempted = info.mAtlasAttempted;

        mObservers = std::move(info.mObservers);

        mReferenceCount = info.mReferenceCount;

        info.mTask.Reset();
        info.mTextureSet.Reset();
        info.mReferenceCount = 0;
      }
      return *this;
    }

  private:
    SvgRasterizeInfo()                                        = delete; // Do not use empty constructor
    SvgRasterizeInfo(const SvgRasterizeInfo& info)            = delete; // Do not use copy constructor
    SvgRasterizeInfo& operator=(const SvgRasterizeInfo& info) = delete; // Do not use copy assign

  public:
    SvgRasterizeId        mId;
    SvgRasterizingTaskPtr mTask; ///< Async task. It would be deleted when rasterizing completed.

    SvgLoadId mLoadId;
    uint32_t  mWidth;
    uint32_t  mHeight;
    bool      mAttemptAtlasing; ///< True if atlas requested.

    RasterizeState   mRasterizeState;
    Dali::TextureSet mTextureSet; ///< The texture set from atlas manager, or rasterized result at index 0.
    Vector4          mAtlasRect;
    bool             mAtlasAttempted; ///< True if atlasing attempted. False if atlasing failed

    ObserverContainer mObservers;

    int32_t mReferenceCount; ///< The number of Svg visuals that use this data.
  };

private: ///< Internal Methods for load
  void LoadOrQueue(SvgLoader::SvgLoadInfo& loadInfo, SvgLoaderObserver* svgObserver);
  void LoadRequest(SvgLoader::SvgLoadInfo& loadInfo, SvgLoaderObserver* svgObserver);
  void LoadSynchronously(SvgLoader::SvgLoadInfo& loadInfo, SvgLoaderObserver* svgObserver);
  void AddLoadObserver(SvgLoader::SvgLoadInfo& loadInfo, SvgLoaderObserver* svgObserver);
  void RemoveLoadObserver(SvgLoader::SvgLoadInfo& loadInfo, SvgLoaderObserver* svgObserver);
  void ProcessLoadQueue();

  /**
   * Notify the current observers that the svg load is complete,
   * then remove the observers from the list.
   * @param[in] loadInfo The load info.
   */
  void NotifyLoadObservers(SvgLoader::SvgLoadInfo& loadInfo);

private: ///< Internal Methods for rasterize
  void RasterizeOrQueue(SvgLoader::SvgRasterizeInfo& rasterizeInfo, SvgLoaderObserver* svgObserver);
  void RasterizeRequest(SvgLoader::SvgRasterizeInfo& rasterizeInfo, SvgLoaderObserver* svgObserver);
  void RasterizeSynchronously(SvgLoader::SvgRasterizeInfo& rasterizeInfo, SvgLoaderObserver* svgObserver);
  void AddRasterizeObserver(SvgLoader::SvgRasterizeInfo& rasterizeInfo, SvgLoaderObserver* svgObserver);
  void RemoveRasterizeObserver(SvgLoader::SvgRasterizeInfo& rasterizeInfo, SvgLoaderObserver* svgObserver);
  void ProcessRasterizeQueue();

  /**
   * Notify the current observers that the svg rasterize is complete,
   * then remove the observers from the list.
   * @param[in] rasterizeInfo The rasterize info.
   */
  void NotifyRasterizeObservers(SvgLoader::SvgRasterizeInfo& rasterizeInfo);

public: ///< Methods for the VisualFactoryCache.
  void SetVisualFactoryCache(VisualFactoryCache& factoryCache)
  {
    mFactoryCache = &factoryCache;
  }

private:
  /**
   * @brief Common method to handle loading completion.
   * @param[in] task The task that has completed.
   */
  void AsyncLoadComplete(SvgTaskPtr task);

  /**
   * @brief Common method to handle rasterizing completion.
   * @param[in] task The task that has completed.
   */
  void AsyncRasterizeComplete(SvgTaskPtr task);

  /**
   * This is called by the SvgLoaderObserver when an observer is destroyed during loading.
   * We use the callback to know when to remove an observer from our notify list.
   * @param[in] observer The observer that generated the callback
   */
  void LoadObserverDestroyed(SvgLoaderObserver* svgObserver);

  /**
   * This is called by the SvgLoaderObserver when an observer is destroyed during rasterizing.
   * We use the callback to know when to remove an observer from our notify list.
   * @param[in] observer The observer that generated the callback
   */
  void RasterizeObserverDestroyed(SvgLoaderObserver* svgObserver);

protected:
  /**
   * Undefined copy constructor.
   */
  SvgLoader(const SvgLoader&) = delete;

  /**
   * Undefined assignment operator.
   */
  SvgLoader& operator=(const SvgLoader& rhs) = delete;

private:
  VisualFactoryCache* mFactoryCache; ///< The holder of visual factory cache. It will be used when we want to get atlas manager;

  SvgLoadId      mCurrentSvgLoadId;
  SvgRasterizeId mCurrentSvgRasterizeId;

  std::vector<SvgLoader::SvgLoadInfo>      mLoadCache{};
  std::vector<SvgLoader::SvgRasterizeInfo> mRasterizeCache{};

  using LoadQueueElement = std::pair<SvgLoadId, SvgLoaderObserver*>;
  Dali::Vector<LoadQueueElement> mLoadQueue{};        ///< Queue of svg load after NotifyLoadObservers
  SvgLoadId                      mLoadingQueueLoadId; ///< SvgLoadId when it is loading. it causes Load SVG to be queued.

  using RasterizeQueueElement = std::pair<SvgRasterizeId, SvgLoaderObserver*>;
  Dali::Vector<RasterizeQueueElement> mRasterizeQueue{};            ///< Queue of svg rasterze after NotifyRasterizeObservers
  SvgRasterizeId                      mRasterizingQueueRasterizeId; ///< SvgRasterizeId when it is rasterizing. it causes Rasterize SVG to be queued.

  std::vector<SvgLoadId>      mLoadRemoveQueue{};      ///< Queue of SvgLoader::SvgLoadInfo to remove at PostProcess. It will be cleared after PostProcess.
  std::vector<SvgRasterizeId> mRasterizeRemoveQueue{}; ///< Queue of SvgLoader::SvgRasterizeInfo to remove at PostProcess. It will be cleared after PostProcess.

  bool mRemoveProcessorRegistered : 1; ///< Flag if remove processor registered or not.
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_SVG_LOADER_H

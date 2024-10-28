/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/visuals/svg/svg-loader.h>

// INTERNAL HEADERS
#include <dali-toolkit/internal/texture-manager/texture-manager-impl.h> ///< for EncodedImageBuffer
#include <dali-toolkit/internal/visuals/image/image-atlas-manager.h>
#include <dali-toolkit/internal/visuals/svg/svg-task.h>
#include <dali-toolkit/internal/visuals/svg/svg-visual.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>

// EXTERNAL HEADERS
#include <dali/devel-api/common/hash.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>
#include <dali/public-api/adaptor-framework/encoded-image-buffer.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_IMAGE_PERFORMANCE_MARKER, false);

#ifdef DEBUG_ENABLED
Debug::Filter* gSvgLoaderLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_SVG_LOADER");

// clang-format off
#define GET_LOAD_STATE_STRING(loadState) \
  loadState == SvgLoader::LoadState::NOT_STARTED   ? "NOT_STARTED"   : \
  loadState == SvgLoader::LoadState::LOADING       ? "LOADING"       : \
  loadState == SvgLoader::LoadState::LOAD_FINISHED ? "LOAD_FINISHED" : \
  loadState == SvgLoader::LoadState::CANCELLED     ? "CANCELLED"     : \
  loadState == SvgLoader::LoadState::LOAD_FAILED   ? "LOAD_FAILED"   : \
                                                     "Unknown"

#define GET_RASTERIZE_STATE_STRING(rasterizeState) \
  rasterizeState == SvgLoader::RasterizeState::NOT_STARTED   ? "NOT_STARTED"   : \
  rasterizeState == SvgLoader::RasterizeState::RASTERIZING   ? "RASTERIZING"   : \
  rasterizeState == SvgLoader::RasterizeState::UPLOADED      ? "UPLOADED"      : \
  rasterizeState == SvgLoader::RasterizeState::UPLOAD_FAILED ? "UPLOAD_FAILED" : \
                                                               "Unknown"
// clang-format on
#endif

constexpr Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);

/**
 * @brief Helper function to set rasterize info from PixelData.
 *
 * @param[in] visualFactoryCache The visual factory cache. It will be used to get atlas manager faster.
 * @param[in] pixelData The rasterized pixel data.
 * @param[out] rasterizeInfo The rasterize info.
 */
void SetTextureSetToRasterizeInfo(VisualFactoryCache* visualFactoryCache, const Dali::PixelData rasterizedPixelData, SvgLoader::SvgRasterizeInfo& rasterizeInfo)
{
  rasterizeInfo.mAtlasAttempted = false;
  if(rasterizeInfo.mAttemptAtlasing)
  {
    // Try to use atlas attempt
    if(DALI_LIKELY(Dali::Adaptor::IsAvailable() && visualFactoryCache))
    {
      auto atlasManager = visualFactoryCache->GetAtlasManager();
      if(atlasManager)
      {
        Vector4 atlasRect;
        auto    textureSet = atlasManager->Add(atlasRect, rasterizedPixelData);
        if(textureSet) // atlasing
        {
          rasterizeInfo.mTextureSet     = textureSet;
          rasterizeInfo.mAtlasRect      = atlasRect;
          rasterizeInfo.mAtlasAttempted = true;

          DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Concise, "  rasterizeId:%d atlasAttempted:%d atlasRect:(%f %f %f %f)\n", rasterizeInfo.mId, rasterizeInfo.mAtlasAttempted, rasterizeInfo.mAtlasRect.x, rasterizeInfo.mAtlasRect.y, rasterizeInfo.mAtlasRect.z, rasterizeInfo.mAtlasRect.w);
        }
      }
    }
  }

  if(!rasterizeInfo.mAtlasAttempted)
  {
    // Atlas failed. Convert pixelData to texture.
    Dali::Texture texture = Texture::New(Dali::TextureType::TEXTURE_2D, Pixel::RGBA8888, rasterizedPixelData.GetWidth(), rasterizedPixelData.GetHeight());
    texture.Upload(rasterizedPixelData);

    rasterizeInfo.mTextureSet = TextureSet::New();
    rasterizeInfo.mTextureSet.SetTexture(0u, texture);

    rasterizeInfo.mAtlasRect = FULL_TEXTURE_RECT;

    DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Concise, "  rasterizeId:%d atlasAttempted:%d rasterizedPixelSize:(%ux%u)\n", rasterizeInfo.mId, rasterizeInfo.mAtlasAttempted, rasterizedPixelData.GetWidth(), rasterizedPixelData.GetHeight());
  }

  rasterizeInfo.mRasterizeState = SvgLoader::RasterizeState::UPLOADED;
}

/**
 * @brief Helper function to create a textureSet and atlasRect from rasterize info.
 *
 * @param[in] rasterizeInfo The rasterize info.
 * @param[out] textureSet The textureSet.
 * @param[out] atlasRect The atlasRect.
 */
void GetTextureSetFromRasterizeInfo(const SvgLoader::SvgRasterizeInfo& rasterizeInfo, Dali::TextureSet& textureSet, Vector4& atlasRect)
{
  if(!rasterizeInfo.mAtlasAttempted)
  {
    atlasRect = FULL_TEXTURE_RECT;
    if(DALI_LIKELY(rasterizeInfo.mTextureSet && rasterizeInfo.mTextureSet.GetTextureCount() > 0u))
    {
      auto texture = rasterizeInfo.mTextureSet.GetTexture(0u);
      if(DALI_LIKELY(texture))
      {
        // Always create new TextureSet here, so we don't share same TextureSets for multiple visuals.
        textureSet = Dali::TextureSet::New();
        textureSet.SetTexture(0u, texture);
      }
    }
  }
  else
  {
    textureSet = rasterizeInfo.mTextureSet;
    atlasRect  = rasterizeInfo.mAtlasRect;
  }
}

} // Anonymous namespace

SvgLoader::SvgLoader()
: mFactoryCache(nullptr),
  mCurrentSvgLoadId(0),
  mCurrentSvgRasterizeId(0),
  mLoadingQueueLoadId(SvgLoader::INVALID_SVG_LOAD_ID),
  mRasterizingQueueRasterizeId(SvgLoader::INVALID_SVG_RASTERIZE_ID),
  mRemoveProcessorRegistered(false)
{
}

SvgLoader::~SvgLoader()
{
  if(mRemoveProcessorRegistered && Adaptor::IsAvailable())
  {
    Adaptor::Get().UnregisterProcessorOnce(*this, true);
    mRemoveProcessorRegistered = false;
  }
}

SvgLoader::SvgLoadId SvgLoader::GenerateUniqueSvgLoadId()
{
  // Skip invalid id generation.
  if(DALI_UNLIKELY(mCurrentSvgLoadId == SvgLoader::INVALID_SVG_LOAD_ID))
  {
    mCurrentSvgLoadId = 0;
  }
  return mCurrentSvgLoadId++;
}

SvgLoader::SvgRasterizeId SvgLoader::GenerateUniqueSvgRasterizeId()
{
  // Skip invalid id generation.
  if(DALI_UNLIKELY(mCurrentSvgRasterizeId == SvgLoader::INVALID_SVG_RASTERIZE_ID))
  {
    mCurrentSvgRasterizeId = 0;
  }
  return mCurrentSvgRasterizeId++;
}

SvgLoader::SvgLoadId SvgLoader::Load(const VisualUrl& url, float dpi, SvgLoaderObserver* svgObserver, bool synchronousLoading)
{
  SvgLoadId loadId     = SvgLoader::INVALID_SVG_LOAD_ID;
  auto      cacheIndex = FindCacheIndexFromLoadCache(url, dpi);

  // Newly append cache now.
  if(cacheIndex == SvgLoader::INVALID_SVG_CACHE_INDEX)
  {
    loadId     = GenerateUniqueSvgLoadId();
    cacheIndex = static_cast<SvgCacheIndex>(static_cast<uint32_t>(mLoadCache.size()));
    mLoadCache.push_back(SvgLoadInfo(loadId, url, dpi));

    if(url.IsBufferResource())
    {
      // Make encoded image buffer url valid until this SvgLoadInfo alive.
      if(DALI_LIKELY(Dali::Adaptor::IsAvailable() && mFactoryCache))
      {
        auto& textureManager = mFactoryCache->GetTextureManager();
        textureManager.UseExternalResource(url);
      }
    }
    DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::General, "SvgLoader::Load( url=%s dpi=%f observer=%p ) New cached index:%d loadId@%d\n", url.GetUrl().c_str(), dpi, svgObserver, cacheIndex, loadId);
  }
  else
  {
    DALI_ASSERT_ALWAYS(static_cast<size_t>(cacheIndex) < mLoadCache.size() && "Invalid cache index");
    loadId = mLoadCache[cacheIndex].mId;
    ++mLoadCache[cacheIndex].mReferenceCount;
    DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::General, "SvgLoader::Load( url=%s dpi=%f observer=%p ) Using cached index:%d loadId@%d\n", url.GetUrl().c_str(), dpi, svgObserver, cacheIndex, loadId);
  }

  auto& loadInfo = mLoadCache[cacheIndex];

  DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::General, "SvgLoader::Load info id:%d, state:%s, refCount=%d\n", loadInfo.mId, GET_LOAD_STATE_STRING(loadInfo.mLoadState), static_cast<int>(loadInfo.mReferenceCount));

  switch(loadInfo.mLoadState)
  {
    case LoadState::LOAD_FAILED: // Failed notifies observer which then stops observing.
    case LoadState::NOT_STARTED:
    {
      if(synchronousLoading)
      {
        // Do not add observer for sync load case.
        LoadSynchronously(loadInfo, svgObserver);
      }
      else
      {
        LoadOrQueue(loadInfo, svgObserver);
      }
      break;
    }
    case LoadState::LOAD_FINISHED:
    {
      if(synchronousLoading || (mLoadingQueueLoadId == SvgLoader::INVALID_SVG_LOAD_ID && mRasterizingQueueRasterizeId == SvgLoader::INVALID_SVG_RASTERIZE_ID))
      {
        // Already load finished. Notify observer.
        if(svgObserver)
        {
          svgObserver->LoadComplete(loadId, loadInfo.mLoadState == LoadState::LOAD_FINISHED ? loadInfo.mVectorImageRenderer : Dali::VectorImageRenderer());
        }
      }
      else
      {
        // We should not notify observer yet. Queue it.
        if(svgObserver)
        {
          LoadOrQueue(loadInfo, svgObserver);
        }
      }
      break;
    }
    case LoadState::CANCELLED:
    {
      // A cancelled svg hasn't finished loading yet. Treat as a loading svg
      // (it's ref count has already been incremented, above)
      loadInfo.mLoadState = LoadState::LOADING;
      DALI_FALLTHROUGH;
    }
    case LoadState::LOADING:
    {
      if(synchronousLoading)
      {
        // Do not add observer for sync load case.
        // And also, we should not notify another observers even if we have already loaded.
        LoadSynchronously(loadInfo, svgObserver);
      }
      else
      {
        AddLoadObserver(loadInfo, svgObserver);
      }
      break;
    }
  }

  return loadId;
}

SvgLoader::SvgRasterizeId SvgLoader::Rasterize(SvgLoadId loadId, uint32_t width, uint32_t height, bool attemptAtlasing, SvgLoaderObserver* svgObserver, bool synchronousLoading)
{
  if(loadId == SvgLoader::INVALID_SVG_LOAD_ID)
  {
    return SvgLoader::INVALID_SVG_RASTERIZE_ID;
  }

  SvgRasterizeId rasterizeId = SvgLoader::INVALID_SVG_RASTERIZE_ID;
  auto           cacheIndex  = FindCacheIndexFromRasterizeCache(loadId, width, height, attemptAtlasing);

  // Newly append cache now.
  if(cacheIndex == SvgLoader::INVALID_SVG_CACHE_INDEX)
  {
    // Increase loadId reference first
    // It would be decreased at rasterizate removal.
    {
      auto loadCacheIndex = GetCacheIndexFromLoadCacheById(loadId);
      DALI_ASSERT_ALWAYS(loadCacheIndex != SvgLoader::INVALID_SVG_CACHE_INDEX && "Invalid cache index");
      ++mLoadCache[loadCacheIndex].mReferenceCount;
      DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::General, "SvgLoader::Rasterize( loadId=%d Size=%ux%u atlas=%d observer=%p ) Increase loadId loadState:%s, refCount=%d\n", loadId, width, height, attemptAtlasing, svgObserver, GET_LOAD_STATE_STRING(mLoadCache[loadCacheIndex].mLoadState), static_cast<int>(mLoadCache[loadCacheIndex].mReferenceCount));
    }

    rasterizeId = GenerateUniqueSvgRasterizeId();
    cacheIndex  = static_cast<SvgCacheIndex>(mRasterizeCache.size());
    mRasterizeCache.push_back(SvgRasterizeInfo(rasterizeId, loadId, width, height, attemptAtlasing));
    DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::General, "SvgLoader::Rasterize( loadId=%d Size=%ux%u atlas=%d observer=%p ) New cached index:%d rasterizeId@%d\n", loadId, width, height, attemptAtlasing, svgObserver, cacheIndex, rasterizeId);
  }
  else
  {
    DALI_ASSERT_ALWAYS(static_cast<size_t>(cacheIndex) < mRasterizeCache.size() && "Invalid cache index");
    rasterizeId = mRasterizeCache[cacheIndex].mId;
    ++mRasterizeCache[cacheIndex].mReferenceCount;
    DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::General, "SvgLoader::Rasterize( loadId=%d Size=%ux%u atlas=%d observer=%p ) Use cached index:%d rasterizeId@%d\n", loadId, width, height, attemptAtlasing, svgObserver, cacheIndex, rasterizeId);
  }

  auto& rasterizeInfo = mRasterizeCache[cacheIndex];

  DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::General, "SvgLoader::Rasterize info id:%d, state:%s, refCount=%d\n", rasterizeInfo.mId, GET_RASTERIZE_STATE_STRING(rasterizeInfo.mRasterizeState), static_cast<int>(rasterizeInfo.mReferenceCount));

  switch(rasterizeInfo.mRasterizeState)
  {
    case RasterizeState::UPLOAD_FAILED: // Failed notifies observer which then stops observing.
    case RasterizeState::NOT_STARTED:
    {
      if(synchronousLoading)
      {
        // Do not add observer for sync load case.
        RasterizeSynchronously(rasterizeInfo, svgObserver);
      }
      else
      {
        RasterizeOrQueue(rasterizeInfo, svgObserver);
      }
      break;
    }
    case RasterizeState::UPLOADED:
    {
      if(synchronousLoading || (mLoadingQueueLoadId == SvgLoader::INVALID_SVG_LOAD_ID && mRasterizingQueueRasterizeId == SvgLoader::INVALID_SVG_RASTERIZE_ID))
      {
        // Already upload finished. Notify observer.
        if(svgObserver)
        {
          Dali::TextureSet textureSet;
          Vector4          atlasRect = FULL_TEXTURE_RECT;
          if(rasterizeInfo.mRasterizeState == RasterizeState::UPLOADED)
          {
            GetTextureSetFromRasterizeInfo(rasterizeInfo, textureSet, atlasRect);
          }
          svgObserver->RasterizeComplete(rasterizeId, textureSet, atlasRect);
        }
      }
      else
      {
        // We should not notify observer yet. Queue it.
        if(svgObserver)
        {
          RasterizeOrQueue(rasterizeInfo, svgObserver);
        }
      }
      break;
    }
    case RasterizeState::RASTERIZING:
    {
      if(synchronousLoading)
      {
        // Do not add observer for sync load case.
        // And also, we should not notify another observers even if we have already loaded.
        RasterizeSynchronously(rasterizeInfo, svgObserver);
      }
      else
      {
        AddRasterizeObserver(rasterizeInfo, svgObserver);
      }
      break;
    }
  }

  return rasterizeId;
}

void SvgLoader::RequestLoadRemove(SvgLoader::SvgLoadId loadId, SvgLoaderObserver* svgObserver)
{
  // Remove observer first
  auto cacheIndex = GetCacheIndexFromLoadCacheById(loadId);
  if(cacheIndex == SvgLoader::INVALID_SVG_CACHE_INDEX)
  {
    return;
  }
  DALI_ASSERT_ALWAYS(static_cast<size_t>(cacheIndex) < mLoadCache.size() && "Invalid cache index");

  auto& loadInfo = mLoadCache[cacheIndex];
  RemoveLoadObserver(loadInfo, svgObserver);
  mLoadRemoveQueue.push_back(loadId);

  if(!mRemoveProcessorRegistered && Adaptor::IsAvailable())
  {
    mRemoveProcessorRegistered = true;
    Adaptor::Get().RegisterProcessorOnce(*this, true);
  }
}

void SvgLoader::RequestRasterizeRemove(SvgLoader::SvgRasterizeId rasterizeId, SvgLoaderObserver* svgObserver, bool removalSynchronously)
{
  // Remove observer first
  auto cacheIndex = GetCacheIndexFromRasterizeCacheById(rasterizeId);
  if(cacheIndex == SvgLoader::INVALID_SVG_CACHE_INDEX)
  {
    return;
  }
  DALI_ASSERT_ALWAYS(static_cast<size_t>(cacheIndex) < mRasterizeCache.size() && "Invalid cache index");

  auto& rasterizeInfo = mRasterizeCache[cacheIndex];
  RemoveRasterizeObserver(rasterizeInfo, svgObserver);

  // Should not remove rasterize info if we are in notify observing.
  if(removalSynchronously && (mLoadingQueueLoadId == SvgLoader::INVALID_SVG_LOAD_ID && mRasterizingQueueRasterizeId == SvgLoader::INVALID_SVG_RASTERIZE_ID))
  {
    RemoveRasterize(rasterizeId);
  }
  else
  {
    mRasterizeRemoveQueue.push_back(rasterizeId);

    if(!mRemoveProcessorRegistered && Adaptor::IsAvailable())
    {
      mRemoveProcessorRegistered = true;
      Adaptor::Get().RegisterProcessorOnce(*this, true);
    }
  }
}

Dali::VectorImageRenderer SvgLoader::GetVectorImageRenderer(SvgLoader::SvgLoadId loadId) const
{
  auto cacheIndex = GetCacheIndexFromLoadCacheById(loadId);
  if(cacheIndex != SvgLoader::INVALID_SVG_CACHE_INDEX)
  {
    DALI_ASSERT_ALWAYS(static_cast<size_t>(cacheIndex) < mLoadCache.size() && "Invalid cache index");
    return mLoadCache[cacheIndex].mVectorImageRenderer;
  }
  return Dali::VectorImageRenderer();
}

void SvgLoader::Process(bool postProcessor)
{
  DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_SVG_LOADER_PROCESS_REMOVE_QUEUE", [&](std::ostringstream& oss) {
    oss << "[r:" << mRasterizeRemoveQueue.size() << ", l:" << mLoadRemoveQueue.size() << "]";
  });

  mRemoveProcessorRegistered = false;

  for(auto& iter : mRasterizeRemoveQueue)
  {
    RemoveRasterize(iter);
  }
  mRasterizeRemoveQueue.clear();

  for(auto& iter : mLoadRemoveQueue)
  {
    RemoveLoad(iter);
  }
  mLoadRemoveQueue.clear();

  DALI_TRACE_END(gTraceFilter, "DALI_SVG_LOADER_PROCESS_REMOVE_QUEUE");
}

SvgLoader::SvgCacheIndex SvgLoader::GetCacheIndexFromLoadCacheById(const SvgLoader::SvgLoadId loadId) const
{
  const uint32_t size = static_cast<uint32_t>(mLoadCache.size());

  for(uint32_t i = 0; i < size; ++i)
  {
    if(mLoadCache[i].mId == loadId)
    {
      return static_cast<SvgCacheIndex>(i);
    }
  }
  return SvgLoader::INVALID_SVG_CACHE_INDEX;
}

SvgLoader::SvgCacheIndex SvgLoader::GetCacheIndexFromRasterizeCacheById(const SvgLoader::SvgRasterizeId rasterizeId) const
{
  const uint32_t size = static_cast<uint32_t>(mRasterizeCache.size());

  for(uint32_t i = 0; i < size; ++i)
  {
    if(mRasterizeCache[i].mId == rasterizeId)
    {
      return static_cast<SvgCacheIndex>(i);
    }
  }
  return SvgLoader::INVALID_SVG_CACHE_INDEX;
}

SvgLoader::SvgCacheIndex SvgLoader::FindCacheIndexFromLoadCache(const VisualUrl& imageUrl, float dpi) const
{
  const uint32_t size = static_cast<uint32_t>(mLoadCache.size());

  // TODO : Let we use hash in future. For now, just PoC

  for(uint32_t i = 0; i < size; ++i)
  {
    if(mLoadCache[i].mImageUrl.GetUrl() == imageUrl.GetUrl() &&
       Dali::Equals(mLoadCache[i].mDpi, dpi))
    {
      return static_cast<SvgCacheIndex>(i);
    }
  }
  return SvgLoader::INVALID_SVG_CACHE_INDEX;
}

SvgLoader::SvgCacheIndex SvgLoader::FindCacheIndexFromRasterizeCache(const SvgLoadId loadId, uint32_t width, uint32_t height, bool attemptAtlasing) const
{
  const uint32_t size = static_cast<uint32_t>(mRasterizeCache.size());

  // TODO : Let we use hash in future. For now, just PoC

  for(uint32_t i = 0; i < size; ++i)
  {
    if(mRasterizeCache[i].mLoadId == loadId &&
       mRasterizeCache[i].mWidth == width &&
       mRasterizeCache[i].mHeight == height)
    {
      // 1. If attemptAtlasing is true, then rasterizeInfo.mAttemptAtlasing should be true. The atlas rect result can be different.
      // 2. If attemptAtlasing is false, then rasterizeInfo.mAtlasAttempted should be false. (We can use attempt failed result even if mAttemptAtlasing is true.)
      if((attemptAtlasing && mRasterizeCache[i].mAttemptAtlasing) ||
         (!attemptAtlasing && !mRasterizeCache[i].mAtlasAttempted))
      {
        return static_cast<SvgCacheIndex>(i);
      }
    }
  }
  return SvgLoader::INVALID_SVG_CACHE_INDEX;
}

void SvgLoader::RemoveLoad(SvgLoader::SvgLoadId loadId)
{
  auto cacheIndex = GetCacheIndexFromLoadCacheById(loadId);
  if(cacheIndex != SvgLoader::INVALID_SVG_CACHE_INDEX)
  {
    DALI_ASSERT_ALWAYS(static_cast<size_t>(cacheIndex) < mLoadCache.size() && "Invalid cache index");

    auto& loadInfo(mLoadCache[cacheIndex]);

    --loadInfo.mReferenceCount;
    DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::General, "SvgLoader::RemoveLoad( url=%s dpi=%f ) cached index:%d loadId@%d, state:%s, refCount=%d\n", loadInfo.mImageUrl.GetUrl().c_str(), loadInfo.mDpi, cacheIndex, loadId, GET_LOAD_STATE_STRING(loadInfo.mLoadState), static_cast<int>(loadInfo.mReferenceCount));

    if(loadInfo.mReferenceCount <= 0)
    {
      if(loadInfo.mLoadState == LoadState::LOADING)
      {
        // Keep the load info in the cache, but mark it as cancelled.
        // It will be removed when async load completed.
        loadInfo.mLoadState = LoadState::CANCELLED;
      }
      else
      {
        if(loadInfo.mImageUrl.IsBufferResource())
        {
          // Unreference image buffer url from texture manager.
          if(DALI_LIKELY(Dali::Adaptor::IsAvailable() && mFactoryCache))
          {
            auto& textureManager = mFactoryCache->GetTextureManager();
            textureManager.RemoveEncodedImageBuffer(loadInfo.mImageUrl);
          }
        }

        // Remove the load info from the cache.
        // Swap last data of cacheContainer.
        if(static_cast<std::size_t>(cacheIndex + 1) < mLoadCache.size())
        {
          // Swap the value between current data and last data.
          std::swap(mLoadCache[cacheIndex], mLoadCache.back());
        }

        // Now we can assume that latest data should be removed. pop_back.
        mLoadCache.pop_back();

        // Now, loadInfo is invalid
      }
    }
  }
}

void SvgLoader::RemoveRasterize(SvgLoader::SvgRasterizeId rasterizeId)
{
  auto cacheIndex = GetCacheIndexFromRasterizeCacheById(rasterizeId);
  if(cacheIndex != SvgLoader::INVALID_SVG_CACHE_INDEX)
  {
    DALI_ASSERT_ALWAYS(static_cast<size_t>(cacheIndex) < mRasterizeCache.size() && "Invalid cache index");

    auto& rasterizeInfo(mRasterizeCache[cacheIndex]);

    --rasterizeInfo.mReferenceCount;
    DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::General, "SvgLoader::RemoveRasterize( loadId=%d Size=%ux%u ) cached index:%d rasterizeId@%d, state:%s, refCount=%d\n", rasterizeInfo.mLoadId, rasterizeInfo.mWidth, rasterizeInfo.mHeight, cacheIndex, rasterizeId, GET_RASTERIZE_STATE_STRING(rasterizeInfo.mRasterizeState), static_cast<int>(rasterizeInfo.mReferenceCount));

    if(rasterizeInfo.mReferenceCount <= 0)
    {
      // Reduce the reference count of LoadId first.
      RemoveLoad(rasterizeInfo.mLoadId);

      if(rasterizeInfo.mRasterizeState == RasterizeState::RASTERIZING && rasterizeInfo.mTask)
      {
        // Cancel rasterize task immediatly!
        Dali::AsyncTaskManager::Get().RemoveTask(rasterizeInfo.mTask);
      }

      if(Dali::Adaptor::IsAvailable() && rasterizeInfo.mAtlasAttempted && mFactoryCache)
      {
        // Remove the atlas from the atlas manager.
        auto atlasManager = mFactoryCache->GetAtlasManager();
        if(atlasManager)
        {
          atlasManager->Remove(rasterizeInfo.mTextureSet, rasterizeInfo.mAtlasRect);
        }
      }

      // Remove the rasterize info from the cache.
      // Swap last data of cacheContainer.
      if(static_cast<std::size_t>(cacheIndex + 1) < mRasterizeCache.size())
      {
        // Swap the value between current data and last data.
        std::swap(mRasterizeCache[cacheIndex], mRasterizeCache.back());
      }

      // Now we can assume that latest data should be removed. pop_back.
      mRasterizeCache.pop_back();

      // Now, rasterize is invalid
    }
  }
}

// Internal Methods for Load

void SvgLoader::LoadOrQueue(SvgLoader::SvgLoadInfo& loadInfo, SvgLoaderObserver* svgObserver)
{
  if(mLoadingQueueLoadId != SvgLoader::INVALID_SVG_LOAD_ID || mRasterizingQueueRasterizeId != SvgLoader::INVALID_SVG_RASTERIZE_ID)
  {
    mLoadQueue.PushBack(LoadQueueElement(loadInfo.mId, svgObserver));
    if(svgObserver)
    {
      DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Verbose, "  (Load)Connect DestructionSignal to observer:%p\n", svgObserver);
      svgObserver->LoadDestructionSignal().Connect(this, &SvgLoader::LoadObserverDestroyed);
    }
  }
  else
  {
    LoadRequest(loadInfo, svgObserver);
  }
}

void SvgLoader::LoadRequest(SvgLoader::SvgLoadInfo& loadInfo, SvgLoaderObserver* svgObserver)
{
  DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Concise, "SvgLoader::LoadRequest(): id:%d observer:%p\n", loadInfo.mId, svgObserver);
  AddLoadObserver(loadInfo, svgObserver);
  loadInfo.mLoadState = LoadState::LOADING;

  EncodedImageBuffer encodedImageBuffer;
  if(loadInfo.mImageUrl.IsBufferResource())
  {
    // Make encoded image buffer url valid until this SvgLoadInfo alive.
    if(DALI_LIKELY(Dali::Adaptor::IsAvailable() && mFactoryCache))
    {
      auto& textureManager = mFactoryCache->GetTextureManager();
      encodedImageBuffer   = textureManager.GetEncodedImageBuffer(loadInfo.mImageUrl);
    }
  }

  loadInfo.mTask = new SvgLoadingTask(loadInfo.mVectorImageRenderer, loadInfo.mId, loadInfo.mImageUrl, encodedImageBuffer, loadInfo.mDpi, MakeCallback(this, &SvgLoader::AsyncLoadComplete));

  Dali::AsyncTaskManager::Get().AddTask(loadInfo.mTask);
}

void SvgLoader::LoadSynchronously(SvgLoader::SvgLoadInfo& loadInfo, SvgLoaderObserver* svgObserver)
{
  DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Concise, "SvgLoader::LoadSynchronously(): id:%d observer:%p\n", loadInfo.mId, svgObserver);

  EncodedImageBuffer encodedImageBuffer;
  if(loadInfo.mImageUrl.IsBufferResource())
  {
    // Make encoded image buffer url valid until this SvgLoadInfo alive.
    if(DALI_LIKELY(Dali::Adaptor::IsAvailable() && mFactoryCache))
    {
      auto& textureManager = mFactoryCache->GetTextureManager();
      encodedImageBuffer   = textureManager.GetEncodedImageBuffer(loadInfo.mImageUrl);
    }
  }

  // Note, we will not store this task after this API called.
  SvgTaskPtr loadingTask = new SvgLoadingTask(loadInfo.mVectorImageRenderer, loadInfo.mId, loadInfo.mImageUrl, encodedImageBuffer, loadInfo.mDpi, nullptr);
  loadingTask->Process();
  if(!loadingTask->HasSucceeded())
  {
    loadInfo.mLoadState = LoadState::LOAD_FAILED;
  }
  else
  {
    loadInfo.mLoadState = LoadState::LOAD_FINISHED;
  }

  if(svgObserver)
  {
    svgObserver->LoadComplete(loadInfo.mId, loadInfo.mLoadState == LoadState::LOAD_FINISHED ? loadInfo.mVectorImageRenderer : Dali::VectorImageRenderer());
  }
}

void SvgLoader::AddLoadObserver(SvgLoader::SvgLoadInfo& loadInfo, SvgLoaderObserver* svgObserver)
{
  DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Concise, "SvgLoader::AddLoadObserver(): id:%d observer:%p\n", loadInfo.mId, svgObserver);

  if(svgObserver)
  {
    loadInfo.mObservers.PushBack(svgObserver);

    DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Verbose, "  (Load)Connect DestructionSignal to observer:%p\n", svgObserver);
    svgObserver->LoadDestructionSignal().Connect(this, &SvgLoader::LoadObserverDestroyed);
  }
}

void SvgLoader::RemoveLoadObserver(SvgLoader::SvgLoadInfo& loadInfo, SvgLoaderObserver* svgObserver)
{
  if(svgObserver)
  {
    const auto iterEnd = loadInfo.mObservers.End();
    const auto iter    = std::find(loadInfo.mObservers.Begin(), iterEnd, svgObserver);
    if(iter != iterEnd)
    {
      // Disconnect and remove the observer.
      DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Verbose, "  (Load)Disconnect DestructionSignal to observer:%p\n", svgObserver);
      svgObserver->LoadDestructionSignal().Disconnect(this, &SvgLoader::LoadObserverDestroyed);
      loadInfo.mObservers.Erase(iter);
    }
    else
    {
      // Given loadId might exist at load queue.
      // Remove observer from the LoadQueue
      for(auto&& element : mLoadQueue)
      {
        if(element.first == loadInfo.mId && element.second == svgObserver)
        {
          DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Verbose, "Remove observer from load queue (loadId:%d, observer:%p)\n", element.first, element.second);
          DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Verbose, "  (Load)Disconnect DestructionSignal to observer:%p\n", svgObserver);
          svgObserver->LoadDestructionSignal().Disconnect(this, &SvgLoader::LoadObserverDestroyed);
          element.second = nullptr;
          break;
        }
      }
    }
  }
}

void SvgLoader::ProcessLoadQueue()
{
  for(auto&& element : mLoadQueue)
  {
    if(element.first == SvgLoader::INVALID_SVG_LOAD_ID)
    {
      continue;
    }

    auto cacheIndex = GetCacheIndexFromLoadCacheById(element.first);
    if(cacheIndex != SvgLoader::INVALID_SVG_CACHE_INDEX)
    {
      DALI_ASSERT_ALWAYS(static_cast<size_t>(cacheIndex) < mLoadCache.size() && "Invalid cache index");

      SvgLoadInfo& loadInfo(mLoadCache[cacheIndex]);
      const auto   loadId      = element.first;
      auto*        svgObserver = element.second;

      DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::General, "SvgLoader::ProcessLoadQueue() loadId=%d, observer=%p, cacheIndex=@%d, loadState:%s\n", loadId, svgObserver, cacheIndex, GET_LOAD_STATE_STRING(loadInfo.mLoadState));

      if((loadInfo.mLoadState == LoadState::LOAD_FINISHED) ||
         (loadInfo.mLoadState == LoadState::LOAD_FAILED))
      {
        if(svgObserver)
        {
          DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Verbose, "  (Load)Disconnect DestructionSignal to observer:%p\n", svgObserver);
          svgObserver->LoadDestructionSignal().Disconnect(this, &SvgLoader::LoadObserverDestroyed);

          svgObserver->LoadComplete(loadId, loadInfo.mLoadState == LoadState::LOAD_FINISHED ? loadInfo.mVectorImageRenderer : Dali::VectorImageRenderer());
        }
      }
      else if(loadInfo.mLoadState == LoadState::LOADING)
      {
        // Note : LOADING state texture cannot be queue.
        // This case be occured when same load id are queue in mLoadQueue.
        AddLoadObserver(loadInfo, svgObserver);
      }
      else
      {
        LoadRequest(loadInfo, svgObserver);
      }
    }
  }

  mLoadQueue.Clear();
}

void SvgLoader::NotifyLoadObservers(SvgLoader::SvgLoadInfo& loadInfo)
{
  SvgLoadId loadId = loadInfo.mId;

  // Empty handle if load failed
  Dali::VectorImageRenderer vectorImageRenderer = loadInfo.mLoadState == LoadState::LOAD_FINISHED ? loadInfo.mVectorImageRenderer : Dali::VectorImageRenderer();

  // If there is an observer: Notify the load is complete, whether successful or not,
  // and erase it from the list
  SvgLoadInfo* info = &loadInfo;

  mLoadingQueueLoadId = loadInfo.mId;

  // Reverse observer list that we can pop_back the observer.
  std::reverse(info->mObservers.Begin(), info->mObservers.End());

  while(info->mObservers.Count())
  {
    SvgLoaderObserver* observer = *(info->mObservers.End() - 1u);

    // During LoadComplete() a Control ResourceReady() signal is emitted.
    // During that signal the app may add remove /add SvgLoad (e.g. via
    // ImageViews).
    // It is possible for observers to be removed from the observer list,
    // and it is also possible for the mLoadCache to be modified,
    // invalidating the reference to the SvgLoadInfo struct.
    // Texture load requests for the same URL are deferred until the end of this
    // method.
    DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Concise, "SvgLoader::NotifyLoadObservers() observer:%p loadId:%d url:%s loadState:%s\n", observer, loadId, info->mImageUrl.GetUrl().c_str(), GET_LOAD_STATE_STRING(info->mLoadState));

    // It is possible for the observer to be deleted.
    // Disconnect and remove the observer first.
    DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Verbose, "  (Load)Disconnect DestructionSignal to observer:%p\n", observer);
    observer->LoadDestructionSignal().Disconnect(this, &SvgLoader::LoadObserverDestroyed);

    info->mObservers.Erase(info->mObservers.End() - 1u);

    observer->LoadComplete(loadId, vectorImageRenderer);

    // Get the textureInfo from the container again as it may have been invalidated.
    auto cacheIndex = GetCacheIndexFromLoadCacheById(loadId);
    if(cacheIndex == SvgLoader::INVALID_SVG_CACHE_INDEX)
    {
      break; // load info has been removed - can stop.
    }
    DALI_ASSERT_ALWAYS(cacheIndex < mLoadCache.size() && "Invalid cache index");
    info = &mLoadCache[cacheIndex];
  }

  mLoadingQueueLoadId = SvgLoader::INVALID_SVG_LOAD_ID;

  ProcessLoadQueue();
  ProcessRasterizeQueue();
}

// Internal Methods for Rasterize

void SvgLoader::RasterizeOrQueue(SvgLoader::SvgRasterizeInfo& rasterizeInfo, SvgLoaderObserver* svgObserver)
{
  if(mLoadingQueueLoadId != SvgLoader::INVALID_SVG_LOAD_ID || mRasterizingQueueRasterizeId != SvgLoader::INVALID_SVG_RASTERIZE_ID)
  {
    mRasterizeQueue.PushBack(RasterizeQueueElement(rasterizeInfo.mId, svgObserver));
    if(svgObserver)
    {
      DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Verbose, "  (Rasterize)Connect DestructionSignal to observer:%p\n", svgObserver);
      svgObserver->RasterizeDestructionSignal().Connect(this, &SvgLoader::RasterizeObserverDestroyed);
    }
  }
  else
  {
    RasterizeRequest(rasterizeInfo, svgObserver);
  }
}

void SvgLoader::RasterizeRequest(SvgLoader::SvgRasterizeInfo& rasterizeInfo, SvgLoaderObserver* svgObserver)
{
  DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Concise, "SvgLoader::RasterizeRequest(): id:%d observer:%p\n", rasterizeInfo.mId, svgObserver);
  AddRasterizeObserver(rasterizeInfo, svgObserver);
  rasterizeInfo.mRasterizeState = RasterizeState::RASTERIZING;

  auto vectorImageRenderer = GetVectorImageRenderer(rasterizeInfo.mLoadId);

  SvgTaskPtr rasterizingTask = new SvgRasterizingTask(vectorImageRenderer, rasterizeInfo.mId, rasterizeInfo.mWidth, rasterizeInfo.mHeight, MakeCallback(this, &SvgLoader::AsyncRasterizeComplete));
#ifdef TRACE_ENABLED
  {
    SvgRasterizingTask* castedRasterizingTask = dynamic_cast<SvgRasterizingTask*>(rasterizingTask.Get());

    // It should not be nullptr, but we need to check nullptr to avoid SVACE false alarm.
    if(DALI_LIKELY(castedRasterizingTask))
    {
      auto loadCacheIndex = GetCacheIndexFromLoadCacheById(rasterizeInfo.mLoadId);
      castedRasterizingTask->SetUrl(mLoadCache[loadCacheIndex].mImageUrl);
    }
  }
#endif

  // Keep SvgTask at info.
  rasterizeInfo.mTask = std::move(rasterizingTask);

  Dali::AsyncTaskManager::Get().AddTask(rasterizeInfo.mTask);
}

void SvgLoader::RasterizeSynchronously(SvgLoader::SvgRasterizeInfo& rasterizeInfo, SvgLoaderObserver* svgObserver)
{
  DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Concise, "SvgLoader::RasterizeSynchronously(): id:%d observer:%p\n", rasterizeInfo.mId, svgObserver);

  auto vectorImageRenderer = GetVectorImageRenderer(rasterizeInfo.mLoadId);

  // Note, we will not store this task after this API called.
  SvgTaskPtr rasterizingTask = new SvgRasterizingTask(vectorImageRenderer, rasterizeInfo.mId, rasterizeInfo.mWidth, rasterizeInfo.mHeight, nullptr);
#ifdef TRACE_ENABLED
  {
    SvgRasterizingTask* castedRasterizingTask = dynamic_cast<SvgRasterizingTask*>(rasterizingTask.Get());

    // It should not be nullptr, but we need to check nullptr to avoid SVACE false alarm.
    if(DALI_LIKELY(castedRasterizingTask))
    {
      auto loadCacheIndex = GetCacheIndexFromLoadCacheById(rasterizeInfo.mLoadId);
      castedRasterizingTask->SetUrl(mLoadCache[loadCacheIndex].mImageUrl);
    }
  }
#endif
  rasterizingTask->Process();

  PixelData rasterizedPixelData = rasterizingTask->GetPixelData();

  if(!rasterizingTask->HasSucceeded() || !rasterizedPixelData)
  {
    rasterizeInfo.mRasterizeState = RasterizeState::UPLOAD_FAILED;
  }
  else if(rasterizeInfo.mRasterizeState != RasterizeState::UPLOADED) ///< Check it to avoid duplicate Upload call.
  {
    SetTextureSetToRasterizeInfo(mFactoryCache, rasterizedPixelData, rasterizeInfo);
  }

  if(svgObserver)
  {
    Dali::TextureSet textureSet;
    Vector4          atlasRect = FULL_TEXTURE_RECT;
    if(rasterizeInfo.mRasterizeState == RasterizeState::UPLOADED)
    {
      GetTextureSetFromRasterizeInfo(rasterizeInfo, textureSet, atlasRect);
    }
    svgObserver->RasterizeComplete(rasterizeInfo.mId, textureSet, atlasRect);
  }
}

void SvgLoader::AddRasterizeObserver(SvgLoader::SvgRasterizeInfo& rasterizeInfo, SvgLoaderObserver* svgObserver)
{
  DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Concise, "SvgLoader::AddRasterizeObserver(): id:%d observer:%p\n", rasterizeInfo.mId, svgObserver);

  if(svgObserver)
  {
    rasterizeInfo.mObservers.PushBack(svgObserver);

    DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Verbose, "  (Rasterize)Connect DestructionSignal to observer:%p\n", svgObserver);
    svgObserver->RasterizeDestructionSignal().Connect(this, &SvgLoader::RasterizeObserverDestroyed);
  }
}

void SvgLoader::RemoveRasterizeObserver(SvgLoader::SvgRasterizeInfo& rasterizeInfo, SvgLoaderObserver* svgObserver)
{
  if(svgObserver)
  {
    const auto iterEnd = rasterizeInfo.mObservers.End();
    const auto iter    = std::find(rasterizeInfo.mObservers.Begin(), iterEnd, svgObserver);
    if(iter != iterEnd)
    {
      // Disconnect and remove the observer.
      DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Verbose, "  (Rasterize)Disconnect DestructionSignal to observer:%p\n", svgObserver);
      svgObserver->RasterizeDestructionSignal().Disconnect(this, &SvgLoader::RasterizeObserverDestroyed);
      rasterizeInfo.mObservers.Erase(iter);
    }
    else
    {
      // Given rasterizeId might exist at rasterize queue.
      // Remove observer from the RasterizeQueue
      for(auto&& element : mRasterizeQueue)
      {
        if(element.first == rasterizeInfo.mId && element.second == svgObserver)
        {
          DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Verbose, "Remove observer from rasterize queue (rasterizeId:%d, observer:%p)\n", element.first, element.second);
          DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Verbose, "  (Rasterize)Disconnect DestructionSignal to observer:%p\n", svgObserver);
          svgObserver->RasterizeDestructionSignal().Disconnect(this, &SvgLoader::RasterizeObserverDestroyed);
          element.second = nullptr;
          break;
        }
      }
    }
  }
}

void SvgLoader::ProcessRasterizeQueue()
{
  for(auto&& element : mRasterizeQueue)
  {
    if(element.first == SvgLoader::INVALID_SVG_RASTERIZE_ID)
    {
      continue;
    }

    auto cacheIndex = GetCacheIndexFromRasterizeCacheById(element.first);
    if(cacheIndex != SvgLoader::INVALID_SVG_CACHE_INDEX)
    {
      DALI_ASSERT_ALWAYS(static_cast<size_t>(cacheIndex) < mRasterizeCache.size() && "Invalid cache index");

      SvgRasterizeInfo& rasterizeInfo(mRasterizeCache[cacheIndex]);
      const auto        rasterizeId = element.first;
      auto*             svgObserver = element.second;

      DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::General, "SvgLoader::ProcessRasterizeQueue() rasterizeId=%d, observer=%p, cacheIndex=@%d, rasterizeState:%s\n", rasterizeId, svgObserver, cacheIndex, GET_RASTERIZE_STATE_STRING(rasterizeInfo.mRasterizeState));

      if((rasterizeInfo.mRasterizeState == RasterizeState::UPLOADED) ||
         (rasterizeInfo.mRasterizeState == RasterizeState::UPLOAD_FAILED))
      {
        if(svgObserver)
        {
          DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Verbose, "  (Rasterize)Disconnect DestructionSignal to observer:%p\n", svgObserver);
          svgObserver->RasterizeDestructionSignal().Disconnect(this, &SvgLoader::RasterizeObserverDestroyed);

          Dali::TextureSet textureSet;
          Vector4          atlasRect = FULL_TEXTURE_RECT;
          if(rasterizeInfo.mRasterizeState == RasterizeState::UPLOADED)
          {
            GetTextureSetFromRasterizeInfo(rasterizeInfo, textureSet, atlasRect);
          }
          svgObserver->RasterizeComplete(rasterizeId, textureSet, atlasRect);
        }
      }
      else if(rasterizeInfo.mRasterizeState == RasterizeState::RASTERIZING)
      {
        // Note : RASTERIZING state texture cannot be queue.
        // This case be occured when same load id are queue in mRasterizeQueue.
        AddRasterizeObserver(rasterizeInfo, svgObserver);
      }
      else
      {
        RasterizeRequest(rasterizeInfo, svgObserver);
      }
    }
  }

  mRasterizeQueue.Clear();
}

void SvgLoader::NotifyRasterizeObservers(SvgLoader::SvgRasterizeInfo& rasterizeInfo)
{
  SvgRasterizeId rasterizeId = rasterizeInfo.mId;

  const bool rasterizationSuccess = rasterizeInfo.mRasterizeState == RasterizeState::UPLOADED;

  // If there is an observer: Notify the load is complete, whether successful or not,
  // and erase it from the list
  SvgRasterizeInfo* info = &rasterizeInfo;

  mRasterizingQueueRasterizeId = rasterizeInfo.mId;

  // Reverse observer list that we can pop_back the observer.
  std::reverse(info->mObservers.Begin(), info->mObservers.End());

  while(info->mObservers.Count())
  {
    SvgLoaderObserver* observer = *(info->mObservers.End() - 1u);

    // During LoadComplete() a Control ResourceReady() signal is emitted.
    // During that signal the app may add remove /add SvgLoad (e.g. via
    // ImageViews).
    // It is possible for observers to be removed from the observer list,
    // and it is also possible for the mLoadCache to be modified,
    // invalidating the reference to the SvgRasterizeInfo struct.
    // Texture load requests for the same URL are deferred until the end of this
    // method.
    DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Concise, "SvgLoader::NotifyRasterizeObservers() observer:%p rasterizeId:%d loaderId:%d atlasAttempted:%d Size:%ux%u rasterizestate:%s\n", observer, rasterizeId, info->mLoadId, info->mAtlasAttempted, info->mWidth, info->mHeight, GET_RASTERIZE_STATE_STRING(info->mRasterizeState));

    // It is possible for the observer to be deleted.
    // Disconnect and remove the observer first.
    DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Verbose, "  (Rasterize)Disconnect DestructionSignal to observer:%p\n", observer);
    observer->RasterizeDestructionSignal().Disconnect(this, &SvgLoader::RasterizeObserverDestroyed);

    info->mObservers.Erase(info->mObservers.End() - 1u);

    Dali::TextureSet textureSet;
    Vector4          atlasRect = FULL_TEXTURE_RECT;
    if(rasterizationSuccess)
    {
      GetTextureSetFromRasterizeInfo(rasterizeInfo, textureSet, atlasRect);
    }

    observer->RasterizeComplete(rasterizeId, textureSet, atlasRect);

    // Get the textureInfo from the container again as it may have been invalidated.
    auto cacheIndex = GetCacheIndexFromRasterizeCacheById(rasterizeId);
    if(cacheIndex == SvgLoader::INVALID_SVG_CACHE_INDEX)
    {
      break; // load info has been removed - can stop.
    }
    DALI_ASSERT_ALWAYS(cacheIndex < mRasterizeCache.size() && "Invalid cache index");
    info = &mRasterizeCache[cacheIndex];
  }

  mRasterizingQueueRasterizeId = SvgLoader::INVALID_SVG_RASTERIZE_ID;

  ProcessLoadQueue();
  ProcessRasterizeQueue();
}

/// From SvgLoadingTask
void SvgLoader::AsyncLoadComplete(SvgTaskPtr task)
{
  auto loadId     = task->GetId();
  auto cacheIndex = GetCacheIndexFromLoadCacheById(loadId);
  DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Concise, "SvgLoader::AsyncLoadComplete( loadId:%d CacheIndex:%u )\n", loadId, cacheIndex);
  if(cacheIndex != SvgLoader::INVALID_SVG_CACHE_INDEX)
  {
    DALI_ASSERT_ALWAYS(cacheIndex < mLoadCache.size() && "Invalid cache index");

    SvgLoadInfo& loadInfo(mLoadCache[cacheIndex]);

    DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Concise, "  loadId:%d Url:%s CacheIndex:%d LoadState: %s\n", loadInfo.mId, loadInfo.mImageUrl.GetUrl().c_str(), cacheIndex, GET_LOAD_STATE_STRING(loadInfo.mLoadState));

    if(loadInfo.mTask == task)
    {
      loadInfo.mTask.Reset();
    }

    if(loadInfo.mLoadState == LoadState::CANCELLED)
    {
      // Note : loadInfo can be invalidated after this call (as the mLoadCache may be modified)
      RemoveLoad(loadId);
    }
    else
    {
      if(!task->HasSucceeded())
      {
        loadInfo.mLoadState = LoadState::LOAD_FAILED;
      }
      else
      {
        loadInfo.mLoadState = LoadState::LOAD_FINISHED;
      }

      // Note : loadInfo can be invalidated after this call (as the mLoadCache may be modified)
      NotifyLoadObservers(loadInfo);
    }
  }
}

/// From SvgRasterizingTask
void SvgLoader::AsyncRasterizeComplete(SvgTaskPtr task)
{
  auto rasterizeId = task->GetId();
  auto cacheIndex  = GetCacheIndexFromRasterizeCacheById(rasterizeId);
  DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Concise, "SvgLoader::AsyncRasterizeComplete( rasterizedId:%d CacheIndex:%u )\n", rasterizeId, cacheIndex);
  if(cacheIndex != SvgLoader::INVALID_SVG_CACHE_INDEX)
  {
    DALI_ASSERT_ALWAYS(cacheIndex < mRasterizeCache.size() && "Invalid cache index");

    SvgRasterizeInfo& rasterizeInfo(mRasterizeCache[cacheIndex]);

    DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Concise, "  rasterizeId:%d loadId:%d attemptAtlasing:%d Size:%ux%u CacheIndex:%d RasterizeState: %s\n", rasterizeInfo.mId, rasterizeInfo.mLoadId, rasterizeInfo.mAttemptAtlasing, rasterizeInfo.mWidth, rasterizeInfo.mHeight, cacheIndex, GET_RASTERIZE_STATE_STRING(rasterizeInfo.mRasterizeState));

    if(rasterizeInfo.mTask == task)
    {
      rasterizeInfo.mTask.Reset();
    }

    PixelData rasterizedPixelData = task->GetPixelData();

    if(!task->HasSucceeded() || !rasterizedPixelData)
    {
      rasterizeInfo.mRasterizeState = RasterizeState::UPLOAD_FAILED;
    }
    else if(rasterizeInfo.mRasterizeState != RasterizeState::UPLOADED) ///< Check it to avoid duplicate Upload call. (e.g. sync rasterize)
    {
      SetTextureSetToRasterizeInfo(mFactoryCache, rasterizedPixelData, rasterizeInfo);
    }

    // Note : rasterizeInfo can be invalidated after this call (as the mRasterizeCache may be modified)
    NotifyRasterizeObservers(rasterizeInfo);
  }
}

/// From ~SvgVisual
void SvgLoader::LoadObserverDestroyed(SvgLoaderObserver* observer)
{
  DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Concise, "SvgLoader::LoadObserverDestroyed(): observer:%p\n", observer);

  for(auto&& loadInfo : mLoadCache)
  {
    for(auto iter = (loadInfo.mObservers.Begin()); iter != (loadInfo.mObservers.End());)
    {
      if(*iter == observer)
      {
        DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Concise, "  (Load)Remove observer from LoadCache (id:%d, observer:%p)\n", loadInfo.mId, observer);
        iter = loadInfo.mObservers.Erase(iter);
      }
      else
      {
        ++iter;
      }
    }
  }

  // Remove element from the LoadQueue
  for(auto&& element : mLoadQueue)
  {
    if(element.second == observer)
    {
      DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Verbose, "Remove observer from load queue (loadId:%d, observer:%p)\n", element.first, element.second);
      element.first  = SvgLoader::INVALID_SVG_LOAD_ID;
      element.second = nullptr;
    }
  }
}

void SvgLoader::RasterizeObserverDestroyed(SvgLoaderObserver* observer)
{
  DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Concise, "SvgLoader::RasterizeObserverDestroyed(): observer:%p\n", observer);

  for(auto&& rasterizeInfo : mRasterizeCache)
  {
    for(auto iter = (rasterizeInfo.mObservers.Begin()); iter != (rasterizeInfo.mObservers.End());)
    {
      if(*iter == observer)
      {
        DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Concise, "  (Rasterize)Remove observer from RasterizeCache (id:%d, observer:%p)\n", rasterizeInfo.mId, observer);
        iter = rasterizeInfo.mObservers.Erase(iter);
      }
      else
      {
        ++iter;
      }
    }
  }

  // Remove element from the RasterizeQueue
  for(auto&& element : mRasterizeQueue)
  {
    if(element.second == observer)
    {
      DALI_LOG_INFO(gSvgLoaderLogFilter, Debug::Verbose, "Remove observer from rasterize queue (rasterizeId:%d, observer:%p)\n", element.first, element.second);
      element.first  = SvgLoader::INVALID_SVG_RASTERIZE_ID;
      element.second = nullptr;
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

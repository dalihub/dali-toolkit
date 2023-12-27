/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali-scene3d/internal/common/image-resource-loader.h>

// EXTERNAL INCLUDES
#include <dali-toolkit/public-api/image-loader/sync-image-loader.h>
#include <dali/devel-api/adaptor-framework/lifecycle-controller.h>
#include <dali/devel-api/common/hash.h>
#include <dali/devel-api/common/map-wrapper.h>
#include <dali/devel-api/threading/mutex.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/connection-tracker.h>

#include <functional> ///< for std::function
#include <memory>     ///< for std::shared_ptr
#include <mutex>
#include <string>
#include <utility> ///< for std::pair

// INTERNAL INCLUDES

namespace
{
constexpr uint32_t MAXIMUM_COLLECTING_ITEM_COUNTS_PER_GC_CALL = 5u;
constexpr uint32_t GC_PERIOD_MILLISECONDS                     = 1000u;

#ifdef DEBUG_ENABLED
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_IMAGE_RESOURCE_LOADER");
#endif

struct ImageInformation
{
  ImageInformation(const std::string           url,
                   const Dali::ImageDimensions dimensions,
                   Dali::FittingMode::Type     fittingMode,
                   Dali::SamplingMode::Type    samplingMode,
                   bool                        orientationCorrection)
  : mUrl(url),
    mDimensions(dimensions),
    mFittingMode(fittingMode),
    mSamplingMode(samplingMode),
    mOrientationCorrection(orientationCorrection)
  {
  }

  bool operator==(const ImageInformation& rhs) const
  {
    // Check url and orientation correction is enough.
    return (mUrl == rhs.mUrl) && (mOrientationCorrection == rhs.mOrientationCorrection);
  }

  std::string              mUrl;
  Dali::ImageDimensions    mDimensions;
  Dali::FittingMode::Type  mFittingMode;
  Dali::SamplingMode::Type mSamplingMode;
  bool                     mOrientationCorrection;
};

// Hash functor list
std::size_t GenerateHash(const ImageInformation& info)
{
  std::vector<std::uint8_t> hashTarget;
  const uint16_t            width  = info.mDimensions.GetWidth();
  const uint16_t            height = info.mDimensions.GetHeight();

  // If either the width or height has been specified, include the resizing options in the hash
  if(width != 0 || height != 0)
  {
    // We are appending 5 bytes to the URL to form the hash input.
    hashTarget.resize(5u);
    std::uint8_t* hashTargetPtr = &(hashTarget[0u]);

    // Pack the width and height (4 bytes total).
    *hashTargetPtr++ = info.mDimensions.GetWidth() & 0xff;
    *hashTargetPtr++ = (info.mDimensions.GetWidth() >> 8u) & 0xff;
    *hashTargetPtr++ = info.mDimensions.GetHeight() & 0xff;
    *hashTargetPtr++ = (info.mDimensions.GetHeight() >> 8u) & 0xff;

    // Bit-pack the FittingMode, SamplingMode and orientation correction.
    // FittingMode=2bits, SamplingMode=3bits, orientationCorrection=1bit
    *hashTargetPtr = (info.mFittingMode << 4u) | (info.mSamplingMode << 1) | (info.mOrientationCorrection ? 1 : 0);
  }
  else
  {
    // We are not including sizing information, but we still need an extra byte for orientationCorrection.
    hashTarget.resize(1u);
    hashTarget[0u] = info.mOrientationCorrection ? 't' : 'f';
  }

  return Dali::CalculateHash(info.mUrl) ^ Dali::CalculateHash(hashTarget);
}

std::size_t GenerateHash(const Dali::PixelData& pixelData, bool mipmapRequired)
{
  return reinterpret_cast<std::size_t>(static_cast<void*>(pixelData.GetObjectPtr())) ^ (static_cast<std::size_t>(mipmapRequired) << (sizeof(std::size_t) * 4));
}

std::size_t GenerateHash(const std::vector<std::vector<Dali::PixelData>>& pixelDataList, bool mipmapRequired)
{
  std::size_t result = 0x12345678u + pixelDataList.size();
  for(const auto& mipmapPixelDataList : pixelDataList)
  {
    result += (result << 5) + mipmapPixelDataList.size();
    for(const auto& pixelData : mipmapPixelDataList)
    {
      result += (result << 5) + GenerateHash(pixelData, false);
    }
  }

  return result ^ (static_cast<std::size_t>(mipmapRequired) << (sizeof(std::size_t) * 4));
}

// Item Creation functor list

Dali::PixelData CreatePixelDataFromImageInfo(const ImageInformation& info, bool /* Not used */)
{
  return Dali::Toolkit::SyncImageLoader::Load(info.mUrl, info.mDimensions, info.mFittingMode, info.mSamplingMode, info.mOrientationCorrection);
}

Dali::Texture CreateTextureFromPixelData(const Dali::PixelData& pixelData, bool mipmapRequired)
{
  Dali::Texture texture;
  if(pixelData)
  {
    texture = Dali::Texture::New(Dali::TextureType::TEXTURE_2D, pixelData.GetPixelFormat(), pixelData.GetWidth(), pixelData.GetHeight());
    texture.Upload(pixelData, 0, 0, 0, 0, pixelData.GetWidth(), pixelData.GetHeight());
    if(mipmapRequired)
    {
      texture.GenerateMipmaps();
    }
  }
  return texture;
}

Dali::Texture CreateCubeTextureFromPixelDataList(const std::vector<std::vector<Dali::PixelData>>& pixelDataList, bool mipmapRequired)
{
  Dali::Texture texture;
  if(!pixelDataList.empty() && !pixelDataList[0].empty())
  {
    texture = Dali::Texture::New(Dali::TextureType::TEXTURE_CUBE, pixelDataList[0][0].GetPixelFormat(), pixelDataList[0][0].GetWidth(), pixelDataList[0][0].GetHeight());
    for(size_t iSide = 0u, iEndSize = pixelDataList.size(); iSide < iEndSize; ++iSide)
    {
      auto& side = pixelDataList[iSide];
      for(size_t iMipLevel = 0u, iEndMipLevel = pixelDataList[0].size(); iMipLevel < iEndMipLevel; ++iMipLevel)
      {
        texture.Upload(side[iMipLevel], Dali::CubeMapLayer::POSITIVE_X + iSide, iMipLevel, 0u, 0u, side[iMipLevel].GetWidth(), side[iMipLevel].GetHeight());
      }
    }
    if(mipmapRequired)
    {
      texture.GenerateMipmaps();
    }
  }

  return texture;
}

// Forward declare, for signal connection.
void DestroyCacheImpl();

class CacheImpl : public Dali::ConnectionTracker
{
public:
  /**
   * @brief Constructor
   */
  CacheImpl()
  : mPixelDataCache{},
    mTextureCache{},
    mCubeTextureCache{},
    mTimer{},
    mLatestCollectedPixelDataIter{mPixelDataCache.begin()},
    mLatestCollectedTextureIter{mTextureCache.begin()},
    mLatestCollectedCubeTextureIter{mCubeTextureCache.begin()},
    mPixelDataContainerUpdated{false},
    mTextureContainerUpdated{false},
    mCubeTextureContainerUpdated{false},
    mDataMutex{},
    mDestroyed{false},
    mFullCollectRequested{false}
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Create CacheImpl\n");

    // We should create CacheImpl at main thread, To ensure delete this cache impl
    Dali::LifecycleController::Get().TerminateSignal().Connect(DestroyCacheImpl);
  }

  /**
   * @brief Destructor
   */
  ~CacheImpl()
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Destroy CacheImpl\n");
    {
      mDataMutex.lock();

      mDestroyed                      = true;
      mPixelDataContainerUpdated      = false;
      mTextureContainerUpdated        = false;
      mCubeTextureContainerUpdated    = false;
      mLatestCollectedPixelDataIter   = decltype(mLatestCollectedPixelDataIter)();   // Invalidate iterator
      mLatestCollectedTextureIter     = decltype(mLatestCollectedTextureIter)();     // Invalidate iterator
      mLatestCollectedCubeTextureIter = decltype(mLatestCollectedCubeTextureIter){}; // Invalidate iterator

      mPixelDataCache.clear();
      mTextureCache.clear();
      mCubeTextureCache.clear();

      mDataMutex.unlock();
    }

    if(mTimer)
    {
      if(Dali::Adaptor::IsAvailable())
      {
        mTimer.Stop();
      }
    }
  }

private: // Unified API for this class
  // Let compare with hash first. And then, check detail keys after.
  using PixelDataCacheContainer   = std::map<std::size_t, std::vector<std::pair<ImageInformation, Dali::PixelData>>>;
  using TextureCacheContainer     = std::map<std::size_t, std::vector<std::pair<Dali::PixelData, Dali::Texture>>>;
  using CubeTextureCacheContainer = std::map<std::size_t, std::vector<std::pair<std::vector<std::vector<Dali::PixelData>>, Dali::Texture>>>;

  /**
   * @brief Try to get cached item, or create new handle if there is no item.
   *
   * @tparam needMutex Whether we need to lock the mutex during this operation, or not.
   * @param[in] cacheContainer The container of key / item pair.
   * @param[in] hashValue The hash value of key.
   * @param[in] key The key of cache item.
   * @param[in] keyFlag The additional flags when we need to create new item.
   * @param[out] containerUpdate True whether container changed or not.
   * @return Item that has been cached. Or newly created.
   */
  template<bool needMutex, typename KeyType, typename ItemType, ItemType (*ItemCreationFunction)(const KeyType&, bool), typename ContainerType>
  ItemType GetOrCreateCachedItem(ContainerType& cacheContainer, std::size_t hashValue, const KeyType& key, bool keyFlag, bool& containerUpdated)
  {
    if constexpr(needMutex)
    {
      mDataMutex.lock();
    }
    ItemType returnItem;

    if(DALI_LIKELY(!mDestroyed))
    {
      bool found = false;

      auto iter = cacheContainer.lower_bound(hashValue);
      if((iter == cacheContainer.end()) || (hashValue != iter->first))
      {
        containerUpdated = true;

        returnItem = ItemCreationFunction(key, keyFlag);
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Create new item\n");
        cacheContainer.insert(iter, {hashValue, {{key, returnItem}}});
      }
      else
      {
        auto& cachePairList = iter->second;
        for(auto jter = cachePairList.begin(), jterEnd = cachePairList.end(); jter != jterEnd; ++jter)
        {
          if(jter->first == key)
          {
            // We found that input pixelData already cached.
            returnItem = jter->second;
            DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Get cached item\n");
            found = true;
            break;
          }
        }

        // If we fail to found same list, just append.
        if(!found)
        {
          containerUpdated = true;

          returnItem = ItemCreationFunction(key, keyFlag);
          DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Create new item\n");
          cachePairList.emplace_back(key, returnItem);
        }
      }
    }

    if constexpr(needMutex)
    {
      mDataMutex.unlock();
    }

    return returnItem;
  }
  /**
   * @brief Try to collect garbages, which reference counts are 1.
   *
   * @tparam needMutex Whether we need to lock the mutex during this operation, or not.
   * @param[in] cacheContainer The container of key / item pair.
   * @param[in] fullCollect True if we need to collect whole container.
   * @param[in, out] containerUpdated True if container information changed. lastIterator will be begin of container when we start collect garbages.
   * @param[in, out] lastIterator The last iterator of container.
   * @oaram[in, out] checkedCount The number of iteration checked total.
   * @return True if we iterate whole container, so we don't need to check anymore. False otherwise
   */
  template<bool needMutex, typename ContainerType, typename Iterator = typename ContainerType::iterator>
  bool CollectGarbages(ContainerType& cacheContainer, bool fullCollect, bool& containerUpdated, Iterator& lastIterator, uint32_t& checkedCount)
  {
    if constexpr(needMutex)
    {
      mDataMutex.lock();
    }

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Collect Garbages : %zu\n", cacheContainer.size());
    // Container changed. We should re-collect garbage from begin again.
    if(fullCollect || containerUpdated)
    {
      lastIterator     = cacheContainer.begin();
      containerUpdated = false;
    }

    for(; lastIterator != cacheContainer.end() && (fullCollect || ++checkedCount <= MAXIMUM_COLLECTING_ITEM_COUNTS_PER_GC_CALL);)
    {
      auto& cachePairList = lastIterator->second;

      for(auto jter = cachePairList.begin(); jter != cachePairList.end();)
      {
        auto& item = jter->second;
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "item : %p, ref count : %u\n", item.GetObjectPtr(), (item ? item.GetBaseObject().ReferenceCount() : 0u));
        if(!item || (item.GetBaseObject().ReferenceCount() == 1u))
        {
          // This item is garbage! just remove it.
          jter = cachePairList.erase(jter);
        }
        else
        {
          ++jter;
        }
      }

      if(cachePairList.empty())
      {
        lastIterator = cacheContainer.erase(lastIterator);
      }
      else
      {
        ++lastIterator;
      }
    }

    if constexpr(needMutex)
    {
      mDataMutex.unlock();
    }

    return (lastIterator != cacheContainer.end());
  }

public: // Called by main thread.
  /**
   * @brief Try to get cached texture, or newly create if there is no texture that already cached.
   *
   * @param[in] pixelData The pixelData of image.
   * @param[in] mipmapRequired True if result texture need to generate mipmap.
   * @return Texture that has been cached. Or empty handle if we fail to found cached item.
   */
  Dali::Texture GetOrCreateCachedTexture(const Dali::PixelData& pixelData, bool mipmapRequired)
  {
    auto hashValue = GenerateHash(pixelData, mipmapRequired);
    return GetOrCreateCachedItem<false, Dali::PixelData, Dali::Texture, CreateTextureFromPixelData>(mTextureCache, hashValue, pixelData, mipmapRequired, mTextureContainerUpdated);
  }

  /**
   * @brief Try to get cached cube texture, or newly create if there is no cube texture that already cached.
   *
   * @param[in] pixelDataList The pixelData list of image.
   * @param[in] mipmapRequired True if result texture need to generate mipmap.
   * @return Texture that has been cached. Or empty handle if we fail to found cached item.
   */
  Dali::Texture GetOrCreateCachedCubeTexture(const std::vector<std::vector<Dali::PixelData>>& pixelDataList, bool mipmapRequired)
  {
    auto hashValue = GenerateHash(pixelDataList, mipmapRequired);
    return GetOrCreateCachedItem<false, std::vector<std::vector<Dali::PixelData>>, Dali::Texture, CreateCubeTextureFromPixelDataList>(mCubeTextureCache, hashValue, pixelDataList, mipmapRequired, mCubeTextureContainerUpdated);
  }

  /**
   * @brief Request incremental gargabe collect.
   *
   * @param[in] fullCollect True if we will collect whole items, or incrementally.
   */
  void RequestGarbageCollect(bool fullCollect)
  {
    if(DALI_LIKELY(Dali::Adaptor::IsAvailable()))
    {
      if(!mTimer)
      {
        mTimer = Dali::Timer::New(GC_PERIOD_MILLISECONDS);
        mTimer.TickSignal().Connect(this, &CacheImpl::OnTick);
      }

      mFullCollectRequested |= fullCollect;

      if(!mTimer.IsRunning())
      {
        // Restart container interating.
        if(!mPixelDataContainerUpdated)
        {
          mDataMutex.lock();
          mPixelDataContainerUpdated = true;
          mDataMutex.unlock();
        }
        mTextureContainerUpdated = true;
        mTimer.Start();
      }
    }
  }

public: // Can be called by worker thread
  /**
   * @brief Try to get cached pixel data, or newly create if there is no pixel data that already cached.
   *
   * @param[in] info The informations of image to load.
   * @return Texture that has been cached. Or empty handle if we fail to found cached item.
   */
  Dali::PixelData GetOrCreateCachedPixelData(const ImageInformation& info)
  {
    auto hashValue = GenerateHash(info);
    return GetOrCreateCachedItem<true, ImageInformation, Dali::PixelData, CreatePixelDataFromImageInfo>(mPixelDataCache, hashValue, info, false, mPixelDataContainerUpdated);
  }

private: // Called by main thread
  bool OnTick()
  {
    // Clear full GC flag
    const bool fullCollect = mFullCollectRequested;
    mFullCollectRequested  = false;

    return IncrementalGarbageCollect(fullCollect);
  }

  /**
   * @brief Remove unused cache item incrementally.
   *
   * @param[in] fullCollect True if we will collect whole items, or incrementally.
   * @return True if there still exist what we need to check clean. False when whole cached items are using now.
   */
  bool IncrementalGarbageCollect(bool fullCollect)
  {
    bool continueTimer = false;

    // Try to collect Texture GC first, due to the reference count of pixelData who become key of textures.
    // After all texture GC finished, then check PixelData cache.
    uint32_t checkedCount = 0u;

    // GC Cube Texture
    continueTimer |= CollectGarbages<false>(mCubeTextureCache, fullCollect, mCubeTextureContainerUpdated, mLatestCollectedCubeTextureIter, checkedCount);

    // GC Texture
    continueTimer |= CollectGarbages<false>(mTextureCache, fullCollect, mTextureContainerUpdated, mLatestCollectedTextureIter, checkedCount);

    // GC PixelData. We should lock mutex during GC pixelData.
    continueTimer |= CollectGarbages<true>(mPixelDataCache, fullCollect, mPixelDataContainerUpdated, mLatestCollectedPixelDataIter, checkedCount);

    return continueTimer;
  }

private:
  PixelDataCacheContainer   mPixelDataCache;
  TextureCacheContainer     mTextureCache;
  CubeTextureCacheContainer mCubeTextureCache;

  Dali::Timer mTimer;

  // Be used when we garbage collection.
  PixelDataCacheContainer::iterator   mLatestCollectedPixelDataIter;
  TextureCacheContainer::iterator     mLatestCollectedTextureIter;
  CubeTextureCacheContainer::iterator mLatestCollectedCubeTextureIter;

  bool mPixelDataContainerUpdated;
  bool mTextureContainerUpdated;
  bool mCubeTextureContainerUpdated;

  std::mutex mDataMutex;

  bool mDestroyed : 1;
  bool mFullCollectRequested : 1;
};

static std::shared_ptr<CacheImpl> gCacheImpl{nullptr};
static Dali::Texture              gEmptyTextureWhiteRGB{};

std::shared_ptr<CacheImpl> GetCacheImpl()
{
  if(DALI_UNLIKELY(!gCacheImpl))
  {
    gCacheImpl = std::make_shared<CacheImpl>();
  }
  return gCacheImpl;
}

void DestroyCacheImpl()
{
  gCacheImpl.reset();

  // Remove texture object when application stopped.
  gEmptyTextureWhiteRGB.Reset();
}

} // namespace

namespace Dali::Scene3D::Internal
{
namespace ImageResourceLoader
{
// Called by main thread..
Dali::PixelData GetEmptyPixelDataWhiteRGB()
{
  static Dali::PixelData emptyPixelData = PixelData::New(new uint8_t[3]{0xff, 0xff, 0xff}, 3, 1, 1, Pixel::RGB888, PixelData::DELETE_ARRAY);
  return emptyPixelData;
}

Dali::Texture GetEmptyTextureWhiteRGB()
{
  if(!gEmptyTextureWhiteRGB)
  {
    Dali::PixelData emptyPixelData = GetEmptyPixelDataWhiteRGB();
    gEmptyTextureWhiteRGB          = Texture::New(TextureType::TEXTURE_2D, emptyPixelData.GetPixelFormat(), emptyPixelData.GetWidth(), emptyPixelData.GetHeight());
    gEmptyTextureWhiteRGB.Upload(emptyPixelData, 0, 0, 0, 0, emptyPixelData.GetWidth(), emptyPixelData.GetHeight());
  }
  return gEmptyTextureWhiteRGB;
}

Dali::Texture GetCachedTexture(Dali::PixelData pixelData, bool mipmapRequired)
{
  return GetCacheImpl()->GetOrCreateCachedTexture(pixelData, mipmapRequired);
}

Dali::Texture GetCachedCubeTexture(const std::vector<std::vector<Dali::PixelData>>& pixelDataList, bool mipmapRequired)
{
  return GetCacheImpl()->GetOrCreateCachedCubeTexture(pixelDataList, mipmapRequired);
}

void RequestGarbageCollect(bool fullCollect)
{
  GetCacheImpl()->RequestGarbageCollect(fullCollect);
}

void EnsureResourceLoaderCreated()
{
  GetCacheImpl();
}

// Can be called by worker thread.
Dali::PixelData GetCachedPixelData(const std::string& url)
{
  return GetCachedPixelData(url, ImageDimensions(), FittingMode::DEFAULT, SamplingMode::BOX_THEN_LINEAR, true);
}

Dali::PixelData GetCachedPixelData(const std::string& url,
                                   ImageDimensions    dimensions,
                                   FittingMode::Type  fittingMode,
                                   SamplingMode::Type samplingMode,
                                   bool               orientationCorrection)
{
  ImageInformation info(url, dimensions, fittingMode, samplingMode, orientationCorrection);
  if(gCacheImpl == nullptr)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "CacheImpl not prepared! load PixelData without cache.\n");
    return CreatePixelDataFromImageInfo(info, false);
  }
  else
  {
    return GetCacheImpl()->GetOrCreateCachedPixelData(info);
  }
}
} // namespace ImageResourceLoader
} // namespace Dali::Scene3D::Internal

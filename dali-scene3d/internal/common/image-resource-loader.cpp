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
#include <dali-scene3d/internal/common/image-resource-loader.h>

// EXTERNAL INCLUDES
#include <dali-toolkit/devel-api/asset-manager/asset-manager.h>
#include <dali/devel-api/adaptor-framework/environment-variable.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/adaptor-framework/lifecycle-controller.h>
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali/devel-api/common/hash.h>
#include <dali/devel-api/common/map-wrapper.h>
#include <dali/devel-api/threading/mutex.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/pixel-data-integ.h>
#include <dali/integration-api/trace.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/connection-tracker.h>

#include <functional> ///< for std::function
#include <memory>     ///< for std::shared_ptr
#include <mutex>
#include <sstream>
#include <string>
#include <utility> ///< for std::pair

// INTERNAL INCLUDES

namespace
{
constexpr uint32_t MAXIMUM_COLLECTING_ITEM_COUNTS_PER_GC_CALL = 5u;
constexpr uint32_t GC_PERIOD_MILLISECONDS                     = 1000u;

constexpr std::string_view PRE_COMPUTED_BRDF_TEXTURE_FILE_NAME = "brdfLUT.png";

#ifdef DEBUG_ENABLED
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_IMAGE_RESOURCE_LOADER");
#endif
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_MODEL_PERFORMANCE_MARKER, false);

bool IsDefaultPixelData(const Dali::PixelData& pixelData)
{
  if(pixelData == Dali::Scene3D::Internal::ImageResourceLoader::GetEmptyPixelDataWhiteRGB() ||
     pixelData == Dali::Scene3D::Internal::ImageResourceLoader::GetEmptyPixelDataWhiteRGBA() ||
     pixelData == Dali::Scene3D::Internal::ImageResourceLoader::GetEmptyPixelDataZAxisRGB() ||
     pixelData == Dali::Scene3D::Internal::ImageResourceLoader::GetEmptyPixelDataZAxisAndAlphaRGBA())
  {
    return true;
  }
  return false;
}

bool SupportPixelDataCache(const Dali::PixelData& pixelData)
{
  // Check given pixelData support to release data after upload.
  // This is cause we need to reduce CPU memory usage.
  if(Dali::Integration::IsPixelDataReleaseAfterUpload(pixelData))
  {
    return true;
  }

  // Check given pixelData is default pixelData.
  if(IsDefaultPixelData(pixelData))
  {
    return true;
  }
  return false;
}

struct ImageInformation
{
  ImageInformation(const std::string           url,
                   const Dali::ImageDimensions dimensions,
                   Dali::SamplingMode::Type    samplingMode)
  : mUrl(url),
    mDimensions(dimensions),
    mSamplingMode(samplingMode)
  {
  }

  bool operator==(const ImageInformation& rhs) const
  {
    return (mUrl == rhs.mUrl) && (mDimensions == rhs.mDimensions) && (mSamplingMode == rhs.mSamplingMode);
  }

  std::string              mUrl;
  Dali::ImageDimensions    mDimensions;
  Dali::SamplingMode::Type mSamplingMode : 5;
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

    // Bit-pack the SamplingMode.
    *hashTargetPtr = (info.mSamplingMode);
  }

  return Dali::CalculateHash(info.mUrl) ^ Dali::CalculateHash(hashTarget);
}

std::size_t GenerateHash(const Dali::PixelData& pixelData, bool mipmapRequired)
{
  return reinterpret_cast<std::size_t>(static_cast<void*>(pixelData.GetObjectPtr())) ^ (static_cast<std::size_t>(mipmapRequired) << (sizeof(std::size_t) * 4));
}

// Item Creation functor list

Dali::PixelData CreatePixelDataFromImageInfo(const ImageInformation& info, bool releasePixelData)
{
  Dali::PixelData pixelData;

  DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_MODEL_LOAD_IMAGE_FROM_FILE", [&](std::ostringstream& oss) {
    oss << "[";
    if(info.mDimensions.GetWidth() > 0 || info.mDimensions.GetHeight() > 0)
    {
      oss << "d:" << info.mDimensions.GetWidth() << "x" << info.mDimensions.GetHeight() << " ";
    }
    oss << "s:" << info.mSamplingMode << " ";
    oss << "u:" << info.mUrl << "]";
  });
  // Load the image synchronously (block the thread here).
  Dali::Devel::PixelBuffer pixelBuffer = Dali::LoadImageFromFile(info.mUrl, info.mDimensions, Dali::FittingMode::DEFAULT, info.mSamplingMode, true);
  if(pixelBuffer)
  {
    pixelData = Dali::Devel::PixelBuffer::Convert(pixelBuffer, releasePixelData);
  }
  DALI_TRACE_END_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_MODEL_LOAD_IMAGE_FROM_FILE", [&](std::ostringstream& oss) {
    oss << "[";
    if(pixelData)
    {
      oss << "d:" << pixelData.GetWidth() << "x" << pixelData.GetHeight() << " f:" << pixelData.GetPixelFormat() << " ";
    }
    oss << "u:" << info.mUrl << "]";
  });
  return pixelData;
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

// Check function whether we can collect given data as garbage, or not.
bool PixelDataCacheCollectable(const ImageInformation& info, const Dali::PixelData& pixelData)
{
  return pixelData.GetBaseObject().ReferenceCount() <= 1;
}

bool TextureCacheCollectable(const Dali::PixelData& pixelData, const Dali::Texture& texture)
{
  return !IsDefaultPixelData(pixelData) &&                  ///< If key is not default pixelData
         pixelData.GetBaseObject().ReferenceCount() <= 2 && ///< And it have reference count as 2 (1 is for the key of this container, and other is PixelData cache.)
         texture.GetBaseObject().ReferenceCount() <= 1;     ///< And nobody use this texture, except this contianer.
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
    mTimer{},
    mLatestCollectedPixelDataIter{mPixelDataCache.begin()},
    mLatestCollectedTextureIter{mTextureCache.begin()},
    mDataMutex{},
    mPixelDataContainerUpdated{false},
    mTextureContainerUpdated{false},
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

      mDestroyed                    = true;
      mPixelDataContainerUpdated    = false;
      mTextureContainerUpdated      = false;
      mLatestCollectedPixelDataIter = decltype(mLatestCollectedPixelDataIter)(); // Invalidate iterator
      mLatestCollectedTextureIter   = decltype(mLatestCollectedTextureIter)();   // Invalidate iterator

      mPixelDataCache.clear();
      mTextureCache.clear();

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
  using PixelDataCacheContainer = std::map<std::size_t, std::vector<std::pair<ImageInformation, Dali::PixelData>>>;
  using TextureCacheContainer   = std::map<std::size_t, std::vector<std::pair<Dali::PixelData, Dali::Texture>>>;

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
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "HashValue : %zu\n", hashValue);
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
  template<typename KeyType, typename ValueType, bool (*Collectable)(const KeyType&, const ValueType&), typename ContainerType, typename Iterator = typename ContainerType::iterator>
  bool CollectGarbages(ContainerType& cacheContainer, bool fullCollect, bool& containerUpdated, Iterator& lastIterator, uint32_t& checkedCount, uint32_t& collectedCount)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Collect Garbages : %zu (checkedCount : %d, fullCollect? %d)\n", cacheContainer.size(), checkedCount, fullCollect);
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
        if(!item || Collectable(jter->first, item))
        {
          DALI_LOG_INFO(gLogFilter, Debug::Verbose, "GC!!!\n");
          // This item is garbage! just remove it.
          ++collectedCount;
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
        mDataMutex.lock();
        mPixelDataContainerUpdated = true;
        mDataMutex.unlock();
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
   * @param[in] releasePixelData Whether we need to release pixel data after upload, or not.
   * @return Texture that has been cached. Or empty handle if we fail to found cached item.
   */
  Dali::PixelData GetOrCreateCachedPixelData(const ImageInformation& info, bool releasePixelData)
  {
    auto hashValue = GenerateHash(info);
    return GetOrCreateCachedItem<true, ImageInformation, Dali::PixelData, CreatePixelDataFromImageInfo>(mPixelDataCache, hashValue, info, releasePixelData, mPixelDataContainerUpdated);
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
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "GC start\n");

    // Try to collect Texture GC first, due to the reference count of pixelData who become key of textures.
    // After all texture GC finished, then check PixelData cache.
    uint32_t checkedCount   = 0u;
    uint32_t collectedCount = 0u;

    // We should lock mutex during GC pixelData.
    mDataMutex.lock();

    // GC Texture
    continueTimer |= CollectGarbages<Dali::PixelData, Dali::Texture, TextureCacheCollectable>(mTextureCache, fullCollect, mTextureContainerUpdated, mLatestCollectedTextureIter, checkedCount, collectedCount);

    // GC PixelData last. If there are some collected Texture before, we should full-collect.
    // (Since most of PixelData use 'ReleaseAfterUpload' flags).
    continueTimer |= CollectGarbages<ImageInformation, Dali::PixelData, PixelDataCacheCollectable>(mPixelDataCache, fullCollect || (collectedCount > 0u), mPixelDataContainerUpdated, mLatestCollectedPixelDataIter, checkedCount, collectedCount);

    mDataMutex.unlock();

    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "GC finished. checkedCount : %u, continueTimer : %d\n", checkedCount, continueTimer);

    return continueTimer;
  }

private:
  PixelDataCacheContainer mPixelDataCache;
  TextureCacheContainer   mTextureCache;

  Dali::Timer mTimer;

  // Be used when we garbage collection.
  PixelDataCacheContainer::iterator mLatestCollectedPixelDataIter;
  TextureCacheContainer::iterator   mLatestCollectedTextureIter;

  std::mutex mDataMutex;

  bool mPixelDataContainerUpdated;
  bool mTextureContainerUpdated;

  bool mDestroyed : 1;
  bool mFullCollectRequested : 1;
};

static std::shared_ptr<CacheImpl> gCacheImpl{nullptr};

// Static singletone instance what we usually used.
static Dali::Texture gEmptyTextureWhiteRGB{};
static Dali::Texture gEmptyCubeTextureWhiteRGB{};
static Dali::Texture gDefaultBrdfTexture{};

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
  gEmptyCubeTextureWhiteRGB.Reset();
  gDefaultBrdfTexture.Reset();
}

} // namespace

namespace Dali::Scene3D::Internal
{
namespace ImageResourceLoader
{
// Called by main thread..
Dali::Texture GetEmptyTextureWhiteRGB()
{
  if(DALI_UNLIKELY(!gEmptyTextureWhiteRGB))
  {
    Dali::PixelData emptyPixelData = GetEmptyPixelDataWhiteRGB();
    gEmptyTextureWhiteRGB          = Texture::New(TextureType::TEXTURE_2D, emptyPixelData.GetPixelFormat(), emptyPixelData.GetWidth(), emptyPixelData.GetHeight());
    gEmptyTextureWhiteRGB.Upload(emptyPixelData, 0, 0, 0, 0, emptyPixelData.GetWidth(), emptyPixelData.GetHeight());
  }
  return gEmptyTextureWhiteRGB;
}

Dali::Texture GetEmptyCubeTextureWhiteRGB()
{
  if(DALI_UNLIKELY(!gEmptyCubeTextureWhiteRGB))
  {
    Dali::PixelData emptyPixelData = GetEmptyPixelDataWhiteRGB();
    gEmptyCubeTextureWhiteRGB      = Texture::New(TextureType::TEXTURE_CUBE, emptyPixelData.GetPixelFormat(), emptyPixelData.GetWidth(), emptyPixelData.GetHeight());
    for(size_t iSide = 0u; iSide < 6; ++iSide)
    {
      gEmptyCubeTextureWhiteRGB.Upload(emptyPixelData, CubeMapLayer::POSITIVE_X + iSide, 0u, 0u, 0u, emptyPixelData.GetWidth(), emptyPixelData.GetHeight());
    }
  }
  return gEmptyCubeTextureWhiteRGB;
}

Dali::Texture GetDefaultBrdfTexture()
{
  if(DALI_UNLIKELY(!gDefaultBrdfTexture))
  {
    Dali::PixelData brdfPixelData = GetDefaultBrdfPixelData();
    gDefaultBrdfTexture           = Texture::New(TextureType::TEXTURE_2D, brdfPixelData.GetPixelFormat(), brdfPixelData.GetWidth(), brdfPixelData.GetHeight());
    gDefaultBrdfTexture.Upload(brdfPixelData, 0, 0, 0, 0, brdfPixelData.GetWidth(), brdfPixelData.GetHeight());
  }
  return gDefaultBrdfTexture;
}

Dali::Texture GetCachedTexture(Dali::PixelData pixelData, bool mipmapRequired)
{
  if(Dali::Adaptor::IsAvailable() && SupportPixelDataCache(pixelData))
  {
    return GetCacheImpl()->GetOrCreateCachedTexture(pixelData, mipmapRequired);
  }
  else
  {
    return CreateTextureFromPixelData(pixelData, mipmapRequired);
  }
}

void RequestGarbageCollect(bool fullCollect)
{
  if(DALI_LIKELY(Dali::Adaptor::IsAvailable()))
  {
    GetCacheImpl()->RequestGarbageCollect(fullCollect);
  }
}

void EnsureResourceLoaderCreated()
{
  if(DALI_LIKELY(Dali::Adaptor::IsAvailable()))
  {
    GetCacheImpl();
  }
}

// Can be called by worker thread.
Dali::PixelData GetEmptyPixelDataWhiteRGB()
{
  static Dali::PixelData emptyPixelData = PixelData::New(new uint8_t[3]{0xff, 0xff, 0xff}, 3, 1, 1, Pixel::RGB888, PixelData::DELETE_ARRAY);
  return emptyPixelData;
}

Dali::PixelData GetEmptyPixelDataWhiteRGBA()
{
  static Dali::PixelData emptyPixelData = PixelData::New(new uint8_t[4]{0xff, 0xff, 0xff, 0xff}, 4, 1, 1, Pixel::RGBA8888, PixelData::DELETE_ARRAY);
  return emptyPixelData;
}

Dali::PixelData GetEmptyPixelDataZAxisRGB()
{
  static Dali::PixelData emptyPixelData = PixelData::New(new uint8_t[3]{0x7f, 0x7f, 0xff}, 3, 1, 1, Pixel::RGB888, PixelData::DELETE_ARRAY);
  return emptyPixelData;
}

Dali::PixelData GetEmptyPixelDataZAxisAndAlphaRGBA()
{
  static Dali::PixelData emptyPixelData = PixelData::New(new uint8_t[4]{0x7f, 0x7f, 0xff, 0xff}, 4, 1, 1, Pixel::RGBA8888, PixelData::DELETE_ARRAY);
  return emptyPixelData;
}

Dali::PixelData GetDefaultBrdfPixelData()
{
  static Dali::Mutex sPixelDataMutex;
  {
    Dali::Mutex::ScopedLock lock(sPixelDataMutex);

    static Dali::PixelData defaultBrdfPixelData;

    if(DALI_UNLIKELY(!defaultBrdfPixelData))
    {
      Devel::PixelBuffer pixelBuffer = Dali::LoadImageFromFile(Dali::Toolkit::AssetManager::GetDaliImagePath() + std::string(PRE_COMPUTED_BRDF_TEXTURE_FILE_NAME));
      if(pixelBuffer)
      {
        defaultBrdfPixelData = Devel::PixelBuffer::Convert(pixelBuffer);
      }
    }
    return defaultBrdfPixelData;
  }
}

Dali::PixelData GetCachedPixelData(const std::string& url)
{
  return GetCachedPixelData(url, ImageDimensions(), SamplingMode::BOX_THEN_LINEAR);
}

Dali::PixelData GetCachedPixelData(const std::string& url,
                                   ImageDimensions    dimensions,
                                   SamplingMode::Type samplingMode)
{
  ImageInformation info(url, dimensions, samplingMode);
  if(gCacheImpl == nullptr)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "CacheImpl not prepared! load PixelData without cache.\n");
    return CreatePixelDataFromImageInfo(info, false);
  }
  else
  {
    return GetCacheImpl()->GetOrCreateCachedPixelData(info, true);
  }
}
} // namespace ImageResourceLoader
} // namespace Dali::Scene3D::Internal

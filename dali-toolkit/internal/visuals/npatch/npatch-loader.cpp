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
 *
 */

// CLASS HEADER
#include <dali-toolkit/internal/visuals/npatch/npatch-loader.h>

// INTERNAL HEADERS
#include <dali-toolkit/internal/visuals/rendering-addon.h>

// EXTERNAL HEADERS
#include <dali/devel-api/common/hash.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
constexpr auto INVALID_CACHE_INDEX = int32_t{-1}; ///< Invalid Cache index
constexpr auto UNINITIALIZED_ID    = int32_t{0};  ///< uninitialised id, use to initialize ids

DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_IMAGE_PERFORMANCE_MARKER, false);
} // Anonymous namespace

NPatchLoader::NPatchLoader()
: mCurrentNPatchDataId(0),
  mRemoveProcessorRegistered(false)
{
}

NPatchLoader::~NPatchLoader()
{
  if(mRemoveProcessorRegistered && Adaptor::IsAvailable())
  {
    Adaptor::Get().UnregisterProcessorOnce(*this, true);
    mRemoveProcessorRegistered = false;
  }
}

NPatchData::NPatchDataId NPatchLoader::GenerateUniqueNPatchDataId()
{
  // Skip invalid id generation.
  if(DALI_UNLIKELY(mCurrentNPatchDataId == NPatchData::INVALID_NPATCH_DATA_ID))
  {
    mCurrentNPatchDataId = 0;
  }
  return mCurrentNPatchDataId++;
}

NPatchData::NPatchDataId NPatchLoader::Load(TextureManager& textureManager, TextureUploadObserver* textureObserver, const VisualUrl& url, const Rect<int>& border, bool& preMultiplyOnLoad, bool synchronousLoading)
{
  NPatchDataPtr data = GetNPatchData(url, border, preMultiplyOnLoad);

  DALI_ASSERT_ALWAYS(data.Get() && "NPatchData creation failed!");

  if(data->GetLoadingState() == NPatchData::LoadingState::LOAD_COMPLETE)
  {
    if(!synchronousLoading)
    {
      // NotifyObserver already done, so
      // data will not iterate observer list.
      // We need to call LoadComplete directly.
      data->NotifyObserver(textureObserver, true);
    }
  }
  else // if NOT_STARTED or LOADING or LOAD_FAILED, try to reload.
  {
    if(!synchronousLoading)
    {
      data->AddObserver(textureObserver);
      // If still LOADING and async, don't need to request reload. Fast return.
      if(data->GetLoadingState() == NPatchData::LoadingState::LOADING)
      {
        return data->GetId();
      }
    }

    data->SetLoadingState(NPatchData::LoadingState::LOADING);

    auto preMultiplyOnLoading = preMultiplyOnLoad ? TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD
                                                  : TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;

    Devel::PixelBuffer pixelBuffer = textureManager.LoadPixelBuffer(url, Dali::ImageDimensions(), FittingMode::DEFAULT, SamplingMode::BOX_THEN_LINEAR, synchronousLoading, data.Get(), true, preMultiplyOnLoading);

    if(pixelBuffer)
    {
      preMultiplyOnLoad = (preMultiplyOnLoading == TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD) ? true : false;
      data->SetLoadedNPatchData(pixelBuffer, preMultiplyOnLoad);
    }
    else if(synchronousLoading)
    {
      data->SetLoadingState(NPatchData::LoadingState::LOAD_FAILED);
    }
  }
  return data->GetId();
}

int32_t NPatchLoader::GetCacheIndexFromId(const NPatchData::NPatchDataId id)
{
  const unsigned int size = mCache.size();

  for(unsigned int i = 0; i < size; ++i)
  {
    if(mCache[i].mData->GetId() == id)
    {
      return i;
    }
  }

  return INVALID_CACHE_INDEX;
}

bool NPatchLoader::GetNPatchData(const NPatchData::NPatchDataId id, NPatchDataPtr& data)
{
  int32_t cacheIndex = GetCacheIndexFromId(id);
  if(cacheIndex != INVALID_CACHE_INDEX)
  {
    data = mCache[cacheIndex].mData;
    return true;
  }
  data = nullptr;
  return false;
}

void NPatchLoader::RequestRemove(NPatchData::NPatchDataId id, TextureUploadObserver* textureObserver)
{
  // Remove observer first
  if(textureObserver)
  {
    int32_t cacheIndex = GetCacheIndexFromId(id);
    if(cacheIndex != INVALID_CACHE_INDEX)
    {
      NPatchInfo& info(mCache[cacheIndex]);

      info.mData->RemoveObserver(textureObserver);
    }
  }

  mRemoveQueue.push_back({id, nullptr});

  if(!mRemoveProcessorRegistered && Adaptor::IsAvailable())
  {
    mRemoveProcessorRegistered = true;
    Adaptor::Get().RegisterProcessorOnce(*this, true);
  }
}

void NPatchLoader::Remove(NPatchData::NPatchDataId id, TextureUploadObserver* textureObserver)
{
  int32_t cacheIndex = GetCacheIndexFromId(id);
  if(cacheIndex == INVALID_CACHE_INDEX)
  {
    return;
  }

  NPatchInfo& info(mCache[cacheIndex]);

  info.mData->RemoveObserver(textureObserver);

  if(--info.mReferenceCount <= 0)
  {
    mCache.erase(mCache.begin() + cacheIndex);
  }
}

void NPatchLoader::Process(bool postProcessor)
{
  DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_NPATCH_LOADER_PROCESS_REMOVE_QUEUE", [&](std::ostringstream& oss) {
    oss << "[" << mRemoveQueue.size() << "]";
  });

  mRemoveProcessorRegistered = false;

  for(auto& iter : mRemoveQueue)
  {
    Remove(iter.first, iter.second);
  }

  mRemoveQueue.clear();

  DALI_TRACE_END(gTraceFilter, "DALI_NPATCH_LOADER_PROCESS_REMOVE_QUEUE");
}

NPatchDataPtr NPatchLoader::GetNPatchData(const VisualUrl& url, const Rect<int>& border, bool& preMultiplyOnLoad)
{
  std::size_t                              hash  = CalculateHash(url.GetUrl());
  std::vector<NPatchInfo>::size_type       index = UNINITIALIZED_ID;
  const std::vector<NPatchInfo>::size_type count = mCache.size();

  NPatchInfo* infoPtr = nullptr;

  for(; index < count; ++index)
  {
    if(mCache[index].mData->GetHash() == hash)
    {
      // hash match, check url as well in case of hash collision
      if(mCache[index].mData->GetUrl().GetUrl() == url.GetUrl())
      {
        // Use cached data. Need to fast-out return.
        if(mCache[index].mData->GetBorder() == border)
        {
          mCache[index].mReferenceCount++;
          return mCache[index].mData;
        }
        else
        {
          if(mCache[index].mData->GetLoadingState() == NPatchData::LoadingState::LOAD_COMPLETE)
          {
            // If we only found LOAD_FAILED case, replace current data. We can reuse texture
            if(infoPtr == nullptr || infoPtr->mData->GetLoadingState() != NPatchData::LoadingState::LOAD_COMPLETE)
            {
              infoPtr = &mCache[index];
            }
          }
          // Still loading pixel buffer. We cannot reuse cached texture yet. Skip checking
          else if(mCache[index].mData->GetLoadingState() == NPatchData::LoadingState::LOADING)
          {
            continue;
          }
          // if LOAD_FAILED, reuse this cached NPatchData, and try to load again.
          else
          {
            if(infoPtr == nullptr)
            {
              infoPtr = &mCache[index];
            }
          }
        }
      }
    }
  }

  // If this is new image loading, make new cache data
  if(infoPtr == nullptr)
  {
    NPatchInfo info(new NPatchData());
    info.mData->SetId(GenerateUniqueNPatchDataId());
    info.mData->SetHash(hash);
    info.mData->SetUrl(url);
    info.mData->SetBorder(border);
    info.mData->SetPreMultiplyOnLoad(preMultiplyOnLoad);

    mCache.emplace_back(std::move(info));
    infoPtr = &mCache.back();
  }
  // Else if LOAD_COMPLETE, Same url but border is different - use the existing texture
  else if(infoPtr->mData->GetLoadingState() == NPatchData::LoadingState::LOAD_COMPLETE)
  {
    NPatchInfo info(new NPatchData());

    info.mData->SetId(GenerateUniqueNPatchDataId());
    info.mData->SetHash(hash);
    info.mData->SetUrl(url);
    info.mData->SetCroppedWidth(infoPtr->mData->GetCroppedWidth());
    info.mData->SetCroppedHeight(infoPtr->mData->GetCroppedHeight());

    info.mData->SetTextures(infoPtr->mData->GetTextures());

    NPatchUtility::StretchRanges stretchRangesX;
    NPatchUtility::StretchRanges stretchRangesY;
    stretchRangesX.PushBack(NPatchUtility::GetValidStrechPointFromBorder(info.mData->GetCroppedWidth(), static_cast<uint32_t>(border.left), static_cast<uint32_t>(border.right)));
    stretchRangesY.PushBack(NPatchUtility::GetValidStrechPointFromBorder(info.mData->GetCroppedHeight(), static_cast<uint32_t>(border.top), static_cast<uint32_t>(border.bottom)));

    info.mData->SetStretchPixelsX(stretchRangesX);
    info.mData->SetStretchPixelsY(stretchRangesY);
    info.mData->SetBorder(border);

    info.mData->SetPreMultiplyOnLoad(infoPtr->mData->IsPreMultiplied());

    info.mData->SetLoadingState(NPatchData::LoadingState::LOAD_COMPLETE);

    mCache.emplace_back(std::move(info));
    infoPtr = &mCache.back();
  }
  // Else, LOAD_FAILED. just increase reference so we can reuse it.
  else
  {
    infoPtr->mReferenceCount++;
  }

  DALI_ASSERT_ALWAYS(infoPtr && "NPatchInfo creation failed!");

  return infoPtr->mData;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

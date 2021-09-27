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
 *
 */

// CLASS HEADER
#include <dali-toolkit/internal/visuals/npatch-loader.h>

// INTERNAL HEADERS
#include <dali-toolkit/internal/visuals/rendering-addon.h>

// EXTERNAL HEADERS
#include <dali/devel-api/common/hash.h>
#include <dali/integration-api/debug.h>

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

} // Anonymous namespace

NPatchLoader::NPatchLoader()
: mCurrentNPatchDataId(0)
{
}

NPatchLoader::~NPatchLoader()
{
}

NPatchData::NPatchDataId NPatchLoader::GenerateUniqueNPatchDataId()
{
  return mCurrentNPatchDataId++;
}

std::size_t NPatchLoader::Load(TextureManager& textureManager, TextureUploadObserver* textureObserver, const VisualUrl& url, const Rect<int>& border, bool& preMultiplyOnLoad, bool synchronousLoading)
{
  std::size_t                                 hash  = CalculateHash(url.GetUrl());
  OwnerContainer<NPatchData*>::SizeType       index = UNINITIALIZED_ID;
  const OwnerContainer<NPatchData*>::SizeType count = mCache.Count();

  for(; index < count; ++index)
  {
    if(mCache[index]->GetHash() == hash)
    {
      // hash match, check url as well in case of hash collision
      if(mCache[index]->GetUrl().GetUrl() == url.GetUrl())
      {
        // Use cached data
        if(mCache[index]->GetBorder() == border)
        {
          if(mCache[index]->GetLoadingState() == NPatchData::LoadingState::LOADING)
          {
            mCache[index]->AddObserver(textureObserver);
          }
          return mCache[index]->GetId(); // valid indices are from 1 onwards
        }
        else
        {
          if(mCache[index]->GetLoadingState() == NPatchData::LoadingState::LOAD_COMPLETE)
          {
            // Same url but border is different - use the existing texture
            NPatchData* newData = new NPatchData();
            newData->SetId(GenerateUniqueNPatchDataId());
            newData->SetHash(hash);
            newData->SetUrl(url);
            newData->SetCroppedWidth(mCache[index]->GetCroppedWidth());
            newData->SetCroppedHeight(mCache[index]->GetCroppedHeight());

            newData->SetTextures(mCache[index]->GetTextures());

            NPatchUtility::StretchRanges stretchRangesX;
            stretchRangesX.PushBack(Uint16Pair(border.left, ((newData->GetCroppedWidth() >= static_cast<unsigned int>(border.right)) ? newData->GetCroppedHeight() - border.right : 0)));

            NPatchUtility::StretchRanges stretchRangesY;
            stretchRangesY.PushBack(Uint16Pair(border.top, ((newData->GetCroppedWidth() >= static_cast<unsigned int>(border.bottom)) ? newData->GetCroppedHeight() - border.bottom : 0)));

            newData->SetStretchPixelsX(stretchRangesX);
            newData->SetStretchPixelsY(stretchRangesY);
            newData->SetBorder(border);

            newData->SetPreMultiplyOnLoad(mCache[index]->IsPreMultiplied());

            newData->SetLoadingState(NPatchData::LoadingState::LOAD_COMPLETE);
            newData->AddObserver(textureObserver);

            mCache.PushBack(newData);
            return newData->GetId(); // valid ids start from 1u
          }
        }
      }
    }
  }

  // If this is new image loading, make new cache data
  NPatchData* data;
  data = new NPatchData();
  data->SetId(GenerateUniqueNPatchDataId());
  data->SetHash(hash);
  data->SetUrl(url);
  data->SetBorder(border);
  data->SetPreMultiplyOnLoad(preMultiplyOnLoad);
  data->AddObserver(textureObserver);
  mCache.PushBack(data);

  auto preMultiplyOnLoading = preMultiplyOnLoad ? TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD
                                                : TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;

  Devel::PixelBuffer pixelBuffer = textureManager.LoadPixelBuffer(url, Dali::ImageDimensions(), FittingMode::DEFAULT, SamplingMode::BOX_THEN_LINEAR, synchronousLoading, data, true, preMultiplyOnLoading);

  if(pixelBuffer)
  {
    preMultiplyOnLoad = (preMultiplyOnLoading == TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD) ? true : false;
    data->SetLoadedNPatchData(pixelBuffer, preMultiplyOnLoad);
  }
  return data->GetId();
}

int32_t NPatchLoader::GetCacheIndexFromId(const NPatchData::NPatchDataId id)
{
  const unsigned int size = mCache.Count();

  for(unsigned int i = 0; i < size; ++i)
  {
    if(mCache[i]->GetId() == id)
    {
      return i;
    }
  }

  return INVALID_CACHE_INDEX;
}

bool NPatchLoader::GetNPatchData(const NPatchData::NPatchDataId id, const NPatchData*& data)
{
  int32_t cacheIndex = GetCacheIndexFromId(id);
  if(cacheIndex != INVALID_CACHE_INDEX)
  {
    data = mCache[cacheIndex];
    return true;
  }
  data = nullptr;
  return false;
}

void NPatchLoader::Remove(std::size_t id, TextureUploadObserver* textureObserver)
{
  int32_t cacheIndex = GetCacheIndexFromId(id);
  if(cacheIndex == INVALID_CACHE_INDEX)
  {
    return;
  }

  NPatchData* data;
  data = mCache[cacheIndex];

  data->RemoveObserver(textureObserver);

  if(data->GetObserverCount() == 0)
  {
    mCache.Erase(mCache.Begin() + cacheIndex);
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

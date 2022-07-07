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
 *
 */

// CLASS HEADER
#include <dali-toolkit/internal/visuals/npatch-data.h>

// INTERNAL HEADERS
#include <dali-toolkit/internal/visuals/rendering-addon.h>

// EXTERNAL HEADERS
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
NPatchData::NPatchData()
: mId(INVALID_NPATCH_DATA_ID),
  mUrl(),
  mTextureSet(),
  mHash(0),
  mCroppedWidth(0),
  mCroppedHeight(0),
  mBorder(0, 0, 0, 0),
  mLoadingState(LoadingState::NOT_STARTED),
  mPreMultiplyOnLoad(false),
  mRenderingMap{nullptr}
{
}

NPatchData::~NPatchData()
{
  // If there is an opacity map, it has to be destroyed using addon call
  if(mRenderingMap)
  {
    RenderingAddOn::Get().DestroyNPatch(mRenderingMap);
  }
}

void NPatchData::SetId(const NPatchDataId id)
{
  mId = id;
}

NPatchData::NPatchDataId NPatchData::GetId() const
{
  return mId;
}

void NPatchData::AddObserver(TextureUploadObserver* textureObserver)
{
  mObserverList.PushBack(textureObserver);
}

void NPatchData::RemoveObserver(TextureUploadObserver* textureObserver)
{
  for(uint32_t index = 0; index < mObserverList.Count(); ++index)
  {
    if(textureObserver == mObserverList[index])
    {
      mObserverList.Erase(mObserverList.begin() + index);
      break;
    }
  }
}

uint32_t NPatchData::GetObserverCount() const
{
  return mObserverList.Count();
}

void NPatchData::SetUrl(const VisualUrl& url)
{
  mUrl = url;
}

VisualUrl NPatchData::GetUrl() const
{
  return mUrl;
}

void NPatchData::SetTextures(const TextureSet textureSet)
{
  mTextureSet = textureSet;
}

TextureSet NPatchData::GetTextures() const
{
  return mTextureSet;
}

void NPatchData::SetStretchPixelsX(const NPatchUtility::StretchRanges stretchPixelsX)
{
  mStretchPixelsX = stretchPixelsX;
}

void NPatchData::SetStretchPixelsY(const NPatchUtility::StretchRanges stretchPixelsY)
{
  mStretchPixelsY = stretchPixelsY;
}

NPatchUtility::StretchRanges NPatchData::GetStretchPixelsX() const
{
  return mStretchPixelsX;
}

NPatchUtility::StretchRanges NPatchData::GetStretchPixelsY() const
{
  return mStretchPixelsY;
}

void NPatchData::SetHash(std::size_t hash)
{
  mHash = hash;
}

std::size_t NPatchData::GetHash() const
{
  return mHash;
}

void NPatchData::SetCroppedWidth(uint32_t croppedWidth)
{
  mCroppedWidth = croppedWidth;
}

void NPatchData::SetCroppedHeight(uint32_t croppedHeight)
{
  mCroppedHeight = croppedHeight;
}

uint32_t NPatchData::GetCroppedWidth() const
{
  return mCroppedWidth;
}

uint32_t NPatchData::GetCroppedHeight() const
{
  return mCroppedHeight;
}

void NPatchData::SetBorder(const Rect<int> border)
{
  mBorder = border;
}

Rect<int> NPatchData::GetBorder() const
{
  return mBorder;
}

void NPatchData::SetPreMultiplyOnLoad(bool preMultiplyOnLoad)
{
  mPreMultiplyOnLoad = preMultiplyOnLoad;
}

bool NPatchData::IsPreMultiplied() const
{
  return mPreMultiplyOnLoad;
}

void NPatchData::SetLoadingState(const LoadingState loadingState)
{
  mLoadingState = loadingState;
}

NPatchData::LoadingState NPatchData::GetLoadingState() const
{
  return mLoadingState;
}

void* NPatchData::GetRenderingMap() const
{
  return mRenderingMap;
}

void NPatchData::SetLoadedNPatchData(Devel::PixelBuffer& pixelBuffer, bool preMultiplied)
{
  if(mBorder == Rect<int>(0, 0, 0, 0))
  {
    NPatchUtility::ParseBorders(pixelBuffer, mStretchPixelsX, mStretchPixelsY);

    // Crop the image
    pixelBuffer.Crop(1, 1, pixelBuffer.GetWidth() - 2, pixelBuffer.GetHeight() - 2);
  }
  else
  {
    mStretchPixelsX.PushBack(Uint16Pair(mBorder.left, ((pixelBuffer.GetWidth() >= static_cast<unsigned int>(mBorder.right)) ? pixelBuffer.GetWidth() - mBorder.right : 0)));
    mStretchPixelsY.PushBack(Uint16Pair(mBorder.top, ((pixelBuffer.GetHeight() >= static_cast<unsigned int>(mBorder.bottom)) ? pixelBuffer.GetHeight() - mBorder.bottom : 0)));
  }

  mCroppedWidth  = pixelBuffer.GetWidth();
  mCroppedHeight = pixelBuffer.GetHeight();

  // Create opacity map
  mRenderingMap = RenderingAddOn::Get().IsValid() ? RenderingAddOn::Get().BuildNPatch(pixelBuffer, this) : nullptr;

  PixelData pixels = Devel::PixelBuffer::Convert(pixelBuffer); // takes ownership of buffer

  Texture texture = Texture::New(TextureType::TEXTURE_2D, pixels.GetPixelFormat(), pixels.GetWidth(), pixels.GetHeight());
  texture.Upload(pixels);

  mTextureSet = TextureSet::New();
  mTextureSet.SetTexture(0u, texture);

  mPreMultiplyOnLoad = preMultiplied;

  mLoadingState = LoadingState::LOAD_COMPLETE;
}

void NPatchData::NotifyObserver(TextureUploadObserver* observer, const bool& loadSuccess)
{
  observer->LoadComplete(
    loadSuccess,
    TextureUploadObserver::TextureInformation(
      TextureUploadObserver::ReturnType::TEXTURE,
      static_cast<TextureManager::TextureId>(mId), ///< Note : until end of NPatchLoader::Load, npatch-visual don't know the id of data.
      mTextureSet,
      mUrl.GetUrl(),
      mPreMultiplyOnLoad));
}

void NPatchData::LoadComplete(bool loadSuccess, TextureInformation textureInformation)
{
  if(loadSuccess)
  {
    if(mLoadingState != LoadingState::LOAD_COMPLETE)
    {
      // If mLoadingState is LOAD_FAILED, just re-set (It can be happened when sync loading is failed, but async loading is succeeded).
      SetLoadedNPatchData(textureInformation.pixelBuffer, textureInformation.preMultiplied);
    }
  }
  else
  {
    if(mLoadingState == LoadingState::LOADING)
    {
      mLoadingState = LoadingState::LOAD_FAILED;
    }
    // If mLoadingState is already LOAD_COMPLETE, we can use uploaded texture (It can be happened when sync loading is succeeded, but async loading is failed).
    else if(mLoadingState == LoadingState::LOAD_COMPLETE)
    {
      loadSuccess = true;
    }
  }

  for(uint32_t index = 0; index < mObserverList.Count(); ++index)
  {
    TextureUploadObserver* observer = mObserverList[index];
    NotifyObserver(observer, loadSuccess);
  }

  mObserverList.Clear();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

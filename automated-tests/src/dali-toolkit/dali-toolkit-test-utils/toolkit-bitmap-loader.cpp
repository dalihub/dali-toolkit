/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include "toolkit-bitmap-loader.h"

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-object.h>
#include <cstring>
#include <semaphore.h>

using namespace Dali;

namespace Dali
{
namespace
{
Dali::BitmapLoader gBitmapLoader;
}


namespace Internal
{

class BitmapLoader : public BaseObject
{
public:
  static IntrusivePtr<BitmapLoader> New( const std::string& url,
                                         ImageDimensions size,
                                         FittingMode::Type fittingMode,
                                         SamplingMode::Type samplingMode,
                                         bool orientationCorrection)
  {
    IntrusivePtr<BitmapLoader> internal = new BitmapLoader( url, size, fittingMode, samplingMode, orientationCorrection );
    return internal;
  }

  BitmapLoader(const std::string& url,
               ImageDimensions size,
               FittingMode::Type fittingMode,
               SamplingMode::Type samplingMode,
               bool orientationCorrection)
  : mSize(size),
    mPixelData(),
    mUrl(url)
  {
    sem_init( &mySemaphore, 0, 0 );
  }

  ~BitmapLoader(){}

  void Load()
  {
    size_t bufferSize  = mSize.GetWidth() * mSize.GetHeight() * 4;
    unsigned char* buffer = static_cast<unsigned char*>(malloc(bufferSize));
    memset(buffer, 0, bufferSize);

    mPixelData = PixelData::New( buffer, mSize.GetWidth(), mSize.GetHeight(), Pixel::RGBA8888, PixelData::FREE);

    sem_post( &mySemaphore );
  }

  PixelDataPtr GetPixelData() const
  {
    return mPixelData;
  }

  const std::string& GetUrl() const
  {
    return mUrl;
  }

  bool IsLoaded()
  {
    return mPixelData ? true : false;
  }

  void WaitForLoading()
  {
    if( mPixelData )
    {
      return;
    }
    sem_wait( &mySemaphore );
  }

  ImageDimensions mSize;
  PixelDataPtr mPixelData;
  const std::string mUrl;
  sem_t mySemaphore;
};

} // internal

inline Internal::BitmapLoader& GetImplementation(Dali::BitmapLoader& handle)
{
  DALI_ASSERT_ALWAYS( handle && "handle is empty" );
  BaseObject& object = handle.GetBaseObject();
  return static_cast<Internal::BitmapLoader&>(object);
}

inline const Internal::BitmapLoader& GetImplementation(const Dali::BitmapLoader& handle)
{
  DALI_ASSERT_ALWAYS( handle && "handle is empty" );
  const BaseObject& object = handle.GetBaseObject();
  return static_cast<const Internal::BitmapLoader&>(object);
}

Dali::BitmapLoader Dali::BitmapLoader::New(std::string const&url, Dali::Uint16Pair size, Dali::FittingMode::Type fittingMode, Dali::SamplingMode::Type samplingMode, bool orientationCorrection)
{
  IntrusivePtr<Internal::BitmapLoader> internal = Internal::BitmapLoader::New(url, size, fittingMode, samplingMode, orientationCorrection);
  gBitmapLoader = BitmapLoader(internal.Get());
  return gBitmapLoader;
}

BitmapLoader::BitmapLoader()
{
}

Dali::BitmapLoader::BitmapLoader(Dali::BitmapLoader const& handle)
:BaseHandle(handle)
{
}
Dali::BitmapLoader::BitmapLoader(Internal::BitmapLoader* internal)
:BaseHandle(internal)
{
}
Dali::BitmapLoader::~BitmapLoader()
{
}
BitmapLoader& BitmapLoader::operator=(const BitmapLoader& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}
void Dali::BitmapLoader::Load()
{
  GetImplementation(*this).Load();
}
PixelDataPtr Dali::BitmapLoader::GetPixelData() const
{
  return GetImplementation(*this).GetPixelData();
}
std::string Dali::BitmapLoader::GetUrl() const
{
  return GetImplementation(*this).GetUrl();
}
bool Dali::BitmapLoader::IsLoaded()
{
  return GetImplementation(*this).IsLoaded();
}
void Dali::BitmapLoader::WaitForLoading()
{
  GetImplementation(*this).WaitForLoading();
}

BitmapLoader Dali::BitmapLoader::GetLatestCreated()
{
  return gBitmapLoader;
}

void Dali::BitmapLoader::ResetLatestCreated()
{
  gBitmapLoader.Reset();
}

} // Dali

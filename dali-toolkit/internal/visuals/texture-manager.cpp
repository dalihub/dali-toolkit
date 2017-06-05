 /*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include "texture-manager.h"

// EXTERNAL HEADERS
#include <dali/devel-api/common/hash.h>
#include <dali/devel-api/images/texture-set-image.h>
#include <dali/integration-api/debug.h>

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali-toolkit/internal/image-loader/async-image-loader-impl.h>
#include <dali-toolkit/internal/image-loader/image-atlas-impl.h>
#include <dali-toolkit/public-api/image-loader/sync-image-loader.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

#ifdef DEBUG_ENABLED
Debug::Filter* gTextureManagerLogFilter = Debug::Filter::New( Debug::NoLogging, false, "LOG_TEXTURE_MANAGER" );
#endif

const uint32_t      DEFAULT_ATLAS_SIZE( 1024u );                     ///< This size can fit 8 by 8 images of average size 128 * 128
const Vector4       FULL_ATLAS_RECT( 0.0f, 0.0f, 1.0f, 1.0f );       ///< UV Rectangle that covers the full Texture
const char * const  BROKEN_IMAGE_URL( DALI_IMAGE_DIR "broken.png" ); ///< URL For the broken image placeholder
const int           INVALID_INDEX( -1 );                             ///< Invalid index used to represent a non-existant TextureInfo struct
const int           INVALID_CACHE_INDEX( -1 ); ///< Invalid Cache index

} // Anonymous namespace


TextureManager::TextureManager()
: mAsyncLocalLoader( Toolkit::AsyncImageLoader::New() ),
  mAsyncRemoteLoader( Toolkit::AsyncImageLoader::New() ),
  mCurrentTextureId( 0 )
{
  mAsyncLocalLoader.ImageLoadedSignal().Connect( this, &TextureManager::AsyncLocalLoadComplete );
  mAsyncRemoteLoader.ImageLoadedSignal().Connect( this, &TextureManager::AsyncRemoteLoadComplete );
}

TextureManager::TextureId TextureManager::RequestLoad(
  const VisualUrl&         url,
  const ImageDimensions    desiredSize,
  FittingMode::Type        fittingMode,
  Dali::SamplingMode::Type samplingMode,
  const UseAtlas           useAtlas,
  TextureUploadObserver*   observer )
{
  // First check if the requested Texture is cached.
  const TextureHash textureHash = GenerateHash( url.GetUrl(), desiredSize, fittingMode, samplingMode, useAtlas );

  // Look up the texture by hash. Note: The extra parameters are used in case of a hash collision.
  int cacheIndex = FindCachedTexture( textureHash, url.GetUrl(), desiredSize, fittingMode, samplingMode, useAtlas );
  TextureManager::TextureId textureId = INVALID_TEXTURE_ID;

  // Check if the requested Texture exists in the cache.
  if( cacheIndex != INVALID_CACHE_INDEX )
  {
    // Mark this texture being used by another client resource.
    ++( mTextureInfoContainer[ cacheIndex ].referenceCount );
    textureId = mTextureInfoContainer[ cacheIndex ].textureId;

    DALI_LOG_INFO( gTextureManagerLogFilter, Debug::Concise, "TextureManager::RequestLoad( url=%s observer=%p ) Using cached texture @%d, textureId=%d\n", url.GetUrl().c_str(), observer, cacheIndex, textureId );
  }
  else
  {
    // We need a new Texture.
    textureId = GenerateUniqueTextureId();
    mTextureInfoContainer.push_back( TextureInfo( textureId, url.GetUrl(), desiredSize, fittingMode, samplingMode, false, useAtlas, textureHash ) );
    cacheIndex = mTextureInfoContainer.size() - 1u;

    DALI_LOG_INFO( gTextureManagerLogFilter, Debug::Concise, "TextureManager::RequestLoad( url=%s observer=%p ) New texture, cacheIndex:%d, textureId=%d\n", url.GetUrl().c_str(), observer, cacheIndex, textureId );
  }

  // The below code path is common whether we are using the cache or not.
  // The textureInfoIndex now refers to either a pre-existing cached TextureInfo, or a new TextureInfo just created.
  TextureInfo& textureInfo( mTextureInfoContainer[ cacheIndex ] );

  DALI_LOG_INFO( gTextureManagerLogFilter, Debug::Concise, "TextureInfo loadState:%s\n",
                 textureInfo.loadState == TextureManager::NOT_STARTED ? "NOT_STARTED" :
                 textureInfo.loadState == TextureManager::LOADING ? "LOADING" :
                 textureInfo.loadState == TextureManager::UPLOADED ? "UPLOADED" :
                 textureInfo.loadState == TextureManager::CANCELLED ? "CANCELLED" : "Unknown" );

  // Check if we should add the observer. Only do this if we have not loaded yet and it will not have loaded by the end of this method.
  switch( textureInfo.loadState )
  {
    case TextureManager::NOT_STARTED:
    {
      LoadTexture( textureInfo );
      ObserveTexture( textureInfo, observer );
      break;
    }
    case TextureManager::LOADING:
    {
      ObserveTexture( textureInfo, observer );
      break;
    }
    case TextureManager::UPLOADED:
    {
      if( observer )
      {
        // The Texture has already loaded. The other observers have already been notified.
        // We need to send a "late" loaded notification for this observer.
        observer->UploadComplete( textureInfo.loadingSucceeded,
                                  textureInfo.textureSet, textureInfo.useAtlas,
                                  textureInfo.atlasRect );
      }
      break;
    }
    case TextureManager::CANCELLED:
    {
      // A cancelled texture hasn't finished loading yet. Treat as a loading texture
      // (it's ref count has already been incremented, above)
      textureInfo.loadState = TextureManager::LOADING;
      ObserveTexture( textureInfo, observer );
      break;
    }
  }

  // Return the TextureId for which this Texture can now be referenced by externally.
  return textureId;
}

void TextureManager::Remove( const TextureManager::TextureId textureId )
{
  int textureInfoIndex = GetCacheIndexFromId( textureId );
  if( textureInfoIndex != INVALID_INDEX )
  {
    TextureInfo& textureInfo( mTextureInfoContainer[ textureInfoIndex ] );


    DALI_LOG_INFO( gTextureManagerLogFilter, Debug::Concise, "TextureManager::Remove(%d) cacheIdx:%d loadState:%s\n",
                   textureId, textureInfoIndex,
                   textureInfo.loadState == TextureManager::NOT_STARTED ? "NOT_STARTED" :
                   textureInfo.loadState == TextureManager::LOADING ? "LOADING" :
                   textureInfo.loadState == TextureManager::UPLOADED ? "UPLOADED" :
                   textureInfo.loadState == TextureManager::CANCELLED ? "CANCELLED" : "Unknown" );

    // Decrement the reference count and check if this is the last user of this Texture.
    if( --textureInfo.referenceCount <= 0 )
    {
      // This is the last remove for this Texture.
      textureInfo.referenceCount = 0;
      bool removeTextureInfo = false;

      // If loaded, we can remove the TextureInfo and the Atlas (if atlased).
      if( textureInfo.loadState == UPLOADED )
      {
        if( textureInfo.atlas )
        {
          textureInfo.atlas.Remove( textureInfo.atlasRect );
        }
        removeTextureInfo = true;
      }
      else if( textureInfo.loadState == LOADING )
      {
        // We mark the textureInfo for removal.
        // Once the load has completed, this method will be called again.
        textureInfo.loadState = CANCELLED;
      }
      else
      {
        // In other states, we are not waiting for a load so we are safe to remove the TextureInfo data.
        removeTextureInfo = true;
      }

      // If the state allows us to remove the TextureInfo data, we do so.
      if( removeTextureInfo )
      {
        // Permanently remove the textureInfo struct.
        mTextureInfoContainer.erase( mTextureInfoContainer.begin() + textureInfoIndex );
      }
    }
  }
}

TextureManager::LoadState TextureManager::GetTextureState( TextureId textureId )
{
  LoadState loadState = TextureManager::NOT_STARTED;

  int cacheIndex = GetCacheIndexFromId( textureId );
  if( cacheIndex != INVALID_CACHE_INDEX )
  {
    TextureInfo& cachedTextureInfo( mTextureInfoContainer[ cacheIndex ] );
    loadState = cachedTextureInfo.loadState;
  }
  return loadState;
}

TextureSet TextureManager::GetTextureSet( TextureId textureId )
{
  TextureSet textureSet;// empty handle

  int cacheIndex = GetCacheIndexFromId( textureId );
  if( cacheIndex != INVALID_CACHE_INDEX )
  {
    TextureInfo& cachedTextureInfo( mTextureInfoContainer[ cacheIndex ] );
    textureSet = cachedTextureInfo.textureSet;
  }
  return textureSet;
}



bool TextureManager::LoadTexture( TextureInfo& textureInfo )
{
  bool success = true;

  if( textureInfo.loadState == NOT_STARTED )
  {
    textureInfo.loadState = LOADING;

    if( !textureInfo.loadSynchronously )
    {
      if( textureInfo.url.IsLocal() )
      {
        mAsyncLocalLoadingInfoContainer.push_back( AsyncLoadingInfo( textureInfo.textureId ) );
        mAsyncLocalLoadingInfoContainer.back().loadId = GetImplementation(mAsyncLocalLoader).Load(
          textureInfo.url, textureInfo.desiredSize,
          textureInfo.fittingMode, textureInfo.samplingMode, true );
      }
      else
      {
        mAsyncRemoteLoadingInfoContainer.push_back( AsyncLoadingInfo( textureInfo.textureId ) );
        mAsyncRemoteLoadingInfoContainer.back().loadId = GetImplementation(mAsyncRemoteLoader).Load(
          textureInfo.url, textureInfo.desiredSize,
          textureInfo.fittingMode, textureInfo.samplingMode, true );
      }
    }
  }

  return success;
}

void TextureManager::ObserveTexture( TextureInfo& textureInfo,
                                     TextureUploadObserver* observer )
{
  if( observer )
  {
    textureInfo.observerList.PushBack( observer );
    observer->DestructionSignal().Connect( this, &TextureManager::ObserverDestroyed );
  }
}

void TextureManager::AsyncLocalLoadComplete( uint32_t id, PixelData pixelData )
{
  AsyncLoadComplete( mAsyncLocalLoadingInfoContainer, id, pixelData );
}

void TextureManager::AsyncRemoteLoadComplete( uint32_t id, PixelData pixelData )
{
  AsyncLoadComplete( mAsyncRemoteLoadingInfoContainer, id, pixelData );
}

void TextureManager::AsyncLoadComplete( AsyncLoadingInfoContainerType& loadingContainer, uint32_t id, PixelData pixelData )
{
  DALI_LOG_INFO( gTextureManagerLogFilter, Debug::Concise, "TextureManager::AsyncLoadComplete( id:%d )\n", id );

  if( loadingContainer.size() >= 1u )
  {
    AsyncLoadingInfo loadingInfo = loadingContainer.front();

    if( loadingInfo.loadId == id )
    {
      int cacheIndex = GetCacheIndexFromId( loadingInfo.textureId );
      if( cacheIndex != INVALID_CACHE_INDEX )
      {
        // Once we have found the TextureInfo data, we call a common function used to process loaded data for both sync and async loads.
        TextureInfo& textureInfo( mTextureInfoContainer[cacheIndex] );

        DALI_LOG_INFO( gTextureManagerLogFilter, Debug::Concise, "  CacheIndex:%d LoadState: %d\n", cacheIndex, textureInfo.loadState );

        // Only perform atlasing if the load has not been cancelled since the request.
        if( textureInfo.loadState != CANCELLED )
        {
          // Perform atlasing and finalize the load.
          PostLoad( textureInfo, pixelData );
        }
        else
        {
          Remove( textureInfo.textureId );
        }
      }
    }

    loadingContainer.pop_front();
  }
}


bool TextureManager::PostLoad( TextureInfo& textureInfo, PixelData pixelData )
{
  bool success = false;

  // Was the load successful?
  if( pixelData && ( pixelData.GetWidth() != 0 ) && ( pixelData.GetHeight() != 0 ) )
  {
    // Regardless of whether the atlasing succeeds or not, we have a valid image, so we mark it as successful.
    success = true;

    bool usingAtlas = false;

    // No atlas support for now
    textureInfo.useAtlas = NO_ATLAS;

    if( ! usingAtlas )
    {
      DALI_LOG_INFO( gTextureManagerLogFilter, Debug::Concise, "  TextureManager::PostLoad() textureId:%d\n", textureInfo.textureId );

      Texture texture = Texture::New( Dali::TextureType::TEXTURE_2D, pixelData.GetPixelFormat(), pixelData.GetWidth(), pixelData.GetHeight() );
      texture.Upload( pixelData );
      textureInfo.textureSet = TextureSet::New();
      textureInfo.textureSet.SetTexture( 0u, texture );
    }
  }

  if( ! success )
  {
    DALI_LOG_ERROR( "TextureManager::AsyncImageLoad(%s) failed\n", textureInfo.url.GetUrl().c_str() );
    // @todo If the load was unsuccessful, upload the broken image.
  }

  // Update the load state.
  // Note: This is regardless of success as we care about whether a
  // load attempt is in progress or not.  If unsuccessful, a broken
  // image is still loaded.
  textureInfo.loadState = UPLOADED;

  // We need to store the load succeeded state as if a future request to load this texture comes in,
  // we need to re-broadcast the UploadComplete notification to that observer.
  textureInfo.loadingSucceeded = success;

  // If there is an observer: Notify the load is complete, whether successful or not:
  const unsigned int observerCount = textureInfo.observerList.Count();
  for( unsigned int i = 0; i < observerCount; ++i )
  {
    TextureUploadObserver* observer = textureInfo.observerList[i];
    if( observer )
    {
      observer->UploadComplete( success, textureInfo.textureSet, textureInfo.useAtlas, textureInfo.atlasRect );
      observer->DestructionSignal().Disconnect( this, &TextureManager::ObserverDestroyed );
    }
  }

  textureInfo.observerList.Clear();

  return success;
}

TextureManager::TextureId TextureManager::GenerateUniqueTextureId()
{
  return mCurrentTextureId++;
}

int TextureManager::GetCacheIndexFromId( const TextureId textureId )
{
  const unsigned int size = mTextureInfoContainer.size();

  for( unsigned int i = 0; i < size; ++i )
  {
    if( mTextureInfoContainer[i].textureId == textureId )
    {
      return i;
    }
  }

  DALI_LOG_WARNING( "Cannot locate TextureId: %d\n", textureId );
  return INVALID_CACHE_INDEX;
}

TextureManager::TextureHash TextureManager::GenerateHash(
  const std::string&             url,
  const ImageDimensions          size,
  const FittingMode::Type        fittingMode,
  const Dali::SamplingMode::Type samplingMode,
  const UseAtlas                 useAtlas )
{
  std::string hashTarget( url );
  const size_t urlLength = hashTarget.length();
  const uint16_t width = size.GetWidth();
  const uint16_t height = size.GetWidth();

  // If either the width or height has been specified, include the resizing options in the hash
  if( width != 0 || height != 0 )
  {
    // We are appending 5 bytes to the URL to form the hash input.
    hashTarget.resize( urlLength + 5u );
    char* hashTargetPtr = &( hashTarget[ urlLength ] );

    // Pack the width and height (4 bytes total).
    *hashTargetPtr++ = size.GetWidth() & 0xff;
    *hashTargetPtr++ = ( size.GetWidth() >> 8u ) & 0xff;
    *hashTargetPtr++ = size.GetHeight() & 0xff;
    *hashTargetPtr++ = ( size.GetHeight() >> 8u ) & 0xff;

    // Bit-pack the FittingMode, SamplingMode and atlasing.
    // FittingMode=2bits, SamplingMode=3bits, useAtlas=1bit
    *hashTargetPtr   = ( fittingMode << 4u ) | ( samplingMode << 1 ) | useAtlas;
  }
  else
  {
    // We are not including sizing information, but we still need an extra byte for atlasing.
    hashTarget.resize( urlLength + 1u );
    // Add the atlasing to the hash input.
    hashTarget[ urlLength ] = useAtlas;
  }

  return Dali::CalculateHash( hashTarget );
}

int TextureManager::FindCachedTexture(
  const TextureManager::TextureHash hash,
  const std::string&                url,
  const ImageDimensions             size,
  const FittingMode::Type           fittingMode,
  const Dali::SamplingMode::Type    samplingMode,
  const bool                        useAtlas )
{
  // Default to an invalid ID, in case we do not find a match.
  int cacheIndex = INVALID_CACHE_INDEX;

  // Iterate through our hashes to find a match.
  const unsigned int count = mTextureInfoContainer.size();
  for( unsigned int i = 0u; i < count; ++i )
  {
    if( mTextureInfoContainer[i].hash == hash )
    {
      // We have a match, now we check all the original parameters in case of a hash collision.
      TextureInfo& textureInfo( mTextureInfoContainer[i] );

      if( ( url == textureInfo.url.GetUrl() ) &&
          ( useAtlas == textureInfo.useAtlas ) &&
          ( size == textureInfo.desiredSize ) &&
          ( ( size.GetWidth() == 0 && size.GetHeight() == 0 ) ||
            ( fittingMode == textureInfo.fittingMode &&
              samplingMode == textureInfo.samplingMode ) ) )
      {
        // The found Texture is a match.
        cacheIndex = i;
        break;
      }
    }
  }

  return cacheIndex;
}

void TextureManager::ObserverDestroyed( TextureUploadObserver* observer )
{
  const unsigned int count = mTextureInfoContainer.size();
  for( unsigned int i = 0; i < count; ++i )
  {
    TextureInfo& textureInfo( mTextureInfoContainer[i] );
    for( TextureInfo::ObserverListType::Iterator j = textureInfo.observerList.Begin(); j != textureInfo.observerList.End(); ++j )
    {
      if( *j == observer )
      {
        textureInfo.observerList.Erase( j );
        break;
      }
    }
  }
}

TextureManager::~TextureManager()
{
  mTextureInfoContainer.clear();
  mAsyncLocalLoadingInfoContainer.clear();
  mAsyncRemoteLoadingInfoContainer.clear();
}




} // namespace Internal

} // namespace Toolkit

} // namespace Dali

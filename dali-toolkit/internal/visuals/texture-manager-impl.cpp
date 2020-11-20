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
 *
 */

// CLASS HEADER
#include <dali-toolkit/internal/visuals/texture-manager-impl.h>

// EXTERNAL HEADERS
#include <cstdlib>
#include <string>
#include <dali/public-api/math/vector4.h>
#include <dali/devel-api/adaptor-framework/environment-variable.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/common/hash.h>
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/rendering/geometry.h>

// INTERNAL HEADERS
#include <dali-toolkit/internal/image-loader/image-atlas-impl.h>
#include <dali-toolkit/public-api/image-loader/sync-image-loader.h>
#include <dali-toolkit/internal/visuals/image-atlas-manager.h>
#include <dali-toolkit/internal/visuals/rendering-addon.h>

namespace
{

constexpr auto DEFAULT_NUMBER_OF_LOCAL_LOADER_THREADS = size_t{4u};
constexpr auto DEFAULT_NUMBER_OF_REMOTE_LOADER_THREADS = size_t{8u};

constexpr auto NUMBER_OF_LOCAL_LOADER_THREADS_ENV = "DALI_TEXTURE_LOCAL_THREADS";
constexpr auto NUMBER_OF_REMOTE_LOADER_THREADS_ENV = "DALI_TEXTURE_REMOTE_THREADS";

size_t GetNumberOfThreads(const char* environmentVariable, size_t defaultValue)
{
  using Dali::EnvironmentVariable::GetEnvironmentVariable;
  auto numberString = GetEnvironmentVariable(environmentVariable);
  auto numberOfThreads = numberString ? std::strtoul(numberString, nullptr, 10) : 0;
  constexpr auto MAX_NUMBER_OF_THREADS = 100u;
  DALI_ASSERT_DEBUG( numberOfThreads < MAX_NUMBER_OF_THREADS );
  return ( numberOfThreads > 0 && numberOfThreads < MAX_NUMBER_OF_THREADS ) ? numberOfThreads : defaultValue;
}

size_t GetNumberOfLocalLoaderThreads()
{
  return GetNumberOfThreads(NUMBER_OF_LOCAL_LOADER_THREADS_ENV, DEFAULT_NUMBER_OF_LOCAL_LOADER_THREADS);
}

size_t GetNumberOfRemoteLoaderThreads()
{
  return GetNumberOfThreads(NUMBER_OF_REMOTE_LOADER_THREADS_ENV, DEFAULT_NUMBER_OF_REMOTE_LOADER_THREADS);
}

} // namespace

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

#define GET_LOAD_STATE_STRING( loadState ) \
  loadState == TextureManager::LoadState::NOT_STARTED ? "NOT_STARTED" :             \
    loadState == TextureManager::LoadState::LOADING ? "LOADING" :                   \
    loadState == TextureManager::LoadState::LOAD_FINISHED ? "LOAD_FINISHED" :       \
    loadState == TextureManager::LoadState::WAITING_FOR_MASK ? "WAITING_FOR_MASK" : \
    loadState == TextureManager::LoadState::MASK_APPLYING ? "MASK_APPLYING" :         \
    loadState == TextureManager::LoadState::MASK_APPLIED ? "MASK_APPLIED" :         \
    loadState == TextureManager::LoadState::UPLOADED ? "UPLOADED" :                 \
    loadState == TextureManager::LoadState::CANCELLED ? "CANCELLED" :               \
    loadState == TextureManager::LoadState::LOAD_FAILED ? "LOAD_FAILED" : "Unknown"

#endif

const uint32_t      DEFAULT_ATLAS_SIZE( 1024u );                     ///< This size can fit 8 by 8 images of average size 128 * 128
const Vector4       FULL_ATLAS_RECT( 0.0f, 0.0f, 1.0f, 1.0f );       ///< UV Rectangle that covers the full Texture
const int           INVALID_INDEX( -1 );                             ///< Invalid index used to represent a non-existant TextureInfo struct
const int           INVALID_CACHE_INDEX( -1 ); ///< Invalid Cache index


void PreMultiply( Devel::PixelBuffer pixelBuffer, TextureManager::MultiplyOnLoad& preMultiplyOnLoad )
{
  if( Pixel::HasAlpha( pixelBuffer.GetPixelFormat() ) )
  {
    if( preMultiplyOnLoad == TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD )
    {
      pixelBuffer.MultiplyColorByAlpha();
    }
  }
  else
  {
    preMultiplyOnLoad = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  }
}

} // Anonymous namespace

TextureManager::MaskingData::MaskingData()
: mAlphaMaskUrl(),
  mAlphaMaskId( INVALID_TEXTURE_ID ),
  mContentScaleFactor( 1.0f ),
  mCropToMask( true )
{
}

TextureManager::TextureManager()
: mAsyncLocalLoaders( GetNumberOfLocalLoaderThreads(), [&]() { return AsyncLoadingHelper(*this); } ),
  mAsyncRemoteLoaders( GetNumberOfRemoteLoaderThreads(), [&]() { return AsyncLoadingHelper(*this); } ),
  mExternalTextures(),
  mLifecycleObservers(),
  mLoadQueue(),
  mBrokenImageUrl(""),
  mCurrentTextureId( 0 ),
  mQueueLoadFlag(false)
{
  // Initialize the AddOn
  RenderingAddOn::Get();
}

TextureManager::~TextureManager()
{
  for( auto iter = mLifecycleObservers.Begin(), endIter = mLifecycleObservers.End(); iter != endIter; ++iter)
  {
    (*iter)->TextureManagerDestroyed();
  }
}

TextureSet TextureManager::LoadAnimatedImageTexture(
  Dali::AnimatedImageLoading animatedImageLoading, uint32_t frameIndex, Dali::SamplingMode::Type samplingMode,
  bool synchronousLoading, TextureManager::TextureId& textureId, Dali::WrapMode::Type wrapModeU, Dali::WrapMode::Type wrapModeV, TextureUploadObserver* textureObserver )
{
  TextureSet textureSet;

  if( synchronousLoading )
  {
    Devel::PixelBuffer pixelBuffer;
    if( animatedImageLoading )
    {
      pixelBuffer = animatedImageLoading.LoadFrame( frameIndex );
    }
    if( !pixelBuffer )
    {
      // use broken image
      pixelBuffer = LoadImageFromFile( mBrokenImageUrl );
      PixelData pixelData;
      if( pixelBuffer )
      {
        pixelData = Devel::PixelBuffer::Convert(pixelBuffer); // takes ownership of buffer
      }
      Texture texture = Texture::New( Dali::TextureType::TEXTURE_2D, pixelData.GetPixelFormat(),
                                      pixelData.GetWidth(), pixelData.GetHeight() );
      texture.Upload( pixelData );
      textureSet = TextureSet::New();
      textureSet.SetTexture( 0u, texture );
    }
    else
    {
      PixelData pixelData = Devel::PixelBuffer::Convert(pixelBuffer); // takes ownership of buffer
      if( !textureSet )
      {
        Texture texture = Texture::New( Dali::TextureType::TEXTURE_2D, pixelData.GetPixelFormat(),
                                        pixelData.GetWidth(), pixelData.GetHeight() );
        texture.Upload( pixelData );
        textureSet = TextureSet::New();
        textureSet.SetTexture( 0u, texture );
      }
    }
  }
  else
  {
    auto preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
    textureId = RequestLoadInternal( animatedImageLoading.GetUrl(), INVALID_TEXTURE_ID, 1.0f, ImageDimensions(), FittingMode::SCALE_TO_FILL,
                                     SamplingMode::BOX_THEN_LINEAR, TextureManager::NO_ATLAS, false, StorageType::UPLOAD_TO_TEXTURE, textureObserver,
                                     true, TextureManager::ReloadPolicy::CACHED, preMultiply, animatedImageLoading, frameIndex );
    TextureManager::LoadState loadState = GetTextureStateInternal( textureId );
    if( loadState == TextureManager::LoadState::UPLOADED )
    {
      // UploadComplete has already been called - keep the same texture set
      textureSet = GetTextureSet( textureId );
    }
  }

  if( textureSet )
  {
    Sampler sampler = Sampler::New();
    sampler.SetWrapMode(  wrapModeU, wrapModeV  );
    textureSet.SetSampler( 0u, sampler );
  }

  return textureSet;
}

Devel::PixelBuffer TextureManager::LoadPixelBuffer(
  const VisualUrl& url, Dali::ImageDimensions desiredSize, Dali::FittingMode::Type fittingMode, Dali::SamplingMode::Type samplingMode, bool synchronousLoading, TextureUploadObserver* textureObserver, bool orientationCorrection, TextureManager::MultiplyOnLoad& preMultiplyOnLoad )
{
  Devel::PixelBuffer pixelBuffer;
  if( synchronousLoading )
  {
    if( url.IsValid() )
    {
      pixelBuffer = LoadImageFromFile( url.GetUrl(), desiredSize, fittingMode, samplingMode,
                                       orientationCorrection  );
      if( pixelBuffer && preMultiplyOnLoad == TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD )
      {
        PreMultiply( pixelBuffer, preMultiplyOnLoad );
      }
    }
  }
  else
  {
    RequestLoadInternal( url, INVALID_TEXTURE_ID, 1.0f, desiredSize, fittingMode, samplingMode, TextureManager::NO_ATLAS,
                         false, StorageType::RETURN_PIXEL_BUFFER, textureObserver, orientationCorrection, TextureManager::ReloadPolicy::FORCED,
                         preMultiplyOnLoad, Dali::AnimatedImageLoading(), 0u );
  }

  return pixelBuffer;
}

TextureSet TextureManager::LoadTexture(
  const VisualUrl& url, Dali::ImageDimensions desiredSize, Dali::FittingMode::Type fittingMode,
  Dali::SamplingMode::Type samplingMode, MaskingDataPointer& maskInfo,
  bool synchronousLoading, TextureManager::TextureId& textureId, Vector4& textureRect,
  Dali::ImageDimensions& textureRectSize, bool& atlasingStatus, bool& loadingStatus,
  Dali::WrapMode::Type wrapModeU, Dali::WrapMode::Type wrapModeV, TextureUploadObserver* textureObserver,
  AtlasUploadObserver* atlasObserver, ImageAtlasManagerPtr imageAtlasManager, bool orientationCorrection,
  TextureManager::ReloadPolicy reloadPolicy, TextureManager::MultiplyOnLoad& preMultiplyOnLoad )
{
  TextureSet textureSet;

  loadingStatus = false;
  textureRect = FULL_ATLAS_RECT;

  if( VisualUrl::TEXTURE == url.GetProtocolType())
  {
    std::string location = url.GetLocation();
    if( location.size() > 0u )
    {
      TextureId id = std::stoi( location );
      for( auto&& elem : mExternalTextures )
      {
        if( elem.textureId == id )
        {
          preMultiplyOnLoad = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
          textureId = elem.textureId;
          return elem.textureSet;
        }
      }
    }
  }
  else if( synchronousLoading )
  {
    PixelData data;
    if( url.IsValid() )
    {
      Devel::PixelBuffer pixelBuffer = LoadImageFromFile( url.GetUrl(), desiredSize, fittingMode, samplingMode,
                                       orientationCorrection  );
      if( maskInfo && maskInfo->mAlphaMaskUrl.IsValid() )
      {
        Devel::PixelBuffer maskPixelBuffer = LoadImageFromFile( maskInfo->mAlphaMaskUrl.GetUrl(), ImageDimensions(),
                                             FittingMode::SCALE_TO_FILL, SamplingMode::NO_FILTER, true  );
        if( maskPixelBuffer )
        {
          pixelBuffer.ApplyMask( maskPixelBuffer, maskInfo->mContentScaleFactor, maskInfo->mCropToMask );
        }
      }
      if( pixelBuffer )
      {
        PreMultiply( pixelBuffer, preMultiplyOnLoad );
        data = Devel::PixelBuffer::Convert(pixelBuffer); // takes ownership of buffer
      }
    }
    if( !data )
    {
      // use broken image
      Devel::PixelBuffer pixelBuffer = LoadImageFromFile( mBrokenImageUrl );
      if( pixelBuffer )
      {
        PreMultiply( pixelBuffer, preMultiplyOnLoad );
        data = Devel::PixelBuffer::Convert(pixelBuffer); // takes ownership of buffer
      }
      Texture texture = Texture::New( Dali::TextureType::TEXTURE_2D, data.GetPixelFormat(),
                                      data.GetWidth(), data.GetHeight() );
      texture.Upload( data );
      textureSet = TextureSet::New();
      textureSet.SetTexture( 0u, texture );
    }
    else
    {
      if( atlasingStatus ) // attempt atlasing
      {
        textureSet = imageAtlasManager->Add( textureRect, data );
      }
      if( !textureSet ) // big image, no atlasing or atlasing failed
      {
        atlasingStatus = false;
        Texture texture = Texture::New( Dali::TextureType::TEXTURE_2D, data.GetPixelFormat(),
                                        data.GetWidth(), data.GetHeight() );
        texture.Upload( data );
        textureSet = TextureSet::New();
        textureSet.SetTexture( 0u, texture );
      }
      else
      {
        textureRectSize.SetWidth(data.GetWidth());
        textureRectSize.SetHeight(data.GetHeight());
      }
    }
  }
  else
  {
    loadingStatus = true;
    if( atlasingStatus )
    {
      textureSet = imageAtlasManager->Add( textureRect, url.GetUrl(), desiredSize, fittingMode, true, atlasObserver);
    }
    if( !textureSet ) // big image, no atlasing or atlasing failed
    {
      atlasingStatus = false;
      if( !maskInfo || !maskInfo->mAlphaMaskUrl.IsValid() )
      {
        textureId = RequestLoad( url, desiredSize, fittingMode, samplingMode, TextureManager::NO_ATLAS,
                                 textureObserver, orientationCorrection, reloadPolicy, preMultiplyOnLoad );
      }
      else
      {
        maskInfo->mAlphaMaskId = RequestMaskLoad( maskInfo->mAlphaMaskUrl );
        textureId = RequestLoad( url,
                                 maskInfo->mAlphaMaskId,
                                 maskInfo->mContentScaleFactor,
                                 desiredSize,
                                 fittingMode, samplingMode,
                                 TextureManager::NO_ATLAS,
                                 maskInfo->mCropToMask,
                                 textureObserver,
                                 orientationCorrection,
                                 reloadPolicy, preMultiplyOnLoad );
      }

      TextureManager::LoadState loadState = GetTextureStateInternal( textureId );
      if( loadState == TextureManager::LoadState::UPLOADED )
      {
        // UploadComplete has already been called - keep the same texture set
        textureSet = GetTextureSet( textureId );
      }

      // If we are loading the texture, or waiting for the ready signal handler to complete, inform
      // caller that they need to wait.
      loadingStatus = ( loadState == TextureManager::LoadState::LOADING ||
                        loadState == TextureManager::LoadState::WAITING_FOR_MASK ||
                        loadState == TextureManager::LoadState::MASK_APPLYING ||
                        loadState == TextureManager::LoadState::MASK_APPLIED ||
                        loadState == TextureManager::LoadState::NOT_STARTED ||
                        mQueueLoadFlag );

    }
    else
    {
      textureRectSize = desiredSize;
    }
  }

  if( ! atlasingStatus && textureSet )
  {
    Sampler sampler = Sampler::New();
    sampler.SetWrapMode(  wrapModeU, wrapModeV  );
    textureSet.SetSampler( 0u, sampler );
  }

  return textureSet;
}

TextureManager::TextureId TextureManager::RequestLoad(
  const VisualUrl&                url,
  const ImageDimensions           desiredSize,
  FittingMode::Type               fittingMode,
  Dali::SamplingMode::Type        samplingMode,
  const UseAtlas                  useAtlas,
  TextureUploadObserver*          observer,
  bool                            orientationCorrection,
  TextureManager::ReloadPolicy    reloadPolicy,
  TextureManager::MultiplyOnLoad& preMultiplyOnLoad )
{
  return RequestLoadInternal( url, INVALID_TEXTURE_ID, 1.0f, desiredSize, fittingMode, samplingMode, useAtlas,
                              false, StorageType::UPLOAD_TO_TEXTURE, observer, orientationCorrection, reloadPolicy,
                              preMultiplyOnLoad, Dali::AnimatedImageLoading(), 0u );
}

TextureManager::TextureId TextureManager::RequestLoad(
  const VisualUrl&                url,
  TextureId                       maskTextureId,
  float                           contentScale,
  const ImageDimensions           desiredSize,
  FittingMode::Type               fittingMode,
  Dali::SamplingMode::Type        samplingMode,
  const UseAtlas                  useAtlas,
  bool                            cropToMask,
  TextureUploadObserver*          observer,
  bool                            orientationCorrection,
  TextureManager::ReloadPolicy    reloadPolicy,
  TextureManager::MultiplyOnLoad& preMultiplyOnLoad )
{
  return RequestLoadInternal( url, maskTextureId, contentScale, desiredSize, fittingMode, samplingMode, useAtlas,
                              cropToMask, StorageType::UPLOAD_TO_TEXTURE, observer, orientationCorrection, reloadPolicy,
                              preMultiplyOnLoad, Dali::AnimatedImageLoading(), 0u );
}

TextureManager::TextureId TextureManager::RequestMaskLoad( const VisualUrl& maskUrl )
{
  // Use the normal load procedure to get the alpha mask.
  auto preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  return RequestLoadInternal( maskUrl, INVALID_TEXTURE_ID, 1.0f, ImageDimensions(), FittingMode::SCALE_TO_FILL,
                              SamplingMode::NO_FILTER, NO_ATLAS, false, StorageType::KEEP_PIXEL_BUFFER, NULL, true,
                              TextureManager::ReloadPolicy::CACHED, preMultiply, Dali::AnimatedImageLoading(), 0u );
}

TextureManager::TextureId TextureManager::RequestLoadInternal(
  const VisualUrl&                url,
  TextureId                       maskTextureId,
  float                           contentScale,
  const ImageDimensions           desiredSize,
  FittingMode::Type               fittingMode,
  Dali::SamplingMode::Type        samplingMode,
  UseAtlas                        useAtlas,
  bool                            cropToMask,
  StorageType                     storageType,
  TextureUploadObserver*          observer,
  bool                            orientationCorrection,
  TextureManager::ReloadPolicy    reloadPolicy,
  TextureManager::MultiplyOnLoad& preMultiplyOnLoad,
  Dali::AnimatedImageLoading      animatedImageLoading,
  uint32_t                        frameIndex )
{
  // First check if the requested Texture is cached.
  bool isAnimatedImage = ( animatedImageLoading ) ? true : false;
  const TextureHash textureHash = GenerateHash( url.GetUrl(), desiredSize, fittingMode, samplingMode, useAtlas,
                                                maskTextureId, storageType, isAnimatedImage, frameIndex );

  TextureManager::TextureId textureId = INVALID_TEXTURE_ID;
  // Look up the texture by hash. Note: The extra parameters are used in case of a hash collision.
  int cacheIndex = FindCachedTexture( textureHash, url.GetUrl(), desiredSize, fittingMode, samplingMode, useAtlas,
                                      maskTextureId, preMultiplyOnLoad, storageType, isAnimatedImage, frameIndex );

  // Check if the requested Texture exists in the cache.
  if( cacheIndex != INVALID_CACHE_INDEX )
  {
    if ( TextureManager::ReloadPolicy::CACHED == reloadPolicy )
    {
      // Mark this texture being used by another client resource. Forced reload would replace the current texture
      // without the need for incrementing the reference count.
      ++( mTextureInfoContainer[ cacheIndex ].referenceCount );
    }
    textureId = mTextureInfoContainer[ cacheIndex ].textureId;

    // Update preMultiplyOnLoad value. It should be changed according to preMultiplied value of the cached info.
    preMultiplyOnLoad = mTextureInfoContainer[ cacheIndex ].preMultiplied ? TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD : TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;

    DALI_LOG_INFO( gTextureManagerLogFilter, Debug::General, "TextureManager::RequestLoad( url=%s observer=%p ) Using cached texture id@%d, textureId=%d\n",
                   url.GetUrl().c_str(), observer, cacheIndex, textureId );
  }

  if( textureId == INVALID_TEXTURE_ID ) // There was no caching, or caching not required
  {
    // We need a new Texture.
    textureId = GenerateUniqueTextureId();
    bool preMultiply = ( preMultiplyOnLoad == TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD );
    mTextureInfoContainer.push_back( TextureInfo( textureId, maskTextureId, url.GetUrl(),
                                                  desiredSize, contentScale, fittingMode, samplingMode,
                                                  false, cropToMask, useAtlas, textureHash, orientationCorrection,
                                                  preMultiply, animatedImageLoading, frameIndex ) );
    cacheIndex = mTextureInfoContainer.size() - 1u;

    DALI_LOG_INFO( gTextureManagerLogFilter, Debug::General, "TextureManager::RequestLoad( url=%s observer=%p ) New texture, cacheIndex:%d, textureId=%d\n",
                   url.GetUrl().c_str(), observer, cacheIndex, textureId );
  }

  // The below code path is common whether we are using the cache or not.
  // The textureInfoIndex now refers to either a pre-existing cached TextureInfo,
  // or a new TextureInfo just created.
  TextureInfo& textureInfo( mTextureInfoContainer[ cacheIndex ] );
  textureInfo.maskTextureId = maskTextureId;
  textureInfo.storageType = storageType;
  textureInfo.orientationCorrection = orientationCorrection;

  DALI_LOG_INFO( gTextureManagerLogFilter, Debug::General, "TextureInfo loadState:%s\n",
                 GET_LOAD_STATE_STRING(textureInfo.loadState ) );

  // Force reloading of texture by setting loadState unless already loading or cancelled.
  if ( TextureManager::ReloadPolicy::FORCED == reloadPolicy &&
       TextureManager::LoadState::LOADING != textureInfo.loadState &&
       TextureManager::LoadState::WAITING_FOR_MASK != textureInfo.loadState &&
       TextureManager::LoadState::MASK_APPLYING != textureInfo.loadState &&
       TextureManager::LoadState::MASK_APPLIED != textureInfo.loadState &&
       TextureManager::LoadState::CANCELLED != textureInfo.loadState )
  {
    DALI_LOG_INFO( gTextureManagerLogFilter, Debug::Verbose, "TextureManager::RequestLoad( url=%s observer=%p ) ForcedReload cacheIndex:%d, textureId=%d\n",
                   url.GetUrl().c_str(), observer, cacheIndex, textureId );

    textureInfo.loadState = TextureManager::LoadState::NOT_STARTED;
  }

  // Check if we should add the observer.
  // Only do this if we have not loaded yet and it will not have loaded by the end of this method.
  switch( textureInfo.loadState )
  {
    case TextureManager::LoadState::LOAD_FAILED: // Failed notifies observer which then stops observing.
    case TextureManager::LoadState::NOT_STARTED:
    {
      LoadOrQueueTexture( textureInfo, observer ); // If called inside NotifyObservers, queues until afterwards
      break;
    }
    case TextureManager::LoadState::LOADING:
    case TextureManager::LoadState::WAITING_FOR_MASK:
    case TextureManager::LoadState::MASK_APPLYING:
    case TextureManager::LoadState::MASK_APPLIED:
    {
      ObserveTexture( textureInfo, observer );
      break;
    }
    case TextureManager::LoadState::UPLOADED:
    {
      if( observer )
      {
        LoadOrQueueTexture( textureInfo, observer );
      }
      break;
    }
    case TextureManager::LoadState::CANCELLED:
    {
      // A cancelled texture hasn't finished loading yet. Treat as a loading texture
      // (it's ref count has already been incremented, above)
      textureInfo.loadState = TextureManager::LoadState::LOADING;
      ObserveTexture( textureInfo, observer );
      break;
    }
    case TextureManager::LoadState::LOAD_FINISHED:
    {
      // Loading has already completed.
      if( observer && textureInfo.storageType == StorageType::RETURN_PIXEL_BUFFER )
      {
        LoadOrQueueTexture( textureInfo, observer );
      }
      break;
    }
  }

  // Return the TextureId for which this Texture can now be referenced by externally.
  return textureId;
}

void TextureManager::Remove( const TextureManager::TextureId textureId, TextureUploadObserver* observer )
{
  int textureInfoIndex = GetCacheIndexFromId( textureId );
  if( textureInfoIndex != INVALID_INDEX )
  {
    TextureInfo& textureInfo( mTextureInfoContainer[ textureInfoIndex ] );

    DALI_LOG_INFO( gTextureManagerLogFilter, Debug::Concise,
                   "TextureManager::Remove(%d) url:%s\n  cacheIdx:%d loadState:%s reference count = %d\n",
                   textureId, textureInfo.url.GetUrl().c_str(),
                   textureInfoIndex, GET_LOAD_STATE_STRING( textureInfo.loadState ), textureInfo.referenceCount );

    // Decrement the reference count and check if this is the last user of this Texture.
    if( --textureInfo.referenceCount <= 0 )
    {
      // This is the last remove for this Texture.
      textureInfo.referenceCount = 0;
      bool removeTextureInfo = false;

      // If loaded, we can remove the TextureInfo and the Atlas (if atlased).
      if( textureInfo.loadState == LoadState::UPLOADED )
      {
        if( textureInfo.atlas )
        {
          textureInfo.atlas.Remove( textureInfo.atlasRect );
        }
        removeTextureInfo = true;
      }
      else if( textureInfo.loadState == LoadState::LOADING )
      {
        // We mark the textureInfo for removal.
        // Once the load has completed, this method will be called again.
        textureInfo.loadState = LoadState::CANCELLED;
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

    if( observer )
    {
      // Remove element from the LoadQueue
      for( auto&& element : mLoadQueue )
      {
        if( element.mObserver == observer )
        {
          mLoadQueue.Erase( &element );
          break;
        }
      }
    }
  }
}

VisualUrl TextureManager::GetVisualUrl( TextureId textureId )
{
  VisualUrl visualUrl("");
  int cacheIndex = GetCacheIndexFromId( textureId );

  if( cacheIndex != INVALID_CACHE_INDEX )
  {
    DALI_LOG_INFO( gTextureManagerLogFilter, Debug::Concise, "TextureManager::GetVisualUrl. Using cached texture id=%d, textureId=%d\n",
                   cacheIndex, textureId );

    TextureInfo& cachedTextureInfo( mTextureInfoContainer[ cacheIndex ] );
    visualUrl = cachedTextureInfo.url;
  }
  return visualUrl;
}

TextureManager::LoadState TextureManager::GetTextureState( TextureId textureId )
{
  LoadState loadState = TextureManager::LoadState::NOT_STARTED;

  int cacheIndex = GetCacheIndexFromId( textureId );
  if( cacheIndex != INVALID_CACHE_INDEX )
  {
    TextureInfo& cachedTextureInfo( mTextureInfoContainer[ cacheIndex ] );
    loadState = cachedTextureInfo.loadState;
  }
  else
  {
    for( auto&& elem : mExternalTextures )
    {
      if( elem.textureId == textureId )
      {
        loadState = LoadState::UPLOADED;
        break;
      }
    }
  }
  return loadState;
}

TextureManager::LoadState TextureManager::GetTextureStateInternal( TextureId textureId )
{
  LoadState loadState = TextureManager::LoadState::NOT_STARTED;

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
  else
  {
    for( auto&& elem : mExternalTextures )
    {
      if( elem.textureId == textureId )
      {
        textureSet = elem.textureSet;
        break;
      }
    }
  }
  return textureSet;
}

std::string TextureManager::AddExternalTexture( TextureSet& textureSet )
{
  TextureManager::ExternalTextureInfo info;
  info.textureId = GenerateUniqueTextureId();
  info.textureSet = textureSet;
  mExternalTextures.emplace_back( info );
  return VisualUrl::CreateTextureUrl( std::to_string( info.textureId ) );
}

TextureSet TextureManager::RemoveExternalTexture( const std::string& url )
{
  if( url.size() > 0u )
  {
    // get the location from the Url
    VisualUrl parseUrl( url );
    if( VisualUrl::TEXTURE == parseUrl.GetProtocolType() )
    {
      std::string location = parseUrl.GetLocation();
      if( location.size() > 0u )
      {
        TextureId id = std::stoi( location );
        const auto end = mExternalTextures.end();
        for( auto iter = mExternalTextures.begin(); iter != end; ++iter )
        {
          if( iter->textureId == id )
          {
            auto textureSet = iter->textureSet;
            mExternalTextures.erase( iter );
            return textureSet;
          }
        }
      }
    }
  }
  return TextureSet();
}

void TextureManager::AddObserver( TextureManager::LifecycleObserver& observer )
{
  // make sure an observer doesn't observe the same object twice
  // otherwise it will get multiple calls to ObjectDestroyed()
  DALI_ASSERT_DEBUG( mLifecycleObservers.End() == std::find( mLifecycleObservers.Begin(), mLifecycleObservers.End(), &observer));
  mLifecycleObservers.PushBack( &observer );
}

void TextureManager::RemoveObserver( TextureManager::LifecycleObserver& observer)
{
  // Find the observer...
  auto endIter =  mLifecycleObservers.End();
  for( auto iter = mLifecycleObservers.Begin(); iter != endIter; ++iter)
  {
    if( (*iter) == &observer)
    {
      mLifecycleObservers.Erase( iter );
      break;
    }
  }
  DALI_ASSERT_DEBUG(endIter != mLifecycleObservers.End());
}

void TextureManager::LoadOrQueueTexture( TextureInfo& textureInfo, TextureUploadObserver* observer )
{
  switch( textureInfo.loadState )
  {
    case LoadState::NOT_STARTED:
    case LoadState::LOAD_FAILED:
    {
      if( mQueueLoadFlag )
      {
        QueueLoadTexture( textureInfo, observer );
      }
      else
      {
        LoadTexture( textureInfo, observer );
      }
      break;
    }
    case LoadState::UPLOADED:
    {
      if( mQueueLoadFlag )
      {
        QueueLoadTexture( textureInfo, observer );
      }
      else
      {
        // The Texture has already loaded. The other observers have already been notified.
        // We need to send a "late" loaded notification for this observer.
        observer->UploadComplete( true, textureInfo.textureId, textureInfo.textureSet,
                                  textureInfo.useAtlas, textureInfo.atlasRect,
                                  textureInfo.preMultiplied );
      }
      break;
    }
    case LoadState::LOADING:
    case LoadState::CANCELLED:
    case LoadState::LOAD_FINISHED:
    case LoadState::WAITING_FOR_MASK:
    case LoadState::MASK_APPLYING:
    case LoadState::MASK_APPLIED:
    {
      break;
    }
  }
}

void TextureManager::QueueLoadTexture( TextureInfo& textureInfo, TextureUploadObserver* observer )
{
  auto textureId = textureInfo.textureId;
  mLoadQueue.PushBack( LoadQueueElement( textureId, observer) );

  observer->DestructionSignal().Connect( this, &TextureManager::ObserverDestroyed );
}

void TextureManager::LoadTexture( TextureInfo& textureInfo, TextureUploadObserver* observer )
{
  DALI_LOG_INFO( gTextureManagerLogFilter, Debug::Concise, "TextureManager::LoadTexture(): url:%s sync:%s\n",
                 textureInfo.url.GetUrl().c_str(), textureInfo.loadSynchronously?"T":"F" );

  textureInfo.loadState = LoadState::LOADING;
  if( !textureInfo.loadSynchronously )
  {
    auto& loadersContainer = textureInfo.url.IsLocalResource() ? mAsyncLocalLoaders : mAsyncRemoteLoaders;
    auto loadingHelperIt = loadersContainer.GetNext();
    auto premultiplyOnLoad = ( textureInfo.preMultiplyOnLoad && textureInfo.maskTextureId == INVALID_TEXTURE_ID ) ?
                               DevelAsyncImageLoader::PreMultiplyOnLoad::ON : DevelAsyncImageLoader::PreMultiplyOnLoad::OFF;
    DALI_ASSERT_ALWAYS(loadingHelperIt != loadersContainer.End());
    if( textureInfo.animatedImageLoading )
    {
      loadingHelperIt->LoadAnimatedImage( textureInfo.textureId, textureInfo.animatedImageLoading, textureInfo.frameIndex );
    }
    else
    {
      loadingHelperIt->Load(textureInfo.textureId, textureInfo.url,
                            textureInfo.desiredSize, textureInfo.fittingMode,
                            textureInfo.samplingMode, textureInfo.orientationCorrection,
                            premultiplyOnLoad );
    }
  }
  ObserveTexture( textureInfo, observer );
}

void TextureManager::ProcessQueuedTextures()
{
  for( auto&& element : mLoadQueue )
  {
    if( !element.mObserver )
    {
      continue;
    }

    int cacheIndex = GetCacheIndexFromId( element.mTextureId );
    if( cacheIndex != INVALID_CACHE_INDEX )
    {
      TextureInfo& textureInfo( mTextureInfoContainer[cacheIndex] );
      if( textureInfo.loadState == LoadState::UPLOADED )
      {
        element.mObserver->UploadComplete( true, textureInfo.textureId, textureInfo.textureSet,
                                           textureInfo.useAtlas, textureInfo.atlasRect,
                                           textureInfo.preMultiplied );
      }
      else if ( textureInfo.loadState == LoadState::LOAD_FINISHED && textureInfo.storageType == StorageType::RETURN_PIXEL_BUFFER )
      {
        element.mObserver->LoadComplete( true, textureInfo.pixelBuffer, textureInfo.url, textureInfo.preMultiplied );
      }
      else
      {
        LoadTexture( textureInfo, element.mObserver );
      }
    }
  }
  mLoadQueue.Clear();
}

void TextureManager::ObserveTexture( TextureInfo& textureInfo,
                                     TextureUploadObserver* observer )
{
  DALI_LOG_INFO( gTextureManagerLogFilter, Debug::Concise, "TextureManager::ObserveTexture(): url:%s observer:%p\n",
                 textureInfo.url.GetUrl().c_str(), observer );

  if( observer )
  {
    textureInfo.observerList.PushBack( observer );
    observer->DestructionSignal().Connect( this, &TextureManager::ObserverDestroyed );
  }
}

void TextureManager::AsyncLoadComplete( AsyncLoadingInfoContainerType& loadingContainer, uint32_t id,
                                        Devel::PixelBuffer pixelBuffer )
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
        TextureInfo& textureInfo( mTextureInfoContainer[cacheIndex] );

        DALI_LOG_INFO( gTextureManagerLogFilter, Debug::Concise,
                       "  textureId:%d Url:%s CacheIndex:%d LoadState: %d\n",
                       textureInfo.textureId, textureInfo.url.GetUrl().c_str(), cacheIndex, textureInfo.loadState );

        if( textureInfo.loadState != LoadState::CANCELLED )
        {
          // textureInfo can be invalidated after this call (as the mTextureInfoContainer may be modified)
          PostLoad( textureInfo, pixelBuffer );
        }
        else
        {
          Remove( textureInfo.textureId, nullptr );
        }
      }
    }

    loadingContainer.pop_front();
  }
}

void TextureManager::PostLoad( TextureInfo& textureInfo, Devel::PixelBuffer& pixelBuffer )
{
  // Was the load successful?
  if( pixelBuffer && ( pixelBuffer.GetWidth() != 0 ) && ( pixelBuffer.GetHeight() != 0 ) )
  {
    // No atlas support for now
    textureInfo.useAtlas = NO_ATLAS;
    textureInfo.preMultiplied = pixelBuffer.IsAlphaPreMultiplied();

    if( textureInfo.storageType == StorageType::UPLOAD_TO_TEXTURE )
    {
      // If there is a mask texture ID associated with this texture, then apply the mask
      // if it's already loaded. If it hasn't, and the mask is still loading,
      // wait for the mask to finish loading.
      if( textureInfo.maskTextureId != INVALID_TEXTURE_ID )
      {
        if( textureInfo.loadState == LoadState::MASK_APPLYING )
        {
          textureInfo.loadState = LoadState::MASK_APPLIED;
          UploadTexture( pixelBuffer, textureInfo );
          NotifyObservers( textureInfo, true );
        }
        else
        {
          LoadState maskLoadState = GetTextureStateInternal( textureInfo.maskTextureId );
          textureInfo.pixelBuffer = pixelBuffer; // Store the pixel buffer temporarily
          if( maskLoadState == LoadState::LOADING )
          {
            textureInfo.loadState = LoadState::WAITING_FOR_MASK;
          }
          else if( maskLoadState == LoadState::LOAD_FINISHED )
          {
            // Send New Task to Thread
            ApplyMask( textureInfo, textureInfo.maskTextureId );
          }
        }
      }
      else
      {
        UploadTexture( pixelBuffer, textureInfo );
        NotifyObservers( textureInfo, true );
      }
    }
    else
    {
      textureInfo.pixelBuffer = pixelBuffer; // Store the pixel data
      textureInfo.loadState = LoadState::LOAD_FINISHED;

      if( textureInfo.storageType == StorageType::RETURN_PIXEL_BUFFER )
      {
        NotifyObservers( textureInfo, true );
      }
      else
      {
        // Check if there was another texture waiting for this load to complete
        // (e.g. if this was an image mask, and its load is on a different thread)
        CheckForWaitingTexture( textureInfo );
      }
    }
  }
  else
  {
    // @todo If the load was unsuccessful, upload the broken image.
    textureInfo.loadState = LoadState::LOAD_FAILED;
    CheckForWaitingTexture( textureInfo );
    NotifyObservers( textureInfo, false );
  }
}

void TextureManager::CheckForWaitingTexture( TextureInfo& maskTextureInfo )
{
  // Search the cache, checking if any texture has this texture id as a
  // maskTextureId:
  const unsigned int size = mTextureInfoContainer.size();

  for( unsigned int cacheIndex = 0; cacheIndex < size; ++cacheIndex )
  {
    if( mTextureInfoContainer[cacheIndex].maskTextureId == maskTextureInfo.textureId &&
        mTextureInfoContainer[cacheIndex].loadState == LoadState::WAITING_FOR_MASK )
    {
      TextureInfo& textureInfo( mTextureInfoContainer[cacheIndex] );

      if( maskTextureInfo.loadState == LoadState::LOAD_FINISHED )
      {
        // Send New Task to Thread
        ApplyMask( textureInfo, maskTextureInfo.textureId );
      }
      else
      {
        textureInfo.pixelBuffer.Reset();
        textureInfo.loadState = LoadState::LOAD_FAILED;
        NotifyObservers( textureInfo, false );
      }
    }
  }
}

void TextureManager::ApplyMask( TextureInfo& textureInfo, TextureId maskTextureId )
{
  int maskCacheIndex = GetCacheIndexFromId( maskTextureId );
  if( maskCacheIndex != INVALID_CACHE_INDEX )
  {
    Devel::PixelBuffer maskPixelBuffer = mTextureInfoContainer[maskCacheIndex].pixelBuffer;
    Devel::PixelBuffer pixelBuffer = textureInfo.pixelBuffer;
    textureInfo.pixelBuffer.Reset();

    DALI_LOG_INFO( gTextureManagerLogFilter, Debug::Concise, "TextureManager::ApplyMask(): url:%s sync:%s\n",
                   textureInfo.url.GetUrl().c_str(), textureInfo.loadSynchronously?"T":"F" );

    textureInfo.loadState = LoadState::MASK_APPLYING;
    auto& loadersContainer = textureInfo.url.IsLocalResource() ? mAsyncLocalLoaders : mAsyncRemoteLoaders;
    auto loadingHelperIt = loadersContainer.GetNext();
    auto premultiplyOnLoad = textureInfo.preMultiplyOnLoad ? DevelAsyncImageLoader::PreMultiplyOnLoad::ON : DevelAsyncImageLoader::PreMultiplyOnLoad::OFF;
    DALI_ASSERT_ALWAYS(loadingHelperIt != loadersContainer.End());
    loadingHelperIt->ApplyMask( textureInfo.textureId, pixelBuffer, maskPixelBuffer, textureInfo.scaleFactor, textureInfo.cropToMask, premultiplyOnLoad );
  }
}

void TextureManager::UploadTexture( Devel::PixelBuffer& pixelBuffer, TextureInfo& textureInfo )
{
  if( textureInfo.useAtlas != USE_ATLAS )
  {
    DALI_LOG_INFO( gTextureManagerLogFilter, Debug::General, "  TextureManager::UploadTexture() New Texture for textureId:%d\n", textureInfo.textureId );

    // Check if this pixelBuffer is premultiplied
    textureInfo.preMultiplied = pixelBuffer.IsAlphaPreMultiplied();

    auto& renderingAddOn = RenderingAddOn::Get();
    if( renderingAddOn.IsValid() )
    {
      renderingAddOn.CreateGeometry( textureInfo.textureId, pixelBuffer );
    }

    Texture texture = Texture::New( Dali::TextureType::TEXTURE_2D, pixelBuffer.GetPixelFormat(),
                                    pixelBuffer.GetWidth(), pixelBuffer.GetHeight() );

    PixelData pixelData = Devel::PixelBuffer::Convert( pixelBuffer );
    texture.Upload( pixelData );
    if ( ! textureInfo.textureSet )
    {
      textureInfo.textureSet = TextureSet::New();
    }
    textureInfo.textureSet.SetTexture( 0u, texture );
  }

  // Update the load state.
  // Note: This is regardless of success as we care about whether a
  // load attempt is in progress or not.  If unsuccessful, a broken
  // image is still loaded.
  textureInfo.loadState = LoadState::UPLOADED;
}

void TextureManager::NotifyObservers( TextureInfo& textureInfo, bool success )
{
  TextureId textureId = textureInfo.textureId;

  // If there is an observer: Notify the load is complete, whether successful or not,
  // and erase it from the list
  TextureInfo* info = &textureInfo;

  mQueueLoadFlag = true;

  while( info->observerList.Count() )
  {
    TextureUploadObserver* observer = info->observerList[0];

    // During UploadComplete() a Control ResourceReady() signal is emitted.
    // During that signal the app may add remove /add Textures (e.g. via
    // ImageViews).
    // It is possible for observers to be removed from the observer list,
    // and it is also possible for the mTextureInfoContainer to be modified,
    // invalidating the reference to the textureInfo struct.
    // Texture load requests for the same URL are deferred until the end of this
    // method.
    DALI_LOG_INFO( gTextureManagerLogFilter, Debug::Concise, "NotifyObservers() url:%s loadState:%s\n",
                   textureInfo.url.GetUrl().c_str(), GET_LOAD_STATE_STRING(textureInfo.loadState ) );

    // It is possible for the observer to be deleted.
    // Disconnect and remove the observer first.
    observer->DestructionSignal().Disconnect( this, &TextureManager::ObserverDestroyed );

    info->observerList.Erase( info->observerList.begin() );

    if( info->storageType == StorageType::RETURN_PIXEL_BUFFER )
    {
      observer->LoadComplete( success, info->pixelBuffer, info->url, info->preMultiplied );
    }
    else
    {
      observer->UploadComplete( success, info->textureId, info->textureSet, info->useAtlas, info->atlasRect,
                                info->preMultiplied );
    }

    // Get the textureInfo from the container again as it may have been invalidated.
    int textureInfoIndex = GetCacheIndexFromId( textureId );
    if( textureInfoIndex == INVALID_CACHE_INDEX)
    {
      break; // texture has been removed - can stop.
    }
    info = &mTextureInfoContainer[ textureInfoIndex ];
  }

  mQueueLoadFlag = false;
  ProcessQueuedTextures();

  if( info->storageType == StorageType::RETURN_PIXEL_BUFFER && info->observerList.Count() == 0 )
  {
    Remove( info->textureId, nullptr );
  }
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

  return INVALID_CACHE_INDEX;
}

TextureManager::TextureHash TextureManager::GenerateHash(
  const std::string&             url,
  const ImageDimensions          size,
  const FittingMode::Type        fittingMode,
  const Dali::SamplingMode::Type samplingMode,
  const UseAtlas                 useAtlas,
  TextureId                      maskTextureId,
  StorageType                    storageType,
  bool                           isAnimationImage,
  uint32_t                       frameIndex )
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
    // FittingMode=2bits, SamplingMode=3bits, useAtlas=1bit, storageType=2bits
    *hashTargetPtr   = ( fittingMode << 6u ) | ( samplingMode << 3 ) | ( useAtlas << 2 ) | static_cast<unsigned int>(storageType);
  }
  else
  {
    // We are not including sizing information, but we still need an extra byte for atlasing.
    hashTarget.resize( urlLength + 1u );

    // Add the atlasing to the hash input.
    switch( useAtlas )
    {
      case UseAtlas::NO_ATLAS:
      {
        hashTarget[ urlLength ] = 'f';
        break;
      }
      case UseAtlas::USE_ATLAS:
      {
        hashTarget[ urlLength ] = 't';
        break;
      }
    }
  }

  if( isAnimationImage )
  {
    auto textureIdIndex = hashTarget.length();
    hashTarget.resize( hashTarget.length() + sizeof( uint32_t ) );
    char* hashTargetPtr = &( hashTarget[ textureIdIndex ] );

    for( size_t byteIter = 0; byteIter < sizeof( uint32_t ); ++byteIter )
    {
      *hashTargetPtr++ = frameIndex & 0xff;
      frameIndex >>= 8u;
    }
  }

  if( maskTextureId != INVALID_TEXTURE_ID )
  {
    auto textureIdIndex = hashTarget.length();
    hashTarget.resize( hashTarget.length() + sizeof( TextureId ) );
    unsigned char* hashTargetPtr = reinterpret_cast<unsigned char*>(&( hashTarget[ textureIdIndex ] ));

    // Append the texture id to the end of the URL byte by byte:
    // (to avoid SIGBUS / alignment issues)
    for( size_t byteIter = 0; byteIter < sizeof( TextureId ); ++byteIter )
    {
      *hashTargetPtr++ = maskTextureId & 0xff;
      maskTextureId >>= 8u;
    }
  }

  return Dali::CalculateHash( hashTarget );
}

int TextureManager::FindCachedTexture(
  const TextureManager::TextureHash hash,
  const std::string&                url,
  const ImageDimensions             size,
  const FittingMode::Type           fittingMode,
  const Dali::SamplingMode::Type    samplingMode,
  const bool                        useAtlas,
  TextureId                         maskTextureId,
  TextureManager::MultiplyOnLoad    preMultiplyOnLoad,
  StorageType                       storageType,
  bool                              isAnimatedImage,
  uint32_t                          frameIndex )
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
          ( maskTextureId == textureInfo.maskTextureId ) &&
          ( size == textureInfo.desiredSize ) &&
          ( ( size.GetWidth() == 0 && size.GetHeight() == 0 ) ||
            ( fittingMode == textureInfo.fittingMode &&
              samplingMode == textureInfo.samplingMode ) ) &&
          ( storageType == textureInfo.storageType ) &&
          ( isAnimatedImage == ( ( textureInfo.animatedImageLoading ) ? true : false ) ) &&
          ( frameIndex == textureInfo.frameIndex ) )
      {
        // 1. If preMultiplyOnLoad is MULTIPLY_ON_LOAD, then textureInfo.preMultiplyOnLoad should be true. The premultiplication result can be different.
        // 2. If preMultiplyOnLoad is LOAD_WITHOUT_MULTIPLY, then textureInfo.preMultiplied should be false.
        if( ( preMultiplyOnLoad == TextureManager::MultiplyOnLoad::MULTIPLY_ON_LOAD && textureInfo.preMultiplyOnLoad )
            || ( preMultiplyOnLoad == TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY && !textureInfo.preMultiplied ) )
        {
          // The found Texture is a match.
          cacheIndex = i;
          break;
        }
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
    for( TextureInfo::ObserverListType::Iterator j = textureInfo.observerList.Begin();
         j != textureInfo.observerList.End(); )
    {
      if( *j == observer )
      {
        j = textureInfo.observerList.Erase( j );
      }
      else
      {
        ++j;
      }
    }
  }

  // Remove element from the LoadQueue
  for( auto&& element : mLoadQueue )
  {
    if( element.mObserver == observer )
    {
      element.mObserver = nullptr;
    }
  }
}


TextureManager::AsyncLoadingHelper::AsyncLoadingHelper(TextureManager& textureManager)
: AsyncLoadingHelper(Toolkit::AsyncImageLoader::New(), textureManager,
                     AsyncLoadingInfoContainerType())
{
}

void TextureManager::AsyncLoadingHelper::LoadAnimatedImage( TextureId                   textureId,
                                                            Dali::AnimatedImageLoading  animatedImageLoading,
                                                            uint32_t                    frameIndex )
{
  mLoadingInfoContainer.push_back( AsyncLoadingInfo( textureId ) );
  auto id = DevelAsyncImageLoader::LoadAnimatedImage( mLoader, animatedImageLoading, frameIndex );
  mLoadingInfoContainer.back().loadId = id;
}

void TextureManager::AsyncLoadingHelper::Load( TextureId                                textureId,
                                               const VisualUrl&                         url,
                                               ImageDimensions                          desiredSize,
                                               FittingMode::Type                        fittingMode,
                                               SamplingMode::Type                       samplingMode,
                                               bool                                     orientationCorrection,
                                               DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad )
{
  mLoadingInfoContainer.push_back( AsyncLoadingInfo( textureId ) );
  auto id = DevelAsyncImageLoader::Load( mLoader, url.GetUrl(), desiredSize, fittingMode, samplingMode, orientationCorrection, preMultiplyOnLoad );
  mLoadingInfoContainer.back().loadId = id;
}

void TextureManager::AsyncLoadingHelper::ApplyMask( TextureId                                textureId,
                                                    Devel::PixelBuffer                       pixelBuffer,
                                                    Devel::PixelBuffer                       maskPixelBuffer,
                                                    float                                    contentScale,
                                                    bool                                     cropToMask,
                                                    DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad )
{
  mLoadingInfoContainer.push_back( AsyncLoadingInfo( textureId ) );
  auto id = DevelAsyncImageLoader::ApplyMask( mLoader, pixelBuffer, maskPixelBuffer, contentScale, cropToMask, preMultiplyOnLoad );
  mLoadingInfoContainer.back().loadId = id;
}

TextureManager::AsyncLoadingHelper::AsyncLoadingHelper(AsyncLoadingHelper&& rhs)
: AsyncLoadingHelper(rhs.mLoader, rhs.mTextureManager, std::move(rhs.mLoadingInfoContainer))
{
}

TextureManager::AsyncLoadingHelper::AsyncLoadingHelper(
    Toolkit::AsyncImageLoader loader,
    TextureManager& textureManager,
    AsyncLoadingInfoContainerType&& loadingInfoContainer)
: mLoader(loader),
  mTextureManager(textureManager),
  mLoadingInfoContainer(std::move(loadingInfoContainer))
{
  DevelAsyncImageLoader::PixelBufferLoadedSignal(mLoader).Connect(
      this, &AsyncLoadingHelper::AsyncLoadComplete);
}

void TextureManager::AsyncLoadingHelper::AsyncLoadComplete(uint32_t           id,
                                                           Devel::PixelBuffer pixelBuffer )
{
  mTextureManager.AsyncLoadComplete( mLoadingInfoContainer, id, pixelBuffer );
}

void TextureManager::SetBrokenImageUrl(const std::string& brokenImageUrl)
{
  mBrokenImageUrl = brokenImageUrl;
}

Geometry TextureManager::GetRenderGeometry(TextureId textureId, uint32_t& frontElements, uint32_t& backElements )
{
  return RenderingAddOn::Get().IsValid() ?
         RenderingAddOn::Get().GetGeometry( textureId, frontElements, backElements) :
         Geometry();
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

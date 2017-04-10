#ifndef DALI_TOOLKIT_TEXTURE_MANAGER_H
#define DALI_TOOLKIT_TEXTURE_MANAGER_H

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
 */

// EXTERNAL INCLUDES
#include <string>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/rendering/texture-set.h>
#include <dali/devel-api/common/owner-container.h>
#include <deque>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/image-loader/image-atlas.h>
#include <dali-toolkit/public-api/image-loader/async-image-loader.h>
#include <dali-toolkit/internal/visuals/texture-upload-observer.h>
#include <dali-toolkit/internal/visuals/visual-url.h>


namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * The TextureManager provides a common Image loading API for Visuals.
 *
 * The TextureManager is responsible for providing sync, async, atlased and non-atlased loads.
 * Texture caching is provided and performed when possible.
 * Broken Images are automatically provided on load failure.
 */
class TextureManager : public ConnectionTracker
{
public:

  typedef int32_t TextureId;       ///< The TextureId type. This is used as a handle to refer to a particular Texture.
  static const int INVALID_TEXTURE_ID = -1; ///< Used to represent a null TextureId or error

  enum UseAtlas
  {
    NO_ATLAS,
    USE_ATLAS
  };

  enum LoadType
  {
    LOAD_ASYNCHRONOUSLY,
    LOAD_SYNCHRONOUSLY
  };

  /**
   * @brief The LoadState Enumeration represents the current state of a particular Textures life-cycle.
   */
  enum LoadState
  {
    NOT_STARTED,     ///< Default
    LOADING,         ///< Loading has been started, but not finished.
    UPLOADED,        ///< Loaded (and ready).
    CANCELLED,       ///< Removed before loading completed
  };

public:

  /**
   * Constructor.
   */
  TextureManager();

  /**
   * Destructor.
   */
  ~TextureManager();


// TextureManager Main API:

  /**
   * @brief Requests an image load of the given URL.
   *
   * The parameters are used to specify how the image is loaded.
   * The observer has the UploadComplete method called when the load is ready.
   *
   * When the client has finished with the Texture, Remove() should be called.
   *
   * @param[in] url               The URL of the image to load
   * @param[in] desiredSize       The size the image is likely to appear at. This can be set to 0,0 for automatic
   * @param[in] fittingMode       The FittingMode to use
   * @param[in] samplingMode      The SamplingMode to use
   * @param[in] useAtlasing       Set to USE_ATLAS to attempt atlasing. If atlasing fails, the image will still be loaded, and marked successful,
   *                              but "useAtlasing" will be set to false in the "UploadCompleted" callback from the TextureManagerUploadObserver.
   * @param[in] observer          The client object should inherit from this and provide the "UploadCompleted" virtual.
   *                              This is called when an image load completes (or fails).
   * @return                      A TextureId to use as a handle to reference this Texture
   */
  TextureId RequestLoad( const VisualUrl&         url,
                         const ImageDimensions    desiredSize,
                         FittingMode::Type        fittingMode,
                         Dali::SamplingMode::Type samplingMode,
                         const UseAtlas           useAtlasing,
                         TextureUploadObserver*   observer );

  /**
   * @brief Remove a Texture from the TextureManager.
   *
   * Textures are cached and therefore only the removal of the last
   * occurrence of a Texture will cause its removal internally.
   *
   * @param[in] textureId The ID of the Texture to remove.
   */
  void Remove( const TextureManager::TextureId textureId );

  /**
   * @brief Get the current state of a texture
   * @param[in] textureId The texture id to query
   * @return The loading state if the texture is valid, or NOT_STARTED if the textureId
   * is not valid.
   */
  LoadState GetTextureState( TextureId textureId );

  /**
   * @brief Get the associated texture set if the texture id is valid
   * @param[in] textureId The texture Id to look up
   * @return the associated texture set, or an empty handle if textureId is not valid
   */
  TextureSet GetTextureSet( TextureId textureId );

private:


  typedef size_t TextureHash; ///< The type used to store the hash used for Texture caching.

  /**
   * @brief This struct is used to manage the life-cycle of Texture loading and caching.
   * TODO-TX: pimpl this
   */
  struct TextureInfo
  {
    TextureInfo( TextureId textureId,
                 const VisualUrl& url,
                 ImageDimensions desiredSize,
                 FittingMode::Type fittingMode,
                 Dali::SamplingMode::Type samplingMode,
                 bool loadSynchronously,
                 UseAtlas useAtlas,
                 TextureManager::TextureHash hash )
    : url( url ),
      desiredSize( desiredSize ),
      useSize( desiredSize ),
      atlasRect( 0.0f, 0.0f, 1.0f, 1.0f ), // Full atlas rectangle
      textureId( textureId ),
      hash( hash ),
      referenceCount( 1u ),
      loadState( NOT_STARTED ),
      fittingMode( fittingMode ),
      samplingMode( samplingMode ),
      loadSynchronously( loadSynchronously ),
      useAtlas( useAtlas ),
      loadingSucceeded( false )
    {
    }

    /**
     * Container type used to store all observer clients of this Texture
     */
    typedef Dali::Vector< TextureUploadObserver* > ObserverListType;

    ObserverListType observerList; ///< Container used to store all observer clients of this Texture
    Toolkit::ImageAtlas atlas;     ///< The atlas this Texture lays within (if any)
    PixelData pixelData;           ///< The PixelData holding the image data (this is used if atlasing is deferred)
    TextureSet textureSet;         ///< The TextureSet holding the Texture
    VisualUrl url;                 ///< The URL of the image
    ImageDimensions desiredSize;   ///< The size requested
    ImageDimensions useSize;       ///< The size used
    Vector4 atlasRect;             ///< The atlas rect used if atlased
    TextureId textureId;           ///< The TextureId associated with this Texture
    TextureManager::TextureHash hash; ///< The hash used to cache this Texture
    int16_t referenceCount;        ///< The reference count of clients using this Texture
    LoadState loadState:3;         ///< The load state showing the load progress of the Texture
    FittingMode::Type fittingMode:2; ///< The requested FittingMode
    Dali::SamplingMode::Type samplingMode:3; ///< The requested SamplingMode
    bool loadSynchronously;        ///< True if synchronous loading was requested
    UseAtlas useAtlas; ///< USE_ATLAS if an atlas was requested. This is updated to false if atlas is not used
    bool loadingSucceeded;         ///< True if the image was loaded successfully
  };

  // Structs:

  /**
   * Struct to hold information about a requested Async load.
   * This is used to look up a TextureManager::TextureId from the returned AsyncLoad Id.
   */
  struct AsyncLoadingInfo
  {
    AsyncLoadingInfo( TextureId textureId )
    : textureId( textureId ),
      loadId( 0 )
    {
    }

    TextureId           textureId;   ///< The external Texture Id assigned to this load
    unsigned short      loadId;      ///< The load Id used by the async loader to reference this load
  };

  /**
   * @brief This struct is used within a container to manage atlas creation and destruction.
   */
  struct AtlasInfo
  {
    AtlasInfo( Toolkit::ImageAtlas atlas, TextureSet textureSet )
    : atlas( atlas ),
      textureSet( textureSet )
    {
    }

    Toolkit::ImageAtlas                 atlas;                          ///< The ImageAtlas object
    TextureSet                          textureSet;                     ///< The TextureSet is kept in the struct to allow fast lookup of TextureSet to Atlas
  };

  // Private typedefs:

  typedef std::deque<AsyncLoadingInfo>  AsyncLoadingInfoContainerType;  ///< The container type used to manage Asynchronous loads in progress
  typedef std::vector<AtlasInfo>        AtlasInfoContainerType;         ///< The container type used to manage Atlas creation and destruction
  typedef std::vector<TextureInfo>      TextureInfoContainerType;       ///< The container type used to manage the life-cycle and caching of Textures

  /**
   * @brief Used internally to initiate a load.
   * @param[in] textureInfo The TextureInfo struct associated with the Texture
   * @return                True if the load was initiated
   */
  bool LoadTexture( TextureInfo& textureInfo );

  /**
   * Add the observer to the observer list
   * @param[in] textureInfo The TextureInfo struct associated with the texture
   * observer The observer wishing to observe the texture upload
   */
  void ObserveTexture( TextureInfo & textureInfo, TextureUploadObserver* observer );

  /**
   * @brief This signal handler is called when the async local loader finishes loading.
   * @param[in] id        This is the async image loaders Id
   * @param[in] pixelData The loaded image data
   */
  void AsyncLocalLoadComplete( uint32_t id, PixelData pixelData );

  /**
   * @brief This signal handler is called when the async local loader finishes loading.
   * @param[in] id        This is the async image loaders Id
   * @param[in] pixelData The loaded image data
   */
  void AsyncRemoteLoadComplete( uint32_t id, PixelData pixelData );

  /**
   * Common method to handle loading completion
   * @param[in] container The Async loading container
   * @param[in] id        This is the async image loaders Id
   * @param[in] pixelData The loaded image data
   */
  void AsyncLoadComplete( AsyncLoadingInfoContainerType& container, uint32_t id, PixelData pixelData );

  /**
   * @brief Performs Post-Load steps including atlasing.
   * @param[in]           textureInfo The struct associated with this Texture
   * @param[in]           pixelData   The image pixelData
   * @return                          True if successful
   */
  bool PostLoad( TextureManager::TextureInfo& textureInfo, PixelData pixelData );

  /**
   * @brief Generates a new, unique TextureId
   * @return A unique TextureId
   */
  TextureManager::TextureId GenerateUniqueTextureId();

  /**
   * @brief Used to lookup an index into the TextureInfoContainer from a TextureId
   * @param[in] textureId The TextureId to look up
   * @return              The cache index
   */
  int GetCacheIndexFromId( TextureId textureId );


  /**
   * @brief Generates a hash for caching based on the input parameters.
   * @param[in] url          The URL of the image to load
   * @param[in] size         The image size
   * @param[in] fittingMode  The FittingMode to use
   * @param[in] samplingMode The SamplingMode to use
   * @param[in] useAtlas     True if atlased
   * @return                 A hash of the provided data for caching.
   */
  TextureHash GenerateHash( const std::string& url, const ImageDimensions size,
                            const FittingMode::Type fittingMode,
                            const Dali::SamplingMode::Type samplingMode, const UseAtlas useAtlas );

  /**
   * @brief Looks up a cached texture by its hash.
   * If found, the given parameters are used to check there is no hash-collision.
   * @param[in] hash The hash to look up
   * @param[in] url          The URL of the image to load
   * @param[in] size         The image size
   * @param[in] fittingMode  The FittingMode to use
   * @param[in] samplingMode The SamplingMode to use
   * @param[in] useAtlas     True if atlased
   * @return                 A TextureId of a cached Texture if found. Or INVALID_TEXTURE_ID if not found.
   */
  TextureManager::TextureId FindCachedTexture( const TextureManager::TextureHash hash, const std::string& url, const ImageDimensions size,
      const FittingMode::Type fittingMode, const Dali::SamplingMode::Type samplingMode, const bool useAtlas );


private:

  /**
   * Undefined copy constructor.
   */
  TextureManager( const TextureManager& );

  /**
   * Undefined assignment operator.
   */
  TextureManager& operator=( const TextureManager& rhs );

  /**
   * This is called by the TextureManagerUploadObserver when an observer is destroyed.
   * We use the callback to know when to remove an observer from our notify list.
   * @param[in] observer The observer that generated the callback
   */
  void ObserverDestroyed( TextureUploadObserver* observer );

private:  // Member Variables:

  AsyncLoadingInfoContainerType         mAsyncLocalLoadingInfoContainer;     ///< Used to manage Asynchronous loads in progress
  AsyncLoadingInfoContainerType         mAsyncRemoteLoadingInfoContainer;     ///< Used to manage Asynchronous loads in progress
  AtlasInfoContainerType                mAtlasContainer;                ///< Used to manage Atlas creation and destruction
  TextureInfoContainerType              mTextureInfoContainer;          ///< Used to manage the life-cycle and caching of Textures
  Toolkit::AsyncImageLoader             mAsyncLocalLoader;              ///< The Asynchronous image loader used to provide all local async loads
  Toolkit::AsyncImageLoader             mAsyncRemoteLoader;             ///< The Asynchronous image loader used to provide all remote async loads
  TextureId                             mCurrentTextureId;              ///< The current value used for the unique Texture Id generation

};


} // name Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXTURE_MANAGER_H

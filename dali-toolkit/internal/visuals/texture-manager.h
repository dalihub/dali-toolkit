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

class MaskTextureObserver;

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

  /**
   * Whether the texture should be atlased or uploaded into it's own GPU texture
   */
  enum UseAtlas
  {
    NO_ATLAS,
    USE_ATLAS
  };

  /**
   * Whether the texture should be stored in CPU memory, or uploaded to a GPU texture
   */
  enum StorageType
  {
    CPU,
    GPU_UPLOAD
  };

  /**
   * Whether the texture should be loaded synchronously or asynchronously.
   */
  enum LoadType
  {
    LOAD_ASYNCHRONOUSLY,
    LOAD_SYNCHRONOUSLY
  };

  /**
   * @brief The LoadState Enumeration represents the current state of a particular Texture's life-cycle.
   */
  enum LoadState
  {
    NOT_STARTED,     ///< Default
    LOADING,         ///< Loading has been started, but not finished.
    LOAD_FINISHED,   ///< Loading has finished. (for CPU storage only)
    WAITING_FOR_MASK,///< Loading has finished, but waiting for mask image
    UPLOADED,        ///< Uploaded and ready. (For GPU upload only)
    CANCELLED,       ///< Removed before loading completed
    LOAD_FAILED      ///< Async loading failed, e.g. connection problem
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
   * @brief Requests an image load of the given URL, when the texture has
   * have loaded, it will perform a CPU blend with the image mask, and upload
   * the blend texture.
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
                         TextureId                maskTextureId,
                         const ImageDimensions    desiredSize,
                         FittingMode::Type        fittingMode,
                         Dali::SamplingMode::Type samplingMode,
                         const UseAtlas           useAtlasing,
                         TextureUploadObserver*   observer );

  /**
   * Requests a masking image to be loaded. This mask is not uploaded to GL,
   * instead, it is stored in CPU memory, and can be used for CPU blending.
   */
  TextureId RequestMaskLoad( const VisualUrl& maskUrl );

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

  /**
   * @brief Requests an image load of the given URL, when the texture has
   * have loaded, if there is a valid maskTextureId, it will perform a
   * CPU blend with the mask, and upload the blend texture.
   *
   * The parameters are used to specify how the image is loaded.
   * The observer has the UploadComplete method called when the load is ready.
   *
   * When the client has finished with the Texture, Remove() should be called.
   *
   * @param[in] url               The URL of the image to load
   * @param[in] maskTextureId     The texture id of an image to use as a mask. If no mask is required, then set to INVALID_TEXTURE_ID
   * @param[in] desiredSize       The size the image is likely to appear at. This can be set to 0,0 for automatic
   * @param[in] fittingMode       The FittingMode to use
   * @param[in] samplingMode      The SamplingMode to use
   * @param[in] useAtlasing       Set to USE_ATLAS to attempt atlasing. If atlasing fails, the image will still be loaded, and marked successful,
   *                              but "useAtlasing" will be set to false in the "UploadCompleted" callback from the TextureManagerUploadObserver.
   * @param[in] storageType,      Whether the pixel data is stored in the cache or uploaded to the GPU
   * @param[in] observer          The client object should inherit from this and provide the "UploadCompleted" virtual.
   *                              This is called when an image load completes (or fails).
   * @return                      A TextureId to use as a handle to reference this Texture
   */
  TextureId RequestInternalLoad(
    const VisualUrl&         url,
    TextureId                maskTextureId,
    const ImageDimensions    desiredSize,
    FittingMode::Type        fittingMode,
    Dali::SamplingMode::Type samplingMode,
    UseAtlas                 useAtlas,
    StorageType              storageType,
    TextureUploadObserver*   observer );


  typedef size_t TextureHash; ///< The type used to store the hash used for Texture caching.

  /**
   * @brief This struct is used to manage the life-cycle of Texture loading and caching.
   */
  struct TextureInfo
  {
    TextureInfo( TextureId textureId,
                 TextureId maskTextureId,
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
      maskTextureId( maskTextureId ),
      hash( hash ),
      referenceCount( 1u ),
      loadState( NOT_STARTED ),
      fittingMode( fittingMode ),
      samplingMode( samplingMode ),
      storageType( GPU_UPLOAD ),
      loadSynchronously( loadSynchronously ),
      useAtlas( useAtlas )
    {
    }

    /**
     * Container type used to store all observer clients of this Texture
     */
    typedef Dali::Vector< TextureUploadObserver* > ObserverListType;

    ObserverListType observerList; ///< Container used to store all observer clients of this Texture
    Toolkit::ImageAtlas atlas;     ///< The atlas this Texture lays within (if any)
    PixelData pixelData;           ///< The PixelData holding the image data (this is used if atlasing is deferred or CPU storage is required)
    TextureSet textureSet;         ///< The TextureSet holding the Texture
    VisualUrl url;                 ///< The URL of the image
    ImageDimensions desiredSize;   ///< The size requested
    ImageDimensions useSize;       ///< The size used
    Vector4 atlasRect;             ///< The atlas rect used if atlased
    TextureId textureId;           ///< The TextureId associated with this Texture
    TextureId maskTextureId;       ///< The mask TextureId to be applied on load
    TextureManager::TextureHash hash; ///< The hash used to cache this Texture
    int16_t referenceCount;        ///< The reference count of clients using this Texture
    LoadState loadState:3;         ///< The load state showing the load progress of the Texture
    FittingMode::Type fittingMode:2; ///< The requested FittingMode
    Dali::SamplingMode::Type samplingMode:3; ///< The requested SamplingMode
    StorageType storageType:1;     ///< CPU storage / GPU upload;
    bool loadSynchronously:1;      ///< True if synchronous loading was requested
    UseAtlas useAtlas:1;           ///< USE_ATLAS if an atlas was requested. This is updated to false if atlas is not used
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
   * @param[in] textureInfo The struct associated with this Texture
   * @param[in] pixelData   The image pixelData
   * @return    True if successful
   */
  void PostLoad( TextureManager::TextureInfo& textureInfo, PixelData pixelData );

  /**
   * Check if there is a texture waiting to be masked. If there
   * is then apply this mask and upload it.
   * @param[in] maskTextureInfo The texture info of the mask that has just loaded.
   */
  void CheckForWaitingTexture( TextureInfo& maskTextureInfo );

  /**
   * Apply the mask texture to the image texture.
   * @param[in] pixelData The image pixelData to apply the mask to
   * @param[in] maskTextureId The texture id of the mask.
   */
  void ApplyMask( PixelData pixelData, TextureId maskTextureId );

  /**
   * Upload the texture specified in pixelData to the appropriate location
   * @param[in] pixelData The image data to upload
   * @param[in] textureInfo The texture info containing the location to
   * store the data to.
   */
  void UploadTexture( PixelData pixelData, TextureInfo& textureInfo );

  /**
   * Mark the texture as complete, and inform observers
   * @param[in] textureInfo The struct associated with this Texture
   */
  void UploadComplete( TextureInfo& textureInfo );

  /**
   * Notify the current observers that the texture upload is complete,
   * then remove the observers from the list.
   * @param[in] textureInfo The struct associated with this Texture
   * @param[in] success If the pixel data was retrieved successfully and uploaded to GPU
   */
  void NotifyObservers( TextureInfo& textureInfo, bool success );

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
   * Only applies size, fitting mode andsampling mode if the size is specified.
   * Only applies maskTextureId if it isn't INVALID_TEXTURE_ID
   * Always applies useAtlas.
   * @param[in] url          The URL of the image to load
   * @param[in] size         The image size
   * @param[in] fittingMode  The FittingMode to use
   * @param[in] samplingMode The SamplingMode to use
   * @param[in] useAtlas     True if atlased
   * @param[in] maskTextureId The masking texture id (or INVALID_TEXTURE_ID)
   * @return                 A hash of the provided data for caching.
   */
  TextureHash GenerateHash( const std::string& url, const ImageDimensions size,
                            const FittingMode::Type fittingMode,
                            const Dali::SamplingMode::Type samplingMode, const UseAtlas useAtlas,
                            TextureId maskTextureId );

  /**
   * @brief Looks up a cached texture by its hash.
   * If found, the given parameters are used to check there is no hash-collision.
   * @param[in] hash          The hash to look up
   * @param[in] url           The URL of the image to load
   * @param[in] size          The image size
   * @param[in] fittingMode   The FittingMode to use
   * @param[in] samplingMode  The SamplingMode to use
   * @param[in] useAtlas      True if atlased
   * @param[in] maskTextureId Optional texture ID to use to mask this image
   * @return A TextureId of a cached Texture if found. Or INVALID_TEXTURE_ID if not found.
   */
  TextureManager::TextureId FindCachedTexture(
    const TextureManager::TextureHash hash,
    const std::string& url,
    const ImageDimensions size,
    const FittingMode::Type fittingMode,
    const Dali::SamplingMode::Type samplingMode,
    const bool useAtlas,
    TextureId maskTextureId );


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

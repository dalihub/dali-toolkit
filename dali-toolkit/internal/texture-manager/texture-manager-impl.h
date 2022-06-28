#ifndef DALI_TOOLKIT_TEXTURE_MANAGER_IMPL_H
#define DALI_TOOLKIT_TEXTURE_MANAGER_IMPL_H

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
 */

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/animated-image-loading.h>
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali/public-api/adaptor-framework/encoded-image-buffer.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/rendering/geometry.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/image-loader/image-atlas.h>
#include <dali-toolkit/internal/helpers/round-robin-container-view.h>
#include <dali-toolkit/internal/texture-manager/texture-cache-manager.h>
#include <dali-toolkit/internal/texture-manager/texture-manager-type.h>
#include <dali-toolkit/internal/texture-manager/texture-upload-observer.h>
#include <dali-toolkit/internal/visuals/visual-url.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class ImageAtlasManager;
typedef IntrusivePtr<ImageAtlasManager> ImageAtlasManagerPtr;
class TextureAsyncLoadingHelper;

/**
 * The TextureManager provides a common Image loading API for Visuals.
 *
 * The TextureManager is responsible for providing sync, async, atlased and non-atlased
 * CPU time alpha masking, animated image loads.
 *
 * Texture caching is provided and performed by TextureCacheManager.
 * TextureUploadObserver.LoadComplete called when async load completed.
 */
class TextureManager : public ConnectionTracker
{
public:
  // Copy enum and types and const values that TextureManager will use.
  using TextureId         = TextureManagerType::TextureId;
  using TextureCacheIndex = TextureManagerType::TextureCacheIndex;
  using TextureHash       = TextureManagerType::TextureHash;

  static constexpr TextureId         INVALID_TEXTURE_ID  = TextureManagerType::INVALID_TEXTURE_ID;
  static constexpr TextureCacheIndex INVALID_CACHE_INDEX = TextureManagerType::INVALID_CACHE_INDEX;

  using UseAtlas       = TextureManagerType::UseAtlas;
  using StorageType    = TextureManagerType::StorageType;
  using LoadType       = TextureManagerType::LoadType;
  using LoadState      = TextureManagerType::LoadState;
  using ReloadPolicy   = TextureManagerType::ReloadPolicy;
  using MultiplyOnLoad = TextureManagerType::MultiplyOnLoad;
  using TextureInfo    = TextureManagerType::TextureInfo;

public:
  struct MaskingData
  {
    MaskingData();
    ~MaskingData() = default;

    VisualUrl                 mAlphaMaskUrl;
    TextureManager::TextureId mAlphaMaskId;
    float                     mContentScaleFactor;
    bool                      mCropToMask;
    bool                      mPreappliedMasking;
    bool                      mMaskImageLoadingFailed;
  };
  using MaskingDataPointer = std::unique_ptr<MaskingData>;

  /**
   * Class to provide lifecycle event on destruction of texture manager.
   */
  struct LifecycleObserver
  {
    /**
     * Called shortly before the texture manager is destroyed.
     */
    virtual void TextureManagerDestroyed() = 0;
  };

  /**
   * Constructor.
   */
  TextureManager();

  /**
   * Destructor.
   */
  ~TextureManager() override;

  // TextureManager Main API:

  /**
   * @brief Requests an frame of animated image load.
   *
   * The parameters are used to specify how the animated image is loaded.
   * The observer has the LoadComplete method called when the load is ready.
   *
   * @param[in]  url                   The URL of the image to load
   * @param[in]  animatedImageLoading  The AnimatedImageLoading that contain the animated image information
   * @param[in]  frameIndex            The frame index to load.
   * @param[out] textureId             The textureId of the frame
   * @param[in, out] maskInfo          Mask info structure
   * @param[in]  samplingMode          The SamplingMode to use
   * @param[in]  wrapModeU             Horizontal Wrap mode
   * @param[in]  wrapModeV             Vertical Wrap mode
   * @param[in]  synchronousLoading    true if the frame should be loaded synchronously
   * @param[in]  textureObserver       The client object should inherit from this and provide the "LoadCompleted" virtual.
   *                                   This is called when an image load completes (or fails).
   * @param[in,out] preMultiplyOnLoad  True if the image color should be multiplied by it's alpha. Set to false if the
   *                                   image has no alpha channel
   *
   * @return                           The texture set containing the frame of animated image, or empty if still loading.
   */
  TextureSet LoadAnimatedImageTexture(const VisualUrl&                url,
                                      Dali::AnimatedImageLoading      animatedImageLoading,
                                      const uint32_t&                 frameIndex,
                                      TextureManager::TextureId&      textureId,
                                      MaskingDataPointer&             maskInfo,
                                      const Dali::SamplingMode::Type& samplingMode,
                                      const Dali::WrapMode::Type&     wrapModeU,
                                      const Dali::WrapMode::Type&     wrapModeV,
                                      const bool&                     synchronousLoading,
                                      TextureUploadObserver*          textureObserver,
                                      TextureManager::MultiplyOnLoad& preMultiplyOnLoad);

  /**
   * @brief Requests an image load of the given URL to get PixelBuffer.
   *
   * The parameters are used to specify how the image is loaded.
   * The observer has the LoadComplete method called when the load is ready.
   *
   * @param[in] url                   The URL of the image to load
   * @param[in] desiredSize           The size the image is likely to appear at. This can be set to 0,0 for automatic
   * @param[in] fittingMode           The FittingMode to use
   * @param[in] samplingMode          The SamplingMode to use
   * @param[in] synchronousLoading    true if the URL should be loaded synchronously
   * @param[in] textureObserver       The client object should inherit from this and provide the "LoadCompleted" virtual.
   *                                  This is called when an image load completes (or fails).
   * @param[in] orientationCorrection Whether to rotate image to match embedded orientation data
   * @param[in,out] preMultiplyOnLoad True if the image color should be multiplied by it's alpha. Set to false if the
   *                                  image has no alpha channel
   *
   * @return                          The pixel buffer containing the image, or empty if still loading.
   */
  Devel::PixelBuffer LoadPixelBuffer(
    const VisualUrl&                url,
    const Dali::ImageDimensions&    desiredSize,
    const Dali::FittingMode::Type&  fittingMode,
    const Dali::SamplingMode::Type& samplingMode,
    const bool&                     synchronousLoading,
    TextureUploadObserver*          textureObserver,
    const bool&                     orientationCorrection,
    TextureManager::MultiplyOnLoad& preMultiplyOnLoad);

  /**
   * @brief Requests an image load of the given URL.
   *
   * The parameters are used to specify how the image is loaded.
   * The observer has the LoadComplete method called when the load is ready.
   *
   * When the client has finished with the Texture, Remove() should be called.
   *
   * @param[in] url                   The URL of the image to load
   * @param[in] desiredSize           The size the image is likely to appear at. This can be set to 0,0 for automatic
   * @param[in] fittingMode           The FittingMode to use
   * @param[in] samplingMode          The SamplingMode to use
   * @param[in, out] maskInfo         Mask info structure
   * @param[in] synchronousLoading    true if the URL should be loaded synchronously
   * @param[out] textureId,           The textureId of the URL
   * @param[out] textureRect          The rectangle within the texture atlas that this URL occupies,
   *                                  this is the rectangle in normalized coordinates.
   * @param[out] textureRectSize      The rectangle within the texture atlas that this URL occupies,
   *                                  this is the same rectangle in pixels.
   * @param[in,out] atlasingStatus    Set to USE_ATLAS to attempt atlasing. If atlasing fails, the image will still
   *                                  be loaded, and marked successful, but this will be set to false.
   *                                  If atlasing succeeds, this will be set to true.
   * @param[out] loadingStatus        The loading status of the texture
   * @param[in] wrapModeU             Horizontal Wrap mode
   * @param[in] wrapModeV             Vertical Wrap mode
   * @param[in] textureObserver       The client object should inherit from this and provide the "LoadCompleted" virtual.
   *                                  This is called when an image load completes (or fails).
   * @param[in] atlasObserver         This is used if the texture is atlased, and will be called instead of
   *                                  textureObserver.LoadCompleted
   * @param[in] imageAtlasManager     The atlas manager to use for atlasing textures
   * @param[in] orientationCorrection Whether to rotate image to match embedded orientation data
   * @param[in] reloadPolicy          Forces a reload of the texture even if already cached
   * @param[in,out] preMultiplyOnLoad True if the image color should be multiplied by it's alpha. Set to false if the
   *                                  image has no alpha channel
   *
   * @return                          The texture set containing the image, or empty if still loading.
   */
  TextureSet LoadTexture(
    const VisualUrl&                    url,
    const Dali::ImageDimensions&        desiredSize,
    const Dali::FittingMode::Type&      fittingMode,
    const Dali::SamplingMode::Type&     samplingMode,
    MaskingDataPointer&                 maskInfo,
    const bool&                         synchronousLoading,
    TextureManager::TextureId&          textureId,
    Dali::Vector4&                      textureRect,
    Dali::ImageDimensions&              textureRectSize,
    bool&                               atlasingStatus,
    bool&                               loadingStatus,
    const Dali::WrapMode::Type&         wrapModeU,
    const Dali::WrapMode::Type&         wrapModeV,
    TextureUploadObserver*              textureObserver,
    AtlasUploadObserver*                atlasObserver,
    ImageAtlasManagerPtr                imageAtlasManager,
    const bool&                         orientationCorrection,
    const TextureManager::ReloadPolicy& reloadPolicy,
    TextureManager::MultiplyOnLoad&     preMultiplyOnLoad);

  /**
   * @brief Remove a Texture from the TextureManager.
   *
   * Textures are cached and therefore only the removal of the last
   * occurrence of a Texture will cause its removal internally.
   *
   * @param[in] textureId The ID of the Texture to remove.
   * @param[in] textureObserver The texture observer.
   */
  void Remove(const TextureManager::TextureId& textureId, TextureUploadObserver* textureObserver);

  /**
   * Add an observer to the object.
   * @param[in] observer The observer to add.
   */
  void AddObserver(TextureManager::LifecycleObserver& observer);

  /**
   * Remove an observer from the object
   * @pre The observer has already been added.
   * @param[in] observer The observer to remove.
   */
  void RemoveObserver(TextureManager::LifecycleObserver& observer);

  /**
   * @brief Returns the geometry associated with texture.
   * @param[in] textureId Id of the texture
   * @param[out] frontElements number of front elements
   * @param[out] backElements number of back elements
   * @return Returns valid geometry object
   */
  Geometry GetRenderGeometry(const TextureManager::TextureId& textureId, std::uint32_t& frontElements, std::uint32_t& backElements);

public:
  // API list that need to access TextureCacheManager.

  /**
   * @copydoc TextureCacheManager::GetVisualUrl
   */
  inline VisualUrl GetVisualUrl(const TextureManager::TextureId& textureId)
  {
    return mTextureCacheManager.GetVisualUrl(textureId);
  }

  /**
   * @copydoc TextureCacheManager::GetTextureSet
   */
  inline TextureSet GetTextureSet(const TextureManager::TextureId& textureId)
  {
    return mTextureCacheManager.GetTextureSet(textureId);
  }

  /**
   * @copydoc TextureCacheManager::RemoveExternalTexture
   */
  inline TextureSet RemoveExternalTexture(const std::string& url)
  {
    return mTextureCacheManager.RemoveExternalTexture(url);
  }

  /**
   * @copydoc TextureCacheManager::RemoveEncodedImageBuffer
   */
  inline EncodedImageBuffer RemoveEncodedImageBuffer(const std::string& url)
  {
    return mTextureCacheManager.RemoveEncodedImageBuffer(url);
  }

  /**
   * @copydoc TextureCacheManager::UseExternalResource
   */
  inline void UseExternalResource(const VisualUrl& url)
  {
    mTextureCacheManager.UseExternalResource(url);
  }

  /**
   * @copydoc TextureCacheManager::GetEncodedImageBuffer
   */
  inline EncodedImageBuffer GetEncodedImageBuffer(const std::string& url)
  {
    return mTextureCacheManager.GetEncodedImageBuffer(url);
  }

  /**
   * @copydoc TextureCacheManager::AddExternalTexture
   */
  inline std::string AddExternalTexture(const TextureSet& texture)
  {
    return mTextureCacheManager.AddExternalTexture(texture);
  }

  /**
   * @copydoc TextureCacheManager::AddEncodedImageBuffer
   */
  inline std::string AddEncodedImageBuffer(const EncodedImageBuffer& encodedImageBuffer)
  {
    return mTextureCacheManager.AddEncodedImageBuffer(encodedImageBuffer);
  }

public: // Load Request API
  /**
   * @brief Requests an image load of the given URL.
   *
   * The parameters are used to specify how the image is loaded.
   * The observer has the LoadComplete method called when the load is ready.
   *
   * When the client has finished with the Texture, Remove() should be called.
   *
   * @param[in] url                   The URL of the image to load
   * @param[in] desiredSize           The size the image is likely to appear at. This can be set to 0,0 for automatic
   * @param[in] fittingMode           The FittingMode to use
   * @param[in] samplingMode          The SamplingMode to use
   * @param[in] useAtlasing           Set to USE_ATLAS to attempt atlasing. If atlasing fails, the image will still be loaded, and marked successful,
   *                                  but "useAtlasing" will be set to false in the "LoadCompleted" callback from the TextureManagerUploadObserver.
   * @param[in] observer              The client object should inherit from this and provide the "LoadCompleted" virtual.
   *                                  This is called when an image load completes (or fails).
   * @param[in] orientationCorrection Whether to rotate image to match embedded orientation data
   * @param[in] reloadPolicy          Forces a reload of the texture even if already cached
   * @param[in,out] preMultiplyOnLoad True if the image color should be multiplied by it's alpha. Set to false if the image has no alpha channel
   * @param[in] synchronousLoading    True if the frame should be loaded synchronously. If you skip this parameter,
   *                                  default is false.
   * @return                          A TextureId to use as a handle to reference this Texture
   */
  TextureId RequestLoad(
    const VisualUrl&                    url,
    const ImageDimensions&              desiredSize,
    const Dali::FittingMode::Type&      fittingMode,
    const Dali::SamplingMode::Type&     samplingMode,
    const TextureManager::UseAtlas&     useAtlasing,
    TextureUploadObserver*              observer,
    const bool&                         orientationCorrection,
    const TextureManager::ReloadPolicy& reloadPolicy,
    TextureManager::MultiplyOnLoad&     preMultiplyOnLoad,
    const bool&                         synchronousLoading = false);

  /**
   * @brief Requests an image load of the given URL, when the texture has
   * have loaded, it will perform a blend with the image mask, and upload
   * the blended texture.
   *
   * The parameters are used to specify how the image is loaded.
   * The observer has the LoadComplete method called when the load is ready.
   *
   * When the client has finished with the Texture, Remove() should be called.
   *
   * @param[in] url                   The URL of the image to load
   * @param[in] maskTextureId         The texture id of an image to mask this with
   *                                  (can be INVALID if no masking required)
   * @param[in] contentScale          The scale factor to apply to the image before masking
   * @param[in] desiredSize           The size the image is likely to appear at. This can be set to 0,0 for automatic
   * @param[in] fittingMode           The FittingMode to use
   * @param[in] samplingMode          The SamplingMode to use
   * @param[in] useAtlasing           Set to USE_ATLAS to attempt atlasing. If atlasing fails, the image will still
   *                                  be loaded, and marked successful,
   *                                  but "useAtlasing" will be set to false in the "LoadCompleted" callback from
   *                                  the TextureManagerUploadObserver.
   * @param[in] cropToMask            Only used with masking, this will crop the scaled image to the mask size.
   *                                  If false, then the mask will be scaled to fit the image before being applied.
   * @param[in] observer              The client object should inherit from this and provide the "LoadCompleted"
   *                                  virtual.
   *                                  This is called when an image load completes (or fails).
   * @param[in] orientationCorrection Whether to rotate image to match embedded orientation data
   * @param[in] reloadPolicy          Forces a reload of the texture even if already cached
   * @param[in,out] preMultiplyOnLoad True if the image color should be multiplied by it's alpha. Set to false if the
   *                                  image has no alpha channel
   * @param[in] synchronousLoading    True if the frame should be loaded synchronously. If you skip this parameter,
   *                                  default is false.
   * @return                          A TextureId to use as a handle to reference this Texture
   */
  TextureId RequestLoad(
    const VisualUrl&                    url,
    const TextureManager::TextureId&    maskTextureId,
    const float&                        contentScale,
    const ImageDimensions&              desiredSize,
    const Dali::FittingMode::Type&      fittingMode,
    const Dali::SamplingMode::Type&     samplingMode,
    const TextureManager::UseAtlas&     useAtlasing,
    const bool&                         cropToMask,
    TextureUploadObserver*              observer,
    const bool&                         orientationCorrection,
    const TextureManager::ReloadPolicy& reloadPolicy,
    TextureManager::MultiplyOnLoad&     preMultiplyOnLoad,
    const bool&                         synchronousLoading = false);

  /**
   * @brief Requests a masking image to be loaded. This mask is not uploaded to GL,
   * instead, it is stored in CPU memory, and can be used for CPU blending.
   * @param[in] maskUrl            The URL of the mask image to load
   * @param[in] storageType,       Whether the pixel data is stored in the cache or uploaded to the GPU
   * @param[in] synchronousLoading True if the frame should be loaded synchronously. If you skip this parameter,
   *                               default is false.
   * @return                       A TextureId to use as a handle to reference this mask Texture
   */
  TextureId RequestMaskLoad(
    const VisualUrl& maskUrl,
    StorageType      storageType,
    const bool&      synchronousLoading = false);

private:
  /**
   * @brief Requests an image load of the given URL, when the texture has
   * have loaded, if there is a valid maskTextureId, it will perform a
   * CPU blend with the mask, and upload the blend texture.
   *
   * The parameters are used to specify how the image is loaded.
   * The observer has the LoadComplete method called when the load is ready.
   *
   * When the client has finished with the Texture, Remove() should be called.
   *
   * @param[in] url                   The URL of the image to load
   * @param[in] maskTextureId         The texture id of an image to use as a mask. If no mask is required, then set
   *                                  to INVALID_TEXTURE_ID
   * @param[in] contentScale          The scaling factor to apply to the content when masking
   * @param[in] desiredSize           The size the image is likely to appear at. This can be set to 0,0 for automatic
   * @param[in] fittingMode           The FittingMode to use
   * @param[in] samplingMode          The SamplingMode to use
   * @param[in] useAtlasing           Set to USE_ATLAS to attempt atlasing. If atlasing fails, the image will still be
   *                                  loaded, and marked successful, but "useAtlasing" will be set to false in the
   *                                  "LoadCompleted" callback from the TextureManagerUploadObserver.
   * @param[in] cropToMask            Whether to crop the target after masking, or scale the mask to the image before
   *                                  masking.
   * @param[in] storageType,          Whether the pixel data is stored in the cache or uploaded to the GPU
   * @param[in] observer              The client object should inherit from this and provide the "LoadCompleted"
   *                                  virtual.
   *                                  This is called when an image load completes (or fails).
   * @param[in] orientationCorrection Whether to rotate image to match embedded orientation data
   * @param[in] reloadPolicy          Forces a reload of the texture even if already cached
   * @param[in,out] preMultiplyOnLoad True if the image color should be multiplied by it's alpha. Set to false if
   *                                  there is no alpha
   * @param[in] animatedImageLoading  The AnimatedImageLoading to load animated image
   * @param[in] frameIndex            The frame index of a frame to be loaded frame
   * @param[in] synchronousLoading    True if the frame should be loaded synchronously. If you skip this parameter,
   *                                  default is false.
   * @return                          A TextureId to use as a handle to reference this Texture
   */
  TextureId RequestLoadInternal(
    const VisualUrl&                    url,
    const TextureManager::TextureId&    maskTextureId,
    const float&                        contentScale,
    const Dali::ImageDimensions&        desiredSize,
    const Dali::FittingMode::Type&      fittingMode,
    const Dali::SamplingMode::Type&     samplingMode,
    const TextureManager::UseAtlas&     useAtlas,
    const bool&                         cropToMask,
    const TextureManager::StorageType&  storageType,
    TextureUploadObserver*              observer,
    const bool&                         orientationCorrection,
    const TextureManager::ReloadPolicy& reloadPolicy,
    TextureManager::MultiplyOnLoad&     preMultiplyOnLoad,
    Dali::AnimatedImageLoading          animatedImageLoading,
    const std::uint32_t&                frameIndex,
    const bool&                         synchronousLoading);

  /**
   * @brief Load a new image synchronously.
   * @param[in] url                   The URL of the image to load
   * @param[in] desiredSize           The size the image is likely to appear at.
   *                                  This can be set to 0,0 for automatic
   * @param[in] fittingMode           The FittingMode to use
   * @param[in] samplingMode          The SamplingMode to use
   * @param[in] orientationCorrection Whether to use image metadata to rotate or flip the image,
   *                                  e.g., from portrait to landscape
   * @return PixelBuffer of loaded image.
   */
  Devel::PixelBuffer LoadImageSynchronously(
    const VisualUrl&                url,
    const Dali::ImageDimensions&    desiredSize,
    const Dali::FittingMode::Type&  fittingMode,
    const Dali::SamplingMode::Type& samplingMode,
    const bool&                     orientationCorrection);

private:
  // Load and queue

  /**
   * Structure to hold info about a texture load queued during NotifyObservers
   */
  struct LoadQueueElement
  {
    LoadQueueElement(TextureManager::TextureId textureId, TextureUploadObserver* observer)
    : mTextureId(textureId),
      mObserver(observer)
    {
    }

    TextureManager::TextureId mTextureId; ///< The texture id of the requested load.
    TextureUploadObserver*    mObserver;  ///< Observer of texture load.
  };

  /**
   * @brief Initiate a load or queue load if NotifyObservers is invoking callbacks
   * @param[in] textureInfo The TextureInfo struct associated with the Texture
   * @param[in] observer The observer wishing to observe the texture upload
   */
  void LoadOrQueueTexture(TextureManager::TextureInfo& textureInfo, TextureUploadObserver* observer);

  /**
   * @brief Queue a texture load to be subsequently handled by ProcessQueuedTextures.
   * @param[in] textureInfo The TextureInfo struct associated with the Texture
   * @param[in] observer The observer wishing to observe the texture upload
   */
  void QueueLoadTexture(const TextureManager::TextureInfo& textureInfo, TextureUploadObserver* observer);

  /**
   * @brief Used internally to initiate a load.
   * @param[in] textureInfo The TextureInfo struct associated with the Texture
   * @param[in] observer The observer wishing to observe the texture upload
   */
  void LoadTexture(TextureManager::TextureInfo& textureInfo, TextureUploadObserver* observer);

  /**
   * @brief Initiate load of textures queued whilst NotifyObservers invoking callbacks.
   */
  void ProcessLoadQueue();

  /**
   * @brief Initiate remove of texture queued whilst NotifyObservers invoking callbacks.
   */
  void ProcessRemoveQueue();

  /**
   * Add the observer to the observer list
   * @param[in] textureInfo The TextureInfo struct associated with the texture
   * @param[in] observer The observer wishing to observe the texture upload
   */
  void ObserveTexture(TextureManager::TextureInfo& textureInfo, TextureUploadObserver* observer);

  /**
   * @brief Performs Post-Load steps including atlasing.
   * @param[in] textureInfo The struct associated with this Texture
   * @param[in] pixelBuffer The image pixelBuffer
   * @return    True if successful
   */
  void PostLoad(TextureManager::TextureInfo& textureInfo, Devel::PixelBuffer& pixelBuffer);

  /**
   * Check if there is a texture waiting to be masked. If there
   * is then apply this mask and upload it.
   * @param[in] maskTextureInfo The texture info of the mask that has just loaded.
   */
  void CheckForWaitingTexture(TextureManager::TextureInfo& maskTextureInfo);

  /**
   * Apply the mask to the pixelBuffer.
   * @param[in] textureInfo The information of texture to apply the mask to
   * @param[in] maskTextureId The texture id of the mask.
   */
  void ApplyMask(TextureManager::TextureInfo& textureInfo, const TextureManager::TextureId& maskTextureId);

  /**
   * Upload the texture specified in pixelBuffer to the appropriate location
   * @param[in] pixelBuffer The image data to upload
   * @param[in] textureInfo The texture info containing the location to
   * store the data to.
   */
  void UploadTexture(Devel::PixelBuffer& pixelBuffer, TextureManager::TextureInfo& textureInfo);

  /**
   * Notify the current observers that the texture upload is complete,
   * then remove the observers from the list.
   * @param[in] textureInfo The struct associated with this Texture
   * @param[in] success If the pixel data was retrieved successfully and uploaded to GPU
   */
  void NotifyObservers(TextureManager::TextureInfo& textureInfo, const bool& success);

  /**
   * Call LoadComplete to the observer.
   * @param[in] observer    The client object should inherit from this and provide the "LoadCompleted"
   * @param[in] textureInfo The struct associated with this Texture
   * @param[in] success     If the pixel data was retrieved successfully and uploaded to GPU
   */
  void EmitLoadComplete(TextureUploadObserver* observer, TextureManager::TextureInfo& textureInfo, const bool& success);

public:
  /**
   * @brief Common method to handle loading completion.
   * TextureAsyncLoadingHelper will call this API After async loading finished.
   * @param[in] textureId   The ID of the texture load complete.
   * @param[in] pixelBuffer The loaded image data
   */
  void AsyncLoadComplete(const TextureManager::TextureId& textureId, Devel::PixelBuffer pixelBuffer);

private:
  /**
   * Deleted copy constructor.
   */
  TextureManager(const TextureManager&) = delete;

  /**
   * Deleted assignment operator.
   */
  TextureManager& operator=(const TextureManager& rhs) = delete;

  /**
   * This is called by the TextureManagerUploadObserver when an observer is destroyed.
   * We use the callback to know when to remove an observer from our notify list.
   * @param[in] observer The observer that generated the callback
   */
  void ObserverDestroyed(TextureUploadObserver* observer);

private:                                    // Member Variables:
  TextureCacheManager mTextureCacheManager; ///< Manager the life-cycle and caching of Textures

  RoundRobinContainerView<TextureAsyncLoadingHelper> mAsyncLocalLoaders;  ///< The Asynchronous image loaders used to provide all local async loads
  RoundRobinContainerView<TextureAsyncLoadingHelper> mAsyncRemoteLoaders; ///< The Asynchronous image loaders used to provide all remote async loads

  Dali::Vector<LifecycleObserver*>        mLifecycleObservers; ///< Lifecycle observers of texture manager
  Dali::Vector<LoadQueueElement>          mLoadQueue;          ///< Queue of textures to load after NotifyObservers
  Dali::Vector<TextureManager::TextureId> mRemoveQueue;        ///< Queue of textures to remove after NotifyObservers
  bool                                    mQueueLoadFlag;      ///< Flag that causes Load Textures to be queued.
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXTURE_MANAGER_IMPL_H

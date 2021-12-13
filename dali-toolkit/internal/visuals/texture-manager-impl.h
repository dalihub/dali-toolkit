#ifndef DALI_TOOLKIT_TEXTURE_MANAGER_IMPL_H
#define DALI_TOOLKIT_TEXTURE_MANAGER_IMPL_H

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
 */

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali/devel-api/common/owner-container.h>
#include <dali/public-api/adaptor-framework/encoded-image-buffer.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/rendering/geometry.h>
#include <dali/public-api/rendering/texture-set.h>
#include <deque>
#include <functional>
#include <memory>
#include <string>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/image-loader/async-image-loader-devel.h>
#include <dali-toolkit/devel-api/image-loader/image-atlas.h>
#include <dali-toolkit/internal/helpers/round-robin-container-view.h>
#include <dali-toolkit/internal/image-loader/async-image-loader-impl.h>
#include <dali-toolkit/internal/visuals/texture-upload-observer.h>
#include <dali-toolkit/internal/visuals/visual-url.h>
#include <dali-toolkit/public-api/image-loader/async-image-loader.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class ImageAtlasManager;
typedef IntrusivePtr<ImageAtlasManager> ImageAtlasManagerPtr;

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
  typedef int32_t  TextureId;               ///< The TextureId type. This is used as a handle to refer to a particular Texture.
  static constexpr int32_t INVALID_TEXTURE_ID = -1; ///< Used to represent a null TextureId or error

  /**
   * Whether the texture should be atlased or uploaded into it's own GPU texture
   */
  enum UseAtlas
  {
    NO_ATLAS,
    USE_ATLAS
  };

  /**
   * Whether the pixel data should be kept in TextureManager, returned with pixelBuffer or uploaded for rendering
   */
  enum class StorageType : uint8_t
  {
    KEEP_PIXEL_BUFFER,
    RETURN_PIXEL_BUFFER,
    UPLOAD_TO_TEXTURE
  };

  /**
   * Whether the texture should be loaded synchronously or asynchronously.
   */
  enum class LoadType : uint8_t
  {
    LOAD_ASYNCHRONOUSLY,
    LOAD_SYNCHRONOUSLY
  };

  /**
   * @brief The LoadState Enumeration represents the current state of a particular Texture's life-cycle.
   */
  enum class LoadState : uint8_t
  {
    NOT_STARTED,      ///< Default
    LOADING,          ///< Loading has been started, but not finished.
    LOAD_FINISHED,    ///< Loading has finished. (for CPU storage only)
    WAITING_FOR_MASK, ///< Loading has finished, but waiting for mask image
    MASK_APPLYING,    ///< Loading has finished, Mask is applying
    MASK_APPLIED,     ///< Loading has finished, Mask is applyied by GPU
    UPLOADED,         ///< Uploaded and ready. (For GPU upload only)
    CANCELLED,        ///< Removed before loading completed
    LOAD_FAILED       ///< Async loading failed, e.g. connection problem
  };

  /**
   * @brief Types of reloading policies
   */
  enum class ReloadPolicy
  {
    CACHED = 0, ///< Loads cached texture if it exists.
    FORCED      ///< Forces reloading of texture.
  };

  /**
   * @brief Whether to multiply alpha into color channels on load
   */
  enum class MultiplyOnLoad
  {
    LOAD_WITHOUT_MULTIPLY = 0, ///< Don't modify the image
    MULTIPLY_ON_LOAD           ///< Multiply alpha into color channels on load
  };

public:
  struct MaskingData
  {
    MaskingData();
    ~MaskingData() = default;

    VisualUrl                 mAlphaMaskUrl;
    TextureManager::TextureId mAlphaMaskId;
    float                     mContentScaleFactor;
    bool                      mCropToMask;
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
   * @param[in]  animatedImageLoading  The AnimatedImageLoading that contain the animated image information
   * @param[in]  frameIndex            The frame index to load.
   * @param[out] loadingStatus         The loading status of the texture
   * @param[out] textureId             The textureId of the frame
   * @param[in, out] maskInfo          Mask info structure
   * @param[in]  samplingMode          The SamplingMode to use
   * @param[in]  wrapModeU             Horizontal Wrap mode
   * @param[in]  wrapModeV             Vertical Wrap mode
   * @param[in]  synchronousLoading    true if the frame should be loaded synchronously
   * @param[in]  useCache              true if this frame loading uses cache.
   * @param[in]  textureObserver       The client object should inherit from this and provide the "UploadCompleted" virtual.
   *                                   This is called when an image load completes (or fails).
   *
   * @return                           The texture set containing the frame of animated image, or empty if still loading.
   */

  TextureSet LoadAnimatedImageTexture(Dali::AnimatedImageLoading animatedImageLoading,
                                      uint32_t                   frameIndex,
                                      bool&                      loadingStatus,
                                      TextureManager::TextureId& textureId,
                                      MaskingDataPointer&        maskInfo,
                                      Dali::SamplingMode::Type   samplingMode,
                                      Dali::WrapMode::Type       wrapModeU,
                                      Dali::WrapMode::Type       wrapModeV,
                                      bool                       synchronousLoading,
                                      bool                       useCache,
                                      TextureUploadObserver*     textureObserver);

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
   * @param[in] textureObserver       The client object should inherit from this and provide the "UploadCompleted" virtual.
   *                                  This is called when an image load completes (or fails).
   * @param[in] orientationCorrection Whether to rotate image to match embedded orientation data
   * @param[in,out] preMultiplyOnLoad True if the image color should be multiplied by it's alpha. Set to false if the
   *                                  image has no alpha channel
   *
   * @return                          The pixel buffer containing the image, or empty if still loading.
   */

  Devel::PixelBuffer LoadPixelBuffer(const VisualUrl&                url,
                                     Dali::ImageDimensions           desiredSize,
                                     Dali::FittingMode::Type         fittingMode,
                                     Dali::SamplingMode::Type        samplingMode,
                                     bool                            synchronousLoading,
                                     TextureUploadObserver*          textureObserver,
                                     bool                            orientationCorrection,
                                     TextureManager::MultiplyOnLoad& preMultiplyOnLoad);

  /**
   * @brief Requests an image load of the given URL.
   *
   * The parameters are used to specify how the image is loaded.
   * The observer has the UploadComplete method called when the load is ready.
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
   * @param[in] textureObserver       The client object should inherit from this and provide the "UploadCompleted" virtual.
   *                                  This is called when an image load completes (or fails).
   * @param[in] atlasObserver         This is used if the texture is atlased, and will be called instead of
   *                                  textureObserver.UploadCompleted
   * @param[in] imageAtlasManager     The atlas manager to use for atlasing textures
   * @param[in] orientationCorrection Whether to rotate image to match embedded orientation data
   * @param[in] reloadPolicy          Forces a reload of the texture even if already cached
   * @param[in,out] preMultiplyOnLoad True if the image color should be multiplied by it's alpha. Set to false if the
   *                                  image has no alpha channel
   *
   * @return                          The texture set containing the image, or empty if still loading.
   */

  TextureSet LoadTexture(const VisualUrl&             url,
                         Dali::ImageDimensions        desiredSize,
                         Dali::FittingMode::Type      fittingMode,
                         Dali::SamplingMode::Type     samplingMode,
                         MaskingDataPointer&          maskInfo,
                         bool                         synchronousLoading,
                         TextureManager::TextureId&   textureId,
                         Vector4&                     textureRect,
                         Dali::ImageDimensions&       textureRectSize,
                         bool&                        atlasingStatus,
                         bool&                        loadingStatus,
                         Dali::WrapMode::Type         wrapModeU,
                         Dali::WrapMode::Type         wrapModeV,
                         TextureUploadObserver*       textureObserver,
                         AtlasUploadObserver*         atlasObserver,
                         ImageAtlasManagerPtr         imageAtlasManager,
                         bool                         orientationCorrection,
                         TextureManager::ReloadPolicy reloadPolicy,
                         MultiplyOnLoad&              preMultiplyOnLoad);

  /**
   * @brief Requests an image load of the given URL.
   *
   * The parameters are used to specify how the image is loaded.
   * The observer has the UploadComplete method called when the load is ready.
   *
   * When the client has finished with the Texture, Remove() should be called.
   *
   * @param[in] url                   The URL of the image to load
   * @param[in] desiredSize           The size the image is likely to appear at. This can be set to 0,0 for automatic
   * @param[in] fittingMode           The FittingMode to use
   * @param[in] samplingMode          The SamplingMode to use
   * @param[in] useAtlasing           Set to USE_ATLAS to attempt atlasing. If atlasing fails, the image will still be loaded, and marked successful,
   *                                  but "useAtlasing" will be set to false in the "UploadCompleted" callback from the TextureManagerUploadObserver.
   * @param[in] observer              The client object should inherit from this and provide the "UploadCompleted" virtual.
   *                                  This is called when an image load completes (or fails).
   * @param[in] orientationCorrection Whether to rotate image to match embedded orientation data
   * @param[in] reloadPolicy          Forces a reload of the texture even if already cached
   * @param[in,out] preMultiplyOnLoad     True if the image color should be multiplied by it's alpha. Set to false if the image has no alpha channel
   * @return                          A TextureId to use as a handle to reference this Texture
   */
  TextureId RequestLoad(const VisualUrl&             url,
                        const ImageDimensions        desiredSize,
                        FittingMode::Type            fittingMode,
                        Dali::SamplingMode::Type     samplingMode,
                        const UseAtlas               useAtlasing,
                        TextureUploadObserver*       observer,
                        bool                         orientationCorrection,
                        TextureManager::ReloadPolicy reloadPolicy,
                        MultiplyOnLoad&              preMultiplyOnLoad);

  /**
   * @brief Requests an image load of the given URL, when the texture has
   * have loaded, it will perform a blend with the image mask, and upload
   * the blended texture.
   *
   * The parameters are used to specify how the image is loaded.
   * The observer has the UploadComplete method called when the load is ready.
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
   *                                  but "useAtlasing" will be set to false in the "UploadCompleted" callback from
   *                                  the TextureManagerUploadObserver.
   * @param[in] cropToMask            Only used with masking, this will crop the scaled image to the mask size.
   *                                  If false, then the mask will be scaled to fit the image before being applied.
   * @param[in] observer              The client object should inherit from this and provide the "UploadCompleted"
   *                                  virtual.
   *                                  This is called when an image load completes (or fails).
   * @param[in] orientationCorrection Whether to rotate image to match embedded orientation data
   * @param[in] reloadPolicy          Forces a reload of the texture even if already cached
   * @param[in] preMultiplyOnLoad     True if the image color should be multiplied by it's alpha. Set to false if the
   *                                  image has no alpha channel
   * @return                          A TextureId to use as a handle to reference this Texture
   */
  TextureId RequestLoad(const VisualUrl&             url,
                        TextureId                    maskTextureId,
                        float                        contentScale,
                        const ImageDimensions        desiredSize,
                        FittingMode::Type            fittingMode,
                        Dali::SamplingMode::Type     samplingMode,
                        const UseAtlas               useAtlasing,
                        bool                         cropToMask,
                        TextureUploadObserver*       observer,
                        bool                         orientationCorrection,
                        TextureManager::ReloadPolicy reloadPolicy,
                        MultiplyOnLoad&              preMultiplyOnLoad);

  /**
   * Requests a masking image to be loaded. This mask is not uploaded to GL,
   * instead, it is stored in CPU memory, and can be used for CPU blending.
   */
  TextureId RequestMaskLoad(const VisualUrl& maskUrl);

  /**
   * @brief Remove a Texture from the TextureManager.
   *
   * Textures are cached and therefore only the removal of the last
   * occurrence of a Texture will cause its removal internally.
   *
   * @param[in] textureId The ID of the Texture to remove.
   * @param[in] textureObserver The texture observer.
   */
  void Remove(const TextureManager::TextureId textureId, TextureUploadObserver* textureObserver);

  /**
   * @brief Get the visualUrl associated with the texture id.
   * @param[in] textureId The texture Id to get
   * @return The visual Url associated with the texture id.
   */
  VisualUrl GetVisualUrl(TextureId textureId);

  /**
   * @brief Get the current state of a texture
   * @param[in] textureId The texture id to query
   * @return The loading state if the texture is valid, or NOT_STARTED if the textureId
   * is not valid.
   */
  LoadState GetTextureState(TextureId textureId);

  /**
   * @brief Get the associated texture set if the texture id is valid
   * @param[in] textureId The texture Id to look up
   * @return the associated texture set, or an empty handle if textureId is not valid
   */
  TextureSet GetTextureSet(TextureId textureId);

  /**
   * @brief Get the encoded image buffer
   * @param[in] textureId The textureId to look up
   * @return the encoded image buffer, or an empty handle if textureId is not valid
   */
  EncodedImageBuffer GetEncodedImageBuffer(TextureId textureId);

  /**
   * @brief Get the encoded image buffer by VisualUrl
   * @param[in] url The url to look up
   * @return the encoded image buffer, or an empty handle if url is not buffer resource or buffer is not valid
   */
  EncodedImageBuffer GetEncodedImageBuffer(const std::string& url);

  /**
   * Adds an external texture to the texture manager
   * @param[in] texture The texture to add
   * @return string containing the URL for the texture
   */
  std::string AddExternalTexture(TextureSet& texture);

  /**
   * Adds an external encoded image buffer to the texture manager
   * @param[in] encodedImageBuffer The image buffer to add
   * @return string containing the URL for the texture
   */
  std::string AddExternalEncodedImageBuffer(const EncodedImageBuffer& encodedImageBuffer);

  /**
   * Removes an external texture from texture manager
   * @param[in] url The string containing the texture to remove
   * @return handle to the texture
   */
  TextureSet RemoveExternalTexture(const std::string& url);

  /**
   * Removes an external encoded image buffer from texture manager
   * @param[in] url The string containing the encoded image buffer to remove
   * @return handle to the encoded image buffer
   */
  EncodedImageBuffer RemoveExternalEncodedImageBuffer(const std::string& url);

  /**
   * @brief Notify that external textures or external encoded image buffers are used.
   * @param[in] url The URL of the texture to use.
   */
  void UseExternalResource(const VisualUrl& url);

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
  Geometry GetRenderGeometry(TextureId textureId, uint32_t& frontElements, uint32_t& backElements);

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
   * @param[in] url                   The URL of the image to load
   * @param[in] maskTextureId         The texture id of an image to use as a mask. If no mask is required, then set
   *                                  to INVALID_TEXTURE_ID
   * @param[in] contentScale          The scaling factor to apply to the content when masking
   * @param[in] desiredSize           The size the image is likely to appear at. This can be set to 0,0 for automatic
   * @param[in] fittingMode           The FittingMode to use
   * @param[in] samplingMode          The SamplingMode to use
   * @param[in] useAtlasing           Set to USE_ATLAS to attempt atlasing. If atlasing fails, the image will still be
   *                                  loaded, and marked successful, but "useAtlasing" will be set to false in the
   *                                  "UploadCompleted" callback from the TextureManagerUploadObserver.
   * @param[in] cropToMask            Whether to crop the target after masking, or scale the mask to the image before
   *                                  masking.
   * @param[in] storageType,          Whether the pixel data is stored in the cache or uploaded to the GPU
   * @param[in] observer              The client object should inherit from this and provide the "UploadCompleted"
   *                                  virtual.
   *                                  This is called when an image load completes (or fails).
   * @param[in] orientationCorrection Whether to rotate image to match embedded orientation data
   * @param[in] reloadPolicy          Forces a reload of the texture even if already cached
   * @param[in] preMultiplyOnLoad     True if the image color should be multiplied by it's alpha. Set to false if
   *                                  there is no alpha
   * @param[in] animatedImageLoading  The AnimatedImageLoading to load animated image
   * @param[in] frameIndex            The frame index of a frame to be loaded frame
   * @return                          A TextureId to use as a handle to reference this Texture
   */
  TextureId RequestLoadInternal(
    const VisualUrl&             url,
    TextureId                    maskTextureId,
    float                        contentScale,
    const ImageDimensions        desiredSize,
    FittingMode::Type            fittingMode,
    Dali::SamplingMode::Type     samplingMode,
    UseAtlas                     useAtlas,
    bool                         cropToMask,
    StorageType                  storageType,
    TextureUploadObserver*       observer,
    bool                         orientationCorrection,
    TextureManager::ReloadPolicy reloadPolicy,
    MultiplyOnLoad&              preMultiplyOnLoad,
    Dali::AnimatedImageLoading   animatedImageLoading,
    uint32_t                     frameIndex,
    bool useCache);

  /**
   * @brief Get the current state of a texture
   * @param[in] textureId The texture id to query
   * @return The loading state if the texture is valid, or NOT_STARTED if the textureId
   * is not valid.
   */
  LoadState GetTextureStateInternal(TextureId textureId);

  typedef size_t TextureHash; ///< The type used to store the hash used for Texture caching.

  // Structs:

  /**
   * @brief This struct is used to manage the life-cycle of Texture loading and caching.
   */
  struct TextureInfo
  {
    TextureInfo(TextureId                   textureId,
                TextureId                   maskTextureId,
                const VisualUrl&            url,
                ImageDimensions             desiredSize,
                float                       scaleFactor,
                FittingMode::Type           fittingMode,
                Dali::SamplingMode::Type    samplingMode,
                bool                        loadSynchronously,
                bool                        cropToMask,
                UseAtlas                    useAtlas,
                TextureManager::TextureHash hash,
                bool                        orientationCorrection,
                bool                        preMultiplyOnLoad,
                Dali::AnimatedImageLoading  animatedImageLoading,
                uint32_t                    frameIndex)
    : url(url),
      desiredSize(desiredSize),
      useSize(desiredSize),
      atlasRect(0.0f, 0.0f, 1.0f, 1.0f), // Full atlas rectangle
      textureId(textureId),
      maskTextureId(maskTextureId),
      hash(hash),
      scaleFactor(scaleFactor),
      referenceCount(1u),
      loadState(LoadState::NOT_STARTED),
      fittingMode(fittingMode),
      samplingMode(samplingMode),
      storageType(StorageType::UPLOAD_TO_TEXTURE),
      animatedImageLoading(animatedImageLoading),
      frameIndex(frameIndex),
      frameCount(0u),
      frameInterval(0u),
      loadSynchronously(loadSynchronously),
      useAtlas(useAtlas),
      cropToMask(cropToMask),
      orientationCorrection(true),
      preMultiplyOnLoad(preMultiplyOnLoad),
      preMultiplied(false)
    {
      isAnimatedImageFormat = (animatedImageLoading) ? true : false;
    }

    /**
     * Container type used to store all observer clients of this Texture
     */
    typedef Dali::Vector<TextureUploadObserver*> ObserverListType;

    ObserverListType            observerList;          ///< Container used to store all observer clients of this Texture
    Toolkit::ImageAtlas         atlas;                 ///< The atlas this Texture lays within (if any)
    Devel::PixelBuffer          pixelBuffer;           ///< The PixelBuffer holding the image data (May be empty after upload)
    TextureSet                  textureSet;            ///< The TextureSet holding the Texture
    VisualUrl                   url;                   ///< The URL of the image
    ImageDimensions             desiredSize;           ///< The size requested
    ImageDimensions             useSize;               ///< The size used
    Vector4                     atlasRect;             ///< The atlas rect used if atlased
    TextureId                   textureId;             ///< The TextureId associated with this Texture
    TextureId                   maskTextureId;         ///< The mask TextureId to be applied on load
    TextureManager::TextureHash hash;                  ///< The hash used to cache this Texture
    float                       scaleFactor;           ///< The scale factor to apply to the Texture when masking
    int16_t                     referenceCount;        ///< The reference count of clients using this Texture
    LoadState                   loadState;             ///< The load state showing the load progress of the Texture
    FittingMode::Type           fittingMode : 3;       ///< The requested FittingMode
    Dali::SamplingMode::Type    samplingMode : 3;      ///< The requested SamplingMode
    StorageType                 storageType;           ///< CPU storage / GPU upload;
    Dali::AnimatedImageLoading  animatedImageLoading;  ///< AnimatedImageLoading that contains animated image information.
    uint32_t                    frameIndex;            ///< Frame index that be loaded, in case of animated image
    uint32_t                    frameCount;            ///< Total frame count of input animated image. If this variable is not 0, this textureInfo is for animated image file format.
    uint32_t                    frameInterval;         ///< Time interval between this frame and next frame of animated image.
    bool                        loadSynchronously : 1; ///< True if synchronous loading was requested
    UseAtlas                    useAtlas : 2;          ///< USE_ATLAS if an atlas was requested.
                                                       ///< This is updated to false if atlas is not used
    bool cropToMask : 1;                               ///< true if the image should be cropped to the mask size.
    bool orientationCorrection : 1;                    ///< true if the image should be rotated to match exif orientation data
    bool preMultiplyOnLoad : 1;                        ///< true if the image's color should be multiplied by it's alpha
    bool preMultiplied : 1;                            ///< true if the image's color was multiplied by it's alpha
    bool isAnimatedImageFormat : 1;                    ///< true if the image is requested from animated image visual.
  };

  /**
   * Structure to hold info about a texture load queued during NotifyObservers
   */
  struct LoadQueueElement
  {
    LoadQueueElement(TextureId textureId, TextureUploadObserver* observer)
    : mTextureId(textureId),
      mObserver(observer)
    {
    }

    TextureId              mTextureId; ///< The texture id of the requested load.
    TextureUploadObserver* mObserver;  ///< Observer of texture load.
  };

  /**
   * Struct to hold information about a requested Async load.
   * This is used to look up a TextureManager::TextureId from the returned AsyncLoad Id.
   */
  struct AsyncLoadingInfo
  {
    AsyncLoadingInfo(TextureId textureId)
    : textureId(textureId),
      loadId(0)
    {
    }

    TextureId textureId; ///< The external Texture Id assigned to this load
    uint32_t  loadId;    ///< The load Id used by the async loader to reference this load
  };

  // Private typedefs:

  typedef std::deque<AsyncLoadingInfo> AsyncLoadingInfoContainerType; ///< The container type used to manage Asynchronous loads in progress
  typedef std::vector<TextureInfo>     TextureInfoContainerType;      ///< The container type used to manage the life-cycle and caching of Textures

  /**
   * @brief Initiate a load or queue load if NotifyObservers is invoking callbacks
   * @param[in] textureInfo The TextureInfo struct associated with the Texture
   * @param[in] observer The observer wishing to observe the texture upload
   */
  void LoadOrQueueTexture(TextureInfo& textureInfo, TextureUploadObserver* observer);

  /**
   * @brief Queue a texture load to be subsequently handled by ProcessQueuedTextures.
   * @param[in] textureInfo The TextureInfo struct associated with the Texture
   * @param[in] observer The observer wishing to observe the texture upload
   */
  void QueueLoadTexture(TextureInfo& textureInfo, TextureUploadObserver* observer);

  /**
   * @brief Used internally to initiate a load.
   * @param[in] textureInfo The TextureInfo struct associated with the Texture
   * @param[in] observer The observer wishing to observe the texture upload
   */
  void LoadTexture(TextureInfo& textureInfo, TextureUploadObserver* observer);

  /**
   * @brief Initiate load of textures queued whilst NotifyObservers invoking callbacks.
   */
  void ProcessQueuedTextures();

  /**
   * Add the observer to the observer list
   * @param[in] textureInfo The TextureInfo struct associated with the texture
   * @param[in] observer The observer wishing to observe the texture upload
   */
  void ObserveTexture(TextureInfo& textureInfo, TextureUploadObserver* observer);

  /**
   * @brief This signal handler is called when the async local loader finishes loading.
   * @param[in] id        This is the async image loaders Id
   * @param[in] pixelBuffer The loaded image data
   */
  void AsyncLocalLoadComplete(uint32_t id, Devel::PixelBuffer pixelBuffer);

  /**
   * @brief This signal handler is called when the async local loader finishes loading.
   * @param[in] id        This is the async image loaders Id
   * @param[in] pixelBuffer The loaded image data
   */
  void AsyncRemoteLoadComplete(uint32_t id, Devel::PixelBuffer pixelBuffer);

  /**
   * Common method to handle loading completion
   * @param[in] container The Async loading container
   * @param[in] id        This is the async image loaders Id
   * @param[in] pixelBuffer The loaded image data
   */
  void AsyncLoadComplete(AsyncLoadingInfoContainerType& container, uint32_t id, Devel::PixelBuffer pixelBuffer);

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
  void CheckForWaitingTexture(TextureInfo& maskTextureInfo);

  /**
   * Apply the mask to the pixelBuffer.
   * @param[in] textureInfo The information of texture to apply the mask to
   * @param[in] maskTextureId The texture id of the mask.
   */
  void ApplyMask(TextureInfo& textureInfo, TextureId maskTextureId);

  /**
   * Upload the texture specified in pixelBuffer to the appropriate location
   * @param[in] pixelBuffer The image data to upload
   * @param[in] textureInfo The texture info containing the location to
   * store the data to.
   */
  void UploadTexture(Devel::PixelBuffer& pixelBuffer, TextureInfo& textureInfo);

  /**
   * Creates tiled geometry of for the texture which separates fully-opaque
   * tiles from ones which use transparency.
   * @param pixelBuffer
   * @param textureInfo
   */
  bool CreateTiledGeometry(const Devel::PixelBuffer& pixelBuffer, TextureInfo& textureInfo);

  /**
   * Mark the texture as complete, and inform observers
   * @param[in] textureInfo The struct associated with this Texture
   */
  void UploadComplete(TextureInfo& textureInfo);

  /**
   * Notify the current observers that the texture upload is complete,
   * then remove the observers from the list.
   * @param[in] textureInfo The struct associated with this Texture
   * @param[in] success If the pixel data was retrieved successfully and uploaded to GPU
   */
  void NotifyObservers(TextureInfo& textureInfo, bool success);

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
  int GetCacheIndexFromId(TextureId textureId);

  /**
   * @brief Generates a hash for caching based on the input parameters.
   * Only applies size, fitting mode andsampling mode if the size is specified.
   * Only applies maskTextureId if it isn't INVALID_TEXTURE_ID
   * Always applies useAtlas.
   * @param[in] url              The URL of the image to load
   * @param[in] size             The image size
   * @param[in] fittingMode      The FittingMode to use
   * @param[in] samplingMode     The SamplingMode to use
   * @param[in] useAtlas         True if atlased
   * @param[in] maskTextureId    The masking texture id (or INVALID_TEXTURE_ID)
   * @return                     A hash of the provided data for caching.
   */
  TextureHash GenerateHash(const std::string& url, const ImageDimensions size, const FittingMode::Type fittingMode, const Dali::SamplingMode::Type samplingMode, const UseAtlas useAtlas, TextureId maskTextureId);

  /**
   * @brief Looks up a cached texture by its hash.
   * If found, the given parameters are used to check there is no hash-collision.
   * @param[in] hash              The hash to look up
   * @param[in] url               The URL of the image to load
   * @param[in] size              The image size
   * @param[in] fittingMode       The FittingMode to use
   * @param[in] samplingMode      The SamplingMode to use
   * @param[in] useAtlas          True if atlased
   * @param[in] maskTextureId     Optional texture ID to use to mask this image
   * @param[in] preMultiplyOnLoad If the image's color should be multiplied by it's alpha. Set to OFF if there is no alpha.
   * @param[in] isAnimatedImage   True if the texture is from animated image.
   * @return                      A TextureId of a cached Texture if found. Or INVALID_TEXTURE_ID if not found.
   */
  TextureManager::TextureId FindCachedTexture(
    const TextureManager::TextureHash hash,
    const std::string&                url,
    const ImageDimensions             size,
    const FittingMode::Type           fittingMode,
    const Dali::SamplingMode::Type    samplingMode,
    const bool                        useAtlas,
    TextureId                         maskTextureId,
    MultiplyOnLoad                    preMultiplyOnLoad,
    bool                              isAnimatedImage);

private:
  /**
   * @brief Helper class to keep the relation between AsyncImageLoader and corresponding LoadingInfo container
   */
  class AsyncLoadingHelper : public ConnectionTracker
  {
  public:
    /**
     * @brief Create an AsyncLoadingHelper.
     * @param[in] textureManager Reference to the texture manager
     */
    AsyncLoadingHelper(TextureManager& textureManager);

    /**
     * @brief Load a new frame of animated image
     * @param[in] textureId             TextureId to reference the texture that will be loaded
     * @param[in] animatedImageLoading  The AnimatedImageLoading to load animated image
     * @param[in] frameIndex            The frame index of a frame to be loaded frame
     */
    void LoadAnimatedImage(TextureId                  textureId,
                           Dali::AnimatedImageLoading animatedImageLoading,
                           uint32_t                   frameIndex);

    /**
     * @brief Load a new texture.
     * @param[in] textureId             TextureId to reference the texture that will be loaded
     * @param[in] url                   The URL of the image to load
     * @param[in] desiredSize           The size the image is likely to appear at.
     *                                  This can be set to 0,0 for automatic
     * @param[in] fittingMode           The FittingMode to use
     * @param[in] samplingMode          The SamplingMode to use
     * @param[in] orientationCorrection Whether to use image metadata to rotate or flip the image,
     *                                  e.g., from portrait to landscape
     * @param[in] preMultiplyOnLoad     if the image's color should be multiplied by it's alpha. Set to OFF if there is no alpha or if the image need to be applied alpha mask.
     */
    void Load(TextureId                                textureId,
              const VisualUrl&                         url,
              ImageDimensions                          desiredSize,
              FittingMode::Type                        fittingMode,
              SamplingMode::Type                       samplingMode,
              bool                                     orientationCorrection,
              DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad);

    /**
     * @brief Apply mask
     * @param [in] id of the texture
     * @param [in] pixelBuffer of the to be masked image
     * @param [in] maskPixelBuffer of the mask image
     * @param [in] contentScale The factor to scale the content
     * @param [in] cropToMask Whether to crop the content to the mask size
     * @param [in] preMultiplyOnLoad if the image's color should be multiplied by it's alpha. Set to OFF if there is no alpha.
     */
    void ApplyMask(TextureId                                textureId,
                   Devel::PixelBuffer                       pixelBuffer,
                   Devel::PixelBuffer                       maskPixelBuffer,
                   float                                    contentScale,
                   bool                                     cropToMask,
                   DevelAsyncImageLoader::PreMultiplyOnLoad preMultiplyOnLoad);

  public:
    AsyncLoadingHelper(const AsyncLoadingHelper&) = delete;
    AsyncLoadingHelper& operator=(const AsyncLoadingHelper&) = delete;

    AsyncLoadingHelper(AsyncLoadingHelper&& rhs);
    AsyncLoadingHelper& operator=(AsyncLoadingHelper&& rhs) = delete;

  private:
    /**
     * @brief Main constructor that used by all other constructors
     */
    AsyncLoadingHelper(Toolkit::AsyncImageLoader       loader,
                       TextureManager&                 textureManager,
                       AsyncLoadingInfoContainerType&& loadingInfoContainer);

    /**
     * @brief Callback to be called when texture loading is complete, it passes the pixel buffer on to texture manager.
     * @param[in] id          Loader id
     * @param[in] pixelBuffer Image data
     */
    void AsyncLoadComplete(uint32_t id, Devel::PixelBuffer pixelBuffer);

  private:
    Toolkit::AsyncImageLoader     mLoader;
    TextureManager&               mTextureManager;
    AsyncLoadingInfoContainerType mLoadingInfoContainer;
  };

  struct ExternalTextureInfo
  {
    TextureId  textureId;
    TextureSet textureSet;
    int16_t    referenceCount{1};
  };

  struct EncodedBufferTextureInfo
  {
    EncodedBufferTextureInfo(TextureId                 textureId,
                             const EncodedImageBuffer& encodedImageBuffer)
    : textureId(textureId),
      encodedImageBuffer(encodedImageBuffer),
      referenceCount(1u)
    {
    }
    TextureId          textureId;
    EncodedImageBuffer encodedImageBuffer;
    int16_t            referenceCount;
  };

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

private:                                                              // Member Variables:
  TextureInfoContainerType                    mTextureInfoContainer;  ///< Used to manage the life-cycle and caching of Textures
  RoundRobinContainerView<AsyncLoadingHelper> mAsyncLocalLoaders;     ///< The Asynchronous image loaders used to provide all local async loads
  RoundRobinContainerView<AsyncLoadingHelper> mAsyncRemoteLoaders;    ///< The Asynchronous image loaders used to provide all remote async loads
  std::vector<ExternalTextureInfo>            mExternalTextures;      ///< Externally provided textures
  std::vector<EncodedBufferTextureInfo>       mEncodedBufferTextures; ///< Externally encoded buffer textures
  Dali::Vector<LifecycleObserver*>            mLifecycleObservers;    ///< Lifecycle observers of texture manager
  Dali::Vector<LoadQueueElement>              mLoadQueue;             ///< Queue of textures to load after NotifyObservers
  TextureId                                   mCurrentTextureId;      ///< The current value used for the unique Texture Id generation
  bool                                        mQueueLoadFlag;         ///< Flag that causes Load Textures to be queued.
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXTURE_MANAGER_IMPL_H

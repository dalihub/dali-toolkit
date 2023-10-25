#ifndef DALI_TOOLKIT_NPATCH_DATA_H
#define DALI_TOOLKIT_NPATCH_DATA_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/rendering/texture-set.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/utility/npatch-utilities.h>
#include <dali-toolkit/internal/texture-manager/texture-manager-impl.h>
#include <dali-toolkit/internal/visuals/visual-url.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class NPatchData : public ConnectionTracker, public Dali::Toolkit::TextureUploadObserver
{
public:
  typedef int32_t           NPatchDataId;                ///< The NPatchDataId type. This is used as a handle to refer to a particular Npatch Data.
  static const NPatchDataId INVALID_NPATCH_DATA_ID = -1; ///< Used to represent a null TextureId or error

  /**
   * @brief Loading State of the NPatch image.
   */
  enum class LoadingState
  {
    NOT_STARTED = 0, ///< NPatch loading is not started yet.
    LOADING,         ///< NPatch is on loading.
    LOAD_COMPLETE,   ///< NPatch loading is completed successfully.
    LOAD_FAILED      ///< NPatch loading is failed.
  };

public:
  /**
   * Constructor
   */
  NPatchData();

  /**
   * Destructor, non-virtual as not a base class
   */
  ~NPatchData();

public:
  /**
   * @brief Set cache data id.
   *
   * @param [in] id cache data id
   */
  void SetId(const NPatchDataId id);

  /**
   * @brief Retrieve cache data id
   *
   * @return cache data id.
   */
  NPatchDataId GetId() const;

  /**
   * @brief Add TextureUploadObserver that uses the image of this cache data.
   *
   * @param [in] textureObserver the TextureUploadObserver that uses the image of this cache data.
   */
  void AddObserver(TextureUploadObserver* textureObserver);

  /**
   * @brief Remove TextureUploadObserver.
   *
   * @param [in] textureObserver the TextureUploadObserver that will be removed in this cache data.
   */
  void RemoveObserver(TextureUploadObserver* textureObserver);

  /**
   * @brief Retrieve the number of observer.
   *
   * @return Return the number of observer.
   */
  uint32_t GetObserverCount() const;

  /**
   * @brief Set NPatch image url.
   *
   * @param [in] url NPatch image url
   */
  void SetUrl(const VisualUrl& url);

  /**
   * @brief Retrieve the image url.
   *
   * @return Return the image url.
   */
  VisualUrl GetUrl() const;

  /**
   * @brief Set texture set on the cache data
   *
   * @param [in] textureSet loaded texture set
   */
  void SetTextures(const TextureSet textureSet);

  /**
   * @brief Retrieve loaded texture set.
   *
   * @return Return loaded texture set.
   */
  TextureSet GetTextures() const;

  /**
   * @brief Set X directional stretchPixels
   *
   * @param [in] stretchPixelsX stretchPixels for X direction
   */
  void SetStretchPixelsX(const NPatchUtility::StretchRanges stretchPixelsX);

  /**
   * @brief Set Y directional stretchPixels
   *
   * @param [in] stretchPixelsY stretchPixels for Y direction
   */
  void SetStretchPixelsY(const NPatchUtility::StretchRanges stretchPixelsY);

  /**
   * @brief Retrieve stretchPixels for X direction.
   *
   * @return Return stretchPixels for X direction.
   */
  NPatchUtility::StretchRanges GetStretchPixelsX() const;

  /**
   * @brief Retrieve stretchPixels for Y direction.
   *
   * @return Return stretchPixels for Y direction.
   */
  NPatchUtility::StretchRanges GetStretchPixelsY() const;

  /**
   * @brief Set cache data hash.
   *
   * @param [in] hash cache hash
   */
  void SetHash(std::size_t hash);

  /**
   * @brief Retrieve hash value of the cache.
   *
   * @return Return hash value of the cache.
   */
  std::size_t GetHash() const;

  /**
   * @brief Set croppedWidth of NPatch
   *
   * @param [in] croppedWidth croppedWidth of NPatch
   */
  void SetCroppedWidth(uint32_t croppedWidth);

  /**
   * @brief Set croppedHeight of NPatch
   *
   * @param [in] croppedHeight croppedHeight of NPatch
   */
  void SetCroppedHeight(uint32_t croppedHeight);

  /**
   * @brief Retrieve croppedWidth of NPatch.
   *
   * @return Return croppedWidth of NPatch.
   */
  uint32_t GetCroppedWidth() const;

  /**
   * @brief Retrieve croppedHeight of NPatch.
   *
   * @return Return croppedHeight of NPatch.
   */
  uint32_t GetCroppedHeight() const;

  /**
   * @brief Set border of NPatch.
   *
   * @param [in] border border of NPatch
   */
  void SetBorder(const Rect<int> border);

  /**
   * @brief Retrieve border of NPatch.
   *
   * @return Return border of NPatch.
   */
  Rect<int> GetBorder() const;

  /**
   * @brief Set whether the loaded image is premultiplied or not
   *
   * @param [in] preMultiplyOnLoad whether the loaded image is premultiplied or not
   */
  void SetPreMultiplyOnLoad(bool preMultiplyOnLoad);

  /**
   * @brief Retrieve whether the loaded image is premultiplied or not.
   *
   * @return Return true if the image is premultiplied alpha.
   */
  bool IsPreMultiplied() const;

  /**
   * @brief Set current loading state.
   *
   * @param [in] loadingState current loading state
   */
  void SetLoadingState(const LoadingState loadingState);

  /**
   * @brief Retrieve current loading state.
   *
   * @return Return current loading state.
   */
  LoadingState GetLoadingState() const;

  /**
   * @brief Retrieve NPatch rendering data.
   *
   * @return Return NPatch rendering data.
   */
  void* GetRenderingMap() const;

  /**
   * @brief Set loaded pixel buffer for the cache data.
   *
   * @param [in] pixelBuffer loaded pixel buffer.
   * @param [in] preMultiplied whether the loaded image is premultiplied or not
   */
  void SetLoadedNPatchData(Devel::PixelBuffer& pixelBuffer, bool preMultiplied);

  /**
   * @brief Send LoadComplete notify with current setuped NPatchData
   *
   * @param [in] observer observer who will be got LoadComplete notify
   * @param [in] loadSuccess whether the image load success or not.
   */
  void NotifyObserver(TextureUploadObserver* observer, const bool& loadSuccess);

private:
  /**
   * @copydoc TextureUploadObserver::LoadComplete
   *
   * To avoid rendering garbage pixels, renderer should be added to actor after the resources are ready.
   * This callback is the place to add the renderer as it would be called once the loading is finished.
   */
  void LoadComplete(bool loadSuccess, TextureInformation textureInformation) override;

  /**
   * This is called by the TextureUploadObserver when an observer is destroyed.
   * We use the callback to know when to remove an observer from our notify list.
   * @param[in] observer The observer that generated the callback
   */
  void ObserverDestroyed(TextureUploadObserver* observer);

private:
  using ObserverListType = Dali::Vector<TextureUploadObserver*>;

  NPatchDataId                 mId;
  ObserverListType             mObserverList;    ///< Container used to store all observer clients of this Texture
  ObserverListType             mQueuedObservers; ///< Container observers when user try to add during notify observers
  VisualUrl                    mUrl;             ///< Url of the N-Patch
  TextureSet                   mTextureSet;      ///< Texture containing the cropped image
  NPatchUtility::StretchRanges mStretchPixelsX;  ///< X stretch pixels
  NPatchUtility::StretchRanges mStretchPixelsY;  ///< Y stretch pixels
  std::size_t                  mHash;            ///< Hash code for the Url
  uint32_t                     mCroppedWidth;    ///< Width of the cropped middle part of N-patch
  uint32_t                     mCroppedHeight;   ///< Height of the cropped middle part of N-patch
  Rect<int>                    mBorder;          ///< The size of the border
  LoadingState                 mLoadingState;    ///< True if the data loading is completed
  void*                        mRenderingMap;    ///< NPatch rendering data

  bool mPreMultiplyOnLoad : 1; ///< Whether to multiply alpha into color channels on load
  bool mObserverNotifying : 1; ///< Whether this NPatchData notifying observers or not.
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_NPATCH_DATA_H

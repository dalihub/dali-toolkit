#ifndef DALI_TOOLKIT_NPATCH_LOADER_H
#define DALI_TOOLKIT_NPATCH_LOADER_H

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
#include <dali/integration-api/processor-interface.h>
#include <dali/public-api/rendering/texture-set.h>
#include <memory> // for std::shared_ptr
#include <string>
#include <utility> // for std::pair

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/utility/npatch-utilities.h>
#include <dali-toolkit/internal/texture-manager/texture-manager-impl.h>
#include <dali-toolkit/internal/visuals/npatch-data.h>
#include <dali-toolkit/internal/visuals/visual-url.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
/**
 * The manager for loading Npatch textures.
 * It caches them internally for better performance; i.e. to avoid loading and
 * parsing the files over and over.
 *
 * Cache is not cleaned during app lifecycle as N patches take considerably
 * small space and there's not usually a lot of them. Usually N patches are specified in
 * toolkit default style and there is 1-2 per control that are shared across the whole application.
 */
class NPatchLoader : public Integration::Processor
{
public:
  /**
   * Constructor
   */
  NPatchLoader();

  /**
   * Destructor, non-virtual as not a base class
   */
  ~NPatchLoader();

  /**
   * @brief Retrieve a texture matching the n-patch url.
   *
   * @param [in] textureManager that will be used to loading image
   * @param [in] textureObserver The NPatchVisual that requested loading.
   * @param [in] url to retrieve
   * @param [in] border The border size of the image
   * @param [in,out] preMultiplyOnLoad True if the image color should be multiplied by it's alpha. Set to false if the
   *                                   image has no alpha channel
   * @param [in] synchronousLoading True if the image will be loaded in synchronous time.
   * @return id of the texture.
   */
  NPatchData::NPatchDataId Load(TextureManager& textureManager, TextureUploadObserver* textureObserver, const VisualUrl& url, const Rect<int>& border, bool& preMultiplyOnLoad, bool synchronousLoading);

  /**
   * @brief Retrieve N patch data matching to an id
   * @param [in] id of data
   * @param [out] data const pointer to the NPatchData
   * @return true if data matching to id was really found
   */
  bool GetNPatchData(const NPatchData::NPatchDataId id, std::shared_ptr<const NPatchData>& data);

  /**
   * @brief Request remove a texture matching id.
   * Erase the observer from the observer list of cache if we need.
   *
   * @param [in] id cache data id
   * @param [in] textureObserver The NPatchVisual that requested loading.
   */
  void RequestRemove(NPatchData::NPatchDataId id, TextureUploadObserver* textureObserver);

  /**
   * @brief Increase reference count of a texture matching id.
   *
   * @param [in] id cache data id
   */
  void IncreaseReference(NPatchData::NPatchDataId id);

protected: // Implementation of Processor
  /**
   * @copydoc Dali::Integration::Processor::Process()
   */
  void Process(bool postProcessor) override;

private:
  NPatchData::NPatchDataId GenerateUniqueNPatchDataId();

  int32_t GetCacheIndexFromId(const NPatchData::NPatchDataId id);

  /**
   * @brief Remove a texture matching id.
   * Erase the observer from the observer list of cache if we need.
   * This API decrease cached NPatchInfo reference.
   * If the NPatchInfo reference become 0, the textureSet will be reset.
   *
   * @param [in] id cache data id
   * @param [in] textureObserver The NPatchVisual that requested loading.
   */
  void Remove(NPatchData::NPatchDataId id, TextureUploadObserver* textureObserver);

private:
  /**
   * @brief Information of NPatchData
   * It also hold ownership of NPatchData memory.
   */
  struct NPatchInfo
  {
    NPatchInfo(std::shared_ptr<NPatchData> data)
    : mData(data),
      mReferenceCount(1u)
    {
    }
    ~NPatchInfo()
    {
    }
    NPatchInfo(NPatchInfo&& info) noexcept // move constructor
    {
      mData                = std::move(info.mData);
      mReferenceCount      = info.mReferenceCount;
      info.mReferenceCount = 0u;
    }
    NPatchInfo& operator=(NPatchInfo&& info) noexcept // move operator
    {
      mData                = std::move(info.mData);
      mReferenceCount      = info.mReferenceCount;
      info.mReferenceCount = 0u;
      return *this;
    }

    NPatchInfo()                       = delete;            // Do not use default constructor
    NPatchInfo(const NPatchInfo& info) = delete;            // Do not use copy constructor
    NPatchInfo& operator=(const NPatchInfo& info) = delete; // Do not use copy assign

    std::shared_ptr<NPatchData> mData;
    std::int16_t                mReferenceCount; ///< The number of N-patch visuals that use this data.
  };

  /**
   * @brief Get cached NPatchData by inputed url and border. If there is no cached data, create new one.
   * @note This API increase cached NPatchInfo reference.
   *
   * @param [in] url to retrieve
   * @param [in] border The border size of the image
   * @param [in,out] preMultiplyOnLoad True if the image color should be multiplied by it's alpha. Set to false if the
   *                                   image has no alpha channel
   * @return NPatchData pointer that Load function will used.
   */
  std::shared_ptr<NPatchData> GetNPatchData(const VisualUrl& url, const Rect<int>& border, bool& preMultiplyOnLoad);

protected:
  /**
   * Undefined copy constructor.
   */
  NPatchLoader(const NPatchLoader&);

  /**
   * Undefined assignment operator.
   */
  NPatchLoader& operator=(const NPatchLoader& rhs);

private:
  NPatchData::NPatchDataId mCurrentNPatchDataId;
  std::vector<NPatchInfo>  mCache;

  std::vector<std::pair<NPatchData::NPatchDataId, TextureUploadObserver*>> mRemoveQueue; ///< Queue of textures to remove at PostProcess. It will be cleared after PostProcess.

  bool mRemoveProcessorRegistered : 1; ///< Flag if remove processor registered or not.
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_NPATCH_LOADER_H

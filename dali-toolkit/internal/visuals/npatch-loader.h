#ifndef DALI_TOOLKIT_NPATCH_LOADER_H
#define DALI_TOOLKIT_NPATCH_LOADER_H

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
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>
#include <dali/public-api/rendering/texture-set.h>
#include <string>

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
class NPatchLoader
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
  std::size_t Load(TextureManager& textureManager, TextureUploadObserver* textureObserver, const VisualUrl& url, const Rect<int>& border, bool& preMultiplyOnLoad, bool synchronousLoading);

  /**
   * @brief Set loaded PixelBuffer and its information
   *
   * @param [in] id cache data id
   * @param [in] pixelBuffer of loaded image
   * @param [in] preMultiplied True if the image had pre-multiplied alpha applied
   */
  void SetNPatchData(std::size_t id, Devel::PixelBuffer& pixelBuffer, bool preMultiplied);

  /**
   * @brief Retrieve N patch data matching to an id
   * @param [in] id of data
   * @param [out] data const pointer to the NPatchData
   * @return true if data matching to id was really found
   */
  bool GetNPatchData(const NPatchData::NPatchDataId id, const NPatchData*& data);

  /**
   * @brief Remove a texture matching id.
   * Erase the observer from the observer list of cache if we need.
   * This API decrease cached NPatchInfo reference.
   * If the NPatchInfo reference become 0, the textureSet will be reset.
   *
   * @param [in] id cache data id
   * @param [in] textureObserver The NPatchVisual that requested loading.
   */
  void Remove(std::size_t id, TextureUploadObserver* textureObserver);

private:
  NPatchData::NPatchDataId GenerateUniqueNPatchDataId();

  int32_t GetCacheIndexFromId(const NPatchData::NPatchDataId id);

private:
  /**
   * @brief Information of NPatchData
   * It also hold ownership of NPatchData memory.
   */
  struct NPatchInfo
  {
    NPatchInfo(NPatchData* data)
    : mData(data),
      mReferenceCount(1u)
    {
    }
    ~NPatchInfo()
    {
      if(mData)
      {
        delete mData;
      }
    }
    NPatchInfo(NPatchInfo&& info) // move constructor
    {
      mData                = std::move(info.mData);
      mReferenceCount      = info.mReferenceCount;
      info.mData           = nullptr;
      info.mReferenceCount = 0u;
    }
    NPatchInfo& operator=(NPatchInfo&& info) // move operator
    {
      mData                = std::move(info.mData);
      mReferenceCount      = info.mReferenceCount;
      info.mData           = nullptr;
      info.mReferenceCount = 0u;
      return *this;
    }

    NPatchInfo()                       = delete; // Do not use default constructor
    NPatchInfo(const NPatchInfo& info) = delete; // Do not use copy constructor

    NPatchData*  mData;
    std::int16_t mReferenceCount; ///< The number of N-patch visuals that use this data.
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
  NPatchData* GetNPatchData(const VisualUrl& url, const Rect<int>& border, bool& preMultiplyOnLoad);

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
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_NPATCH_LOADER_H

#ifndef DALI_TOOLKIT_NPATCH_LOADER_H
#define DALI_TOOLKIT_NPATCH_LOADER_H

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
 */

// EXTERNAL INCLUDES
#include <string>
#include <dali/public-api/rendering/texture-set.h>
#include <dali/public-api/math/uint-16-pair.h>
#include <dali/devel-api/common/owner-container.h>
#include <dali/devel-api/adaptor-framework/pixel-buffer.h>

// INTERNAL HEADERS
#include <dali-toolkit/internal/visuals/texture-manager-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace NPatchBuffer
{

void GetRedOffsetAndMask( Dali::Pixel::Format pixelFormat, int& byteOffset, int& bitMask );

} // namespace NPatchBuffer

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

  typedef Dali::Vector< Uint16Pair > StretchRanges;

  enum
  {
    UNINITIALIZED_ID = 0 ///< uninitialised id, use to initialize ids
  };

  struct Data
  {
    Data()
    : url(),
      textureSet(),
      hash( 0 ),
      croppedWidth( 0 ),
      croppedHeight( 0 ),
      border( 0, 0, 0, 0 ),
      loadCompleted( false )
    {}

    std::string url;                              ///< Url of the N-Patch
    TextureSet textureSet;                        ///< Texture containing the cropped image
    StretchRanges stretchPixelsX;                 ///< X stretch pixels
    StretchRanges stretchPixelsY;                 ///< Y stretch pixels
    std::size_t hash;                             ///< Hash code for the Url
    uint32_t croppedWidth;                        ///< Width of the cropped middle part of N-patch
    uint32_t croppedHeight;                       ///< Height of the cropped middle part of N-patch
    Rect< int > border;                           ///< The size of the border
    bool loadCompleted;                           ///< True if the data loading is completed
  };

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
  std::size_t Load( TextureManager& textureManager, TextureUploadObserver* textureObserver, const std::string& url, const Rect< int >& border, bool& preMultiplyOnLoad, bool synchronousLoading );

  /**
   * @brief Set loaded PixelBuffer and its information
   *
   * @param [in] id cache data id
   * @param [in] pixelBuffer of loaded image
   */
  void SetNPatchData( std::size_t id, Devel::PixelBuffer& pixelBuffer );

  /**
   * @brief Retrieve N patch data matching to an id
   * @param [in] id of data
   * @param [out] data const pointer to the data
   * @return true if data matching to id was really found
   */
  bool GetNPatchData( std::size_t id, const Data*& data );

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

  OwnerContainer< Data* > mCache;

};

} // name Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_NPATCH_LOADER_H

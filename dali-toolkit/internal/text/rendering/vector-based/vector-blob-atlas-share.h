#ifndef __DALI_TOOLKIT_VECTOR_BLOB_ATLAS_SHARE_H__
#define __DALI_TOOLKIT_VECTOR_BLOB_ATLAS_SHARE_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/rendering/vector-based/vector-blob-atlas.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

/**
 * @brief A singleton for sharing atlases containing vector data
 *
 */
class VectorBlobAtlasShare : public BaseHandle
{
public:

  /**
   * @brief Create a VectorBlobAtlasShare handle.
   *
   * Calling member functions with an uninitialised handle is not allowed.
   */
  VectorBlobAtlasShare();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~VectorBlobAtlasShare();

  /**
   * @brief Create or retrieve VectorBlobAtlasShare singleton.
   *
   * @return A handle to the VectorBlobAtlasShare control.
   */
  static VectorBlobAtlasShare Get();

  /**
   * @brief Retrieve the current (empty or partially empty) atlas.
   *
   * @return The current atlas.
   */
  VectorBlobAtlas* GetCurrentAtlas();

  /**
   * @brief Retrieve a new empty atlas.
   *
   * @pre The current atlas should be full.
   * @return A new atlas.
   */
  VectorBlobAtlas* GetNewAtlas();

private:

  class Impl;

  explicit DALI_INTERNAL VectorBlobAtlasShare( VectorBlobAtlasShare::Impl* impl );

};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_VECTOR_BLOB_ATLAS_SHARE_H__

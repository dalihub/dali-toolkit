#ifndef DALI_SCENE_LOADER_CUBE_DATA_H
#define DALI_SCENE_LOADER_CUBE_DATA_H
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
 *
 */

// INTERNAL INCLUDES
#include "dali-scene-loader/public-api/api.h"

// EXTERNAL INCLUDES
#include "dali/public-api/common/vector-wrapper.h"
#include "dali/public-api/images/pixel-data.h"
#include "dali/public-api/rendering/texture.h"

namespace Dali
{
namespace SceneLoader
{
/**
 * @brief Stores the pixel data objects for each face of the cube texture and their mipmaps.
 */
class DALI_SCENE_LOADER_API CubeData
{
public:
  /**
   * @brief Create cube texture from image file
   * @return Texture loaded cube texture.
   */
  Texture CreateTexture() const;

public:
  std::vector<std::vector<PixelData> > data;
};

} // namespace SceneLoader
} // namespace Dali

#endif // DALI_SCENE_LOADER_CUBE_DATA_H

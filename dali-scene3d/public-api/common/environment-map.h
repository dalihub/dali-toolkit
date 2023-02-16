#ifndef DALI_SCENE3D_COMMON_ENVIRONMENT_MAP_H
#define DALI_SCENE3D_COMMON_ENVIRONMENT_MAP_H
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
 *
 */

namespace Dali
{
namespace Scene3D
{

/**
 * @brief The Environment Map types
 * @SINCE_2_2.11
 */
enum class EnvironmentMapType
{
  AUTO,           ///< Automatically detects the type of input environment map.
                  ///  DALi finds the closest aspect ratio of the input texture to guess the environment map type.
  CUBEMAP,        ///< Environment map in cubemap
                  ///  DALi supports cross and array type of cube map for the vertical and horizontal direction.
                  ///  For example, if the aspect ratio of a texture is 4x3, the texture is classified as a horizontal cross type.
                  ///  On the other hand, if the aspect ratio is 6x1, it can be sorted as a horizontal array.
  EQUIRECTANGULAR ///< Environment map in equirectangular projection. Usually equirectangular image has 2x1 aspect ratio.
};

} // namespace Scene3D
} // namespace Dali

#endif // DALI_SCENE3D_COMMON_ENVIRONMENT_MAP_H

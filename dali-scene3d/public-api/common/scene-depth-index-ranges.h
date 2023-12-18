#ifndef DALI_SCENE3D_SCENE_DEPTH_INDEX_RANGES_H
#define DALI_SCENE3D_SCENE_DEPTH_INDEX_RANGES_H

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
namespace DepthIndex
{

/**
 * @brief Depth index ranges to define rendering order.
 * @SINCE_2_3.3
 */
enum Ranges
{
  /**
   * @brief Depth index range for 3D scene content.
   * @details The range of the scene content is between [SCENE, SCENE + 999]
   * @SINCE_2_3.3
   */
  SCENE = -2000,

  /**
   * @brief Depth index range for UI scene content.
   * @details The range of the UI content is between [UI, UI + 999].
   * Some of internally created Renderer of Toolkit::Control already has
   * default depth index value.
   * Developer can fix the default values for their design.
   * @SINCE_2_3.3
   */
  UI = 0
};

} // namespace DepthIndex

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_SCENE_DEPTH_INDEX_RANGES_H

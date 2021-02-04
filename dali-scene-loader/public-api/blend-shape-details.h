#ifndef DALI_SCENE_LOADER_BLEND_SHAPE_DETAILS_H
#define DALI_SCENE_LOADER_BLEND_SHAPE_DETAILS_H
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
 *
 */

// INTERNAL INCLUDES
#include "dali-scene-loader/public-api/api.h"

// EXTERNAL INCLUDES
#include <string>
#include "dali/public-api/actors/actor.h"
#include "dali/public-api/rendering/shader.h"

namespace Dali
{
namespace SceneLoader
{
struct MeshDefinition;
struct MeshGeometry;

struct DALI_SCENE_LOADER_API BlendShapes
{
  enum class Version
  {
    VERSION_1_0,
    VERSION_2_0,
    INVALID
  };

  struct Component
  {
    enum
    {
      POSITIONS = 0x1,
      NORMALS   = 0x2,
      TANGENTS  = 0x4
    };
  };

  // shader properties - animatable (uniforms)
  static const std::string NUMBER_OF_BLEND_SHAPES; ///< Integer number of blend shapes loaded.
  static const std::string UNNORMALIZE_FACTOR;     ///< Scalar(s) for position components of blend shapes; Version 1.0: float array (1 per blend shape); Version 2.0: single float.
  static const std::string COMPONENT_SIZE;         ///< Integer offset from one component (positions / normals / tangents) of a blend shape to the next.

  // shader properties - read-only (not available as uniforms)
  static const std::string COMPONENTS; ///< Integer bitmask of the blend shape components that the shader uses; refer to the Components enum.

  // actor property (instance) - animatable (uniforms)
  static const std::string WEIGHTS_UNIFORM; ///< The weight of each blend shape in a float array

  /**
   * @brief Registers properties based on the mesh definition (and geometry) and identified by the above string constants,
   *  on the given @a shader and @a actor.
   */
  static void ConfigureProperties(const std::pair<MeshDefinition, MeshGeometry>& mesh, Shader shader, Actor actor);

  BlendShapes() = delete;
};

} // namespace SceneLoader
} // namespace Dali

#endif // DALI_SCENE_LOADER_BLEND_SHAPE_DETAILS_H

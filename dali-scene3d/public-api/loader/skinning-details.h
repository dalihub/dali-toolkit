#ifndef DALI_SCENE3D_LOADER_SKINNING_DETAILS_H_
#define DALI_SCENE3D_LOADER_SKINNING_DETAILS_H_
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

// EXTERNAL INCLUDES
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/rendering/shader.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>
#include <dali-scene3d/public-api/loader/index.h>
#include <dali-scene3d/public-api/model-components/model-primitive.h>

namespace Dali::Scene3D::Loader
{
struct DALI_SCENE3D_API Skinning
{
public:
  struct BoneData
  {
    Dali::Scene3D::ModelPrimitive primitive;
    Dali::Constraint              constraint;
    Scene3D::Loader::Index        boneIndex;
    std::string                   propertyName;
    Matrix                        inverseMatrix;
  };

  /**
   * @brief Upper limit on the number of joints supported.
   * @SINCE_2_0.7
   */
  static const uint32_t MAX_JOINTS;

  /**
   * @brief Name of bone matrix uniform (array).
   * @SINCE_2_0.7
   */
  static const char* BONE_UNIFORM_NAME;

  Skinning() = delete;
};

} // namespace Dali::Scene3D::Loader

#endif // DALI_SCENE3D_LOADER_SKINNING_DETAILS_H_

#ifndef DALI_SCENE3D_LOADERER_SKELETON_H
#define DALI_SCENE3D_LOADERER_SKELETON_H
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

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/loader/index.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/math/matrix.h>

namespace Dali::Scene3D::Loader
{
/*
 * @brief A set of joints (stored as node indices), and an optional root node index.
 * @note The list of joints must not be empty and must not contain INVALID_INDEX.
 * @note Client code should not change the order of joints, as they are indexed by mesh vertex data.
 * @note Client code should make no assumption about the relation of the joint IDs.
 */
struct DALI_SCENE3D_API SkeletonDefinition
{
  struct Joint
  {
    Index  mNodeIdx;
    Matrix mInverseBindMatrix;
  };

  using Vector = std::vector<SkeletonDefinition>;

  Index              mRootNodeIdx = INVALID_INDEX;
  std::vector<Joint> mJoints;
};

} // namespace Dali::Scene3D::Loader

#endif //DALI_SCENE3D_LOADERER_SKELETON_H

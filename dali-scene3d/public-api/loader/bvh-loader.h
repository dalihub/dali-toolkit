#ifndef DALI_SCENE3D_LOADER_BVH_LOADER_H
#define DALI_SCENE3D_LOADER_BVH_LOADER_H

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
#include <dali-scene3d/public-api/loader/animation-definition.h>

namespace Dali::Scene3D::Loader
{
/**
 * @brief Loads motion capture data from bvh file format.
 *
 * @SINCE_2_1.32
 * @param[in] path The file path.
 * @param[in] animationName Name of the motion capture animation
 * @param[in] useRootTranslationOnly True to use only root translation with rotation animation.
 * @param[in] scale The scale factor to set on the position property manually.
 * @return AnimationDefinition that includes joint animation information.
 */
DALI_SCENE3D_API AnimationDefinition LoadBvh(const std::string& path, const std::string& animationName, bool useRootTranslationOnly, const Vector3& scale = Vector3::ONE);

/**
 * @brief Loads motion capture data from bvh data stream.
 *
 * @SINCE_2_2.23
 * @param[in] rawBuffer The bvh buffer.
 * @param[in] rawBufferLength The length of buffer.
 * @param[in] animationName Name of the motion capture animation
 * @param[in] useRootTranslationOnly True to use only root translation with rotation animation.
 * @param[in] scale The scale factor to set on the position property manually.
 * @return AnimationDefinition that includes joint animation information.
 */
DALI_SCENE3D_API AnimationDefinition LoadBvhFromBuffer(const uint8_t* rawBuffer, int rawBufferLength, const std::string& animationName, bool useRootTranslationOnly, const Vector3& scale = Vector3::ONE);

} // namespace Dali::Scene3D::Loader

#endif // DALI_SCENE3D_LOADER_BVH_LOADER_H
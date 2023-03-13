#ifndef DALI_SCENE3D_LOADER_NAVIGATION_MESH_FACTORY_H
#define DALI_SCENE3D_LOADER_NAVIGATION_MESH_FACTORY_H

/*
* Copyright (c) 2023 Samsung Electronics Co., Ltd.

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
#include <dali/public-api/rendering/geometry.h>
#include <dali/public-api/rendering/texture.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/algorithm/navigation-mesh.h>
#include <dali-scene3d/public-api/api.h>

namespace Dali::Scene3D::Loader
{
struct DALI_SCENE3D_API NavigationMeshFactory
{
public:
  /**
   * @brief Creates NavigationMesh object from file
   *
   * @param[in] filename file to load
   * @return Valid NavigationMesh or nullptr
   */
  static std::unique_ptr<Algorithm::NavigationMesh> CreateFromFile(std::string filename);

  /**
   * @brief Creates NavigationMesh object from binary buffer
   *
   * @param[in] buffer buffer with data
   * @return Valid NavigationMesh or nullptr
   */
  static std::unique_ptr<Algorithm::NavigationMesh> CreateFromBuffer(const std::vector<uint8_t>& buffer);
};
} // namespace Dali::Scene3D::Loader

#endif // DALI_SCENE3D_INTERNAL_LOADER_NAVIGATION_MESH_FACTORY_H

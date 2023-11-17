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
/**
 * @brief Factory to create a NavigationMesh from various sources.
 * @SINCE_2_2.12
 */
struct DALI_SCENE3D_API NavigationMeshFactory
{
public:
  /**
   * @brief Creates NavigationMesh object from file
   *
   * @SINCE_2_2.12
   * @param[in] filename file to load
   * @return Valid NavigationMesh or nullptr
   */
  static std::unique_ptr<Algorithm::NavigationMesh> CreateFromFile(std::string filename);

  /**
   * @brief Creates NavigationMesh object from binary buffer
   *
   * @SINCE_2_2.12
   * @param[in] buffer buffer with data
   * @return Valid NavigationMesh or nullptr
   */
  static std::unique_ptr<Algorithm::NavigationMesh> CreateFromBuffer(const std::vector<uint8_t>& buffer);

  /**
   * @brief Creates new mesh from lists of vertices and faces
   *
   * List of faces contains indices into the vertex list
   *
   * @SINCE_2_2.53
   * @param[in] vertices List of Vector3 vertices
   * @param[in] vertexNormals List of Vector3 vertices
   * @param[in] faceIndices List of faces
   * @return Valid NavigationMesh or nullptr
   */
  static std::unique_ptr<Algorithm::NavigationMesh> CreateFromVertexFaceList(const std::vector<Vector3>& vertices, const std::vector<Vector3>& vertexNormals, const std::vector<uint32_t>& faceIndices);

  /**
   * @brief Creates new mesh from lists of vertices and faces
   *
   * List of faces contains indices into the vertex list
   *
   * This function reduces number of array copys when called from NUI.
   *
   * @SINCE_2_2.53
   * @param[in] vertices Pointer to C-style array of vertices
   * @param[in] vertexCount Number of vertices
   * @param[in] vertexNormals to C-style array of vertex normals
   * @param[in] faceIndices Pointer to C-style array of face elements indices
   * @param[in] indexCount Number of indices
   * @return Valid NavigationMesh or nullptr
   */
  static std::unique_ptr<Algorithm::NavigationMesh> CreateFromVertexFaceList(const Vector3* vertices, const Vector3* vertexNormals, uint32_t vertexCount, const uint32_t* faceIndices, uint32_t indexCount);

  /**
   * @brief Serializes mesh data to the binary format.
   *
   * The binary data returned by the function can be used
   * as an input for NavigationMeshFactory::CreateFromBuffer()
   *
   * @SINCE_2_2.53
   * @param[in] navigationMesh Navigation mesh to serialize
   * @return Buffer containing serialized mesh data
   */
  static std::vector<uint8_t> GetMeshBinary(const Dali::Scene3D::Algorithm::NavigationMesh& navigationMesh);
};
} // namespace Dali::Scene3D::Loader

#endif // DALI_SCENE3D_INTERNAL_LOADER_NAVIGATION_MESH_FACTORY_H

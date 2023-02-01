#ifndef DALI_SCENE3D_NAVIGATION_MESH_HEADER_H
#define DALI_SCENE3D_NAVIGATION_MESH_HEADER_H
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
 */

// EXTERNAL INCLUDES
#include <inttypes.h>

namespace Dali::Scene3D::Internal::Algorithm
{
/**
 * @struct NavigationMeshHeader
 *
 * Base header structure contains only the version field. It will allow adding changes to the
 * exporter while maintaining backward compatibility.
 */
struct NavigationMeshHeader
{
  uint32_t checksum; ///< Checksum (used to test for endianness, tested by reader )
  uint32_t version;  ///< Version of the API
};

/**
 * @struct NavigationMeshHeader_V10
 *
 * Extension of header for version 1.0 of NavigationMesh binary file
 */
struct NavigationMeshHeader_V10 : public NavigationMeshHeader
{
  uint32_t dataOffset; ///< Offset where data starts (depends on endianness)

  uint32_t vertexCount;      ///< total count of vertices
  uint32_t vertexDataOffset; ///< offset into data array where vertices start

  uint32_t edgeCount;      ///< total count of edges
  uint32_t edgeDataOffset; ///< offset into data array where edges are stored

  uint32_t polyCount;      ///< total number of polys
  uint32_t polyDataOffset; ///< offset into data array where edges are stored

  float gravityVector[3]; /// Gravity vector for the data (down vector)
};

}
#endif // DALI_SCENE3D_NAVIGATION_MESH_HEADER_H

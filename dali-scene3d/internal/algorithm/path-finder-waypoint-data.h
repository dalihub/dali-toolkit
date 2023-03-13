#ifndef DALI_SCENE3D_INTERNAL_PATH_FINDER_WAYPOINT_DATA_H
#define DALI_SCENE3D_INTERNAL_PATH_FINDER_WAYPOINT_DATA_H

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

// INTERNAL INCLUDES
#include <dali-scene3d/internal/algorithm/navigation-mesh-impl.h>
#include <dali-scene3d/public-api/algorithm/path-finder-waypoint.h>

// EXTERNAL INCLUDES
#include <cinttypes>

namespace Dali::Scene3D::Internal::Algorithm
{
class NavigationMesh;

/**
 * Structure containing data describing single waypoint
 *
 * The structure stores:
 * - node of NavigationMesh within which the waypoint is placed
 * - a point within NavigationMesh face space (2D)
 * - a point within NavigationMesh parent transform space (3D)
 * Additionally, it stores data used by algorithm:
 * - face - pointer to the face of NavigationMesh
 * - edge - Edge between this waypoint and next on the path
 */
struct WayPointData
{
  uint32_t                    nodeIndex; ///< Index of the node/face
  const NavigationMesh::Face* face;      ///< Polygon containing waypoint
  Dali::Vector2               point2d;   ///< waypoint point in the polygon space, origin is a polygon centre
  Dali::Vector3               point3d;   ///< point in the 3D space in the navmesh parent space

  // internal data needed for processing
  const NavigationMesh::Edge* edge; ///< Edge between this face and next face
};
} // namespace Dali::Scene3D::Internal::Algorithm

#endif // DALI_SCENE3D_INTERNAL_PATH_FINDER_WAYPOINT_DATA_H

#ifndef DALI_SCENE3D_INTERNAL_PATH_FINDER_SPFA_H
#define DALI_SCENE3D_INTERNAL_PATH_FINDER_SPFA_H

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
#include <dali-scene3d/public-api/algorithm/path-finder.h>

namespace Dali::Scene3D::Internal::Algorithm
{
class PathFinderAlgorithmSPFA : public Dali::Scene3D::Algorithm::PathFinderBase
{
public:
  /**
   * @brief Constructor
   *
   * @param[in] navMesh Navigation mesh to associate with the algorithm
   */
  explicit PathFinderAlgorithmSPFA(Dali::Scene3D::Algorithm::NavigationMesh& navMesh);

  /**
   * @brief Destructor
   */
  ~PathFinderAlgorithmSPFA() override;

  /**
   * @brief Looks for a path from point A to point B.
   *
   * @param[in] positionFrom source position in NavigationMesh parent space
   * @param[in] positionTo target position in NavigationMesh parent space
   * @return List of waypoints for path
   */
  Scene3D::Algorithm::WayPointList FindPath(const Dali::Vector3& PositionFrom, const Dali::Vector3& PositionTo) override;

  /**
   * @brief Finds path between NavigationMesh faces
   *
   * @param[in] polyIndexFrom Index of start polygon
   * @param[in] polyIndexTo Index of end polygon
   * @return List of waypoints for path
   */
  Scene3D::Algorithm::WayPointList FindPath(uint32_t sourcePolyIndex, uint32_t targetPolyIndex) override;

  [[nodiscard]] inline const NavigationMesh::Face* Face(uint32_t i) const
  {
    return mNavigationMesh->GetFace(i);
  }

  /**
   * Build the graph of nodes
   * distance between nodes is weight of node
   */
  void PrepareData();

  Scene3D::Algorithm::WayPointList OptimizeWaypoints(Scene3D::Algorithm::WayPointList& waypoints) const;

  /**
   * Structure describes single node of pathfinding algorithm
   */
  struct FaceNode
  {
    uint32_t faceIndex; ///< Index of face which is associated with node

    // neighbours
    uint32_t faces[3];  ///< List of neighbouring faces (max 3 for a triangle)
    uint32_t edges[3];  ///< List of edges (max 3 for a triangle)
    float    weight[3]; ///< List of weights (by distance) to each neighbour
  };

  NavigationMesh*       mNavigationMesh; ///< Pointer to a valid NavigationMesh
  std::vector<FaceNode> mNodes;          ///< List of nodes
};
} // namespace Dali::Scene3D::Internal::Algorithm
#endif // DALI_SCENE3D_INTERNAL_PATH_FINDER_SPFA_H

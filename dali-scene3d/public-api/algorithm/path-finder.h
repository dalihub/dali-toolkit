#ifndef DALI_SCENE3D_PATH_FINDER_H
#define DALI_SCENE3D_PATH_FINDER_H

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

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/algorithm/navigation-mesh.h>
#include <dali-scene3d/public-api/algorithm/path-finder-waypoint.h>
#include <dali-scene3d/public-api/api.h>

namespace Dali::Scene3D::Algorithm
{
using WayPointList = std::vector<Scene3D::Algorithm::WayPoint>;

/**
 * List of enums to be used when not using custom implementation
 * of path finding.
 */
enum class PathFinderAlgorithm
{
  DIJKSTRA_SHORTEST_PATH, ///< Using A* variant (Dijkstra) finding a shortest path
  SPFA,                   ///< Using SPFA-SLF (Shortest Path Fast Algorithm with Short Label First) finding a shortest path.
  SPFA_DOUBLE_WAY,        ///< Using SPFA-SLF double way. It might not find shortest, but will use less memory.

  DEFAULT = DIJKSTRA_SHORTEST_PATH, ///< Default algorithm to use
};

/**
 * @class PathFinderBase
 *
 * Base class for implementation of pathfinding algorithms.
 */
class DALI_SCENE3D_API PathFinderBase
{
public:
  /**
   * @brief Destructor
   */
  virtual ~PathFinderBase() = default;

  /**
   * @brief Looks for a path from point A to point B.
   *
   * @param[in] positionFrom source position in NavigationMesh parent space
   * @param[in] positionTo target position in NavigationMesh parent space
   * @return List of waypoints for path or empty vector if no success
   */
  virtual WayPointList FindPath(const Dali::Vector3& positionFrom, const Dali::Vector3& positionTo) = 0;

  /**
   * @brief Finds path between NavigationMesh faces
   *
   * @param[in] polyIndexFrom Index of start polygon
   * @param[in] polyIndexTo Index of end polygon
   * @return List of waypoints for path or empty vector if no success
   */
  virtual WayPointList FindPath(FaceIndex polyIndexFrom, FaceIndex polyIndexTo) = 0;
};

/**
 * @class PathFinder
 *
 * PathFinder runs path finding algorithm on associated NavigationMesh
 * and returns a list of waypoints.
 */
class DALI_SCENE3D_API PathFinder
{
public:
  /**
   * @brief Creates new instance of path finder
   * @param[in] navigationMesh Navigation mesh to associate with
   * @param[in] algorithm algorithm to use
   * @return Valid pointer to PathFinder object or nullptr
   */
  static std::unique_ptr<PathFinder> New(NavigationMesh& navigationMesh, PathFinderAlgorithm algorithm);

  /**
   * @brief Looks for a path from point A to point B.
   *
   * The function looks for the path between point A (positionFrom) and B (positionTo). It runs
   * the algorithm on the associated NavigationMesh and automatically looks for the floor point.
   *
   * It will fail if:
   * - Any point is outside the navigation mesh
   * - The path doesn't exist
   *
   * Both points should be defined in the same space as is used by the NavigationMesh.
   *
   * @param[in] positionFrom Source position
   * @param[in] positionTo Target position
   * @return List of waypoints for path or empty list on failure
   */
  WayPointList FindPath(const Dali::Vector3& positionFrom, const Dali::Vector3& positionTo);

  /**
   * @brief Looks for a path between specified NavigationMesh faces
   *
   * The function looks for the path between given faces (provided as indices).
   *
   * It will fail if:
   * - index < 0 or index > NavigationMesh::GetFaceCount()
   * - The path doesn't exist
   *
   * @param[in] faceIndexFrom Source face index
   * @param[in] faceIndexTo Target face index
   * @return List of waypoints for path or empty list on failure
   */
  WayPointList FindPath(FaceIndex faceIndexFrom, FaceIndex faceIndexTo);

private:
  PathFinder() = delete;

  DALI_INTERNAL explicit PathFinder(std::unique_ptr<PathFinderBase>&& baseImpl);

  std::unique_ptr<PathFinderBase> mImpl;
};

} // namespace Dali::Scene3D::Algorithm

#endif // DALI_SCENE3D_PATH_FINDER_H

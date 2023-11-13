#ifndef DALI_SCENE3D_NAVIGATION_MESH_H
#define DALI_SCENE3D_NAVIGATION_MESH_H

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
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>

#include <cinttypes>
#include <cstdio>
#include <limits>
#include <memory>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>

namespace Dali::Scene3D::Internal::Algorithm
{
class NavigationMesh;
}

namespace Dali::Scene3D::Loader
{
class NavigationMeshFactory;
}

constexpr auto NAVIGATION_MESH_MAX_VERTICES_PER_FACE = 3u;
constexpr auto NAVIGATION_MESH_MAX_EDGES_PER_FACE    = 3u;
constexpr auto NAVIGATION_MESH_MAX_COMPONENTS_3D     = 3u;
constexpr auto NAVIGATION_MESH_MAX_COMPONENTS_2D     = 2u;

namespace Dali::Scene3D::Algorithm
{
// Using PImpling but not usual DALi handles as this object isn't supposed to be refcounted
using NavigationMeshImpl = Dali::Scene3D::Internal::Algorithm::NavigationMesh;

// Make each to change each index value's type here.
using VertexIndex = uint16_t;
using EdgeIndex   = uint16_t;
using FaceIndex   = uint16_t;

/**
 * @class NavigationMesh
 *
 * NavigationMesh is a set of connected faces. The data contains
 * Polygons (Polys), Edges and Vertices and describes relations
 * between (for example, edge knows which polys are on each side).
 *
 * NavigationMesh uses any coordinate system that it has been exported with.
 *
 * The mesh is exported with gravity direction. This is because various editors
 * may define UP vector differently. Note, the Gravity vector points DOWN.
 *
 * - All calculation take place in the navigation mesh local space
 * - The NavigationMesh should use a correct transformation matrix (SetSceneTransform())
 * - Without transform, the NavigationMesh space stays local (compatible with exporter tool)
 * - The NavigationMesh defines Gravity vector (down)
 * - The finding floor results are returned back into the scene space (set with SetSceneTransform()).
 *
 */
class DALI_SCENE3D_API NavigationMesh
{
public:
  /**
   * @struct Face
   *
   * Describes a single polygon's face
   */
  struct Face
  {
    VertexIndex vertex[NAVIGATION_MESH_MAX_VERTICES_PER_FACE]; ///< Vertices per face
    EdgeIndex   edge[NAVIGATION_MESH_MAX_EDGES_PER_FACE];      ///< Edges per face
    float       normal[NAVIGATION_MESH_MAX_COMPONENTS_3D];     ///< Normal vector
    float       center[NAVIGATION_MESH_MAX_COMPONENTS_3D];     ///< Barycentric coordinates
  };

  /**
   * @struct Edge
   *
   * Describes a single edge
   */
  struct Edge
  {
    VertexIndex vertex[NAVIGATION_MESH_MAX_COMPONENTS_2D]; ///< Vertices making the edge
    FaceIndex   face[NAVIGATION_MESH_MAX_COMPONENTS_2D];   ///< Faces on both sides of edge
  };

  /**
   * @struct Vertex
   *
   * Describes a single Vertex
   *
   */
  struct Vertex
  {
    union
    {
      float coordinates[NAVIGATION_MESH_MAX_COMPONENTS_3D]; ///< Coordinates of vertex
      struct
      {
        float x, y, z;
      };
    };
  };

  NavigationMesh() = delete;

public:
  /**
   * @brief Destructor
   */
  ~NavigationMesh();

  /**
   * @brief Returns total number of faces
   *
   * @return number of faces
   */
  [[nodiscard]] uint32_t GetFaceCount() const;

  /**
   * @brief Returns total number of edges
   *
   * @return number of edges
   */
  [[nodiscard]] uint32_t GetEdgeCount() const;

  /**
   * @brief Returns total number of vertices
   *
   * @return number of vertices
   */
  [[nodiscard]] uint32_t GetVertexCount() const;

  /**
   * @brief Looks for the floor under specified position
   * @param[in] position Position to investigate
   * @param[in] outPosition Position on the floor in found
   * @param[out] faceIndex Index of NavigationMesh face associated with floor
   *
   * @return True if floor has been found, False otherwise
   */
  bool FindFloor(const Dali::Vector3& position, Dali::Vector3& outPosition, FaceIndex& faceIndex);

  /**
   * @brief Looks for a floor starting from specified face
   *
   * The function performs lookup starting from the specified face. If 'dontCheckNeighbours' is 'true'
   * then function will fail if 'position' falls outside boundries of face. If 'dontCheckNeighbours'
   * is 'false' the function will continue search expanding onto neighbouring faces.
   *
   * @param[in] position Position to investigate
   * @param[in] faceIndex Face index to start lookup
   * @param[in] dontCheckNeighbours If true, the neighbouring faces won't be tested
   * @param[out] outPosition Result of lookup
   *
   * @return True on success, false otherwise
   */
  bool FindFloorForFace(const Dali::Vector3& position, FaceIndex faceIndex, bool dontCheckNeighbours, Dali::Vector3& outPosition);

  /**
   * @brief Returns pointer to Face structure
   * @param[in] index Index of face to retrieve
   * @return Pointer to valid Face structure or nullptr
   */
  [[nodiscard]] const Face* GetFace(FaceIndex index) const;

  /**
   * @brief Returns edge structure
   * @param[in] index Index of edge to retrieve
   * @return Pointer to valid Edge structure or nullptr
   */
  [[nodiscard]] const Edge* GetEdge(EdgeIndex index) const;

  /**
   * @brief Returns vertex structure
   * @param[in] index Index of vertex to retrieve
   * @return Pointer to valid Vertex structure or nullptr
   */
  [[nodiscard]] const Vertex* GetVertex(VertexIndex index) const;

  /**
   * @brief Sets static transform for the navigation mesh object
   *
   * The NavigationMesh may require to be transformed into the coordinates
   * of the scene object. The exporter exports navigation geometry in a local
   * space. The transform must be set in order to use the navigation mesh
   * in the scene space (most likely DALi coordinate space).
   *
   * The scene transform matrix can be set in the DALi event thread using
   * Dali::DevelActor::GetWorldTransform(sceneActor)
   *
   * For example:
   * @code
   * Actor parentActorOfNavigationMesh; // non-null object
   * Dali::DevelActor::GetWorldTransform(parentActorOfNavigationMesh);
   * navigationMesh->SetSceneTransform(parentActorOfNavigationMesh);
   * @endcode
   *
   * The transform remains static until changed by calling SetSceneTransform() again.
   * It means that if the matrix is obtained from the actor and actor transform will
   * change the navigation mesh won't be aligned anymore.
   *
   * @param[in] transform Valid transform 4x4 matrix
   */
  void SetSceneTransform(const Dali::Matrix& transform);

  /**
   * @brief transforms point into the NavigationMesh local space
   *
   * Transforms a 3D point into navigation mesh space (space used when
   * NavigationMesh has been created, most likely 3D editor space).
   *
   * @param[in] point Point to transform
   * @return Point transformed to the local space
   */
  Dali::Vector3 PointSceneToLocal(const Dali::Vector3& point) const;

  /**
   * @brief Transforms point into the parent transform space
   *
   * Transforms the given point into the parent space (set with SetSceneTransform()).
   *
   * @param[in] point Point to transform
   * @return Point transformed into the parent space
   */
  Dali::Vector3 PointLocalToScene(const Dali::Vector3& point) const;

  /**
   * @brief Returns direction of the gravity vector
   *
   * Gravity vector points down.
   *
   * @return Gravity vector 3D
   */
  Dali::Vector3 GetGravityVector() const;

  /**
   * @brief Performs ray/face intersect test
   * @param[in] origin Origin of ray
   * @param[in] direction Direction of ray
   *
   * @SINCE_2_3.0
   * @return Valid FaceIndex on hit or NULL_FACE on miss
   */
  [[nodiscard]] FaceIndex RayFaceIntersect(const Vector3& origin, const Vector3& direction) const;

  static constexpr FaceIndex NULL_FACE{std::numeric_limits<FaceIndex>::max()}; ///< Represents null face
  static constexpr EdgeIndex NULL_EDGE{std::numeric_limits<EdgeIndex>::max()}; ///< Represents null edge

public:
  DALI_INTERNAL explicit NavigationMesh(NavigationMeshImpl* impl);

  std::unique_ptr<NavigationMeshImpl> mImpl;
};

// Alias name for collider mesh
// TODO: currently ColliderMesh is NavigationMesh however
//       there should be separation from data and algorithms.
//       Both, NavigationMesh and ColliderMesh use the same
//       data structures but differ in the way they use data.
using ColliderMesh = NavigationMesh;

} // namespace Dali::Scene3D::Algorithm
#endif // DALI_SCENE3D_NAVIGATION_MESH_H

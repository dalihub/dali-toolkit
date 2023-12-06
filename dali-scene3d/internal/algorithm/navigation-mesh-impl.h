#ifndef DALI_SCENE3D_INTERNAL_NAVIGATION_MESH_H
#define DALI_SCENE3D_INTERNAL_NAVIGATION_MESH_H

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

// EXTERNAL EXTERNAL
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>

#include <cinttypes>
#include <cstdio>
#include <mutex>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/algorithm/navigation-mesh-header.h>
#include <dali-scene3d/public-api/algorithm/navigation-mesh.h>

namespace Dali::Scene3D::Loader
{
class NavigationMeshFactory;
}

namespace Dali::Scene3D::Internal::Algorithm
{
// Internal Navigation ray structure
struct NavigationRay
{
  Dali::Vector3 origin;    // Origin of ray
  Dali::Vector3 direction; // Direction of ray
};

// Make each to change each index value's type here.
using VertexIndex = Dali::Scene3D::Algorithm::VertexIndex;
using EdgeIndex   = Dali::Scene3D::Algorithm::EdgeIndex;
using FaceIndex   = Dali::Scene3D::Algorithm::FaceIndex;

/**
 * @class NavigationMesh
 */
class NavigationMesh
{
public:
  using Face   = Dali::Scene3D::Algorithm::NavigationMesh::Face;
  using Edge   = Dali::Scene3D::Algorithm::NavigationMesh::Edge;
  using Vertex = Dali::Scene3D::Algorithm::NavigationMesh::Vertex;

private:
  friend class Scene3D::Loader::NavigationMeshFactory;

  /**
   * Constructor
   */
  NavigationMesh(const std::vector<uint8_t>& buffer);

public:
  /**
   * Destructor
   */
  ~NavigationMesh() = default;

  /**
   * Result of Ray/Polygon intersection
   */
  struct IntersectResult
  {
    Dali::Vector3 point;
    float         distance;
    FaceIndex     faceIndex;
    bool          result;
  };

  /**
   * @copydoc Dali::Scene3D::Algorithm::NavigationMesh::GetFaceCount()
   */
  [[nodiscard]] uint32_t GetFaceCount() const;

  /**
   * @copydoc Dali::Scene3D::Algorithm::NavigationMesh::GetEdgeCount()
   */
  [[nodiscard]] uint32_t GetEdgeCount() const;

  /**
   * @copydoc Dali::Scene3D::Algorithm::NavigationMesh::GetVertexCount()
   */
  [[nodiscard]] uint32_t GetVertexCount() const;

  /**
   * @copydoc Dali::Scene3D::Algorithm::NavigationMesh::FindFloorForFace()
   */
  bool FindFloorForFace(const Dali::Vector3& position, FaceIndex faceIndex, bool dontCheckNeighbours, Dali::Vector3& outPosition);

  /**
   * @copydoc Dali::Scene3D::Algorithm::NavigationMesh::FindFloor()
   */
  bool FindFloor(const Dali::Vector3& position, Dali::Vector3& outPosition);

  /**
   * @copydoc Dali::Scene3D::Algorithm::NavigationMesh::FindFloor()
   */
  bool FindFloor(const Dali::Vector3& position, Dali::Vector3& outPosition, FaceIndex& faceIndex);

  /**
   * @copydoc Dali::Scene3D::Algorithm::NavigationMesh::GetFace()
   */
  [[nodiscard]] const Face* GetFace(FaceIndex index) const;

  /**
   * @copydoc Dali::Scene3D::Algorithm::NavigationMesh::GetEdge()
   */
  [[nodiscard]] const Edge* GetEdge(EdgeIndex index) const;

  /**
   * @copydoc Dali::Scene3D::Algorithm::NavigationMesh::GetVertex()
   */
  [[nodiscard]] const Vertex* GetVertex(VertexIndex index) const;

  /**
   * @copydoc Dali::Scene3D::Algorithm::NavigationMesh::SetSceneTransform()
   */
  void SetTransform(const Dali::Matrix& transform);

  /**
   * Tests intersection between navigation ray and face
   */
  IntersectResult NavigationRayFaceIntersection(NavigationRay& ray, const Face& face) const;

  /**
   * @brief Test ray against the mesh and returns intersection result
   * @param[in] rayOrig Input ray to test collision
   *
   * @return Valid IntersectResult structure
   */
  IntersectResult RayCastIntersect(NavigationRay& rayOrig) const;

  /**
   * @copydoc Dali::Scene3D::Algorithm::NavigationMesh::PointSceneToLocal()
   */
  Dali::Vector3 PointSceneToLocal(const Dali::Vector3& point) const;

  /**
   * @copydoc Dali::Scene3D::Algorithm::NavigationMesh::PointLocalToScene()
   */
  Dali::Vector3 PointLocalToScene(const Dali::Vector3& point) const;

  /**
   * @copydoc Dali::Scene3D::Algorithm::NavigationMesh::GetGravityVector()
   */
  [[nodiscard]] Dali::Vector3 GetGravityVector() const;

  /**
   * @brief Returns binary data of the mesh
   * @return Reference to the binary buffer
   */
  [[nodiscard]] const std::vector<uint8_t>& GetData() const
  {
    return mBuffer;
  }

private:
  std::vector<uint8_t>     mBuffer;           //< Data buffer
  NavigationMeshHeader_V10 mHeader;           //< Navigation mesh header
  FaceIndex                mCurrentFace;      //< Current face (last floor position)
  Dali::Matrix             mTransform;        //< Transform matrix
  Dali::Matrix             mTransformInverse; //< Inverse of the transform matrix
};

inline Internal::Algorithm::NavigationMesh& GetImplementation(Dali::Scene3D::Algorithm::NavigationMesh& navigationMesh)
{
  return *navigationMesh.mImpl;
}

inline const Internal::Algorithm::NavigationMesh& GetImplementation(const Dali::Scene3D::Algorithm::NavigationMesh& navigationMesh)
{
  return *navigationMesh.mImpl;
}

} // namespace Dali::Scene3D::Internal::Algorithm

#endif // DALI_SCENE3D_INTERNAL_NAVIGATION_MESH_H

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

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/algorithm/navigation-mesh.h>
#include <dali-scene3d/public-api/algorithm/path-finder.h>

// INTERNAL EXTERNAL
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>

#include <cinttypes>
#include <cstdio>
#include <mutex>
#include <vector>
#include "navigation-mesh-header.h"

namespace Dali::Scene3D::Loader
{
class NavigationMeshFactory;
}

namespace Dali::Scene3D::Internal::Algorithm
{

class NavigationRay;

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
    uint16_t      faceIndex;
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
   * Looks for floor only within the face
   * @param[in] position Position to be projected onto the face
   * @param[in] faceIndex Face index
   * @param[in] dontCheckNeighbours states whether to traverse onto neighbouring faces
   * @param[out] outPosition Output position
   *
   * @return true if success
   */
  bool FindFloorForFace(const Dali::Vector3& position, uint32_t faceIndex, bool dontCheckNeighbours, Dali::Vector3& outPosition);

  /**
   * @copydoc Dali::Scene3D::Algorithm::NavigationMesh::FindFloor()
   */
  bool FindFloor(const Dali::Vector3& position, Dali::Vector3& outPosition);

  /**
   * @copydoc Dali::Scene3D::Algorithm::NavigationMesh::FindFloor()
   */
  bool FindFloor(const Dali::Vector3& position, Dali::Vector3& outPosition, uint32_t& faceIndex);

  /**
   * @copydoc Dali::Scene3D::Algorithm::NavigationMesh::GetFace()
   */
  [[nodiscard]] const Face* GetFace(int index) const;

  /**
   * @copydoc Dali::Scene3D::Algorithm::NavigationMesh::GetEdge()
   */
  [[nodiscard]] const Edge* GetEdge(int index) const;

  /**
   * @copydoc Dali::Scene3D::Algorithm::NavigationMesh::GetVertex()
   */
  [[nodiscard]] const Vertex* GetVertex(int index) const;

  /**
   * @copydoc Dali::Scene3D::Algorithm::NavigationMesh::SetSceneTransform()
   */
  void SetTransform(const Dali::Matrix& transform);

  /**
   * Tests intersection between navigation ray and face
   */
  IntersectResult NavigationRayFaceIntersection(NavigationRay& ray, const Face& face);

  /**
   * @copydoc Dali::Scene3D::Algorithm::NavigationMesh::PointSceneToLocal()
   */
  Dali::Vector3 PointSceneToLocal(const Dali::Vector3& point);

  /**
   * @copydoc Dali::Scene3D::Algorithm::NavigationMesh::PointLocalToScene()
   */
  Dali::Vector3 PointLocalToScene(const Dali::Vector3& point);

  /**
   * @copydoc Dali::Scene3D::Algorithm::NavigationMesh::GetGravityVector()
   */
  [[nodiscard]] Dali::Vector3 GetGravityVector() const;

private:
  std::vector<uint8_t> mBuffer;                    //< Data buffer
  NavigationMeshHeader_V10 mHeader;                //< Navigation mesh header
  uint16_t             mCurrentFace;               //< Current face (last floor position)
  Dali::Matrix         mTransform;                 //< Transform matrix
  Dali::Matrix         mTransformInverse;          //< Inverse of the transform matrix
};

inline Internal::Algorithm::NavigationMesh& GetImplementation(Dali::Scene3D::Algorithm::NavigationMesh& navigationMesh)
{
  return *navigationMesh.mImpl;
}

} // namespace Dali::Scene3D::Internal::Algorithm

#endif // DALI_SCENE3D_INTERNAL_NAVIGATION_MESH_H

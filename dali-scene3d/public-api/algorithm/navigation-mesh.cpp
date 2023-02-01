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

// CLASS HEADER
#include <dali-scene3d/public-api/algorithm/navigation-mesh.h>

// INTERNAL HEADERS
#include <dali-scene3d/internal/algorithm/navigation-mesh-impl.h>

using Dali::Vector3;

namespace Dali::Scene3D::Algorithm
{

NavigationMesh::NavigationMesh( NavigationMeshImpl* impl )
{
  mImpl.reset( impl );
}

NavigationMesh::~NavigationMesh() = default;

[[nodiscard]] uint32_t NavigationMesh::GetFaceCount() const
{
  return mImpl->GetFaceCount();
}

[[nodiscard]] uint32_t NavigationMesh::GetEdgeCount() const
{
  return mImpl->GetEdgeCount();
}

[[nodiscard]] uint32_t NavigationMesh::GetVertexCount() const
{
  return mImpl->GetVertexCount();
}

bool NavigationMesh::FindFloor(const Dali::Vector3& position, Dali::Vector3& outPosition, uint32_t& polyIndex)
{
  return mImpl->FindFloor(position, outPosition, polyIndex);
}

bool NavigationMesh::FindFloorForFace(const Dali::Vector3& position, uint32_t faceIndex, bool dontCheckNeighbours, Dali::Vector3& outPosition)
{
  return mImpl->FindFloorForFace(position, faceIndex, dontCheckNeighbours, outPosition);
}

[[nodiscard]] const NavigationMesh::Face* NavigationMesh::GetFace(int index) const
{
  return mImpl->GetFace(index);
}

[[nodiscard]] const NavigationMesh::Edge* NavigationMesh::GetEdge(int index) const
{
  return mImpl->GetEdge(index);
}

[[nodiscard]] const NavigationMesh::Vertex* NavigationMesh::GetVertex(int index) const
{
  return mImpl->GetVertex(index);
}

void NavigationMesh::SetSceneTransform(const Dali::Matrix& transform)
{
  mImpl->SetTransform(transform);
}

Dali::Vector3 NavigationMesh::PointSceneToLocal(const Dali::Vector3& point)
{
  return mImpl->PointSceneToLocal(point);
}

Dali::Vector3 NavigationMesh::PointLocalToScene(const Dali::Vector3& point)
{
  return mImpl->PointLocalToScene(point);
}

Dali::Vector3 NavigationMesh::GetGravityVector() const
{
  return mImpl->GetGravityVector();
}

}
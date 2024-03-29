/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
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
#include <dali-scene3d/internal/algorithm/navigation-mesh-impl.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

#include <algorithm>
#include <filesystem>

using Dali::Vector3;

namespace Dali::Scene3D::Internal::Algorithm
{
using Poly   = Dali::Scene3D::Algorithm::NavigationMesh::Face;
using Edge   = Dali::Scene3D::Algorithm::NavigationMesh::Edge;
using Vertex = Dali::Scene3D::Algorithm::NavigationMesh::Vertex;

/**
 * Helper function calculating intersection point between triangle and ray
 */
static bool RayTriangleIntersect(
  const Vector3& origin, const Vector3& direction, const Vector3& vertex0, const Vector3& vertex1, const Vector3& vertex2, const Vector3& normal, float& outDistance, Vector3& position)
{
  auto N = normal;
  N.Normalize();
  // Step 1: finding P

  // check if ray and plane are parallel ?
  float NdotRayDirection = N.Dot(direction);
  if(Dali::Equals(NdotRayDirection, 0.0f))
  {
    return false; // they are parallel so they don'outDistance intersect !
  }

  // compute d parameter using equation 2
  float d = -N.Dot(vertex0);

  // compute outDistance (equation 3)
  outDistance = -(N.Dot(origin) + d) / NdotRayDirection;

  // check if the triangle is in behind the ray
  if(outDistance < 0) return false; // the triangle is behind

  // compute the intersection point using equation 1
  auto P = origin + (direction * outDistance);

  position = P;

  auto edge0 = vertex1 - vertex0;
  auto edge1 = vertex2 - vertex1;
  auto edge2 = vertex0 - vertex2;
  auto C0    = P - vertex0;
  auto C1    = P - vertex1;
  auto C2    = P - vertex2;

  auto r0 = N.Dot(edge0.Cross(C0));
  auto r1 = N.Dot(edge1.Cross(C1));
  auto r2 = N.Dot(edge2.Cross(C2));
  if(r0 > 0 &&
     r1 > 0 &&
     r2 > 0) return true; // P is inside the triangle

  return false; // this ray hits the triangle
}

NavigationMesh::NavigationMesh(const std::vector<uint8_t>& buffer)
{
  mBuffer.resize(buffer.size());
  std::copy(buffer.begin(), buffer.end(), mBuffer.begin());

  // Setup header from the buffer
  mHeader      = *reinterpret_cast<NavigationMeshHeader_V10*>(mBuffer.data());
  mCurrentFace = Scene3D::Algorithm::NavigationMesh::NULL_FACE;
}

[[nodiscard]] uint32_t NavigationMesh::GetFaceCount() const
{
  return mHeader.polyCount;
}

[[nodiscard]] uint32_t NavigationMesh::GetEdgeCount() const
{
  return mHeader.edgeCount;
}

[[nodiscard]] uint32_t NavigationMesh::GetVertexCount() const
{
  return mHeader.vertexCount;
}

bool NavigationMesh::FindFloorForFace(const Dali::Vector3& position, FaceIndex faceIndex, bool dontCheckNeighbours, Dali::Vector3& outPosition)
{
  if(faceIndex == ::Dali::Scene3D::Algorithm::NavigationMesh::NULL_FACE)
  {
    faceIndex = mCurrentFace;
  }

  // No current face, do full check
  if(mCurrentFace == ::Dali::Scene3D::Algorithm::NavigationMesh::NULL_FACE)
  {
    return FindFloor(position, outPosition);
  }

  NavigationRay ray;
  ray.origin = PointSceneToLocal(Dali::Vector3(position)); // origin is equal position

  // Ray direction matches gravity direction
  ray.direction = Vector3(mHeader.gravityVector);

  IntersectResult result = NavigationRayFaceIntersection(ray, *GetFace(faceIndex));
  if(result.result)
  {
    outPosition = PointLocalToScene(result.point);
    return true;
  }
  else
  {
    if(dontCheckNeighbours)
    {
      return false;
    }

    // Test neighbouring by edges
    const auto& poly = GetFace(faceIndex);

    // collect faces sharing edges
    std::vector<FaceIndex> neighbourFaces;

    for(auto edgeIndex : poly->edge)
    {
      const auto& edge = *GetEdge(edgeIndex);

      // store faces
      for(auto edgeFaceIndex : edge.face)
      {
        if(edgeFaceIndex != ::Dali::Scene3D::Algorithm::NavigationMesh::NULL_FACE && edgeFaceIndex != faceIndex)
        {
          neighbourFaces.emplace_back(edgeFaceIndex);
        }
      }
    }

    if(neighbourFaces.empty())
    {
      return false;
    }

    for(const auto& neighbourFaceIndex : neighbourFaces)
    {
      if(FindFloorForFace(position, neighbourFaceIndex, true, outPosition))
      {
        mCurrentFace = neighbourFaceIndex;
        return true;
      }
    }
  }

  return false;
}

/**
 * Drops observer onto the floor
 *
 * When dropping observer, the nearest floor face is found
 */
bool NavigationMesh::FindFloor(const Dali::Vector3& position, Dali::Vector3& outPosition)
{
  FaceIndex faceIndex = 0u;
  return FindFloor(position, outPosition, faceIndex);
}

bool NavigationMesh::FindFloor(const Dali::Vector3& position, Dali::Vector3& outPosition, FaceIndex& outFaceIndex)
{
  [[maybe_unused]] auto newPos = PointSceneToLocal(Dali::Vector3(position));

  NavigationRay ray;

  ray.origin = PointSceneToLocal(Dali::Vector3(position)); // origin is equal position

  // Ray direction matches gravity direction
  ray.direction = Vector3(mHeader.gravityVector);

  const auto                   POLY_COUNT = GetFaceCount();
  std::vector<IntersectResult> results;
  for(auto faceIndex = 0u; faceIndex < POLY_COUNT; ++faceIndex)
  {
    auto result = NavigationRayFaceIntersection(ray, *GetFace(faceIndex));
    if(result.result)
    {
      result.faceIndex = faceIndex;
      results.emplace_back(result);
    }
  }

  // find minimal distance to the floor and return that position and distance
  if(results.empty())
  {
    return false;
  }

  std::sort(results.begin(), results.end(), [](const IntersectResult& lhs, const IntersectResult& rhs) { return lhs.distance < rhs.distance; });

  outPosition  = PointLocalToScene(results.front().point);
  outFaceIndex = results.front().faceIndex;
  mCurrentFace = outFaceIndex;

  return true;
}

const Poly* NavigationMesh::GetFace(FaceIndex index) const
{
  auto* basePtr = reinterpret_cast<const Poly*>(mBuffer.data() + mHeader.dataOffset + mHeader.polyDataOffset);
  return &basePtr[index];
}

const Edge* NavigationMesh::GetEdge(EdgeIndex index) const
{
  auto* basePtr = reinterpret_cast<const Edge*>(mBuffer.data() + mHeader.dataOffset + mHeader.edgeDataOffset);
  return &basePtr[index];
}

const Vertex* NavigationMesh::GetVertex(VertexIndex index) const
{
  auto* basePtr = reinterpret_cast<const Vertex*>(mBuffer.data() + mHeader.dataOffset + mHeader.vertexDataOffset);
  return &basePtr[index];
}

NavigationMesh::IntersectResult NavigationMesh::NavigationRayFaceIntersection(NavigationRay& ray, const Face& face) const
{
  auto vi0 = *GetVertex(face.vertex[0]);
  auto vi1 = *GetVertex(face.vertex[1]);
  auto vi2 = *GetVertex(face.vertex[2]);

  IntersectResult result{Vector3::ZERO, 0.0f, 0u, false};

  result.result = RayTriangleIntersect(ray.origin, ray.direction, Vector3(vi0.x, vi0.y, vi0.z), Vector3(vi1.x, vi1.y, vi1.z), Vector3(vi2.x, vi2.y, vi2.z), Vector3(face.normal), result.distance, result.point);
  return result;
}

NavigationMesh::IntersectResult NavigationMesh::RayCastIntersect(NavigationRay& rayOrig) const
{
  auto                       faceCount = GetFaceCount();
  std::list<IntersectResult> results;

  NavigationRay ray;

  ray.origin = PointSceneToLocal(rayOrig.origin); // origin is equal position

  // Ray direction matches gravity direction
  ray.direction = PointSceneToLocal(rayOrig.origin + rayOrig.direction) - ray.origin;
  ray.direction.Normalize();
  for(auto i = 0u; i < faceCount; ++i)
  {
    auto result = NavigationRayFaceIntersection(ray, *GetFace(i));
    if(result.result)
    {
      result.faceIndex = i;
      if(results.empty())
      {
        results.push_back(result);
      }
      else
      {
        for(auto it = results.begin(); it != results.end(); ++it)
        {
          if((*it).distance > result.distance)
          {
            results.insert(it, result);
            break;
          }
        }
      }
    }
  }
  if(!results.empty())
  {
    return results.front();
  }
  else
  {
    return IntersectResult{Vector3::ZERO, 0.0f, 0u, false};
  }
}

void NavigationMesh::SetTransform(const Dali::Matrix& transform)
{
  mTransform        = transform;
  mTransformInverse = mTransform;
  mTransformInverse.Invert();
}

Dali::Vector3 NavigationMesh::PointSceneToLocal(const Dali::Vector3& point) const
{
  Dali::Vector4 invNewPos = mTransformInverse * Dali::Vector4(point.x, point.y, point.z, 1.0f);

  return Dali::Vector3(invNewPos.AsFloat());
}

Dali::Vector3 NavigationMesh::PointLocalToScene(const Dali::Vector3& point) const
{
  // Transform point into scene transform space
  Dali::Vector4 newPos = mTransform * Dali::Vector4(point.x, point.y, point.z, 1.0f);

  return Dali::Vector3(newPos.AsFloat());
}

Dali::Vector3 NavigationMesh::GetGravityVector() const
{
  return Dali::Vector3(mHeader.gravityVector);
}

} // namespace Dali::Scene3D::Internal::Algorithm
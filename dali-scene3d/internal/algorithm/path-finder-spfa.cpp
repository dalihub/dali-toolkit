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
#include <dali-scene3d/internal/algorithm/path-finder-spfa.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <limits>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/algorithm/path-finder-waypoint-data.h>
#include <dali-scene3d/public-api/algorithm/path-finder-waypoint.h>

using WayPointList = Dali::Scene3D::Algorithm::WayPointList;

namespace Dali::Scene3D::Internal::Algorithm
{
PathFinderAlgorithmSPFA::PathFinderAlgorithmSPFA(Dali::Scene3D::Algorithm::NavigationMesh& navMesh)
: mNavigationMesh(&GetImplementation(navMesh))
{
  PrepareData();
}

PathFinderAlgorithmSPFA::~PathFinderAlgorithmSPFA() = default;

Scene3D::Algorithm::WayPointList PathFinderAlgorithmSPFA::FindPath(const Dali::Vector3& positionFrom, const Dali::Vector3& positionTo)
{
  Dali::Vector3 outPosFrom;
  uint32_t      polyIndexFrom;
  auto          result = mNavigationMesh->FindFloor(positionFrom, outPosFrom, polyIndexFrom);

  Scene3D::Algorithm::WayPointList waypoints;

  if(result)
  {
    Dali::Vector3 outPosTo;
    uint32_t      polyIndexTo;
    result = mNavigationMesh->FindFloor(positionTo, outPosTo, polyIndexTo);

    if(result)
    {
      // Get waypoints
      waypoints = FindPath(polyIndexFrom, polyIndexTo);

      // replace first and last waypoint
      auto& wpFrom = static_cast<WayPointData&>(waypoints[0]);
      auto& wpTo   = static_cast<WayPointData&>(waypoints.back());

      Vector2 fromCenter(wpFrom.point3d.x, wpFrom.point3d.y);
      wpFrom.point3d = outPosFrom;
      wpFrom.point2d = fromCenter - Vector2(outPosFrom.x, outPosFrom.y);

      Vector2 toCenter(wpTo.point3d.x, wpTo.point3d.y);
      wpTo.point3d = outPosTo;
      wpTo.point2d = toCenter - Vector2(outPosTo.x, outPosTo.y);
      wpTo.point3d = outPosTo;
    }
  }

  // Returns waypoints with non-zero size of empty vector in case of failure (no path to be found)
  return waypoints;
}

Scene3D::Algorithm::WayPointList PathFinderAlgorithmSPFA::FindPath(uint32_t sourcePolyIndex, uint32_t targetPolyIndex)
{
  auto                  nodeCount = uint32_t(mNodes.size());
  std::vector<float>    dist;
  std::vector<uint32_t> prev;
  std::vector<bool>     queued;

  dist.resize(mNodes.size());
  prev.resize(mNodes.size());
  queued.resize(mNodes.size());

  std::list<uint32_t> nodeQueue;

  [[maybe_unused]] auto sourcePos = Dali::Vector3(Face(sourcePolyIndex)->center);

  for(auto i = 0u; i < nodeCount; ++i)
  {
    dist[i]   = std::numeric_limits<float>::infinity();
    prev[i]   = Scene3D::Algorithm::NavigationMesh::NULL_FACE; // set prev to null polygon
    queued[i] = false;
  }

  // Set distance of source
  dist[sourcePolyIndex]   = 0.0f;
  queued[sourcePolyIndex] = true;
  nodeQueue.push_back(sourcePolyIndex);

  while(!nodeQueue.empty())
  {
    // find minimum distance
    auto minDistIndex = nodeQueue.front();
    nodeQueue.pop_front();
    queued[minDistIndex] = false;

    // Skip operator when minDistIndex is target
    if(minDistIndex == targetPolyIndex)
    {
      continue;
    }

    // check the neighbours
    for(auto i = 0u; i < 3; ++i)
    {
      auto nIndex = mNodes[minDistIndex].faces[i];
      if(nIndex != Scene3D::Algorithm::NavigationMesh::NULL_FACE)
      {
        auto alt = dist[minDistIndex] + mNodes[minDistIndex].weight[i];
        if(alt < dist[nIndex])
        {
          dist[nIndex] = alt;
          prev[nIndex] = minDistIndex;

          if(!queued[nIndex])
          {
            queued[nIndex] = true;
            if(!nodeQueue.empty() && dist[nIndex] < dist[nodeQueue.front()])
            {
              nodeQueue.push_front(nIndex);
            }
            else
            {
              nodeQueue.push_back(nIndex);
            }
          }
        }
      }
    }
  }

  // Compute distances for each node back to the source
  auto                u = targetPolyIndex;
  std::list<uint32_t> q;
  if(prev[u] != Scene3D::Algorithm::NavigationMesh::NULL_FACE || u == sourcePolyIndex)
  {
    while(u != Scene3D::Algorithm::NavigationMesh::NULL_FACE)
    {
      q.push_front(u);
      u = prev[u];
    }
  }

  WayPointList waypoints;
  waypoints.resize(q.size());

  if(q.size() == 0)
  {
    // Fail to found path. Return empty list.
    return waypoints;
  }

  auto index = 0u;
  auto prevN = 0u;
  for(auto n : q)
  {
    auto& wp     = static_cast<WayPointData&>(waypoints[index]);
    wp.face      = mNavigationMesh->GetFace(n);
    wp.nodeIndex = n;

    wp.edge = nullptr;
    // set the common edge with previous node
    if(index > 0)
    {
      const auto& node = mNodes[prevN];
      for(auto i = 0u; i < 3; ++i)
      {
        if(node.faces[i] == wp.nodeIndex)
        {
          wp.edge = mNavigationMesh->GetEdge(node.edges[i]);
          break;
        }
      }
    }

    prevN = n;
    index++;
  }

  return OptimizeWaypoints(waypoints);
}

void PathFinderAlgorithmSPFA::PrepareData()
{
  // Build the list structure connecting the nodes
  auto faceCount = mNavigationMesh->GetFaceCount();

  mNodes.resize(faceCount);

  // for each face build the list
  for(auto i = 0u; i < faceCount; ++i)
  {
    auto&       node = mNodes[i];
    const auto* face = mNavigationMesh->GetFace(i);
    auto        c0   = Dali::Vector3(face->center);

    // for each edge add neighbouring face and compute distance to set the weight of node
    for(auto edgeIndex = 0u; edgeIndex < 3; ++edgeIndex)
    {
      const auto* edge = mNavigationMesh->GetEdge(face->edge[edgeIndex]);
      auto        p1   = edge->face[0];
      auto        p2   = edge->face[1];

      // One of faces is current face so ignore it
      auto p                = ((p1 != i) ? p1 : p2);
      node.faces[edgeIndex] = p;
      if(p != ::Dali::Scene3D::Algorithm::NavigationMesh::NULL_FACE)
      {
        node.edges[edgeIndex]  = face->edge[edgeIndex];
        auto c1                = Dali::Vector3(mNavigationMesh->GetFace(p)->center);
        node.weight[edgeIndex] = (c1 - c0).Length();
      }
    }
  }
}

[[maybe_unused]] static float ccw(const Dali::Vector2& A, const Dali::Vector2& B, const Dali::Vector2& C)
{
  return (C.y - A.y) * (B.x - A.x) > (B.y - A.y) * (C.x - A.x);
}

[[maybe_unused]] static bool intersect(const Dali::Vector2& A, const Dali::Vector2& B, const Dali::Vector2& C, const Dali::Vector2& D)
{
  return ccw(A, C, D) != ccw(B, C, D) && ccw(A, B, C) != ccw(A, B, D);
}

Scene3D::Algorithm::WayPointList PathFinderAlgorithmSPFA::OptimizeWaypoints(WayPointList& waypoints) const
{
  WayPointList optimizedWaypoints;
  optimizedWaypoints.emplace_back(waypoints[0]);
  optimizedWaypoints.reserve(waypoints.size());

  auto startIndex = 1u;

  bool finished = false;
  for(auto j = 0; !finished; ++j)
  {
    auto&       startWaypoint     = optimizedWaypoints.back();
    const auto& startWaypointData = static_cast<const WayPointData&>(startWaypoint);

    // add new-last waypoint which will be overriden as long as intersection takes place
    optimizedWaypoints.emplace_back();
    for(auto wpIndex = startIndex; wpIndex < waypoints.size(); ++wpIndex)
    {
      if(wpIndex == waypoints.size() - 1)
      {
        optimizedWaypoints.back() = waypoints.back();
        finished                  = true;
        continue;
      }
      // Points between centres of faces

      const auto& wpData = static_cast<const WayPointData&>(waypoints[wpIndex]);

      auto Pa0 = Dali::Vector2(startWaypointData.face->center[0], startWaypointData.face->center[1]);
      auto Pa1 = Dali::Vector2(wpData.face->center[0], wpData.face->center[1]);

      bool doesIntersect = true;
      for(auto i = startIndex; i < wpIndex; ++i)
      {
        const auto& wp = static_cast<WayPointData&>(waypoints[i]);
        // Skip starting waypoint
        if(wp.face == startWaypointData.face)
        {
          continue;
        }
        auto Pb0  = mNavigationMesh->GetVertex(wp.edge->vertex[0]);
        auto Pb1  = mNavigationMesh->GetVertex(wp.edge->vertex[1]);
        auto vPb0 = Dali::Vector2(Pb0->x, Pb0->y);
        auto vPb1 = Dali::Vector2(Pb1->x, Pb1->y);

        doesIntersect = intersect(Pa0, Pa1, vPb0, vPb1);
        if(!doesIntersect)
        {
          break;
        }
      }

      if(!doesIntersect)
      {
        optimizedWaypoints.back() = waypoints[wpIndex - 1];
        startIndex                = wpIndex - 1;
        break;
      }
    }
  }

  for(auto& wp : optimizedWaypoints)
  {
    auto& wpData   = static_cast<WayPointData&>(wp);
    wpData.point3d = mNavigationMesh->PointLocalToScene(Dali::Vector3(wpData.face->center));
    wpData.point2d = Vector2::ZERO;
  }

  return optimizedWaypoints;
}
} // namespace Dali::Scene3D::Internal::Algorithm

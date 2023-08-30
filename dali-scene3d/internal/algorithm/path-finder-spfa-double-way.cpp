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
#include <dali-scene3d/internal/algorithm/path-finder-spfa-double-way.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <limits>
#include <unordered_set>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/algorithm/path-finder-waypoint-data.h>
#include <dali-scene3d/public-api/algorithm/path-finder-waypoint.h>

using WayPointList  = Dali::Scene3D::Algorithm::WayPointList;
using FaceNodeIndex = Dali::Scene3D::Internal::Algorithm::PathFinderAlgorithmSPFADoubleWay::FaceNodeIndex;

namespace
{
constexpr float PRIORITY_SCALE_FACTOR = 0.7f; ///< The value of heuristic factor that how much will you consider
                                              ///  direction of source --> target. If 0.0f, we will use only dist.

/**
 * @brief Get the Component Id object
 *
 * @param[in,out] components Container of components id stored.
 * @param[in] index index what we want to get components's id.
 * @return FaceIndex top-value of this components.
 */
FaceNodeIndex GetComponentId(std::vector<FaceNodeIndex>& components, FaceNodeIndex index)
{
  if(components[index] == index)
  {
    return index;
  }
  // Get my parent's components id, and update myself.
  FaceNodeIndex ret        = GetComponentId(components, components[index]);
  return components[index] = ret;
}

/**
 * @brief Combine two elements by Union-Find algorithm.
 *
 * @param[in,out] components Container of components id stored.
 * @param[in,out] componentsLevel Container of components level stored.
 * @param[in] index0 index of components what we want to be combined.
 * @param[in] index1 index of components what we want to be combined.
 */
void ComponentsCombine(std::vector<FaceNodeIndex>& components, std::vector<FaceNodeIndex>& componentsLevel, FaceNodeIndex index0, FaceNodeIndex index1)
{
  FaceNodeIndex ancestor0 = GetComponentId(components, index0);
  FaceNodeIndex ancestor1 = GetComponentId(components, index1);
  if(ancestor0 == ancestor1)
  {
    return;
  }

  if(componentsLevel[ancestor0] < componentsLevel[ancestor1])
  {
    components[ancestor0] = ancestor1;
  }
  else
  {
    components[ancestor1] = ancestor0;
    if(componentsLevel[ancestor0] == componentsLevel[ancestor1])
    {
      ++componentsLevel[ancestor0];
    }
  }
}
} // namespace

namespace Dali::Scene3D::Internal::Algorithm
{
PathFinderAlgorithmSPFADoubleWay::PathFinderAlgorithmSPFADoubleWay(Dali::Scene3D::Algorithm::NavigationMesh& navMesh)
: mNavigationMesh(&GetImplementation(navMesh))
{
  PrepareData();
}

PathFinderAlgorithmSPFADoubleWay::~PathFinderAlgorithmSPFADoubleWay() = default;

float PathFinderAlgorithmSPFADoubleWay::DistancePanaltyCalculate(FaceIndex index) const noexcept
{
  return dist[index] - priority[index] * PRIORITY_SCALE_FACTOR;
}

Scene3D::Algorithm::WayPointList PathFinderAlgorithmSPFADoubleWay::FindPath(const Dali::Vector3& positionFrom, const Dali::Vector3& positionTo)
{
  Dali::Vector3 outPosFrom;
  FaceIndex     polyIndexFrom;
  auto          result = mNavigationMesh->FindFloor(positionFrom, outPosFrom, polyIndexFrom);

  Scene3D::Algorithm::WayPointList waypoints;

  if(result)
  {
    Dali::Vector3 outPosTo;
    FaceIndex     polyIndexTo;
    result = mNavigationMesh->FindFloor(positionTo, outPosTo, polyIndexTo);

    if(result)
    {
      // Get waypoints
      waypoints = FindPath(polyIndexFrom, polyIndexTo);

      if(!waypoints.empty())
      {
        // replace first and last waypoint
        auto& wpFrom = static_cast<WayPointData&>(waypoints[0]);
        auto& wpTo   = static_cast<WayPointData&>(waypoints.back());

        Vector2 fromCenter(wpFrom.point3d.x, wpFrom.point3d.y);
        wpFrom.point3d = outPosFrom;
        wpFrom.point2d = fromCenter - Vector2(outPosFrom.x, outPosFrom.y);

        Vector2 toCenter(wpTo.point3d.x, wpTo.point3d.y);
        wpTo.point3d = outPosTo;
        wpTo.point2d = toCenter - Vector2(outPosTo.x, outPosTo.y);
      }
    }
  }

  // Returns waypoints with non-zero size of empty vector in case of failure (no path to be found)
  return waypoints;
}

Scene3D::Algorithm::WayPointList PathFinderAlgorithmSPFADoubleWay::FindPath(FaceIndex sourcePolyIndex, FaceIndex targetPolyIndex)
{
  // Fast return if source and target index is same.
  if(sourcePolyIndex == targetPolyIndex)
  {
    WayPointList waypoints;
    waypoints.resize(1);

    auto& wp     = static_cast<WayPointData&>(waypoints[0]);
    wp.face      = mNavigationMesh->GetFace(sourcePolyIndex);
    wp.nodeIndex = sourcePolyIndex;
    wp.edge      = nullptr;

    return OptimizeWaypoints(waypoints);
  }

  // Fast return if source and target index is not in same components.
  // That mean, there is no path. Return empty list.
  if(GetComponentId(componentIds, sourcePolyIndex) != GetComponentId(componentIds, targetPolyIndex))
  {
    return WayPointList();
  }

  // pair<navimesh FaceIndex, is backward direction>
  using queueItem = std::pair<FaceIndex, uint8_t>;

  std::list<queueItem> nodeQueue;

  std::unordered_set<FaceIndex> usedPolyIndexs[2];

  // Set distance of source and target
  dist[sourcePolyIndex]     = 0.0f;
  dist[targetPolyIndex]     = 0.0f;
  priority[sourcePolyIndex] = 0.0f;
  priority[targetPolyIndex] = 0.0f;
  queued[sourcePolyIndex]   = true;
  queued[targetPolyIndex]   = true;
  nodeQueue.push_back(std::make_pair(sourcePolyIndex, 0));
  nodeQueue.push_back(std::make_pair(targetPolyIndex, 1));
  usedPolyIndexs[0].insert(sourcePolyIndex);
  usedPolyIndexs[1].insert(targetPolyIndex);

  bool      foundPath          = false;
  FaceIndex forwardEndIndex    = Scene3D::Algorithm::NavigationMesh::NULL_FACE;
  FaceIndex backwardStartIndex = Scene3D::Algorithm::NavigationMesh::NULL_FACE;

  const auto sourcePos = Dali::Vector3(Face(sourcePolyIndex)->center);
  const auto targetPos = Dali::Vector3(Face(targetPolyIndex)->center);
  Vector3    direction = targetPos - sourcePos;
  direction.Normalize();

  // Note : we always success to found path since source and target is in same components.
  while(!foundPath)
  {
    // find minimum distance
    auto minDistIndex = nodeQueue.front().first;
    auto isBackward   = nodeQueue.front().second;
    nodeQueue.pop_front();
    queued[minDistIndex] = false;

    // check the neighbours
    for(auto i = 0u; i < 3 && !foundPath; ++i)
    {
      auto nIndex = mNodes[minDistIndex].faces[i];
      if(nIndex != Scene3D::Algorithm::NavigationMesh::NULL_FACE)
      {
        if(usedPolyIndexs[!isBackward].count(nIndex))
        {
          // We found path!
          foundPath = true;
          if(isBackward)
          {
            forwardEndIndex    = nIndex;
            backwardStartIndex = minDistIndex;
          }
          else
          {
            forwardEndIndex    = minDistIndex;
            backwardStartIndex = nIndex;
          }
          break;
        }

        usedPolyIndexs[isBackward].insert(nIndex);

        auto alt = dist[minDistIndex] + mNodes[minDistIndex].weight[i];
        if(alt < dist[nIndex])
        {
          dist[nIndex] = alt;

          if(isBackward)
          {
            prevBackward[nIndex] = minDistIndex;
            if(priority[nIndex] < 0.0f)
            {
              const auto currentPos = Dali::Vector3(Face(nIndex)->center);
              Vector3    diff       = currentPos - targetPos;
              priority[nIndex]      = std::max(0.0f, -direction.Dot(diff));
            }
          }
          else
          {
            prevForward[nIndex] = minDistIndex;
            if(priority[nIndex] < 0.0f)
            {
              const auto currentPos = Dali::Vector3(Face(nIndex)->center);
              Vector3    diff       = currentPos - sourcePos;
              priority[nIndex]      = std::max(0.0f, direction.Dot(diff));
            }
          }

          if(!queued[nIndex])
          {
            queued[nIndex] = true;
            if(!nodeQueue.empty() && DistancePanaltyCalculate(nIndex) < DistancePanaltyCalculate(nodeQueue.front().first))
            {
              nodeQueue.push_front(std::make_pair(nIndex, isBackward));
            }
            else
            {
              nodeQueue.push_back(std::make_pair(nIndex, isBackward));
            }
          }
        }
      }
    }
  }

  // Build path of face index
  std::list<FaceIndex> q;
  {
    FaceIndex u = forwardEndIndex;
    while(u != Scene3D::Algorithm::NavigationMesh::NULL_FACE)
    {
      q.push_front(u);
      u = prevForward[u];
    }
  }
  {
    FaceIndex u = backwardStartIndex;
    while(u != Scene3D::Algorithm::NavigationMesh::NULL_FACE)
    {
      q.push_back(u);
      u = prevBackward[u];
    }
  }

  WayPointList waypoints;
  waypoints.resize(q.size());

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

  // Reset informations what we used.
  // Forward indexes
  for(const auto& i : usedPolyIndexs[0])
  {
    dist[i]         = std::numeric_limits<float>::infinity();
    priority[i]     = -1.0f;                                         // Initialize by negative value, that we didn't calculate yet.
    prevForward[i]  = Scene3D::Algorithm::NavigationMesh::NULL_FACE; // set prev to null polygon
    prevBackward[i] = Scene3D::Algorithm::NavigationMesh::NULL_FACE; // set prev to null polygon
    queued[i]       = false;
  }
  // Backward indexes
  for(const auto& i : usedPolyIndexs[1])
  {
    dist[i]         = std::numeric_limits<float>::infinity();
    priority[i]     = -1.0f;                                         // Initialize by negative value, that we didn't calculate yet.
    prevForward[i]  = Scene3D::Algorithm::NavigationMesh::NULL_FACE; // set prev to null polygon
    prevBackward[i] = Scene3D::Algorithm::NavigationMesh::NULL_FACE; // set prev to null polygon
    queued[i]       = false;
  }

  return OptimizeWaypoints(waypoints);
}

void PathFinderAlgorithmSPFADoubleWay::PrepareData()
{
  // Build the list structure connecting the nodes
  auto faceCount = mNavigationMesh->GetFaceCount();

  mNodes.resize(faceCount);
  dist.resize(faceCount);
  priority.resize(faceCount);
  prevForward.resize(faceCount);
  prevBackward.resize(faceCount);
  componentIds.resize(faceCount);
  queued.resize(faceCount);

  // Temperal container for components level. It will be used for Union-Find algorithm.
  std::vector<FaceNodeIndex> componentLevels(faceCount);

  // Initialize path informations.
  for(FaceNodeIndex i = 0u; i < faceCount; ++i)
  {
    dist[i]         = std::numeric_limits<float>::infinity();
    priority[i]     = -1.0f;                                         // Initialize by negative value, that we didn't calculate yet.
    prevForward[i]  = Scene3D::Algorithm::NavigationMesh::NULL_FACE; // set prev to null polygon
    prevBackward[i] = Scene3D::Algorithm::NavigationMesh::NULL_FACE; // set prev to null polygon
    queued[i]       = false;

    componentIds[i]    = i; // Components id should be initialized by itself.
    componentLevels[i] = 0u;
  }

  // for each face build the list
  // TODO : Currently, we are assume that FaceNodeIndex is matched with FaceIndex 1:1. This might be changed in future.
  for(FaceNodeIndex i = 0u; i < faceCount; ++i)
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

        // Connect two components
        ComponentsCombine(componentIds, componentLevels, i, p);
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

Scene3D::Algorithm::WayPointList PathFinderAlgorithmSPFADoubleWay::OptimizeWaypoints(WayPointList& waypoints) const
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

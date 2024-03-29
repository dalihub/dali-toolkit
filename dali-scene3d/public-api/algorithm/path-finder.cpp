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
#include <dali-scene3d/public-api/algorithm/path-finder.h>

// INTERNAL INCLUDES
// default algorithm
#include <dali-scene3d/internal/algorithm/path-finder-dijkstra.h>
#include <dali-scene3d/internal/algorithm/path-finder-spfa-double-way.h>
#include <dali-scene3d/internal/algorithm/path-finder-spfa.h>

namespace Dali::Scene3D::Algorithm
{
std::unique_ptr<PathFinder> PathFinder::New(NavigationMesh& navigationMesh, PathFinderAlgorithm algorithm)
{
  PathFinderBase* impl = nullptr;

  switch(algorithm)
  {
    case PathFinderAlgorithm::DIJKSTRA_SHORTEST_PATH:
    {
      impl = new Dali::Scene3D::Internal::Algorithm::PathFinderAlgorithmDijkstra(navigationMesh);
      break;
    }
    case PathFinderAlgorithm::SPFA:
    {
      impl = new Dali::Scene3D::Internal::Algorithm::PathFinderAlgorithmSPFA(navigationMesh);
      break;
    }
    case PathFinderAlgorithm::SPFA_DOUBLE_WAY:
    {
      impl = new Dali::Scene3D::Internal::Algorithm::PathFinderAlgorithmSPFADoubleWay(navigationMesh);
      break;
    }
  }

  if(!impl)
  {
    return {};
  }

  auto retval = std::unique_ptr<PathFinderBase>();
  retval.reset(impl);
  return std::unique_ptr<Algorithm::PathFinder>(new Algorithm::PathFinder(std::move(retval)));
}

WayPointList PathFinder::FindPath(const Dali::Vector3& positionFrom, const Dali::Vector3& positionTo)
{
  return mImpl->FindPath(positionFrom, positionTo);
}

WayPointList PathFinder::FindPath(FaceIndex polyIndexFrom, FaceIndex polyIndexTo)
{
  return mImpl->FindPath(polyIndexFrom, polyIndexTo);
}

PathFinder::PathFinder(std::unique_ptr<PathFinderBase>&& baseImpl)
{
  mImpl = std::move(baseImpl);
}

} // namespace Dali::Scene3D::Algorithm

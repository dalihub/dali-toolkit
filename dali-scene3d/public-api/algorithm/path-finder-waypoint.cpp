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
#include <dali-scene3d/public-api/algorithm/path-finder-waypoint.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/algorithm/navigation-mesh-impl.h>
#include <dali-scene3d/internal/algorithm/path-finder-waypoint-data.h>

// EXTERNAL INCLUDES
#include <cinttypes>

namespace Dali::Scene3D::Algorithm
{

WayPoint::WayPoint()
{
  mImpl = std::make_unique<Scene3D::Internal::Algorithm::WayPointData>();
}

WayPoint::~WayPoint() = default;

uint32_t WayPoint::GetNavigationMeshFaceIndex() const
{
  return mImpl->nodeIndex;
}

Dali::Vector2 WayPoint::GetFaceLocalSpacePosition() const
{
  return mImpl->point2d;
}

Dali::Vector3 WayPoint::GetScenePosition() const
{
  return mImpl->point3d;
}

WayPoint::WayPoint(const WayPoint& wp)
{
  mImpl = std::make_unique<Internal::Algorithm::WayPointData>();
  *mImpl = *wp.mImpl;
}

WayPoint& WayPoint::operator=(const WayPoint& wp)
{
  mImpl = std::make_unique<Internal::Algorithm::WayPointData>();
  *mImpl = *wp.mImpl;
  return *this;
}

WayPoint::operator Internal::Algorithm::WayPointData&()
{
  return *mImpl;
}

}

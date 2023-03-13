#ifndef DALI_SCENE3D_PATH_FINDER_WAYPOINT_H
#define DALI_SCENE3D_PATH_FINDER_WAYPOINT_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
 *  limitations under the License.
 */

// EXTERNAL INCLUDES
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>

#include <cinttypes>
#include <memory>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>

namespace Dali::Scene3D::Internal::Algorithm
{
class WayPointData;
}

namespace Dali::Scene3D::Algorithm
{
/**
 * @class WayPoint
 *
 * The class represents a public interface to the WayPoint object
 */
class DALI_SCENE3D_API WayPoint
{
public:
  /**
   * @brief Constructor
   */
  WayPoint();

  /**
   * @brief Destructor
   */
  ~WayPoint();

  /**
   * @brief Returns index of bounding face within the NavigationMesh
   *
   * Function returns index of face withing the NavigationMesh
   * that the waypoint is associated with.
   *
   * @return Valid index of the face
   */
  [[nodiscard]] uint32_t GetNavigationMeshFaceIndex() const;

  /**
   * @brief Returns local 2D position in face space
   *
   * The face space uses the face barycentre as an origin. The x-axis is
   * aligned with x-axis of the NavigationMesh.
   *
   * @return Valid 2D location vector
   */
  [[nodiscard]] Dali::Vector2 GetFaceLocalSpacePosition() const;

  /**
   * @brief Returns waypoint 3D position in scene space
   *
   * Returns the 3D position of the waypoint in the scene space
   * of associated NavigationMesh object (using transformation set with
   * NavigationMesh::SetSceneTransform()).
   *
   * @return Valid 3D location vector
   */
  [[nodiscard]] Dali::Vector3 GetScenePosition() const;

  /**
   * @brief Copy constructor
   *
   * Only copy semantics is allowed on the WayPoint object
   */
  WayPoint(const WayPoint&);

  /**
   * @brief Copy assignment operator
   *
   * Only copy semantics is allowed on the WayPoint object
   *
   * @return Copy of source object
   */
  WayPoint& operator=(const WayPoint&);

private:
  std::unique_ptr<Internal::Algorithm::WayPointData> mImpl;

public:
  DALI_INTERNAL operator Internal::Algorithm::WayPointData&();
};
} // namespace Dali::Scene3D::Algorithm

#endif // DALI_SCENE3D_PATH_FINDER_WAYPOINT_H

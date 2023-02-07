#ifndef DALI_SCENE3D_LOADER_CAMERA_PARAMETERS_H
#define DALI_SCENE3D_LOADER_CAMERA_PARAMETERS_H
/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
 *
 */

// INTERNAL INCLUDES
#include "dali-scene3d/public-api/api.h"
#include "dali-scene3d/public-api/loader/view-projection.h"

// EXTERNAL INCLUDES
#include "dali/public-api/math/matrix.h"
#include "dali/public-api/math/vector3.h"

namespace Dali
{
class CameraActor;

namespace Scene3D
{
namespace Loader
{
struct DALI_SCENE3D_API CameraParameters
{
  Matrix matrix           = Matrix::IDENTITY;
  float  orthographicSize = 1.f;
  float  aspectRatio      = 1.f;
  float  yFov             = 60.f;
  float  zNear            = 0.1f;
  float  zFar             = 1000.f;
  bool   isPerspective    = true;

  /**
   * @return The view-projection matrix of the camera.
   */
  ViewProjection GetViewProjection() const;

  /**
   * @brief Calculates the @a position, @a orientation and @a scale that's defined
   *        for this camera, and writes it into the respective variable.
   */
  void CalculateTransformComponents(Vector3& position, Quaternion& orientation, Vector3& scale) const;

  /**
   * @brief Configures the camera in the way that it is supposed to be used with
   *        scene3d scenes. This means inverted Y and a rotation of 180 degrees
   *        along the Y axis, plus whatever the parameters define.
   */
  void ConfigureCamera(CameraActor& camera) const;
};

} // namespace Loader
} // namespace Scene3D
} // namespace Dali

#endif // DALI_SCENE3D_LOADER_CAMERA_PARAMETERS_H

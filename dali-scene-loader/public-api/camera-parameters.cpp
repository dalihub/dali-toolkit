/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include "dali-scene-loader/public-api/camera-parameters.h"
#include "dali-scene-loader/public-api/utils.h"
#include "dali/integration-api/debug.h"
#include "dali/public-api/actors/camera-actor.h"
#include "dali/public-api/math/quaternion.h"

namespace Dali
{
namespace SceneLoader
{
namespace
{
/**
 * @brief Creates a perspective matrix.
 *
 * @param[out] result The perspective matrix.
 * @param[in] left The coordinate for the left vertical clipping plane.
 * @param[in] right The coordinate for the right vertical clipping plane.
 * @param[in] bottom The coordinate for the bottom horizontal clipping plane.
 * @param[in] top The coordinate for the top horizontal clipping plane.
 * @param[in] near The distance to the near depth clipping plane.
 * @param[in] far The distance to the far depth clipping plane.
 * @param[in] invertYAxis Whether to invert the 'Y' axis.
 */
void Frustum(Matrix& result, float left, float right, float bottom, float top, float nearPlane, float farPlane, bool invertYAxis)
{
  float deltaZ = farPlane - nearPlane;
  if((nearPlane <= 0.0f) || (farPlane <= 0.0f) || Equals(right, left) || Equals(bottom, top) || (deltaZ <= 0.0f))
  {
    DALI_LOG_ERROR("Invalid parameters passed into Frustum!\n");
    DALI_ASSERT_DEBUG("Invalid parameters passed into Frustum!");
    return;
  }

  float deltaX = right - left;
  float deltaY = invertYAxis ? bottom - top : top - bottom;

  result.SetIdentity();

  float* m = result.AsFloat();
  m[0]     = -2.0f * nearPlane / deltaX;
  m[1] = m[2] = m[3] = 0.0f;

  m[5] = -2.0f * nearPlane / deltaY;
  m[4] = m[6] = m[7] = 0.0f;

  m[8]  = (right + left) / deltaX;
  m[9]  = (top + bottom) / deltaY;
  m[10] = (nearPlane + farPlane) / deltaZ;
  m[11] = 1.0f;

  m[14] = -2.0f * nearPlane * farPlane / deltaZ;
  m[12] = m[13] = m[15] = 0.0f;
}

/**
 * @brief Creates a perspective projection matrix.
 *
 * It calls Frustum()
 *
 * @param[out] result The perspective projection matrix.
 * @param[in] fovy The vertical field of view.
 * @param[in] aspect The aspect ratio.
 * @param[in] nearPlane The distance to the near depth clipping plane.
 * @param[in] farPlane The distance to the far depth clipping plane.
 * @param[in] invertYAxis Whether to invert the 'Y' axis.
 */
void Perspective(Matrix& result, float fovy, float aspect, float nearPlane, float farPlane, bool invertYAxis)
{
  float frustumH = tanf(fovy * 0.5f) * nearPlane;
  float frustumW = frustumH * aspect;

  Frustum(result, -frustumW, frustumW, -frustumH, frustumH, nearPlane, farPlane, invertYAxis);
}

/**
* @brief Creates an orthographic projection matrix.
*
* @param[out] result The orthographic projection matrix.
* @param[in] left The coordinate for the left vertical clipping plane.
* @param[in] right The coordinate for the right vertical clipping plane.
* @param[in] bottom The coordinate for the bottom horizontal clipping plane.
* @param[in] top The coordinate for the top horizontal clipping plane.
* @param[in] nearPlane The distance to the near depth clipping plane.
* @param[in] farPlane The distance to the far depth clipping plane.
* @param[in] invertYAxis Whether to invert the 'Y' axis.
*/
void Orthographic(Matrix& result, float left, float right, float bottom, float top, float nearPlane, float farPlane, bool invertYAxis)
{
  if(Equals(right, left) || Equals(top, bottom) || Equals(farPlane, nearPlane))
  {
    DALI_LOG_ERROR("Cannot create orthographic projection matrix with a zero dimension.\n");
    DALI_ASSERT_DEBUG("Cannot create orthographic projection matrix with a zero dimension.");
    return;
  }

  float deltaX = right - left;
  float deltaY = invertYAxis ? bottom - top : top - bottom;
  float deltaZ = farPlane - nearPlane;

  float* m = result.AsFloat();
  m[0]     = -2.0f / deltaX;
  m[1]     = 0.0f;
  m[2]     = 0.0f;
  m[3]     = 0.0f;

  m[4] = 0.0f;
  m[5] = -2.0f / deltaY;
  m[6] = 0.0f;
  m[7] = 0.0f;

  m[8]  = 0.0f;
  m[9]  = 0.0f;
  m[10] = 2.0f / deltaZ;
  m[11] = 0.0f;
  m[12] = -(right + left) / deltaX;
  m[13] = -(top + bottom) / deltaY;
  m[14] = -(nearPlane + farPlane) / deltaZ;
  m[15] = 1.0f;
}

} // namespace

ViewProjection CameraParameters::GetViewProjection() const
{
  ViewProjection viewProjection;
  // The projection matrix.
  if(isPerspective)
  {
    Perspective(viewProjection.GetProjection(),
                Radian(Degree(yFov)),
                1.f,
                zNear,
                zFar,
                true);
  }
  else
  {
    Orthographic(viewProjection.GetProjection(),
                 orthographicSize.x,
                 orthographicSize.y,
                 orthographicSize.z,
                 orthographicSize.w,
                 zNear,
                 zFar,
                 true);
  }

  // The view matrix.
  const Quaternion viewQuaternion(ANGLE_180, Vector3::YAXIS);
  Vector3          translation;
  Quaternion       cameraOrientation;
  Vector3          scale;
  matrix.GetTransformComponents(translation, cameraOrientation, scale);
  cameraOrientation *= viewQuaternion;

  viewProjection.GetView().SetInverseTransformComponents(scale,
                                                         cameraOrientation,
                                                         translation);

  viewProjection.Update();
  return viewProjection;
}

void CameraParameters::CalculateTransformComponents(Vector3& position, Quaternion& orientation, Vector3& scale) const
{
  matrix.GetTransformComponents(position, orientation, scale);

  // The CameraActor is expected to look down the negative Z axis, towards the scene.
  // Here we emulate the default direction of the camera in DALi.
  Quaternion viewQuaternion(ANGLE_180, Vector3::YAXIS);
  orientation *= viewQuaternion;
}

void CameraParameters::ConfigureCamera(CameraActor& camera) const
{
  SetActorCentered(camera);

  if(isPerspective)
  {
    camera.SetProjectionMode(Camera::PERSPECTIVE_PROJECTION);
    camera.SetNearClippingPlane(zNear);
    camera.SetFarClippingPlane(zFar);
    camera.SetFieldOfView(Radian(Degree(yFov)));
  }
  else
  {
    camera.SetProjectionMode(Camera::ORTHOGRAPHIC_PROJECTION);
    camera.SetOrthographicProjection(orthographicSize.x,
                                     orthographicSize.y,
                                     orthographicSize.z,
                                     orthographicSize.w,
                                     zNear,
                                     zFar);
  }

  // model
  Vector3    camTranslation;
  Vector3    camScale;
  Quaternion camOrientation;
  CalculateTransformComponents(camTranslation, camOrientation, camScale);

  camera.SetInvertYAxis(true);
  camera.SetProperty(Actor::Property::POSITION, camTranslation);
  camera.SetProperty(Actor::Property::ORIENTATION, camOrientation);
  camera.SetProperty(Actor::Property::SCALE, camScale);
}

} // namespace SceneLoader
} // namespace Dali

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

// Enable debug log for test coverage
#define DEBUG_ENABLED 1

#include <dali-test-suite-utils.h>
#include <dali/devel-api/actors/camera-actor-devel.h>
#include <string_view>
#include "dali-scene3d/public-api/loader/camera-parameters.h"

using namespace Dali;
using namespace Dali::Scene3D::Loader;

int UtcDaliCameraParameters(void)
{
  Quaternion       qView = Quaternion(Radian(Degree(180.f)), Vector3::YAXIS);
  CameraParameters camParams;
  camParams.matrix.SetTransformComponents(Vector3::ONE * 2.f,
                                          qView,
                                          Vector3::ZAXIS * -100.f);
  camParams.orthographicSize = 3.0f;
  camParams.aspectRatio      = 1.0f;
  camParams.yFov             = Degree(Radian(M_PI * .5)).degree;
  camParams.zNear            = 1.f;
  camParams.zFar             = 1000.f;

  Vector3    scale;
  Quaternion orientation;
  Vector3    position;
  camParams.CalculateTransformComponents(position, orientation, scale);
  DALI_TEST_EQUAL(scale, Vector3::ONE * 2.f);
  DALI_TEST_EQUAL(orientation, Quaternion::IDENTITY); // 2 180 degrees rotations along y
  DALI_TEST_EQUAL(position, Vector3::ZAXIS * -100.f);

  TestApplication app;
  CameraActor     camera = CameraActor::New();
  for(auto i : {false, true})
  {
    camParams.isPerspective = i;

    auto   viewProjection = camParams.GetViewProjection();
    Matrix view{false};
    Matrix::Multiply(view, Matrix(qView), camParams.matrix);
    view.Invert();
    DALI_TEST_EQUAL(viewProjection.GetView(), view);

    camParams.ConfigureCamera(camera);
    DALI_TEST_EQUAL(camParams.zNear, camera.GetNearClippingPlane());
    DALI_TEST_EQUAL(camParams.zFar, camera.GetFarClippingPlane());

    DALI_TEST_EQUAL(camera.GetInvertYAxis(), true);
    DALI_TEST_EQUAL(camera.GetProperty(Actor::Property::POSITION).Get<Vector3>(), position);
    DALI_TEST_EQUAL(camera.GetProperty(Actor::Property::ORIENTATION).Get<Quaternion>(), orientation);
    DALI_TEST_EQUAL(camera.GetProperty(Actor::Property::SCALE).Get<Vector3>(), scale);

    if(camParams.isPerspective)
    {
      DALI_TEST_EQUAL(camera.GetProperty(Dali::CameraActor::Property::FIELD_OF_VIEW).Get<float>(), Radian(Degree(camParams.yFov)).radian);
    }
    else
    {
      DALI_TEST_EQUAL(camera.GetProperty(Dali::DevelCameraActor::Property::ORTHOGRAPHIC_SIZE).Get<float>(), camParams.orthographicSize);
    }
  }

  END_TEST;
}

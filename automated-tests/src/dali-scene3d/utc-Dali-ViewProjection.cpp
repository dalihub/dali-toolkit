/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

#include "dali-scene3d/public-api/loader/view-projection.h"
#include <dali-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Scene3D::Loader;

namespace
{
void SetPerspectiveProjection(Matrix& m)
{
  auto data = m.AsFloat();
  float near = 1.f;
  float far = 10.f;
  float right = 1.f;
  float left = -right;
  float top = .75f;
  float bottom = -top;
  data[0] = 2.f * near / (right - left);
  data[5] = 2.f * near / (top - bottom);
  data[8] = (right + left) / (right - left);
  data[9] = (top + bottom) / (top - bottom);
  data[10] = (far + near) / (far - near);
  data[11] = -1.f;
  data[14] = 2 * far * near / (far - near);
}

}

int UtcDaliViewProjection(void)
{
  ViewProjection vp;
  Matrix viewMatrix;
  viewMatrix.SetTransformComponents( Vector3::ONE,
    Quaternion(Radian(Degree(90.f)), Vector3::YAXIS),
    Vector3::XAXIS * 200.f);
  vp.GetView() = viewMatrix;

  Matrix projectionMatrix;
  SetPerspectiveProjection(projectionMatrix);
  vp.GetProjection() = projectionMatrix;

  vp.Update();

  [&](const ViewProjection& vp) {
    DALI_TEST_EQUAL(vp.GetView(), viewMatrix);
    DALI_TEST_EQUAL(vp.GetProjection(), projectionMatrix);
  }(vp);

  Matrix expectedViewProjection{ false };
  Matrix::Multiply(expectedViewProjection, viewMatrix, projectionMatrix);
  auto viewProjectionResult = vp.GetViewProjection();
  DALI_TEST_EQUAL(viewProjectionResult, expectedViewProjection);

  Matrix expectedInverseProjection{ projectionMatrix };
  expectedInverseProjection.Invert();
  auto inverseProjectionResult = vp.GetInverseProjection();
  DALI_TEST_EQUAL(inverseProjectionResult, expectedInverseProjection);

  END_TEST;
}

int UtcDaliViewProjectionUpdateFail(void)
{
  ViewProjection vp;
  DALI_TEST_ASSERTION(vp.Update(), "Failed to find inverse");

  END_TEST;
}

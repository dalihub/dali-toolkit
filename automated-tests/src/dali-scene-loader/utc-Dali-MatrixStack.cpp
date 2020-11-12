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

#include "dali-scene-loader/public-api/matrix-stack.h"
#include <dali-test-suite-utils.h>
#include <string_view>

using namespace Dali;
using namespace Dali::SceneLoader;

int UtcDaliMatrixStack(void)
{
  MatrixStack stack;
  DALI_TEST_CHECK(stack.IsEmpty());

  Matrix testMatrix;
  testMatrix.SetTransformComponents(Vector3::ONE * 5.f,
    Quaternion(Radian(Degree(-45.f)), Vector3::XAXIS),
    Vector3(100.f, 0.f, -200.f));

  stack.Push(testMatrix);
  DALI_TEST_CHECK(!stack.IsEmpty());
  DALI_TEST_EQUAL(stack.Top(), testMatrix);

  stack.Push(Matrix::IDENTITY);
  DALI_TEST_CHECK(!stack.IsEmpty());
  DALI_TEST_EQUAL(stack.Top(), testMatrix);

  stack.Push(testMatrix);

  Matrix testMatrix2;
  Matrix::Multiply(testMatrix2, testMatrix, testMatrix);
  DALI_TEST_CHECK(!stack.IsEmpty());
  DALI_TEST_EQUAL(stack.Top(), testMatrix2);

  stack.PopAll();
  DALI_TEST_CHECK(stack.IsEmpty());

  stack.Push(Matrix::IDENTITY);
  DALI_TEST_EQUAL(stack.Top(), Matrix::IDENTITY);

  stack.Push(testMatrix);
  DALI_TEST_EQUAL(stack.Top(), testMatrix);

  stack.Pop();
  DALI_TEST_EQUAL(stack.Top(), Matrix::IDENTITY);
  DALI_TEST_CHECK(!stack.IsEmpty());

  stack.Pop();
  DALI_TEST_CHECK(stack.IsEmpty());

  END_TEST;
}

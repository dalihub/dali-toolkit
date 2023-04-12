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
 *
 */

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <stdlib.h>
#include <iostream>

#include <dali-scene3d/internal/model-components/model-primitive-impl.h>

using namespace Dali;
using namespace Dali::Toolkit;

void model_components_model_primitive_impl_startup(void)
{
  test_return_value = TET_UNDEF;
}

void model_components_model_primitive_impl_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
} // namespace

// Method test
int UtcDaliModelPrimitiveImplSetData(void)
{
  ToolkitTestApplication application;

  Scene3D::ModelPrimitive modelPrimitive = Scene3D::ModelPrimitive::New();

  tet_printf("Check primitive don't have material initial time\n");

  DALI_TEST_CHECK(!GetImplementation(modelPrimitive).GetRenderer());
  DALI_TEST_CHECK(!modelPrimitive.GetGeometry());
  DALI_TEST_CHECK(!modelPrimitive.GetMaterial());

  Dali::Geometry geometry = Dali::Geometry::New();
  Dali::Scene3D::Material material = Dali::Scene3D::Material::New();

  modelPrimitive.SetGeometry(geometry);
  modelPrimitive.SetMaterial(material);
  DALI_TEST_CHECK(geometry == modelPrimitive.GetGeometry());
  DALI_TEST_CHECK(material == modelPrimitive.GetMaterial());
  DALI_TEST_CHECK(GetImplementation(modelPrimitive).GetRenderer());

  END_TEST;
}
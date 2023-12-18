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

#include <dali-scene3d/public-api/model-components/model-primitive.h>

using namespace Dali;
using namespace Dali::Toolkit;

void model_components_model_primitive_startup(void)
{
  test_return_value = TET_UNDEF;
}

void model_components_model_primitive_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
} // namespace

// Positive test case for a method
int UtcDaliModelPrimitiveNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliModelPrimitiveNew");

  Scene3D::ModelPrimitive modelPrimitive = Scene3D::ModelPrimitive::New();
  DALI_TEST_CHECK(modelPrimitive);
  END_TEST;
}

// Positive test case for a method
int UtcDaliModelPrimitiveDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliModelPrimitiveDownCast");

  Scene3D::ModelPrimitive modelPrimitive = Scene3D::ModelPrimitive::New();
  BaseHandle              handle(modelPrimitive);

  Scene3D::ModelPrimitive modelPrimitive2 = Scene3D::ModelPrimitive::DownCast(handle);
  DALI_TEST_CHECK(modelPrimitive);
  DALI_TEST_CHECK(modelPrimitive2);
  DALI_TEST_CHECK(modelPrimitive2 == modelPrimitive);
  END_TEST;
}

int UtcDaliModelPrimitiveTypeRegistry(void)
{
  ToolkitTestApplication application;

  TypeRegistry typeRegistry = TypeRegistry::Get();
  DALI_TEST_CHECK(typeRegistry);

  TypeInfo typeInfo = typeRegistry.GetTypeInfo("ModelPrimitive");
  DALI_TEST_CHECK(typeInfo);

  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK(handle);

  Scene3D::ModelPrimitive modelPrimitive = Scene3D::ModelPrimitive::DownCast(handle);
  DALI_TEST_CHECK(modelPrimitive);

  END_TEST;
}

int UtcDaliModelPrimitiveCopyAndAssignment(void)
{
  ToolkitTestApplication application;

  Scene3D::ModelPrimitive modelPrimitive = Scene3D::ModelPrimitive::New();
  DALI_TEST_CHECK(modelPrimitive);

  Scene3D::ModelPrimitive copy(modelPrimitive);
  DALI_TEST_CHECK(modelPrimitive == copy);

  Scene3D::ModelPrimitive assign;
  DALI_TEST_CHECK(!assign);

  assign = copy;
  DALI_TEST_CHECK(assign == modelPrimitive);

  END_TEST;
}

int UtcDaliModelPrimitiveMoveConstructor(void)
{
  ToolkitTestApplication application;

  Scene3D::ModelPrimitive modelPrimitive = Scene3D::ModelPrimitive::New();
  DALI_TEST_EQUALS(1, modelPrimitive.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  Scene3D::ModelPrimitive moved = std::move(modelPrimitive);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!modelPrimitive);

  END_TEST;
}

int UtcDaliModelPrimitiveMoveAssignment(void)
{
  ToolkitTestApplication application;

  Scene3D::ModelPrimitive modelPrimitive = Scene3D::ModelPrimitive::New();
  DALI_TEST_EQUALS(1, modelPrimitive.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  Scene3D::ModelPrimitive moved;
  moved = std::move(modelPrimitive);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  END_TEST;
}

// Method test
int UtcDaliModelPrimitiveSetGetGeometry(void)
{
  ToolkitTestApplication application;

  Scene3D::ModelPrimitive modelPrimitive = Scene3D::ModelPrimitive::New();

  tet_printf("Check primitive don't have renderer initial time\n");

  DALI_TEST_CHECK(!modelPrimitive.GetGeometry());

  Dali::Geometry geometry = Dali::Geometry::New();

  modelPrimitive.SetGeometry(geometry);
  DALI_TEST_CHECK(geometry == modelPrimitive.GetGeometry());

  tet_printf("Replace as new Renderer\n");

  Dali::Geometry geometry2 = Dali::Geometry::New();

  modelPrimitive.SetGeometry(geometry2);
  DALI_TEST_CHECK(geometry != modelPrimitive.GetGeometry());
  DALI_TEST_CHECK(geometry2 == modelPrimitive.GetGeometry());
  END_TEST;
}

int UtcDaliModelPrimitiveSetGetMaterial(void)
{
  ToolkitTestApplication application;

  Scene3D::ModelPrimitive modelPrimitive = Scene3D::ModelPrimitive::New();

  tet_printf("Check primitive don't have material initial time\n");

  DALI_TEST_CHECK(!modelPrimitive.GetMaterial());

  Dali::Scene3D::Material material = Dali::Scene3D::Material::New();

  modelPrimitive.SetMaterial(material);
  DALI_TEST_CHECK(material == modelPrimitive.GetMaterial());

  tet_printf("Replace as new Material\n");

  Dali::Scene3D::Material material2 = Dali::Scene3D::Material::New();

  modelPrimitive.SetMaterial(material2);
  DALI_TEST_CHECK(material != modelPrimitive.GetMaterial());
  DALI_TEST_CHECK(material2 == modelPrimitive.GetMaterial());

  END_TEST;
}

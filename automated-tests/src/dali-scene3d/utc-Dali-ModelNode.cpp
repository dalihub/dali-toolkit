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

#include <dali-scene3d/public-api/model-components/model-node.h>

using namespace Dali;
using namespace Dali::Toolkit;

void model_components_model_node_startup(void)
{
  test_return_value = TET_UNDEF;
}

void model_components_model_node_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
} // namespace

// Negative test case for a method
int UtcDaliModelNodeUninitialized(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliModelNodeUninitialized");

  Scene3D::ModelNode modelNode;

  try
  {
    // New() must be called to create a ModelNode or it wont be valid.
    Actor a = Actor::New();
    modelNode.Add(a);
    DALI_TEST_CHECK(false);
  }
  catch(Dali::DaliException& e)
  {
    // Tests that a negative test of an assertion succeeds
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_CHECK(!modelNode);
  }
  END_TEST;
}

// Positive test case for a method
int UtcDaliModelNodeNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliModelNodeNew");

  Scene3D::ModelNode modelNode = Scene3D::ModelNode::New();
  DALI_TEST_CHECK(modelNode);
  END_TEST;
}

// Positive test case for a method
int UtcDaliModelNodeDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliModelNodeDownCast");

  Scene3D::ModelNode modelNode = Scene3D::ModelNode::New();
  BaseHandle         handle(modelNode);

  Scene3D::ModelNode modelNode2 = Scene3D::ModelNode::DownCast(handle);
  DALI_TEST_CHECK(modelNode);
  DALI_TEST_CHECK(modelNode2);
  DALI_TEST_CHECK(modelNode2 == modelNode);
  END_TEST;
}

int UtcDaliModelNodeTypeRegistry(void)
{
  ToolkitTestApplication application;

  TypeRegistry typeRegistry = TypeRegistry::Get();
  DALI_TEST_CHECK(typeRegistry);

  TypeInfo typeInfo = typeRegistry.GetTypeInfo("ModelNode");
  DALI_TEST_CHECK(typeInfo);

  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK(handle);

  Scene3D::ModelNode modelNode = Scene3D::ModelNode::DownCast(handle);
  DALI_TEST_CHECK(modelNode);

  END_TEST;
}

// Positive test case for a method
int UtcDaliModelNodeAddRemove(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliModelAddRemove");

  Scene3D::ModelNode modelNode = Scene3D::ModelNode::New();
  DALI_TEST_CHECK(modelNode);

  Actor actor = Actor::New();
  DALI_TEST_CHECK(!actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));

  modelNode.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  modelNode.SetProperty(Actor::Property::SIZE, application.GetScene().GetSize());
  modelNode.Add(actor);
  application.GetScene().Add(modelNode);

  DALI_TEST_CHECK(actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));

  modelNode.Remove(actor);

  DALI_TEST_CHECK(!actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));
  END_TEST;
}

int UtcDaliModelNodeCopyAndAssignment(void)
{
  ToolkitTestApplication application;

  Scene3D::ModelNode modelNode = Scene3D::ModelNode::New();
  DALI_TEST_CHECK(modelNode);

  Scene3D::ModelNode copy(modelNode);
  DALI_TEST_CHECK(modelNode == copy);

  Scene3D::ModelNode assign;
  DALI_TEST_CHECK(!assign);

  assign = copy;
  DALI_TEST_CHECK(assign == modelNode);

  END_TEST;
}

int UtcDaliModelNodeMoveConstructor(void)
{
  ToolkitTestApplication application;

  Scene3D::ModelNode modelNode = Scene3D::ModelNode::New();
  DALI_TEST_EQUALS(1, modelNode.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  modelNode.SetProperty(Actor::Property::SENSITIVE, false);
  DALI_TEST_CHECK(false == modelNode.GetProperty<bool>(Actor::Property::SENSITIVE));

  Scene3D::ModelNode moved = std::move(modelNode);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(false == moved.GetProperty<bool>(Actor::Property::SENSITIVE));
  DALI_TEST_CHECK(!modelNode);

  END_TEST;
}

int UtcDaliModelNodeMoveAssignment(void)
{
  ToolkitTestApplication application;

  Scene3D::ModelNode modelNode = Scene3D::ModelNode::New();
  DALI_TEST_EQUALS(1, modelNode.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  modelNode.SetProperty(Actor::Property::SENSITIVE, false);
  DALI_TEST_CHECK(false == modelNode.GetProperty<bool>(Actor::Property::SENSITIVE));

  Scene3D::ModelNode moved;
  moved = std::move(modelNode);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(false == moved.GetProperty<bool>(Actor::Property::SENSITIVE));
  DALI_TEST_CHECK(!modelNode);

  END_TEST;
}

int UtcDaliModelNodeOnSizeSet(void)
{
  ToolkitTestApplication application;

  Scene3D::ModelNode modelNode = Scene3D::ModelNode::New();

  application.GetScene().Add(modelNode);

  application.SendNotification();
  application.Render();

  Vector2 size(200.0f, 300.0f);
  modelNode.SetProperty(Actor::Property::SIZE, size);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(modelNode.GetCurrentProperty<Vector2>(Actor::Property::SIZE), size, TEST_LOCATION);

  END_TEST;
}

// Method test

int UtcDaliModelNodeAddRemovePrimitive(void)
{
  tet_infoline(" UtcDaliModelNodeAddPrimitive.");

  ToolkitTestApplication application;

  Scene3D::ModelNode modelNode = Scene3D::ModelNode::New();

  uint32_t expect = 0u;

  tet_printf("Test empty primitive case\n");

  DALI_TEST_EQUALS(expect, modelNode.GetModelPrimitiveCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!modelNode.GetModelPrimitive(0u));
  DALI_TEST_CHECK(!modelNode.GetModelPrimitive(1u));

  Scene3D::ModelPrimitive primitive1 = Scene3D::ModelPrimitive::New();
  Scene3D::ModelPrimitive primitive2 = Scene3D::ModelPrimitive::New();

  Scene3D::Material material = Scene3D::Material::New();

  primitive1.SetMaterial(material);
  primitive2.SetMaterial(material);

  tet_printf("Test primitive appended during off scene\n");

  modelNode.AddModelPrimitive(primitive1);
  ++expect;
  DALI_TEST_EQUALS(expect, modelNode.GetModelPrimitiveCount(), TEST_LOCATION);
  DALI_TEST_CHECK(primitive1 == modelNode.GetModelPrimitive(0u));
  DALI_TEST_CHECK(!modelNode.GetModelPrimitive(1u));

  tet_printf("Test primitive appended during on scene\n");

  application.GetScene().Add(modelNode);

  modelNode.AddModelPrimitive(primitive2);
  ++expect;
  DALI_TEST_EQUALS(expect, modelNode.GetModelPrimitiveCount(), TEST_LOCATION);
  DALI_TEST_CHECK(primitive1 == modelNode.GetModelPrimitive(0u));
  DALI_TEST_CHECK(primitive2 == modelNode.GetModelPrimitive(1u));

  modelNode.RemoveModelPrimitive(primitive1);
  DALI_TEST_CHECK(primitive2 == modelNode.GetModelPrimitive(0u));
  DALI_TEST_CHECK(!modelNode.GetModelPrimitive(1u));

  modelNode.RemoveModelPrimitive(0u);
  DALI_TEST_CHECK(!modelNode.GetModelPrimitive(0u));
  DALI_TEST_CHECK(!modelNode.GetModelPrimitive(1u));

  modelNode.Unparent();

  END_TEST;
}

int UtcDaliModelNodeFindChildModelNodeByName(void)
{
  tet_infoline(" UtcDaliModelNodeFindChildModelNodeByName.");

  ToolkitTestApplication application;

  Scene3D::ModelNode modelNode = Scene3D::ModelNode::New();
  Scene3D::ModelNode modelNode1 = Scene3D::ModelNode::New();
  Scene3D::ModelNode modelNode2 = Scene3D::ModelNode::New();

  modelNode1.SetProperty(Dali::Actor::Property::NAME, "modelNode1");
  modelNode2.SetProperty(Dali::Actor::Property::NAME, "modelNode2");
  modelNode.Add(modelNode1);
  modelNode.Add(modelNode2);

  Scene3D::ModelNode child1 = modelNode.FindChildModelNodeByName("modelNode1");
  DALI_TEST_CHECK(child1);
  DALI_TEST_EQUALS(child1, modelNode1, TEST_LOCATION);

  Scene3D::ModelNode child2 = modelNode.FindChildModelNodeByName("modelNode2");
  DALI_TEST_CHECK(child2);
  DALI_TEST_EQUALS(child2, modelNode2, TEST_LOCATION);

  END_TEST;
}
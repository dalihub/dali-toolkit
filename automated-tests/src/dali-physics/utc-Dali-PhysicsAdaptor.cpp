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

#include <stdlib.h>
#include <iostream>
#include <typeinfo>

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-physics/dali-physics.h>
#include <dali-toolkit-test-suite-utils.h>
#include <toolkit-event-thread-callback.h>

#include <dali-toolkit/devel-api/controls/alignment/alignment.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/events/hit-test-algorithm.h>

#include <bullet/btBulletDynamicsCommon.h>

using namespace Dali;
using namespace Dali::Toolkit::Physics;

void utc_dali_physics3d_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_physics3d_cleanup(void)
{
  test_return_value = TET_PASS;
}

btRigidBody* CreateBody(btDiscreteDynamicsWorld* bulletWorld)
{
  btSphereShape* ball = new btSphereShape(30);
  btVector3      localInertia(0.f, 0.f, 0.f);
  ball->calculateLocalInertia(10, localInertia);
  btTransform transform;
  transform.setIdentity();
  auto* motionState = new btDefaultMotionState(transform);

  btRigidBody::btRigidBodyConstructionInfo ci(10, motionState, ball, localInertia);

  btRigidBody* body = new btRigidBody(ci);
  body->setFriction(0.5f);
  body->setRestitution(0.5f);
  bulletWorld->addRigidBody(body);
  return body;
}

int UtcDaliPhysicsCreateAdaptorP1(void)
{
  ToolkitTestApplication application;

  Matrix     transform(true);
  Uint16Pair size(640, 480);

  PhysicsAdaptor handle = PhysicsAdaptor::New(transform, size);
  DALI_TEST_CHECK(handle);

  END_TEST;
}

int UtcDaliPhysicsCreateAdaptorN1(void)
{
  ToolkitTestApplication application;

  PhysicsAdaptor handle;
  DALI_TEST_CHECK(!handle);

  END_TEST;
}

int UtcDaliPhysicsDowncastP1(void)
{
  ToolkitTestApplication application;

  Matrix     transform(true);
  Uint16Pair size(640, 480);

  BaseHandle handle = PhysicsAdaptor::New(transform, size);

  auto adaptor = PhysicsAdaptor::DownCast(handle);
  DALI_TEST_CHECK(adaptor);
  //Following only works if type is registered
  //DALI_TEST_EQUALS("PhysicsAdaptor", adaptor.GetTypeName(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliPhysicsDowncastN1(void)
{
  BaseHandle handle;
  auto       adaptor = PhysicsAdaptor::DownCast(handle);
  DALI_TEST_CHECK(!adaptor);

  DALI_TEST_CHECK(typeid(PhysicsAdaptor) == typeid(decltype(adaptor)));
  END_TEST;
}

int UtcDaliPhysicsAdaptorMoveConstructor(void)
{
  ToolkitTestApplication application;
  tet_infoline("Testing the move constructor");

  Matrix         transform(true);
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);
  DALI_TEST_CHECK(adaptor);

  PhysicsAdaptor moved = std::move(adaptor);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!adaptor);
  DALI_TEST_CHECK(moved != adaptor);

  END_TEST;
}

int UtcDaliPhysicsAdaptorCopyConstructor(void)
{
  ToolkitTestApplication application;
  tet_infoline("Testing the move constructor");

  Matrix         transform(true);
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);

  DALI_TEST_CHECK(adaptor);

  PhysicsAdaptor altAdaptor = adaptor;
  DALI_TEST_CHECK(altAdaptor);
  DALI_TEST_CHECK(adaptor);
  DALI_TEST_CHECK(altAdaptor == adaptor); // should point at same object

  END_TEST;
}

int UtcDaliPhysicsAdaptorCopyAssign(void)
{
  ToolkitTestApplication application;
  tet_infoline("Testing the copy assign");

  Matrix         transform(true);
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);
  DALI_TEST_CHECK(adaptor);

  PhysicsAdaptor altAdaptor = adaptor;
  DALI_TEST_CHECK(altAdaptor);
  DALI_TEST_CHECK(adaptor);
  DALI_TEST_CHECK(altAdaptor == adaptor); // should point at same object

  END_TEST;
}

int UtcDaliPhysicsAdaptorMoveAssignment(void)
{
  ToolkitTestApplication application;
  tet_infoline("Testing the move constructor");

  Matrix         transform(true);
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);
  DALI_TEST_CHECK(adaptor);

  PhysicsAdaptor moved;
  moved = std::move(adaptor);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!adaptor);

  END_TEST;
}

int UtcDaliPhysicsSetTimestep(void)
{
  ToolkitTestApplication application;

  Matrix     transform(true);
  Uint16Pair size(640, 480);

  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);
  adaptor.SetTimestep(1.0f / 60.0f);

  DALI_TEST_EQUALS(adaptor.GetTimestep(), 1.0f / 60.0f, 0.0001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysicsGetTimestep(void)
{
  ToolkitTestApplication application;

  Matrix     transform(true);
  Uint16Pair size(640, 480);

  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);
  adaptor.SetTimestep(1.0f / 60.0f);
  float timestep = adaptor.GetTimestep();
  float expected = 1.0f / 60.0f;
  DALI_TEST_EQUALS(timestep, expected, 0.0001f, TEST_LOCATION);

  adaptor.SetTimestep(1.0f / 120.0f);
  timestep = adaptor.GetTimestep();
  expected = 1.0f / 120.0f;
  DALI_TEST_EQUALS(timestep, expected, 0.0001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysicsGetPhysicsAccessorP1(void)
{
  ToolkitTestApplication application;

  Matrix     transform(true);
  Uint16Pair size(640, 480);

  PhysicsAdaptor                           adaptor  = PhysicsAdaptor::New(transform, size);
  PhysicsAdaptor::ScopedPhysicsAccessorPtr accessor = adaptor.GetPhysicsAccessor();
  DALI_TEST_CHECK(accessor.get() != nullptr);

  Dali::Any world = accessor->GetNative();
  DALI_TEST_CHECK(!world.Empty());

  END_TEST;
}

int UtcDaliPhysicsGetPhysicsAccessorN1(void)
{
  ToolkitTestApplication application;

  PhysicsAdaptor handle;
  DALI_TEST_CHECK(!handle);

  try
  {
    auto ptr = handle.GetPhysicsAccessor();
    DALI_TEST_CHECK(ptr == nullptr);

    tet_result(TET_FAIL);
  }
  catch(DaliException& e)
  {
    DALI_TEST_ASSERT(e, "Physics adaptor handle is empty", TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliPhysicsAdaptorGetRootActor(void)
{
  tet_infoline("Test that the root actor can be retrieved");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 2.0f));
  Uint16Pair     size(640, 480);
  auto           scene     = application.GetScene();
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  scene.Add(rootActor);

  DALI_TEST_CHECK(rootActor);
  DALI_TEST_EQUALS(rootActor.GetProperty<Vector2>(Actor::Property::SIZE), Vector2(640.0f, 480.0f), 0.001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysicsAdaptorCreateDebugLayer(void)
{
  ToolkitTestApplication application;
  Matrix                 transform(true);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 2.0f));
  Uint16Pair size(640, 480);
  auto       scene = application.GetScene();

  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  scene.Add(rootActor);
  Window window = DevelWindow::Get(rootActor);

  Layer layer = adaptor.CreateDebugLayer(window);
  DALI_TEST_CHECK(layer);

  adaptor.SetDebugState(PhysicsAdaptor::DebugState::ON);

  btRigidBody* body{nullptr};
  {
    auto accessor            = adaptor.GetPhysicsAccessor();
    auto bulletWorld         = accessor->GetNative().Get<btDiscreteDynamicsWorld*>();
    body                     = CreateBody(bulletWorld);
    Dali::Actor ballActor    = Toolkit::ImageView::New(TEST_RESOURCE_DIR "/gallery-small-1.jpg");
    auto        physicsActor = adaptor.AddActorBody(ballActor, body);
    physicsActor.AsyncSetPhysicsPosition(Vector3(0.f, 0.f, 0.f));
  }
  Test::WaitForEventThreadTrigger(1);
  application.SendNotification();
  application.Render();

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliPhysicsAdaptorTranslateToPhysicsSpace1(void)
{
  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 2.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);

  Vector3 a(30, 20, 10);
  Vector3 expected = a * 2.0f;
  DALI_TEST_EQUALS(adaptor.TranslateToPhysicsSpace(a), expected, 0.0001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysicsAdaptorTranslateToPhysicsSpace2(void)
{
  ToolkitTestApplication application;
  Matrix                 transform(false);
  tet_infoline("Test that using an alternative scale doesn't change rotation");
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 2.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);

  // Rotation shouldn't change under this scale
  Quaternion q(Degree(30.0f), Vector3::XAXIS);
  DALI_TEST_EQUALS(adaptor.TranslateToPhysicsSpace(q), q, 0.0001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysicsAdaptorTranslateToPhysicsSpace3(void)
{
  ToolkitTestApplication application;
  Matrix                 transform(false);
  tet_infoline("Test that using an inverted Y scale also inverts quaternions");

  transform.SetIdentityAndScale(Vector3(1.0f, -1.0f, 1.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);

  Quaternion q(Degree(30.0f), Vector3::ZAXIS);
  Quaternion qp(Degree(-30.0f), Vector3::ZAXIS); // We have mirrored along Y axis, so Z rot is opposite.

  DALI_TEST_EQUALS(adaptor.TranslateToPhysicsSpace(q), qp, 0.0001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysicsAdaptorTranslateToPhysicsSpace4(void)
{
  ToolkitTestApplication application;
  Matrix                 transform(false);
  tet_infoline("Test that using an inverted Y scale also inverts quaternions");

  transform.SetIdentityAndScale(Vector3(1.0f, -1.0f, 1.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);

  Quaternion q(Degree(30.0f), Vector3::XAXIS);
  Quaternion qp(Degree(-30.0f), Vector3::XAXIS); // We have mirrored along Y axis, so Z rot is opposite.

  DALI_TEST_EQUALS(adaptor.TranslateToPhysicsSpace(q), qp, 0.0001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysicsAdaptorTranslateToPhysicsSpace5(void)
{
  ToolkitTestApplication application;
  Matrix                 transform(false);
  tet_infoline("Test that using an inverted Y scale also inverts quaternions, except along Y axis");

  transform.SetIdentityAndScale(Vector3(1.0f, -1.0f, 1.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);

  Quaternion q(Degree(30.0f), Vector3::YAXIS);
  Quaternion qp(Degree(30.0f), Vector3::YAXIS);

  DALI_TEST_EQUALS(adaptor.TranslateToPhysicsSpace(q), qp, 0.0001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysicsAdaptorTranslateFromPhysicsSpace1(void)
{
  ToolkitTestApplication application;
  Matrix                 transform(false);
  tet_infoline("Test that using a double scale halves position");

  transform.SetIdentityAndScale(Vector3(2.0f, -2.0f, 2.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);

  Vector3 position(20.0f, 20.0f, 0.0f);
  Vector3 expected(10.0f, -10.0f, 0.0f);

  DALI_TEST_EQUALS(adaptor.TranslateFromPhysicsSpace(position), expected, 0.0001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysicsAdaptorConvertVectorToPhysicsSpace01(void)
{
  ToolkitTestApplication application;
  Matrix                 transform(false);
  tet_infoline("Test that using a translation does not translate vector");

  transform.SetIdentityAndScale(Vector3(1.0f, 1.0f, 1.0f));
  transform.SetTranslation(Vector3(0.0f, 100.0f, 0.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);
  Vector3        vector(20.0f, 20.0f, 0.0f);
  DALI_TEST_EQUALS(adaptor.ConvertVectorToPhysicsSpace(vector), vector, 0.0001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysicsAdaptorConvertVectorToPhysicsSpace02(void)
{
  ToolkitTestApplication application;
  Matrix                 transform(false);
  tet_infoline("Test that using a translation with inverse Y does not translate vector");

  transform.SetIdentityAndScale(Vector3(1.0f, -1.0f, 1.0f));
  transform.SetTranslation(Vector3(0.0f, 100.0f, 0.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);
  Vector3        vector(20.0f, 20.0f, 0.0f);
  Vector3        expected(20.0f, -20.0f, 0.0f);
  DALI_TEST_EQUALS(adaptor.ConvertVectorToPhysicsSpace(vector), expected, 0.0001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysicsAdaptorConvertVectorFromPhysicsSpace01(void)
{
  ToolkitTestApplication application;
  Matrix                 transform(false);
  tet_infoline("Test that using a translation does not translate vector");

  transform.SetIdentityAndScale(Vector3(1.0f, 1.0f, 1.0f));
  transform.SetTranslation(Vector3(0.0f, 100.0f, 0.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);
  Vector3        vector(20.0f, 20.0f, 0.0f);
  DALI_TEST_EQUALS(adaptor.ConvertVectorFromPhysicsSpace(vector), vector, 0.0001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysicsAdaptorConvertVectorFromPhysicsSpace02(void)
{
  ToolkitTestApplication application;
  Matrix                 transform(false);
  tet_infoline("Test that using a translation with inverse Y does not translate vector");

  transform.SetIdentityAndScale(Vector3(1.0f, -1.0f, 1.0f));
  transform.SetTranslation(Vector3(0.0f, 100.0f, 0.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);
  Vector3        vector(20.0f, 20.0f, 0.0f);
  Vector3        expected(20.0f, -20.0f, 0.0f);
  DALI_TEST_EQUALS(adaptor.ConvertVectorFromPhysicsSpace(vector), expected, 0.0001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysicsAdaptorSetTransformAndSize(void)
{
  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 2.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);

  Vector3 a(30, 20, 10);
  Vector3 expected = a * 2.0f;
  DALI_TEST_EQUALS(adaptor.TranslateToPhysicsSpace(a), expected, 0.0001f, TEST_LOCATION);

  transform.SetIdentityAndScale(Vector3(1.0f, -1.0f, 1.0f));
  transform.SetTranslation(Vector3(0.0f, 100.0f, 0.0f));
  adaptor.SetTransformAndSize(transform, size);

  Vector3 expect2(30, 80, 10);
  DALI_TEST_EQUALS(adaptor.TranslateToPhysicsSpace(a), expect2, 0.0001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysicsAdaptorSetIntegrationState(void)
{
  tet_infoline("Test that changing the integration state is reflected");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 2.0f));
  Uint16Pair     size(640, 480);
  auto           scene     = application.GetScene();
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  scene.Add(rootActor);

  DALI_TEST_CHECK(adaptor.GetIntegrationState() == PhysicsAdaptor::IntegrationState::ON);

  adaptor.SetIntegrationState(PhysicsAdaptor::IntegrationState::OFF);
  DALI_TEST_CHECK(adaptor.GetIntegrationState() == PhysicsAdaptor::IntegrationState::OFF);

  END_TEST;
}

int UtcDaliPhysicsAdaptorGetIntegrationState(void)
{
  tet_infoline("Test that changing the integration state is reflected");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 2.0f));
  Uint16Pair     size(640, 480);
  auto           scene     = application.GetScene();
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  scene.Add(rootActor);

  adaptor.SetIntegrationState(PhysicsAdaptor::IntegrationState::OFF);
  DALI_TEST_CHECK(adaptor.GetIntegrationState() == PhysicsAdaptor::IntegrationState::OFF);

  adaptor.SetIntegrationState(PhysicsAdaptor::IntegrationState::ON);
  DALI_TEST_CHECK(adaptor.GetIntegrationState() == PhysicsAdaptor::IntegrationState::ON);

  // Can't test actual integration step runs without adding actors - see utc-Dali-PhysicsActor.cpp.
  END_TEST;
}

int UtcDaliPhysicsAdaptorSetDebugState(void)
{
  tet_infoline("Test that changing the debug state is reflected");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 2.0f));
  Uint16Pair     size(640, 480);
  auto           scene     = application.GetScene();
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  scene.Add(rootActor);

  DALI_TEST_CHECK(adaptor.GetDebugState() == PhysicsAdaptor::DebugState::OFF);

  adaptor.SetDebugState(PhysicsAdaptor::DebugState::ON);
  DALI_TEST_CHECK(adaptor.GetDebugState() == PhysicsAdaptor::DebugState::ON);

  adaptor.SetDebugState(PhysicsAdaptor::DebugState::OFF);
  DALI_TEST_CHECK(adaptor.GetDebugState() == PhysicsAdaptor::DebugState::OFF);

  END_TEST;
}

int UtcDaliPhysicsAdaptorGetDebugState(void)
{
  tet_infoline("Test that changing the debug state is reflected");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 2.0f));
  Uint16Pair     size(640, 480);
  auto           scene     = application.GetScene();
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  scene.Add(rootActor);

  adaptor.SetDebugState(PhysicsAdaptor::DebugState::OFF);
  DALI_TEST_CHECK(adaptor.GetDebugState() == PhysicsAdaptor::DebugState::OFF);

  adaptor.SetDebugState(PhysicsAdaptor::DebugState::ON);
  DALI_TEST_CHECK(adaptor.GetDebugState() == PhysicsAdaptor::DebugState::ON);

  // Can't test actual debug step runs without adding actors - see utc-Dali-PhysicsActor.cpp.
  END_TEST;
}

int UtcDaliPhysicsAdaptorAddActorBody(void)
{
  tet_infoline("Test that an actor/body pair can be added");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 2.0f));
  Uint16Pair     size(640, 480);
  auto           scene     = application.GetScene();
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  scene.Add(rootActor);

  auto accessor    = adaptor.GetPhysicsAccessor();
  auto bulletWorld = accessor->GetNative().Get<btDiscreteDynamicsWorld*>();

  btRigidBody* body         = CreateBody(bulletWorld);
  Dali::Actor  ballActor    = Toolkit::ImageView::New("gallery-small-1.jpg");
  auto         physicsActor = adaptor.AddActorBody(ballActor, body);

  DALI_TEST_CHECK(physicsActor);
  int id = ballActor[Actor::Property::ID];

  DALI_TEST_EQUALS(physicsActor.GetId(), id, TEST_LOCATION);
  DALI_TEST_EQUALS(physicsActor.GetBody().Get<btRigidBody*>(), body, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysicsAdaptorGetPhysicsActor(void)
{
  tet_infoline("Test that an actor/body pair can be retrieved");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 2.0f));
  Uint16Pair     size(640, 480);
  auto           scene     = application.GetScene();
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  scene.Add(rootActor);

  auto accessor    = adaptor.GetPhysicsAccessor();
  auto bulletWorld = accessor->GetNative().Get<btDiscreteDynamicsWorld*>();

  btRigidBody* body         = CreateBody(bulletWorld);
  Dali::Actor  ballActor    = Toolkit::ImageView::New("gallery-small-1.jpg");
  auto         physicsActor = adaptor.AddActorBody(ballActor, body);

  DALI_TEST_CHECK(physicsActor);

  PhysicsActor testActor = adaptor.GetPhysicsActor(body);
  DALI_TEST_CHECK(testActor);
  DALI_TEST_CHECK(physicsActor == testActor);

  END_TEST;
}

int UtcDaliPhysicsAdaptorBuildPickingRay(void)
{
  tet_infoline("Test that a touch can be converted to a picking ray");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 2.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);

  Vector3 from, to;
  adaptor.BuildPickingRay(Vector3(), -Vector3::ZAXIS, from, to);

  DALI_TEST_EQUALS(from, Vector3(), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(to, Vector3(0.0f, 0.0f, -20000.0f), 0.001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysicsAdaptorProjectPoint(void)
{
  tet_infoline("Test that a point is projected into physics space");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 2.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);

  // distance is in physics units, not DALi units!
  Vector3 projectedPoint = adaptor.ProjectPoint(Vector3(), -Vector3::ZAXIS, 200);

  DALI_TEST_EQUALS(projectedPoint, Vector3(0.0f, 0.0f, -200.0f), 0.001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysicsAdaptorQueue(void)
{
  tet_infoline("Test that Queue and CreateSyncPoint both work");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 2.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  auto           scene     = application.GetScene();
  scene.Add(rootActor);

  btRigidBody* body{nullptr};
  {
    auto accessor            = adaptor.GetPhysicsAccessor();
    auto bulletWorld         = accessor->GetNative().Get<btDiscreteDynamicsWorld*>();
    body                     = CreateBody(bulletWorld);
    Dali::Actor ballActor    = Toolkit::ImageView::New("gallery-small-1.jpg");
    auto        physicsActor = adaptor.AddActorBody(ballActor, body);
  }

  tet_infoline("Test that Queue works without accessor");
  adaptor.Queue([body]() {
    body->getWorldTransform().setOrigin(btVector3(100.0f, 20.0f, 20.0f));
  });
  adaptor.CreateSyncPoint();

  application.SendNotification();
  application.Render();
  // Should trigger an Update

  {
    auto accessor = adaptor.GetPhysicsAccessor();

    btVector3 origin = body->getWorldTransform().getOrigin();
    DALI_TEST_EQUALS(origin.x(), 100.0f, 0.001f, TEST_LOCATION);
    DALI_TEST_EQUALS(origin.y(), 20.0f, 0.001f, TEST_LOCATION);
    DALI_TEST_EQUALS(origin.z(), 20.0f, 0.001f, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliPhysicsAdaptorCreateSyncPoint(void)
{
  tet_infoline("Test that a delayed CreateSyncPoint delays update");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 2.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  auto           scene     = application.GetScene();
  scene.Add(rootActor);

  btRigidBody* body{nullptr};
  {
    auto accessor            = adaptor.GetPhysicsAccessor();
    auto bulletWorld         = accessor->GetNative().Get<btDiscreteDynamicsWorld*>();
    body                     = CreateBody(bulletWorld);
    Dali::Actor ballActor    = Toolkit::ImageView::New("gallery-small-1.jpg");
    auto        physicsActor = adaptor.AddActorBody(ballActor, body);

    tet_infoline("Test that Queue works with accessor");
    adaptor.Queue([body]() {
      body->getWorldTransform().setOrigin(btVector3(100.0f, 20.0f, 20.0f));
    });
  }

  // Should trigger an Update without processing queue
  application.SendNotification();
  application.Render();

  {
    auto accessor = adaptor.GetPhysicsAccessor();

    btVector3 origin = body->getWorldTransform().getOrigin();
    DALI_TEST_EQUALS(origin.x(), 0.0f, 0.001f, TEST_LOCATION);
    DALI_TEST_EQUALS(origin.y(), 0.0f, 0.001f, TEST_LOCATION);
    DALI_TEST_EQUALS(origin.z(), 0.0f, 0.001f, TEST_LOCATION);
  }

  // Should now execute queue
  adaptor.CreateSyncPoint();
  application.SendNotification();
  application.Render();

  {
    auto accessor = adaptor.GetPhysicsAccessor();

    btVector3 origin = body->getWorldTransform().getOrigin();
    DALI_TEST_EQUALS(origin.x(), 100.0f, 0.001f, TEST_LOCATION);
    DALI_TEST_EQUALS(origin.y(), 20.0f, 0.001f, TEST_LOCATION);
    DALI_TEST_EQUALS(origin.z(), 20.0f, 0.001f, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliPhysicsAdaptorHitTestP(void)
{
  tet_infoline("Test that hit testing finds a body");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(1.0f, 1.0f, 1.0f));
  Uint16Pair     size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT);
  const Vector2  center(TestApplication::DEFAULT_SURFACE_WIDTH * 0.5f, TestApplication::DEFAULT_SURFACE_HEIGHT * 0.5f);
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  auto           scene     = application.GetScene();
  scene.Add(rootActor);

  {
    auto         accessor    = adaptor.GetPhysicsAccessor(); // Prevent integration
    auto         bulletWorld = accessor->GetNative().Get<btDiscreteDynamicsWorld*>();
    Dali::Actor  ballActor   = Toolkit::ImageView::New(TEST_RESOURCE_DIR "/gallery-small-1.jpg");
    btRigidBody* body        = CreateBody(bulletWorld);
    body->getWorldTransform().setOrigin(btVector3(0.f, 0.f, 0.f));

    ballActor[Actor::Property::PARENT_ORIGIN] = ParentOrigin::CENTER;
    ballActor[Actor::Property::ANCHOR_POINT]  = AnchorPoint::CENTER;

    auto physicsActor = adaptor.AddActorBody(ballActor, body);
  }
  Test::WaitForEventThreadTrigger(1);

  // Should trigger an Update without processing queue
  application.SendNotification();
  application.Render();

  Vector3 origin, direction;
  Dali::HitTestAlgorithm::BuildPickingRay(scene.GetRenderTaskList().GetTask(0), center, origin, direction);
  Vector3 from, to;
  adaptor.BuildPickingRay(origin, direction, from, to); // Hit test centre of screen

  {
    auto    accessor = adaptor.GetPhysicsAccessor();
    Vector3 localPivot;
    float   distanceFromCamera;
    auto    body = accessor->HitTest(from, to, localPivot, distanceFromCamera);

    DALI_TEST_CHECK(!body.Empty());
  }

  END_TEST;
}

// todo:
// Hit Test
// PhysicsDebugRenderer.... Elide?!

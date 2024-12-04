/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <test-addon-manager.h>
#include <toolkit-event-thread-callback.h>

#include <dali-toolkit/devel-api/controls/alignment/alignment.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/events/hit-test-algorithm.h>

#include <chipmunk/chipmunk.h>

using namespace Dali;
using namespace Dali::Toolkit::Physics;

void utc_dali_physics2d_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_physics2d_cleanup(void)
{
  test_return_value = TET_PASS;
}

cpBody* CreateBody(cpSpace* space)
{
  const float BALL_MASS       = 10.0f;
  const float BALL_RADIUS     = 26.0f;
  const float BALL_ELASTICITY = 0.5f;
  const float BALL_FRICTION   = 0.5f;

  cpBody* body = cpSpaceAddBody(space, cpBodyNew(BALL_MASS, cpMomentForCircle(BALL_MASS, 0.0f, BALL_RADIUS, cpvzero)));

  cpShape* shape = cpSpaceAddShape(space, cpCircleShapeNew(body, BALL_RADIUS, cpvzero));
  cpShapeSetElasticity(shape, BALL_ELASTICITY);
  cpShapeSetFriction(shape, BALL_FRICTION);

  return body;
}

// Defines a PolyShape
cpBody* CreateHexBody(cpSpace* space)
{
  const float MASS       = 10.0f;
  const float RADIUS     = 26.0f;
  const float ELASTICITY = 0.5f;
  const float FRICTION   = 0.5f;

  cpVect hexagon[6];
  for(int i = 0; i < 6; i++)
  {
    cpFloat angle = -CP_PI * 2.0f * i / 6.0f;
    hexagon[i]    = cpvmult(cpv(cos(angle), sin(angle)), RADIUS - 1.0f);
  }

  cpBody*  body  = cpSpaceAddBody(space, cpBodyNew(MASS, cpMomentForPoly(MASS, 6, hexagon, cpvzero, 0.0f)));
  cpShape* shape = cpSpaceAddShape(space, cpPolyShapeNew(body, 6, hexagon, cpTransformIdentity, 1.0f));

  cpShapeSetElasticity(shape, ELASTICITY);
  cpShapeSetFriction(shape, FRICTION);

  return body;
}

// Defines a SegmentShape
cpBody* CreateSegBody(cpSpace* space)
{
  const float MASS       = 10.0f;
  const float RADIUS     = 26.0f;
  const float ELASTICITY = 0.5f;
  const float FRICTION   = 0.5f;

  cpVect   a     = cpv(0, 100);
  cpVect   b     = cpv(100, 0);
  cpBody*  body  = cpSpaceAddBody(space, cpBodyNew(MASS, cpMomentForSegment(MASS, a, b, 0.0f)));
  cpShape* shape = cpSpaceAddShape(space, cpSegmentShapeNew(body, a, b, RADIUS));

  cpShapeSetElasticity(shape, ELASTICITY);
  cpShapeSetFriction(shape, FRICTION);

  return body;
}

int UtcDaliPhysics2DCreateAdaptorP1(void)
{
  ToolkitTestApplication application;

  Matrix     transform(true);
  Uint16Pair size(640, 480);

  PhysicsAdaptor handle = PhysicsAdaptor::New(transform, size);
  DALI_TEST_CHECK(handle);

  END_TEST;
}

int UtcDaliPhysics2DCreateAdaptorN1(void)
{
  ToolkitTestApplication application;

  PhysicsAdaptor handle;
  DALI_TEST_CHECK(!handle);

  END_TEST;
}

int UtcDaliPhysics2DDowncastP1(void)
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

int UtcDaliPhysics2DDowncastN1(void)
{
  BaseHandle handle;
  auto       adaptor = PhysicsAdaptor::DownCast(handle);
  DALI_TEST_CHECK(!adaptor);

  DALI_TEST_CHECK(typeid(PhysicsAdaptor) == typeid(decltype(adaptor)));
  END_TEST;
}

int UtcDaliPhysics2DAdaptorMoveConstructor(void)
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

int UtcDaliPhysics2DAdaptorCopyConstructor(void)
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

int UtcDaliPhysics2DAdaptorCopyAssign(void)
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

int UtcDaliPhysics2DAdaptorMoveAssignment(void)
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

int UtcDaliPhysics2DSetTimestep(void)
{
  ToolkitTestApplication application;

  Matrix     transform(true);
  Uint16Pair size(640, 480);

  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);
  adaptor.SetTimestep(1.0f / 60.0f);

  DALI_TEST_EQUALS(adaptor.GetTimestep(), 1.0f / 60.0f, 0.0001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysics2DGetTimestep(void)
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

int UtcDaliPhysics2DGetPhysicsAccessorP1(void)
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

int UtcDaliPhysics2DGetPhysicsAccessorN1(void)
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

int UtcDaliPhysics2DAdaptorGetRootActor(void)
{
  tet_infoline("Test that the root actor can be retrieved");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 1.0f));
  Uint16Pair     size(640, 480);
  auto           scene     = application.GetScene();
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  scene.Add(rootActor);

  DALI_TEST_CHECK(rootActor);
  DALI_TEST_EQUALS(rootActor.GetProperty<Vector2>(Actor::Property::SIZE), Vector2(640.0f, 480.0f), 0.001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysics2DAdaptorCreateDebugLayer(void)
{
  ToolkitTestApplication application;
  Test::AddOnManager::Initialize(); // DebugLayer requires GLES addon so initialize the manager

  Matrix transform(true);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 1.0f));
  Uint16Pair size(640, 480);
  auto       scene = application.GetScene();

  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  scene.Add(rootActor);
  Window window = DevelWindow::Get(rootActor);

  Layer layer = adaptor.CreateDebugLayer(window);
  DALI_TEST_CHECK(layer);

  adaptor.SetDebugState(PhysicsAdaptor::DebugState::ON);

  cpBody* body{nullptr};
  cpBody* body2{nullptr};
  cpBody* body3{nullptr};
  {
    auto accessor = adaptor.GetPhysicsAccessor();
    auto space    = accessor->GetNative().Get<cpSpace*>();

    body                     = CreateBody(space);
    Dali::Actor ballActor    = Toolkit::ImageView::New("gallery-small-1.jpg");
    auto        physicsActor = adaptor.AddActorBody(ballActor, body);
    cpBodySetPosition(body, cpv(0, 0));

    // Constraint should create a dot in debug
    cpBody* staticBody = cpSpaceGetStaticBody(space);
    cpSpaceAddConstraint(space, cpPivotJointNew(staticBody, body, cpv(10, 10)));

    body2                     = CreateHexBody(space);
    Dali::Actor ballActor2    = Toolkit::ImageView::New("gallery-small-1.jpg");
    auto        physicsActor2 = adaptor.AddActorBody(ballActor2, body2);
    cpBodySleep(body2);

    body3                     = CreateSegBody(space);
    Dali::Actor ballActor3    = Toolkit::ImageView::New("gallery-small-1.jpg");
    auto        physicsActor3 = adaptor.AddActorBody(ballActor3, body3);
  }
  Test::WaitForEventThreadTrigger(1);

  // Render - if it doesn't crash, great!
  application.SendNotification();
  application.Render();

  Uint16Pair size2(480, 640);
  adaptor.SetTransformAndSize(transform, size2);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliPhysics2DAdaptorCreateDebugLayerN(void)
{
  ToolkitTestApplication application;
  // DebugLayer requires GLES addon so don't initialize the manager so it's not loaded. This should cause us to throw.

  Matrix transform(true);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 1.0f));
  Uint16Pair size(640, 480);
  auto       scene = application.GetScene();

  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  scene.Add(rootActor);
  Window window = DevelWindow::Get(rootActor);

  try
  {
    Layer layer = adaptor.CreateDebugLayer(window);
    DALI_TEST_CHECK(false); // Should not get here.
  }
  catch(...)
  {
    DALI_TEST_CHECK(true);
  }

  END_TEST;
}

int UtcDaliPhysics2DAdaptorTranslateToPhysicsSpace1(void)
{
  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 1.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);

  Vector3 a(30, 20, 10);
  Vector3 expected(60, 40, 10);
  DALI_TEST_EQUALS(adaptor.TranslateToPhysicsSpace(a), expected, 0.0001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysics2DAdaptorTranslateToPhysicsSpace2(void)
{
  ToolkitTestApplication application;
  Matrix                 transform(false);
  tet_infoline("Test that using an alternative scale doesn't change rotation");
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 1.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);

  // Rotation shouldn't change under this scale
  Quaternion q(Degree(30.0f), Vector3::XAXIS);
  Quaternion expected(Degree(30.0f), Vector3::XAXIS);
  DALI_TEST_EQUALS(adaptor.TranslateToPhysicsSpace(q), expected, 0.0001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysics2DAdaptorTranslateToPhysicsSpace3(void)
{
  ToolkitTestApplication application;
  Matrix                 transform(false);
  tet_infoline("Test that using an inverted Y scale inverts rotation");

  transform.SetIdentityAndScale(Vector3(1.0f, -1.0f, 1.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);

  Quaternion q(Degree(30.0f), Vector3::ZAXIS);
  Quaternion expected(Degree(-30.0f), Vector3::ZAXIS);

  DALI_TEST_EQUALS(adaptor.TranslateToPhysicsSpace(q), expected, 0.0001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysics2DAdaptorTranslateFromPhysicsSpace1(void)
{
  ToolkitTestApplication application;
  Matrix                 transform(false);
  tet_infoline("Test that using a double scale halves position");

  transform.SetIdentityAndScale(Vector3(2.0f, -2.0f, 1.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);

  Vector3 position(20.0f, 20.0f, 0.0f);
  Vector3 expected(10.0f, -10.0f, 0.0f);

  DALI_TEST_EQUALS(adaptor.TranslateFromPhysicsSpace(position), expected, 0.0001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysics2DAdaptorConvertVectorToPhysicsSpace01(void)
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

int UtcDaliPhysics2DAdaptorConvertVectorToPhysicsSpace02(void)
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

int UtcDaliPhysics2DAdaptorConvertVectorFromPhysicsSpace01(void)
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

int UtcDaliPhysics2DAdaptorConvertVectorFromPhysicsSpace02(void)
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

int UtcDaliPhysics2DAdaptorSetTransformAndSize(void)
{
  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 1.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);

  Vector3 a(30, 20, 10);
  Vector3 expected(60, 40, 10);
  DALI_TEST_EQUALS(adaptor.TranslateToPhysicsSpace(a), expected, 0.0001f, TEST_LOCATION);

  transform.SetIdentityAndScale(Vector3(1.0f, -1.0f, 1.0f));
  transform.SetTranslation(Vector3(0.0f, 100.0f, 0.0f));
  adaptor.SetTransformAndSize(transform, size);

  Vector3 expect2(30, 80, 10);
  DALI_TEST_EQUALS(adaptor.TranslateToPhysicsSpace(a), expect2, 0.0001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysics2DAdaptorSetIntegrationState(void)
{
  tet_infoline("Test that changing the integration state is reflected");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 1.0f));
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

int UtcDaliPhysics2DAdaptorGetIntegrationState(void)
{
  tet_infoline("Test that changing the integration state is reflected");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 1.0f));
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

int UtcDaliPhysics2DAdaptorSetDebugState(void)
{
  tet_infoline("Test that changing the debug state is reflected");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 1.0f));
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

int UtcDaliPhysics2DAdaptorGetDebugState(void)
{
  tet_infoline("Test that changing the debug state is reflected");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 1.0f));
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

int UtcDaliPhysics2DAdaptorAddActorBody(void)
{
  tet_infoline("Test that an actor/body pair can be added");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 1.0f));
  Uint16Pair     size(640, 480);
  auto           scene     = application.GetScene();
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  scene.Add(rootActor);

  auto accessor = adaptor.GetPhysicsAccessor();
  auto space    = accessor->GetNative().Get<cpSpace*>();

  cpBody*     body         = CreateBody(space);
  Dali::Actor ballActor    = Toolkit::ImageView::New("gallery-small-1.jpg");
  auto        physicsActor = adaptor.AddActorBody(ballActor, body);

  DALI_TEST_CHECK(physicsActor);
  int id = ballActor[Actor::Property::ID];

  DALI_TEST_EQUALS(physicsActor.GetId(), id, TEST_LOCATION);
  DALI_TEST_EQUALS(physicsActor.GetBody().Get<cpBody*>(), body, TEST_LOCATION);

  END_TEST;
}

void removeShape(cpBody* body, cpShape* shape, void* data)
{
  cpSpace* space = static_cast<cpSpace*>(data);
  cpSpaceRemoveShape(space, shape);
  cpShapeSetBody(shape, nullptr);
  cpShapeFree(shape);
}

int UtcDaliPhysics2DAdaptorRemoveActorBodyP01(void)
{
  tet_infoline("Test that an actor/body pair can be removed");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 2.0f));
  Uint16Pair     size(640, 480);
  auto           scene     = application.GetScene();
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  scene.Add(rootActor);

  cpBody* body;
  {
    auto accessor = adaptor.GetPhysicsAccessor();
    auto space    = accessor->GetNative().Get<cpSpace*>();

    body = CreateBody(space);
  }
  Dali::Actor ballActor    = Toolkit::ImageView::New("gallery-small-1.jpg");
  auto        physicsActor = adaptor.AddActorBody(ballActor, body);

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  adaptor.RemoveActorBody(physicsActor);
  DALI_TEST_CHECK(!ballActor.GetParent());

  {
    auto accessor = adaptor.GetPhysicsAccessor();
    auto space    = accessor->GetNative().Get<cpSpace*>();

    try
    {
      cpBodyEachShape(body, removeShape, space);
      cpSpaceRemoveBody(space, body);
      tet_result(TET_PASS);
    }
    catch(std::exception& e)
    {
      tet_result(TET_FAIL);
    }
  }

  END_TEST;
}

int UtcDaliPhysics2DAdaptorRemoveActorBodyN01(void)
{
  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 2.0f));
  Uint16Pair     size(640, 480);
  auto           scene     = application.GetScene();
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  scene.Add(rootActor);

  tet_infoline("Test that removing a physics actor that hasn't been created with AddActorBody does nothing");
  Dali::Actor  actor        = Dali::Actor::New();
  cpBody*      body         = cpBodyNew(1.0f, 1.0f);
  PhysicsActor physicsActor = PhysicsActor::New(actor, body, adaptor);
  ;
  try
  {
    adaptor.RemoveActorBody(physicsActor);
    tet_result(TET_PASS);
  }
  catch(std::exception& e)
  {
    // Should fail silently, without exception!
    tet_result(TET_FAIL);
  }

  END_TEST;
}

int UtcDaliPhysics2DAdaptorRemoveActorBodyN02(void)
{
  tet_infoline("Test that an empty actor/body pair doesn't break adaptor");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 2.0f));
  Uint16Pair     size(640, 480);
  auto           scene     = application.GetScene();
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  scene.Add(rootActor);

  PhysicsActor physicsActor;
  try
  {
    adaptor.RemoveActorBody(physicsActor);
    tet_result(TET_FAIL);
  }
  catch(DaliException& e)
  {
    DALI_TEST_ASSERT(e, "Physics actor handle is empty", TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliPhysics2DAdaptorGetPhysicsActor(void)
{
  tet_infoline("Test that an actor/body pair can be retrieved");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 1.0f));
  Uint16Pair     size(640, 480);
  auto           scene     = application.GetScene();
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  scene.Add(rootActor);

  auto accessor = adaptor.GetPhysicsAccessor();
  auto space    = accessor->GetNative().Get<cpSpace*>();

  cpBody*     body         = CreateBody(space);
  Dali::Actor ballActor    = Toolkit::ImageView::New("gallery-small-1.jpg");
  auto        physicsActor = adaptor.AddActorBody(ballActor, body);

  DALI_TEST_CHECK(physicsActor);

  PhysicsActor testActor = adaptor.GetPhysicsActor(body);
  DALI_TEST_CHECK(testActor);
  DALI_TEST_CHECK(physicsActor == testActor);

  END_TEST;
}

int UtcDaliPhysics2DAdaptorBuildPickingRay(void)
{
  tet_infoline("Test that picking ray converts screen coords");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(1.0f, 1.0f, 1.0f));
  Uint16Pair     size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT);
  const Vector2  center(TestApplication::DEFAULT_SURFACE_WIDTH * 0.5f, TestApplication::DEFAULT_SURFACE_HEIGHT * 0.5f);
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  auto           scene     = application.GetScene();
  scene.Add(rootActor);

  Vector3 from, to;
  adaptor.BuildPickingRay(Vector3(center), Vector3(center), from, to); // Hit test centre of screen
  Vector3 physCenter = adaptor.TranslateToPhysicsSpace(Vector3(center));
  DALI_TEST_EQUALS(from, physCenter, 0.001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysics2DAdaptorProjectPoint(void)
{
  tet_infoline("Test that a point is projected into physics space");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 1.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);

  Vector3 projectedPoint = adaptor.ProjectPoint(Vector3(), -Vector3::ZAXIS, 200);

  DALI_TEST_EQUALS(projectedPoint, Vector3(0.0f, 0.0f, 0.0f), 0.001f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysics2DAdaptorQueue(void)
{
  tet_infoline("Test that Queue and CreateSyncPoint both work");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 1.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  auto           scene     = application.GetScene();
  scene.Add(rootActor);

  cpBody* body{nullptr};
  {
    auto accessor            = adaptor.GetPhysicsAccessor();
    auto space               = accessor->GetNative().Get<cpSpace*>();
    body                     = CreateBody(space);
    Dali::Actor ballActor    = Toolkit::ImageView::New("gallery-small-1.jpg");
    auto        physicsActor = adaptor.AddActorBody(ballActor, body);
  }

  tet_infoline("Test that Queue works without accessor");
  adaptor.Queue([body]() {
    cpBodySetPosition(body, cpv(100.0f, 20.0f));
  });
  adaptor.CreateSyncPoint();

  application.SendNotification();
  application.Render();
  // Should trigger an Update

  {
    auto   accessor = adaptor.GetPhysicsAccessor();
    cpVect origin   = cpBodyGetPosition(body);

    DALI_TEST_EQUALS(origin.x, cpFloat(100.0f), 0.001f, TEST_LOCATION);
    DALI_TEST_EQUALS(origin.y, cpFloat(20.0f), 0.001f, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliPhysics2DAdaptorCreateSyncPoint(void)
{
  tet_infoline("Test that a delayed CreateSyncPoint delays update");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 1.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  auto           scene     = application.GetScene();
  scene.Add(rootActor);

  cpBody* body{nullptr};
  {
    auto accessor            = adaptor.GetPhysicsAccessor();
    auto space               = accessor->GetNative().Get<cpSpace*>();
    body                     = CreateBody(space);
    Dali::Actor ballActor    = Toolkit::ImageView::New("gallery-small-1.jpg");
    auto        physicsActor = adaptor.AddActorBody(ballActor, body);

    tet_infoline("Test that Queue works with accessor");
    adaptor.Queue([body]() {
      cpBodySetPosition(body, cpv(100.0f, 20.0f));
    });
  }

  // Should trigger an Update without processing queue
  application.SendNotification();
  application.Render();

  {
    auto accessor = adaptor.GetPhysicsAccessor();

    cpVect origin = cpBodyGetPosition(body);
    DALI_TEST_EQUALS(origin.x, cpFloat(0.0f), 0.01f, TEST_LOCATION);
    DALI_TEST_EQUALS(origin.y, cpFloat(0.0f), 0.01f, TEST_LOCATION);
  }

  // Should now execute queue
  adaptor.CreateSyncPoint();
  application.SendNotification();
  application.Render();

  {
    auto accessor = adaptor.GetPhysicsAccessor();

    cpVect origin = cpBodyGetPosition(body);
    DALI_TEST_EQUALS(origin.x, cpFloat(100.0f), 0.01f, TEST_LOCATION);
    DALI_TEST_EQUALS(origin.y, cpFloat(20.0f), 0.01f, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliPhysics2DAdaptorHitTestP(void)
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
    auto        accessor  = adaptor.GetPhysicsAccessor(); // Prevent integration
    auto        space     = accessor->GetNative().Get<cpSpace*>();
    Dali::Actor ballActor = Toolkit::ImageView::New(TEST_RESOURCE_DIR "/gallery-small-1.jpg");
    cpBody*     body      = CreateBody(space);
    cpBodySetPosition(body, cpv(center.x, center.y));

    ballActor[Actor::Property::PARENT_ORIGIN] = ParentOrigin::CENTER;
    ballActor[Actor::Property::ANCHOR_POINT]  = AnchorPoint::CENTER;

    auto physicsActor = adaptor.AddActorBody(ballActor, body);
  }
  Test::WaitForEventThreadTrigger(1);

  // Should trigger an Update without processing queue
  application.SendNotification();
  application.Render();

  Vector3 from, to;
  adaptor.BuildPickingRay(Vector3(center), Vector3(center), from, to); // Hit test centre of screen

  {
    auto    accessor = adaptor.GetPhysicsAccessor();
    Vector3 localPivot;
    float   distanceFromCamera;

    cpShapeFilter GRAB_FILTER = {CP_NO_GROUP, 1u << 31, 1u << 31};
    Dali::Any     nativeFilter{GRAB_FILTER};

    auto body = accessor->HitTest(from, from, nativeFilter, localPivot, distanceFromCamera);

    DALI_TEST_CHECK(!body.Empty());
  }

  END_TEST;
}

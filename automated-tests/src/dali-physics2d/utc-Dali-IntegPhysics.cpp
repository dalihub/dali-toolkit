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

#include <dali-physics/integration-api/integ-physics-adaptor.h>
#include <dali-physics/integration-api/physics-world.h>

#include <dali-toolkit-test-suite-utils.h>
#include <toolkit-event-thread-callback.h>

#include <dali-toolkit/devel-api/controls/alignment/alignment.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/events/hit-test-algorithm.h>

#include <chipmunk/chipmunk.h>

using namespace Dali;
using namespace Dali::Toolkit::Physics;

static const char* BALL_IMAGE = TEST_RESOURCE_DIR "/gallery-small-1.jpg";

static cpBody* CreateBody(cpSpace* space)
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
static cpBody* CreateHexBody(cpSpace* space)
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

int UtcDaliPhysics2DIntegrationGetPhysicsWorld(void)
{
  ToolkitTestApplication application;
  tet_infoline("Testing getting the physics world");

  Matrix         transform(true);
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);
  DALI_TEST_CHECK(adaptor);

  Dali::Toolkit::Physics::Integration::PhysicsWorld world = Dali::Toolkit::Physics::Integration::GetPhysicsWorld(adaptor);

  DALI_TEST_CHECK(world.GetImpl() != nullptr);

  END_TEST;
}

int UtcDaliPhysics2DIntegrationPhysicsWorldLockUnlock(void)
{
  ToolkitTestApplication application;

  Matrix         transform(true);
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  auto           scene     = application.GetScene();
  scene.Add(rootActor);

  Dali::Actor ballActor = Toolkit::ImageView::New(BALL_IMAGE);

  cpBody*      body{nullptr};
  PhysicsActor physicsActor;
  {
    auto accessor = adaptor.GetPhysicsAccessor();
    auto space    = accessor->GetNative().Get<cpSpace*>();
    body          = CreateBody(space);
    physicsActor  = adaptor.AddActorBody(ballActor, body);
  }

  DALI_TEST_CHECK(adaptor);
  Dali::Toolkit::Physics::Integration::PhysicsWorld world = Dali::Toolkit::Physics::Integration::GetPhysicsWorld(adaptor);

  world.Lock();
  world.Unlock();

  {
    tet_infoline("Test that creating accessor after unlocking this does not lock up!");
    auto accessor = adaptor.GetPhysicsAccessor();
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliPhysics2DIntegrationPhysicsWorldGetNative(void)
{
  ToolkitTestApplication application;
  tet_infoline("Testing getting the native world through Integ API");

  Matrix         transform(true);
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  auto           scene     = application.GetScene();
  scene.Add(rootActor);

  Dali::Actor ballActor = Toolkit::ImageView::New(BALL_IMAGE);

  cpBody*      body{nullptr};
  PhysicsActor physicsActor;
  {
    auto accessor = adaptor.GetPhysicsAccessor();
    auto space    = accessor->GetNative().Get<cpSpace*>();
    body          = CreateBody(space);
    physicsActor  = adaptor.AddActorBody(ballActor, body);
  }

  DALI_TEST_CHECK(adaptor);
  Dali::Toolkit::Physics::Integration::PhysicsWorld world = Dali::Toolkit::Physics::Integration::GetPhysicsWorld(adaptor);

  world.Lock();
  cpBody* newBody{nullptr};
  try
  {
    auto     worldImpl = world.GetNative();
    cpSpace* space     = worldImpl.Get<cpSpace*>();
    DALI_TEST_CHECK(space != nullptr);

    newBody = CreateHexBody(space);
  }
  catch(DaliException& e)
  {
    tet_result(TET_FAIL);
  }
  world.Unlock();

  DALI_TEST_CHECK(newBody != nullptr);
  END_TEST;
}

int UtcDaliPhysics2DIntegrationPhysicsWorldHitTest(void)
{
  ToolkitTestApplication application;
  tet_infoline("Testing the Hit Test works through Integ API");

  const Vector2 center(TestApplication::DEFAULT_SURFACE_WIDTH * 0.5f, TestApplication::DEFAULT_SURFACE_HEIGHT * 0.5f);

  Matrix         transform(true);
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  auto           scene     = application.GetScene();
  scene.Add(rootActor);

  Dali::Actor ballActor = Toolkit::ImageView::New(BALL_IMAGE);

  cpBody*      body{nullptr};
  PhysicsActor physicsActor;
  {
    auto accessor = adaptor.GetPhysicsAccessor();
    auto space    = accessor->GetNative().Get<cpSpace*>();
    body          = CreateBody(space);
    physicsActor  = adaptor.AddActorBody(ballActor, body);
  }

  DALI_TEST_CHECK(adaptor);

  Vector3 from, to;
  adaptor.BuildPickingRay(Vector3(center), Vector3(center), from, to);

  Dali::Toolkit::Physics::Integration::PhysicsWorld world = Dali::Toolkit::Physics::Integration::GetPhysicsWorld(adaptor);
  world.Lock();
  try
  {
    auto     worldImpl = world.GetNative();
    cpSpace* space     = worldImpl.Get<cpSpace*>();
    DALI_TEST_CHECK(space != nullptr);

    cpShapeFilter GRAB_FILTER = {CP_NO_GROUP, 1u << 31, 1u << 31};
    Dali::Any     nativeFilter{GRAB_FILTER};
    Vector3       localPivot;
    float         distanceFromCamera;
    auto          body = world.HitTest(from, from, nativeFilter, localPivot, distanceFromCamera);

    DALI_TEST_CHECK(!body.Empty());
    cpBody* cbody = body.Get<cpBody*>();

    DALI_TEST_CHECK(cbody != nullptr);
  }
  catch(DaliException& e)
  {
    tet_result(TET_FAIL);
  }
  world.Unlock();

  END_TEST;
}

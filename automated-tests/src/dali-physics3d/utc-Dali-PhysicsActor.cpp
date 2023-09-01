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

#include <bullet/btBulletDynamicsCommon.h>
#include <stdlib.h>
#include <iostream>

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-toolkit-test-suite-utils.h>
#include <toolkit-event-thread-callback.h>

#include <dali-physics/dali-physics.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/alignment/alignment.h>

using namespace Dali;
using namespace Dali::Toolkit::Physics;

extern btRigidBody* CreateBody(btDiscreteDynamicsWorld* bulletWorld);

const char* BALL_IMAGE = TEST_RESOURCE_DIR "/gallery-small-1.jpg";

int UtcDaliPhysics3DActorNew(void)
{
  ToolkitTestApplication application;

  btRigidBody* body{nullptr};

  Matrix         transform(true);
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);

  auto accessor         = adaptor.GetPhysicsAccessor();
  auto bulletWorld      = accessor->GetNative().Get<btDiscreteDynamicsWorld*>();
  body                  = CreateBody(bulletWorld);
  Dali::Actor ballActor = Toolkit::ImageView::New(BALL_IMAGE);

  PhysicsActor physicsActor = PhysicsActor::New(ballActor, body, adaptor);

  DALI_TEST_CHECK(physicsActor);
  END_TEST;
}

int UtcDaliPhysics3DActorDownCastP(void)
{
  ToolkitTestApplication application;

  Matrix         transform(true);
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);

  btRigidBody* body{nullptr};
  auto         accessor     = adaptor.GetPhysicsAccessor();
  auto         bulletWorld  = accessor->GetNative().Get<btDiscreteDynamicsWorld*>();
  body                      = CreateBody(bulletWorld);
  Dali::Actor  ballActor    = Toolkit::ImageView::New(BALL_IMAGE);
  PhysicsActor physicsActor = PhysicsActor::New(ballActor, body, adaptor);
  BaseHandle   handle(physicsActor);

  PhysicsActor actor2 = PhysicsActor::DownCast(handle);
  DALI_TEST_CHECK(actor2);
  DALI_TEST_EQUALS(physicsActor.GetId(), actor2.GetId(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysics3DActorDownCastN(void)
{
  BaseHandle   uninitializedHandle;
  PhysicsActor actor = PhysicsActor::DownCast(uninitializedHandle);
  DALI_TEST_CHECK(!actor);
  END_TEST;
}

int UtcDaliPhysics3DActorMoveConstructor(void)
{
  ToolkitTestApplication application;
  tet_infoline("Testing the move constructor");

  Matrix         transform(true);
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);

  btRigidBody* body{nullptr};
  auto         accessor     = adaptor.GetPhysicsAccessor();
  auto         bulletWorld  = accessor->GetNative().Get<btDiscreteDynamicsWorld*>();
  body                      = CreateBody(bulletWorld);
  Dali::Actor  ballActor    = Toolkit::ImageView::New(BALL_IMAGE);
  PhysicsActor physicsActor = PhysicsActor::New(ballActor, body, adaptor);

  DALI_TEST_CHECK(physicsActor);
  uint32_t id = physicsActor.GetId();

  PhysicsActor moved = std::move(physicsActor);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!physicsActor);
  DALI_TEST_CHECK(moved != physicsActor);
  DALI_TEST_EQUALS(moved.GetId(), id, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysics3DActorCopyConstructor(void)
{
  ToolkitTestApplication application;
  tet_infoline("Testing the move constructor");

  Matrix         transform(true);
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);

  btRigidBody* body{nullptr};
  auto         accessor     = adaptor.GetPhysicsAccessor();
  auto         bulletWorld  = accessor->GetNative().Get<btDiscreteDynamicsWorld*>();
  body                      = CreateBody(bulletWorld);
  Dali::Actor  ballActor    = Toolkit::ImageView::New(BALL_IMAGE);
  PhysicsActor physicsActor = PhysicsActor::New(ballActor, body, adaptor);

  DALI_TEST_CHECK(physicsActor);
  uint32_t id = physicsActor.GetId();

  PhysicsActor selectedActor(physicsActor);
  DALI_TEST_CHECK(selectedActor);
  DALI_TEST_CHECK(physicsActor);
  DALI_TEST_CHECK(selectedActor == physicsActor); // should point at same object
  DALI_TEST_EQUALS(selectedActor.GetId(), id, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysics3DActorCopyAssign(void)
{
  ToolkitTestApplication application;
  tet_infoline("Testing the copy assign");

  Matrix         transform(true);
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);

  btRigidBody* body{nullptr};
  auto         accessor     = adaptor.GetPhysicsAccessor();
  auto         bulletWorld  = accessor->GetNative().Get<btDiscreteDynamicsWorld*>();
  body                      = CreateBody(bulletWorld);
  Dali::Actor  ballActor    = Toolkit::ImageView::New(BALL_IMAGE);
  PhysicsActor physicsActor = PhysicsActor::New(ballActor, body, adaptor);

  DALI_TEST_CHECK(physicsActor);
  uint32_t id = physicsActor.GetId();

  PhysicsActor selectedActor = physicsActor;
  DALI_TEST_CHECK(selectedActor);
  DALI_TEST_CHECK(physicsActor);
  DALI_TEST_CHECK(selectedActor == physicsActor); // should point at same object
  DALI_TEST_EQUALS(selectedActor.GetId(), id, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysics3DActorMoveAssignment(void)
{
  ToolkitTestApplication application;
  tet_infoline("Testing the move constructor");

  Matrix         transform(true);
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);

  btRigidBody* body{nullptr};
  auto         accessor     = adaptor.GetPhysicsAccessor();
  auto         bulletWorld  = accessor->GetNative().Get<btDiscreteDynamicsWorld*>();
  body                      = CreateBody(bulletWorld);
  Dali::Actor  ballActor    = Toolkit::ImageView::New(BALL_IMAGE);
  PhysicsActor physicsActor = PhysicsActor::New(ballActor, body, adaptor);

  DALI_TEST_CHECK(physicsActor);
  uint32_t id = physicsActor.GetId();

  PhysicsActor moved;
  moved = std::move(physicsActor);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!physicsActor);
  DALI_TEST_EQUALS(moved.GetId(), id, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysics3DActorGetIdP(void)
{
  ToolkitTestApplication application;
  tet_infoline("Testing the ID Getter");

  Matrix         transform(true);
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor = PhysicsAdaptor::New(transform, size);

  btRigidBody* body{nullptr};
  auto         accessor     = adaptor.GetPhysicsAccessor();
  auto         bulletWorld  = accessor->GetNative().Get<btDiscreteDynamicsWorld*>();
  body                      = CreateBody(bulletWorld);
  Dali::Actor  ballActor    = Toolkit::ImageView::New(BALL_IMAGE);
  PhysicsActor physicsActor = adaptor.AddActorBody(ballActor, body);
  int          id           = physicsActor.GetId();
  int          actorId      = ballActor[Actor::Property::ID];
  DALI_TEST_EQUALS(id, actorId, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysics3DActorGetIdN(void)
{
  ToolkitTestApplication application;
  tet_infoline("Testing the ID Getter");

  PhysicsActor physicsActor;
  try
  {
    uint32_t id __attribute__((unused)) = physicsActor.GetId();
    tet_result(TET_FAIL);
  }
  catch(DaliException e)
  {
    DALI_TEST_ASSERT(e, "Physics actor handle is empty", TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliPhysics3DActorGetBodyP(void)
{
  ToolkitTestApplication application;
  tet_infoline("Testing the body Getter");

  Matrix         transform(true);
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  auto           scene     = application.GetScene();
  scene.Add(rootActor);

  PhysicsActor physicsActor;
  btRigidBody* body{nullptr};
  {
    auto accessor         = adaptor.GetPhysicsAccessor();
    auto bulletWorld      = accessor->GetNative().Get<btDiscreteDynamicsWorld*>();
    body                  = CreateBody(bulletWorld);
    Dali::Actor ballActor = Toolkit::ImageView::New(BALL_IMAGE);

    physicsActor = adaptor.AddActorBody(ballActor, body);
  }

  application.Render();
  Test::WaitForEventThreadTrigger(1);

  Dali::Any any = physicsActor.GetBody();
  DALI_TEST_EQUALS(any.Get<btRigidBody*>(), body, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPhysics3DActorGetBodyN(void)
{
  ToolkitTestApplication application;
  tet_infoline("Testing the ID Getter");

  PhysicsActor physicsActor;
  try
  {
    Dali::Any any __attribute__((unused)) = physicsActor.GetBody();
    tet_result(TET_FAIL);
  }
  catch(DaliException e)
  {
    DALI_TEST_ASSERT(e, "Physics actor handle is empty", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliPhysics3DActorSetPosition(void)
{
  tet_infoline("Test the AsyncSetPhysicsPosition() function");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 2.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  auto           scene     = application.GetScene();
  scene.Add(rootActor);

  Dali::Actor ballActor = Toolkit::ImageView::New(BALL_IMAGE);

  btRigidBody* body{nullptr};
  PhysicsActor physicsActor;
  {
    auto accessor    = adaptor.GetPhysicsAccessor();
    auto bulletWorld = accessor->GetNative().Get<btDiscreteDynamicsWorld*>();
    body             = CreateBody(bulletWorld);
    physicsActor     = adaptor.AddActorBody(ballActor, body);
    physicsActor.AsyncSetPhysicsPosition(Vector3(10, 20, -30));
  }

  Test::WaitForEventThreadTrigger(1);
  adaptor.CreateSyncPoint();
  application.SendNotification();
  application.Render();

  // Run 2 frames to ensure both buffers are set.
  application.SendNotification();
  application.Render();
  {
    auto accessor = adaptor.GetPhysicsAccessor();
    auto actor    = rootActor.FindChildById(physicsActor.GetId());
    // Warning - physics properties are never reflected in the event size cache.
    // Have to use GetCurrentProperty to see the updated values.
    DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(10, 20, -30), 0.0001f, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliPhysics3DActorSetRotation1(void)
{
  tet_infoline("Test the AsyncSetPhysicsRotation() function");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, -2.0f, 2.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  auto           scene     = application.GetScene();
  scene.Add(rootActor);

  Dali::Actor ballActor = Toolkit::ImageView::New(BALL_IMAGE);

  btRigidBody* body{nullptr};
  PhysicsActor physicsActor;
  {
    auto accessor    = adaptor.GetPhysicsAccessor();
    auto bulletWorld = accessor->GetNative().Get<btDiscreteDynamicsWorld*>();
    body             = CreateBody(bulletWorld);
    physicsActor     = adaptor.AddActorBody(ballActor, body);
    physicsActor.AsyncSetPhysicsRotation(Quaternion(Degree(30), Vector3::YAXIS));
  }

  Test::WaitForEventThreadTrigger(1);
  adaptor.CreateSyncPoint();
  application.SendNotification();
  application.Render();

  // Run 2 frames to ensure both buffers are set.
  application.SendNotification();
  application.Render();
  {
    auto accessor = adaptor.GetPhysicsAccessor();
    auto actor    = rootActor.FindChildById(physicsActor.GetId());
    // Warning - physics properties are never reflected in the event size cache.
    // Have to use GetCurrentProperty to see the updated values.
    Quaternion q = actor.GetCurrentProperty<Quaternion>(Actor::Property::ORIENTATION);
    Quaternion expected(Degree(30), Vector3::YAXIS);
    DALI_TEST_EQUALS(q, expected, 0.0001f, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliPhysics3DActorSetRotation2(void)
{
  tet_infoline("Test the AsyncSetPhysicsRotation() function");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, -2.0f, 2.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  auto           scene     = application.GetScene();
  scene.Add(rootActor);

  Dali::Actor ballActor = Toolkit::ImageView::New(BALL_IMAGE);

  btRigidBody* body{nullptr};
  PhysicsActor physicsActor;
  {
    auto accessor    = adaptor.GetPhysicsAccessor();
    auto bulletWorld = accessor->GetNative().Get<btDiscreteDynamicsWorld*>();
    body             = CreateBody(bulletWorld);
    physicsActor     = adaptor.AddActorBody(ballActor, body);
    physicsActor.AsyncSetPhysicsRotation(Quaternion(Degree(30), Vector3::ZAXIS));
  }

  Test::WaitForEventThreadTrigger(1);
  adaptor.CreateSyncPoint();
  application.SendNotification();
  application.Render();

  // Run 2 frames to ensure both buffers are set.
  application.SendNotification();
  application.Render();
  {
    auto accessor = adaptor.GetPhysicsAccessor();
    auto actor    = rootActor.FindChildById(physicsActor.GetId());
    // Warning - physics properties are never reflected in the event size cache.
    // Have to use GetCurrentProperty to see the updated values.
    Quaternion q = actor.GetCurrentProperty<Quaternion>(Actor::Property::ORIENTATION);
    Quaternion expected(Degree(30), Vector3::ZAXIS);
    DALI_TEST_EQUALS(q, expected, 0.0001f, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliPhysics3DActorGetActorPosition(void)
{
  tet_infoline("Test the GetActorPosition() function");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 2.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  auto           scene     = application.GetScene();
  scene.Add(rootActor);

  Dali::Actor ballActor = Toolkit::ImageView::New(BALL_IMAGE);

  btRigidBody* body{nullptr};
  PhysicsActor physicsActor;
  {
    auto accessor    = adaptor.GetPhysicsAccessor();
    auto bulletWorld = accessor->GetNative().Get<btDiscreteDynamicsWorld*>();
    body             = CreateBody(bulletWorld);
    physicsActor     = adaptor.AddActorBody(ballActor, body);
    physicsActor.AsyncSetPhysicsPosition(Vector3(10, 20, -30));
  }

  Test::WaitForEventThreadTrigger(1);
  adaptor.CreateSyncPoint();
  application.SendNotification();
  application.Render();

  // Run 2 frames to ensure both buffers are set.
  application.SendNotification();
  application.Render();
  {
    auto accessor = adaptor.GetPhysicsAccessor();
    DALI_TEST_EQUALS(physicsActor.GetActorPosition(), Vector3(10, 20, -30), 0.0001f, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliPhysics3DActorGetActorRotation(void)
{
  tet_infoline("Test the GetActorRotation() function");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 2.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  auto           scene     = application.GetScene();
  scene.Add(rootActor);

  Dali::Actor ballActor = Toolkit::ImageView::New(BALL_IMAGE);

  btRigidBody* body{nullptr};
  PhysicsActor physicsActor;
  {
    auto accessor    = adaptor.GetPhysicsAccessor();
    auto bulletWorld = accessor->GetNative().Get<btDiscreteDynamicsWorld*>();
    body             = CreateBody(bulletWorld);
    physicsActor     = adaptor.AddActorBody(ballActor, body);
    physicsActor.AsyncSetPhysicsRotation(Quaternion(Degree(30), Vector3::ZAXIS));
  }

  Test::WaitForEventThreadTrigger(1);
  adaptor.CreateSyncPoint();
  application.SendNotification();
  application.Render();

  // Run 2 frames to ensure both buffers are set.
  application.SendNotification();
  application.Render();
  {
    auto accessor = adaptor.GetPhysicsAccessor();
    DALI_TEST_EQUALS(physicsActor.GetActorRotation(), Quaternion(Degree(30), Vector3::ZAXIS), 0.0001f, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliPhysics3DActorGetPhysicsPosition(void)
{
  tet_infoline("Test the GetPhysicsPosition() function");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, 2.0f, 2.0f));

  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  auto           scene     = application.GetScene();
  scene.Add(rootActor);

  Dali::Actor ballActor = Toolkit::ImageView::New(BALL_IMAGE);

  btRigidBody* body{nullptr};
  PhysicsActor physicsActor;
  {
    auto accessor    = adaptor.GetPhysicsAccessor();
    auto bulletWorld = accessor->GetNative().Get<btDiscreteDynamicsWorld*>();
    body             = CreateBody(bulletWorld);
    physicsActor     = adaptor.AddActorBody(ballActor, body);
    physicsActor.AsyncSetPhysicsPosition(Vector3(10, 20, -30));
  }

  Test::WaitForEventThreadTrigger(1);
  adaptor.CreateSyncPoint();
  application.SendNotification();
  application.Render();

  // Run 2 frames to ensure both buffers are set.
  application.SendNotification();
  application.Render();
  {
    auto    accessor = adaptor.GetPhysicsAccessor();
    Vector4 pos      = transform * Vector4(10, 20, -30, 1);
    DALI_TEST_EQUALS(physicsActor.GetPhysicsPosition(), Vector3(pos), 0.0001f, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliPhysics3DActorGetPhysicsRotation(void)
{
  tet_infoline("Test the GetPhysicsRotation() function");

  ToolkitTestApplication application;
  Matrix                 transform(false);
  transform.SetIdentityAndScale(Vector3(2.0f, -2.0f, 2.0f));
  Uint16Pair     size(640, 480);
  PhysicsAdaptor adaptor   = PhysicsAdaptor::New(transform, size);
  Actor          rootActor = adaptor.GetRootActor();
  auto           scene     = application.GetScene();
  scene.Add(rootActor);

  Dali::Actor ballActor = Toolkit::ImageView::New(BALL_IMAGE);

  btRigidBody* body{nullptr};
  PhysicsActor physicsActor;
  {
    auto accessor    = adaptor.GetPhysicsAccessor();
    auto bulletWorld = accessor->GetNative().Get<btDiscreteDynamicsWorld*>();
    body             = CreateBody(bulletWorld);
    physicsActor     = adaptor.AddActorBody(ballActor, body);
    physicsActor.AsyncSetPhysicsRotation(Quaternion(Degree(30), Vector3::ZAXIS));
  }

  Test::WaitForEventThreadTrigger(1);
  adaptor.CreateSyncPoint();
  application.SendNotification();
  application.Render();

  // Run 2 frames to ensure both buffers are set.
  application.SendNotification();
  application.Render();
  {
    auto accessor = adaptor.GetPhysicsAccessor();
    DALI_TEST_EQUALS(physicsActor.GetPhysicsRotation(), Quaternion(Degree(-30), Vector3::ZAXIS), 0.0001f, TEST_LOCATION);
  }

  END_TEST;
}

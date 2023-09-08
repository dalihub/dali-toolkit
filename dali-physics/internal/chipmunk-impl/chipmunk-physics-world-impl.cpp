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

// Class Header
#include <dali-physics/internal/chipmunk-impl/chipmunk-physics-world-impl.h>

// External Headers

// Internal Headers
#include <dali/dali.h>
#include <dali/devel-api/common/stage-devel.h>
#include <dali/devel-api/update/frame-callback-interface.h>

namespace
{
inline cpVect ConvertVector(Dali::Vector3 vector)
{
  return cpv(vector.x, vector.y);
}

static void ShapeFreeWrap(cpSpace* space, cpShape* shape, void* unused)
{
  cpSpaceRemoveShape(space, shape);
  cpShapeFree(shape);
}

static void PostShapeFree(cpShape* shape, cpSpace* space)
{
  cpSpaceAddPostStepCallback(space, (cpPostStepFunc)ShapeFreeWrap, shape, NULL);
}

static void ConstraintFreeWrap(cpSpace* space, cpConstraint* constraint, void* unused)
{
  cpSpaceRemoveConstraint(space, constraint);
  cpConstraintFree(constraint);
}

static void PostConstraintFree(cpConstraint* constraint, cpSpace* space)
{
  cpSpaceAddPostStepCallback(space, (cpPostStepFunc)ConstraintFreeWrap, constraint, NULL);
}

static void BodyFreeWrap(cpSpace* space, cpBody* body, void* unused)
{
  cpSpaceRemoveBody(space, body);
  cpBodyFree(body);
}

static void PostBodyFree(cpBody* body, cpSpace* space)
{
  cpSpaceAddPostStepCallback(space, (cpPostStepFunc)BodyFreeWrap, body, NULL);
}
} // namespace

namespace Dali::Toolkit::Physics::Internal
{
std::unique_ptr<PhysicsWorld> ChipmunkPhysicsWorld::New(Dali::Actor rootActor, Dali::CallbackBase* updateCallback)
{
  std::unique_ptr<ChipmunkPhysicsWorld> world = std::make_unique<ChipmunkPhysicsWorld>(rootActor, updateCallback);
  world->Initialize();
  return world;
}

ChipmunkPhysicsWorld::ChipmunkPhysicsWorld(Dali::Actor rootActor, Dali::CallbackBase* updateCallback)
: PhysicsWorld(rootActor, updateCallback)
{
}

void ChipmunkPhysicsWorld::OnInitialize(/*void* dynamicsWorld*/)
{
  // @todo Should enable developer to optionally supply their own created cpSpace.
  mSpace = cpSpaceNew();
  cpSpaceSetIterations(mSpace, 30);
  cpSpaceSetSleepTimeThreshold(mSpace, 0.5f);
  cpSpaceSetGravity(mSpace, cpv(0, -200));
}

ChipmunkPhysicsWorld::~ChipmunkPhysicsWorld()
{
  Dali::Mutex::ScopedLock lock(mMutex);
  if(mSpace)
  {
    cpSpaceEachShape(mSpace, (cpSpaceShapeIteratorFunc)PostShapeFree, mSpace);
    cpSpaceEachConstraint(mSpace, (cpSpaceConstraintIteratorFunc)PostConstraintFree, mSpace);
    cpSpaceEachBody(mSpace, (cpSpaceBodyIteratorFunc)PostBodyFree, mSpace);
    cpSpaceFree(mSpace);
    mSpace = nullptr;
  }
}

Dali::Any ChipmunkPhysicsWorld::GetNative()
{
  return mSpace;
}

void ChipmunkPhysicsWorld::Integrate(float timestep)
{
  if(mPhysicsIntegrateState == Physics::PhysicsAdaptor::IntegrationState::ON)
  {
    cpSpaceStep(mSpace, timestep);
  }

  if(mPhysicsDebugState == Physics::PhysicsAdaptor::DebugState::ON)
  {
    if(mDebugRenderer)
    {
      cpSpaceDebugDraw(mSpace, const_cast<cpSpaceDebugDrawOptions*>(&mDebugRenderer->GetDebugDrawOptions()));
    }
  }
}

Dali::Any ChipmunkPhysicsWorld::HitTest(Dali::Vector3 rayFromWorld, Dali::Vector3 rayToWorld, Dali::Any nativeFilter, Dali::Vector3& localPivot, float& distanceFromCamera)
{
  cpVect           spacePosition = cpv(rayFromWorld.x, rayFromWorld.y);
  cpFloat          radius        = 5.0f;
  cpPointQueryInfo info          = {0};
  cpShapeFilter    filter        = nativeFilter.Get<cpShapeFilter>();
  cpShape*         shape         = cpSpacePointQueryNearest(mSpace, spacePosition, radius, filter, &info);
  cpBody*          hitBody{nullptr};

  if(shape && cpBodyGetMass(cpShapeGetBody(shape)) < INFINITY)
  {
    // Use the closest point on the surface if the click is outside the shape.
    cpVect nearest = (info.distance > 0.0f ? info.point : spacePosition);
    hitBody        = cpShapeGetBody(shape);
    cpVect local   = cpBodyWorldToLocal(hitBody, nearest);
    localPivot.x   = local.x;
    localPivot.y   = local.y;
    localPivot.z   = 0.0;
  }

  Dali::Any bodyPtr;
  // Only set non-null ptr into bodyPtr, leave empty if null.
  if(hitBody)
  {
    bodyPtr = hitBody;
  }
  return bodyPtr;
}

} // namespace Dali::Toolkit::Physics::Internal

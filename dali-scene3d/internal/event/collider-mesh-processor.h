#pragma once

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
 *
 */

// EXTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/controls/model/model.h>

namespace Dali::Scene3D
{
namespace Internal
{
class ColliderMeshProcessor;
}

class Collidable
{
public:
  Collidable() = default;

  Dali::Actor GetCollidableActor() const
  {
    return mCollidableActor.GetHandle();
  }

protected:
  void SetCollidableActor(Dali::Actor collidableActor)
  {
    mCollidableActor = collidableActor;
  }

private:
  Dali::WeakHandle<Dali::Actor> mCollidableActor;
};

class ColliderMeshProcessor : public BaseHandle
{
public:
  ColliderMeshProcessor()  = default;
  ~ColliderMeshProcessor() = default;

  static ColliderMeshProcessor Get();

  void ColliderMeshChanged(Collidable& collidable);

private:
  explicit ColliderMeshProcessor(Internal::ColliderMeshProcessor* impl);
};

} // namespace Dali::Scene3D

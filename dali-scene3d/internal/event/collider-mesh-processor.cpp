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

// CLASS HEADER
#include <dali-scene3d/internal/event/collider-mesh-processor.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/common/singleton-service.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/event/collider-mesh-processor-impl.h>

namespace Dali::Scene3D
{
ColliderMeshProcessor ColliderMeshProcessor::Get()
{
  ColliderMeshProcessor processor;

  // Check whether the processor has already created
  SingletonService singletonService(SingletonService::Get());
  if(singletonService)
  {
    Dali::BaseHandle handle = singletonService.GetSingleton(typeid(ColliderMeshProcessor));
    if(handle)
    {
      // If so, downcast the handle of singleton to focus manager
      processor = ColliderMeshProcessor(dynamic_cast<Internal::ColliderMeshProcessor*>(handle.GetObjectPtr()));
    }

    if(!processor)
    {
      // If not, create the focus manager and register it as a singleton
      processor = ColliderMeshProcessor(new Internal::ColliderMeshProcessor());
      singletonService.Register(typeid(processor), processor);
    }
  }

  return processor;
}

void ColliderMeshProcessor::ColliderMeshChanged(Scene3D::Model model)
{
  GetImpl(*this).ColliderMeshChanged(model);
}

ColliderMeshProcessor::ColliderMeshProcessor(Internal::ColliderMeshProcessor* impl)
: BaseHandle(impl)
{
}

} // namespace Dali::Scene3D

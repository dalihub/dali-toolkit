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
#include <dali/integration-api/processor-interface.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/connection-tracker.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/event/collider-mesh-processor.h>
#include <dali-scene3d/public-api/controls/model/model.h>
#include <dali-scene3d/public-api/controls/scene-view/scene-view.h>

namespace Dali::Scene3D
{
namespace Internal
{
class ColliderMeshProcessor : public BaseObject, public Dali::ConnectionTracker, public Integration::Processor
{
public:
  ColliderMeshProcessor();

  ~ColliderMeshProcessor();

  void ColliderMeshChanged(Collidable& collidable);

private:
  void ModelOnScene(Actor actor);

  void AddSceneViewParentToProcessingQueue(Actor actor);

protected: // Implementation of Processor
  /**
   * @copydoc Dali::Integration::Processor::Process()
   */
  void Process(bool /*postProcessor*/) override;

  /**
   * @copydoc Dali::Integration::Processor::GetProcessorName()
   */
  std::string_view GetProcessorName() const override
  {
    return "ColliderMeshProcessor";
  }

private:
  std::vector<Scene3D::SceneView> mSceneViewsToProcess;
  std::vector<Scene3D::SceneView> mConnectedSceneViews;
};

} // namespace Internal

inline Internal::ColliderMeshProcessor& GetImpl(ColliderMeshProcessor& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<Internal::ColliderMeshProcessor&>(handle);
}

inline const Internal::ColliderMeshProcessor& GetImpl(const ColliderMeshProcessor& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::ColliderMeshProcessor&>(handle);
}

} // namespace Dali::Scene3D

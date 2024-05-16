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
#include <dali-scene3d/internal/event/collider-mesh-processor-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/events/hit-test-algorithm.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <algorithm>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/controls/model/model-impl.h>
#include <dali-scene3d/internal/controls/scene-view/scene-view-impl.h>
#include <dali-scene3d/public-api/controls/scene-view/scene-view.h>

namespace Dali::Scene3D::Internal
{
namespace
{
struct ColliderMeshData
{
  Scene3D::Model                                model;
  Scene3D::ModelNode                            modelNode;
  const Dali::Scene3D::Algorithm::ColliderMesh& colliderMesh;
  Matrix                                        worldMatrix{false};
};
using ColliderMeshDataContainer = std::vector<ColliderMeshData>;

void IterateThroughChildren(Actor actor, ColliderMeshDataContainer& meshData)
{
  if(actor)
  {
    const auto childCount = actor.GetChildCount();
    for(auto i = 0u; i < childCount; ++i)
    {
      Actor          child = actor.GetChildAt(i);
      Scene3D::Model model = Scene3D::Model::DownCast(child);
      if(model)
      {
        const Model::ColliderMeshContainer& colliderMeshes = GetImpl(model).GetNodeColliderMeshContainer();
        for(const auto& colliderMeshItem : colliderMeshes)
        {
          // If actor name is empty, then assume the mesh if for the model itself
          [[maybe_unused]] int     actorId   = colliderMeshItem.first;
          Dali::Scene3D::ModelNode modelNode = colliderMeshItem.second;
          if(modelNode)
          {
            meshData.push_back({model, modelNode, GetImplementation(modelNode).GetColliderMesh(), DevelActor::GetWorldTransform(modelNode)});
          }
        }
      }
      IterateThroughChildren(child, meshData);
    }
  }
}

class SceneViewTouchHandler
{
public:
  bool operator()(Actor actor, const TouchEvent& touchEvent)
  {
    Scene3D::SceneView sceneView = Scene3D::SceneView::DownCast(actor);
    bool               retVal(false);
    if(sceneView)
    {
      // Get nav-mesh information for the children
      std::vector<ColliderMeshData> meshData;
      IterateThroughChildren(sceneView, meshData);

      auto renderTask  = touchEvent.GetRenderTask();
      auto cameraActor = renderTask.GetCameraActor();

      const auto& result = touchEvent.GetScreenPosition(0);
      float       x = 0.0f, y = 0.0f;
      Vector3     origin;
      Vector3     direction;
      cameraActor.ScreenToLocal(x, y, result.x, result.y);

      auto sceneViewRenderTask = GetImpl(sceneView).GetRenderTask();

      if(sceneViewRenderTask && HitTestAlgorithm::BuildPickingRay(sceneViewRenderTask, result, origin, direction))
      {
        for(auto& mesh : meshData)
        {
          // Set transform for the collider mesh
          const_cast<Dali::Scene3D::Algorithm::ColliderMesh&>(mesh.colliderMesh).SetSceneTransform(mesh.worldMatrix);
          auto res = mesh.colliderMesh.RayFaceIntersect(origin, direction);
          if(res != Dali::Scene3D::Algorithm::NavigationMesh::NULL_FACE)
          {
            Scene3D::Model            model     = mesh.model;
            Scene3D::Internal::Model& modelImpl = GetImpl(model);
            retVal                              = modelImpl.EmitMeshHitSignal(mesh.modelNode);
            break;
          }
        }
      }
    }
    return retVal;
  }
};

} // unnamed namespace

ColliderMeshProcessor::ColliderMeshProcessor()
{
  Adaptor::Get().RegisterProcessor(*this, true);
}

ColliderMeshProcessor::~ColliderMeshProcessor()
{
  if(Adaptor::IsAvailable())
  {
    Adaptor::Get().UnregisterProcessor(*this, true);
  }
}

void ColliderMeshProcessor::ColliderMeshChanged(Collidable* collidable)
{
  if(!collidable)
  {
    DALI_LOG_ERROR("The collider should not be null.\n");
  }

  Actor actor = collidable->GetCollidableActor();
  if(actor)
  {
    if(actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
    {
      AddSceneViewParentToProcessingQueue(collidable->GetCollidableActor());
    }
    else
    {
      // TODO: Check if signal already connected
      collidable->GetCollidableActor().OnSceneSignal().Connect(this, &ColliderMeshProcessor::ModelOnScene);
    }
  }
}

void ColliderMeshProcessor::ModelOnScene(Actor actor)
{
  Scene3D::Model model = Scene3D::Model::DownCast(actor);
  if(model)
  {
    AddSceneViewParentToProcessingQueue(model);
  }
  model.OnSceneSignal().Disconnect(this, &ColliderMeshProcessor::ModelOnScene);
}

void ColliderMeshProcessor::AddSceneViewParentToProcessingQueue(Actor actor)
{
  Actor currentActor = actor;
  do
  {
    currentActor = currentActor.GetParent();
    Scene3D::SceneView sceneView(Scene3D::SceneView::DownCast(currentActor));
    if(sceneView)
    {
      mSceneViewsToProcess.push_back(sceneView);
      break;
    }
  } while(currentActor);
}

void ColliderMeshProcessor::Process(bool /* postProcess */)
{
  // Remove any duplicates
  std::sort(mSceneViewsToProcess.begin(), mSceneViewsToProcess.end());
  mSceneViewsToProcess.erase(std::unique(mSceneViewsToProcess.begin(), mSceneViewsToProcess.end()), mSceneViewsToProcess.end());

  for(auto& sceneView : mSceneViewsToProcess)
  {
    std::vector<ColliderMeshData> meshData;
    IterateThroughChildren(sceneView, meshData);

    if(meshData.size())
    {
      // TODO: Get SceneView Camera parameters and calculate bounding box
      // for now, it's a pass-thru algorthm
      if(std::find(mConnectedSceneViews.begin(), mConnectedSceneViews.end(), sceneView) == mConnectedSceneViews.end())
      {
        mConnectedSceneViews.push_back(sceneView);
        // TODO: Consider passing in camera parameters and meshData into SceneViewTouchHandler
        sceneView.TouchedSignal().Connect(this, SceneViewTouchHandler());
      }
    }
  }
  mSceneViewsToProcess.clear();
}

} // namespace Dali::Scene3D::Internal

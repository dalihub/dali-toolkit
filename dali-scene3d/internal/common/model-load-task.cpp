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
#include <dali-scene3d/internal/common/model-load-task.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <filesystem>

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
namespace
{
static constexpr Vector3 Y_DIRECTION(1.0f, -1.0f, 1.0f);
} // namespace

ModelLoadTask::ModelLoadTask(const std::string& modelUrl, const std::string& resourceDirectoryUrl, CallbackBase* callback)
: AsyncTask(callback),
  mModelUrl(modelUrl),
  mResourceDirectoryUrl(resourceDirectoryUrl),
  mModelCacheManager(Scene3D::Internal::ModelCacheManager::Get()),
  mLoadResult(mModelCacheManager.GetModelLoadResult(mModelUrl)),
  mHasSucceeded(false)
{
}

ModelLoadTask::~ModelLoadTask()
{
}

void ModelLoadTask::Process()
{
  if(mResourceDirectoryUrl.empty())
  {
    std::filesystem::path modelUrl(mModelUrl);
    mResourceDirectoryUrl = std::string(modelUrl.parent_path()) + "/";
  }

  Dali::Scene3D::Loader::ResourceBundle::PathProvider pathProvider = [&](Dali::Scene3D::Loader::ResourceType::Value type) {
    return mResourceDirectoryUrl;
  };

  mModelLoader = std::make_shared<Dali::Scene3D::Loader::ModelLoader>(mModelUrl, mResourceDirectoryUrl, mLoadResult);

  bool                   loadSucceeded            = false;
  Dali::ConditionalWait& loadSceneConditionalWait = mModelCacheManager.GetLoadSceneConditionalWaitInstance(mModelUrl);
  {
    ConditionalWait::ScopedLock lock(loadSceneConditionalWait);
    if(mModelCacheManager.IsSceneLoaded(mModelUrl))
    {
      loadSucceeded = true;
    }
    else
    {
      mModelCacheManager.SetSceneLoading(mModelUrl, true);

      loadSucceeded = mModelLoader->LoadModel(pathProvider, true);

      // Mesh of glTF and dli is defined in right hand coordinate system, with positive Y for Up direction.
      // Because DALi uses left hand system, Y direciton will be flipped for environment map sampling.
      for(auto&& env : GetResources().mEnvironmentMaps)
      {
        env.first.mYDirection = Y_DIRECTION;
      }

      mModelCacheManager.SetSceneLoading(mModelUrl, false);
      mModelCacheManager.SetSceneLoaded(mModelUrl, loadSucceeded);
    }
  }

  if(!loadSucceeded)
  {
    DALI_LOG_ERROR("Failed to load scene from '%s'\n", mModelUrl.c_str());
    return;
  }

  mHasSucceeded = true;
}

bool ModelLoadTask::IsReady()
{
  return true;
}

bool ModelLoadTask::HasSucceeded() const
{
  return mHasSucceeded;
}

Dali::Scene3D::Loader::SceneDefinition& ModelLoadTask::GetScene() const
{
  return mModelLoader->GetScene();
}

Dali::Scene3D::Loader::ResourceBundle& ModelLoadTask::GetResources() const
{
  return mModelLoader->GetResources();
}

std::vector<Dali::Scene3D::Loader::AnimationDefinition>& ModelLoadTask::GetAnimations() const
{
  return mModelLoader->GetAnimations();
}

std::vector<Dali::Scene3D::Loader::CameraParameters>& ModelLoadTask::GetCameras() const
{
  return mModelLoader->GetCameras();
}

Dali::Scene3D::Loader::Customization::Choices& ModelLoadTask::GetResourceChoices()
{
  return mModelLoader->GetResourceChoices();
}

} // namespace Internal

} // namespace Scene3D

} // namespace Dali

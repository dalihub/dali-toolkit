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

// INTERNAL INCLUDES
#include <dali-scene3d/internal/common/model-cache-manager.h>
#include <dali-scene3d/public-api/loader/animation-definition.h>
#include <dali-scene3d/public-api/loader/camera-parameters.h>
#include <dali-scene3d/public-api/loader/dli-loader.h>
#include <dali-scene3d/public-api/loader/gltf2-loader.h>
#include <dali-scene3d/public-api/loader/light-parameters.h>
#include <dali-scene3d/public-api/loader/node-definition.h>
#include <dali-scene3d/public-api/loader/shader-definition-factory.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
namespace
{
static constexpr Vector3 Y_DIRECTION(1.0f, -1.0f, 1.0f);

static constexpr std::string_view OBJ_EXTENSION      = ".obj";
static constexpr std::string_view GLTF_EXTENSION     = ".gltf";
static constexpr std::string_view DLI_EXTENSION      = ".dli";
static constexpr std::string_view METADATA_EXTENSION = "metadata";
} // namespace

ModelLoadTask::ModelLoadTask(const std::string& modelUrl, const std::string& resourceDirectoryUrl, CallbackBase* callback)
: AsyncTask(callback),
  mModelUrl(modelUrl),
  mResourceDirectoryUrl(resourceDirectoryUrl),
  mHasSucceeded(false),
  mModelCacheManager(ModelCacheManager::Get()),
  mLoadResult(mModelCacheManager.GetModelLoadResult(modelUrl))
{
}

ModelLoadTask::~ModelLoadTask()
{
}

void ModelLoadTask::Process()
{
  uint32_t               cacheRefCount                  = mModelCacheManager.GetModelCacheRefCount(mModelUrl);
  Dali::ConditionalWait& loadSceneConditionalWait       = mModelCacheManager.GetLoadSceneConditionalWaitInstance(mModelUrl);
  Dali::ConditionalWait& loadRawResourceConditionalWait = mModelCacheManager.GetLoadRawResourceConditionalWaitInstance(mModelUrl);

  std::filesystem::path modelUrl(mModelUrl);
  if(mResourceDirectoryUrl.empty())
  {
    mResourceDirectoryUrl = std::string(modelUrl.parent_path()) + "/";
  }
  std::string extension = modelUrl.extension();
  std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

  Dali::Scene3D::Loader::ResourceBundle::PathProvider pathProvider = [&](Dali::Scene3D::Loader::ResourceType::Value type) {
    return mResourceDirectoryUrl;
  };

  {
    ConditionalWait::ScopedLock lock(loadSceneConditionalWait);

    while(cacheRefCount > 1 && mModelCacheManager.IsSceneLoading(mModelUrl))
    {
      loadSceneConditionalWait.Wait();
    }
  }

  {
    ConditionalWait::ScopedLock lock(loadSceneConditionalWait);

    if(!mModelCacheManager.IsSceneLoaded(mModelUrl))
    {
      mModelCacheManager.SetSceneLoading(mModelUrl, true);

      std::filesystem::path metaDataUrl = modelUrl;
      metaDataUrl.replace_extension(METADATA_EXTENSION.data());

      Dali::Scene3D::Loader::LoadSceneMetadata(metaDataUrl.c_str(), mLoadResult.mSceneMetadata);

      mLoadResult.mAnimationDefinitions.clear();

      if(extension == DLI_EXTENSION)
      {
        Dali::Scene3D::Loader::DliLoader              loader;
        Dali::Scene3D::Loader::DliLoader::InputParams input{
          pathProvider(Dali::Scene3D::Loader::ResourceType::Mesh),
          nullptr,
          {},
          {},
          nullptr,
          {}};
        Dali::Scene3D::Loader::DliLoader::LoadParams loadParams{input, mLoadResult};
        if(!loader.LoadScene(mModelUrl, loadParams))
        {
          DALI_LOG_ERROR("Failed to load scene from '%s': %s\n", mModelUrl.c_str(), loader.GetParseError().c_str());

          mModelCacheManager.SetSceneLoaded(mModelUrl, false);
          mModelCacheManager.SetSceneLoading(mModelUrl, false);
          mModelCacheManager.UnreferenceModelCache(mModelUrl);

          return;
        }
      }
      else if(extension == GLTF_EXTENSION)
      {
        Dali::Scene3D::Loader::ShaderDefinitionFactory sdf;
        sdf.SetResources(mLoadResult.mResources);
        Dali::Scene3D::Loader::LoadGltfScene(mModelUrl, sdf, mLoadResult);
      }
      else
      {
        DALI_LOG_ERROR("Unsupported model type.\n");

        mModelCacheManager.SetSceneLoaded(mModelUrl, false);
        mModelCacheManager.SetSceneLoading(mModelUrl, false);
        mModelCacheManager.UnreferenceModelCache(mModelUrl);

        return;
      }

      mModelCacheManager.SetSceneLoaded(mModelUrl, true);
      mModelCacheManager.SetSceneLoading(mModelUrl, false);
    }
  }

  loadSceneConditionalWait.Notify();

  {
    ConditionalWait::ScopedLock lock(loadRawResourceConditionalWait);

    while(cacheRefCount > 1 && mLoadResult.mResources.mRawResourcesLoading)
    {
      loadRawResourceConditionalWait.Wait();
    }
  }

  {
    ConditionalWait::ScopedLock lock(loadRawResourceConditionalWait);

    for(auto iRoot : mLoadResult.mScene.GetRoots())
    {
      mResourceRefCounts.push_back(mLoadResult.mResources.CreateRefCounter());
      mLoadResult.mScene.CountResourceRefs(iRoot, mResourceChoices, mResourceRefCounts.back());
      mLoadResult.mResources.CountEnvironmentReferences(mResourceRefCounts.back());

      mLoadResult.mResources.LoadRawResources(mResourceRefCounts.back(), pathProvider);

      // glTF Mesh is defined in right hand coordinate system, with positive Y for Up direction.
      // Because DALi uses left hand system, Y direciton will be flipped for environment map sampling.
      for(auto&& env : mLoadResult.mResources.mEnvironmentMaps)
      {
        env.first.mYDirection = Y_DIRECTION;
      }
    }
  }

  loadRawResourceConditionalWait.Notify();

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

} // namespace Internal

} // namespace Scene3D

} // namespace Dali

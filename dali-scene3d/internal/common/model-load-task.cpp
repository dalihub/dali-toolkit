/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/trace.h>
#include <filesystem>

#ifdef TRACE_ENABLED
#include <chrono>
#include <iomanip>
#include <sstream>
#include <thread>
#endif

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
namespace
{
static constexpr Vector3 Y_DIRECTION(1.0f, -1.0f, 1.0f);

DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_MODEL_PERFORMANCE_MARKER, false);

#ifdef TRACE_ENABLED
uint64_t GetNanoseconds()
{
  // Get the time of a monotonic clock since its epoch.
  auto epoch    = std::chrono::steady_clock::now().time_since_epoch();
  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(epoch);
  return static_cast<uint64_t>(duration.count());
}
#endif
} // namespace

ModelLoadTask::ModelLoadTask(const std::string& modelUrl, const std::string& resourceDirectoryUrl, CallbackBase* callback)
: AsyncTask(callback, AsyncTask::PriorityType::LOW),
  mModelUrl(modelUrl),
  mResourceDirectoryUrl(resourceDirectoryUrl),
  mModelCacheManager(Scene3D::Internal::ModelCacheManager::Get()),
  mLoadResult(mModelCacheManager.GetModelLoadResult(mModelUrl)),
  mHasSucceeded(false)
{
  mModelCacheManager.ReferenceModelCache(mModelUrl);

  if(mResourceDirectoryUrl.empty())
  {
    std::filesystem::path modelUrl(mModelUrl);
    mResourceDirectoryUrl = std::string(modelUrl.parent_path()) + "/";
  }

  mModelLoader = std::make_unique<Dali::Scene3D::Loader::ModelLoader>(mModelUrl, mResourceDirectoryUrl, mLoadResult);
}

ModelLoadTask::~ModelLoadTask()
{
  mModelLoader.reset();

  mModelCacheManager.UnreferenceModelCache(mModelUrl);
}

void ModelLoadTask::Process()
{
#ifdef TRACE_ENABLED
  uint64_t mStartTimeNanoSceonds = 0;
  uint64_t mEndTimeNanoSceonds   = 0;
  if(gTraceFilter && gTraceFilter->IsTraceEnabled())
  {
    mStartTimeNanoSceonds = GetNanoseconds();
    DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_MODEL_LOADING_TASK", [&](std::ostringstream& oss)
                                            { oss << "[u:" << mModelUrl << ",dir:" << mResourceDirectoryUrl << "]"; });
  }
#endif

  Dali::Scene3D::Loader::ResourceBundle::PathProvider pathProvider = [&](Dali::Scene3D::Loader::ResourceType::Value type)
  {
    return mResourceDirectoryUrl;
  };

  bool loadSucceeded = false;

#ifdef TRACE_ENABLED
  bool useCachedModel = false;
#endif

  {
    // Lock model url during process, so let we do not try to load same model multiple times.
    mModelCacheManager.LockModelLoadScene(mModelUrl);
    if(mModelCacheManager.IsSceneLoaded(mModelUrl))
    {
#ifdef TRACE_ENABLED
      useCachedModel = true;
#endif

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
    mModelCacheManager.UnlockModelLoadScene(mModelUrl);
  }

#ifdef TRACE_ENABLED
  if(gTraceFilter && gTraceFilter->IsTraceEnabled())
  {
    mEndTimeNanoSceonds = GetNanoseconds();
    DALI_TRACE_END_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_MODEL_LOADING_TASK", [&](std::ostringstream& oss)
                                          {
      oss << std::fixed << std::setprecision(3);
      oss << "[";
      oss << "d:" << static_cast<float>(mEndTimeNanoSceonds - mStartTimeNanoSceonds) / 1000000.0f << "ms ";
      oss << "c?" << useCachedModel << " ";
      oss << "s?" << loadSucceeded << " ";
      oss << "dir:" << mResourceDirectoryUrl << " ";
      oss << "u:" << mModelUrl << "]"; });
  }
#endif
  if(!loadSucceeded)
  {
    DALI_LOG_ERROR("Failed to load scene from '%s'\n", mModelUrl.c_str());
    return;
  }

  mHasSucceeded = true;
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

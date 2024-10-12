#ifndef DALI_SCENE3D_MODEL_LOAD_TASK_H
#define DALI_SCENE3D_MODEL_LOAD_TASK_H

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
 */

// EXTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/async-task-manager.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/images/pixel-data.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/common/model-cache-manager.h>
#include <dali-scene3d/public-api/loader/load-result.h>
#include <dali-scene3d/public-api/loader/model-loader.h>
#include <dali-scene3d/public-api/loader/scene-definition.h>
#include <dali/devel-api/threading/conditional-wait.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
class ModelLoadTask;
typedef IntrusivePtr<ModelLoadTask> ModelLoadTaskPtr;

class ModelLoadTask : public AsyncTask
{
public:
  /**
   * Constructor
   * @param[in] modelUrl Model file path.(e.g., glTF, and DLI).
   * @param[in] resourceDirectoryUrl Resource file path that includes binary, image etc.
   * @param[in] callback The callback that is called when the operation is completed.
   */
  ModelLoadTask(const std::string& modelUrl, const std::string& resourceDirectoryUrl, CallbackBase* callback);

  /**
   * Destructor.
   */
  ~ModelLoadTask();

  /**
   * Whether the task has succeeded.
   * @return True if the task has succeeded.
   */
  bool HasSucceeded() const;

  /**
   * @brief Retrieves loaded scene
   * @return SceneDefinition that is loaded from file
   */
  Dali::Scene3D::Loader::SceneDefinition& GetScene() const;

  /**
   * @brief Retrieves resource bunder that includes resource information
   * @return ResourceBundle for model resources
   */
  Dali::Scene3D::Loader::ResourceBundle& GetResources() const;

  /**
   * @brief Retrieves loaded AnimationDefinition
   * @return AnimationDefinition that is loaded from file
   */
  std::vector<Dali::Scene3D::Loader::AnimationDefinition>& GetAnimations() const;

  /**
   * @brief Retrieves loaded CameraParameters
   * @return CameraParameters list that is loaded from file
   */
  std::vector<Dali::Scene3D::Loader::CameraParameters>& GetCameras() const;

  /**
   * @brief Retrieves ResourceChoices
   * @return Choices for loaded Resources
   */
  Dali::Scene3D::Loader::Customization::Choices& GetResourceChoices();

public: // Implementation of AsyncTask
  /**
   * @copydoc Dali::AsyncTask::Process()
   */
  void Process();

  /**
   * @copydoc Dali::AsyncTask::IsReady()
   */
  bool IsReady();

  /**
   * @copydoc Dali::AsyncTask::GetTaskName()
   */
  std::string_view GetTaskName() const override
  {
    return "ModelLoadTask";
  }

private:
  // Undefined
  ModelLoadTask(const ModelLoadTask& task) = delete;

  // Undefined
  ModelLoadTask& operator=(const ModelLoadTask& task) = delete;

  using ModelLoaderUniquePtr = std::unique_ptr<Dali::Scene3D::Loader::ModelLoader>;

  std::string                       mModelUrl;
  std::string                       mResourceDirectoryUrl;
  ModelLoaderUniquePtr              mModelLoader;
  ModelCacheManager                 mModelCacheManager;
  Dali::Scene3D::Loader::LoadResult mLoadResult;
  bool                              mHasSucceeded;
};

} // namespace Internal

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_LOAD_TASK_H

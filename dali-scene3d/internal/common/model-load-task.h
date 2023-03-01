#ifndef DALI_SCENE3D_MODEL_LOAD_TASK_H
#define DALI_SCENE3D_MODEL_LOAD_TASK_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/images/pixel-data.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/common/model-cache-manager.h>
#include <dali-scene3d/public-api/loader/load-result.h>
#include <dali-scene3d/public-api/loader/scene-definition.h>
#include <dali/devel-api/threading/conditional-wait.h>
#include <dali/public-api/adaptor-framework/async-task-manager.h>

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
   * Process the task
   */
  void Process() override;

  /**
   * Whether the task is ready to process.
   * @return True if the task is ready to process.
   */
  bool IsReady() override;

  /**
   * Whether the task has succeeded.
   * @return True if the task has succeeded.
   */
  bool HasSucceeded() const;

private:
  // Undefined
  ModelLoadTask(const ModelLoadTask& task) = delete;

  // Undefined
  ModelLoadTask& operator=(const ModelLoadTask& task) = delete;

public:
  std::string mModelUrl;
  std::string mResourceDirectoryUrl;

  Dali::Scene3D::Loader::Customization::Choices mResourceChoices;
  Dali::Scene3D::Loader::ResourceRefCounts      mResourceRefCount;
  bool                                          mHasSucceeded;

  ModelCacheManager                 mModelCacheManager;
  Dali::Scene3D::Loader::LoadResult mLoadResult;
};

} // namespace Internal

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_LOAD_TASK_H

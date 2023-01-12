#ifndef DALI_SCENE3D_MODEL_LOAD_TASK_H
#define DALI_SCENE3D_MODEL_LOAD_TASK_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali-scene3d/public-api/loader/load-result.h>
#include <dali-scene3d/public-api/loader/scene-definition.h>
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
   * @param[in] modelUrl model file path.(e.g., glTF, and DLI).
   * @param[in] resourceDirectoryUrl resource file path that includes binary, image etc.
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

  Dali::Scene3D::Loader::ResourceBundle                        mResources;
  Dali::Scene3D::Loader::SceneDefinition                       mScene;
  Dali::Scene3D::Loader::SceneMetadata                         mMetaData;
  std::vector<Dali::Scene3D::Loader::AnimationGroupDefinition> mAnimGroups;
  std::vector<Dali::Scene3D::Loader::CameraParameters>         mCameraParameters;
  std::vector<Dali::Scene3D::Loader::LightParameters>          mLights;
  std::vector<Dali::Scene3D::Loader::AnimationDefinition>      mAnimations;

  Dali::Scene3D::Loader::Customization::Choices         mResourceChoices;
  std::vector<Dali::Scene3D::Loader::ResourceRefCounts> mResourceRefCounts;
  bool                                                  mHasSucceeded;
};

} // namespace Internal

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_LOAD_TASK_H

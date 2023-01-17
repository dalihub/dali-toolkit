#ifndef DALI_SCENE3D_ENVIRONMENT_MAP_LOAD_TASK_H
#define DALI_SCENE3D_ENVIRONMENT_MAP_LOAD_TASK_H

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
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/adaptor-framework/async-task-manager.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/loader/load-result.h>
#include <dali-scene3d/public-api/loader/scene-definition.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
class EnvironmentMapLoadTask;
typedef IntrusivePtr<EnvironmentMapLoadTask> EnvironmentMapLoadTaskPtr;

class EnvironmentMapLoadTask : public AsyncTask
{
public:
  /**
   * Constructor
   * @param[in] environmentMapUrl The url of the environment map image file.
   * @param[in] callback The callback that is called when the operation is completed.
   */
  EnvironmentMapLoadTask(const std::string& environmentMapUrl, CallbackBase* callback);

  /**
   * Destructor.
   */
  ~EnvironmentMapLoadTask();

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

  /**
   * Retrieves loaded Environment Map
   * @return CubeData loaded from url.
   *
   * TODO: Supports Equirectangular environment map
   */
  Dali::Scene3D::Loader::CubeData GetEnvironmentMap() const;

private:
  // Undefined
  EnvironmentMapLoadTask(const EnvironmentMapLoadTask& task) = delete;

  // Undefined
  EnvironmentMapLoadTask& operator=(const EnvironmentMapLoadTask& task) = delete;

private:
  std::string                     mEnvironmentMapUrl;
  Dali::Scene3D::Loader::CubeData mEnvironmentMapPixelData;

  bool mIsReady;
  bool mHasSucceeded;
};

} // namespace Internal

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_ENVIRONMENT_MAP_LOAD_TASK_H

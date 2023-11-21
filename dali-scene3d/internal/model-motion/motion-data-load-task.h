#ifndef DALI_SCENE3D_MODEL_MOTION_MOTION_DATA_LOAD_TASK_H
#define DALI_SCENE3D_MODEL_MOTION_MOTION_DATA_LOAD_TASK_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/async-task-manager.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/loader/animation-definition.h>
#include <dali-scene3d/public-api/model-motion/motion-data.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
class MotionDataLoadTask;
typedef IntrusivePtr<MotionDataLoadTask> MotionDataLoadTaskPtr;

/**
 * @brief AsyncTask for load MotionData from various type.
 * After load finished, it will keep result as Loader::AnimationDefinition.
 */
class MotionDataLoadTask : public AsyncTask
{
public:
  enum class LoadMethod
  {
    BVH_FILE,
    BVH_BUFFER,
    FACIAL_FILE,
    FACIAL_BUFFER,
  };

public:
  /**
   * Constructor for load bvh from file.
   */
  MotionDataLoadTask(const std::string& path, bool useRootTranslationOnly, const Vector3& scale, CallbackBase* callback);

  /**
   * Constructor for load bvh from buffer.
   */
  MotionDataLoadTask(const uint8_t* rawBuffer, int rawBufferLength, bool useRootTranslationOnly, const Vector3& scale, CallbackBase* callback);

  /**
   * Constructor for load facial from file.
   */
  MotionDataLoadTask(const std::string& url, CallbackBase* callback);

  /**
   * Constructor for load facial from buffer.
   */
  MotionDataLoadTask(const uint8_t* rawBuffer, int rawBufferLength, CallbackBase* callback);

  /**
   * Destructor.
   */
  ~MotionDataLoadTask();

  /**
   * Process the task
   */
  void Process() override;

  /**
   * Whether the task is ready to process.
   * @return True if the task is ready to process.
   */
  bool IsReady() override;

public:
  const Scene3D::Loader::AnimationDefinition& GetAnimationDefinition() const;

private:
  // Undefined
  MotionDataLoadTask(const MotionDataLoadTask& task) = delete;

  // Undefined
  MotionDataLoadTask& operator=(const MotionDataLoadTask& task) = delete;

private:
  std::string mFileUrl;
  uint8_t*    mRawBuffer;
  int         mRawBufferLength;
  Vector3     mScale;
  bool        mUseRootTranslationOnly;

  Scene3D::Loader::AnimationDefinition mAnimationDefinition;
  LoadMethod                           mLoadMethod;
};

} // namespace Internal

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_MOTION_MOTION_DATA_LOAD_TASK_H
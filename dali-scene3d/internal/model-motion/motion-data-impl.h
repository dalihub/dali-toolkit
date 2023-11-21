#ifndef DALI_SCENE3D_MODEL_MOTION_MOTION_DATA_IMPL_H
#define DALI_SCENE3D_MODEL_MOTION_MOTION_DATA_IMPL_H

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
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>

#include <dali/public-api/common/vector-wrapper.h>
#include <utility> ///< for std::pair

// INTERNAL INCLUDES
#include <dali-scene3d/internal/model-motion/motion-data-load-task.h>
#include <dali-scene3d/public-api/loader/animation-definition.h>
#include <dali-scene3d/public-api/model-motion/motion-data.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
using MotionDataPtr = IntrusivePtr<MotionData>;

/**
 * @brief Internal data for Scene3D::MotionData.
 */
class MotionData : public BaseObject
{
public: // Creation & Destruction
  /**
   * @brief Create a new MotionData object.
   * @return A smart-pointer to the newly allocated MotionData.
   */
  static MotionDataPtr New();

protected:
  /**
   * @brief Construct a new MotionData.
   */
  MotionData();

  /**
   * @brief Second-phase constructor.
   */
  void Initialize();

  /**
   * @brief Virtual destructor.
   */
  virtual ~MotionData();

public: // Public Method
  /**
   * @copydoc Dali::Scene3D::MotionData::GetMotionCount()
   */
  uint32_t GetMotionCount() const;

  /**
   * @copydoc Dali::Scene3D::MotionData::GetIndex()
   */
  Scene3D::MotionIndex GetIndex(uint32_t index) const;

  /**
   * @copydoc Dali::Scene3D::MotionData::GetValue()
   */
  Scene3D::MotionValue GetValue(uint32_t index) const;

  /**
   * @copydoc Dali::Scene3D::MotionData::Add()
   */
  void Add(Scene3D::MotionIndex index, Scene3D::MotionValue value);

  /**
   * @copydoc Dali::Scene3D::MotionData::Clear()
   */
  void Clear();

  /**
   * @copydoc Dali::Scene3D::MotionData::SetDuration()
   */
  void SetDuration(float durationSeconds);

  /**
   * @copydoc Dali::Scene3D::MotionData::GetDuration()
   */
  float GetDuration() const;

  /**
   * @copydoc Dali::Scene3D::MotionData::LoadBvh()
   */
  void LoadBvh(const std::string& path, bool useRootTranslationOnly, const Vector3& scale = Vector3::ONE, bool synchronousLoad = false);

  /**
   * @copydoc Dali::Scene3D::MotionData::LoadBvhFromBuffer()
   */
  void LoadBvhFromBuffer(const uint8_t* rawBuffer, int rawBufferLength, bool useRootTranslationOnly, const Vector3& scale = Vector3::ONE, bool synchronousLoad = false);

  /**
   * @copydoc Dali::Scene3D::MotionData::LoadFacialAnimation()
   */
  void LoadFacialAnimation(const std::string& url, bool synchronousLoad = false);

  /**
   * @copydoc Dali::Scene3D::MotionData::LoadFacialAnimationFromBuffer()
   */
  void LoadFacialAnimationFromBuffer(const uint8_t* rawBuffer, int rawBufferLength, bool synchronousLoad = false);

public:
  /**
   * @copydoc Dali::Scene3D::MotionData::LoadCompletedSignal()
   */
  Scene3D::MotionData::LoadCompletedSignalType& LoadCompletedSignal()
  {
    return mLoadCompletedSignal;
  }

private:
  /**
   * @brief Request motion data load task process.
   *
   * @param[in] synchronousLoad True if we want to load result synchronously.
   */
  void RequestMotionDataLoad(bool synchronousLoad);

  /**
   * @brief Cancel requested motion data load task process.
   */
  void CancelMotionDataLoad();

private:
  /**
   * @brief Called when async load task completed.
   *
   * @param[in] task Completed tasks
   */
  void OnLoadCompleted(MotionDataLoadTaskPtr task);

private:
  // Asynchronous loading variable
  MotionDataLoadTaskPtr                        mMotionDataLoadTask{nullptr};
  Scene3D::MotionData::LoadCompletedSignalType mLoadCompletedSignal;

  std::vector<std::pair<Scene3D::MotionIndex, Scene3D::MotionValue>> mMotions{};

  float mDurationSeconds{0.0f}; ///< Duration of animation if we generate this motion data as Dali::Animation.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::MotionData& GetImplementation(Dali::Scene3D::MotionData& motionData)
{
  DALI_ASSERT_ALWAYS(motionData && "MotionData handle is empty");

  BaseObject& handle = motionData.GetBaseObject();

  return static_cast<Internal::MotionData&>(handle);
}

inline const Internal::MotionData& GetImplementation(const Dali::Scene3D::MotionData& motionData)
{
  DALI_ASSERT_ALWAYS(motionData && "MotionData handle is empty");

  const BaseObject& handle = motionData.GetBaseObject();

  return static_cast<const Internal::MotionData&>(handle);
}

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_MOTION_MOTION_DATA_IMPL_H
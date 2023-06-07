#ifndef DALI_SCENE3D_MODEL_MOTION_MOTION_DATA_H
#define DALI_SCENE3D_MODEL_MOTION_MOTION_DATA_H

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
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>
#include <dali-scene3d/public-api/model-motion/motion-index/motion-index.h>
#include <dali-scene3d/public-api/model-motion/motion-value.h>

namespace Dali
{
namespace Scene3D
{
//Forward declarations.
namespace Internal
{
class MotionData;
} // namespace Internal

/**
 * @addtogroup dali_scene3d_model_motion_motion_data
 * @{
 */

/**
 * @brief List of model motion definitions.
 * Each motion has pair of MotionIndex and MotionValue.
 * MotionIndex is abstract class that specify the target of motion.
 * MotionValue is target value of motion. It can be KeyFrames.
 *
 * We can generate list of motions by MotionIndex and MotionValue classes.
 *
 * @code
 *
 * MotionData motionData = MotionData::New(3.0f);
 *
 * // Make MotionIndex with MotionPropertyIndex
 * // Make MotionValue with Dali::Property::Value
 * motionData.Add(MotionPropertyIndex::New("nodeName", "color"), MotionValue::New(Color::RED));
 *
 * // Make MotionIndex with MotionTransformIndex
 * // Make MotionValue with Dali::KeyFrames
 * KeyFrames keyFrames = KeyFrames::New();
 * keyFrames.Add(0.0f, 0.0f);
 * keyFrames.Add(0.0f, 1.0f);
 * motionData.Add(MotionTransformIndex::New("nodeName", MotionTransformIndex::TransformType::POSITION_X), MotionValue::New(keyFrames));
 *
 * // Make MotionIndex with BlendShapeIndex
 * motionData.Add(BlendShapeIndex::New("nodeName", 0u), motionData.GetValue(1u));
 *
 * @endcode
 *
 * We can request to load MotionData from file or buffer asynchronously.
 * If load completed, LoadCompetedSignal will be emmited.
 *
 * @code
 *
 * MotionData motionData = MotionData::New();
 * motionData.LoadCompletedSignal().Connect(&OnLoadCompleted);
 * motionData.LoadBvh("bvhFilename.bvh", Vector3::ONE);
 *
 * @endcode
 *
 * We can generate animation of Scene3D::Model from MotionData class.
 * Or, just set values.
 *
 * @code
 *
 * // Generate animation from loaded Model
 * Dali::Animation animation = model.GenerateMotionDataAnimation(motionData);
 * animation.Play();
 *
 * // Set values from loaded Model.
 * model2.SetMotionData(motionData);
 *
 * @endcode
 * @note We don't check duplicated MotionIndex internally.
 * @SINCE_2_2.34
 */
class DALI_SCENE3D_API MotionData : public Dali::BaseHandle
{
public:
  /// @brief LoadCompleted signal type. @SINCE_2_2.34
  typedef Signal<void(MotionData)> LoadCompletedSignalType;

public: // Creation & Destruction
  /**
   * @brief Create an initialized MotionData.
   *
   * @SINCE_2_2.34
   * @return A handle to a newly allocated Dali resource
   */
  static MotionData New();

  /**
   * @brief Create an initialized MotionData with duration.
   *
   * @SINCE_2_2.34
   * @param[in] durationSeconds Duration of animation as seconds.
   * @return A handle to a newly allocated Dali resource
   */
  static MotionData New(float durationSeconds);

  /**
   * @brief Creates an uninitialized MotionData.
   *
   * Only derived versions can be instantiated. Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   *
   * @SINCE_2_2.34
   */
  MotionData();

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   *
   * @SINCE_2_2.34
   */
  ~MotionData();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_2_2.34
   * @param[in] motionData Handle to an object
   */
  MotionData(const MotionData& motionData);

  /**
   * @brief Move constructor
   *
   * @SINCE_2_2.34
   * @param[in] rhs A reference to the moved handle
   */
  MotionData(MotionData&& rhs) noexcept;

  /**
   * @brief Assignment operator.
   *
   * @SINCE_2_2.34
   * @param[in] motionData Handle to an object
   * @return reference to this
   */
  MotionData& operator=(const MotionData& motionData);

  /**
   * @brief Move assignment
   *
   * @SINCE_2_2.34
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  MotionData& operator=(MotionData&& rhs) noexcept;

  /**
   * @brief Downcasts an Object handle to MotionData.
   *
   * If handle points to a MotionData, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @SINCE_2_2.34
   * @param[in] handle Handle to an object
   * @return Handle to a MotionData or an uninitialized handle
   */
  static MotionData DownCast(BaseHandle handle);

public: // Public Method
  /**
   * @brief Get the number of motions what we added
   *
   * @SINCE_2_2.34
   * @return The number of motions
   */
  uint32_t GetMotionCount() const;

  /**
   * @brief Get MotionIndex from given index'th.
   *
   * @SINCE_2_2.34
   * @param[in] index The index of motion list.
   * @return Index of motion, or empty handle if invalid index inputed.
   */
  MotionIndex GetIndex(uint32_t index) const;

  /**
   * @brief Get MotionValue from given index'th.
   *
   * @SINCE_2_2.34
   * @param[in] index The index of motion list.
   * @return Value of motion, or empty handle if invalid index inputed.
   */
  MotionValue GetValue(uint32_t index) const;

  /**
   * @brief Append new motion.
   * @note We don't check duplicated MotionIndex internally.
   *
   * @SINCE_2_2.34
   * @param[in] index index of motion.
   * @param[in] value value of motion.
   */
  void Add(MotionIndex index, MotionValue value);

  /**
   * @brief Clear all stored motion data.
   *
   * @SINCE_2_2.34
   */
  void Clear();

  /**
   * @brief Set the duration of this motion data if it be generated as Dali::Animation.
   *
   * @SINCE_2_2.34
   * @param[in] durationSeconds Duration of animation as seconds.
   */
  void SetDuration(float durationSeconds);

  /**
   * @brief Get the duration of this motion data if it be generated as Dali::Animation.
   *
   * @SINCE_2_2.34
   * @return The duration of this motion data seconds. Default is 0.0f
   */
  float GetDuration() const;

  /**
   * @brief Load MotionData from bvh file.
   * It will use Dali::Scene3D::Loader::LoadBvh() internally.
   * LoadCompleteSignal() will be emitted after load completed.
   *
   * @SINCE_2_2.34
   * @param[in] path The file path.
   * @param[in] scale The scale factor to set on the position property manually.
   * @param[in] synchronousLoad True if we want to load result synchronously. Default is false.
   */
  void LoadBvh(const std::string& path, const Vector3& scale = Vector3::ONE, bool synchronousLoad = false);

  /**
   * @brief Load MotionData from bvh buffer.
   * It will use Dali::Scene3D::Loader::LoadBvhFromBuffer() internally.
   * LoadCompleteSignal() will be emitted after load completed.
   *
   * @SINCE_2_2.34
   * @param[in] rawBuffer The bvh buffer containing the facial animation as bvh format string.
   * @param[in] rawBufferLength The length of buffer.
   * @param[in] scale The scale factor to set on the position property manually.
   * @param[in] synchronousLoad True if we want to load result synchronously. Default is false.
   */
  void LoadBvhFromBuffer(const uint8_t* rawBuffer, int rawBufferLength, const Vector3& scale = Vector3::ONE, bool synchronousLoad = false);

  /**
   * @brief Load MotionData from facial defined json file.
   * It will use Dali::Scene3D::Loader::LoadFacialAnimation() internally.
   * LoadCompleteSignal() will be emitted after load completed.
   *
   * @SINCE_2_2.34
   * @param[in] url The file path.
   * @param[in] synchronousLoad True if we want to load result synchronously. Default is false.
   */
  void LoadFacialAnimation(const std::string& url, bool synchronousLoad = false);

  /**
   * @brief Load MotionData from facial defined json file.
   * It will use Dali::Scene3D::Loader::LoadFacialAnimationFromBuffer() internally.
   * LoadCompleteSignal() will be emitted after load completed.
   *
   * @SINCE_2_2.34
   * @param[in] rawBuffer The raw buffer containing the facial animation as json format string.
   * @param[in] rawBufferLength The length of raw buffer.
   * @param[in] synchronousLoad True if we want to load result synchronously. Default is false.
   */
  void LoadFacialAnimationFromBuffer(const uint8_t* rawBuffer, int rawBufferLength, bool synchronousLoad = false);

public:
  /**
   * @brief This signal is emitted after motion data are loaded completed.
   * @note Signal will be emitted even if we request load synchronously.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(MotionData motionData);
   * @endcode
   *
   * @SINCE_2_2.34
   * @return The signal to connect to.
   */
  LoadCompletedSignalType& LoadCompletedSignal();

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Scene3D::Internal implementation.
   *
   * @param[in] implementation The MotionData implementation
   */
  DALI_INTERNAL MotionData(Dali::Scene3D::Internal::MotionData* implementation);
  /// @endcond
};

/**
 * @}
 */

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_MOTION_MOTION_DATA_H
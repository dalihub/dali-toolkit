#ifndef DALI_SCENE3D_MODEL_MOTION_MOTION_VALUE_H
#define DALI_SCENE3D_MODEL_MOTION_MOTION_VALUE_H

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
 *
 */

// EXTERNAL INCLUDES
#include <dali/public-api/animation/key-frames.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/property-value.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>

namespace Dali
{
namespace Scene3D
{
//Forward declarations.
namespace Internal
{
class MotionValue;
} // namespace Internal

/**
 * @addtogroup dali_scene3d_model_motion
 * @{
 */

/**
 * @brief This MotionValue be used for target value of each MotionIndex.
 * We can get and set MotionValue as 2 types : Property::Value and KeyFrames.
 *
 * Each types will be cross-converted internally.
 * For example, when we set Property::Value, we can get KeyFrames with 2 frames, and target value is setted.
 *
 * @note The type of property should be matched with MotionIndex required.
 * @SINCE_2_2.34
 */
class DALI_SCENE3D_API MotionValue : public Dali::BaseHandle
{
public:
  /**
   * @brief Determine whether current stored value is PropertyValue, or KeyFrames.
   * @SINCE_2_2.34
   */
  enum class ValueType
  {
    INVALID = -1, ///< Value is null, or invalid.

    PROPERTY_VALUE = 0, ///< Value is PropertyValue.
    KEY_FRAMES,         ///< Value is KeyFrames.
  };

public: // Creation & Destruction
  /**
   * @brief Create an initialized MotionValue.
   *
   * @SINCE_2_2.34
   * @return A handle to a newly allocated Dali resource
   */
  static MotionValue New();

  /**
   * @brief Create an initialized MotionValue with Property::Value.
   *
   * @SINCE_2_2.34
   * @param[in] propertyValue The static value.
   * @return A handle to a newly allocated Dali resource
   */
  static MotionValue New(Property::Value propertyValue);

  /**
   * @brief Create an initialized MotionValue with KeyFrames.
   *
   * @SINCE_2_2.34
   * @param[in] keyFrames The keyframes value.
   * @return A handle to a newly allocated Dali resource
   */
  static MotionValue New(KeyFrames keyFrames);

  /**
   * @brief Creates an uninitialized MotionValue.
   *
   * Only derived versions can be instantiated. Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   *
   * @SINCE_2_2.34
   */
  MotionValue();

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   *
   * @SINCE_2_2.34
   */
  ~MotionValue();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_2_2.34
   * @param[in] motionValue Handle to an object
   */
  MotionValue(const MotionValue& motionValue);

  /**
   * @brief Move constructor
   *
   * @SINCE_2_2.34
   * @param[in] rhs A reference to the moved handle
   */
  MotionValue(MotionValue&& rhs) noexcept;

  /**
   * @brief Assignment operator.
   *
   * @SINCE_2_2.34
   * @param[in] motionValue Handle to an object
   * @return reference to this
   */
  MotionValue& operator=(const MotionValue& motionValue);

  /**
   * @brief Move assignment
   *
   * @SINCE_2_2.34
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  MotionValue& operator=(MotionValue&& rhs) noexcept;

  /**
   * @brief Downcasts an Object handle to MotionValue.
   *
   * If handle points to a MotionValue, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @SINCE_2_2.34
   * @param[in] handle Handle to an object
   * @return Handle to a MotionValue or an uninitialized handle
   */
  static MotionValue DownCast(BaseHandle handle);

public: // Public Method
  /**
   * @brief Get the type of value this MotionValue hold.
   * The type of value will be changed what we set.
   *
   * @SINCE_2_2.34
   * @return Type of value.
   */
  ValueType GetValueType() const;

  /**
   * @brief Set the value as Property::Value type.
   *
   * @SINCE_2_2.34
   * @param[in] propertyValue The static value.
   */
  void SetValue(Property::Value propertyValue);

  /**
   * @brief Set the value as KeyFrames type.
   *
   * @SINCE_2_2.34
   * @param[in] keyFrames The keyframes value.
   */
  void SetValue(KeyFrames keyFrames);

  /**
   * @brief Make value as Invalid type.
   *
   * @SINCE_2_2.34
   */
  void Invalidate();

  /**
   * @brief Make value as Invalid type.
   * @see Dali::Scene3D::MotionValue::Invalidate()
   *
   * @SINCE_2_2.34
   */
  void Clear();

  /**
   * @brief Get the value as Property::Value.
   * If ValueType is KEY_FRAMES, it will return last value of stored KeyFrames.
   *
   * @SINCE_2_2.34
   * @return Property value, or empty if it is invalid.
   */
  Property::Value GetPropertyValue() const;

  /**
   * @brief Get the value as KeyFrames
   * If ValueType is PROPERTY_VALUE, it will create new KeyFrames by stored Property::Value.
   *
   * @SINCE_2_2.34
   * @return Keyframes handle, or empty if it is invalid.
   */
  KeyFrames GetKeyFrames() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Scene3D::Internal implementation.
   *
   * @param[in] implementation The MotionValue implementation
   */
  DALI_INTERNAL MotionValue(Dali::Scene3D::Internal::MotionValue* implementation);
  /// @endcond
};

/**
 * @}
 */

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_MOTION_MOTION_VALUE_H

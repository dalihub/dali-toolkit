#ifndef __DALI_TOOLKIT_SHEAR_EFFECT_H__
#define __DALI_TOOLKIT_SHEAR_EFFECT_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/dali.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

/**
 * ShearEffect is a custom shader effect to achieve shear effects in Image actors
 */
class ShearEffect : public ShaderEffect
{
public:

  /**
   * Create an uninitialized ShearEffect; this can be initialized with ShearEffect::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  ShearEffect();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~ShearEffect();

  /**
   * Create an initialized ShearEffect.
   * @return A handle to a newly allocated Dali resource.
   */
  static ShearEffect New();

  /**
   * Set the center point of the shear effect in screen coordinates.
   * @param [in] center The new center point.
   */
  void SetCenter(const Vector2& center);

  /**
   * Set the angle of the shear effect in the X axis.
   * @param [in] angle The new angle.
   */
  void SetAngleXAxis(float angle);

  /**
   * Set the angle of the shear effect in the Y axis.
   * @param [in] angle The new angle.
   */
  void SetAngleYAxis(float angle);

  /**
   * Get the name for the center property
   * @return A std::string containing the property name
   */
  const std::string& GetCenterPropertyName() const;

  /**
   * Get the name for the X axis property
   * @return A std::string containing the property name
   */
  const std::string& GetAngleXAxisPropertyName() const;

  /**
   * Get the name for the Y axis property
   * @return A std::string containing the property name
   */
  const std::string& GetAngleYAxisPropertyName() const;


private: // Not intended for application developers
  ShearEffect(ShaderEffect handle);
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SHEAR_EFFECT_H__

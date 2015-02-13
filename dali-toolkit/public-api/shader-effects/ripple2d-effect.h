#ifndef __DALI_TOOLKIT_SHADER_EFFECT_RIPPLE2D_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_RIPPLE2D_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/shader-effects/shader-effect.h>

namespace Dali
{

namespace Toolkit
{

/**
 * @brief Ripple2DEffect is a custom shader effect to achieve 2d ripple effects on Image actors.
 */
class DALI_IMPORT_API Ripple2DEffect : public ShaderEffect
{
public:

  /**
   * @brief Create an uninitialized Ripple2DEffect; this can be initialized with Ripple2DEffect::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  Ripple2DEffect();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Ripple2DEffect();

  /**
   * @brief Create an initialized Ripple2DEffect.
   *
   * @return A handle to a newly allocated Dali resource.
   */
  static Ripple2DEffect New();

  /**
   * @brief Set the amplitude of the 2d ripple.
   *
   * @param[in] amplitude The amplitude in float.
   */
  void SetAmplitude(float amplitude);

  /**
   * @brief Set the time duration for the 2d ripple.
   *
   * @param[in] time The time duration in float.
   */
  void SetTime(float time);

  /**
   * @brief Get the name for the amplitude property.
   *
   * @return A std::string containing the property name
   */
  const std::string& GetAmplitudePropertyName() const;

  /**
   * @brief Get the name for the time property.
   *
   * which can be used in Animation API's
   * @return A std::string containing the property name
   */
  const std::string& GetTimePropertyName() const;

private:
  DALI_INTERNAL Ripple2DEffect(ShaderEffect handle);

};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SHADER_EFFECT_RIPPLE2D_H__

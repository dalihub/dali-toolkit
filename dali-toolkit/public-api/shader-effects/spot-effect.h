#ifndef __DALI_TOOLKIT_SHADER_EFFECT_SPOT_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_SPOT_H__

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
 * SpotEffect2D is a custom shader effect to achieve spot effects on Image actors
 */
class DALI_IMPORT_API SpotEffect : public ShaderEffect
{
public:

  /**
   * Create an uninitialized SpotEffect; this can be initialized with SpotEffect::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  SpotEffect();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~SpotEffect();

  /**
   * Create an initialized SpotEffect.
   * @return A handle to a newly allocated Dali resource.
   */
  static SpotEffect New();

  /**
   * Set the center of the spot.
   * @param[in] center The center in Vector2.
   */
  void SetCenter(const Vector2& center);

  /**
   * Set the radius of the spot.
   * @param[in] radius The radius in float.
   */
  void SetRadius(float radius);

  /**
   * Get the name for the center property
   * @return A std::string containing the property name
   */
  const std::string& GetCenterPropertyName() const;

  /**
   * Get the name for the radius property
   * @return A std::string containing the property name
   */
  const std::string& GetRadiusPropertyName() const;

private:
  DALI_INTERNAL SpotEffect(ShaderEffect handle);
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SHADER_EFFECT_SPOT_H__

#ifndef __DALI_TOOLKIT_SHADER_EFFECT_RIPPLE_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_RIPPLE_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// INTERNAL INCLUDES
#include <dali/dali.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

/**
 * RippleEffect is a custom shader effect to achieve ripple effects on Image actors
 */
class RippleEffect : public ShaderEffect
{
public:

  /**
   * Create an uninitialized RippleEffect; this can be initialized with RippleEffect::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  RippleEffect();

  /**
   * Virtual destructor.
   */
  virtual ~RippleEffect();

  /**
   * Create an initialized RippleEffect.
   * @return A handle to a newly allocated Dali resource.
   */
  static RippleEffect New();

  /**
   * Set the amplitude of the effect.
   * @param [in] amplitude The new amplitude.
   */
  void SetAmplitude(float amplitude);

  /**
   * Set the center point of the effect as screen coordinates.
   * @param [in] center The new center point.
   */
  void SetCenter(const Vector2& center);

  /**
   * Set the time duration for the ripple.
   * @param[in] time The time duration in float.
   */
  void SetTime(float time);

  /**
   * Get the name for the amplitude property
   * @return A std::string containing the property name
   */
  const std::string& GetAmplitudePropertyName() const;

  /**
   * Get the name for the center property
   * which can be used in Animation API's
   * @return A std::string containing the property name
   */
  const std::string& GetCenterPropertyName() const;

  /**
   * Get the name for the time property
   * which can be used in Animation API's
   * @return A std::string containing the property name
   */
  const std::string& GetTimePropertyName() const;

private:
  RippleEffect(ShaderEffect handle);

};

} // namespace Toolkit

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TOOLKIT_SHADER_EFFECT_RIPPLE_H__

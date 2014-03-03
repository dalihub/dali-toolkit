#ifndef __DALI_TOOLKIT_SHADER_EFFECT_SWIRL_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_SWIRL_H__

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
 * SwirlEffect is a custom shader effect to achieve swirl effects in Image actors
 */
class SwirlEffect : public ShaderEffect
{
public:

  /**
   * Create an uninitialized SwirlEffect; this can be initialized with SwirlEffect::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  SwirlEffect();

  /**
   * Virtual destructor.
   */
  virtual ~SwirlEffect();

  /**
   * Create an initialized SwirlEffect.
   * @return A handle to a newly allocated Dali resource.
   */
  static SwirlEffect New(bool warp);

  /**
   * Set the angle of the swirl.
   * @param[in] angle The angle in float.
   */
  void SetAngle(float angle);

  /**
   * Set the center of the swirl.
   * @param[in] center The center in Vector2.
   */
  void SetCenter(const Vector2& center);

  /**
   * Set the radius of the swirl.
   * @param[in] radius The radius in float.
   */
  void SetRadius(float radius);

  /**
   * Get the name for the angle property
   * @return A std::string containing the property name
   */
  const std::string& GetAnglePropertyName() const;

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

private: // Not intended for application developers
  SwirlEffect(ShaderEffect handle);
};

} // namespace Toolkit

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TOOLKIT_SHADER_EFFECT_SWIRL_H__

#ifndef __DALI_TOOLKIT_SHADER_EFFECT_BENDY_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_BENDY_H__

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

// INTERNAL INCLUDES
#include <dali/dali.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

/**
 * BendyEffect is a custom shader effect to achieve bendy effects in Image actors
 */
class BendyEffect : public ShaderEffect
{
public:

  /**
   * Create an uninitialized BendyEffect; this can be initialized with BendyEffect::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  BendyEffect();

  /**
   * Virtual destructor.
   */
  virtual ~BendyEffect();

  /**
   * Create an initialized BendyEffect.
   * @return A handle to a newly allocated Dali resource.
   */
  static BendyEffect New();

  /**
   * Set the center point of the bendy effect.
   * @param [in] center The new center point.
   */
  void SetCenter(const Vector2& center);

  /**
   * Set the direction of the bendy effect.
   * @param [in] direction The new direction.
   */
  void SetDirection(const Vector2& direction);

  /**
   * Set the radius of the bendy effect.
   * @param [in] radius The new radius.
   */
  void SetRadius(float radius);

  /**
   * Get the name for the center property
   * @return A std::string containing the property name
   */
  const std::string& GetCenterPropertyName() const;

  /**
   * Get the name for the direction property
   * which can be used in Animation API's
   * @return A std::string containing the property name
   */
  const std::string& GetDirectionPropertyName() const;

  /**
   * Get the name for the radius property
   * which can be used in Animation API's
   * @return A std::string containing the property name
   */
  const std::string& GetRadiusPropertyName() const;


private: // Not intended for application developers
  BendyEffect(ShaderEffect handle);
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SHADER_EFFECT_BENDY_H__

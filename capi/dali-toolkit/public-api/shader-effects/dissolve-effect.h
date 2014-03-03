#ifndef __DALI_TOOLKIT_SHADER_EFFECT_DISSOLVE_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_DISSOLVE_H__

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
 * DissolveEffect is a custom shader effect to achieve Dissolve effects in Image actors
 */
class DissolveEffect : public ShaderEffect
{
public:

  /**
   * Create an uninitialized DissolveEffect; this can be initialized with DissolveEffect::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  DissolveEffect();

  /**
   * Virtual destructor.
   */
  virtual ~DissolveEffect();

  /**
   * Create an initialized DissolveEffect.
   * @param[in] useHighPrecision True if using high precision in fragment shader for fully random noise, false otherwise
   * @return A handle to a newly allocated Dali resource.
   */
  static DissolveEffect New( bool useHighPrecision = true);

  /**
   * Set the dissolve central line
   * Use one point (position) and one direction ( displacement ) vector to define this line
   * As we use the texture coordinate as pixel position to calculate random offset,
   * the line should passing through rectangle {(0,0),(0,1),(1,0),(1,1)},
   * so make the position parameter with two component values between 0.0 to 1.0
   * @param[in] position The point ( locates within rectangle {(0,0),(0,1),(1,0),(1,1)} ) passed through by the central line
   * @param[in] displacement The direction of the central line
   */
  void SetCentralLine( const Vector2& position, const Vector2& displacement );

  /**
   * Sets the distortion applied to the effect texture.
   * This value is proportional to the distortion applied; a value of zero means no distortion.
   * @param [in] distortion The distortion value.
   */
  void SetDistortion( float distortion );

  /**
   * Get the name for the distortion property
   * @return A std::string containing the property name
   */
  const std::string& GetDistortionPropertyName() const;

private: // Not intended for application developers
  DissolveEffect(ShaderEffect handle);
};

} // namespace Toolkit

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TOOLKIT_SHADER_EFFECT_DISSOLVE_H__

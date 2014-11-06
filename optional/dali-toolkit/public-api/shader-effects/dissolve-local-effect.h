#ifndef __DALI_TOOLKIT_SHADER_EFFECT_LOCAL_DISSOLVE_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_LOCAL_DISSOLVE_H__

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
 * DissolveLocalEffect is a custom shader effect to achieve Dissolve effects in multiple small areas of Image actors
 */
class DALI_IMPORT_API DissolveLocalEffect : public ShaderEffect
{
public:

  /**
   * Create an uninitialized DissolveLocalEffect; this can be initialized with DissolveLocalEffect::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  DissolveLocalEffect();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~DissolveLocalEffect();

  /**
   * Create an initialized DissolveLocalEffect.
   * @param[in] numberOfDimples The number of dimples
   * @return A handle to a newly allocated Dali resource.
   */
  static DissolveLocalEffect New( unsigned int numberOfDimples );

  /**
   * Get the number of dimples the shader supports.
   * @return The number of dimples in the shader.
   */
  unsigned int GetNumberOfDimples() const;

  /**
  * Set the transparency of the drifted pixels.
  * @param[in] transparency The transparency of the drifted pixels.
  */
  void SetTransparency( float transparency);

  /**
   * Set the certer position of a dimple.
   * @param[in] index The index of the dimple to change.
   * @param[in] center The center position of the dimple.
   * @pre index has to be between 0 and GetNumberOfDimples() - 1
   */
  void SetCenter( unsigned int index, const Vector2& center );

  /**
   * Set the propogation radius of a dimple.
   * @param[in] index The index of the dimple to change.
   * @param[in] radius The propagation radius of the dimple.
   * @pre index has to be between 0 and GetNumberOfDimples() - 1
   */
  void SetRadius( unsigned int index, float radius );

  /**
   * Sets the distortion applied to the effect texture.
   * This value is proportional to the distortion applied; a value of zero means no distortion.
   * @param[in] index The index of the dimple to change.
   * @param[in] distortion The distortion value.
   * @pre index has to be between 0 and GetNumberOfDimples() - 1
   */
  void SetDistortion( unsigned int index, float distortion );

  /**
   * Get the name of the center property of a dimple.
   * @param[in] index The index of the dimple.
   * @return A std::string containing the property name.
   * @pre index has to be between 0 and GetNumberOfDimples() - 1
   */
  std::string GetCenterPropertyName( unsigned int index ) const;

  /**
   * Get the name of the radius property of a dimple.
   * @param[in] index The index of the dimple.
   * @return A std::string containing the property name
   * @pre index has to be between 0 and GetNumberOfDimples() - 1
   */
  std::string GetRadiusPropertyName( unsigned int index ) const;

  /**
   * Get the name for the distortion property of a dimple
   * @param[in] index the index of a dimple.
   * @return A std::string containing the property name
   * @pre index has to be between 0 and GetNumberOfDimples() - 1
   */
  std::string GetDistortionPropertyName( unsigned int index ) const;

private: // Not intended for application developers

  DALI_INTERNAL DissolveLocalEffect( ShaderEffect handle );

private:

  unsigned int mNumberOfDimples;  ///< The number of dimples the shader supports

};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SHADER_EFFECT_LOCAL_DISSOLVE_H__

#ifndef __DALI_TOOLKIT_WATER_EFFECT_H__
#define __DALI_TOOLKIT_WATER_EFFECT_H__

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

namespace Internal DALI_INTERNAL
{

/**
 * WaterEffect implementation class
 */
class WaterEffect;

} // namespace Internal

/**
 * WaterEffect is a custom shader effect to achieve water like effects on Image actors
 *
 * Usage example:
 *
 *   WaterEffect waterEffect = WaterEffect::New( numberOfDrops );
 *
 *   actor.SetShaderEffect( waterEffect );
 *
 *   // Set initial values
 *   waterEffect.SetCenter( 1, centerPosition );
 *   waterEffect.SetPropagation( 1, INITIAL_RADIUS );
 *
 *   // Animate the wave propagation
 *   std::string propertyName = waterEffect.GetPropagationPropertyName( 1 );
 *   animation.AnimateTo( Property(waterEffect, propertyName), FINAL_RADIUS );
 */
class WaterEffect : public ShaderEffect
{
public:

  /**
   * Create an uninitialized WaterEffect; this can be initialized with WaterEffect::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  WaterEffect();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~WaterEffect();

  /**
   * Create an initialized WaterEffect.
   * @param [in] numberOfWaves The number of waves.
   * @return A handle to a newly allocated Dali resource
   */
  static WaterEffect New( unsigned int numberOfWaves );

  /**
   * Get the number of waves the shader supports.
   * @return The number of waves in the shader.
   */
  unsigned int GetNumberOfWaves() const;

  /**
   * Set the amplitude of a wave.
   * @param [in] index The index of the wave to change
   * @param [in] amplitude The new amplitude of the wave
   * @pre index has to be between 0 and GetNumberOfWaves() - 1
   */
  void SetAmplitude( unsigned int index, float amplitude );

  /**
   * Set the center point of a wave in texture coordinates.
   * @param [in] index The index of the wave to change
   * @param [in] center The center point of the wave
   * @pre index has to be between 0 and GetNumberOfWaves() - 1
   */
  void SetCenter( unsigned int index, const Vector2& center );

  /**
   * Set the propagation radius of a wave.
   * @param [in] index The index of the wave to change
   * @param [in] radius The propagation radius
   * @pre index has to be between 0 and GetNumberOfWaves() - 1
   */
  void SetPropagation( unsigned int index, float radius );

  /**
   * Get the amplitude of a wave.
   * @param [in] index The index of the wave
   * @return The new amplitude of the wave
   * @pre index has to be between 0 and GetNumberOfWaves() - 1
   */
  float GetAmplitude( unsigned int index ) const;

  /**
   * Get the center point of a wave in texture coordinates.
   * @param [in] index The index of the wave
   * @return The center point of the wave
   * @pre index has to be between 0 and GetNumberOfWaves() - 1
   */
  Vector2 GetCenter( unsigned int index ) const;

  /**
   * Get the propagation radius of a wave.
   * @param [in] index The index of the wave
   * @return The propagation radius
   * @pre index has to be between 0 and GetNumberOfWaves() - 1
   */
  float GetPropagation( unsigned int index ) const;

  /**
   * Get the name for the amplitude property of a wave.
   * @param [in] index The index of the wave
   * @return A std::string containing the property name
   * @pre index has to be between 0 and GetNumberOfWaves() - 1
   */
  std::string GetAmplitudePropertyName( unsigned int index ) const;

  /**
   * Get the name for the center property of a wave.
   * @param [in] index The index of the wave
   * @return A std::string containing the property name
   * @pre index has to be between 0 and GetNumberOfWaves() - 1
   */
  std::string GetCenterPropertyName( unsigned int index ) const;

  /**
   * Get the name for the propagation property.
   * @param [in] index The index of the wave
   * @return A std::string containing the property name
   * @pre index has to be between 0 and GetNumberOfWaves() - 1
   */
  std::string GetPropagationPropertyName( unsigned int index ) const;

public: // Not intended for developer use

  WaterEffect( ShaderEffect handle, Internal::WaterEffect* shaderExtension );
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_WATER_EFFECT_H__

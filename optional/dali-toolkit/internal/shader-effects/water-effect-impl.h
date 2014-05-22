#ifndef __DALI_TOOLKIT_INTERNAL_WATER_EFFECT_H__
#define __DALI_TOOLKIT_INTERNAL_WATER_EFFECT_H__

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
#include <sstream>
#include <cmath>

// INTERNAL INCLUDES
#include <dali/dali.h>
#include <dali-toolkit/public-api/shader-effects/water-effect.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * WaterEffect implementation class
 */
class WaterEffect : public ShaderEffect::Extension
{
public:

  /**
   * @copydoc Dali::Toolkit::WaterEffect::WaterEffect
   */
  WaterEffect( unsigned int numberOfWaves );

  /**
   * @copydoc Dali::Toolkit::WaterEffect::~WaterEffect
   */
  virtual ~WaterEffect();

  /**
   * @copydoc Dali::Toolkit::WaterEffect::GetNumberOfWaves
   */
  unsigned int GetNumberOfWaves() const;

  /**
   * @copydoc Dali::Toolkit::WaterEffect::CreateShaderEffect
   */
  static Dali::Toolkit::WaterEffect CreateShaderEffect( unsigned int numberOfWaves );

  /**
   * @copydoc Dali::Toolkit::WaterEffect::SetAmplitude
   */
  void SetAmplitude( unsigned int index, float amplitude );

  /**
   * @copydoc Dali::Toolkit::WaterEffect::SetCenter
   */
  void SetCenter( unsigned int index, const Vector2& center );

  /**
   * @copydoc Dali::Toolkit::WaterEffect::SetPropagation
   */
  void SetPropagation( unsigned int index, float radius );

  /**
   * @copydoc Dali::Toolkit::WaterEffect::GetPropagation
   */
  float GetPropagation( unsigned int index ) const;

  /**
   * @copydoc Dali::Toolkit::WaterEffect::GetAmplitude
   */
  float GetAmplitude( unsigned int index ) const;

  /**
   * @copydoc Dali::Toolkit::WaterEffect::GetCenter
   */
  Vector2 GetCenter( unsigned int index ) const;

  /**
   * @copydoc Dali::Toolkit::WaterEffect::GetAmplitudePropertyName
   */
  std::string GetAmplitudePropertyName( unsigned int index ) const;

  /**
   * @copydoc Dali::Toolkit::WaterEffect::GetCenterPropertyName
   */
  std::string GetCenterPropertyName( unsigned int index ) const;

  /**
   * @copydoc Dali::Toolkit::WaterEffect::GetPropagationPropertyName
   */
  std::string GetPropagationPropertyName( unsigned int index ) const;

private:

  void Initialize( ShaderEffect shaderEffect );

private:

  ShaderEffect mShaderEffect;

  unsigned int mNumberOfWaves;


private:

  // Undefined copy constructor.
  WaterEffect( const WaterEffect& );

  // Undefined assignment operator.
  WaterEffect& operator=( const WaterEffect& );

};

} // namespace Internal

inline Internal::WaterEffect& GetImpl( Toolkit::WaterEffect& waterEffect )
{
  return static_cast<Internal::WaterEffect&>( waterEffect.GetExtension() );
}

inline const Internal::WaterEffect& GetImpl( const Toolkit::WaterEffect& waterEffect )
{
  return static_cast<const Internal::WaterEffect&>( waterEffect.GetExtension() );
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_WATER_EFFECT_H__

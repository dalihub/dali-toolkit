#ifndef __DALI_TOOLKIT_SHADER_EFFECT_SQUARE_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_SQUARE_H__

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
 * SquareDissolveEffect is a custom shader effect to achieve square effects in Image actors
 */
class DALI_IMPORT_API SquareDissolveEffect : public ShaderEffect
{
public:

  /**
   * Create an uninitialized SquareDissolveEffect; this can be initialized with BendyEffect::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  SquareDissolveEffect();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~SquareDissolveEffect();

  /**
   * Create an initialized SquareDissolveEffect.
   * @return A handle to a newly allocated Dali resource.
   */
  static SquareDissolveEffect New();

  /**
   * Set the step of the square effect.
   * @param [in] step the new step.
   */
  void SetStep(float step);

  /**
   * Set the rows of the square dissolve effect.
   * @param [in] rows the new rows value.
   */
  void SetRows(float rows);

  /**
   * Set the columns of the square dissolve effect.
   * @param [in] columns the new columns value.
   */
  void SetColumns(float columns);

  /**
   * Set the texture size of the square dissolve.
   * @param[in] textureSize The texture size in Vector2.
   */
  void SetTextureSize(const Vector2& textureSize);

  /**
   * Get the name for the step property
   * which can be used in Animation API's
   * @return A std::string containing the property name
   */
  const std::string& GetStepPropertyName() const;

  /**
   * Get the name for the rows property
   * which can be used in Animation API's
   * @return A std::string containing the property name
   */
  const std::string& GetRowsPropertyName() const;

  /**
   * Get the name for the columns property
   * which can be used in Animation API's
   * @return A std::string containing the property name
   */
  const std::string& GetColumnsPropertyName() const;

  /**
   * Get the name for the texSize property
   * which can be used in Animation API's
   * @return A std::string containing the property name
   */
  const std::string& GetTexSizePropertyName() const;

private: // Not intended for application developers
  DALI_INTERNAL SquareDissolveEffect(ShaderEffect handle);
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SHADER_EFFECT_SQUARE_H__

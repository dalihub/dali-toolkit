#ifndef __DALI_TOOLKIT_SHADER_EFFECT_BLIND_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_BLIND_H__

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

/**
 * BlindEffect is a custom shader effect to achieve blind effects in Image actors
 */
class BlindEffect : public ShaderEffect
{
public:

  /**
   * Create an uninitialized BlindEffect; this can be initialized with BlindEffect::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  BlindEffect();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~BlindEffect();

  /**
   * Create an initialized ~BlindEffect.
   * @return A handle to a newly allocated Dali resource.
   */
  static BlindEffect New();


  /**
   * Set the step of the blind effect.
   * @param [in] step The step
   */
  void SetStep(float step);

  /**
   * Get the name for the step property
   * which can be used in Animation API's
   * @return A std::string containing the property name
   */
  const std::string& GetStepPropertyName() const;

private: // Not intended for application developers
  BlindEffect(ShaderEffect handle);
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SHADER_EFFECT_BLIND_H__

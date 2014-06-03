#ifndef __DALI_TOOLKIT_SHADER_EFFECT_DISTANCEFIELD_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_DISTANCEFIELD_H__

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
 * DistanceFieldEffect is a custom shader effect to achieve distance field on Image actors
 */
class DistanceFieldEffect : public ShaderEffect
{
public:

  /**
   * Create an uninitialized DistanceFieldEffect; this can be initialized with DistanceFieldEffect::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  DistanceFieldEffect();

  /**
   * Virtual destructor.
   */
  virtual ~DistanceFieldEffect();

  /**
   * Create an initialized DistanceFieldEffect.
   * @return A handle to a newly allocated Dali resource.
   */
  static DistanceFieldEffect New();

  /**
   * Set the shadow state
   * @param[in] shadowEnable value - true, enable shadow.
   * @note Shadow cannot be used with glow/and or outline.
   */
  void SetShadow(bool shadowEnable);

  /**
   * Set the shadow color multiplier (e.g. output RGB)
   * @param[in] color Shadow color value
   */
  void SetShadowColor(const Vector4& color);

  /**
   * Set the shadow offset
   * @param[in] color shadow offset value
   */
  void SetShadowOffset(const Vector2& color);

  /**
   * Set the glow state
   * @param[in] glowEnable value - true, enable glow.
   */

  void SetGlow(bool glowEnable);

  /**
   * Set the glow color multiplier (e.g. output RGB)
   * @param[in] color Glow color value
   */
  void SetGlowColor(const Vector4& color);

  /**
   * Set the glow boundary factor
   * @param[in] glowBoundary glow boundary
   */
  void SetGlowBoundary(float glowBoundary);

  /**
   * Set the outline state
   * @param[in] outlineEnable value - true, enable outline.
   */

  void SetOutline(bool outlineEnable);

  /**
   * Set the outline color multiplier (e.g. output RGB)
   * @param[in] color Outline color value
   */
  void SetOutlineColor(const Vector4& color);

  /**
   * Sets the outline parameters.
   * @param[in] outlineParams  Thickness of outline. The outline thickness is determined by two parameters.
   *              params[0] (0-1) Specifies the distance field value for the center of the outline.
   *                      0 <= params[0] <= 1
   *              params[1] (0-1) Specifies the softness/width/anti-aliasing of the outlines inner edge.
   *                      0 <= params[0] <= 1
   */
  void SetOutlineParams(const Vector2& outlineParams);

  /**
   * Set soft edge smoothing
   * @param[in] smoothing Specify the distance field value for the center of the edge.
   *                      0 <= params <= 1
   */
  void SetSmoothingEdge(float smoothing);

  /**
   * Get the name for the outline-enable property
   * @return A std::string containing the property name
   */
  const std::string& GetOutlineEnablePropertyName() const;

  /**
   * Get the name for the glow-enable property
   * @return A std::string containing the property name
   */
  const std::string& GetGlowEnablePropertyName() const;

  /**
   * Get the name for the shadow-enable property
   * @return A std::string containing the property name
   */
  const std::string& GetShadowEnablePropertyName() const;

  /**
   * Get the name for the radius property
   * @return A std::string containing the property name
   */
  const std::string& GetColorPropertyName() const;

  /**
   * Get the name for the smoothing property
   * @return A std::string containing the property name
   */
  const std::string& GetSmoothingPropertyName() const;

  /**
   * Get the name for the outline color property
   * @return A std::string containing the property name
   */
  const std::string& GetOutlineColorPropertyName() const;

  /**
   * Get the name for the outline size property
   * @return A std::string containing the property name
   */
  const std::string& GetOutlineSizePropertyName() const;

  /**
   * Get the name for the shadow color property
   * @return A std::string containing the property name
   */
  const std::string& GetShadowColorPropertyName() const;

  /**
   * Get the name for the shadow offset property
   * @return A std::string containing the property name
   */
  const std::string& GetShadowOffsetPropertyName() const;

  /**
   * Get the name for the glow color property
   * @return A std::string containing the property name
   */
  const std::string& GetGlowColorPropertyName() const;

  /**
   * Get the name for the glow boundary property
   * @return A std::string containing the property name
   */
  const std::string& GetGlowBoundaryPropertyName() const;

private:
  DistanceFieldEffect(ShaderEffect handle);

};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SHADER_EFFECT_SPOT_H__

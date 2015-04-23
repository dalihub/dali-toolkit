#ifndef __DALI_TOOLKIT_CAROUSEL_EFFECT_H__
#define __DALI_TOOLKIT_CAROUSEL_EFFECT_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
 * CarouselEffect is a custom shader effect to achieve Carousel effects in actors
 *
 * A Carousel has a Radius property which can be +ve (appear as if viewing from the outside of
 * a cylinder/sphere)
 * or -ve (appear as if viewing from the inside of a cylinder/sphere).
 *
 * It can be a horizontal or vertical (cylindrical) or both (spherical). The AnglePerUnit
 * property provides this functionality as a Vector2.
 *
 * Finally, the carousel's center position can be specified as a Screen coordinate (top-left being
 * the origin).
 */
class DALI_IMPORT_API CarouselEffect : public ShaderEffect
{
public:

  /**
   * Create an uninitialized CarouselEffect; this can be initialized with CarouselEffect::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  CarouselEffect();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~CarouselEffect();

  /**
   * Create an initialized CarouselEffect.
   * @return A handle to a newly allocated Dali resource.
   */
  static CarouselEffect New();

  /**
   * Set the radius of the Carousel effect.
   * A positive Radius will bend toward the camera,
   * while a negative Radius will bend away from the camera.
   * @param[in] radius The new radius.
   */
  void SetRadius( float radius);

  /**
   * Sets the center point of the carousel (in screen coordinates)
   * this is where the peek of the carousel should appear.
   * this defaults to top-left corner (0.0f, 0.0f).
   *
   * @param[in] center The center point.
   */
  void SetCenter( const Vector2& center );

  /**
   * Set the angle deviation of Carousel in degrees per
   * geometric unit for each axis. For example if you
   * wish for the horizontal angle deviation to vary from +/- 10
   * degrees, then a Value of 20.0f / stageWidth for the X
   * component should be specified.
   *
   * @param[in] angle the Angle Spread in X and Y axes.
   */
  void SetAnglePerUnit( const Vector2& angle );

  /**
   * Get the name for the radius property
   * @return A std::string containing the property name
   */
  const std::string& GetRadiusPropertyName() const;

  /**
   * Get the name for the center property
   * @return A std::string containing the property name
   */
  const std::string& GetCenterPropertyName() const;

  /**
   * Get the name for the angle spread property
   * @return A std::string containing the property name
   */
  const std::string& GetAnglePerUnitPropertyName() const;


private: // Not intended for application developers
  DALI_INTERNAL CarouselEffect(ShaderEffect handle);
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_CAROUSEL_EFFECT_H__

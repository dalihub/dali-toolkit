#ifndef __DALI_TOOLKIT_INTERNAL_RADIAL_GRADIENT_H__
#define __DALI_TOOLKIT_INTERNAL_RADIAL_GRADIENT_H__

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

#include <dali-toolkit/internal/controls/renderers/gradient/gradient.h>

#include <dali/public-api/math/vector2.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * Radial gradients change color circularly.
 * The color transition starts from the center of the circle and distribute outwardly.
 */
class RadialGradient : public Gradient
{
public:

  /**
   * Contructor.
   * @param[in] center The center of the gradient circle onto which the 0% gradient stop is mapped.
   * @param[in] radius The radius of the outmost circle onto which the 100% gradient stop is mapped.
   */
  RadialGradient( const Vector2& center, float radius );

  /**
   * Destructor.
   */
  virtual ~RadialGradient();

  /**
   * Set the center and radius of the outermost circle for the radial gradient.
   * @param[in] center The center of the gradient circle onto which the 0% gradient stop is mapped.
   * @param[in] radius The radius of the outmost circle onto which the 100% gradient stop is mapped.
   */
  void SetCenterAndRadius( const Vector2& center, float radius );

  /**
   * Get the center of the gradient circle.
   * @return The center of the gradient circle.
   */
  const Vector2& GetCenter() const;

  /**
   * Get the radius of the outmost gradient circle.
   * @return The radius of the outmost gradient circle.
   */
  float GetRadius() const;

private:

  Vector2 mCenter;
  float   mRadius;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* __DALI_TOOLKIT_INTERNAL_RADIAL_GRADIENT_H__ */

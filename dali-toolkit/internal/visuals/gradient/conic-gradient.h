#ifndef DALI_TOOLKIT_INTERNAL_CONIC_GRADIENT_H
#define DALI_TOOLKIT_INTERNAL_CONIC_GRADIENT_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit/internal/visuals/gradient/gradient.h>

#include <dali/public-api/math/vector2.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
/**
 * Conic gradient creates a color transition around a center point, rotating based on the angle from a defined start direction.
 * It blends colors smoothly as the angle increases around the center.
 */
class ConicGradient : public Gradient
{
public:
  /**
   * Contructor.
   * @param[in] center The point around which the conic gradient rotates.
   * @param[in] startAngle The initial angle from which the gradient begins.
   */
  ConicGradient(const Vector2& center, Dali::Radian startAngle);

  /**
   * Destructor.
   */
  virtual ~ConicGradient();

  /**
   * Get the center of the conic gradient.
   * @return The center of the conic gradient.
   */
  const Vector2& GetCenter() const;

  /**
   * Get the start angle of the conic gradient.
   * @return The start angle of the conic gradient.
   */
  Dali::Radian GetStartAngle() const;

private:
  // Undefined
  ConicGradient(const ConicGradient& gradient);

  // Undefined
  ConicGradient& operator=(const ConicGradient& handle);

private:
  /**
   * Set the center and start angle for the conic gradient.
   * @param[in] center The point around which the conic gradient rotates.
   * @param[in] startAngle The initial angle from which the gradient begins.
   * @note An angle out side the range of 0 to 2pi is converted into an equivalent angle within that range.
   */
  void SetCenterAndStartAngle(const Vector2& center, Dali::Radian startAngle);

  Vector2 mCenter;
  Dali::Radian mStartAngle;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_INTERNAL_CONIC_GRADIENT_H */

#ifndef DALI_TOOLKIT_INTERNAL_LINEAR_GRADIENT_H
#define DALI_TOOLKIT_INTERNAL_LINEAR_GRADIENT_H

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

#include <dali-toolkit/internal/visuals/gradient/gradient.h>

#include <dali/public-api/math/vector2.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * Linear gradients change color evenly along a straight line.
 * The gradient is defined by an axis (the gradient line) at any specified angles.
 */
class LinearGradient : public Gradient
{
public:

  /**
   * Constructor.
   * @param[in] startPosition The starting point onto which the 0% gradient stops are mapped.
   * @param[in] endPosition The ending point r onto which the 100% gradient stops are mapped.
   */
  LinearGradient( const Vector2& startPosition, const Vector2& endPosition );

  /**
   * Destructor.
   */
  virtual ~LinearGradient();

  /**
   * Set the starting and ending points of the vector onto which the gradient stops are mapped.
   * @param[in] startPosition The starting point of the gradient vector.
   * @param[in] endPosition The ending point of the gradient vector.
   */
  void SetStartAndEndPosition( const Vector2& startPosition, const Vector2& endPosition );

  /**
   * Get the stating point of the gradient vector.
   * @return The stating point of the gradient vector.
   */
  const Vector2& GetStartPosition() const;

  /**
   * Get the ending point of the gradient vector.
   * @return The ending point of the gradient vector.
   */
  const Vector2& GetEndPosition() const;

private:

  // Undefined
  LinearGradient( const LinearGradient& gradient );

  // Undefined
  LinearGradient& operator=( const LinearGradient& handle );

private:

  Vector2 mStartPosition;
  Vector2 mEndPosition;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_INTERNAL_LINEAR_GRADIENT_H */

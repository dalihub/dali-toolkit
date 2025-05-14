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

// CLASS HEADER
#include "conic-gradient.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
ConicGradient::ConicGradient(const Vector2& center, Dali::Radian startAngle)
: Gradient()
{
  SetCenterAndStartAngle(center, startAngle);
}

ConicGradient::~ConicGradient()
{
}

void ConicGradient::SetCenterAndStartAngle(const Vector2& center, Dali::Radian startAngle)
{
  mCenter     = center;
  mStartAngle = WrapInDomain(startAngle, Dali::ANGLE_0, Dali::ANGLE_360);

  // Calculate the transform aligning to the circle
  Matrix3 invertedAlignMatrix(1.f, 0.f, 0.f, 0.f, 1.f, 0.f, -mCenter.x, -mCenter.y, 1.f);

  mAlignmentTransform = invertedAlignMatrix;
}

const Vector2& ConicGradient::GetCenter() const
{
  return mCenter;
}

Dali::Radian ConicGradient::GetStartAngle() const
{
  return mStartAngle;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

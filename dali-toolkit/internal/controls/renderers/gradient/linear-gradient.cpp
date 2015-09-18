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

#include "linear-gradient.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

LinearGradient::LinearGradient( const Vector2& startPosition, const Vector2& endPosition )
: Gradient()
{
  SetStartAndEndPosition( startPosition, endPosition );
}

LinearGradient::~LinearGradient()
{}

void LinearGradient::SetStartAndEndPosition( const Vector2& startPosition, const Vector2& endPosition )
{
  mStartPosition = startPosition;
  mEndPosition = endPosition;

  // Calculate the transform aligning to the gradient line
  float dx = mEndPosition.x - mStartPosition.x;
  float dy = mEndPosition.y - mStartPosition.y;
  Matrix3 alignMatrix( dy, -dx, 0.f, dx, dy, 0.f, mStartPosition.x, mStartPosition.y, 1.f );
  alignMatrix.Invert();

  mAlignmentTransform = alignMatrix;
}

const Vector2& LinearGradient::GetStartPosition() const
{
  return mStartPosition;
}

const Vector2& LinearGradient::GetEndPosition() const
{
  return mEndPosition;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

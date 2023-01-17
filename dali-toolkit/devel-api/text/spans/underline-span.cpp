/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/text/spans/underline-span.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/spannable/spans/underline-span-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
UnderlineSpan UnderlineSpan::New()
{
  return Internal::UnderlineSpan::New();
}

UnderlineSpan UnderlineSpan::NewSolid(Vector4 color, float height)
{
  return Internal::UnderlineSpan::NewSolid(color, height);
}

UnderlineSpan UnderlineSpan::NewDashed(Vector4 color, float height, float dashGap, float dashWidth)
{
  return Internal::UnderlineSpan::NewDashed(color, height, dashGap, dashWidth);
}

UnderlineSpan UnderlineSpan::NewDouble(Vector4 color, float height)
{
  return Internal::UnderlineSpan::NewDouble(color, height);
}

UnderlineSpan::UnderlineSpan(Internal::UnderlineSpan* internal)
: BaseSpan(internal)
{
}

UnderlineSpan::UnderlineSpan() = default;

UnderlineSpan::UnderlineSpan(const UnderlineSpan& rhs) = default;

UnderlineSpan::UnderlineSpan(UnderlineSpan&& rhs) = default;

UnderlineSpan& UnderlineSpan::operator=(const UnderlineSpan& rhs) = default;

UnderlineSpan& UnderlineSpan::operator=(UnderlineSpan&& rhs) = default;

UnderlineSpan::~UnderlineSpan() = default;

//Methods

Text::Underline::Type UnderlineSpan::GetType() const
{
  return GetImplementation(*this).GetType();
}

bool UnderlineSpan::IsTypeDefined() const
{
  return GetImplementation(*this).IsTypeDefined();
}

Vector4 UnderlineSpan::GetColor() const
{
  return GetImplementation(*this).GetColor();
}

bool UnderlineSpan::IsColorDefined() const
{
  return GetImplementation(*this).IsColorDefined();
}

float UnderlineSpan::GetHeight() const
{
  return GetImplementation(*this).GetHeight();
}

bool UnderlineSpan::IsHeightDefined() const
{
  return GetImplementation(*this).IsHeightDefined();
}

float UnderlineSpan::GetDashGap() const
{
  return GetImplementation(*this).GetDashGap();
}

bool UnderlineSpan::IsDashGapDefined() const
{
  return GetImplementation(*this).IsDashGapDefined();
}

float UnderlineSpan::GetDashWidth() const
{
  return GetImplementation(*this).GetDashWidth();
}

bool UnderlineSpan::IsDashWidthDefined() const
{
  return GetImplementation(*this).IsDashWidthDefined();
}

UnderlineSpan UnderlineSpan::DownCast(BaseHandle handle)
{
  return UnderlineSpan(dynamic_cast<Dali::Toolkit::Text::Internal::UnderlineSpan*>(handle.GetObjectPtr()));
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

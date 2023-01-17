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
#include <dali-toolkit/devel-api/text/spans/foreground-color-span.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/spannable/spans/foreground-color-span-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
ForegroundColorSpan ForegroundColorSpan::New(Vector4 color)
{
  return Internal::ForegroundColorSpan::New(color);
}

ForegroundColorSpan::ForegroundColorSpan(Internal::ForegroundColorSpan* internal)
: BaseSpan(internal)
{
}

ForegroundColorSpan::ForegroundColorSpan() = default;

ForegroundColorSpan::ForegroundColorSpan(const ForegroundColorSpan& rhs) = default;

ForegroundColorSpan::ForegroundColorSpan(ForegroundColorSpan&& rhs) = default;

ForegroundColorSpan& ForegroundColorSpan::operator=(const ForegroundColorSpan& rhs) = default;

ForegroundColorSpan& ForegroundColorSpan::operator=(ForegroundColorSpan&& rhs) = default;

ForegroundColorSpan::~ForegroundColorSpan() = default;

//Methods
Vector4 ForegroundColorSpan::GetForegroundColor() const
{
  return GetImplementation(*this).GetForegroundColor();
}

bool ForegroundColorSpan::IsForegroundColorDefined() const
{
  return GetImplementation(*this).IsForegroundColorDefined();
}

ForegroundColorSpan ForegroundColorSpan::DownCast(BaseHandle handle)
{
  return ForegroundColorSpan(dynamic_cast<Dali::Toolkit::Text::Internal::ForegroundColorSpan*>(handle.GetObjectPtr()));
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

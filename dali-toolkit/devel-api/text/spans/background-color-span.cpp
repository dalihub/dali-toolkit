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
#include <dali-toolkit/devel-api/text/spans/background-color-span.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/spannable/spans/background-color-span-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
BackgroundColorSpan BackgroundColorSpan::New(const Vector4 &color)
{
  return Internal::BackgroundColorSpan::New(color);
}

BackgroundColorSpan::BackgroundColorSpan(Internal::BackgroundColorSpan* internal)
: BaseSpan(internal)
{
}

BackgroundColorSpan::BackgroundColorSpan() = default;

BackgroundColorSpan::BackgroundColorSpan(const BackgroundColorSpan& rhs) = default;

BackgroundColorSpan::BackgroundColorSpan(BackgroundColorSpan&& rhs) = default;

BackgroundColorSpan& BackgroundColorSpan::operator=(const BackgroundColorSpan& rhs) = default;

BackgroundColorSpan& BackgroundColorSpan::operator=(BackgroundColorSpan&& rhs) = default;

BackgroundColorSpan::~BackgroundColorSpan() = default;

//Methods
const Vector4 BackgroundColorSpan::GetBackgroundColor() const
{
  return GetImplementation(*this).GetBackgroundColor();
}

bool BackgroundColorSpan::IsBackgroundColorDefined() const
{
  return GetImplementation(*this).IsBackgroundColorDefined();
}

BackgroundColorSpan BackgroundColorSpan::DownCast(BaseHandle handle)
{
  return BackgroundColorSpan(dynamic_cast<Dali::Toolkit::Text::Internal::BackgroundColorSpan*>(handle.GetObjectPtr()));
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

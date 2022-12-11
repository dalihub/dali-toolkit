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
#include <dali-toolkit/devel-api/text/spans/strikethrough-span.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/spannable/spans/strikethrough-span-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
StrikethroughSpan StrikethroughSpan::New()
{
  return Internal::StrikethroughSpan::New();
}

StrikethroughSpan StrikethroughSpan::New(Vector4 color, float height)
{
  return Internal::StrikethroughSpan::New(color, height);
}

StrikethroughSpan::StrikethroughSpan(Internal::StrikethroughSpan* internal)
: BaseSpan(internal)
{
}

StrikethroughSpan::StrikethroughSpan()                             = default;
StrikethroughSpan::StrikethroughSpan(const StrikethroughSpan& rhs) = default;
StrikethroughSpan::StrikethroughSpan(StrikethroughSpan&& rhs)      = default;
StrikethroughSpan& StrikethroughSpan::operator=(const StrikethroughSpan& rhs) = default;
StrikethroughSpan& StrikethroughSpan::operator=(StrikethroughSpan&& rhs) = default;
StrikethroughSpan::~StrikethroughSpan()                                  = default;

//Methods
const Vector4 StrikethroughSpan::GetColor() const
{
  return GetImplementation(*this).GetColor();
}

bool StrikethroughSpan::IsColorDefined() const
{
  return GetImplementation(*this).IsColorDefined();
}

const float StrikethroughSpan::GetHeight() const
{
  return GetImplementation(*this).GetHeight();
}

bool StrikethroughSpan::IsHeightDefined() const
{
  return GetImplementation(*this).IsHeightDefined();
}

StrikethroughSpan StrikethroughSpan::DownCast(BaseHandle handle)
{
  return StrikethroughSpan(dynamic_cast<Dali::Toolkit::Text::Internal::StrikethroughSpan*>(handle.GetObjectPtr()));
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
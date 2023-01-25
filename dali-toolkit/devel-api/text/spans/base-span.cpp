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
#include <dali-toolkit/devel-api/text/spans/base-span.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/spannable/spans/base-span-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
BaseSpan::BaseSpan() = default;

BaseSpan::BaseSpan(Internal::BaseSpan* internal)
: BaseHandle(internal)
{
}

BaseSpan::BaseSpan(const BaseSpan& rhs) = default;

BaseSpan::BaseSpan(BaseSpan&& rhs) = default;

BaseSpan& BaseSpan::operator=(const BaseSpan& rhs) = default;

BaseSpan& BaseSpan::operator=(BaseSpan&& rhs) = default;

BaseSpan::~BaseSpan() = default;

BaseSpan BaseSpan::DownCast(BaseHandle handle)
{
  return BaseSpan(dynamic_cast<Dali::Toolkit::Text::Internal::BaseSpan*>(handle.GetObjectPtr()));
}

Dali::Toolkit::Text::SpanType::Value BaseSpan::GetSpanType() const
{
  return GetImplementation(*this).GetSpanType();
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

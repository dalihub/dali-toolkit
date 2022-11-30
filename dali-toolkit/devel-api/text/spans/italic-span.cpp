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
#include <dali-toolkit/devel-api/text/spans/italic-span.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/spannable/spans/italic-span-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
ItalicSpan ItalicSpan::New()
{
  return Internal::ItalicSpan::New();
}

ItalicSpan::ItalicSpan() = default;

ItalicSpan::ItalicSpan(const ItalicSpan& rhs) = default;

ItalicSpan::ItalicSpan(ItalicSpan&& rhs) = default;

ItalicSpan& ItalicSpan::operator=(const ItalicSpan& rhs) = default;

ItalicSpan& ItalicSpan::operator=(ItalicSpan&& rhs) = default;

ItalicSpan::~ItalicSpan() = default;


ItalicSpan ItalicSpan::DownCast(BaseHandle handle)
{
  return ItalicSpan(dynamic_cast<Dali::Toolkit::Text::Internal::ItalicSpan*>(handle.GetObjectPtr()));
}

ItalicSpan::ItalicSpan(Internal::ItalicSpan* internal)
: BaseSpan(internal)
{
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

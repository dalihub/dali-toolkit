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
#include <dali-toolkit/devel-api/text/range.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/string-text/range-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
Range Range::New(u_int32_t startIndex, u_int32_t endIndex)
{
  return Internal::Range::New(startIndex, endIndex);
}

Range::Range() = default;

Range::Range(Internal::Range* internal)
: BaseHandle(internal)
{
}

Range::Range(const Range& rhs) = default;

Range::Range(Range&& rhs) = default;

Range& Range::operator=(const Range& rhs) = default;

Range& Range::operator=(Range&& rhs) = default;

Range::~Range() = default;

//Methods
u_int32_t Range::GetStartIndex() const
{
  return GetImplementation(*this).GetStartIndex();
}

u_int32_t Range::GetEndIndex() const
{
  return GetImplementation(*this).GetEndIndex();
}

u_int32_t Range::GetNumberOfIndices() const
{
  return GetImplementation(*this).GetNumberOfIndices();
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

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
#include <dali-toolkit/devel-api/text/spanned.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/spannable/spanned-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
Spanned::Spanned() = default;

Spanned::Spanned(Internal::Spanned* internal)
: CharacterSequence(internal)
{
}

std::vector<BaseSpan> Spanned::GetAllSpans() const
{
  return GetImplementation(*this).GetAllSpans();
}

void Spanned::RetrieveAllSpansAndRanges(std::vector<BaseSpan>& spans, std::vector<Range>& ranges) const
{
  GetImplementation(*this).RetrieveAllSpansAndRanges(spans, ranges);
}

Spanned::Spanned(const Spanned& rhs) = default;

Spanned::Spanned(Spanned&& rhs) = default;

Spanned& Spanned::operator=(const Spanned& rhs) = default;

Spanned& Spanned::operator=(Spanned&& rhs) = default;

Spanned::~Spanned() = default;

} // namespace Text

} // namespace Toolkit

} // namespace Dali

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
#include <dali-toolkit/devel-api/text/spannable.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/spannable/spannable-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
Spannable::Spannable() = default;

Spannable::Spannable(Internal::Spannable* internal)
: Spanned(internal)
{
}

bool Spannable::AttachSpan(BaseSpan styleSpan, Range range)
{
  return GetImplementation(*this).AttachSpan(styleSpan, range);
}

bool Spannable::DetachSpan(const BaseSpan& styleSpan)
{
  return GetImplementation(*this).DetachSpan(styleSpan);
}

Spannable::Spannable(const Spannable& rhs) = default;

Spannable::Spannable(Spannable&& rhs) = default;

Spannable& Spannable::operator=(const Spannable& rhs) = default;

Spannable& Spannable::operator=(Spannable&& rhs) = default;

Spannable::~Spannable() = default;

} // namespace Text

} // namespace Toolkit

} // namespace Dali

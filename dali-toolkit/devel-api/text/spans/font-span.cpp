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
#include <dali-toolkit/devel-api/text/spans/font-span.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/spannable/spans/font-span-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
FontSpan FontSpan::New(const std::string                             familyName,
                       const float                                   sizeInPoints,
                       const Dali::TextAbstraction::FontWeight::Type weight,
                       const Dali::TextAbstraction::FontWidth::Type  width,
                       const Dali::TextAbstraction::FontSlant::Type  slant)
{
  return Internal::FontSpan::New(familyName, sizeInPoints, weight, width, slant);
}

FontSpan::FontSpan(Internal::FontSpan* internal)
: BaseSpan(internal)
{
}

FontSpan::FontSpan() = default;

FontSpan::FontSpan(const FontSpan& rhs) = default;

FontSpan::FontSpan(FontSpan&& rhs) = default;

FontSpan& FontSpan::operator=(const FontSpan& rhs) = default;

FontSpan& FontSpan::operator=(FontSpan&& rhs) = default;

FontSpan::~FontSpan() = default;

//Methods
const std::string FontSpan::GetFamilyName() const
{
  return GetImplementation(*this).GetFamilyName();
}

bool FontSpan::IsFamilyNameDefined() const
{
  return GetImplementation(*this).IsFamilyNameDefined();
}

const Dali::TextAbstraction::FontWeight::Type FontSpan::GetWeight() const
{
  return GetImplementation(*this).GetWeight();
}

bool FontSpan::IsWeightDefined() const
{
  return GetImplementation(*this).IsWeightDefined();
}

const Dali::TextAbstraction::FontWidth::Type FontSpan::GetWidth() const
{
  return GetImplementation(*this).GetWidth();
}

bool FontSpan::IsWidthDefined() const
{
  return GetImplementation(*this).IsWidthDefined();
}

const Dali::TextAbstraction::FontSlant::Type FontSpan::GetSlant() const
{
  return GetImplementation(*this).GetSlant();
}

bool FontSpan::IsSlantDefined() const
{
  return GetImplementation(*this).IsSlantDefined();
}

const float FontSpan::GetSize() const
{
  return GetImplementation(*this).GetSize();
}

bool FontSpan::IsSizeDefined() const
{
  return GetImplementation(*this).IsSizeDefined();
}

FontSpan FontSpan::DownCast(BaseHandle handle)
{
  return FontSpan(dynamic_cast<Dali::Toolkit::Text::Internal::FontSpan*>(handle.GetObjectPtr()));
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

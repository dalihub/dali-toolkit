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
#include <dali-toolkit/devel-api/text/spans/character-spacing-span.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/spannable/spans/character-spacing-span-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
CharacterSpacingSpan CharacterSpacingSpan::New(const float value)
{
  return Internal::CharacterSpacingSpan::New(value);
}

CharacterSpacingSpan::CharacterSpacingSpan(Internal::CharacterSpacingSpan* internal)
: BaseSpan(internal)
{
}

CharacterSpacingSpan::CharacterSpacingSpan() = default;

CharacterSpacingSpan::CharacterSpacingSpan(const CharacterSpacingSpan& rhs) = default;

CharacterSpacingSpan::CharacterSpacingSpan(CharacterSpacingSpan&& rhs) = default;

CharacterSpacingSpan& CharacterSpacingSpan::operator=(const CharacterSpacingSpan& rhs) = default;

CharacterSpacingSpan& CharacterSpacingSpan::operator=(CharacterSpacingSpan&& rhs) = default;

CharacterSpacingSpan::~CharacterSpacingSpan() = default;

//Methods
float CharacterSpacingSpan::GetCharacterSpacing() const
{
  return GetImplementation(*this).GetCharacterSpacing();
}

bool CharacterSpacingSpan::IsCharacterSpacingDefined() const
{
  return GetImplementation(*this).IsCharacterSpacingDefined();
}

CharacterSpacingSpan CharacterSpacingSpan::DownCast(BaseHandle handle)
{
  return CharacterSpacingSpan(dynamic_cast<Dali::Toolkit::Text::Internal::CharacterSpacingSpan*>(handle.GetObjectPtr()));
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

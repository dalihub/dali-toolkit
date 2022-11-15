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
#include <dali-toolkit/devel-api/text/character-sequence.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/string-text/character-sequence-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
CharacterSequence::CharacterSequence() = default;

CharacterSequence::CharacterSequence(Internal::CharacterSequence* internal)
: BaseHandle(internal)
{
}

CharacterSequence::CharacterSequence(const CharacterSequence& rhs) = default;

CharacterSequence::CharacterSequence(CharacterSequence&& rhs) = default;

CharacterSequence& CharacterSequence::operator=(const CharacterSequence& rhs) = default;

CharacterSequence& CharacterSequence::operator=(CharacterSequence&& rhs) = default;

CharacterSequence::~CharacterSequence() = default;

//Methods

Vector<uint32_t> CharacterSequence::GetCharacters() const
{
  return GetImplementation(*this).GetCharacters();
}

uint32_t CharacterSequence::GetNumberOfCharacters() const
{
  return GetImplementation(*this).GetNumberOfCharacters();
}

std::string CharacterSequence::ToString() const
{
  return GetImplementation(*this).ToString();
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

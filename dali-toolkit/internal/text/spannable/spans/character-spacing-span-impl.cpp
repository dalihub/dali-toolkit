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
#include <dali-toolkit/internal/text/spannable/spans/character-spacing-span-impl.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/color-run.h>
#include <dali-toolkit/internal/text/markup-tags-and-attributes.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal
{
struct CharacterSpacingSpan::Impl
{
  float mCharacterSpacing;            ///< The character-spacing-value.
  bool  mCharacterSpacingDefined : 1; ///< Whether the character-spacing-value is defined.
};

CharacterSpacingSpan::CharacterSpacingSpan()
: BaseSpan(Dali::Toolkit::Text::SpanType::Value::CHARACTER_SPACING)
{
  mImpl = std::make_unique<Impl>();
}

CharacterSpacingSpan ::~CharacterSpacingSpan()
{
}

Dali::Toolkit::Text::CharacterSpacingSpan CharacterSpacingSpan::New(const float& value)
{
  CharacterSpacingSpanPtr object = new CharacterSpacingSpan();
  object->SetCharacterSpacing(value);

  Dali::Toolkit::Text::CharacterSpacingSpan handle = Dali::Toolkit::Text::CharacterSpacingSpan(object.Get());

  return handle;
}

//Methods
const float CharacterSpacingSpan::GetCharacterSpacing() const
{
  return mImpl->mCharacterSpacing;
}

bool CharacterSpacingSpan::IsCharacterSpacingDefined() const
{
  return mImpl->mCharacterSpacingDefined;
}

void CharacterSpacingSpan::SetCharacterSpacing(const float& value)
{
  mImpl->mCharacterSpacing        = value;
  mImpl->mCharacterSpacingDefined = true;
}

void CharacterSpacingSpan::CreateStyleCharacterRun(IntrusivePtr<LogicalModel>& logicalModel, const Dali::Toolkit::Text::Range& range) const
{
  CharacterSpacingCharacterRun characterSpacingCharacterRun;
  characterSpacingCharacterRun.characterRun.characterIndex     = range.GetStartIndex();
  characterSpacingCharacterRun.characterRun.numberOfCharacters = range.GetNumberOfIndices();
  characterSpacingCharacterRun.value                           = mImpl->mCharacterSpacing;
  logicalModel->mCharacterSpacingCharacterRuns.PushBack(characterSpacingCharacterRun);
  logicalModel->mCharacterSpacingRunsUpdated = true;
}

} // namespace Internal

} // namespace Text

} // namespace Toolkit

} // namespace Dali

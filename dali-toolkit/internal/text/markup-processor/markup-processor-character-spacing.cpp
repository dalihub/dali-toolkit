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

// FILE HEADER
#include <dali-toolkit/internal/text/markup-processor/markup-processor-character-spacing.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/character-spacing-character-run.h>
#include <dali-toolkit/internal/text/markup-processor/markup-processor-attribute-helper-functions.h>
#include <dali-toolkit/internal/text/markup-processor/markup-processor-helper-functions.h>
#include <dali-toolkit/internal/text/markup-tags-and-attributes.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
void ProcessValueAttribute(const Attribute& attribute, CharacterSpacingCharacterRun& characterSpacingCharacterRun)
{
  characterSpacingCharacterRun.value = ProcessFloatAttribute(attribute);
}

void ProcessCharacterSpacingTag(const Tag& tag, CharacterSpacingCharacterRun& characterSpacingCharacterRun)
{
  for(Vector<Attribute>::ConstIterator it    = tag.attributes.Begin(),
                                       endIt = tag.attributes.End();
      it != endIt;
      ++it)
  {
    const Attribute& attribute(*it);

    if(TokenComparison(MARKUP::CHARACTER_SPACING_ATTRIBUTES::VALUE, attribute.nameBuffer, attribute.nameLength))
    {
      ProcessValueAttribute(attribute, characterSpacingCharacterRun);
    }
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

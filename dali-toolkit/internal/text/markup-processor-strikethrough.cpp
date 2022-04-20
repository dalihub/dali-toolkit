/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/markup-processor-strikethrough.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/markup-processor-attribute-helper-functions.h>
#include <dali-toolkit/internal/text/markup-processor-helper-functions.h>
#include <dali-toolkit/internal/text/markup-tags-and-attributes.h>
#include <dali-toolkit/internal/text/strikethrough-character-run.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
void ProcessColorAttribute(const Attribute& attribute, StrikethroughCharacterRun& strikethroughRun)

{
  ColorStringToVector4(attribute.valueBuffer, attribute.valueLength, strikethroughRun.properties.color);
  strikethroughRun.properties.colorDefined = true;
}

void ProcessHeightAttribute(const Attribute& attribute, StrikethroughCharacterRun& strikethroughRun)
{
  strikethroughRun.properties.height        = ProcessFloatAttribute(attribute);
  strikethroughRun.properties.heightDefined = true;
}

void ProcessStrikethroughTag(const Tag& tag, StrikethroughCharacterRun& strikethroughRun)
{
  for(Vector<Attribute>::ConstIterator it    = tag.attributes.Begin(),
                                       endIt = tag.attributes.End();
      it != endIt;
      ++it)
  {
    const Attribute& attribute(*it);

    if(TokenComparison(MARKUP::STRIKETHROUGH_ATTRIBUTES::COLOR, attribute.nameBuffer, attribute.nameLength))
    {
      ProcessColorAttribute(attribute, strikethroughRun);
    }
    else if(TokenComparison(MARKUP::STRIKETHROUGH_ATTRIBUTES::HEIGHT, attribute.nameBuffer, attribute.nameLength))
    {
      ProcessHeightAttribute(attribute, strikethroughRun);
    }
  }
}

void OverrideNestedStrikethroughCharacterRuns(Vector<StrikethroughCharacterRun>& strikethroughCharacterRuns)
{
  // Handle nested tags
  // The inner tag inherit the attributes of the outer tag and override them when defined in the inner tag.
  // Example:
  // <s height='5.0f' color='blue'> outer tag before  <s color='green'> inner tag </s> outer tag after </s>
  // "outer tag before" and  "outer tag after" have height = 5.0f and color = 'blue'
  // "inner tag" has height = 5.0f and color = 'green'

  if(strikethroughCharacterRuns.Count() > 0u)
  {
    Vector<StrikethroughCharacterRun>::ConstIterator preIt = strikethroughCharacterRuns.Begin();

    Vector<StrikethroughCharacterRun>::Iterator      it    = strikethroughCharacterRuns.Begin() + 1;
    Vector<StrikethroughCharacterRun>::ConstIterator endIt = strikethroughCharacterRuns.End();

    while(it != endIt)
    {
      const StrikethroughCharacterRun& run                = *it;
      const CharacterIndex&            characterIndex     = run.characterRun.characterIndex;
      const Length&                    numberOfCharacters = run.characterRun.numberOfCharacters;

      const StrikethroughCharacterRun& preRun                = *preIt;
      const CharacterIndex&            preCharacterIndex     = preRun.characterRun.characterIndex;
      const Length&                    preNumberOfCharacters = preRun.characterRun.numberOfCharacters;

      if((preCharacterIndex <= characterIndex) &&
         ((characterIndex + numberOfCharacters) <= (preCharacterIndex + preNumberOfCharacters)))
      {
        it->properties.CopyIfNotDefined(preIt->properties);
      }

      it++;
      preIt++;
    }
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

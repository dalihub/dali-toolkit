
/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/emoji-helper.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
bool IsTextPresentationSequence(const TextAbstraction::Script&    currentRunScript,
                                const TextAbstraction::Character& character)
{
  return (IsSymbolOrEmojiOrTextScript(currentRunScript) && TextAbstraction::IsTextPresentationSelector(character));
}

bool IsEmojiPresentationSequence(const TextAbstraction::Script&    currentRunScript,
                                 const TextAbstraction::Character& character)
{
  return ((IsSymbolOrEmojiScript(currentRunScript) || IsEmojiColorScript(currentRunScript)) &&
          TextAbstraction::IsEmojiPresentationSelector(character));
}

bool IsEmojiSequence(const TextAbstraction::Script&    currentRunScript,
                     const TextAbstraction::Character& character,
                     const TextAbstraction::Script&    characterScript)
{
  return (!(TextAbstraction::IsNegativeSquaredLatinCapitalLetter(character)) &&
          (IsOneOfEmojiScripts(currentRunScript) &&
           (IsOneOfEmojiScripts(characterScript) ||
            TextAbstraction::IsZeroWidthJoiner(character) ||
            TextAbstraction::IsZeroWidthNonJoiner(character) ||
            TextAbstraction::IsEmojiItem(character) ||
            TextAbstraction::IsMiscellaneousSymbolsAndArrowsEmoji(character) ||
            TextAbstraction::IsDingbatsEmoji(character))));
}

bool IsNewSequence(const Character* const         textBuffer,
                   const TextAbstraction::Script& currentRunScript,
                   const Length&                  currentCharacterIndex,
                   const Length&                  lastCharacterIndex,
                   TextAbstraction::Script&       currentCharacterScript)
{
  // Until now we have two cases : VariationSelector & Keycap
  // In-case there are more cases then should be added in this function

  return IsNewKeycapSequence(textBuffer, currentCharacterIndex, lastCharacterIndex, currentCharacterScript) ||
         IsNewVariationSelectorSequence(textBuffer, currentRunScript, currentCharacterIndex, lastCharacterIndex, currentCharacterScript);
}

//

bool IsNewKeycapSequence(const Character* const   textBuffer,
                         const Length&            currentCharacterIndex,
                         const Length&            lastCharacterIndex,
                         TextAbstraction::Script& currentCharacterScript)
{
  // Ref: https://www.unicode.org/Public/emoji/14.0/emoji-sequences.txt Search on "Emoji_Keycap_Sequence"
  // Ref: https://www.unicode.org/Public/emoji/14.0/emoji-test.txt Search on "subgroup: keycap"

  // Default initialization does not keycap sequence
  bool isNewKeycapSequence = false;

  if(currentCharacterIndex <= lastCharacterIndex)
  {
    Character currentCharacter = *(textBuffer + currentCharacterIndex);
    if(IsStartForKeycapSequence(currentCharacter))
    {
      if(!isNewKeycapSequence && currentCharacterIndex + 2 <= lastCharacterIndex)
      {
        Character characterOne = *(textBuffer + currentCharacterIndex + 1);
        Character characterTwo = *(textBuffer + currentCharacterIndex + 2);

        if(TextAbstraction::IsEmojiPresentationSelector(characterOne) &&
           TextAbstraction::IsCombiningEnclosingKeycap(characterTwo))
        {
          isNewKeycapSequence    = true;
          currentCharacterScript = TextAbstraction::EMOJI_COLOR;
        }
      } // if(!isNewKeycapSequence && currentCharacterIndex + 2 <= lastCharacterIndex)
    }   // if(IsStartForKeycapSequence(currentCharacter))
  }     // if(currentCharacterIndex < lastCharacterIndex)

  return isNewKeycapSequence;
}

bool IsNewVariationSelectorSequence(const Character* const         textBuffer,
                                    const TextAbstraction::Script& currentRunScript,
                                    const Length&                  currentCharacterIndex,
                                    const Length&                  lastCharacterIndex,
                                    TextAbstraction::Script&       currentCharacterScript)
{
  // Ref: Emoji and Text Presentation Selectors: https://www.unicode.org/reports/tr51/#Emoji_Variation_Selectors
  // Ref: Emoji Variation Sequences for UTS #51: https://www.unicode.org/Public/14.0.0/ucd/emoji/emoji-variation-sequences.txt

  // Default initialization does not VariationSelector sequence
  bool isNewVariationSelectorSequence = false;

  if(currentCharacterIndex <= lastCharacterIndex)
  {
    Character currentCharacter = *(textBuffer + currentCharacterIndex);
    if(TextAbstraction::IsEmojiVariationSequences(currentCharacter))
    {
      if(!isNewVariationSelectorSequence && currentCharacterIndex + 1 <= lastCharacterIndex)
      {
        Character characterVS = *(textBuffer + currentCharacterIndex + 1);

        if(TextAbstraction::IsEmojiPresentationSelector(characterVS))
        {
          isNewVariationSelectorSequence = currentRunScript != TextAbstraction::EMOJI_COLOR;
          currentCharacterScript         = TextAbstraction::EMOJI_COLOR;
        }
        else if(TextAbstraction::IsTextPresentationSelector(characterVS))
        {
          isNewVariationSelectorSequence = currentRunScript != TextAbstraction::EMOJI_TEXT;
          currentCharacterScript         = TextAbstraction::EMOJI_TEXT;
        }
        else if(!TextAbstraction::IsZeroWidthJoiner(characterVS))
        {
          // Start of a new sequence if the next glyph is not a variation selector or zwj.
          isNewVariationSelectorSequence = true;
        }
      } // if(!isNewVariationSelectorSequence && currentCharacterIndex + 1 <= lastCharacterIndex)
    }   // if(TextAbstraction::IsEmojiVariationSequences(currentCharacter))
    else if(!TextAbstraction::IsEmojiPresentationSelector(currentCharacter) &&
            !TextAbstraction::IsTextPresentationSelector(currentCharacter) &&
            !TextAbstraction::IsZeroWidthJoiner(currentCharacter) &&
            !TextAbstraction::IsEmojiModifier(currentCharacter))
    {
      if(currentCharacterIndex > 0)
      {
        Character prevCharacter = *(textBuffer + currentCharacterIndex - 1);
        if(TextAbstraction::IsEmojiVariationSequences(prevCharacter))
        {
          // The end of a variation sequence, start of a new sequence.
          isNewVariationSelectorSequence = true;
        }
      }
    }
  }     // if(currentCharacterIndex < lastCharacterIndex)

  return isNewVariationSelectorSequence;
}

bool IsStartForKeycapSequence(const TextAbstraction::Character& character)
{
  return (TextAbstraction::IsASCIIDigits(character) ||
          TextAbstraction::CHAR_NUMBER_SIGN == character ||
          TextAbstraction::CHAR_ASTERISK == character);
}

bool IsScriptChangedToFollowSequence(const TextAbstraction::Script&    currentRunScript,
                                     const TextAbstraction::Character& character,
                                     TextAbstraction::Script&          currentCharacterScript)
{
  bool isUpdated = false;

  // Keycap cases
  if(TextAbstraction::IsCombiningEnclosingKeycap(character))
  {
    if(TextAbstraction::EMOJI == currentRunScript)
    {
      // Keycap and unqualified
      // Emoji request a default presentation for an emoji character.
      isUpdated              = (currentCharacterScript != TextAbstraction::EMOJI);
      currentCharacterScript = TextAbstraction::EMOJI;
    }
    else if(TextAbstraction::EMOJI_COLOR == currentRunScript)
    {
      // Keycap and fully-qualified
      // Emoji request an emoji presentation for an emoji character.
      isUpdated              = (currentCharacterScript != TextAbstraction::EMOJI_COLOR);
      currentCharacterScript = TextAbstraction::EMOJI_COLOR;
    }
  }
  // Emoji(Text) Presentation cases
  else if(IsTextPresentationSequence(currentRunScript, character))
  {
    // Emoji request a text presentation for an emoji character.
    isUpdated              = (currentCharacterScript != TextAbstraction::EMOJI_TEXT);
    currentCharacterScript = TextAbstraction::EMOJI_TEXT;
  }
  // Emoji(Color) Presentation cases
  else if(IsEmojiPresentationSequence(currentRunScript, character))
  {
    // Emoji request an emoji presentation for an emoji character.
    isUpdated              = (currentCharacterScript != TextAbstraction::EMOJI_COLOR);
    currentCharacterScript = TextAbstraction::EMOJI_COLOR;
  }
  // Default Emoji
  else if(IsEmojiScript(currentRunScript) && IsEmojiScript(currentCharacterScript))
  {
    // Emoji request an emoji presentation for an emoji character.
    isUpdated              = (currentCharacterScript != TextAbstraction::EMOJI);
    currentCharacterScript = TextAbstraction::EMOJI;
  }
  // Emoji sequences
  else if(IsEmojiSequence(currentRunScript, character, currentCharacterScript) && currentCharacterScript != TextAbstraction::EMOJI_TEXT)
  {
    // Emoji request an emoji presentation for an emoji character.
    isUpdated              = (currentCharacterScript != TextAbstraction::EMOJI_COLOR);
    currentCharacterScript = TextAbstraction::EMOJI_COLOR;
  }

  return isUpdated;
}

Character GetVariationSelectorByScript(const TextAbstraction::Script& script)
{
  Character character = 0u;
  if(TextAbstraction::EMOJI_COLOR == script)
  {
    character = TextAbstraction::CHAR_VARIATION_SELECTOR_16;
  }
  else if(TextAbstraction::EMOJI_TEXT == script)
  {
    character = TextAbstraction::CHAR_VARIATION_SELECTOR_15;
  }

  return character;
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
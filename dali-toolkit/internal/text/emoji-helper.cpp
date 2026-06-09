
/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
namespace
{
struct EmojiElement
{
  Length                  nextIndex{0u};
  TextAbstraction::Script script{TextAbstraction::UNKNOWN};
  bool                    hasColorSelector{false};
  bool                    hasTextSelector{false};
  bool                    hasModifier{false};
  bool                    hasTagSequence{false};
};

bool IsPresentationSelector(Character character)
{
  return TextAbstraction::IsEmojiPresentationSelector(character) ||
         TextAbstraction::IsTextPresentationSelector(character);
}

bool IsEmojiCodepointRangeFallback(Character character)
{
  return (0x1F000u <= character && character <= 0x1FAFFu);
}

bool IsEmojiBase(Character character, TextAbstraction::Script script, bool allowComponentBase)
{
  if(TextAbstraction::IsEmojiPresentationSelector(character) ||
     TextAbstraction::IsTextPresentationSelector(character) ||
     TextAbstraction::IsZeroWidthJoiner(character) ||
     TextAbstraction::IsZeroWidthNonJoiner(character) ||
     TextAbstraction::IsCombiningEnclosingKeycap(character) ||
     TextAbstraction::IsEmojiModifier(character) ||
     TextAbstraction::IsTagSpec(character) ||
     TextAbstraction::IsTagEnd(character))
  {
    return false;
  }

  if(IsStartForKeycapSequence(character))
  {
    return false;
  }

  if(TextAbstraction::IsEmojiComponent(character))
  {
    return allowComponentBase;
  }

  if(TextAbstraction::IsNegativeSquaredLatinCapitalLetter(character))
  {
    return false;
  }

  return IsOneOfEmojiScripts(script) ||
         TextAbstraction::IsEmojiVariationSequences(character) ||
         TextAbstraction::IsRegionalIndicator(character) ||
         TextAbstraction::IsMiscellaneousSymbolsAndArrowsEmoji(character) ||
         TextAbstraction::IsDingbatsEmoji(character) ||
         IsEmojiCodepointRangeFallback(character);
}

bool IsVariationFallbackBase(Character character, TextAbstraction::Script script)
{
  if(TextAbstraction::IsEmojiPresentationSelector(character) ||
     TextAbstraction::IsTextPresentationSelector(character) ||
     TextAbstraction::IsZeroWidthJoiner(character) ||
     TextAbstraction::IsZeroWidthNonJoiner(character) ||
     TextAbstraction::IsCombiningEnclosingKeycap(character) ||
     TextAbstraction::IsEmojiModifier(character) ||
     TextAbstraction::IsEmojiComponent(character) ||
     TextAbstraction::IsTagSpec(character) ||
     TextAbstraction::IsTagEnd(character) ||
     TextAbstraction::IsNegativeSquaredLatinCapitalLetter(character))
  {
    return false;
  }

  return IsOneOfEmojiScripts(script) ||
         TextAbstraction::IsSymbolScript(script) ||
         IsStartForKeycapSequence(character) ||
         TextAbstraction::IsEmojiVariationSequences(character) ||
         TextAbstraction::IsMiscellaneousSymbolsAndArrowsEmoji(character) ||
         TextAbstraction::IsDingbatsEmoji(character) ||
         IsEmojiCodepointRangeFallback(character);
}

TextAbstraction::Script ResolveEmojiBaseScript(TextAbstraction::Script baseScript)
{
  if(TextAbstraction::COMMON == baseScript || TextAbstraction::UNKNOWN == baseScript)
  {
    return TextAbstraction::EMOJI;
  }

  return baseScript;
}

bool ConsumeTagSequence(const Character* const textBuffer, Length& index, const Length& lastCharacterIndex)
{
  const Length tagStartIndex = index;

  while(index <= lastCharacterIndex && TextAbstraction::IsTagSpec(*(textBuffer + index)))
  {
    ++index;
  }

  if(index <= lastCharacterIndex && TextAbstraction::IsTagEnd(*(textBuffer + index)) && index > tagStartIndex)
  {
    ++index;
    return true;
  }

  index = tagStartIndex;
  return false;
}

bool ConsumeEmojiElement(const Character* const textBuffer, const Length& currentCharacterIndex,
                         const Length& lastCharacterIndex, bool allowComponentBase, EmojiElement& element)
{
  if(currentCharacterIndex > lastCharacterIndex)
  {
    return false;
  }

  const Character               character = *(textBuffer + currentCharacterIndex);
  const TextAbstraction::Script script    = TextAbstraction::GetCharacterScript(character);
  const bool                    hasVariationFallbackSelector =
    (currentCharacterIndex + 1u <= lastCharacterIndex) &&
    IsPresentationSelector(*(textBuffer + currentCharacterIndex + 1u));

  if(!IsEmojiBase(character, script, allowComponentBase) &&
     !(hasVariationFallbackSelector && IsVariationFallbackBase(character, script)))
  {
    return false;
  }

  Length index = currentCharacterIndex + 1u;

  element.nextIndex = index;
  element.script    = ResolveEmojiBaseScript(script);

  while(index <= lastCharacterIndex && IsPresentationSelector(*(textBuffer + index)))
  {
    const Character selector = *(textBuffer + index);
    element.hasColorSelector = element.hasColorSelector || TextAbstraction::IsEmojiPresentationSelector(selector);
    element.hasTextSelector  = element.hasTextSelector || TextAbstraction::IsTextPresentationSelector(selector);
    ++index;
  }

  while(index <= lastCharacterIndex && TextAbstraction::IsEmojiModifier(*(textBuffer + index)))
  {
    element.hasModifier = true;
    ++index;
  }

  if(index <= lastCharacterIndex && TextAbstraction::IsTagSpec(*(textBuffer + index)))
  {
    element.hasTagSequence = ConsumeTagSequence(textBuffer, index, lastCharacterIndex);
  }

  if(element.hasColorSelector || element.hasModifier || element.hasTagSequence)
  {
    element.script = TextAbstraction::EMOJI_COLOR;
  }
  else if(element.hasTextSelector)
  {
    element.script = TextAbstraction::EMOJI_TEXT;
  }

  element.nextIndex = index;
  return true;
}

bool GetKeycapSequence(const Character* const textBuffer, const Length& currentCharacterIndex,
                       const Length& lastCharacterIndex, Length& sequenceLength,
                       TextAbstraction::Script& sequenceScript)
{
  const Character character = *(textBuffer + currentCharacterIndex);
  if(!IsStartForKeycapSequence(character))
  {
    return false;
  }

  Length index            = currentCharacterIndex + 1u;
  bool   hasColorSelector = false;
  bool   hasTextSelector  = false;

  while(index <= lastCharacterIndex && IsPresentationSelector(*(textBuffer + index)))
  {
    const Character selector = *(textBuffer + index);
    hasColorSelector         = hasColorSelector || TextAbstraction::IsEmojiPresentationSelector(selector);
    hasTextSelector          = hasTextSelector || TextAbstraction::IsTextPresentationSelector(selector);
    ++index;
  }

  if(index <= lastCharacterIndex && TextAbstraction::IsCombiningEnclosingKeycap(*(textBuffer + index)))
  {
    sequenceLength = index - currentCharacterIndex + 1u;
    if(hasColorSelector)
    {
      sequenceScript = TextAbstraction::EMOJI_COLOR;
    }
    else if(hasTextSelector)
    {
      sequenceScript = TextAbstraction::EMOJI_TEXT;
    }
    else
    {
      sequenceScript = TextAbstraction::EMOJI;
    }
    return true;
  }

  return false;
}

bool GetRegionalIndicatorSequence(const Character* const textBuffer, const Length& currentCharacterIndex,
                                  const Length& lastCharacterIndex, Length& sequenceLength,
                                  TextAbstraction::Script& sequenceScript)
{
  if(!TextAbstraction::IsRegionalIndicator(*(textBuffer + currentCharacterIndex)))
  {
    return false;
  }

  if(currentCharacterIndex + 1u <= lastCharacterIndex &&
     TextAbstraction::IsRegionalIndicator(*(textBuffer + currentCharacterIndex + 1u)))
  {
    sequenceLength = 2u;
    sequenceScript = TextAbstraction::EMOJI_COLOR;
    return true;
  }

  sequenceLength = 1u;
  sequenceScript = TextAbstraction::EMOJI;
  return true;
}

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

bool IsEmojiSequence(const TextAbstraction::Script& currentRunScript, const TextAbstraction::Character& character,
                     const TextAbstraction::Script& characterScript)
{
  return (!(TextAbstraction::IsNegativeSquaredLatinCapitalLetter(character)) &&
          (IsOneOfEmojiScripts(currentRunScript) &&
           (IsOneOfEmojiScripts(characterScript) || TextAbstraction::IsZeroWidthJoiner(character) ||
            TextAbstraction::IsZeroWidthNonJoiner(character) || TextAbstraction::IsEmojiItem(character) ||
            TextAbstraction::IsMiscellaneousSymbolsAndArrowsEmoji(character) ||
            TextAbstraction::IsDingbatsEmoji(character))));
}

bool IsNewKeycapSequence(const Character* const textBuffer, const Length& currentCharacterIndex,
                         const Length& lastCharacterIndex, TextAbstraction::Script& currentCharacterScript);

bool IsNewVariationSelectorSequence(const Character* const textBuffer, const TextAbstraction::Script& currentRunScript,
                                    const Length& currentCharacterIndex, const Length& lastCharacterIndex,
                                    TextAbstraction::Script& currentCharacterScript);

} // unnamed namespace

bool IsNewSequence(const Character* const textBuffer, const TextAbstraction::Script& currentRunScript,
                   const Length& currentCharacterIndex, const Length& lastCharacterIndex,
                   TextAbstraction::Script& currentCharacterScript)
{
  // Until now we have two cases : VariationSelector & Keycap
  // In-case there are more cases then should be added in this function

  return IsNewKeycapSequence(textBuffer, currentCharacterIndex, lastCharacterIndex, currentCharacterScript) ||
         IsNewVariationSelectorSequence(textBuffer, currentRunScript, currentCharacterIndex, lastCharacterIndex,
                                        currentCharacterScript);
}

bool GetEmojiSequence(const Character* const textBuffer, const Length& currentCharacterIndex,
                      const Length& lastCharacterIndex, const TextAbstraction::Script& currentCharacterScript,
                      Length& sequenceLength, TextAbstraction::Script& sequenceScript)
{
  sequenceLength = 0u;
  sequenceScript = currentCharacterScript;

  if(currentCharacterIndex > lastCharacterIndex)
  {
    return false;
  }

  if(GetKeycapSequence(textBuffer, currentCharacterIndex, lastCharacterIndex, sequenceLength, sequenceScript))
  {
    return true;
  }

  if(GetRegionalIndicatorSequence(textBuffer,
                                  currentCharacterIndex,
                                  lastCharacterIndex,
                                  sequenceLength,
                                  sequenceScript))
  {
    return true;
  }

  EmojiElement firstElement;
  if(!ConsumeEmojiElement(textBuffer, currentCharacterIndex, lastCharacterIndex, false, firstElement))
  {
    return false;
  }

  bool   hasJoinerSequence = false;
  Length nextIndex         = firstElement.nextIndex;
  sequenceScript           = firstElement.script;

  while(nextIndex <= lastCharacterIndex && TextAbstraction::IsZeroWidthJoiner(*(textBuffer + nextIndex)))
  {
    Length       afterJoinerIndex = nextIndex + 1u;
    EmojiElement nextElement;

    if(afterJoinerIndex <= lastCharacterIndex &&
       ConsumeEmojiElement(textBuffer, afterJoinerIndex, lastCharacterIndex, true, nextElement))
    {
      hasJoinerSequence = true;
      sequenceScript    = TextAbstraction::EMOJI_COLOR;
      nextIndex         = nextElement.nextIndex;
    }
    else
    {
      // Keep a dangling ZWJ with the preceding emoji unit. It is malformed, but shaping it with the preceding
      // emoji is a better fallback than splitting a sequence in the middle.
      hasJoinerSequence = true;
      sequenceScript    = TextAbstraction::EMOJI_COLOR;
      nextIndex         = afterJoinerIndex;
      break;
    }
  }

  if(hasJoinerSequence)
  {
    sequenceScript = TextAbstraction::EMOJI_COLOR;
  }

  sequenceLength = nextIndex - currentCharacterIndex;
  return sequenceLength > 0u;
}

namespace
{
bool IsNewKeycapSequence(const Character* const textBuffer, const Length& currentCharacterIndex,
                         const Length& lastCharacterIndex, TextAbstraction::Script& currentCharacterScript)
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
    } // if(IsStartForKeycapSequence(currentCharacter))
  } // if(currentCharacterIndex < lastCharacterIndex)

  return isNewKeycapSequence;
}

bool IsNewVariationSelectorSequence(const Character* const textBuffer, const TextAbstraction::Script& currentRunScript,
                                    const Length& currentCharacterIndex, const Length& lastCharacterIndex,
                                    TextAbstraction::Script& currentCharacterScript)
{
  // Ref: Emoji and Text Presentation Selectors: https://www.unicode.org/reports/tr51/#Emoji_Variation_Selectors
  // Ref: Emoji Variation Sequences for UTS #51:
  // https://www.unicode.org/Public/14.0.0/ucd/emoji/emoji-variation-sequences.txt

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
        else if(TextAbstraction::IsASCIIDigits(currentCharacter) || TextAbstraction::IsASCIIPS(currentCharacter))
        {
          // There is no variation selector.
          isNewVariationSelectorSequence = false;
        }
        else if(!TextAbstraction::IsZeroWidthJoiner(characterVS))
        {
          // Start of a new sequence if the next glyph is not a variation selector or zwj.
          isNewVariationSelectorSequence = true;
        }
      } // if(!isNewVariationSelectorSequence && currentCharacterIndex + 1 <= lastCharacterIndex)
    } // if(TextAbstraction::IsEmojiVariationSequences(currentCharacter))
    else if(!TextAbstraction::IsEmojiPresentationSelector(currentCharacter) &&
            !TextAbstraction::IsTextPresentationSelector(currentCharacter) &&
            !TextAbstraction::IsZeroWidthJoiner(currentCharacter) &&
            !TextAbstraction::IsEmojiModifier(currentCharacter))
    {
      if(currentCharacterIndex > 0)
      {
        Character prevCharacter = *(textBuffer + currentCharacterIndex - 1);
        if(TextAbstraction::IsEmojiVariationSequences(prevCharacter) &&
           !TextAbstraction::IsASCIIDigits(prevCharacter) && !TextAbstraction::IsASCIIPS(prevCharacter))
        {
          // The end of a variation sequence, start of a new sequence.
          isNewVariationSelectorSequence = true;
        }
      }
    }
  } // if(currentCharacterIndex < lastCharacterIndex)

  return isNewVariationSelectorSequence;
}
} // unnamed namespace

bool IsStartForKeycapSequence(const TextAbstraction::Character& character)
{
  return (TextAbstraction::IsASCIIDigits(character) || TextAbstraction::CHAR_NUMBER_SIGN == character ||
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
  else if(IsEmojiSequence(currentRunScript, character, currentCharacterScript) &&
          currentCharacterScript != TextAbstraction::EMOJI_TEXT)
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

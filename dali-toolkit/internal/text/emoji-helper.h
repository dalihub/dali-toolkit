#ifndef DALI_TOOLKIT_TEXT_EMOJI_HELPER_H
#define DALI_TOOLKIT_TEXT_EMOJI_HELPER_H

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

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/text-definitions.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/text-abstraction/emoji-character-properties.h>
#include <dali/devel-api/text-abstraction/emoji-helper.h>
#include <dali/devel-api/text-abstraction/script.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * @brief Whether the case is a new sequence and set script according to the case.
 *
 * @param[in] textBuffer The text.
 * @param[in] currentRunScript The script of current run.
 * @param[in] currentCharacterIndex The index of current character.
 * @param[in] lastCharacterIndex The index of last character.
 * @param[out] currentCharacterScript The current character script to update according to sequence.
 *
 * @return @e true the case is a new sequence
 */
bool IsNewSequence(const Character* const textBuffer, const TextAbstraction::Script& currentRunScript,
                   const Length& currentCharacterIndex, const Length& lastCharacterIndex,
                   TextAbstraction::Script& currentCharacterScript);

/**
 * @brief Finds an emoji sequence starting at @p currentCharacterIndex.
 *
 * This helper is used by script-run segmentation before shaping. Every character that belongs to the returned
 * sequence should keep the returned script so HarfBuzz receives the whole emoji sequence as one shaping unit.
 * It accepts both fully-qualified and common fallback forms, such as missing/wrong variation selectors around ZWJ
 * chains.
 *
 * @param[in] textBuffer The text.
 * @param[in] currentCharacterIndex The index of current character.
 * @param[in] lastCharacterIndex The index of last character.
 * @param[in] currentCharacterScript The script detected for the current character.
 * @param[out] sequenceLength Number of characters in the emoji sequence.
 * @param[out] sequenceScript Script to assign to every character in the sequence.
 *
 * @return @e true if an emoji sequence or emoji unit starts at @p currentCharacterIndex.
 */
bool GetEmojiSequence(const Character* const textBuffer, const Length& currentCharacterIndex,
                      const Length& lastCharacterIndex, const TextAbstraction::Script& currentCharacterScript,
                      Length& sequenceLength, TextAbstraction::Script& sequenceScript);

/**
 * @brief Whether the character is ASCII digits | # Number Sign | * Asterisk.
 *
 * @param[in] character The character.
 *
 * @return @e true if the character is ASCII digits | # Number Sign | * Asterisk.
 */
bool IsStartForKeycapSequence(const TextAbstraction::Character& character);

/**
 * @brief Check sequence case and update script of character if needed.
 *
 * @param[in] currentRunScript The script of current run.
 * @param[in] character The next character.
 * @param[out] script The current character script to update according to sequence.
 *
 * @return @e true if the script is changed
 */
bool IsScriptChangedToFollowSequence(const TextAbstraction::Script&    currentRunScript,
                                     const TextAbstraction::Character& character, TextAbstraction::Script& script);

/**
 * @brief Determine the Variation Selector according to script.
 *
 * @param[in] script The script.
 *
 * @return CHAR_VARIATION_SELECTOR_15 in-case EMOJI_TEXT or CHAR_VARIATION_SELECTOR_16 in-case EMOJI_COLOR or 0
 * otherwise
 */
Character GetVariationSelectorByScript(const TextAbstraction::Script& script);

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_EMOJI_HELPER_H

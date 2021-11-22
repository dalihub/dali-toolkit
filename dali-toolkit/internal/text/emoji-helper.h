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
 * @brief Whether the sequence is a variation sequence consisting of an emoji character followed by a text presentation selector.
 *
 * @param[in] currentRunScript The script of current run.
 * @param[in] character The next character.
 *
 * @return @e true if the sequence is text presentation sequence.
 */
bool IsTextPresentationSequence(const TextAbstraction::Script& currentRunScript, const TextAbstraction::Character& character);

/**
 * @brief Whether the sequence is a variation sequence consisting of an emoji character followed by a emoji presentation selector.
 *
 * @param[in] currentRunScript The script of current run.
 * @param[in] character The next character.
 *
 * @return @e true if the sequence is emoji presentation sequence.
 */
bool IsEmojiPresentationSequence(const TextAbstraction::Script&    currentRunScript,
                                 const TextAbstraction::Character& character);

/**
 * @brief Whether the sequence is an emoji sequence.
 *
 * @param[in] currentRunScript The script of current run.
 * @param[in] character The next character.
 * @param[in] characterScript The script of the next character.
 *
 * @return @e true if the sequence is an emoji sequence.
 */
bool IsEmojiSequence(const TextAbstraction::Script&    currentRunScript,
                     const TextAbstraction::Character& character,
                     const TextAbstraction::Script&    characterScript);

/**
 * @brief Whether the sequence is a keycap sequence and set script according to the case.
 *
 * @param[in] textBuffer The text.
 * @param[in] currentCharacterIndex The index of current character.
 * @param[in] lastCharacterIndex The index of last character.
 * @param[out] currentCharacterScript The current character script to update it in-case it's the Keycap sequence.
 *
 * @return @e true if @p currentRunScript is ASCII_DIGITS and @p character is COMBINING_ENCLOSING_KEYCAP
 */
bool IsNewKeycapSequence(const Character* const   textBuffer,
                         const Length&            currentCharacterIndex,
                         const Length&            lastCharacterIndex,
                         TextAbstraction::Script& currentCharacterScript);

/**
 * @brief Whether the sequence is a variation selector sequence and set script according to the case.
 *
 * @param[in] textBuffer The text.
 * @param[in] currentRunScript The script of current run.
 * @param[in] currentCharacterIndex The index of current character.
 * @param[in] lastCharacterIndex The index of last character.
 * @param[out] currentCharacterScript The current character script to update it in-case it's the VariationSelector sequence.
 *
 * @return @e true if @p currentRunScript is ASCII_DIGITS and @p character is COMBINING_ENCLOSING_KEYCAP
 */
bool IsNewVariationSelectorSequence(const Character* const         textBuffer,
                                    const TextAbstraction::Script& currentRunScript,
                                    const Length&                  currentCharacterIndex,
                                    const Length&                  lastCharacterIndex,
                                    TextAbstraction::Script&       currentCharacterScript);

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
bool IsNewSequence(const Character* const         textBuffer,
                   const TextAbstraction::Script& currentRunScript,
                   const Length&                  currentCharacterIndex,
                   const Length&                  lastCharacterIndex,
                   TextAbstraction::Script&       currentCharacterScript);

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
                                     const TextAbstraction::Character& character,
                                     TextAbstraction::Script&          script);

/**
 * @brief Check sequence case and update script of character if needed.
 *
 * @param[in] currentRunScript The script of current run.
 * @param[in] character The next character.
 * @param[out] script The current character script to update according to sequence.
 *
 * @return @e true if the script is changed
 */
bool HandleEmojiSequence(const Character* const         textBuffer,
                         const Length&                  currentCharacterIndex,
                         const Length&                  lastCharacterIndex,
                         const TextAbstraction::Script& currentRunScript,
                         TextAbstraction::Script&       currentCharacterScript);

/**
 * @brief Determine the Variation Selector according to script.
 *
 * @param[in] script The script.
 *
 * @return CHAR_VARIATION_SELECTOR_15 in-case EMOJI_TEXT or CHAR_VARIATION_SELECTOR_16 in-case EMOJI_COLOR or 0 otherwise
 */
Character GetVariationSelectorByScript(const TextAbstraction::Script& script);

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_EMOJI_HELPER_H
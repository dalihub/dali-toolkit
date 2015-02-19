#ifndef __DALI_TOOLKIT_TEXT_SCRIPT_H__
#define __DALI_TOOLKIT_TEXT_SCRIPT_H__

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/text/text-definitions.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

/**
 * @brief Retrieves a character's script.
 *
 * @param[in] character The character.
 *
 * @return The chraracter's script.
 */
Script GetCharacterScript( Character character );

/**
 * @brief Whether the character is a zero width non joiner.
 *
 * @param[in] character The character.
 *
 * @return @e true if the character is a zero width non joiner.
 */
bool IsZeroWidthNonJoiner( Character character );

/**
 * @brief Whether the character is a zero width joiner.
 *
 * @param[in] character The character.
 *
 * @return @e true if the character is a zero width joiner.
 */
bool IsZeroWidthJoiner( Character character );

/**
 * @brief Whether the character is a zero width space.
 *
 * @param[in] character The character.
 *
 * @return @e true if the character is a zero width space.
 */
bool IsZeroWidthSpace( Character character );

/**
 * @brief Whether the character is a left to right mark.
 *
 * @param[in] character The character.
 *
 * @return @e true if the character is a left to right mark.
 */
bool IsLeftToRightMark( Character character );

/**
 * @brief Whether the character is a right to left mark.
 *
 * @param[in] character The character.
 *
 * @return @e true if the character is a right to left mark.
 */
bool IsRightToLeftMark( Character character );

/**
 * @brief Whether the character is a thin space.
 *
 * @param[in] character The character.
 *
 * @return @e true if the character is a thin space.
 */
bool IsThinSpace( Character character );

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_SCRIPT_H__

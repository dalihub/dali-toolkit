#ifndef __DALI_TOOLKIT_CHARACTER_SET_CONVERSION_H__
#define __DALI_TOOLKIT_CHARACTER_SET_CONVERSION_H__

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

// EXTERNAL INCLUDES
#include <stdint.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

namespace Toolkit
{

/**
 * @brief Retrieves the number of characters of the text array encoded in UTF8
 *
 * @param[in] utf8 The pointer to the UTF8 array.
 * @param[in] length The length of the UTF8 array.
 *
 * @return The number of characters.
 */
uint32_t GetNumberOfUtf8Characters( const uint8_t* const utf8, uint32_t length );

/**
 * @brief Retrieves the number of bytes needed to encode in UTF8 the given text array encoded in UTF32.
 *
 * @param[in] utf32 The pointer to the UTF32 array.
 * @param[in] numberOfCharacters The number of characters of the UTF32 array.
 *
 * @return The number of bytes.
 */
uint32_t GetNumberOfUtf8Bytes( const uint32_t* const utf32, uint32_t numberOfCharacters );

/**
 * @brief Converts a text array encoded in UTF8 into a text array encoded in UTF32.
 *
 * The @p utf32 buffer needs to be big enough to store all the characters.
 *
 * @param[in] utf8 The pointer to the UTF8 array.
 * @param[in] length The length of the UTF8 array.
 * @param[out] utf32 The pointer to the UTF32 array.
 *
 * @return The number of characters.
 */
uint32_t Utf8ToUtf32( const uint8_t* const utf8, uint32_t length, uint32_t* utf32 );

/**
 * @brief Converts a text array encoded in UTF32 into a text array encoded in UTF8.
 *
 * The @p utf8 buffer needs to be big enough to store all the characters.
 *
 * @param[in] utf32 The pointer to the UTF32 array.
 * @param[in] numberOfCharacters The number of characters of the UTF32 array.
 * @param[out] utf8 The pointer to the UTF8 array.
 *
 * @return The number of bytes.
 */
uint32_t Utf32ToUtf8( const uint32_t* const utf32, uint32_t numberOfCharacters, uint8_t* utf8 );

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_CHARACTER_SET_CONVERSION_H__

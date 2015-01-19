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

// FILE HEATHER
#include <dali-toolkit/public-api/text/character-set-conversion.h>

namespace Dali
{

namespace Toolkit
{

namespace
{
  const static uint8_t U1 = 1u;
  const static uint8_t U2 = 2u;
  const static uint8_t U3 = 3u;
  const static uint8_t U4 = 4u;
  const static uint8_t U0 = 0u;
  const static uint8_t UTF8_LENGTH[256] = {
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, // lead byte = 0xxx xxxx (U+0000 - U+007F + some extended ascii characters)
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1, U1, U1, U1, U1, U1, U1, U1, U1, //
    U1, U1,                                 //

    U2, U2, U2, U2, U2, U2, U2, U2, U2, U2, //
    U2, U2, U2, U2, U2, U2, U2, U2, U2, U2, // lead byte = 110x xxxx (U+0080 - U+07FF)
    U2, U2, U2, U2, U2, U2, U2, U2, U2, U2, //
    U2, U2,                                 //

    U3, U3, U3, U3, U3, U3, U3, U3, U3, U3, // lead byte = 1110 xxxx (U+0800 - U+FFFF)
    U3, U3, U3, U3, U3, U3,                 //

    U4, U4, U4, U4, U4, U4, U4, U4,         // lead byte = 1111 0xxx (U+10000 - U+1FFFFF)

    U0, U0, U0, U0,                         // Non valid.
    U0, U0, U0, U0,                         // Non valid.
  };
} // namespace

uint32_t GetNumberOfUtf8Characters( const uint8_t* const utf8, uint32_t length )
{
  uint32_t numberOfCharacters = 0u;

  const uint8_t* begin = utf8;
  const uint8_t* end = utf8 + length;

  for( ; begin < end ; begin += UTF8_LENGTH[*begin], ++numberOfCharacters );

  return numberOfCharacters;
}

uint32_t GetNumberOfUtf8Bytes( const uint32_t* const utf32, uint32_t numberOfCharacters )
{
  uint32_t numberOfBytes = 0u;

  const uint32_t* begin = utf32;
  const uint32_t* end = utf32 + numberOfCharacters;

  for( ; begin < end; ++begin )
  {
    const uint32_t code = *begin;

    if( code < 0x80u )
    {
      ++numberOfBytes;
    }
    else if( code < 0x800u )
    {
      numberOfBytes += U2;
    }
    else if( code < 0x10000u )
    {
      numberOfBytes += U3;
    }
    else if( code < 0x200000u )
    {
      numberOfBytes += U4;
    }
  }

  return numberOfBytes;
}

uint32_t Utf8ToUtf32( const uint8_t* const utf8, uint32_t length, uint32_t* utf32 )
{
  uint32_t numberOfCharacters = 0u;

  const uint8_t* begin = utf8;
  const uint8_t* end = utf8 + length;

  for( ; begin < end ; ++numberOfCharacters )
  {
    const uint8_t leadByte = *begin;
    switch( UTF8_LENGTH[leadByte] )
    {
    case U1:
    {
      *utf32 = leadByte;
      begin++;
      break;
    }
    case U2:
    {
      uint32_t& code = *utf32;
      code = leadByte & 0x1fu;
      begin++;
      code <<= 6u;
      code |= *begin++ & 0x3fu;
      break;
    }
    case U3:
    {
      uint32_t& code = *utf32;
      code = leadByte & 0x1fu;
      begin++;
      code <<= 6u;
      code |= *begin++ & 0x3fu;
      code <<= 6u;
      code |= *begin++ & 0x3fu;
      break;
    }
    case U4:
    {
      uint32_t& code = *utf32;
      code = leadByte & 0x1fu;
      begin++;
      code <<= 6u;
      code |= *begin++ & 0x3fu;
      code <<= 6u;
      code |= *begin++ & 0x3fu;
      code <<= 6u;
      code |= *begin++ & 0x3fu;
      break;
    }
    }
  }

  return numberOfCharacters;
}

void Utf32ToUtf8( const uint32_t* const utf32, uint32_t numberOfCharacters, uint8_t* utf8 )
{
  const uint32_t* begin = utf32;
  const uint32_t* end = utf32 + numberOfCharacters;

  for( ; begin < end; ++begin )
  {
    const uint32_t code = *begin;

    if( code < 0x80u )
    {
      *utf8++ = code;
    }
    else if( code < 0x800u )
    {
      *utf8++ = static_cast<uint8_t>(   code >> 6u )           | 0xc0u; // lead byte for 2 byte sequence
      *utf8++ = static_cast<uint8_t>(   code          & 0x3f ) | 0x80u; // continuation byte
    }
    else if( code < 0x10000u )
    {
      *utf8++ = static_cast<uint8_t>(   code >> 12u )          | 0xe0u; // lead byte for 2 byte sequence
      *utf8++ = static_cast<uint8_t>( ( code >> 6u )  & 0x3f ) | 0x80u; // continuation byte
      *utf8++ = static_cast<uint8_t>(   code          & 0x3f ) | 0x80u; // continuation byte
    }
    else if( code < 0x200000u )
    {
      *utf8++ = static_cast<uint8_t>(   code >> 18u )          | 0xf0u; // lead byte for 2 byte sequence
      *utf8++ = static_cast<uint8_t>( ( code >> 12u ) & 0x3f ) | 0x80u; // continuation byte
      *utf8++ = static_cast<uint8_t>( ( code >> 6u )  & 0x3f ) | 0x80u; // continuation byte
      *utf8++ = static_cast<uint8_t>(   code          & 0x3f ) | 0x80u; // continuation byte
    }
  }
}

} // namespace Toolkit

} // namespace Dali

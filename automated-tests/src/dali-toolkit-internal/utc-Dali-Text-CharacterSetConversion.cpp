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

#include <iostream>

#include <stdlib.h>
#include <dali-toolkit/internal/text/character-set-conversion.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>


using namespace Dali;
using namespace Toolkit;
using namespace Text;

// Tests the following functions for scripts with different number of bytes per character.
// Latin 1 byte per character, Arabic 2 bytes per character, Devanagari 3 bytes per character and emojis 4 bytes per character.
//
// uint32_t GetNumberOfUtf8Characters( const uint8_t* const utf8, uint32_t length );
// uint32_t GetNumberOfUtf8Bytes( const uint32_t* const utf32, uint32_t numberOfCharacters );
// uint32_t Utf8ToUtf32( const uint8_t* const utf8, uint32_t length, uint32_t* utf32 );
// uint32_t Utf32ToUtf8( const uint32_t* const utf32, uint32_t numberOfCharacters, uint8_t* utf8 );
//     void Utf32ToUtf8( const uint32_t* const utf32, uint32_t numberOfCharacters, std::string& utf8 );
//

//////////////////////////////////////////////////////////

namespace
{

struct GetNumberOfUtf8CharactersData
{
  std::string  description;        ///< Description of the test.
  std::string  text;               ///< input text.
  unsigned int numberOfCharacters; ///< The expected number of characters.
};

bool GetNumberOfUtf8CharactersTest( const GetNumberOfUtf8CharactersData& data )
{
  return GetNumberOfUtf8Characters( reinterpret_cast<const uint8_t*>( data.text.c_str() ), data.text.size() ) == data.numberOfCharacters;
}

//////////////////////////////////////////////////////////

struct GetNumberOfUtf8BytesData
{
  std::string   description;        ///< Description of the test.
  unsigned int* utf32;              ///< input text in utf32.
  unsigned int  numberOfCharacters; ///< The number of characters.
  unsigned int  numberOfBytes;      ///< The expected number of bytes in utf8.
};

bool GetNumberOfUtf8BytesTest( const GetNumberOfUtf8BytesData& data )
{
  return GetNumberOfUtf8Bytes( data.utf32, data.numberOfCharacters ) == data.numberOfBytes;
}

//////////////////////////////////////////////////////////

struct Utf8ToUtf32Data
{
  std::string   description; ///< Description of the test.
  std::string   text;        ///< input text.
  unsigned int* utf32;       ///< The expected text (array of bytes with text encoded in utf32).
};


bool Utf8ToUtf32Test( const Utf8ToUtf32Data& data )
{
  Vector<uint32_t> utf32;
  utf32.Resize( data.text.size() );

  const uint32_t numberOfCharacters = Utf8ToUtf32( reinterpret_cast<const uint8_t* const>( data.text.c_str() ),
                                                   data.text.size(),
                                                   utf32.Begin() );

  for( unsigned int index = 0u; index < numberOfCharacters; ++index )
  {
    if( data.utf32[index] != utf32[index] )
    {
      return false;
    }
  }

  return true;
}

} // namespace

//////////////////////////////////////////////////////////

struct Utf32ToUtf8Data
{
  std::string   description;        ///< Description of the test.
  unsigned int* utf32;              ///< The input text (array of bytes with text encoded in utf32).
  unsigned int  numberOfCharacters; ///< The number of characters.
  std::string   text;               ///< The expected text.
};

bool Utf32ToUtf8Test( const Utf32ToUtf8Data& data )
{
  std::string text;

  Utf32ToUtf8( data.utf32, data.numberOfCharacters, text );

  return text == data.text;
}

//////////////////////////////////////////////////////////

int UtcDaliTextCharacterSetConversionGetNumberOfUtf8Characters(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextCharacterSetConversionGetNumberOfUtf8Characters");

  const GetNumberOfUtf8CharactersData data[] =
  {
    {
      "Latin script",
      "Hello World",
      11u,
    },
    {
      "Arabic script",
      "مرحبا بالعالم",
      13u,
    },
    {
      "Devanagari script",
      "हैलो वर्ल्ड",
      11u,
    },
    {
      "Emojis",
      "\xF0\x9F\x98\x81 \xF0\x9F\x98\x82 \xF0\x9F\x98\x83 \xF0\x9F\x98\x84",
      7u,
    },
  };
  const unsigned int numberOfTests = 4u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    if( !GetNumberOfUtf8CharactersTest( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextCharacterSetConversionGetNumberOfUtf8Bytes(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextCharacterSetConversionGetNumberOfUtf8Bytes");

  unsigned int utf32_01[] = { 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64 }; // Hello World
  unsigned int utf32_02[] = { 0x645, 0x631, 0x62D, 0x628, 0x627, 0x20, 0x628, 0x627, 0x644, 0x639, 0x627, 0x644, 0x645 }; // مرحبا بالعالم
  unsigned int utf32_03[] = { 0x939, 0x948, 0x932, 0x94B, 0x20, 0x935, 0x930, 0x94D, 0x932, 0x94D, 0x921 }; // हैलो वर्ल्ड
  unsigned int utf32_04[] = { 0x1F601, 0x20, 0x1F602, 0x20, 0x1F603, 0x20, 0x1F604 }; // Emojis

  const GetNumberOfUtf8BytesData data[] =
  {
    {
      "Latin script",
      utf32_01,
      11u,
      11u,
    },
    {
      "Arabic script",
      utf32_02,
      13u,
      25u,
    },
    {
      "Devanagari script",
      utf32_03,
      11u,
      31u,
    },
    {
      "Emojis",
      utf32_04,
      7u,
      19u,
    },
  };
  const unsigned int numberOfTests = 4u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    if( !GetNumberOfUtf8BytesTest( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextCharacterSetConversionUtf8ToUtf32(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextCharacterSetConversionGetNumberOfUtf8Bytes");

  unsigned int utf32_01[] = { 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64 }; // Hello World
  unsigned int utf32_02[] = { 0x645, 0x631, 0x62D, 0x628, 0x627, 0x20, 0x628, 0x627, 0x644, 0x639, 0x627, 0x644, 0x645 }; // مرحبا بالعالم
  unsigned int utf32_03[] = { 0x939, 0x948, 0x932, 0x94B, 0x20, 0x935, 0x930, 0x94D, 0x932, 0x94D, 0x921 }; // हैलो वर्ल्ड
  unsigned int utf32_04[] = { 0x1F601, 0x20, 0x1F602, 0x20, 0x1F603, 0x20, 0x1F604 }; // Emojis

  const Utf8ToUtf32Data data[] =
  {
    {
      "Latin script",
      "Hello World",
      utf32_01,
    },
    {
      "Arabic script",
      "مرحبا بالعالم",
      utf32_02,
    },
    {
      "Devanagari script",
      "हैलो वर्ल्ड",
      utf32_03,
    },
    {
      "Emojis",
      "\xF0\x9F\x98\x81 \xF0\x9F\x98\x82 \xF0\x9F\x98\x83 \xF0\x9F\x98\x84",
      utf32_04,
    },
  };
  const unsigned int numberOfTests = 4u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    if( !Utf8ToUtf32Test( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextCharacterSetConversionUtf32ToUtf8(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextCharacterSetConversionUtf32ToUtf8");

  unsigned int utf32_01[] = { 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64 }; // Hello World
  unsigned int utf32_02[] = { 0x645, 0x631, 0x62D, 0x628, 0x627, 0x20, 0x628, 0x627, 0x644, 0x639, 0x627, 0x644, 0x645 }; // مرحبا بالعالم
  unsigned int utf32_03[] = { 0x939, 0x948, 0x932, 0x94B, 0x20, 0x935, 0x930, 0x94D, 0x932, 0x94D, 0x921 }; // हैलो वर्ल्ड
  unsigned int utf32_04[] = { 0x1F601, 0x20, 0x1F602, 0x20, 0x1F603, 0x20, 0x1F604 }; // Emojis

  struct Utf32ToUtf8Data data[] =
  {
    {
      "Latin script",
      utf32_01,
      11u,
      "Hello World",
    },
    {
      "Arabic script",
      utf32_02,
      13u,
      "مرحبا بالعالم",
    },
    {
      "Devanagari script",
      utf32_03,
      11u,
      "हैलो वर्ल्ड",
    },
    {
      "Emojis",
      utf32_04,
      7u,
      "\xF0\x9F\x98\x81 \xF0\x9F\x98\x82 \xF0\x9F\x98\x83 \xF0\x9F\x98\x84",
    },
  };

  const unsigned int numberOfTests = 4u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    if( !Utf32ToUtf8Test( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

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
#include <dali-toolkit/internal/text/segmentation.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>


using namespace Dali;
using namespace Toolkit;
using namespace Text;

// Tests the following functions with different scripts.
// void SetLineBreakInfo( const Vector<Character>& text, Vector<LineBreakInfo>& lineBreakInfo );
// void SetWordBreakInfo( const Vector<Character>& text, Vector<WordBreakInfo>& wordBreakInfo );

//////////////////////////////////////////////////////////

namespace
{

struct BreakInfoData
{
  std::string description; ///< Description of the test.
  std::string text;        ///< input text.
  std::string breakInfo;   ///< The expected break info.
};

bool LineBreakInfoTest( const BreakInfoData& data )
{
  // 1) Convert to utf32
  Vector<Character> utf32;
  utf32.Resize( data.text.size() );

  const uint32_t numberOfCharacters = Utf8ToUtf32( reinterpret_cast<const uint8_t* const>( data.text.c_str() ),
                                                   data.text.size(),
                                                   &utf32[0u] );
  utf32.Resize( numberOfCharacters );

  // 2) Set the line break info.
  Vector<LineBreakInfo> lineBreakInfo;
  lineBreakInfo.Resize( numberOfCharacters );

  SetLineBreakInfo( utf32, lineBreakInfo );

  // 3) compare the results
  std::ostringstream breakInfo;

  for( unsigned int index = 0u; index < numberOfCharacters; ++index )
  {
    breakInfo << static_cast<unsigned int>( lineBreakInfo[index] );
  }

  return data.breakInfo == breakInfo.str();
}

bool WordBreakInfoTest( const BreakInfoData& data )
{
  // 1) Convert to utf32
  Vector<Character> utf32;
  utf32.Resize( data.text.size() );

  const uint32_t numberOfCharacters = Utf8ToUtf32( reinterpret_cast<const uint8_t* const>( data.text.c_str() ),
                                                   data.text.size(),
                                                   &utf32[0u] );
  utf32.Resize( numberOfCharacters );

  // 2) Set the word break info.
  Vector<WordBreakInfo> wordBreakInfo;
  wordBreakInfo.Resize( numberOfCharacters );

  SetWordBreakInfo( utf32, wordBreakInfo );

  // 3) compare the results
  std::ostringstream breakInfo;

  for( unsigned int index = 0u; index < numberOfCharacters; ++index )
  {
    breakInfo << static_cast<unsigned int>( wordBreakInfo[index] );
  }

  return data.breakInfo == breakInfo.str();
}

} // namespace

//////////////////////////////////////////////////////////

int UtcDaliTextSegnemtationSetLineBreakInfo(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextSegnemtationSetLineBreakInfo");

  struct BreakInfoData data[] =
  {
    {
      "Zero characters",
      "",
      "",
    },
    {
      "Latin script",
      "Hello world",
      "22222122220",
    },
    {
      "Latin script with \n",
      "Hello\nworld",
      "22222022220",
    },
    {
      "Japanese script",
      "こんにちは世界",
      "1111110",
    },
    {
      "Japanese script with \n",
      "こんにちは\n世界",
      "11112010",
    },
    {
      "Chinese script",
      "你好世界",
      "1110",
    },
    {
      "Chinese script with \n",
      "你好\n世界",
      "12010",
    }
  };
  const unsigned int numberOfTests = 7u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    if( !LineBreakInfoTest( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextSegnemtationSetWordBreakInfo(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextSegnemtationSetWordBreakInfo");

  struct BreakInfoData data[] =
  {
    {
      "Zero characters",
      "",
      "",
    },
    {
      "Latin script",
      "Hello world",
      "11110011110",
    },
    {
      "Latin script with \n",
      "Hello\nworld",
      "11110011110",
    },
    {
      "Japanese script",
      "こんにちは世界",
      "0000000",
    },
    {
      "Japanese script with \n",
      "こんにちは\n世界",
      "00000000",
    },
    {
      "Chinese script",
      "你好世界",
      "0000",
    },
    {
      "Chinese script with \n",
      "你好\n世界",
      "00000",
    }
  };
  const unsigned int numberOfTests = 7u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    if( !WordBreakInfoTest( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

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
// void SetLineBreakInfo( const Vector<Character>& text,
//                        Vector<LineBreakInfo>& lineBreakInfo );
// void SetWordBreakInfo( const Vector<Character>& text,
//                        CharacterIndex startIndex,
//                        Length numberOfCharacters,
//                        Vector<WordBreakInfo>& wordBreakInfo );

//////////////////////////////////////////////////////////

namespace
{

struct BreakInfoData
{
  std::string description;        ///< Description of the test.
  std::string text;               ///< input text.
  uint32_t    index;              ///< The index from where to start to query the break info.
  uint32_t    numberOfCharacters; ///< The requested number of characters.
  std::string breakInfo;          ///< The expected break info.
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

  if( data.breakInfo != breakInfo.str() )
  {
    std::cout << "  expected : [" << data.breakInfo << "]" << std::endl;
    std::cout << "       got : [" << breakInfo.str() << "]" << std::endl;
    return false;
  }

  return true;
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

  // 2) Set the word break info for the whole text.
  Vector<WordBreakInfo> wordBreakInfo;
  wordBreakInfo.Resize( numberOfCharacters );

  SetWordBreakInfo( utf32,
                    0u,
                    numberOfCharacters,
                    wordBreakInfo );

  // 3) Update the word text info if it's requested for part of the text.
  if( ( 0u != data.index ) &&
      ( numberOfCharacters != data.numberOfCharacters ) )
  {
    // Clear part of the word break info.
    wordBreakInfo.Erase( wordBreakInfo.Begin() + data.index,
                         wordBreakInfo.Begin() + data.index + data.numberOfCharacters );

    // Update the word break info.
    SetWordBreakInfo( utf32,
                      data.index,
                      data.numberOfCharacters,
                      wordBreakInfo );
  }

  // 4) compare the results
  std::ostringstream breakInfo;

  for( unsigned int index = 0u; index < numberOfCharacters; ++index )
  {
    breakInfo << static_cast<unsigned int>( wordBreakInfo[index] );
  }

  if( data.breakInfo != breakInfo.str() )
  {
    std::cout << "                text : [" << data.text << "]" << std::endl;
    std::cout << "               index : " <<  data.index << std::endl;
    std::cout << "  numberOfCharacters : " <<  data.numberOfCharacters << std::endl;
    std::cout << "            expected : [" << data.breakInfo << "]" << std::endl;
    std::cout << "                 got : [" << breakInfo.str() << "]" << std::endl;
    return false;
  }

  return true;
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
      0u,
      0u,
      "",
    },
    {
      "Latin script",
      "Lorem ipsum dolor sit amet, aeque definiebas ea mei, posse iracundia ne cum.\n"
      "Usu ne nisl maiorum iudicabit, veniam epicurei oporteat eos an.\n"
      "Ne nec nulla regione albucius, mea doctus delenit ad!\n"
      "Et everti blandit adversarium mei, eam porro neglegentur suscipiantur an.\n"
      "Quidam corpora at duo. An eos possim scripserit?",
      0u,
      317u,
      "22222122222122222122212222212222212222222222122122221222221222222222122122220"
      "2221221222212222222122222222221222222122222222122222222122212220"
      "221222122222122222221222222222122212222221222222212220"
      "22122222212222222122222222222122221222122222122222222222122222222222212220"
      "222222122222221221222212212221222222122222222220",
    },
    {
      "Japanese script",
      "韓国側は北朝鮮当局を通じて米ドルで賃金を支払う。\n"
      "国際社会から様々な経済制裁を受ける北朝鮮にとっては出稼ぎ労働などと並んで重要な外貨稼ぎの手段となっている。\n"
      "韓国統一省によると15年だけで1320億ウォン（約130億円）が同工業団地を通じ北朝鮮に支払われたという。",
      0u,
      132u,
      "1111111111111111111111220"
      "111111211111111111111111111111111111111111111111111220"
      "11111111121111122211111212211211111111111111111111120",
    },
    {
      "Chinese script",
      "在被捕的64人中，警方落案起訴了35名男子和3名女子，他們年齡介乎15到70歲。\n"
      "38人中有1人獲准保釋。\n"
      "16名年齡介乎14到33歲的被捕人士獲准保釋候查，另有10人仍被拘留作進一步調查。",
      0u,
      95u,
      "11112112111111112111111112111111121121220"
      "2111111111220"
      "21111112112111111111111211121111111111120",
    }
  };
  const unsigned int numberOfTests = 4u;

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
      "Zero characters.",
      "",
      0u,
      0u,
      "",
    },
    {
      "Latin script, full text.",
      "Lorem ipsum dolor sit amet, aeque definiebas ea mei, posse iracundia ne cum.\n"
      "Usu ne nisl maiorum iudicabit, veniam epicurei oporteat eos an.\n"
      "Ne nec nulla regione albucius, mea doctus delenit ad!\n"
      "Et everti blandit adversarium mei, eam porro neglegentur suscipiantur an.\n"
      "Quidam corpora at duo. An eos possim scripserit?",
      0u,
      317u,
      "11110011110011110011001110001111001111111110010011000111100111111110010011000"
      "1100100111001111110011111111000111110011111110011111110011001000"
      "100110011110011111100111111100011001111100111111001000"
      "10011111001111110011111111110011000110011110011111111110011111111111001000"
      "111110011111100100110001001100111110011111111100",
    },
    {
      "Latin script, update first paragraph.",
      "Lorem ipsum dolor sit amet, aeque definiebas ea mei, posse iracundia ne cum.\n"
      "Usu ne nisl maiorum iudicabit, veniam epicurei oporteat eos an.\n"
      "Ne nec nulla regione albucius, mea doctus delenit ad!\n"
      "Et everti blandit adversarium mei, eam porro neglegentur suscipiantur an.\n"
      "Quidam corpora at duo. An eos possim scripserit?",
      0u,
      77u,
      "11110011110011110011001110001111001111111110010011000111100111111110010011000"
      "1100100111001111110011111111000111110011111110011111110011001000"
      "100110011110011111100111111100011001111100111111001000"
      "10011111001111110011111111110011000110011110011111111110011111111111001000"
      "111110011111100100110001001100111110011111111100",
    },
    {
      "Latin script, update middle paragraphs.",
      "Lorem ipsum dolor sit amet, aeque definiebas ea mei, posse iracundia ne cum.\n"
      "Usu ne nisl maiorum iudicabit, veniam epicurei oporteat eos an.\n"
      "Ne nec nulla regione albucius, mea doctus delenit ad!\n"
      "Et everti blandit adversarium mei, eam porro neglegentur suscipiantur an.\n"
      "Quidam corpora at duo. An eos possim scripserit?",
      77u,
      118u,
      "11110011110011110011001110001111001111111110010011000111100111111110010011000"
      "1100100111001111110011111111000111110011111110011111110011001000"
      "100110011110011111100111111100011001111100111111001000"
      "10011111001111110011111111110011000110011110011111111110011111111111001000"
      "111110011111100100110001001100111110011111111100",
    },
    {
      "Latin script, update last paragraph.",
      "Lorem ipsum dolor sit amet, aeque definiebas ea mei, posse iracundia ne cum.\n"
      "Usu ne nisl maiorum iudicabit, veniam epicurei oporteat eos an.\n"
      "Ne nec nulla regione albucius, mea doctus delenit ad!\n"
      "Et everti blandit adversarium mei, eam porro neglegentur suscipiantur an.\n"
      "Quidam corpora at duo. An eos possim scripserit?",
      269u,
      48u,
      "11110011110011110011001110001111001111111110010011000111100111111110010011000"
      "1100100111001111110011111111000111110011111110011111110011001000"
      "100110011110011111100111111100011001111100111111001000"
      "10011111001111110011111111110011000110011110011111111110011111111111001000"
      "111110011111100100110001001100111110011111111100",
    },
    {
      "Japanese script, full text.",
      "韓国側は北朝鮮当局を通じて米ドルで賃金を支払う。\n"
      "国際社会から様々な経済制裁を受ける北朝鮮にとっては出稼ぎ労働などと並んで重要な外貨稼ぎの手段となっている。\n"
      "韓国統一省によると15年だけで1320億ウォン（約130億円）が同工業団地を通じ北朝鮮に支払われたという。",
      0u,
      132u,
      "0000000000000010000000000"
      "000000000000000000000000000000000000000000000000000000"
      "00000000010000011100110001100000000000000000000000000",
    },
    {
      "Japanese script, update first paragraph.",
      "韓国側は北朝鮮当局を通じて米ドルで賃金を支払う。\n"
      "国際社会から様々な経済制裁を受ける北朝鮮にとっては出稼ぎ労働などと並んで重要な外貨稼ぎの手段となっている。\n"
      "韓国統一省によると15年だけで1320億ウォン（約130億円）が同工業団地を通じ北朝鮮に支払われたという。",
      0u,
      25u,
      "0000000000000010000000000"
      "000000000000000000000000000000000000000000000000000000"
      "00000000010000011100110001100000000000000000000000000",
    },
    {
      "Japanese script, update middle paragraph.",
      "韓国側は北朝鮮当局を通じて米ドルで賃金を支払う。\n"
      "国際社会から様々な経済制裁を受ける北朝鮮にとっては出稼ぎ労働などと並んで重要な外貨稼ぎの手段となっている。\n"
      "韓国統一省によると15年だけで1320億ウォン（約130億円）が同工業団地を通じ北朝鮮に支払われたという。",
      25u,
      54u,
      "0000000000000010000000000"
      "000000000000000000000000000000000000000000000000000000"
      "00000000010000011100110001100000000000000000000000000",
    },
    {
      "Japanese script, update last paragraph.",
      "韓国側は北朝鮮当局を通じて米ドルで賃金を支払う。\n"
      "国際社会から様々な経済制裁を受ける北朝鮮にとっては出稼ぎ労働などと並んで重要な外貨稼ぎの手段となっている。\n"
      "韓国統一省によると15年だけで1320億ウォン（約130億円）が同工業団地を通じ北朝鮮に支払われたという。",
      79u,
      53u,
      "0000000000000010000000000"
      "000000000000000000000000000000000000000000000000000000"
      "00000000010000011100110001100000000000000000000000000",
    },
    {
      "Chinese script, full text.",
      "在被捕的64人中，警方落案起訴了35名男子和3名女子，他們年齡介乎15到70歲。\n"
      "38人中有1人獲准保釋。\n"
      "16名年齡介乎14到33歲的被捕人士獲准保釋候查，另有10人仍被拘留作進一步調查。",
      0u,
      95u,
      "00001000000000001000000000000000010010000"
      "1000000000000"
      "10000001001000000000000000010000000000000",
    },
    {
      "Chinese script, update first paragraph.",
      "在被捕的64人中，警方落案起訴了35名男子和3名女子，他們年齡介乎15到70歲。\n"
      "38人中有1人獲准保釋。\n"
      "16名年齡介乎14到33歲的被捕人士獲准保釋候查，另有10人仍被拘留作進一步調查。",
      0u,
      41u,
      "00001000000000001000000000000000010010000"
      "1000000000000"
      "10000001001000000000000000010000000000000",
    },
    {
      "Chinese script, update middle paragraph.",
      "在被捕的64人中，警方落案起訴了35名男子和3名女子，他們年齡介乎15到70歲。\n"
      "38人中有1人獲准保釋。\n"
      "16名年齡介乎14到33歲的被捕人士獲准保釋候查，另有10人仍被拘留作進一步調查。",
      41u,
      13u,
      "00001000000000001000000000000000010010000"
      "1000000000000"
      "10000001001000000000000000010000000000000",
    },
    {
      "Chinese script, update last paragraph.",
      "在被捕的64人中，警方落案起訴了35名男子和3名女子，他們年齡介乎15到70歲。\n"
      "38人中有1人獲准保釋。\n"
      "16名年齡介乎14到33歲的被捕人士獲准保釋候查，另有10人仍被拘留作進一步調查。",
      54u,
      41u,
      "00001000000000001000000000000000010010000"
      "1000000000000"
      "10000001001000000000000000010000000000000",
    }
  };
  const unsigned int numberOfTests = 13u;

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

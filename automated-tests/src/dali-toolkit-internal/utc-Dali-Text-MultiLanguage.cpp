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
#include <dali-toolkit/internal/text/multi-language-support.h>
#include <dali-toolkit/internal/text/logical-model-impl.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>


using namespace Dali;
using namespace Toolkit;
using namespace Text;

// Tests the following functions with different scripts.
// Constructor, destructor and MultilanguageSupport::Get()
// void MultilanguageSupport::SetScripts( const Vector<Character>& text, const Vector<LineBreakInfo>& lineBreakInfo, Vector<ScriptRun>& scripts );
// void MultilanguageSupport::ValidateFonts( const Vector<Character>& text,
//                                           const Vector<ScriptRun>& scripts,
//                                           const Vector<FontDescriptionRun>& fontDescriptions,
//                                           FontId defaultFontId,
//                                           Vector<FontRun>& fonts );

//////////////////////////////////////////////////////////

namespace
{

struct ScriptsData
{
  std::string description;      ///< Description of the experiment.
  std::string text;             ///< Input text.
  Vector<ScriptRun> scriptRuns; ///< Expected script runs.
};

struct ValidateFontsData
{
  std::string description;  ///< Description of the experiment.
  std::string text;         ///< Input text.
};

//////////////////////////////////////////////////////////

bool ScriptsTest( const ScriptsData& data )
{
  MultilanguageSupport multilanguageSupport = MultilanguageSupport::Get();

  // 1) Convert to utf32
  Vector<Character> utf32;
  utf32.Resize( data.text.size() );

  const uint32_t numberOfCharacters = Utf8ToUtf32( reinterpret_cast<const uint8_t* const>( data.text.c_str() ),
                                                   data.text.size(),
                                                   &utf32[0u] );
  utf32.Resize( numberOfCharacters );

  // 2) Set the script info.
  Vector<ScriptRun> scripts;
  multilanguageSupport.SetScripts( utf32,
                                   scripts );

  // 3) Compare the results.

  tet_printf( "Testing %s\n", data.description.c_str() );
  if( scripts.Count() != data.scriptRuns.Count() )
  {
    tet_printf("ScriptsTest FAIL: different number of scripts. %d, should be %d\n", scripts.Count(), data.scriptRuns.Count() );
    return false;
  }

  for( unsigned int index = 0u; index < scripts.Count(); ++index )
  {
    const ScriptRun& scriptRun1 = scripts[index];
    const ScriptRun& scriptRun2 = data.scriptRuns[index];

    if( scriptRun1.characterRun.characterIndex != scriptRun2.characterRun.characterIndex )
    {
      tet_printf("ScriptsTest FAIL: different character index. %d, should be %d\n", scriptRun1.characterRun.characterIndex, scriptRun2.characterRun.characterIndex );
      return false;
    }

    if( scriptRun1.characterRun.numberOfCharacters != scriptRun2.characterRun.numberOfCharacters )
    {
      tet_printf("ScriptsTest FAIL: different number of characters. %d, should be %d\n", scriptRun1.characterRun.numberOfCharacters, scriptRun2.characterRun.numberOfCharacters );
      return false;
    }

    if( scriptRun1.script != scriptRun2.script )
    {
      tet_printf("ScriptsTest FAIL: different script. %s, should be %s\n", TextAbstraction::ScriptName[scriptRun1.script], TextAbstraction::ScriptName[scriptRun2.script] );
      return false;
    }
  }

  return true;
}

bool ValidateFontTest( const ValidateFontsData& data )
{
  MultilanguageSupport multilanguageSupport = MultilanguageSupport::Get();

  // 1) Convert to utf32
  Vector<Character> utf32;
  utf32.Resize( data.text.size() );

  const uint32_t numberOfCharacters = Utf8ToUtf32( reinterpret_cast<const uint8_t* const>( data.text.c_str() ),
                                                   data.text.size(),
                                                   &utf32[0u] );
  utf32.Resize( numberOfCharacters );

  // 2) Set the script info.
  Vector<ScriptRun> scripts;
  multilanguageSupport.SetScripts( utf32,
                                   scripts );

  // To be completed ...
  Vector<FontDescriptionRun> fontDescriptions;
  FontId defaultFontId = 0u;
  Vector<FontRun> fonts;

  // 3) Validate the fonts
  multilanguageSupport.ValidateFonts( utf32,
                                      scripts,
                                      fontDescriptions,
                                      defaultFontId,
                                      fonts );
  return true;
}

} // namespace

int UtcDaliTextMultiLanguageConstructor(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextMultiLanguageConstructor");

  MultilanguageSupport multilanguageSupport;
  DALI_TEST_CHECK( !multilanguageSupport );

  MultilanguageSupport multilanguageSupport1 = MultilanguageSupport::Get();
  DALI_TEST_CHECK( multilanguageSupport1 );

  // To increase coverage.
  MultilanguageSupport multilanguageSupport2 = MultilanguageSupport::Get();
  DALI_TEST_CHECK( multilanguageSupport2 );

  DALI_TEST_CHECK( multilanguageSupport1 == multilanguageSupport2 );

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextMultiLanguageSetScripts(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextMultiLanguageSetScripts" );

  // Void text.
  Vector<ScriptRun> scriptRuns00;

  // Hello world.
  Vector<ScriptRun> scriptRuns01;
  ScriptRun scriptRun0100 =
  {
    {
      0u,
      11u,
    },
    TextAbstraction::LATIN
  };
  scriptRuns01.PushBack( scriptRun0100 );

  // Mix of LTR '\n'and RTL
  Vector<ScriptRun> scriptRuns02;
  ScriptRun scriptRun0200 =
  {
    {
      0u,
      12u,
    },
    TextAbstraction::LATIN
  };
  ScriptRun scriptRun0201 =
  {
    {
      12u,
      13u,
    },
    TextAbstraction::ARABIC
  };
  scriptRuns02.PushBack( scriptRun0200 );
  scriptRuns02.PushBack( scriptRun0201 );

  // Mix of RTL '\n'and LTR
  Vector<ScriptRun> scriptRuns03;
  ScriptRun scriptRun0300 =
  {
    {
      0u,
      14u,
    },
    TextAbstraction::ARABIC
  };
  ScriptRun scriptRun0301 =
  {
    {
      14u,
      11u,
    },
    TextAbstraction::LATIN
  };
  scriptRuns03.PushBack( scriptRun0300 );
  scriptRuns03.PushBack( scriptRun0301 );

  // White spaces. At the beginning of the text.
  Vector<ScriptRun> scriptRuns04;
  ScriptRun scriptRun0400 =
  {
    {
      0u,
      16u,
    },
    TextAbstraction::LATIN
  };
  scriptRuns04.PushBack( scriptRun0400 );

  // White spaces. At the end of the text.
  Vector<ScriptRun> scriptRuns05;
  ScriptRun scriptRun0500 =
  {
    {
      0u,
      16u,
    },
    TextAbstraction::LATIN
  };
  scriptRuns05.PushBack( scriptRun0500 );

  // White spaces. At the middle of the text.
  Vector<ScriptRun> scriptRuns06;
  ScriptRun scriptRun0600 =
  {
    {
      0u,
      16u,
    },
    TextAbstraction::LATIN
  };
  scriptRuns06.PushBack( scriptRun0600 );

  // White spaces between different scripts.
  Vector<ScriptRun> scriptRuns07;
  ScriptRun scriptRun0700 =
  {
    {
      0u,
      8u,
    },
    TextAbstraction::LATIN
  };
  ScriptRun scriptRun0701 =
  {
    {
      8u,
      5u,
    },
    TextAbstraction::HANGUL
  };
  scriptRuns07.PushBack( scriptRun0700 );
  scriptRuns07.PushBack( scriptRun0701 );

  // White spaces between different scripts and differetn directions. Starting LTR.
  Vector<ScriptRun> scriptRuns08;
  ScriptRun scriptRun0800 =
  {
    {
      0u,
      18u,
    },
    TextAbstraction::LATIN
  };
  ScriptRun scriptRun0801 =
  {
    {
      18u,
      14u,
    },
    TextAbstraction::ARABIC
  };
  ScriptRun scriptRun0802 =
  {
    {
      32u,
      18u,
    },
    TextAbstraction::HANGUL
  };
  scriptRuns08.PushBack( scriptRun0800 );
  scriptRuns08.PushBack( scriptRun0801 );
  scriptRuns08.PushBack( scriptRun0802 );

  // White spaces between different scripts and differetn directions. Starting RTL.
  Vector<ScriptRun> scriptRuns09;
  ScriptRun scriptRun0900 =
  {
    {
      0u,
      21u,
    },
    TextAbstraction::ARABIC
  };
  ScriptRun scriptRun0901 =
  {
    {
      21u,
      16u,
    },
    TextAbstraction::LATIN
  };
  ScriptRun scriptRun0902 =
  {
    {
      37u,
      10u,
    },
    TextAbstraction::HANGUL
  };
  ScriptRun scriptRun0903 =
  {
    {
      47u,
      20u,
    },
    TextAbstraction::ARABIC
  };
  scriptRuns09.PushBack( scriptRun0900 );
  scriptRuns09.PushBack( scriptRun0901 );
  scriptRuns09.PushBack( scriptRun0902 );
  scriptRuns09.PushBack( scriptRun0903 );

  // Paragraphs with different directions.
  Vector<ScriptRun> scriptRuns10;
  ScriptRun scriptRun1000 =
  {
    {
      0u,
      20u,
    },
    TextAbstraction::ARABIC
  };
  ScriptRun scriptRun1001 =
  {
    {
      20u,
      12u,
    },
    TextAbstraction::HEBREW
  };
  ScriptRun scriptRun1002 =
  {
    {
      32u,
      17u,
    },
    TextAbstraction::ARABIC
  };
  ScriptRun scriptRun1003 =
  {
    {
      49u,
      18u,
    },
    TextAbstraction::LATIN
  };
  ScriptRun scriptRun1004 =
  {
    {
      67u,
      14u,
    },
    TextAbstraction::HANGUL
  };
  ScriptRun scriptRun1005 =
  {
    {
      81u,
      19u,
    },
    TextAbstraction::ARABIC
  };
  ScriptRun scriptRun1006 =
  {
    {
      100u,
      13u,
    },
    TextAbstraction::LATIN
  };
  ScriptRun scriptRun1007 =
  {
    {
      113u,
      16u,
    },
    TextAbstraction::HEBREW
  };
  ScriptRun scriptRun1008 =
  {
    {
      129u,
      20u,
    },
    TextAbstraction::LATIN
  };
  ScriptRun scriptRun1009 =
  {
    {
      149u,
      14u,
    },
    TextAbstraction::ARABIC
  };
  ScriptRun scriptRun1010 =
  {
    {
      163u,
      35u,
    },
    TextAbstraction::HANGUL
  };
  scriptRuns10.PushBack( scriptRun1000 );
  scriptRuns10.PushBack( scriptRun1001 );
  scriptRuns10.PushBack( scriptRun1002 );
  scriptRuns10.PushBack( scriptRun1003 );
  scriptRuns10.PushBack( scriptRun1004 );
  scriptRuns10.PushBack( scriptRun1005 );
  scriptRuns10.PushBack( scriptRun1006 );
  scriptRuns10.PushBack( scriptRun1007 );
  scriptRuns10.PushBack( scriptRun1008 );
  scriptRuns10.PushBack( scriptRun1009 );
  scriptRuns10.PushBack( scriptRun1010 );

  // Paragraphs with no scripts mixed with paragraphs with scripts.
  Vector<ScriptRun> scriptRuns11;
  ScriptRun scriptRun1100 =
  {
    {
      0u,
      31u,
    },
    TextAbstraction::LATIN
  };
  ScriptRun scriptRun1101 =
  {
    {
      31u,
      21u,
    },
    TextAbstraction::HEBREW
  };
  scriptRuns11.PushBack( scriptRun1100 );
  scriptRuns11.PushBack( scriptRun1101 );

  // Paragraphs with no scripts.
  Vector<ScriptRun> scriptRuns12;
  ScriptRun scriptRun1200 =
  {
    {
      0u,
      11u,
    },
    TextAbstraction::LATIN
  };
  scriptRuns12.PushBack( scriptRun1200 );

  const ScriptsData data[] =
  {
    {
      "void text",
      "",
      scriptRuns00,
    },
    {
      "Easy latin script",
      "Hello world",
      scriptRuns01,
    },
    {
      "Mix of LTR '\\n'and RTL",
      "Hello world\nمرحبا بالعالم",
      scriptRuns02,
    },
    {
      "Mix of RTL '\\n'and LTR",
      "مرحبا بالعالم\nHello world",
      scriptRuns03,
    },
    {
      "White spaces. At the beginning of the text.",
      "    Hello world.",
      scriptRuns04,
    },
    {
      "White spaces. At the end of the text.",
      "Hello world.    ",
      scriptRuns05,
    },
    {
      "White spaces. At the middle of the text.",
      "Hello     world.",
      scriptRuns06,
    },
    {
      "White spaces between different scripts.",
      "  Hel   세계   ",
      scriptRuns07,
    },
    {
      "White spaces between different scripts and differetn directions. Starting LTR.",
      "  Hello   world   مرحبا  بالعالم     안녕하세요   세계   ",
      scriptRuns08,
    },
    {
      "White spaces between different scripts and differetn directions. Starting RTL.",
      "   مرحبا  بالعالم    Hello   world   안녕하세요   세계   مرحبا  بالعالم   ",
      scriptRuns09
    },
    {
      "Paragraphs with different directions.",
      "   مرحبا  بالعالم   שלום עולם   مرحبا  بالعالم  \n "
      " Hello   world   안녕하세요   세계   \n "
      "  مرحبا  بالعالم  Hello   world    שלום עולם  \n  "
      " Hello   world    مرحبا  بالعالم    안녕하세요   세계   \n "
      "   안녕하세요   세계   ",
      scriptRuns10
    },
    {
      "Paragraphs with no scripts mixed with paragraphs with scripts.",
      "  \n  \n   Hello   world  \n  \n  \n   שלום עולם  \n \n \n  ",
      scriptRuns11
    },
    {
      "Paragraphs with no scripts.",
      "  \n  \n  \n  ",
      scriptRuns12
    }
  };
  const unsigned int numberOfTests = 13u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    if( !ScriptsTest( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextMultiLanguageValidateFonts01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextMultiLanguageValidateFonts");

  const ValidateFontsData data[] =
  {
    {
      "void text",
      "",
    },
    {
      "Easy latin script",
      "Hello world",
    },
  };
  const unsigned int numberOfTests = 2u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    if( !ValidateFontTest( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

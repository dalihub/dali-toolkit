/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <unistd.h>

#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali-toolkit/internal/text/character-set-conversion.h>
#include <dali-toolkit/internal/text/logical-model-impl.h>
#include <dali-toolkit/internal/text/multi-language-helper-functions.h>
#include <dali-toolkit/internal/text/multi-language-support.h>
#include <dali-toolkit/internal/text/segmentation.h>
#include <dali-toolkit/internal/text/text-run-container.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Toolkit;
using namespace Text;

// Tests the following functions with different scripts.
//
// void MergeFontDescriptions( const Vector<FontDescriptionRun>& fontDescriptions,
//                             const TextAbstraction::FontDescription& defaultFontDescription,
//                             TextAbstraction::PointSize26Dot6 defaultPointSize,
//                             CharacterIndex characterIndex,
//                             TextAbstraction::FontDescription& fontDescription,
//                             TextAbstraction::PointSize26Dot6& fontPointSize,
//                             bool& isDefaultFont );
//
// Script GetScript( Length index,
//                   Vector<ScriptRun>::ConstIterator& scriptRunIt,
//                   const Vector<ScriptRun>::ConstIterator& scriptRunEndIt );
//
// Constructor, destructor and MultilanguageSupport::Get()
//
// void MultilanguageSupport::SetScripts( const Vector<Character>& text,
//                                        CharacterIndex startIndex,
//                                        Length numberOfCharacters,
//                                        Vector<ScriptRun>& scripts );
//
// void MultilanguageSupport::ValidateFonts( const Vector<Character>& text,
//                                           const Vector<ScriptRun>& scripts,
//                                           const Vector<FontDescriptionRun>& fontDescriptions,
//                                           const TextAbstraction::FontDescription& defaultFontDescription,
//                                           TextAbstraction::PointSize26Dot6 defaultFontPointSize,
//                                           CharacterIndex startIndex,
//                                           Length numberOfCharacters,
//                                           Vector<FontRun>& fonts );

//////////////////////////////////////////////////////////

namespace
{

const std::string DEFAULT_FONT_DIR( "/resources/fonts" );
const unsigned int EMOJI_FONT_SIZE = 3840u; // 60 * 64
const unsigned int NON_DEFAULT_FONT_SIZE = 40u;

struct MergeFontDescriptionsData
{
  std::string description;                                 ///< Description of the experiment.
  Vector<FontDescriptionRun> fontDescriptionRuns;          ///< The font description runs.
  TextAbstraction::FontDescription defaultFontDescription; ///< The default font description.
  TextAbstraction::PointSize26Dot6 defaultPointSize;       ///< The default point size.
  unsigned int startIndex;                                 ///< The start index.
  unsigned int numberOfCharacters;                         ///< The number of characters.
  Vector<FontId> expectedFontIds;                          ///< The expected font ids.
  Vector<bool> expectedIsDefault;                          ///< The expected font ids.
};

struct ScriptsData
{
  std::string description;         ///< Description of the experiment.
  std::string text;                ///< Input text.
  unsigned int index;              ///< The index of the first character to update the script.
  unsigned int numberOfCharacters; ///< The numbers of characters to update the script.
  Vector<ScriptRun> scriptRuns;    ///< Expected script runs.
};

struct ValidateFontsData
{
  std::string                description;         ///< Description of the experiment.
  std::string                text;                ///< Input text.
  std::string                defaultFont;         ///< The default font.
  unsigned int               defaultFontSize;     ///< The default font size.
  unsigned int               index;               ///< The index of the first character to update the script.
  unsigned int               numberOfCharacters;  ///< The numbers of characters to update the script.
  Vector<FontDescriptionRun> fontDescriptionRuns; ///< The font description runs.
  Vector<FontRun>            fontRuns;            ///< The expected font runs.
};

//////////////////////////////////////////////////////////
bool MergeFontDescriptionsTest( const MergeFontDescriptionsData& data )
{
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();

  Vector<FontId> fontIds;
  fontIds.Resize( data.startIndex + data.numberOfCharacters, 0u );
  Vector<bool> isDefaultFont;
  isDefaultFont.Resize( data.startIndex + data.numberOfCharacters, true );

  for( unsigned int index = data.startIndex; index < data.startIndex + data.numberOfCharacters; ++index )
  {
    TextAbstraction::FontDescription fontDescription;
    TextAbstraction::PointSize26Dot6 fontPointSize = TextAbstraction::FontClient::DEFAULT_POINT_SIZE;

    MergeFontDescriptions( data.fontDescriptionRuns,
                           data.defaultFontDescription,
                           data.defaultPointSize,
                           index,
                           fontDescription,
                           fontPointSize,
                           isDefaultFont[index] );

    if( !isDefaultFont[index] )
    {
      fontIds[index] = fontClient.GetFontId( fontDescription, fontPointSize );
    }
  }

  if( fontIds.Count() != data.expectedFontIds.Count() )
  {
    std::cout << data.description << " Different number of font ids : " << fontIds.Count() << ", expected : " << data.expectedFontIds.Count() << std::endl;
    return false;
  }

  for( unsigned int index = 0u; index < fontIds.Count(); ++index )
  {
    if( fontIds[index] != data.expectedFontIds[index] )
    {
      std::cout << data.description << " Different font id at index : " << index << ", font id : " << fontIds[index] << ", expected : " << data.expectedFontIds[index] << std::endl;
      std::cout << "           font ids : ";
      for( unsigned int i=0;i<fontIds.Count();++i)
      {
        std::cout << fontIds[i] << " ";
      }
      std::cout << std::endl;
      std::cout << "  expected font ids : ";
      for( unsigned int i=0;i<data.expectedFontIds.Count();++i)
      {
        std::cout << data.expectedFontIds[i] << " ";
      }
      std::cout << std::endl;
      return false;
    }

    if( isDefaultFont[index] != data.expectedIsDefault[index] )
    {
      std::cout << data.description << " Different 'is font default' at index : " << index << ", is font default : " << isDefaultFont[index] << ", expected : " << data.expectedIsDefault[index] << std::endl;
      return false;
    }
  }

  return true;
}

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
                                   0u,
                                   numberOfCharacters,
                                   scripts );

  if( ( 0u != data.index ) ||
      ( numberOfCharacters != data.numberOfCharacters ) )
  {
    // 3) Clear the scripts.
    ClearCharacterRuns( data.index,
                        data.index + data.numberOfCharacters - 1u,
                        scripts );

    multilanguageSupport.SetScripts( utf32,
                                     data.index,
                                     data.numberOfCharacters,
                                     scripts );
  }

  // 4) Compare the results.

  tet_printf( "Testing %s\n", data.description.c_str() );
  if( scripts.Count() != data.scriptRuns.Count() )
  {
    tet_printf("ScriptsTest FAIL: different number of scripts. %d, should be %d\n", scripts.Count(), data.scriptRuns.Count() );
    for( Vector<ScriptRun>::ConstIterator it = scripts.Begin(); it != scripts.End(); ++it)
    {
      const ScriptRun& run = *it;

      std::cout << "  index : " << run.characterRun.characterIndex << ", num chars : " << run.characterRun.numberOfCharacters << ", script : [" << TextAbstraction::ScriptName[run.script] << "]" << std::endl;
    }
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
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();

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
                                   0u,
                                   numberOfCharacters,
                                   scripts );

  char* pathNamePtr = get_current_dir_name();
  const std::string pathName( pathNamePtr );
  free( pathNamePtr );

  // Get the default font id.
  const FontId defaultFontId = fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + data.defaultFont,
                                                     data.defaultFontSize );
  TextAbstraction::FontDescription defaultFontDescription;
  fontClient.GetDescription( defaultFontId, defaultFontDescription );

  const TextAbstraction::PointSize26Dot6 defaultPointSize = fontClient.GetPointSize( defaultFontId );

  Vector<FontRun> fontRuns;

  // 3) Validate the fonts.
  multilanguageSupport.ValidateFonts( utf32,
                                      scripts,
                                      data.fontDescriptionRuns,
                                      defaultFontDescription,
                                      defaultPointSize,
                                      0u,
                                      numberOfCharacters,
                                      fontRuns );

  if( ( 0u != data.index ) ||
      ( numberOfCharacters != data.numberOfCharacters ) )
  {
    // 4) Clear the fonts.
    ClearCharacterRuns( data.index,
                        data.index + data.numberOfCharacters - 1u,
                        fontRuns );

    multilanguageSupport.ValidateFonts( utf32,
                                        scripts,
                                        data.fontDescriptionRuns,
                                        defaultFontDescription,
                                        defaultPointSize,
                                        data.index,
                                        data.numberOfCharacters,
                                        fontRuns );
  }

  // 5) Compare the results.
  if( data.fontRuns.Count() != fontRuns.Count() )
  {
    std::cout << "  Different number of font runs : " << fontRuns.Count() << ", expected : " << data.fontRuns.Count() << std::endl;
    return false;
  }


  for( unsigned int index = 0; index < data.fontRuns.Count(); ++index )
  {
    const FontRun& run = fontRuns[index];
    const FontRun& expectedRun = data.fontRuns[index];

    if( run.characterRun.characterIndex != expectedRun.characterRun.characterIndex )
    {
      std::cout << "  character run : " << index << ", index : " << run.characterRun.characterIndex << ", expected : " << expectedRun.characterRun.characterIndex << std::endl;
      return false;
    }
    if( run.characterRun.numberOfCharacters != expectedRun.characterRun.numberOfCharacters )
    {
      std::cout << "  character run : " << index << ", num chars : " << run.characterRun.numberOfCharacters << ", expected : " << expectedRun.characterRun.numberOfCharacters << std::endl;
      return false;
    }
    if( run.fontId != expectedRun.fontId )
    {
      std::cout << "  character run : " << index << ", font : " << run.fontId << ", expected : " << expectedRun.fontId << std::endl;
      return false;
    }
  }

  return true;
}

} // namespace

int UtcDaliTextGetScript(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextGetScript");

  Script script = TextAbstraction::LATIN;

  // Text with no scripts.
  Vector<ScriptRun> scriptRuns;
  Vector<ScriptRun>::ConstIterator scriptRunIt = scriptRuns.Begin();
  script = GetScript( 0u,
                      scriptRunIt,
                      scriptRuns.End() );

  DALI_TEST_CHECK( TextAbstraction::UNKNOWN == script );

  const unsigned int numberOfCharacters = 7u;
  // Add scripts.
  ScriptRun scriptRun01 =
  {
    {
      0u,
      2u,
    },
    TextAbstraction::LATIN
  };
  ScriptRun scriptRun02 =
  {
    {
      2u,
      2u,
    },
    TextAbstraction::HEBREW
  };
  ScriptRun scriptRun03 =
  {
    {
      4u,
      2u,
    },
    TextAbstraction::ARABIC
  };
  scriptRuns.PushBack( scriptRun01 );
  scriptRuns.PushBack( scriptRun02 );
  scriptRuns.PushBack( scriptRun03 );

  // Expected results
  TextAbstraction::Script expectedScripts[]=
  {
    TextAbstraction::LATIN,
    TextAbstraction::LATIN,
    TextAbstraction::HEBREW,
    TextAbstraction::HEBREW,
    TextAbstraction::ARABIC,
    TextAbstraction::ARABIC,
    TextAbstraction::UNKNOWN
  };

  scriptRunIt = scriptRuns.Begin();
  for( unsigned int index = 0u; index < numberOfCharacters; ++index )
  {
    script = GetScript( index,
                        scriptRunIt,
                        scriptRuns.End() );

    DALI_TEST_CHECK( expectedScripts[index] == script );
  }
  DALI_TEST_CHECK( scriptRunIt == scriptRuns.End() );

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextMergeFontDescriptions(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextMergeFontDescriptions");

  // Load some fonts.

  char* pathNamePtr = get_current_dir_name();
  const std::string pathName( pathNamePtr );
  free( pathNamePtr );

  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/dejavu/DejaVuSans.ttf" );
  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/dejavu/DejaVuSerif.ttf" );
  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/dejavu/DejaVuSerif.ttf", NON_DEFAULT_FONT_SIZE );
  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/dejavu/DejaVuSerif-Bold.ttf" );
  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/dejavu/DejaVuSerif-Italic.ttf" );

  // To test the font width as GetFontId() with the font file path can't cache the width property.
  TextAbstraction::FontDescription widthDescription;
  widthDescription.path = "";
  widthDescription.family = "DejaVu Serif";
  widthDescription.weight = TextAbstraction::FontWeight::NORMAL;
  widthDescription.width = TextAbstraction::FontWidth::EXPANDED;
  widthDescription.slant = TextAbstraction::FontSlant::NORMAL;
  fontClient.GetFontId( widthDescription );

  // Test.

  TextAbstraction::FontDescription defaultFontDescription01;
  Vector<FontDescriptionRun> fontDescriptionRuns01;
  Vector<FontId> expectedFontIds01;
  Vector<bool> expectedIsFontDefault01;

  TextAbstraction::FontDescription defaultFontDescription02;
  Vector<FontDescriptionRun> fontDescriptionRuns02;
  Vector<FontId> expectedFontIds02;
  expectedFontIds02.PushBack( 0u );
  expectedFontIds02.PushBack( 0u );
  Vector<bool> expectedIsFontDefault02;
  expectedIsFontDefault02.PushBack( true );
  expectedIsFontDefault02.PushBack( true );

  TextAbstraction::FontDescription defaultFontDescription03;
  defaultFontDescription03.family = "DejaVu Serif";
  Vector<FontDescriptionRun> fontDescriptionRuns03;

  FontDescriptionRun fontDescription0301 =
  {
    {
      0u,
      2u
    },
    const_cast<char*>( "DejaVu Sans" ),
    11u,
    TextAbstraction::FontWeight::NONE,
    TextAbstraction::FontWidth::NONE,
    TextAbstraction::FontSlant::NONE,
    TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
    true,
    false,
    false,
    false,
    false
  };
  FontDescriptionRun fontDescription0302 =
  {
    {
      2u,
      2u
    },
    NULL,
    0u,
    TextAbstraction::FontWeight::NONE,
    TextAbstraction::FontWidth::NONE,
    TextAbstraction::FontSlant::ITALIC,
    TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
    false,
    false,
    false,
    true,
    false
  };
  FontDescriptionRun fontDescription0303 =
  {
    {
      4u,
      2u
    },
    NULL,
    0u,
    TextAbstraction::FontWeight::BOLD,
    TextAbstraction::FontWidth::NONE,
    TextAbstraction::FontSlant::NONE,
    TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
    false,
    true,
    false,
    false,
    false
  };
  FontDescriptionRun fontDescription0304 =
  {
    {
      6u,
      2u
    },
    NULL,
    0u,
    TextAbstraction::FontWeight::NONE,
    TextAbstraction::FontWidth::NONE,
    TextAbstraction::FontSlant::NONE,
    NON_DEFAULT_FONT_SIZE,
    false,
    false,
    false,
    false,
    true
  };
  FontDescriptionRun fontDescription0305 =
  {
    {
      8u,
      2u
    },
    NULL,
    0u,
    TextAbstraction::FontWeight::NONE,
    TextAbstraction::FontWidth::EXPANDED,
    TextAbstraction::FontSlant::NONE,
    TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
    false,
    false,
    true,
    false,
    false
  };

  fontDescriptionRuns03.PushBack( fontDescription0301 );
  fontDescriptionRuns03.PushBack( fontDescription0302 );
  fontDescriptionRuns03.PushBack( fontDescription0303 );
  fontDescriptionRuns03.PushBack( fontDescription0304 );
  fontDescriptionRuns03.PushBack( fontDescription0305 );

  Vector<FontId> expectedFontIds03;
  expectedFontIds03.PushBack( 1u );
  expectedFontIds03.PushBack( 1u );
  expectedFontIds03.PushBack( 5u );
  expectedFontIds03.PushBack( 5u );
  expectedFontIds03.PushBack( 4u );
  expectedFontIds03.PushBack( 4u );
  expectedFontIds03.PushBack( 3u );
  expectedFontIds03.PushBack( 3u );
  expectedFontIds03.PushBack( 6u );
  expectedFontIds03.PushBack( 6u );
  Vector<bool> expectedIsFontDefault03;
  expectedIsFontDefault03.PushBack( false );
  expectedIsFontDefault03.PushBack( false );
  expectedIsFontDefault03.PushBack( false );
  expectedIsFontDefault03.PushBack( false );
  expectedIsFontDefault03.PushBack( false );
  expectedIsFontDefault03.PushBack( false );
  expectedIsFontDefault03.PushBack( false );
  expectedIsFontDefault03.PushBack( false );
  expectedIsFontDefault03.PushBack( false );
  expectedIsFontDefault03.PushBack( false );

  const MergeFontDescriptionsData data[] =
  {
    {
      "void text.",
      fontDescriptionRuns01,
      defaultFontDescription01,
      TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
      0u,
      0u,
      expectedFontIds01,
      expectedIsFontDefault01
    },
    {
      "No description runs.",
      fontDescriptionRuns02,
      defaultFontDescription02,
      TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
      0u,
      2u,
      expectedFontIds02,
      expectedIsFontDefault02
    },
    {
      "Some description runs.",
      fontDescriptionRuns03,
      defaultFontDescription03,
      TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
      0u,
      10u,
      expectedFontIds03,
      expectedIsFontDefault03
    }
  };
  const unsigned int numberOfTests = 3u;

  for( unsigned int index = 0u; index < numberOfTests; ++index )
  {
    if( !MergeFontDescriptionsTest( data[index] ) )
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
  END_TEST;
}

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
      15u,
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
      15u,
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
      15u,
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
      18u,
    },
    TextAbstraction::HANGUL
  };
  ScriptRun scriptRun1011 =
  {
    {
      181u,
      17u,
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
  scriptRuns10.PushBack( scriptRun1011 );

  // Paragraphs with no scripts mixed with paragraphs with scripts.
  Vector<ScriptRun> scriptRuns11;
  ScriptRun scriptRun1100 =
  {
    {
      0u,
      3u,
    },
    TextAbstraction::LATIN
  };
  ScriptRun scriptRun1101 =
  {
    {
      3u,
      3u,
    },
    TextAbstraction::LATIN
  };
  ScriptRun scriptRun1102 =
  {
    {
      6u,
      19u,
    },
    TextAbstraction::LATIN
  };
  ScriptRun scriptRun1103 =
  {
    {
      25u,
      3u,
    },
    TextAbstraction::LATIN
  };
  ScriptRun scriptRun1104 =
  {
    {
      28u,
      3u,
    },
    TextAbstraction::LATIN
  };
  ScriptRun scriptRun1105 =
  {
    {
      31u,
      15u,
    },
    TextAbstraction::HEBREW
  };
  ScriptRun scriptRun1106 =
  {
    {
      46u,
      2u,
    },
    TextAbstraction::LATIN
  };
  ScriptRun scriptRun1107 =
  {
    {
      48u,
      2u,
    },
    TextAbstraction::LATIN
  };
  ScriptRun scriptRun1108 =
  {
    {
      50u,
      2u,
    },
    TextAbstraction::LATIN
  };
  scriptRuns11.PushBack( scriptRun1100 );
  scriptRuns11.PushBack( scriptRun1101 );
  scriptRuns11.PushBack( scriptRun1102 );
  scriptRuns11.PushBack( scriptRun1103 );
  scriptRuns11.PushBack( scriptRun1104 );
  scriptRuns11.PushBack( scriptRun1105 );
  scriptRuns11.PushBack( scriptRun1106 );
  scriptRuns11.PushBack( scriptRun1107 );
  scriptRuns11.PushBack( scriptRun1108 );

  // Paragraphs with no scripts.
  Vector<ScriptRun> scriptRuns12;
  ScriptRun scriptRun1200 =
  {
    {
      0u,
      3u,
    },
    TextAbstraction::LATIN
  };
  ScriptRun scriptRun1201 =
  {
    {
      3u,
      3u,
    },
    TextAbstraction::LATIN
  };
  ScriptRun scriptRun1202 =
  {
    {
      6u,
      3u,
    },
    TextAbstraction::LATIN
  };
  ScriptRun scriptRun1203 =
  {
    {
      9u,
      2u,
    },
    TextAbstraction::LATIN
  };
  scriptRuns12.PushBack( scriptRun1200 );
  scriptRuns12.PushBack( scriptRun1201 );
  scriptRuns12.PushBack( scriptRun1202 );
  scriptRuns12.PushBack( scriptRun1203 );

  Vector<ScriptRun> scriptRuns13;
  ScriptRun scriptRun1301 =
  {
    {
      0u,
      4u,
    },
    TextAbstraction::LATIN // An unknown script is transformed to LATIN
  };
  scriptRuns13.PushBack( scriptRun1301 );

  const ScriptsData data[] =
  {
    {
      "void text",
      "",
      0u,
      0u,
      scriptRuns00,
    },
    {
      "Easy latin script",
      "Hello world",
      0u,
      11u,
      scriptRuns01,
    },
    {
      "Mix of LTR '\\n'and RTL",
      "Hello world\nمرحبا بالعالم",
      0u,
      25u,
      scriptRuns02,
    },
    {
      "Update mix of LTR '\\n'and RTL. Update LTR",
      "Hello world\nمرحبا بالعالم",
      0u,
      12u,
      scriptRuns02,
    },
    {
      "Update mix of LTR '\\n'and RTL. Update RTL",
      "Hello world\nمرحبا بالعالم",
      12u,
      13u,
      scriptRuns02,
    },
    {
      "Mix of RTL '\\n'and LTR",
      "مرحبا بالعالم\nHello world",
      0u,
      25u,
      scriptRuns03,
    },
    {
      "Update mix of RTL '\\n'and LTR. Update RTL",
      "مرحبا بالعالم\nHello world",
      0u,
      14u,
      scriptRuns03,
    },
    {
      "Update mix of RTL '\\n'and LTR. Update LTR",
      "مرحبا بالعالم\nHello world",
      14u,
      11u,
      scriptRuns03,
    },
    {
      "White spaces. At the beginning of the text.",
      "    Hello world",
      0u,
      15u,
      scriptRuns04,
    },
    {
      "White spaces. At the end of the text.",
      "Hello world    ",
      0u,
      15u,
      scriptRuns05,
    },
    {
      "White spaces. At the middle of the text.",
      "Hello     world",
      0u,
      15u,
      scriptRuns06,
    },
    {
      "White spaces between different scripts.",
      "  Hel   세계   ",
      0u,
      13u,
      scriptRuns07,
    },
    {
      "White spaces between different scripts and differetn directions. Starting LTR.",
      "  Hello   world   مرحبا  بالعالم     안녕하세요   세계   ",
      0u,
      50u,
      scriptRuns08,
    },
    {
      "White spaces between different scripts and differetn directions. Starting RTL.",
      "   مرحبا  بالعالم    Hello   world   안녕하세요   세계   مرحبا  بالعالم   ",
      0u,
      67u,
      scriptRuns09
    },
    {
      "Paragraphs with different directions.",
      "   مرحبا  بالعالم   שלום עולם   مرحبا  بالعالم  \n "
      " Hello   world   안녕하세요   세계   \n "
      "  مرحبا  بالعالم  Hello   world    שלום עולם  \n  "
      " Hello   world    مرحبا  بالعالم    안녕하세요   세계   \n "
      "   안녕하세요   세계   ",
      0u,
      198u,
      scriptRuns10
    },
    {
      "Update paragraphs with different directions. Update initial paragraphs.",
      "   مرحبا  بالعالم   שלום עולם   مرحبا  بالعالم  \n "
      " Hello   world   안녕하세요   세계   \n "
      "  مرحبا  بالعالم  Hello   world    שלום עולם  \n  "
      " Hello   world    مرحبا  بالعالم    안녕하세요   세계   \n "
      "   안녕하세요   세계   ",
      0u,
      81u,
      scriptRuns10
    },
    {
      "Update paragraphs with different directions. Update middle paragraphs.",
      "   مرحبا  بالعالم   שלום עולם   مرحبا  بالعالم  \n "
      " Hello   world   안녕하세요   세계   \n "
      "  مرحبا  بالعالم  Hello   world    שלום עולם  \n  "
      " Hello   world    مرحبا  بالعالم    안녕하세요   세계   \n "
      "   안녕하세요   세계   ",
      49u,
      80u,
      scriptRuns10
    },
    {
      "Update paragraphs with different directions. Update final paragraphs.",
      "   مرحبا  بالعالم   שלום עולם   مرحبا  بالعالم  \n "
      " Hello   world   안녕하세요   세계   \n "
      "  مرحبا  بالعالم  Hello   world    שלום עולם  \n  "
      " Hello   world    مرحبا  بالعالم    안녕하세요   세계   \n "
      "   안녕하세요   세계   ",
      129u,
      69u,
      scriptRuns10
    },
    {
      "Paragraphs with no scripts mixed with paragraphs with scripts.",
      "  \n  \n   Hello   world  \n  \n  \n   שלום עולם  \n \n \n  ",
      0u,
      52u,
      scriptRuns11
    },
    {
      "Paragraphs with no scripts.",
      "  \n  \n  \n  ",
      0u,
      11u,
      scriptRuns12
    },
    {
      "Update paragraphs with no scripts. Update initial paragraphs.",
      "  \n  \n  \n  ",
      0u,
      3u,
      scriptRuns12
    },
    {
      "Update paragraphs with no scripts. Update middle paragraphs.",
      "  \n  \n  \n  ",
      3u,
      6u,
      scriptRuns12
    },
    {
      "Update paragraphs with no scripts. Update final paragraphs.",
      "  \n  \n  \n  ",
      9u,
      2u,
      scriptRuns12
    },
    {
      "Unknown scripts.",
      "ᚩᚯᚱᚸ", // Runic script not currentlu supported.
      0u,
      4u,
      scriptRuns13
    }
  };
  const unsigned int numberOfTests = 24u;

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

  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();

  char* pathNamePtr = get_current_dir_name();
  const std::string pathName( pathNamePtr );
  free( pathNamePtr );

  const PointSize26Dot6 pointSize01 = static_cast<PointSize26Dot6>( 21.f * 64.f );
  const PointSize26Dot6 pointSize02 = static_cast<PointSize26Dot6>( 35.f * 64.f );

  // Load some fonts.
  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansArabicRegular.ttf" );
  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansHebrewRegular.ttf" );
  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/BreezeColorEmoji.ttf", EMOJI_FONT_SIZE );
  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansRegular.ttf", pointSize01 );
  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansRegular.ttf", pointSize02 );
  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansHebrewRegular.ttf", pointSize01 );
  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansHebrewRegular.ttf", pointSize02 );

  // Font id 1 --> TizenSansArabicRegular.ttf
  // Font id 2 --> TizenSansHebrewRegular.ttf
  // Font id 3 --> BreezeColorEmoji.ttf
  // Font id 4 --> TizenSansRegular.ttf, size 8
  // Font id 5 --> TizenSansRegular.ttf, size 16
  // Font id 6 --> TizenSansHebrewRegular.ttf, size 8
  // Font id 7 --> TizenSansHebrewRegular.ttf, size 16
  // Font id 8 --> (default)

  Vector<FontRun> fontRuns01;
  Vector<FontDescriptionRun> fontDescriptions01;

  FontRun fontRun0201 =
  {
    {
      0u,
      11u
    },
    8u
  };
  Vector<FontRun> fontRuns02;
  fontRuns02.PushBack( fontRun0201 );

  FontDescriptionRun fontDescription0201 =
  {
    {
      0u,
      11u
    },
    const_cast<char*>( "TizenSans" ),
    9u,
    TextAbstraction::FontWeight::NORMAL,
    TextAbstraction::FontWidth::NORMAL,
    TextAbstraction::FontSlant::NORMAL,
    0u,
    true,
    false,
    false,
    false,
    false
  };
  Vector<FontDescriptionRun> fontDescriptions02;
  fontDescriptions02.PushBack( fontDescription0201 );

  FontRun fontRun0301 =
  {
    {
      0u,
      12u
    },
    8u
  };
  FontRun fontRun0302 =
  {
    {
      12u,
      12u
    },
    8u
  };
  FontRun fontRun0303 =
  {
    {
      24u,
      4u
    },
    8u
  };
  Vector<FontRun> fontRuns03;
  fontRuns03.PushBack( fontRun0301 );
  fontRuns03.PushBack( fontRun0302 );
  fontRuns03.PushBack( fontRun0303 );

  Vector<FontDescriptionRun> fontDescriptions03;

  FontRun fontRun0701 =
  {
    {
      0u,
      4u
    },
    2u
  };
  FontRun fontRun0702 =
  {
    {
      4u,
      1u
    },
    8u
  };
  FontRun fontRun0703 =
  {
    {
      5u,
      4u
    },
    2u
  };
  Vector<FontRun> fontRuns07;
  fontRuns07.PushBack( fontRun0701 );
  fontRuns07.PushBack( fontRun0702 );
  fontRuns07.PushBack( fontRun0703 );

  FontDescriptionRun fontDescription0701 =
  {
    {
      0u,
      4u
    },
    const_cast<char*>( "TizenSansHebrew" ),
    15u,
    TextAbstraction::FontWeight::NORMAL,
    TextAbstraction::FontWidth::NORMAL,
    TextAbstraction::FontSlant::NORMAL,
    0u,
    true,
    false,
    false,
    false,
    false
  };
  FontDescriptionRun fontDescription0702 =
  {
    {
      5u,
      4u
    },
    const_cast<char*>( "TizenSansHebrew" ),
    15u,
    TextAbstraction::FontWeight::NORMAL,
    TextAbstraction::FontWidth::NORMAL,
    TextAbstraction::FontSlant::NORMAL,
    0u,
    true,
    false,
    false,
    false,
    false
  };
  Vector<FontDescriptionRun> fontDescriptions07;
  fontDescriptions07.PushBack( fontDescription0701 );
  fontDescriptions07.PushBack( fontDescription0702 );

  FontRun fontRun0801 =
  {
    {
      0u,
      9u
    },
    2u
  };
  Vector<FontRun> fontRuns08;
  fontRuns08.PushBack( fontRun0801 );

  Vector<FontDescriptionRun> fontDescriptions08;

  FontRun fontRun0901 =
  {
    {
      0u,
      4u
    },
    3u
  };
  Vector<FontRun> fontRuns09;
  fontRuns09.PushBack( fontRun0901 );

  Vector<FontDescriptionRun> fontDescriptions09;
  FontDescriptionRun fontDescription0901 =
  {
    {
      0u,
      4u
    },
    const_cast<char*>( "BreezeColorEmoji" ),
    16u,
    TextAbstraction::FontWeight::NORMAL,
    TextAbstraction::FontWidth::NORMAL,
    TextAbstraction::FontSlant::NORMAL,
    EMOJI_FONT_SIZE,
    true,
    false,
    false,
    false,
    true
  };
  fontDescriptions09.PushBack( fontDescription0901 );

  FontRun fontRun1001 =
  {
    {
      0u,
      13u
    },
    4u
  };
  FontRun fontRun1002 =
  {
    {
      13u,
      9u
    },
    6u
  };
  FontRun fontRun1003 =
  {
    {
      22u,
      15u
    },
    5u
  };
  FontRun fontRun1004 =
  {
    {
      37u,
      9u
    },
    7u
  };
  Vector<FontRun> fontRuns10;
  fontRuns10.PushBack( fontRun1001 );
  fontRuns10.PushBack( fontRun1002 );
  fontRuns10.PushBack( fontRun1003 );
  fontRuns10.PushBack( fontRun1004 );

  FontDescriptionRun fontDescription1001 =
  {
    {
      0u,
      13u
    },
    const_cast<char*>( "TizenSans" ),
    9u,
    TextAbstraction::FontWeight::NORMAL,
    TextAbstraction::FontWidth::NORMAL,
    TextAbstraction::FontSlant::NORMAL,
    pointSize01,
    true,
    false,
    false,
    false,
    true
  };
  FontDescriptionRun fontDescription1002 =
  {
    {
      13u,
      9u
    },
    const_cast<char*>( "TizenSansHebrew" ),
    15u,
    TextAbstraction::FontWeight::NORMAL,
    TextAbstraction::FontWidth::NORMAL,
    TextAbstraction::FontSlant::NORMAL,
    pointSize01,
    true,
    false,
    false,
    false,
    true
  };
  FontDescriptionRun fontDescription1003 =
  {
    {
      22u,
      15u
    },
    const_cast<char*>( "TizenSans" ),
    9u,
    TextAbstraction::FontWeight::NORMAL,
    TextAbstraction::FontWidth::NORMAL,
    TextAbstraction::FontSlant::NORMAL,
    pointSize02,
    true,
    false,
    false,
    false,
    true
  };
  FontDescriptionRun fontDescription1004 =
  {
    {
      37u,
      9u
    },
    const_cast<char*>( "TizenSansHebrew" ),
    15u,
    TextAbstraction::FontWeight::NORMAL,
    TextAbstraction::FontWidth::NORMAL,
    TextAbstraction::FontSlant::NORMAL,
    pointSize02,
    true,
    false,
    false,
    false,
    true
  };
  Vector<FontDescriptionRun> fontDescriptions10;
  fontDescriptions10.PushBack( fontDescription1001 );
  fontDescriptions10.PushBack( fontDescription1002 );
  fontDescriptions10.PushBack( fontDescription1003 );
  fontDescriptions10.PushBack( fontDescription1004 );

  const ValidateFontsData data[] =
  {
    {
      "void text.",
      "",
      "/tizen/TizenSansRegular.ttf",
      TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
      0u,
      0u,
      fontDescriptions01,
      fontRuns01
    },
    {
      "Easy latin script.",
      "Hello world",
      "/tizen/TizenSansRegular.ttf",
      TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
      0u,
      11u,
      fontDescriptions02,
      fontRuns02
    },
    {
      "Different paragraphs.",
      "Hello world\nhello world\ndemo",
      "/tizen/TizenSansRegular.ttf",
      TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
      0u,
      28u,
      fontDescriptions03,
      fontRuns03
    },
    {
      "Different paragraphs. Update the initial paragraph.",
      "Hello world\nhello world\ndemo",
      "/tizen/TizenSansRegular.ttf",
      TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
      0u,
      12u,
      fontDescriptions03,
      fontRuns03
    },
    {
      "Different paragraphs. Update the middle paragraph.",
      "Hello world\nhello world\ndemo",
      "/tizen/TizenSansRegular.ttf",
      TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
      12u,
      12u,
      fontDescriptions03,
      fontRuns03
    },
    {
      "Different paragraphs. Update the final paragraph.",
      "Hello world\nhello world\ndemo",
      "/tizen/TizenSansRegular.ttf",
      TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
      24u,
      4u,
      fontDescriptions03,
      fontRuns03
    },
    {
      "Hebrew text. Default font: latin",
      "שלום עולם",
      "/tizen/TizenSansRegular.ttf",
      TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
      0u,
      9u,
      fontDescriptions07,
      fontRuns07
    },
    {
      "Hebrew text. Default font: hebrew",
      "שלום עולם",
      "/tizen/TizenSansHebrewRegular.ttf",
      TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
      0u,
      9u,
      fontDescriptions08,
      fontRuns08
    },
    {
      "Emojis",
      "\xF0\x9F\x98\x81\xF0\x9F\x98\x82\xF0\x9F\x98\x83\xF0\x9F\x98\x84",
      "/tizen/BreezeColorEmoji.ttf",
      EMOJI_FONT_SIZE,
      0u,
      4u,
      fontDescriptions09,
      fontRuns09
    },
    {
      "Mix text. Default font: latin. Different font sizes",
      "Hello world, שלום עולם, hello world, שלום עולם",
      "/tizen/TizenSansRegular.ttf",
      TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
      0u,
      46u,
      fontDescriptions10,
      fontRuns10
    },
  };
  const unsigned int numberOfTests = 10u;

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

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
// void MultilanguageSupport::ValidateFonts( const Vector<Character>& text, const Vector<ScriptRun>& scripts, Vector<FontRun>& fonts );

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

  // 2) Set the line break info.
  Vector<LineBreakInfo> lineBreakInfo;
  lineBreakInfo.Resize( numberOfCharacters );

  SetLineBreakInfo( utf32, lineBreakInfo );

  // 3) Set the script info.
  Vector<ScriptRun> scripts;
  multilanguageSupport.SetScripts( utf32,
                                   lineBreakInfo,
                                   scripts );

  // 4) Compare the results.

  if( scripts.Count() != data.scriptRuns.Count() )
  {
    tet_infoline("ScriptsTest: different number of scripts.");
    return false;
  }

  for( unsigned int index = 0u; index < scripts.Count(); ++index )
  {
    const ScriptRun& scriptRun1 = scripts[index];
    const ScriptRun& scriptRun2 = data.scriptRuns[index];

    if( scriptRun1.characterRun.characterIndex != scriptRun2.characterRun.characterIndex )
    {
      tet_infoline("ScriptsTest: different character index.");
      return false;
    }

    if( scriptRun1.characterRun.numberOfCharacters != scriptRun2.characterRun.numberOfCharacters )
    {
      tet_infoline("ScriptsTest: different number of characters.");
      return false;
    }

    if( scriptRun1.script != scriptRun2.script )
    {
      tet_infoline("ScriptsTest: different script.");
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

  // 2) Set the line break info.
  Vector<LineBreakInfo> lineBreakInfo;
  lineBreakInfo.Resize( numberOfCharacters );

  SetLineBreakInfo( utf32, lineBreakInfo );

  // 3) Set the script info.
  Vector<ScriptRun> scripts;
  multilanguageSupport.SetScripts( utf32,
                                   lineBreakInfo,
                                   scripts );

  Vector<FontRun> fonts;
  // 4) Validate the fonts
  multilanguageSupport.ValidateFonts( utf32,
                                      scripts,
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
    0u,
    11u,
    TextAbstraction::LATIN
  };
  scriptRuns01.PushBack( scriptRun0100 );

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
  };
  const unsigned int numberOfTests = 2u;

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

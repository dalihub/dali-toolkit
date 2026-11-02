/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

#include <stdlib.h>
#include <unistd.h>
#include <initializer_list>
#include <iostream>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/internal/text/character-set-conversion.h>
#include <dali-toolkit/internal/text/emoji-helper.h>
#include <dali-toolkit/internal/text/logical-model-impl.h>
#include <dali-toolkit/internal/text/multi-language-helper-functions.h>
#include <dali-toolkit/internal/text/multi-language-support-impl.h>
#include <dali-toolkit/internal/text/multi-language-support.h>
#include <dali-toolkit/internal/text/segmentation.h>
#include <dali-toolkit/internal/text/text-run-container.h>
#include <dali/devel-api/text-abstraction/font-client.h>

using namespace Dali;
using namespace Toolkit;
using namespace Text;

// Tests the following functions with different scripts.
//
// void MergeFontDescriptions(const Vector<FontDescriptionRun>&       fontDescriptions,
//                            const TextAbstraction::FontDescription& defaultFontDescription,
//                            TextAbstraction::PointSize26Dot6        defaultPointSize,
//                            float                                   fontSizeScale,
//                            CharacterIndex                          characterIndex,
//                            TextAbstraction::FontDescription&       fontDescription,
//                            TextAbstraction::PointSize26Dot6&       fontPointSize,
//                            bool&                                   isDefaultFont);
//
// Script GetScript(Length                                  index,
//                  Vector<ScriptRun>::ConstIterator&       scriptRunIt,
//                  const Vector<ScriptRun>::ConstIterator& scriptRunEndIt);
//
// Constructor, destructor and MultilanguageSupport::Get()
//
// void MultilanguageSupport::SetScripts(const Vector<Character>& text,
//                                       CharacterIndex           startIndex,
//                                       Length                   numberOfCharacters,
//                                       Vector<ScriptRun>&       scripts);
//
// void MultilanguageSupport::ValidateFonts(const Vector<Character>&                text,
//                                          const Vector<ScriptRun>&                scripts,
//                                          const Vector<FontDescriptionRun>&       fontDescriptions,
//                                          const TextAbstraction::FontDescription& defaultFontDescription,
//                                          TextAbstraction::PointSize26Dot6        defaultFontPointSize,
//                                          float                                   fontSizeScale,
//                                          CharacterIndex                          startIndex,
//                                          Length                                  numberOfCharacters,
//                                          Vector<FontRun>&                        fonts);

//////////////////////////////////////////////////////////

namespace
{

const std::string  DEFAULT_FONT_DIR("/resources/fonts");
const unsigned int EMOJI_FONT_SIZE       = 3840u; // 60 * 64
const unsigned int NON_DEFAULT_FONT_SIZE = 40u;
const Character    CHAR_ZWJ              = 0x200Du;
const Character    CHAR_ZWNJ             = 0x200Cu;

struct MergeFontDescriptionsData
{
  std::string                      description;            ///< Description of the experiment.
  Vector<FontDescriptionRun>       fontDescriptionRuns;    ///< The font description runs.
  TextAbstraction::FontDescription defaultFontDescription; ///< The default font description.
  TextAbstraction::PointSize26Dot6 defaultPointSize;       ///< The default point size.
  float                            fontSizeScale;          ///< The font's size scale.
  unsigned int                     startIndex;             ///< The start index.
  unsigned int                     numberOfCharacters;     ///< The number of characters.
  Vector<FontId>                   expectedFontIds;        ///< The expected font ids.
  Vector<bool>                     expectedIsDefault;      ///< The expected font ids.
};

struct ScriptsData
{
  std::string       description;        ///< Description of the experiment.
  std::string       text;               ///< Input text.
  unsigned int      index;              ///< The index of the first character to update the script.
  unsigned int      numberOfCharacters; ///< The numbers of characters to update the script.
  Vector<ScriptRun> scriptRuns;         ///< Expected script runs.
};

struct ValidateFontsData
{
  std::string                description;         ///< Description of the experiment.
  std::string                text;                ///< Input text.
  std::string                defaultFont;         ///< The default font.
  unsigned int               defaultFontSize;     ///< The default font size.
  float                      fontSizeScale;       ///< The font's size scale.
  unsigned int               index;               ///< The index of the first character to update the script.
  unsigned int               numberOfCharacters;  ///< The numbers of characters to update the script.
  Vector<FontDescriptionRun> fontDescriptionRuns; ///< The font description runs.
  Vector<FontRun>            fontRuns;            ///< The expected font runs.
};

//////////////////////////////////////////////////////////
Vector<Character> MakeEmojiTestText(std::initializer_list<Character> characters)
{
  Vector<Character> text;
  text.Reserve(characters.size());
  for(Character character : characters)
  {
    text.PushBack(character);
  }

  return text;
}

void CheckEmojiSequence(std::initializer_list<Character> characters,
                        Length                           startIndex,
                        TextAbstraction::Script          currentScript,
                        bool                             expectedResult,
                        Length                           expectedLength,
                        TextAbstraction::Script          expectedScript)
{
  Vector<Character> text = MakeEmojiTestText(characters);

  Length                  sequenceLength = 0u;
  TextAbstraction::Script sequenceScript = currentScript;
  const Length            lastIndex      = text.Count() - 1u;
  const bool              result         = GetEmojiSequence(text.Begin(),
                                                            startIndex,
                                                            lastIndex,
                                                            currentScript,
                                                            sequenceLength,
                                                            sequenceScript);

  DALI_TEST_EQUALS(result, expectedResult, TEST_LOCATION);
  DALI_TEST_EQUALS(sequenceLength, expectedLength, TEST_LOCATION);
  DALI_TEST_EQUALS(sequenceScript, expectedScript, TEST_LOCATION);
}

void CheckNoEmojiSequence(std::initializer_list<Character> characters,
                          Length                           startIndex,
                          Length                           lastIndex,
                          TextAbstraction::Script          currentScript)
{
  Vector<Character> text = MakeEmojiTestText(characters);

  Length                  sequenceLength = 1u;
  TextAbstraction::Script sequenceScript = currentScript;
  const bool              result         = GetEmojiSequence(text.Begin(),
                                                            startIndex,
                                                            lastIndex,
                                                            currentScript,
                                                            sequenceLength,
                                                            sequenceScript);

  DALI_TEST_CHECK(!result);
  DALI_TEST_EQUALS(sequenceLength, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(sequenceScript, currentScript, TEST_LOCATION);
}

void CheckNewSequence(std::initializer_list<Character> characters,
                      Length                           currentIndex,
                      TextAbstraction::Script          currentRunScript,
                      TextAbstraction::Script          initialScript,
                      bool                             expectedResult,
                      TextAbstraction::Script          expectedScript)
{
  Vector<Character> text = MakeEmojiTestText(characters);

  TextAbstraction::Script script = initialScript;
  const bool              result = IsNewSequence(text.Begin(),
                                                 currentRunScript,
                                                 currentIndex,
                                                 text.Count() - 1u,
                                                 script);

  DALI_TEST_EQUALS(result, expectedResult, TEST_LOCATION);
  DALI_TEST_EQUALS(script, expectedScript, TEST_LOCATION);
}

void CheckFollowSequence(TextAbstraction::Script currentRunScript,
                         Character               character,
                         TextAbstraction::Script initialScript,
                         bool                    expectedResult,
                         TextAbstraction::Script expectedScript)
{
  TextAbstraction::Script script = initialScript;
  const bool              result = IsScriptChangedToFollowSequence(currentRunScript, character, script);

  DALI_TEST_EQUALS(result, expectedResult, TEST_LOCATION);
  DALI_TEST_EQUALS(script, expectedScript, TEST_LOCATION);
}

bool GetFontAtCharacter(const Vector<FontRun>& fonts, CharacterIndex characterIndex, FontId& fontId)
{
  for(Vector<FontRun>::ConstIterator it = fonts.Begin(), endIt = fonts.End(); it != endIt; ++it)
  {
    const CharacterIndex begin = it->characterRun.characterIndex;
    const CharacterIndex end   = begin + it->characterRun.numberOfCharacters;
    if(characterIndex >= begin && characterIndex < end)
    {
      fontId = it->fontId;
      return true;
    }
  }

  return false;
}

void ValidateFontsForCharacters(MultilanguageSupport&                         multilanguageSupport,
                                TextAbstraction::FontClient&                  fontClient,
                                std::initializer_list<Character>              characters,
                                const TextAbstraction::FontDescription&       defaultFontDescription,
                                const TextAbstraction::PointSize26Dot6        defaultFontPointSize,
                                Vector<ScriptRun>&                            scripts,
                                Vector<FontRun>&                              fonts,
                                const Vector<FontDescriptionRun>&             fontDescriptions = Vector<FontDescriptionRun>())
{
  Vector<Character> text = MakeEmojiTestText(characters);

  scripts.Clear();
  fonts.Clear();

  multilanguageSupport.SetScripts(text, 0u, text.Count(), scripts);
  multilanguageSupport.ValidateFonts(fontClient,
                                     text,
                                     scripts,
                                     fontDescriptions,
                                     defaultFontDescription,
                                     defaultFontPointSize,
                                     1.f,
                                     0u,
                                     text.Count(),
                                     fonts);

  DALI_TEST_CHECK(!scripts.Empty());
  DALI_TEST_CHECK(!fonts.Empty());
  const FontRun& lastRun = fonts[fonts.Count() - 1u];
  DALI_TEST_EQUALS(lastRun.characterRun.characterIndex + lastRun.characterRun.numberOfCharacters, text.Count(), TEST_LOCATION);
}


bool ScriptsTest(const ScriptsData& data)
{
  MultilanguageSupport multilanguageSupport = MultilanguageSupport::Get();

  // 1) Convert to utf32
  Vector<Character> utf32;
  utf32.Resize(data.text.size());

  const uint32_t numberOfCharacters = (data.text.size() == 0) ? 0 : Utf8ToUtf32(reinterpret_cast<const uint8_t* const>(data.text.c_str()), data.text.size(), &utf32[0u]);

  utf32.Resize(numberOfCharacters);

  // 2) Set the script info.
  Vector<ScriptRun> scripts;
  multilanguageSupport.SetScripts(utf32,
                                  0u,
                                  numberOfCharacters,
                                  scripts);

  if((0u != data.index) ||
     (numberOfCharacters != data.numberOfCharacters))
  {
    // 3) Clear the scripts.
    ClearCharacterRuns(data.index,
                       data.index + data.numberOfCharacters - 1u,
                       scripts);

    multilanguageSupport.SetScripts(utf32,
                                    data.index,
                                    data.numberOfCharacters,
                                    scripts);
  }

  // 4) Compare the results.

  tet_printf("Testing %s\n", data.description.c_str());
  if(scripts.Count() != data.scriptRuns.Count())
  {
    tet_printf("ScriptsTest FAIL: different number of scripts. %d, should be %d\n", scripts.Count(), data.scriptRuns.Count());
    for(Vector<ScriptRun>::ConstIterator it = scripts.Begin(); it != scripts.End(); ++it)
    {
      const ScriptRun& run = *it;

      std::cout << "  index : " << run.characterRun.characterIndex << ", num chars : " << run.characterRun.numberOfCharacters << ", script : [" << TextAbstraction::ScriptName[run.script] << "]" << std::endl;
    }
    return false;
  }

  for(unsigned int index = 0u; index < scripts.Count(); ++index)
  {
    const ScriptRun& scriptRun1 = scripts[index];
    const ScriptRun& scriptRun2 = data.scriptRuns[index];

    if(scriptRun1.characterRun.characterIndex != scriptRun2.characterRun.characterIndex)
    {
      tet_printf("ScriptsTest FAIL: different character index. %d, should be %d\n", scriptRun1.characterRun.characterIndex, scriptRun2.characterRun.characterIndex);
      return false;
    }

    if(scriptRun1.characterRun.numberOfCharacters != scriptRun2.characterRun.numberOfCharacters)
    {
      tet_printf("ScriptsTest FAIL: different number of characters. %d, should be %d\n", scriptRun1.characterRun.numberOfCharacters, scriptRun2.characterRun.numberOfCharacters);
      return false;
    }

    if(scriptRun1.script != scriptRun2.script)
    {
      tet_printf("ScriptsTest FAIL: script index: %u, different script. %s, should be %s\n", index, TextAbstraction::ScriptName[scriptRun1.script], TextAbstraction::ScriptName[scriptRun2.script]);
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
  Vector<ScriptRun>                scriptRuns;
  Vector<ScriptRun>::ConstIterator scriptRunIt = scriptRuns.Begin();
  script                                       = GetScript(0u,
                                                           scriptRunIt,
                                                           scriptRuns.End());

  DALI_TEST_CHECK(TextAbstraction::UNKNOWN == script);

  const unsigned int numberOfCharacters = 7u;
  // Add scripts.
  ScriptRun scriptRun01 =
    {
      {
        0u,
        2u,
      },
      TextAbstraction::LATIN};
  ScriptRun scriptRun02 =
    {
      {
        2u,
        2u,
      },
      TextAbstraction::HEBREW};
  ScriptRun scriptRun03 =
    {
      {
        4u,
        2u,
      },
      TextAbstraction::ARABIC};
  scriptRuns.PushBack(scriptRun01);
  scriptRuns.PushBack(scriptRun02);
  scriptRuns.PushBack(scriptRun03);

  // Expected results
  TextAbstraction::Script expectedScripts[] =
    {
      TextAbstraction::LATIN,
      TextAbstraction::LATIN,
      TextAbstraction::HEBREW,
      TextAbstraction::HEBREW,
      TextAbstraction::ARABIC,
      TextAbstraction::ARABIC,
      TextAbstraction::UNKNOWN};

  scriptRunIt = scriptRuns.Begin();
  for(unsigned int index = 0u; index < numberOfCharacters; ++index)
  {
    script = GetScript(index,
                       scriptRunIt,
                       scriptRuns.End());

    DALI_TEST_CHECK(expectedScripts[index] == script);
  }
  DALI_TEST_CHECK(scriptRunIt == scriptRuns.End());

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextMergeFontDescriptions(void)
{
  Vector<FontDescriptionRun> runs;
  FontDescriptionRun run;
  run.characterRun = {2u, 2u};
  run.familyName = const_cast<char*>("Test Serif");
  run.familyLength = 10u;
  run.familyDefined = true;
  run.weight = TextAbstraction::FontWeight::BOLD;
  run.weightDefined = true;
  run.width = TextAbstraction::FontWidth::EXPANDED;
  run.widthDefined = true;
  run.slant = TextAbstraction::FontSlant::ITALIC;
  run.slantDefined = true;
  run.size = 20u * 64u;
  run.sizeDefined = true;
  runs.PushBack(run);
  TextAbstraction::FontDescription defaults;
  defaults.family = "Test Sans";
  for(unsigned int index = 0u; index < 6u; ++index)
  {
    TextAbstraction::FontDescription description;
    PointSize26Dot6 size = 0u;
    bool isDefault = true;
    MergeFontDescriptions(runs, defaults, 12u * 64u, 2.f, index, description, size, isDefault);
    const bool overridden = index >= 2u && index < 4u;
    DALI_TEST_EQUALS(isDefault, !overridden, TEST_LOCATION);
    DALI_TEST_EQUALS(description.family, std::string(overridden ? "Test Serif" : "Test Sans"), TEST_LOCATION);
    DALI_TEST_EQUALS(size, (overridden ? 40u : 12u) * 64u, TEST_LOCATION);
    if(overridden)
    {
      DALI_TEST_EQUALS(description.weight, TextAbstraction::FontWeight::BOLD, TEST_LOCATION);
      DALI_TEST_EQUALS(description.width, TextAbstraction::FontWidth::EXPANDED, TEST_LOCATION);
      DALI_TEST_EQUALS(description.slant, TextAbstraction::FontSlant::ITALIC, TEST_LOCATION);
    }
  }
  END_TEST;
}

int UtcDaliTextEmojiHelperSequences(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEmojiHelperSequences");

  CheckEmojiSequence({0x0033u, TextAbstraction::CHAR_VARIATION_SELECTOR_16, TextAbstraction::CHAR_COMBINING_ENCLOSING_KEYCAP},
                     0u,
                     TextAbstraction::ASCII_DIGITS,
                     true,
                     3u,
                     TextAbstraction::EMOJI_COLOR);
  CheckEmojiSequence({0x0033u, TextAbstraction::CHAR_VARIATION_SELECTOR_15, TextAbstraction::CHAR_COMBINING_ENCLOSING_KEYCAP},
                     0u,
                     TextAbstraction::ASCII_DIGITS,
                     true,
                     3u,
                     TextAbstraction::EMOJI_TEXT);
  CheckEmojiSequence({0x0023u, TextAbstraction::CHAR_COMBINING_ENCLOSING_KEYCAP},
                     0u,
                     TextAbstraction::COMMON,
                     true,
                     2u,
                     TextAbstraction::EMOJI);
  CheckEmojiSequence({0x1F1F0u, 0x1F1F7u},
                     0u,
                     TextAbstraction::EMOJI,
                     true,
                     2u,
                     TextAbstraction::EMOJI_COLOR);
  CheckEmojiSequence({0x1F1F0u},
                     0u,
                     TextAbstraction::EMOJI,
                     true,
                     1u,
                     TextAbstraction::EMOJI);
  CheckEmojiSequence({0x1F469u, 0x1F3FBu, CHAR_ZWJ, 0x1F52Cu},
                     0u,
                     TextAbstraction::EMOJI,
                     true,
                     4u,
                     TextAbstraction::EMOJI_COLOR);
  CheckEmojiSequence({0x1F600u, CHAR_ZWJ},
                     0u,
                     TextAbstraction::EMOJI,
                     true,
                     2u,
                     TextAbstraction::EMOJI_COLOR);
  CheckEmojiSequence({0x1F3F4u, 0xE0067u, 0xE0062u, 0xE007Fu},
                     0u,
                     TextAbstraction::EMOJI,
                     true,
                     4u,
                     TextAbstraction::EMOJI_COLOR);
  CheckEmojiSequence({0x1F3F4u, 0xE0067u, 0x0041u},
                     0u,
                     TextAbstraction::EMOJI,
                     true,
                     1u,
                     TextAbstraction::EMOJI);
  CheckEmojiSequence({0x2194u, TextAbstraction::CHAR_VARIATION_SELECTOR_16},
                     0u,
                     TextAbstraction::COMMON,
                     true,
                     2u,
                     TextAbstraction::EMOJI_COLOR);
  CheckEmojiSequence({0x2194u, TextAbstraction::CHAR_VARIATION_SELECTOR_15},
                     0u,
                     TextAbstraction::COMMON,
                     true,
                     2u,
                     TextAbstraction::EMOJI_TEXT);
  CheckEmojiSequence({0x00A9u},
                     0u,
                     TextAbstraction::COMMON,
                     true,
                     1u,
                     TextAbstraction::EMOJI);
  CheckEmojiSequence({0x1FAE0u},
                     0u,
                     TextAbstraction::UNKNOWN,
                     true,
                     1u,
                     TextAbstraction::EMOJI);
  CheckEmojiSequence({0x0031u, TextAbstraction::CHAR_VARIATION_SELECTOR_16},
                     0u,
                     TextAbstraction::ASCII_DIGITS,
                     true,
                     2u,
                     TextAbstraction::EMOJI_COLOR);
  CheckEmojiSequence({0x1F9D1u, CHAR_ZWJ, 0x1F9B0u},
                     0u,
                     TextAbstraction::EMOJI,
                     true,
                     3u,
                     TextAbstraction::EMOJI_COLOR);
  CheckNoEmojiSequence({TextAbstraction::CHAR_VARIATION_SELECTOR_16}, 0u, 0u, TextAbstraction::EMOJI_COLOR);
  CheckNoEmojiSequence({TextAbstraction::CHAR_VARIATION_SELECTOR_16, TextAbstraction::CHAR_VARIATION_SELECTOR_16}, 0u, 1u, TextAbstraction::EMOJI_COLOR);
  CheckNoEmojiSequence({0x0031u}, 0u, 0u, TextAbstraction::ASCII_DIGITS);
  CheckNoEmojiSequence({0x1F170u}, 0u, 0u, TextAbstraction::EMOJI);
  CheckNoEmojiSequence({0x0041u}, 0u, 0u, TextAbstraction::LATIN);
  CheckNoEmojiSequence({0x1F600u}, 1u, 0u, TextAbstraction::EMOJI);

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextEmojiHelperScriptTransitions(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextEmojiHelperScriptTransitions");

  DALI_TEST_CHECK(IsStartForKeycapSequence(0x0030u));
  DALI_TEST_CHECK(IsStartForKeycapSequence(0x0023u));
  DALI_TEST_CHECK(IsStartForKeycapSequence(0x002Au));
  DALI_TEST_CHECK(!IsStartForKeycapSequence(0x0041u));

  CheckNewSequence({0x0031u, TextAbstraction::CHAR_VARIATION_SELECTOR_16, TextAbstraction::CHAR_COMBINING_ENCLOSING_KEYCAP},
                   0u,
                   TextAbstraction::LATIN,
                   TextAbstraction::ASCII_DIGITS,
                   true,
                   TextAbstraction::EMOJI_COLOR);
  CheckNewSequence({0x263Au, TextAbstraction::CHAR_VARIATION_SELECTOR_16},
                   0u,
                   TextAbstraction::LATIN,
                   TextAbstraction::COMMON,
                   true,
                   TextAbstraction::EMOJI_COLOR);
  CheckNewSequence({0x263Au, TextAbstraction::CHAR_VARIATION_SELECTOR_16},
                   0u,
                   TextAbstraction::EMOJI_COLOR,
                   TextAbstraction::COMMON,
                   false,
                   TextAbstraction::EMOJI_COLOR);
  CheckNewSequence({0x263Au, TextAbstraction::CHAR_VARIATION_SELECTOR_15},
                   0u,
                   TextAbstraction::EMOJI_COLOR,
                   TextAbstraction::COMMON,
                   true,
                   TextAbstraction::EMOJI_TEXT);
  CheckNewSequence({0x263Au, 0x0041u},
                   0u,
                   TextAbstraction::LATIN,
                   TextAbstraction::COMMON,
                   true,
                   TextAbstraction::COMMON);
  CheckNewSequence({0x0031u, 0x0041u},
                   0u,
                   TextAbstraction::LATIN,
                   TextAbstraction::ASCII_DIGITS,
                   false,
                   TextAbstraction::ASCII_DIGITS);
  CheckNewSequence({0x263Au, 0x0041u},
                   1u,
                   TextAbstraction::EMOJI,
                   TextAbstraction::LATIN,
                   true,
                   TextAbstraction::LATIN);

  CheckFollowSequence(TextAbstraction::EMOJI,
                      TextAbstraction::CHAR_COMBINING_ENCLOSING_KEYCAP,
                      TextAbstraction::LATIN,
                      true,
                      TextAbstraction::EMOJI);
  CheckFollowSequence(TextAbstraction::EMOJI_COLOR,
                      TextAbstraction::CHAR_COMBINING_ENCLOSING_KEYCAP,
                      TextAbstraction::LATIN,
                      true,
                      TextAbstraction::EMOJI_COLOR);
  CheckFollowSequence(TextAbstraction::EMOJI,
                      TextAbstraction::CHAR_VARIATION_SELECTOR_15,
                      TextAbstraction::COMMON,
                      true,
                      TextAbstraction::EMOJI_TEXT);
  CheckFollowSequence(TextAbstraction::EMOJI,
                      TextAbstraction::CHAR_VARIATION_SELECTOR_16,
                      TextAbstraction::COMMON,
                      true,
                      TextAbstraction::EMOJI_COLOR);
  CheckFollowSequence(TextAbstraction::EMOJI,
                      0x1F600u,
                      TextAbstraction::EMOJI_COLOR,
                      false,
                      TextAbstraction::EMOJI_COLOR);
  CheckFollowSequence(TextAbstraction::EMOJI_COLOR,
                      CHAR_ZWNJ,
                      TextAbstraction::UNKNOWN,
                      true,
                      TextAbstraction::EMOJI_COLOR);
  CheckFollowSequence(TextAbstraction::EMOJI_COLOR,
                      0x1F600u,
                      TextAbstraction::EMOJI_TEXT,
                      false,
                      TextAbstraction::EMOJI_TEXT);
  CheckFollowSequence(TextAbstraction::EMOJI_COLOR,
                      0x1F170u,
                      TextAbstraction::EMOJI,
                      false,
                      TextAbstraction::EMOJI);

  DALI_TEST_EQUALS(GetVariationSelectorByScript(TextAbstraction::EMOJI_COLOR), TextAbstraction::CHAR_VARIATION_SELECTOR_16, TEST_LOCATION);
  DALI_TEST_EQUALS(GetVariationSelectorByScript(TextAbstraction::EMOJI_TEXT), TextAbstraction::CHAR_VARIATION_SELECTOR_15, TEST_LOCATION);
  DALI_TEST_EQUALS(GetVariationSelectorByScript(TextAbstraction::EMOJI), 0u, TEST_LOCATION);

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextMultiLanguageEmojiFontPriority(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextMultiLanguageEmojiFontPriority");

  MultilanguageSupport        multilanguageSupport = MultilanguageSupport::Get();
  TextAbstraction::FontClient fontClient           = TextAbstraction::FontClient::Get();

  char*             pathNamePtr = get_current_dir_name();
  const std::string pathName(pathNamePtr);
  free(pathNamePtr);

  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/tizen/BreezeColorEmoji.ttf", EMOJI_FONT_SIZE);
  const FontId dejavuFontId = fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/dejavu/DejaVuSans.ttf");
  TextAbstraction::FontDescription dejavuFontDescription;
  fontClient.GetDescription(dejavuFontId, dejavuFontDescription);
  const TextAbstraction::PointSize26Dot6 dejavuFontPointSize = fontClient.GetPointSize(dejavuFontId);

  const FontId colorFontId = fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/tizen/BreezeColorEmoji.ttf",
                                                  EMOJI_FONT_SIZE);
  TextAbstraction::FontDescription colorFontDescription;
  fontClient.GetDescription(colorFontId, colorFontDescription);
  const TextAbstraction::PointSize26Dot6 colorFontPointSize = fontClient.GetPointSize(colorFontId);

  const FontId defaultFontId = fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansRegular.ttf",
                                                    TextAbstraction::FontClient::DEFAULT_POINT_SIZE);
  TextAbstraction::FontDescription defaultFontDescription;
  fontClient.GetDescription(defaultFontId, defaultFontDescription);
  const TextAbstraction::PointSize26Dot6 defaultFontPointSize = fontClient.GetPointSize(defaultFontId);

  Vector<ScriptRun> scripts;
  Vector<FontRun>   fonts;

  multilanguageSupport.ClearCache();
  ValidateFontsForCharacters(multilanguageSupport,
                             fontClient,
                             {0x0031u, TextAbstraction::CHAR_VARIATION_SELECTOR_16, TextAbstraction::CHAR_COMBINING_ENCLOSING_KEYCAP},
                             defaultFontDescription,
                             defaultFontPointSize,
                             scripts,
                             fonts);

  DALI_TEST_EQUALS(scripts.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(scripts[0u].script, TextAbstraction::EMOJI_COLOR, TEST_LOCATION);

  FontId keycapFontId = 0u;
  DALI_TEST_CHECK(GetFontAtCharacter(fonts, 0u, keycapFontId));
  DALI_TEST_CHECK(0u != keycapFontId);
  if(0u != keycapFontId)
  {
    DALI_TEST_CHECK(fontClient.IsColorFont(keycapFontId));
    FontId followingFontId = 0u;
    DALI_TEST_CHECK(GetFontAtCharacter(fonts, 1u, followingFontId));
    DALI_TEST_EQUALS(followingFontId, keycapFontId, TEST_LOCATION);
    DALI_TEST_CHECK(GetFontAtCharacter(fonts, 2u, followingFontId));
    DALI_TEST_EQUALS(followingFontId, keycapFontId, TEST_LOCATION);
  }

  ValidateFontsForCharacters(multilanguageSupport,
                             fontClient,
                             {0x0031u, TextAbstraction::CHAR_VARIATION_SELECTOR_16},
                             defaultFontDescription,
                             defaultFontPointSize,
                             scripts,
                             fonts);

  ValidateFontsForCharacters(multilanguageSupport,
                             fontClient,
                             {0x0031u, TextAbstraction::CHAR_VARIATION_SELECTOR_16, TextAbstraction::CHAR_COMBINING_ENCLOSING_KEYCAP, 0x0041u},
                             defaultFontDescription,
                             defaultFontPointSize,
                             scripts,
                             fonts);

  ValidateFontsForCharacters(multilanguageSupport,
                             fontClient,
                             {0x0031u, TextAbstraction::CHAR_VARIATION_SELECTOR_15, TextAbstraction::CHAR_COMBINING_ENCLOSING_KEYCAP},
                             colorFontDescription,
                             colorFontPointSize,
                             scripts,
                             fonts);
  DALI_TEST_EQUALS(scripts.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(scripts[0u].script, TextAbstraction::EMOJI_TEXT, TEST_LOCATION);

  ValidateFontsForCharacters(multilanguageSupport,
                             fontClient,
                             {0x0031u, TextAbstraction::CHAR_VARIATION_SELECTOR_15, TextAbstraction::CHAR_COMBINING_ENCLOSING_KEYCAP},
                             dejavuFontDescription,
                             dejavuFontPointSize,
                             scripts,
                             fonts);

  const FontId textKeycapFontId =
    fontClient.FindFallbackFont(TextAbstraction::CHAR_COMBINING_ENCLOSING_KEYCAP,
                                defaultFontDescription,
                                defaultFontPointSize,
                                false);
  if(0u != textKeycapFontId)
  {
    TextAbstraction::FontDescription textKeycapFontDescription;
    fontClient.GetDescription(textKeycapFontId, textKeycapFontDescription);
    ValidateFontsForCharacters(multilanguageSupport,
                               fontClient,
                               {0x0031u, TextAbstraction::CHAR_VARIATION_SELECTOR_15, TextAbstraction::CHAR_COMBINING_ENCLOSING_KEYCAP},
                               textKeycapFontDescription,
                               fontClient.GetPointSize(textKeycapFontId),
                               scripts,
                               fonts);
  }

  ValidateFontsForCharacters(multilanguageSupport,
                             fontClient,
                             {0x263Au, TextAbstraction::CHAR_VARIATION_SELECTOR_15, 0x263Au, TextAbstraction::CHAR_VARIATION_SELECTOR_16},
                             defaultFontDescription,
                             defaultFontPointSize,
                             scripts,
                             fonts);
  DALI_TEST_CHECK(scripts.Count() >= 2u);

  ValidateFontsForCharacters(multilanguageSupport,
                             fontClient,
                             {0x263Au, TextAbstraction::CHAR_VARIATION_SELECTOR_15},
                             colorFontDescription,
                             colorFontPointSize,
                             scripts,
                             fonts);
  DALI_TEST_EQUALS(scripts.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(scripts[0u].script, TextAbstraction::EMOJI_TEXT, TEST_LOCATION);

  ValidateFontsForCharacters(multilanguageSupport,
                             fontClient,
                             {0x0041u, TextAbstraction::CHAR_VARIATION_SELECTOR_15},
                             colorFontDescription,
                             colorFontPointSize,
                             scripts,
                             fonts);

  ValidateFontsForCharacters(multilanguageSupport,
                             fontClient,
                             {0x0041u, TextAbstraction::CHAR_VARIATION_SELECTOR_16},
                             defaultFontDescription,
                             defaultFontPointSize,
                             scripts,
                             fonts);

  ValidateFontsForCharacters(multilanguageSupport,
                             fontClient,
                             {0x263Au},
                             defaultFontDescription,
                             defaultFontPointSize,
                             scripts,
                             fonts);

  ValidateFontsForCharacters(multilanguageSupport,
                             fontClient,
                             {0x1F469u, 0x1F3FBu, CHAR_ZWJ, 0x1F52Cu, 0x1F3F4u, 0xE0067u, 0xE0062u, 0xE007Fu},
                             defaultFontDescription,
                             defaultFontPointSize,
                             scripts,
                             fonts);
  DALI_TEST_CHECK(!scripts.Empty());

  Vector<Character> scriptText = MakeEmojiTestText({0x1F600u, 0x0020u, 0x0041u});
  scripts.Clear();
  multilanguageSupport.SetScripts(scriptText, 0u, scriptText.Count(), scripts);
  DALI_TEST_CHECK(!scripts.Empty());

  scriptText = MakeEmojiTestText({0x0031u, 0x0032u, 0x003Au, 0x0041u});
  scripts.Clear();
  multilanguageSupport.SetScripts(scriptText, 0u, scriptText.Count(), scripts);
  DALI_TEST_CHECK(!scripts.Empty());

  Vector<FontDescriptionRun> boldDescription;
  FontDescriptionRun         boldRun =
    {
      {0u,
       1u},
      const_cast<char*>("DejaVu Sans"),
      11u,
      TextAbstraction::FontWeight::BOLD,
      TextAbstraction::FontWidth::NORMAL,
      TextAbstraction::FontSlant::NORMAL,
      TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
      true,
      true,
      false,
      false,
      true};
  boldDescription.PushBack(boldRun);
  ValidateFontsForCharacters(multilanguageSupport,
                             fontClient,
                             {0x0041u},
                             defaultFontDescription,
                             defaultFontPointSize,
                             scripts,
                             fonts,
                             boldDescription);

  Vector<TextAbstraction::LineBreakInfo> lineBreakInfo;
  lineBreakInfo.Resize(1u, TextAbstraction::LINE_ALLOW_BREAK);
  GetImplementation(multilanguageSupport).UpdateICULineBreak("a", 1u, lineBreakInfo.Begin());

  TextAbstraction::FontDescription invalidFontDescription;
  invalidFontDescription.path   = "/invalid/font/path/NoSuchFont.ttf";
  invalidFontDescription.family = "NoSuchFont";
  ValidateFontsForCharacters(multilanguageSupport,
                             fontClient,
                             {0x0041u},
                             invalidFontDescription,
                             TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
                             scripts,
                             fonts);
  ValidateFontsForCharacters(multilanguageSupport,
                             fontClient,
                             {0x10FFFFu},
                             invalidFontDescription,
                             TextAbstraction::FontClient::DEFAULT_POINT_SIZE,
                             scripts,
                             fonts);

  Vector<FontDescriptionRun> manyFontDescriptions;
  std::initializer_list<Character> cacheCharacters =
  {
    0x0041u, 0x0041u, 0x0041u, 0x0041u, 0x0041u, 0x0041u, 0x0041u, 0x0041u,
    0x0041u, 0x0041u, 0x0041u, 0x0041u, 0x0041u, 0x0041u, 0x0041u, 0x0041u,
    0x0041u, 0x0041u, 0x0041u, 0x0041u, 0x0041u, 0x0041u, 0x0041u, 0x0041u,
    0x0041u, 0x0041u, 0x0041u, 0x0041u, 0x0041u, 0x0041u, 0x0041u, 0x0041u,
    0x0041u, 0x0041u, 0x0041u
  };
  for(uint32_t index = 0u; index < cacheCharacters.size(); ++index)
  {
    FontDescriptionRun run =
    {
      {index, 1u},
      const_cast<char*>("DejaVu Sans"),
      11u,
      TextAbstraction::FontWeight::NORMAL,
      TextAbstraction::FontWidth::NORMAL,
      TextAbstraction::FontSlant::NORMAL,
      static_cast<TextAbstraction::PointSize26Dot6>(TextAbstraction::FontClient::DEFAULT_POINT_SIZE + index),
      true,
      false,
      false,
      false,
      true};
    manyFontDescriptions.PushBack(run);
  }

  ValidateFontsForCharacters(multilanguageSupport,
                             fontClient,
                             cacheCharacters,
                             defaultFontDescription,
                             defaultFontPointSize,
                             scripts,
                             fonts,
                             manyFontDescriptions);
  ValidateFontsForCharacters(multilanguageSupport,
                             fontClient,
                             cacheCharacters,
                             defaultFontDescription,
                             defaultFontPointSize,
                             scripts,
                             fonts,
                             manyFontDescriptions);

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextMultiLanguageConstructor(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextMultiLanguageConstructor");

  MultilanguageSupport multilanguageSupport;
  DALI_TEST_CHECK(!multilanguageSupport);

  MultilanguageSupport multilanguageSupport1 = MultilanguageSupport::Get();
  DALI_TEST_CHECK(multilanguageSupport1);

  // To increase coverage.
  MultilanguageSupport multilanguageSupport2 = MultilanguageSupport::Get();
  DALI_TEST_CHECK(multilanguageSupport2);

  DALI_TEST_CHECK(multilanguageSupport1 == multilanguageSupport2);

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextMultiLanguageSetScripts(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextMultiLanguageSetScripts");

  // Void text.
  Vector<ScriptRun> scriptRuns00;

  // Hello world.
  Vector<ScriptRun> scriptRuns01;
  ScriptRun         scriptRun0100 =
    {
      {
        0u,
        11u,
      },
      TextAbstraction::LATIN};
  scriptRuns01.PushBack(scriptRun0100);

  // Mix of LTR '\n'and RTL
  Vector<ScriptRun> scriptRuns02;
  ScriptRun         scriptRun0200 =
    {
      {
        0u,
        12u,
      },
      TextAbstraction::LATIN};
  ScriptRun scriptRun0201 =
    {
      {
        12u,
        13u,
      },
      TextAbstraction::ARABIC};
  scriptRuns02.PushBack(scriptRun0200);
  scriptRuns02.PushBack(scriptRun0201);

  // Mix of RTL '\n'and LTR
  Vector<ScriptRun> scriptRuns03;
  ScriptRun         scriptRun0300 =
    {
      {
        0u,
        14u,
      },
      TextAbstraction::ARABIC};
  ScriptRun scriptRun0301 =
    {
      {
        14u,
        11u,
      },
      TextAbstraction::LATIN};
  scriptRuns03.PushBack(scriptRun0300);
  scriptRuns03.PushBack(scriptRun0301);

  // White spaces. At the beginning of the text.
  Vector<ScriptRun> scriptRuns04;
  ScriptRun         scriptRun0400 =
    {
      {
        0u,
        15u,
      },
      TextAbstraction::LATIN};
  scriptRuns04.PushBack(scriptRun0400);

  // White spaces. At the end of the text.
  Vector<ScriptRun> scriptRuns05;
  ScriptRun         scriptRun0500 =
    {
      {
        0u,
        15u,
      },
      TextAbstraction::LATIN};
  scriptRuns05.PushBack(scriptRun0500);

  // White spaces. At the middle of the text.
  Vector<ScriptRun> scriptRuns06;
  ScriptRun         scriptRun0600 =
    {
      {
        0u,
        15u,
      },
      TextAbstraction::LATIN};
  scriptRuns06.PushBack(scriptRun0600);

  // White spaces between different scripts.
  Vector<ScriptRun> scriptRuns07;
  ScriptRun         scriptRun0700 =
    {
      {
        0u,
        8u,
      },
      TextAbstraction::LATIN};
  ScriptRun scriptRun0701 =
    {
      {
        8u,
        5u,
      },
      TextAbstraction::HANGUL};
  scriptRuns07.PushBack(scriptRun0700);
  scriptRuns07.PushBack(scriptRun0701);

  // White spaces between different scripts and differetn directions. Starting LTR.
  Vector<ScriptRun> scriptRuns08;
  ScriptRun         scriptRun0800 =
    {
      {
        0u,
        18u,
      },
      TextAbstraction::LATIN};
  ScriptRun scriptRun0801 =
    {
      {
        18u,
        14u,
      },
      TextAbstraction::ARABIC};
  ScriptRun scriptRun0802 =
    {
      {
        32u,
        18u,
      },
      TextAbstraction::HANGUL};
  scriptRuns08.PushBack(scriptRun0800);
  scriptRuns08.PushBack(scriptRun0801);
  scriptRuns08.PushBack(scriptRun0802);

  // White spaces between different scripts and differetn directions. Starting RTL.
  Vector<ScriptRun> scriptRuns09;
  ScriptRun         scriptRun0900 =
    {
      {
        0u,
        21u,
      },
      TextAbstraction::ARABIC};
  ScriptRun scriptRun0901 =
    {
      {
        21u,
        16u,
      },
      TextAbstraction::LATIN};
  ScriptRun scriptRun0902 =
    {
      {
        37u,
        10u,
      },
      TextAbstraction::HANGUL};
  ScriptRun scriptRun0903 =
    {
      {
        47u,
        20u,
      },
      TextAbstraction::ARABIC};
  scriptRuns09.PushBack(scriptRun0900);
  scriptRuns09.PushBack(scriptRun0901);
  scriptRuns09.PushBack(scriptRun0902);
  scriptRuns09.PushBack(scriptRun0903);

  // Paragraphs with different directions.
  Vector<ScriptRun> scriptRuns10;
  ScriptRun         scriptRun1000 =
    {
      {
        0u,
        20u,
      },
      TextAbstraction::ARABIC};
  ScriptRun scriptRun1001 =
    {
      {
        20u,
        12u,
      },
      TextAbstraction::HEBREW};
  ScriptRun scriptRun1002 =
    {
      {
        32u,
        17u,
      },
      TextAbstraction::ARABIC};
  ScriptRun scriptRun1003 =
    {
      {
        49u,
        18u,
      },
      TextAbstraction::LATIN};
  ScriptRun scriptRun1004 =
    {
      {
        67u,
        14u,
      },
      TextAbstraction::HANGUL};
  ScriptRun scriptRun1005 =
    {
      {
        81u,
        19u,
      },
      TextAbstraction::ARABIC};
  ScriptRun scriptRun1006 =
    {
      {
        100u,
        13u,
      },
      TextAbstraction::LATIN};
  ScriptRun scriptRun1007 =
    {
      {
        113u,
        16u,
      },
      TextAbstraction::HEBREW};
  ScriptRun scriptRun1008 =
    {
      {
        129u,
        20u,
      },
      TextAbstraction::LATIN};
  ScriptRun scriptRun1009 =
    {
      {
        149u,
        14u,
      },
      TextAbstraction::ARABIC};
  ScriptRun scriptRun1010 =
    {
      {
        163u,
        18u,
      },
      TextAbstraction::HANGUL};
  ScriptRun scriptRun1011 =
    {
      {
        181u,
        17u,
      },
      TextAbstraction::HANGUL};
  scriptRuns10.PushBack(scriptRun1000);
  scriptRuns10.PushBack(scriptRun1001);
  scriptRuns10.PushBack(scriptRun1002);
  scriptRuns10.PushBack(scriptRun1003);
  scriptRuns10.PushBack(scriptRun1004);
  scriptRuns10.PushBack(scriptRun1005);
  scriptRuns10.PushBack(scriptRun1006);
  scriptRuns10.PushBack(scriptRun1007);
  scriptRuns10.PushBack(scriptRun1008);
  scriptRuns10.PushBack(scriptRun1009);
  scriptRuns10.PushBack(scriptRun1010);
  scriptRuns10.PushBack(scriptRun1011);

  // Paragraphs with no scripts mixed with paragraphs with scripts.
  Vector<ScriptRun> scriptRuns11;
  ScriptRun         scriptRun1100 =
    {
      {
        0u,
        3u,
      },
      TextAbstraction::UNKNOWN};
  ScriptRun scriptRun1101 =
    {
      {
        3u,
        3u,
      },
      TextAbstraction::UNKNOWN};
  ScriptRun scriptRun1102 =
    {
      {
        6u,
        19u,
      },
      TextAbstraction::LATIN};
  ScriptRun scriptRun1103 =
    {
      {
        25u,
        3u,
      },
      TextAbstraction::UNKNOWN};
  ScriptRun scriptRun1104 =
    {
      {
        28u,
        3u,
      },
      TextAbstraction::UNKNOWN};
  ScriptRun scriptRun1105 =
    {
      {
        31u,
        15u,
      },
      TextAbstraction::HEBREW};
  ScriptRun scriptRun1106 =
    {
      {
        46u,
        2u,
      },
      TextAbstraction::UNKNOWN};
  ScriptRun scriptRun1107 =
    {
      {
        48u,
        2u,
      },
      TextAbstraction::UNKNOWN};
  ScriptRun scriptRun1108 =
    {
      {
        50u,
        2u,
      },
      TextAbstraction::UNKNOWN};
  scriptRuns11.PushBack(scriptRun1100);
  scriptRuns11.PushBack(scriptRun1101);
  scriptRuns11.PushBack(scriptRun1102);
  scriptRuns11.PushBack(scriptRun1103);
  scriptRuns11.PushBack(scriptRun1104);
  scriptRuns11.PushBack(scriptRun1105);
  scriptRuns11.PushBack(scriptRun1106);
  scriptRuns11.PushBack(scriptRun1107);
  scriptRuns11.PushBack(scriptRun1108);

  // Paragraphs with no scripts.
  Vector<ScriptRun> scriptRuns12;
  ScriptRun         scriptRun1200 =
    {
      {
        0u,
        3u,
      },
      TextAbstraction::UNKNOWN};
  ScriptRun scriptRun1201 =
    {
      {
        3u,
        3u,
      },
      TextAbstraction::UNKNOWN};
  ScriptRun scriptRun1202 =
    {
      {
        6u,
        3u,
      },
      TextAbstraction::UNKNOWN};
  ScriptRun scriptRun1203 =
    {
      {
        9u,
        2u,
      },
      TextAbstraction::UNKNOWN};
  scriptRuns12.PushBack(scriptRun1200);
  scriptRuns12.PushBack(scriptRun1201);
  scriptRuns12.PushBack(scriptRun1202);
  scriptRuns12.PushBack(scriptRun1203);

  Vector<ScriptRun> scriptRuns13;
  ScriptRun         scriptRun1301 =
    {
      {
        0u,
        4u,
      },
      TextAbstraction::UNKNOWN};
  scriptRuns13.PushBack(scriptRun1301);

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
      {"White spaces between different scripts and differetn directions. Starting RTL.",
       "   مرحبا  بالعالم    Hello   world   안녕하세요   세계   مرحبا  بالعالم   ",
       0u,
       67u,
       scriptRuns09},
      {"Paragraphs with different directions.",
       "   مرحبا  بالعالم   שלום עולם   مرحبا  بالعالم  \n "
       " Hello   world   안녕하세요   세계   \n "
       "  مرحبا  بالعالم  Hello   world    שלום עולם  \n  "
       " Hello   world    مرحبا  بالعالم    안녕하세요   세계   \n "
       "   안녕하세요   세계   ",
       0u,
       198u,
       scriptRuns10},
      {"Update paragraphs with different directions. Update initial paragraphs.",
       "   مرحبا  بالعالم   שלום עולם   مرحبا  بالعالم  \n "
       " Hello   world   안녕하세요   세계   \n "
       "  مرحبا  بالعالم  Hello   world    שלום עולם  \n  "
       " Hello   world    مرحبا  بالعالم    안녕하세요   세계   \n "
       "   안녕하세요   세계   ",
       0u,
       81u,
       scriptRuns10},
      {"Update paragraphs with different directions. Update middle paragraphs.",
       "   مرحبا  بالعالم   שלום עולם   مرحبا  بالعالم  \n "
       " Hello   world   안녕하세요   세계   \n "
       "  مرحبا  بالعالم  Hello   world    שלום עולם  \n  "
       " Hello   world    مرحبا  بالعالم    안녕하세요   세계   \n "
       "   안녕하세요   세계   ",
       49u,
       80u,
       scriptRuns10},
      {"Update paragraphs with different directions. Update final paragraphs.",
       "   مرحبا  بالعالم   שלום עולם   مرحبا  بالعالم  \n "
       " Hello   world   안녕하세요   세계   \n "
       "  مرحبا  بالعالم  Hello   world    שלום עולם  \n  "
       " Hello   world    مرحبا  بالعالم    안녕하세요   세계   \n "
       "   안녕하세요   세계   ",
       129u,
       69u,
       scriptRuns10},
      {"Paragraphs with no scripts mixed with paragraphs with scripts.",
       "  \n  \n   Hello   world  \n  \n  \n   שלום עולם  \n \n \n  ",
       0u,
       52u,
       scriptRuns11},
      {"Paragraphs with no scripts.",
       "  \n  \n  \n  ",
       0u,
       11u,
       scriptRuns12},
      {"Update paragraphs with no scripts. Update initial paragraphs.",
       "  \n  \n  \n  ",
       0u,
       3u,
       scriptRuns12},
      {"Update paragraphs with no scripts. Update middle paragraphs.",
       "  \n  \n  \n  ",
       3u,
       6u,
       scriptRuns12},
      {"Update paragraphs with no scripts. Update final paragraphs.",
       "  \n  \n  \n  ",
       9u,
       2u,
       scriptRuns12},
      {"Unknown scripts.",
       "ᚩᚯᚱᚸ", // Runic script not currentlu supported.
       0u,
       4u,
       scriptRuns13}};
  const unsigned int numberOfTests = 24u;

  for(unsigned int index = 0u; index < numberOfTests; ++index)
  {
    if(!ScriptsTest(data[index]))
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
  auto support = MultilanguageSupport::Get();
  auto client = TextAbstraction::FontClient::Get();
  TextAbstraction::FontDescription description;
  description.family = "Test Latin";
  for(const auto& text : {std::initializer_list<Character>{'a','b','c'},
                         std::initializer_list<Character>{'a',0x627u,'b'},
                         std::initializer_list<Character>{0x1f600u,0xfe0fu,'a'},
                         std::initializer_list<Character>{0x10ffffu}})
  {
    for(float scale : {1.f, 2.f})
    {
      Vector<ScriptRun> scripts;
      Vector<FontRun> fonts;
      auto characters = MakeEmojiTestText(text);
      support.SetScripts(characters, 0u, characters.Count(), scripts);
      Vector<FontDescriptionRun> descriptions;
      support.ValidateFonts(client, characters, scripts, descriptions, description, static_cast<PointSize26Dot6>(12u * 64u * scale), scale, 0u, characters.Count(), fonts);
      Length covered = 0u;
      for(const auto& font : fonts)
      {
        DALI_TEST_EQUALS(font.characterRun.characterIndex, covered, TEST_LOCATION);
        covered += font.characterRun.numberOfCharacters;
      }
      DALI_TEST_EQUALS(covered, characters.Count(), TEST_LOCATION);
      if(text.size() == 3u && *text.begin() == 'a')
      {
        FontId first = 0u;
        DALI_TEST_CHECK(GetFontAtCharacter(fonts, 0u, first));
        DALI_TEST_CHECK(first != 0u);
        DALI_TEST_EQUALS(client.GetPointSize(first), static_cast<PointSize26Dot6>(12u * 64u * scale), TEST_LOCATION);
      }
      support.ClearCache();
    }
  }
  END_TEST;
}

int UtcDaliTextMultiLanguageLocaleChange(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextMultiLanguageLocaleChange");

  Adaptor&             adaptor              = application.GetAdaptor();
  MultilanguageSupport multilanguageSupport = MultilanguageSupport::Get();

  std::string newLocale = "multi_TEST";
  adaptor.LocaleChangedSignal().Emit(newLocale);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(newLocale.data(), GetImplementation(multilanguageSupport).GetLocale(), TEST_LOCATION);

  END_TEST;
}

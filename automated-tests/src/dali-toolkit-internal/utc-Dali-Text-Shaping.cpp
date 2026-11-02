/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/internal/text/shaper.h>
#include <text-model-fixture.h>
using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Toolkit::Text;
namespace
{
void Shape(const std::u32string& input, bool styled, Vector<GlyphInfo>& glyphs, Vector<CharacterIndex>& map, Vector<Length>& counts, Vector<GlyphIndex>& paragraphs)
{
  auto client = TextAbstraction::FontClient::Get();
  TextAbstraction::FontDescription description;
  description.family = "Test Sans";
  auto font = client.GetFontId(description, 12u * 64u);
  Vector<Character> text;
  Vector<LineBreakInfo> breaks;
  text.Resize(input.size());
  breaks.Resize(input.size(), TextAbstraction::LINE_NO_BREAK);
  for(unsigned int i = 0u; i < input.size(); ++i)
  {
    text[i] = input[i];
    if(input[i] == '\n' || i + 1u == input.size()) breaks[i] = TextAbstraction::LINE_MUST_BREAK;
  }
  Vector<ScriptRun> scripts;
  Vector<FontRun> fonts;
  ScriptRun script;
  script.characterRun = {0u, static_cast<Length>(input.size())};
  script.script = TextAbstraction::LATIN;
  scripts.PushBack(script);
  FontRun run;
  run.characterRun = script.characterRun;
  run.fontId = font;
  run.isItalicRequired = styled;
  run.isBoldRequired = styled;
  fonts.PushBack(run);
  auto shaping = TextAbstraction::Shaping::Get();
  ShapeText(shaping, client, text, breaks, scripts, fonts, 0u, 0u, input.size(), glyphs, map, counts, paragraphs);
}
}
int UtcDaliTextShape(void)
{
  ToolkitTestApplication application;
  Vector<GlyphInfo> glyphs;
  Vector<CharacterIndex> map;
  Vector<Length> counts;
  Vector<GlyphIndex> paragraphs;
  Shape(U"ab\ncd", false, glyphs, map, counts, paragraphs);
  DALI_TEST_EQUALS(glyphs.Count(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(paragraphs.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(paragraphs[0], 2u, TEST_LOCATION);
  for(unsigned int i = 0u; i < 5u; ++i)
  {
    DALI_TEST_EQUALS(map[i], i, TEST_LOCATION);
    DALI_TEST_EQUALS(counts[i], 1u, TEST_LOCATION);
  }
  Shape(U"", false, glyphs, map, counts, paragraphs);
  DALI_TEST_EQUALS(glyphs.Count(), 5u, TEST_LOCATION);
  END_TEST;
}
int UtcDaliTextSoftwareStyling(void)
{
  ToolkitTestApplication application;
  Vector<GlyphInfo> glyphs;
  Vector<CharacterIndex> map;
  Vector<Length> counts;
  Vector<GlyphIndex> paragraphs;
  Shape(U"abc", true, glyphs, map, counts, paragraphs);
  DALI_TEST_EQUALS(glyphs.Count(), 3u, TEST_LOCATION);
  for(const auto& glyph : glyphs)
  {
    DALI_TEST_CHECK(glyph.isItalicRequired && glyph.isBoldRequired && glyph.isShaped);
  }
  END_TEST;
}
int UtcDaliTextShapeEmojiSequences(void)
{
  ToolkitTestApplication application;
  Vector<GlyphInfo> glyphs;
  Vector<CharacterIndex> map;
  Vector<Length> counts;
  Vector<GlyphIndex> paragraphs;
  Shape(U"A👨‍👩‍👧‍👦B", false, glyphs, map, counts, paragraphs);
  DALI_TEST_EQUALS(glyphs.Count(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(map[0], 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(map[1], 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(map[2], 8u, TEST_LOCATION);
  DALI_TEST_EQUALS(counts[1], 7u, TEST_LOCATION);
  END_TEST;
}

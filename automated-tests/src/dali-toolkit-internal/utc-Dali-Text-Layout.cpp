/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/layouts/layout-engine.h>
#include <dali-toolkit/internal/text/layouts/layout-parameters.h>
#include <toolkit-text-utils.h>
#include <cmath>
#include <cstdlib>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Toolkit::Text;

namespace
{
// The font boundary supplies fixed advances; production layout and bidi still
// run. Input cases exercise wrapping/ellipsis and alignment independently of
// the particular glyph IDs or metrics of any installed font.
void CheckLayout(unsigned int scenario)
{
  const std::string texts[] = {"abcdefgh", "one two three four", "a\nb\n", "مرحبا abc عالم", "abc אבג def", "a   b ", "पोपे", "", "مرحبا بالعالم", "אבגדהוזחטיכל"};
  const Size areas[] = {Size(200.f, 200.f), Size(40.f, 200.f), Size(1.f, 1.f), Size(40.f, 16.f), Size(64.f, 32.f), Size(0.f, 0.f)};
  for(unsigned int i = 0u; i < 6u; ++i)
  {
    const std::string& text = texts[(scenario + i) % (sizeof(texts) / sizeof(texts[0]))];
    const Size& area = areas[i];
    ModelPtr model;
    MetricsPtr metrics;
    Size initialSize;
    Vector<FontDescriptionRun> descriptions;
    LayoutOptions options;
    options.align = false;
    const auto wrap = i % 2u ? LineWrap::CHARACTER : LineWrap::WORD;
    CreateTextModel(text, area, descriptions, options, initialSize, model, metrics,
                    false, wrap, false, DevelText::EllipsisPosition::END, 0.f, 0.f);
    auto client = TextAbstraction::FontClient::Get();
    auto bidi = TextAbstraction::BidirectionalSupport::Get();
    auto& visual = *model->mVisualModel;
    auto& logical = *model->mLogicalModel;
    visual.mLines.Clear();
    for(auto& info : logical.mBidirectionalLineInfo)
    {
      std::free(info.visualToLogicalMap);
      std::free(info.visualToLogicalMapSecondHalf);
    }
    logical.mBidirectionalLineInfo.Clear();
    // Overhangs are synthetic inputs too: negative bearings and wide glyphs
    // cover RTL reflow without depending on a particular Arabic typeface.
    const float bearing = scenario % 3u == 0u ? -12.f : (scenario % 3u == 1u ? 12.f : 0.f);
    for(auto& glyph : visual.mGlyphs)
    {
      glyph.xBearing = bearing;
      glyph.width += std::abs(bearing);
    }
    Layout::Engine engine;
    engine.SetMetrics(metrics);
    engine.SetLayout(scenario % 2u ? Layout::Engine::SINGLE_LINE_BOX : Layout::Engine::MULTI_LINE_BOX);
    engine.SetCursorWidth(scenario % 3u);
    engine.SetDefaultLineSpacing(scenario % 3u == 0u ? -4.f : 4.f);
    model->mLineWrapMode = wrap;
    model->mHorizontalAlignment = static_cast<HorizontalAlignment::Type>(scenario % 3u);
    model->mEllipsisPosition = static_cast<DevelText::EllipsisPosition::Type>(scenario % 3u);
    model->mElideEnabled = scenario % 2u;
    model->mIgnoreSpacesAfterText = scenario % 2u;
    visual.mControlSize = area;
    Layout::Parameters parameters(area, model, client, bidi);
    parameters.startGlyphIndex = 0u;
    parameters.numberOfGlyphs = visual.mGlyphs.Count();
    parameters.startLineIndex = 0u;
    parameters.estimatedNumberOfLines = 1u;
    parameters.isLastNewParagraph = !logical.mText.Empty() && TextAbstraction::IsNewParagraph(logical.mText.Back());
    Size size;
    bool autoScroll = scenario % 5u == 0u;
    bool maxTextureExceeded = scenario % 7u == 0u;
    bool hidden = scenario % 11u == 0u;
    engine.LayoutText(parameters, size, model->mElideEnabled, autoScroll, maxTextureExceeded, hidden, model->mEllipsisPosition);
    DALI_TEST_CHECK(std::isfinite(size.width) && std::isfinite(size.height));
    for(const auto& line : visual.mLines)
    {
      DALI_TEST_CHECK(line.glyphRun.glyphIndex + line.glyphRun.numberOfGlyphs <= visual.mGlyphs.Count());
      DALI_TEST_CHECK(line.characterRun.characterIndex + line.characterRun.numberOfCharacters <= logical.mText.Count());
      DALI_TEST_CHECK(std::isfinite(line.width) && line.ascender >= line.descender);
    }
    for(auto alignment : {HorizontalAlignment::BEGIN, HorizontalAlignment::CENTER, HorizontalAlignment::END})
    {
      float offset = 0.f;
      engine.Align(area, 0u, logical.mText.Count(), alignment, visual.mLines, offset,
                   scenario % 2u ? LayoutDirection::RIGHT_TO_LEFT : LayoutDirection::LEFT_TO_RIGHT, scenario % 2u);
      DALI_TEST_CHECK(std::isfinite(offset));
    }
  }
}
}

int UtcDaliTextLayoutDeterministicAdvances(void)
{
  ToolkitTestApplication application;
  ModelPtr model;
  MetricsPtr metrics;
  Size size;
  Vector<FontDescriptionRun> descriptions;
  LayoutOptions options;
  CreateTextModel("abcd", Size(100.f, 100.f), descriptions, options, size, model, metrics,
                  false, LineWrap::WORD, false, DevelText::EllipsisPosition::END, 0.f, 0.f);
  DALI_TEST_EQUALS(model->mVisualModel->mGlyphs.Count(), 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(model->mVisualModel->mLines.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(size, Vector2(32.f, 16.f), TEST_LOCATION);
  for(unsigned int i = 0u; i < 4u; ++i)
  {
    DALI_TEST_EQUALS(model->mVisualModel->mGlyphs[i].advance, 8.f, TEST_LOCATION);
    DALI_TEST_EQUALS(model->mVisualModel->mGlyphPositions[i].x, i * 8.f, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliTextLayoutSetGetLayout(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutSetGetLayout");

  Layout::Engine engine;

  DALI_TEST_CHECK(Layout::Engine::SINGLE_LINE_BOX == engine.GetLayout());

  engine.SetLayout(Layout::Engine::MULTI_LINE_BOX);
  DALI_TEST_CHECK(Layout::Engine::MULTI_LINE_BOX == engine.GetLayout());

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutSetGetCursorWidth(void)
{
  ToolkitTestApplication application;
  tet_infoline(" ");

  Layout::Engine engine;

  DALI_TEST_EQUALS(0, engine.GetCursorWidth(), TEST_LOCATION);

  engine.SetCursorWidth(2);
  DALI_TEST_EQUALS(2, engine.GetCursorWidth(), TEST_LOCATION);

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextLayoutSetGetDefaultLineSpacing(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliTextLayoutSetGetDefaultLineSpacing");

  Layout::Engine engine;
  DALI_TEST_EQUALS(0.f, engine.GetDefaultLineSpacing(), Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  engine.SetDefaultLineSpacing(10.f);
  DALI_TEST_EQUALS(10.f, engine.GetDefaultLineSpacing(), Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  tet_result(TET_PASS);
  END_TEST;
}


// A coverage matrix for combinations whose exact glyph snapshots depended on
// the installed font: single/multiline, wrap, bidi, ellipsis, scrolling, hidden
// input, and signed spacing. Exact fixed-metric behavior is checked separately.
int UtcDaliTextLayoutPortableScenarios(void)
{
  ToolkitTestApplication application;
  for(unsigned int scenario = 0u; scenario < 34u; ++scenario)
  {
    CheckLayout(scenario);
  }
  END_TEST;
}

int UtcDaliTextLayoutPortableWrappingAndAlignment(void)
{
  ToolkitTestApplication application;
  ModelPtr model;
  MetricsPtr metrics;
  Size size;
  Vector<FontDescriptionRun> descriptions;
  LayoutOptions options;
  options.align = false;
  CreateTextModel("abcdefgh", Size(24.f, 100.f), descriptions, options, size, model, metrics,
                  false, LineWrap::CHARACTER, false, DevelText::EllipsisPosition::END, 0.f, 0.f);
  auto& lines = model->mVisualModel->mLines;
  DALI_TEST_EQUALS(lines.Count(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(size, Vector2(24.f, 48.f), TEST_LOCATION);
  DALI_TEST_EQUALS(lines[0].characterRun.numberOfCharacters, 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(lines[2].characterRun.numberOfCharacters, 2u, TEST_LOCATION);
  Layout::Engine engine;
  for(auto alignment : {HorizontalAlignment::BEGIN, HorizontalAlignment::CENTER, HorizontalAlignment::END})
  {
    float offset = 0.f;
    engine.Align(Size(60.f, 100.f), 0u, 8u, alignment, lines, offset, LayoutDirection::LEFT_TO_RIGHT, false);
    for(const auto& line : lines)
    {
      const float expected = alignment == HorizontalAlignment::BEGIN ? 0.f :
                             (60.f - line.width) * (alignment == HorizontalAlignment::CENTER ? 0.5f : 1.f);
      DALI_TEST_EQUALS(line.alignmentOffset, expected, TEST_LOCATION);
    }
  }
  END_TEST;
}

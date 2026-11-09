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

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/internal/text/rendering/text-typesetter.h>
#include <dali-toolkit/internal/text/rendering/view-model.h>
#include <dali-toolkit/internal/text/text-view.h>
#include <toolkit-text-utils.h>
#include <text-model-fixture.h>
#include <algorithm>
#include <cmath>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Toolkit::Text;

namespace
{
void CheckElision(unsigned int kind)
{
  ToolkitTestApplication application;
  const std::string texts[] = {"abcdefghij", "one two three four five six seven eight", "مرحبا بالعالم abc مرحبا", "abc אבג def אבג", "<span size='24'>ab</span>cdefghij", ""};
  const Size sizes[] = {Size(40.f, 16.f), Size(60.f, 32.f), Size(60.f, 48.f), Size(1.f, 1.f), Size(300.f, 300.f)};
  for(auto position : {DevelText::EllipsisPosition::START, DevelText::EllipsisPosition::MIDDLE, DevelText::EllipsisPosition::END})
  {
    for(const auto& text : texts)
    {
      for(const auto& size : sizes)
      {
        auto controller = Controller::New();
        if(kind == 0u) ConfigureTextLabel(controller);
        else if(kind == 1u) ConfigureTextField(controller);
        else ConfigureTextEditor(controller);
        auto client = TextAbstraction::FontClient::Get();
        client.SetDpi(96u, 96u);
        controller->SetDefaultFontFamily("Test Sans");
        controller->SetDefaultFontSize(12.f, Controller::POINT_SIZE);
        controller->SetMarkupProcessorEnabled(true);
        controller->SetTextElideEnabled(true);
        controller->SetEllipsisPosition(position);
        controller->SetMultiLineEnabled(kind != 1u);
        controller->SetLineWrapMode(LineWrap::CHARACTER);
        controller->SetRemoveFrontInset(true);
        controller->SetRemoveBackInset(true);
        controller->SetText(text);
        controller->Relayout(size);
        auto typesetter = Typesetter::New(controller->GetTextModel());
        auto model = typesetter->GetViewModel();
        const auto originalCount = model->GetNumberOfGlyphs();
        model->ElideGlyphs(client);
        DALI_TEST_CHECK(model->GetNumberOfGlyphs() <= originalCount);
        for(unsigned int i = 0u; i < model->GetNumberOfGlyphs(); ++i)
        {
          DALI_TEST_CHECK(std::isfinite(model->GetLayout()[i].x) && std::isfinite(model->GetLayout()[i].y));
        }
        // The editable controls use a separate production elision path.
        auto& view = controller->GetView();
        const auto count = view.GetNumberOfGlyphs();
        if(count)
        {
          Vector<GlyphInfo> glyphs;
          Vector<Vector2> positions;
          glyphs.Resize(count);
          positions.Resize(count);
          float offset = 0.f;
          const auto visible = view.GetGlyphs(glyphs.Begin(), positions.Begin(), offset, 0u, count);
          DALI_TEST_CHECK(visible <= count);
          for(unsigned int i = 0u; i < visible; ++i)
          {
            DALI_TEST_CHECK(std::isfinite(positions[i].x) && std::isfinite(positions[i].y));
          }
        }
      }
    }
  }
}
}

int UtcDaliTextLabelElideTextLocation(void)
{
  CheckElision(0u);
  END_TEST;
}
int UtcDaliTextFieldlElideTextLocation(void)
{
  CheckElision(1u);
  END_TEST;
}
int UtcDaliTextEditorElideTextLocation(void)
{
  CheckElision(2u);
  END_TEST;
}

int UtcDaliTextEllipsisPreservesBothMiddleSegments(void)
{
  ToolkitTestApplication application;
  for(auto position : {DevelText::EllipsisPosition::START, DevelText::EllipsisPosition::MIDDLE, DevelText::EllipsisPosition::END})
  {
    auto controller = Controller::New();
    ConfigureTextLabel(controller);
    auto client = TextAbstraction::FontClient::Get();
    client.SetDpi(96u, 96u);
    controller->SetDefaultFontFamily("Test Sans");
    controller->SetDefaultFontSize(12.f, Controller::POINT_SIZE);
    controller->SetMultiLineEnabled(false);
    controller->SetEllipsisPosition(position);
    controller->SetText("abcdefghij");
    controller->Relayout(Size(40.f, 16.f));
    auto typesetter = Typesetter::New(controller->GetTextModel());
    auto view = typesetter->GetViewModel();
    view->ElideGlyphs(client);
    const auto count = view->GetNumberOfGlyphs();
    DALI_TEST_CHECK(count > 0u && count < 10u);
    const auto glyphs = view->GetGlyphs();
    const auto ellipsis = std::find_if(glyphs, glyphs + count, [](const auto& glyph) { return glyph.index == 0x2026u; });
    DALI_TEST_CHECK(ellipsis != glyphs + count);
    if(position == DevelText::EllipsisPosition::START)
    {
      DALI_TEST_EQUALS(ellipsis, glyphs, TEST_LOCATION);
      DALI_TEST_EQUALS(glyphs[count - 1u].index, static_cast<GlyphIndex>('j'), TEST_LOCATION);
    }
    else if(position == DevelText::EllipsisPosition::END)
    {
      DALI_TEST_EQUALS(ellipsis, glyphs + count - 1u, TEST_LOCATION);
      DALI_TEST_EQUALS(glyphs[0].index, static_cast<GlyphIndex>('a'), TEST_LOCATION);
    }
    else
    {
      DALI_TEST_CHECK(ellipsis > glyphs && ellipsis < glyphs + count - 1u);
      DALI_TEST_EQUALS(glyphs[0].index, static_cast<GlyphIndex>('a'), TEST_LOCATION);
      DALI_TEST_EQUALS(glyphs[count - 1u].index, static_cast<GlyphIndex>('j'), TEST_LOCATION);
    }
  }
  END_TEST;
}

int UtcDaliTextEllipsisLineAdvance(void)
{
  ToolkitTestApplication           application;
  auto                             fontClient = TextAbstraction::FontClient::New(96u, 96u);
  TextAbstraction::FontDescription description;
  description.family            = "Test Sans";
  const FontId    fontId        = fontClient.GetFontId(description, 12u * 64u);
  const GlyphInfo ellipsisGlyph = fontClient.GetEllipsisGlyph(fontClient.GetPointSize(fontId));

  // Deliberately non-contiguous runs isolate advance line selection. With 15
  // glyphs the wrong sum stays in bounds but changes the result; with 13 it
  // also reads beyond the glyph buffer. Geometry comes from the local fixture.
  for(Length sourceCount : {15u, 13u})
  {
    auto  model              = TextTest::MakeModel(std::u32string(sourceCount, U'a'));
    auto& visual             = *model->mVisualModel;
    model->mElideEnabled     = true;
    model->mEllipsisPosition = DevelText::EllipsisPosition::END;
    visual.mControlSize      = Size(40.f, 32.f);
    visual.SetLayoutSize(Size(40.f, 32.f));
    for(GlyphIndex index = 0u; index < sourceCount; ++index)
    {
      visual.mGlyphs[index].fontId  = fontId;
      visual.mGlyphs[index].advance = index < 10u ? 1.f : 100.f;
    }
    LineRun lines[3]{};
    lines[0].glyphRun = {0u, 2u};
    lines[1].glyphRun = {5u, 5u};
    lines[2].glyphRun = {10u, 3u};
    lines[1].ellipsis = true;
    visual.mLines.Clear();
    for(const auto& line : lines)
    {
      visual.mLines.PushBack(line);
    }

    const GlyphInfo sourceGlyph    = visual.mGlyphs[9u];
    const Vector2   sourcePosition = visual.mGlyphPositions[9u];
    ViewModel       view(model.Get());
    for(unsigned int repeat = 0u; repeat < 2u; ++repeat)
    {
      view.ElideGlyphs(fontClient);
      DALI_TEST_EQUALS(view.GetNumberOfGlyphs(), 10u, TEST_LOCATION);
      DALI_TEST_EQUALS(view.GetEndIndexOfElidedGlyphs(), 9u, TEST_LOCATION);
      DALI_TEST_CHECK(view.GetEndIndexOfElidedGlyphs() < view.GetNumberOfGlyphs());
      DALI_TEST_EQUALS(view.GetGlyphs()[9u].fontId, ellipsisGlyph.fontId, TEST_LOCATION);
      DALI_TEST_EQUALS(view.GetGlyphs()[9u].index, ellipsisGlyph.index, TEST_LOCATION);
      DALI_TEST_EQUALS(visual.mGlyphs.Count(), sourceCount, TEST_LOCATION);
      DALI_TEST_EQUALS(visual.mGlyphs[9u].index, sourceGlyph.index, TEST_LOCATION);
      DALI_TEST_EQUALS(visual.mGlyphs[9u].advance, sourceGlyph.advance, TEST_LOCATION);
      DALI_TEST_EQUALS(visual.mGlyphPositions[9u], sourcePosition, TEST_LOCATION);
      for(LineIndex index = 0u; index < 3u; ++index)
      {
        DALI_TEST_EQUALS(visual.mLines[index].glyphRun.glyphIndex, lines[index].glyphRun.glyphIndex, TEST_LOCATION);
        DALI_TEST_EQUALS(visual.mLines[index].glyphRun.numberOfGlyphs, lines[index].glyphRun.numberOfGlyphs, TEST_LOCATION);
      }
    }
  }
  END_TEST;
}

int UtcDaliTextEllipsisStartAdvanceCompatibility(void)
{
  ToolkitTestApplication application;
  auto                   controller = Controller::New();
  ConfigureTextLabel(controller);
  auto fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(96u, 96u);
  controller->SetDefaultFontFamily("Test Sans");
  controller->SetDefaultFontSize(12.f, Controller::POINT_SIZE);
  controller->SetMultiLineEnabled(true);
  controller->SetTextElideEnabled(true);
  controller->SetEllipsisPosition(DevelText::EllipsisPosition::START);
  controller->SetHorizontalAlignment(HorizontalAlignment::CENTER);
  controller->SetLineWrapMode(LineWrap::CHARACTER);
  controller->SetText("a\nbb\nccc\ndddd\neeeee");
  controller->Relayout(Size(60.f, 32.f), Dali::LayoutDirection::RIGHT_TO_LEFT);

  const auto model = controller->GetTextModel();
  DALI_TEST_EQUALS(model->GetNumberOfLines(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(model->GetLines()[0u].ellipsis);
  DALI_TEST_EQUALS(model->GetLines()[0u].glyphRun.glyphIndex, 9u, TEST_LOCATION);
  DALI_TEST_EQUALS(model->GetLines()[0u].glyphRun.numberOfGlyphs, 5u, TEST_LOCATION);

  // START layout retains a source suffix. Its legacy advance lookup differs
  // from END; changing it also changes the existing centered output.
  ViewModel view(model);
  for(unsigned int repeat = 0u; repeat < 2u; ++repeat)
  {
    view.ElideGlyphs(fontClient);
    DALI_TEST_EQUALS(view.GetNumberOfGlyphs(), 10u, TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetStartIndexOfElidedGlyphs(), 9u, TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetElidedOffset(), 10.f, TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetGlyphs()[0u].index, 0x2026u, TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetGlyphs()[9u].index, static_cast<GlyphIndex>('e'), TEST_LOCATION);
  }
  END_TEST;
}

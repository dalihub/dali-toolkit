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

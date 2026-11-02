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

#include <cmath>
#include <text-model-fixture.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/text/spannable-string.h>
#include <dali-toolkit/internal/text/controller/text-controller.h>
#include <dali-toolkit/internal/text/font-description-run.h>
#include <dali-toolkit/internal/text/rendering/text-typesetter.h>
#include <dali-toolkit/internal/text/rendering/view-model.h>
#include <toolkit-text-utils.h>

using namespace Dali;
using namespace Toolkit;
using namespace Text;

namespace
{
const std::string DEFAULT_FONT_DIR("/resources/fonts");

const Size  CONTROL_SIZE(200.f, 400.f);
const char* LOREM_IPSUM =
  "Lorem ipsum dolor sit amet, aeque definiebas ea mei, posse iracundia ne cum.\n"
  "Usu ne nisl maiorum iudicabit, veniam epicurei oporteat eos an.\n"
  "Ne nec nulla regione albucius, mea doctus delenit ad!\n"
  "Et everti blandit adversarium mei, eam porro neglegentur suscipiantur an.\n"
  "Quidam corpora at duo. An eos possim scripserit?\n\n"
  "Aťqui dicant sěnťenťíae aň vel!\n"
  "Vis viris médiocrem elaboraret ét, verear civibus moderatius ex duo!\n"
  "Án veri laborě iňtěgré quó, mei aď poššit lobortis, mei prompťa čonsťitůťó eů.\n"
  "Aliquip sanctůs delicáta quí ěá, et natum aliquam est?\n"
  "Asšúm sapěret usu ůť.\n"
  "Síť ut apeirián laboramúš percipitur, sůas hařum ín éos?\n";
const Vector2 LOREM_SCROLL_POSITION(0.f, -265.f);
const Length  LOREM_NUMBER_OF_LINES             = 35u;
const Length  LOREM_NUMBER_OF_LINES_ELIDED      = 21u;
const Length  LOREM_NUMBER_OF_GLYPHS            = 632u;
const Length  LOREM_NUMBER_OF_GLYPHS_ELIDED     = 395u;
const Length  LOREM_NUMBER_OF_CHARACTERS        = 633u;
const Length  LOREM_NUMBER_OF_CHARACTERS_ELIDED = 633u;

// The expected layout size for UtcDaliTextViewModelGetLayoutSize
const Size LAYOUT_SIZE(182.f, 48.f);

// The expected color indices for UtcDaliTextViewModelGetColors
const ColorIndex COLOR_INDICES[]  = {0u, 0u, 0u, 0u, 0u, 0u, 1u, 1u, 1u, 2u, 2u, 2u, 2u, 2u, 1u, 1u, 1u, 1u, 1u, 3u, 1u, 1u, 1u, 0u, 0u, 0u, 0u};
const Length     NUMBER_OF_COLORS = 3u;
const Vector4    COLORS[]         = {Color::RED, Color::BLUE, Color::GREEN};

} // namespace

int UtcDaliTextViewModel(void)
{
  tet_infoline(" UtcDaliTextViewModel");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  // Tests the rendering controller has been created.
  TypesetterPtr typesetter = Typesetter::New(controller->GetTextModel());
  DALI_TEST_CHECK(typesetter);

  // Tests the view model has been created.
  ViewModel* model = typesetter->GetViewModel();
  DALI_TEST_CHECK(NULL != model);

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextViewModelGetControlSize(void)
{
  tet_infoline(" UtcDaliTextViewModelGetControlSize");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  // Tests the rendering controller has been created.
  TypesetterPtr typesetter = Typesetter::New(controller->GetTextModel());
  DALI_TEST_CHECK(typesetter);

  // Tests the view model has been created.
  ViewModel* model = typesetter->GetViewModel();
  DALI_TEST_CHECK(NULL != model);

  // Configures the text controller similarly to the text-editor.
  ConfigureTextEditor(controller);

  // The text has not been laid-out. The stored control's size should be zero.
  DALI_TEST_EQUALS(Size::ZERO, model->GetControlSize(), TEST_LOCATION);

  // Sets a text and relais-out.
  controller->SetText("Hello world");
  controller->Relayout(CONTROL_SIZE);

  // The control's size should be stored now.
  DALI_TEST_EQUALS(CONTROL_SIZE, model->GetControlSize(), TEST_LOCATION);

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextViewModelGetLayoutSize(void)
{
  auto model = TextTest::MakeModel(U"abcdefgh");
  ViewModel view(model.Get());
  DALI_TEST_EQUALS(view.GetLayoutSize(), Vector2(40.f, 40.f), TEST_LOCATION);
  model->mVisualModel->SetLayoutSize(Vector2(70.f, 90.f));
  DALI_TEST_EQUALS(view.GetLayoutSize(), Vector2(70.f, 90.f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextViewModelGetScrollPosition(void)
{
  auto model = TextTest::MakeModel();
  ViewModel view(model.Get());
  DALI_TEST_EQUALS(view.GetScrollPosition(), Vector2::ZERO, TEST_LOCATION);
  model->mScrollPosition = Vector2(-17.f, 9.f);
  DALI_TEST_EQUALS(view.GetScrollPosition(), Vector2(-17.f, 9.f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextViewModelGetAlignment(void)
{
  tet_infoline(" UtcDaliTextViewModelGetAlignment");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  // Tests the rendering controller has been created.
  TypesetterPtr typesetter = Typesetter::New(controller->GetTextModel());
  DALI_TEST_CHECK(typesetter);

  // Tests the view model has been created.
  ViewModel* model = typesetter->GetViewModel();
  DALI_TEST_CHECK(NULL != model);

  DALI_TEST_EQUALS(Text::HorizontalAlignment::BEGIN, model->GetHorizontalAlignment(), TEST_LOCATION);
  DALI_TEST_EQUALS(Text::VerticalAlignment::TOP, model->GetVerticalAlignment(), TEST_LOCATION);

  controller->SetHorizontalAlignment(Text::HorizontalAlignment::CENTER);
  controller->SetVerticalAlignment(Text::VerticalAlignment::CENTER);

  DALI_TEST_EQUALS(Text::HorizontalAlignment::CENTER, model->GetHorizontalAlignment(), TEST_LOCATION);
  DALI_TEST_EQUALS(Text::VerticalAlignment::CENTER, model->GetVerticalAlignment(), TEST_LOCATION);

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextViewModelIsTextElideEnabled(void)
{
  tet_infoline(" UtcDaliTextViewModelIsTextElideEnabled");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  // Tests the rendering controller has been created.
  TypesetterPtr typesetter = Typesetter::New(controller->GetTextModel());
  DALI_TEST_CHECK(typesetter);

  // Tests the view model has been created.
  ViewModel* model = typesetter->GetViewModel();
  DALI_TEST_CHECK(NULL != model);

  // Configures the text controller similarly to the text-editor.
  ConfigureTextEditor(controller);

  // Elide text should be disabled.
  DALI_TEST_CHECK(!model->IsTextElideEnabled());

  // Configures the text controller similarly to the text-label.
  ConfigureTextLabel(controller);

  // Elide text should be enabled.
  DALI_TEST_CHECK(model->IsTextElideEnabled());

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextViewModelGetCharacters(void)
{
  tet_infoline(" UtcDaliTextViewModelGetLines");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  // Tests the rendering controller has been created.
  TypesetterPtr typesetter = Typesetter::New(controller->GetTextModel());
  DALI_TEST_CHECK(typesetter);

  // Tests the view model has been created.
  ViewModel* model = typesetter->GetViewModel();
  DALI_TEST_CHECK(NULL != model);

  // Configures the text controller similarly to the text-editor.
  ConfigureTextEditor(controller);

  // The number of characters should be zero.
  DALI_TEST_EQUALS(0u, model->GetNumberOfCharacters(), TEST_LOCATION);

  // Sets a text and relais-out.
  controller->SetText(LOREM_IPSUM);
  controller->Relayout(CONTROL_SIZE);

  DALI_TEST_EQUALS(LOREM_NUMBER_OF_CHARACTERS, model->GetNumberOfCharacters(), TEST_LOCATION);
  // Configures the text controller similarly to the text-label.
  ConfigureTextLabel(controller);

  // Relais-out for the text-label configuration.
  controller->Relayout(Size(100.f, 100.f)); // Change the size to force a relayout.
  controller->Relayout(CONTROL_SIZE);

  DALI_TEST_EQUALS(LOREM_NUMBER_OF_CHARACTERS_ELIDED, model->GetNumberOfCharacters(), TEST_LOCATION);

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextViewModelGetLines(void)
{
  auto model = TextTest::MakeModel(U"abcdefgh");
  ViewModel view(model.Get());
  DALI_TEST_EQUALS(view.GetNumberOfLines(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetLines()[1].characterRun.characterIndex, 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetLines()[1].ascender, 15.f, TEST_LOCATION);
  model->mVisualModel->mLines.Clear();
  DALI_TEST_EQUALS(view.GetNumberOfLines(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextViewModelGetGlyphsLayout(void)
{
  auto model = TextTest::MakeModel();
  ViewModel view(model.Get());
  DALI_TEST_EQUALS(view.GetNumberOfGlyphs(), 4u, TEST_LOCATION);
  for(unsigned int i = 0u; i < 4u; ++i)
  {
    DALI_TEST_EQUALS(view.GetGlyphs()[i].index, static_cast<GlyphIndex>('a') + i, TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetLayout()[i], Vector2(i * 10.f + 2.f, -15.f), TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliTextViewModelGetColors(void)
{
  tet_infoline(" UtcDaliTextViewModelGetColors");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  // Tests the rendering controller has been created.
  TypesetterPtr typesetter = Typesetter::New(controller->GetTextModel());
  DALI_TEST_CHECK(typesetter);

  // Tests the view model has been created.
  ViewModel* model = typesetter->GetViewModel();
  DALI_TEST_CHECK(NULL != model);

  // Configures the text controller similarly to the text-label.
  ConfigureTextLabel(controller);

  // Sets a text and relais-out.
  controller->SetMarkupProcessorEnabled(true);
  controller->SetText("Lorem <color value='red'>ips<color value='blue'>um do</color>lor s<color value='green'>i</color>t a</color>met.");
  controller->Relayout(CONTROL_SIZE);

  DALI_TEST_EQUALS(Color::BLACK, model->GetDefaultColor(), TEST_LOCATION);

  const ColorIndex* const colorIndicesBuffer = model->GetColorIndices();

  const Length numberOfGlyphs = model->GetNumberOfGlyphs();
  for(ColorIndex index = 0u; index < numberOfGlyphs; ++index)
  {
    DALI_TEST_EQUALS(COLOR_INDICES[index], *(colorIndicesBuffer + index), TEST_LOCATION);
  }

  const Vector4* const colors = model->GetColors();
  for(unsigned int index = 0u; index < NUMBER_OF_COLORS; ++index)
  {
    DALI_TEST_EQUALS(COLORS[index], *(colors + index), TEST_LOCATION);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextViewModelElideText01(void)
{
  ToolkitTestApplication application(480u, 800u, 96.f, 96.f);
  auto client = TextAbstraction::FontClient::Get();
  {
    auto model = TextTest::MakeModel(U"abcdefgh", 8u);
    model->mElideEnabled = true;
    model->mEllipsisPosition = DevelText::EllipsisPosition::END;
    model->mVisualModel->mControlSize = Vector2(40.f, 20.f);
    TextAbstraction::FontDescription font;
    font.family = "Test Sans";
    const auto fontId = client.GetFontId(font, 12u * 64u);
    for(auto& glyph : model->mVisualModel->mGlyphs) glyph.fontId = fontId;
    model->mVisualModel->mLines[0].ellipsis = true;
    auto& line = model->mVisualModel->mLines[0];
    line.glyphRun.numberOfGlyphs = 4u;
    line.characterRun.numberOfCharacters = 4u;
    line.width = 40.f;
    ViewModel view(model.Get());
    view.ElideGlyphs(client);
    DALI_TEST_CHECK(view.GetNumberOfGlyphs() > 0u && view.GetNumberOfGlyphs() <= 8u);
    for(unsigned int i = 0u; i < view.GetNumberOfGlyphs(); ++i)
      DALI_TEST_CHECK(std::isfinite(view.GetLayout()[i].x));
  }
  END_TEST;
}

int UtcDaliTextViewModelElideText02(void)
{
  ToolkitTestApplication application(480u, 800u, 96.f, 96.f);
  auto client = TextAbstraction::FontClient::Get();
  {
    auto model = TextTest::MakeModel(U"abcdefgh", 8u);
    model->mElideEnabled = true;
    model->mEllipsisPosition = DevelText::EllipsisPosition::START;
    model->mVisualModel->mControlSize = Vector2(40.f, 20.f);
    TextAbstraction::FontDescription font;
    font.family = "Test Sans";
    const auto fontId = client.GetFontId(font, 12u * 64u);
    for(auto& glyph : model->mVisualModel->mGlyphs) glyph.fontId = fontId;
    model->mVisualModel->mLines[0].ellipsis = true;
    auto& line = model->mVisualModel->mLines[0];
    line.glyphRun.numberOfGlyphs = 4u;
    line.characterRun.numberOfCharacters = 4u;
    line.width = 40.f;
    ViewModel view(model.Get());
    view.ElideGlyphs(client);
    DALI_TEST_CHECK(view.GetNumberOfGlyphs() > 0u && view.GetNumberOfGlyphs() <= 8u);
    for(unsigned int i = 0u; i < view.GetNumberOfGlyphs(); ++i)
      DALI_TEST_CHECK(std::isfinite(view.GetLayout()[i].x));
  }
  {
    auto model = TextTest::MakeModel(U"abcdefgh", 8u);
    model->mElideEnabled = true;
    model->mEllipsisPosition = DevelText::EllipsisPosition::MIDDLE;
    model->mVisualModel->mControlSize = Vector2(40.f, 20.f);
    TextAbstraction::FontDescription font;
    font.family = "Test Sans";
    const auto fontId = client.GetFontId(font, 12u * 64u);
    for(auto& glyph : model->mVisualModel->mGlyphs) glyph.fontId = fontId;
    model->mVisualModel->mLines[0].ellipsis = true;
    auto& line = model->mVisualModel->mLines[0];
    line.glyphRun.numberOfGlyphs = 4u;
    line.characterRun.numberOfCharacters = 4u;
    line.width = 40.f;
    ViewModel view(model.Get());
    view.ElideGlyphs(client);
    DALI_TEST_CHECK(view.GetNumberOfGlyphs() > 0u && view.GetNumberOfGlyphs() <= 8u);
    for(unsigned int i = 0u; i < view.GetNumberOfGlyphs(); ++i)
      DALI_TEST_CHECK(std::isfinite(view.GetLayout()[i].x));
  }
  {
    auto model = TextTest::MakeModel(U"abcdefgh", 8u);
    model->mElideEnabled = true;
    model->mEllipsisPosition = DevelText::EllipsisPosition::END;
    model->mVisualModel->mControlSize = Vector2(40.f, 20.f);
    TextAbstraction::FontDescription font;
    font.family = "Test Sans";
    const auto fontId = client.GetFontId(font, 12u * 64u);
    for(auto& glyph : model->mVisualModel->mGlyphs) glyph.fontId = fontId;
    model->mVisualModel->mLines[0].ellipsis = true;
    auto& line = model->mVisualModel->mLines[0];
    line.glyphRun.numberOfGlyphs = 4u;
    line.characterRun.numberOfCharacters = 4u;
    line.width = 40.f;
    ViewModel view(model.Get());
    view.ElideGlyphs(client);
    DALI_TEST_CHECK(view.GetNumberOfGlyphs() > 0u && view.GetNumberOfGlyphs() <= 8u);
    for(unsigned int i = 0u; i < view.GetNumberOfGlyphs(); ++i)
      DALI_TEST_CHECK(std::isfinite(view.GetLayout()[i].x));
  }
  END_TEST;
}

int UtcDaliTextViewModelGetFontRuns(void)
{
  tet_infoline(" UtcDaliTextViewModelGetFontRuns");
  ToolkitTestApplication application;

  // Load some fonts.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(93u, 93u);

  char*             pathNamePtr = get_current_dir_name();
  const std::string pathName(pathNamePtr);
  free(pathNamePtr);

  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansRegular.ttf");

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  // Tests the rendering controller has been created.
  TypesetterPtr typesetter = Typesetter::New(controller->GetTextModel());
  DALI_TEST_CHECK(typesetter);

  // Tests the view model has been created.
  ViewModel* model = typesetter->GetViewModel();
  DALI_TEST_CHECK(NULL != model);

  // Configures the text controller similarly to the text-editor.
  ConfigureTextEditor(controller);

  // Sets a text and relais-out.
  controller->SetMarkupProcessorEnabled(true);
  controller->SetText("<font family='TizenSansRegular' size='10'>Hello </font>Hello<font family='TizenSansRegular' size='15'>Hello</font>");
  controller->Relayout(CONTROL_SIZE);

  const Vector<FontRun>& validFonts = model->GetFontRuns();

  // The font-runs should be equal to number of segments have different fonts.
  DALI_TEST_EQUALS(validFonts.Count(), 3u, TEST_LOCATION);

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextViewModelGetFontDescriptionRuns(void)
{
  tet_infoline(" UtcDaliTextViewModelGetFontDescriptionRuns");
  ToolkitTestApplication application;

  // Load some fonts.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(93u, 93u);

  char*             pathNamePtr = get_current_dir_name();
  const std::string pathName(pathNamePtr);
  free(pathNamePtr);

  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansRegular.ttf");

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  // Tests the rendering controller has been created.
  TypesetterPtr typesetter = Typesetter::New(controller->GetTextModel());
  DALI_TEST_CHECK(typesetter);

  // Tests the view model has been created.
  ViewModel* model = typesetter->GetViewModel();
  DALI_TEST_CHECK(NULL != model);

  // Configures the text controller similarly to the text-editor.
  ConfigureTextEditor(controller);

  // Sets a text and relais-out.
  controller->SetMarkupProcessorEnabled(true);
  controller->SetText("<font family='TizenSansRegular' size='10'>Hello </font>Hello<font family='TizenSansRegular' size='15'>Hello</font>");
  controller->Relayout(CONTROL_SIZE);

  const Vector<FontDescriptionRun>& validFonts = model->GetFontDescriptionRuns();

  // The font-description-runs should be equal number of the used fonts.
  DALI_TEST_EQUALS(validFonts.Count(), 2u, TEST_LOCATION);

  tet_result(TET_PASS);
  END_TEST;
}
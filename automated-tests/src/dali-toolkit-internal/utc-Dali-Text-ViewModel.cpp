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
const Size  CONTROL_SMALL_SIZE(50.f, 100.f);
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
const Length  LOREM_NUMBER_OF_GLYPHS_ELIDED     = 393u;
const Length  LOREM_NUMBER_OF_CHARACTERS        = 633u;
const Length  LOREM_NUMBER_OF_CHARACTERS_ELIDED = 633u;

// The expected layout size for UtcDaliTextViewModelGetLayoutSize
const Size LAYOUT_SIZE(182.f, 48.f);

// The expected color indices for UtcDaliTextViewModelGetColors
const ColorIndex COLOR_INDICES[]  = {0u, 0u, 0u, 0u, 0u, 0u, 1u, 1u, 1u, 2u, 2u, 2u, 2u, 2u, 1u, 1u, 1u, 1u, 1u, 3u, 1u, 1u, 1u, 0u, 0u, 0u, 0u};
const Length     NUMBER_OF_COLORS = 3u;
const Vector4    COLORS[]         = {Color::RED, Color::BLUE, Color::GREEN};

struct ElideData
{
  std::string  description;
  std::string  text;
  Vector2      size;
  unsigned int numberOfLines;
  unsigned int numberOfGlyphs;
  float*       positions;
};

bool ElideTest(const ElideData& data)
{
  std::cout << "  testing : " << data.description << std::endl;

  // Load some fonts.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(93u, 93u);

  char*             pathNamePtr = get_current_dir_name();
  const std::string pathName(pathNamePtr);
  free(pathNamePtr);

  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansRegular.ttf");
  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansHebrewRegular.ttf");
  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansArabicRegular.ttf");

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

  controller->SetText(data.text);
  controller->Relayout(data.size);

  // Elide the glyphs.
  model->ElideGlyphs(fontClient);

  if(data.numberOfLines != model->GetNumberOfLines())
  {
    std::cout << "  different number of lines : " << model->GetNumberOfLines() << ", expected : " << data.numberOfLines << std::endl;
    return false;
  }

  if(data.numberOfGlyphs != model->GetNumberOfGlyphs())
  {
    std::cout << "  different number of glyphs : " << model->GetNumberOfGlyphs() << ", expected : " << data.numberOfGlyphs << std::endl;
    return false;
  }

  const Vector2* const layoutBuffer  = model->GetLayout();
  const Length         numberOfLines = model->GetNumberOfLines();

  if(numberOfLines != 0u)
  {
    const LineRun& lastLine               = *(model->GetLines() + numberOfLines - 1u);
    const Length   numberOfLastLineGlyphs = data.numberOfGlyphs - lastLine.glyphRun.glyphIndex;

    std::cout << "  last line alignment offset : " << floor(lastLine.alignmentOffset) << std::endl;

    for(unsigned int index = 0u; index < numberOfLastLineGlyphs; ++index)
    {
      if(*(data.positions + index) != floor(lastLine.alignmentOffset + (*(layoutBuffer + lastLine.glyphRun.glyphIndex + index)).x))
      {
        std::cout << "  different layout :";
        for(unsigned int i = 0; i < numberOfLastLineGlyphs; ++i)
        {
          std::cout << " " << floor(lastLine.alignmentOffset + (*(layoutBuffer + lastLine.glyphRun.glyphIndex + i)).x);
        }
        std::cout << std::endl;
        std::cout << "          expected :";
        for(unsigned int i = 0; i < numberOfLastLineGlyphs; ++i)
        {
          std::cout << " " << *(data.positions + i);
        }
        std::cout << std::endl;
        return false;
      }
    }
  }

  return true;
}

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
  tet_infoline(" UtcDaliTextViewModelGetLayoutSize");
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

  // The text has not been laid-out. The stored control's size should be zero.
  DALI_TEST_EQUALS(Size::ZERO, model->GetLayoutSize(), TEST_LOCATION);

  // Sets a text and relais-out.
  controller->SetMarkupProcessorEnabled(true);
  controller->SetRemoveFrontInset(true);
  controller->SetRemoveBackInset(true);
  controller->SetText("<font family='TizenSansRegular' size='10'>Lorem ipsum dolor sit amet, aeque definiebas ea mei, posse iracundia ne cum.</font>");
  controller->Relayout(CONTROL_SIZE);

  // The control's size should be stored now.
  DALI_TEST_EQUALS(LAYOUT_SIZE, model->GetLayoutSize(), TEST_LOCATION);

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextViewModelGetScrollPosition(void)
{
  tet_infoline(" UtcDaliTextViewModelGetScrollPosition");
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

  // No text has been set. The scroll position should be zero.
  DALI_TEST_EQUALS(Vector2::ZERO, model->GetScrollPosition(), TEST_LOCATION);

  // Gains the keyboard focus, sets a big text and relais-out.
  controller->KeyboardFocusGainEvent();
  controller->SetText(LOREM_IPSUM);
  controller->Relayout(CONTROL_SIZE);

  // The text should be scrolled to the end.
  DALI_TEST_EQUALS(LOREM_SCROLL_POSITION, model->GetScrollPosition(), TEST_LOCATION);

  tet_result(TET_PASS);
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

  // The number of lines should be zero.
  DALI_TEST_EQUALS(0u, model->GetNumberOfLines(), TEST_LOCATION);
  DALI_TEST_CHECK(NULL == model->GetLines());

  // Sets a text and relais-out.
  controller->SetText(LOREM_IPSUM);
  controller->Relayout(CONTROL_SIZE);

  DALI_TEST_EQUALS(LOREM_NUMBER_OF_LINES, model->GetNumberOfLines(), TEST_LOCATION);
  DALI_TEST_CHECK(NULL != model->GetLines());

  // Configures the text controller similarly to the text-label.
  ConfigureTextLabel(controller);

  // Relais-out for the text-label configuration.
  controller->Relayout(Size(100.f, 100.f)); // Change the size to force a relayout.
  controller->Relayout(CONTROL_SIZE);

  DALI_TEST_EQUALS(LOREM_NUMBER_OF_LINES_ELIDED, model->GetNumberOfLines(), TEST_LOCATION);
  DALI_TEST_CHECK(NULL != model->GetLines());

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextViewModelGetGlyphsLayout(void)
{
  tet_infoline(" UtcDaliTextViewModelGetGlyphsLayout");
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

  // The number of glyphs should be zero.
  DALI_TEST_EQUALS(0u, model->GetNumberOfGlyphs(), TEST_LOCATION);
  DALI_TEST_CHECK(NULL == model->GetGlyphs());
  DALI_TEST_CHECK(NULL == model->GetLayout());

  // Sets a text and relais-out.
  controller->SetText(LOREM_IPSUM);
  controller->Relayout(CONTROL_SIZE);

  DALI_TEST_EQUALS(LOREM_NUMBER_OF_GLYPHS, model->GetNumberOfGlyphs(), TEST_LOCATION);
  DALI_TEST_CHECK(NULL != model->GetGlyphs());
  DALI_TEST_CHECK(NULL != model->GetLayout());

  // Configures the text controller similarly to the text-label.
  ConfigureTextLabel(controller);

  // Relais-out for the text-label configuration.
  controller->Relayout(Size(100.f, 100.f)); // Change the size to force a relayout.
  controller->Relayout(CONTROL_SIZE);

  // Elide the glyphs.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(93u, 93u);
  model->ElideGlyphs(fontClient);

  DALI_TEST_EQUALS(LOREM_NUMBER_OF_GLYPHS_ELIDED, model->GetNumberOfGlyphs(), TEST_LOCATION);
  DALI_TEST_CHECK(NULL != model->GetGlyphs());
  DALI_TEST_CHECK(NULL != model->GetLayout());

  tet_result(TET_PASS);
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
  tet_infoline(" UtcDaliTextViewModelElideText01");
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

  // The number of glyphs should be zero.
  DALI_TEST_EQUALS(0u, model->GetNumberOfGlyphs(), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, model->GetNumberOfLines(), TEST_LOCATION);
  DALI_TEST_CHECK(NULL == model->GetGlyphs());
  DALI_TEST_CHECK(NULL == model->GetLayout());

  // Sets a text and relais-out.
  controller->SetText(LOREM_IPSUM);
  controller->Relayout(CONTROL_SIZE);

  // Keep the pointers to the glyphs and layout.
  // As the text is not elided with this configuration, the pointers shoud be the same after calling the ElideGlyphs() method.
  const GlyphInfo* const glyphsModel  = model->GetGlyphs();
  const Vector2*         layoutsModel = model->GetLayout();

  // Elide the glyphs. Text shouldn't be elided with this configuration.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(93u, 93u);
  model->ElideGlyphs(fontClient);

  DALI_TEST_CHECK(glyphsModel == model->GetGlyphs());
  DALI_TEST_CHECK(layoutsModel == model->GetLayout());

  DALI_TEST_EQUALS(LOREM_NUMBER_OF_GLYPHS, model->GetNumberOfGlyphs(), TEST_LOCATION);
  DALI_TEST_EQUALS(LOREM_NUMBER_OF_LINES, model->GetNumberOfLines(), TEST_LOCATION);

  // Configures the text controller similarly to the text-label.
  ConfigureTextLabel(controller);

  // Clear the text and relais-out.
  controller->SetText("");
  controller->Relayout(CONTROL_SIZE);

  DALI_TEST_EQUALS(0u, model->GetNumberOfGlyphs(), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, model->GetNumberOfLines(), TEST_LOCATION);

  // Elide the glyphs. Should not add the ellipsis glyph.
  model->ElideGlyphs(fontClient);

  DALI_TEST_EQUALS(0u, model->GetNumberOfGlyphs(), TEST_LOCATION);

  // Sets a text that doesn't need to be elided.
  controller->SetText("Hello\n");
  controller->Relayout(CONTROL_SIZE);

  // Elide the glyphs.
  model->ElideGlyphs(fontClient);

  DALI_TEST_EQUALS(6u, model->GetNumberOfGlyphs(), TEST_LOCATION);
  DALI_TEST_EQUALS(2u, model->GetNumberOfLines(), TEST_LOCATION);

  // Sets a text and relais-out.
  controller->SetText(LOREM_IPSUM);
  controller->Relayout(CONTROL_SIZE);

  // Elide the glyphs.
  model->ElideGlyphs(fontClient);

  DALI_TEST_EQUALS(LOREM_NUMBER_OF_GLYPHS_ELIDED, model->GetNumberOfGlyphs(), TEST_LOCATION);
  DALI_TEST_EQUALS(LOREM_NUMBER_OF_LINES_ELIDED, model->GetNumberOfLines(), TEST_LOCATION);
  const GlyphInfo* const glyphs  = model->GetGlyphs();
  const Vector2*         layouts = model->GetLayout();
  DALI_TEST_CHECK(NULL != glyphs);
  DALI_TEST_CHECK(NULL != layouts);

  // When the ellipsis is enabled, at least a glyph has to be rendered.
  // Even if the given width is too narrow for rendering an ellipsis glyph.
  controller->SetText("…");
  Vector3 sizeEllipsis = controller->GetNaturalSize();
  controller->SetText("A");
  Vector3 sizeA     = controller->GetNaturalSize();
  float   testWidth = sizeA.width < sizeEllipsis.width ? sizeA.width : sizeEllipsis.width - 1.0;

  controller->SetText("AB");
  Vector3 sizeAB = controller->GetNaturalSize();

  controller->Relayout(Size(testWidth, sizeAB.height));

  // Elide the glyphs.
  model->ElideGlyphs(fontClient);
  DALI_TEST_EQUALS(1u, model->GetNumberOfGlyphs(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, model->GetNumberOfLines(), TEST_LOCATION);

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextViewModelElideText02(void)
{
  tet_infoline(" UtcDaliTextViewModelElideText02");

  Size textSize00(100.f, 100.f);

  Size  textSize01(80.f, 100.f);
  float positions01[] = {0.0f, 8.0f, 16.0f, 26.0f, 34.0f, 42.0f, 46.0f, 56.0f, 64.0f};

  Size  textSize02(80.f, 100.f);
  float positions02[] = {69.0f, 63.0f, 59.0f, 51.0f, 47.0f, 43.0f, 34.0f, 26.0f, 7.0f};

  Size  textSize03(80.f, 100.f);
  float positions03[] = {78.0f, 72.0f, 66.0f, 62.0f, 57.0f, 50.0f, 45.0f, 41.0f, 39.0f, 33.0f, 29.0f, 23.0f, 3.0f};

  Size  textSize04(80.f, 10.f);
  float positions04[] = {1.f};

  struct ElideData data[] =
    {
      {"void text",
       "",
       textSize00,
       0u,
       0u,
       NULL},
      {"Latin script",
       "<font family='TizenSans'>Lorem ipsum dolor sit amet, aeque definiebas ea mei, posse iracundia ne cum.</font>",
       textSize01,
       5u,
       36u,
       positions01},
      {"Hebrew script",
       "<font family='TizenSansHebrew'>צעד על לשון המלצת לאחרונה, אם לכאן שנורו סרבול מדע, קרן דת שפות להפוך.</font>",
       textSize02,
       5u,
       44u,
       positions02},
      {"Arabic script",
       "<font family='TizenSansArabic'>عل النفط ديسمبر الإمداد بال, بين وترك شعار هو. لمّ من المبرمة النفط بالسيطرة, أم يتم تحرّك وبغطاء, عدم في لإعادة وإقامة رجوعهم.</font>",
       textSize03,
       5u,
       66u,
       positions03},
      {"Small control size, no line fits.",
       "<font family='TizenSans'>Lorem ipsum dolor sit amet, aeque definiebas ea mei, posse iracundia ne cum.</font>",
       textSize04,
       1u,
       1u,
       positions04}};
  const unsigned int numberOfTests = 5u;

  for(unsigned int index = 0u; index < numberOfTests; ++index)
  {
    ToolkitTestApplication application;
    if(!ElideTest(data[index]))
    {
      tet_result(TET_FAIL);
    }
  }

  tet_result(TET_PASS);
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
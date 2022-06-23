/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <limits>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/text/bitmap-font.h>
#include <dali-toolkit/devel-api/text/text-enumerations-devel.h>
#include <dali-toolkit/internal/text/rendering/text-typesetter.h>
#include <dali-toolkit/internal/text/rendering/view-model.h>
#include <dali-toolkit/internal/text/text-controller.h>
#include <dali/devel-api/text-abstraction/bitmap-font.h>
#include <toolkit-environment-variable.h>
#include <toolkit-text-utils.h>

using namespace Dali;
using namespace Toolkit;
using namespace Text;

namespace
{
const std::string     DEFAULT_FONT_DIR("/resources/fonts");
const PointSize26Dot6 EMOJI_FONT_SIZE = 3840u; // 60 * 64

constexpr auto DALI_RENDERED_GLYPH_COMPRESS_POLICY = "DALI_RENDERED_GLYPH_COMPRESS_POLICY";
} // namespace

int UtcDaliTextTypesetter(void)
{
  tet_infoline(" UtcDaliTextTypesetter");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  // Tests the rendering controller has been created.
  TypesetterPtr typesetter = Typesetter::New(controller->GetTextModel());
  DALI_TEST_CHECK(typesetter);

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextTypesetterGetViewModel(void)
{
  tet_infoline(" UtcDaliTextTypesetter");
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

int UtcDaliTextRenderingControllerRenderRGBA(void)
{
  tet_infoline(" UtcDaliTextRenderingControllerRenderRGBA");
  ToolkitTestApplication application;

  // Load some fonts.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();

  char*             pathNamePtr = get_current_dir_name();
  const std::string pathName(pathNamePtr);
  free(pathNamePtr);

  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/tizen/BreezeColorEmoji.ttf", EMOJI_FONT_SIZE);
  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansRegular.ttf");

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  // Configures the text controller similarly to the text-label.
  ConfigureTextLabel(controller);

  // Sets the text.
  controller->SetMarkupProcessorEnabled(true);
  controller->SetText("<font family='TizenSansRegular'>Hello world </font><font family='BreezeColorEmoji'>\xF0\x9F\x98\x81</font>");

  // Creates the text's model and relais-out the text.
  const Size relayoutSize(120.f, 60.f);
  controller->Relayout(relayoutSize);

  // Tests the rendering controller has been created.
  TypesetterPtr renderingController = Typesetter::New(controller->GetTextModel());
  DALI_TEST_CHECK(renderingController);

  // Renders the text and creates the final bitmap.
  PixelData bitmap = renderingController->Render(relayoutSize, Toolkit::DevelText::TextDirection::LEFT_TO_RIGHT);
  DALI_TEST_CHECK(bitmap);

  DALI_TEST_EQUALS(120u, bitmap.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(60u, bitmap.GetHeight(), TEST_LOCATION);
  DALI_TEST_EQUALS(Pixel::RGBA8888, bitmap.GetPixelFormat(), TEST_LOCATION);

  // Changes vertical alignment.
  controller->SetVerticalAlignment(Text::VerticalAlignment::CENTER);
  controller->Relayout(relayoutSize);

  // Renders the text and creates the final bitmap.
  bitmap = renderingController->Render(relayoutSize, Toolkit::DevelText::TextDirection::LEFT_TO_RIGHT);
  DALI_TEST_CHECK(bitmap);

  DALI_TEST_EQUALS(120u, bitmap.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(60u, bitmap.GetHeight(), TEST_LOCATION);
  DALI_TEST_EQUALS(Pixel::RGBA8888, bitmap.GetPixelFormat(), TEST_LOCATION);

  controller->SetVerticalAlignment(Text::VerticalAlignment::BOTTOM);
  controller->Relayout(relayoutSize);

  // Renders the text and creates the final bitmap.
  bitmap = renderingController->Render(relayoutSize, Toolkit::DevelText::TextDirection::LEFT_TO_RIGHT);
  DALI_TEST_CHECK(bitmap);

  DALI_TEST_EQUALS(120u, bitmap.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(60u, bitmap.GetHeight(), TEST_LOCATION);
  DALI_TEST_EQUALS(Pixel::RGBA8888, bitmap.GetPixelFormat(), TEST_LOCATION);

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextRenderingControllerRenderLuminance(void)
{
  tet_infoline(" UtcDaliTextRenderingControllerRenderLuminance");
  ToolkitTestApplication application;

  // Load some fonts.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();

  char*             pathNamePtr = get_current_dir_name();
  const std::string pathName(pathNamePtr);
  free(pathNamePtr);

  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/tizen/BreezeColorEmoji.ttf", EMOJI_FONT_SIZE);
  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansRegular.ttf");

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  // Configures the text controller similarly to the text-label.
  ConfigureTextLabel(controller);

  // Sets the text.
  controller->SetMarkupProcessorEnabled(true);
  controller->SetText("<font family='TizenSansRegular'>Hello world </font><font family='BreezeColorEmoji'>\xF0\x9F\x98\x81</font>");

  // Creates the text's model and relais-out the text.
  const Size relayoutSize(120.f, 60.f);
  controller->Relayout(relayoutSize);

  // Tests the rendering controller has been created.
  TypesetterPtr renderingController = Typesetter::New(controller->GetTextModel());
  DALI_TEST_CHECK(renderingController);

  // Renders the text and creates the final bitmap.
  PixelData bitmap = renderingController->Render(relayoutSize, Toolkit::DevelText::TextDirection::LEFT_TO_RIGHT, Typesetter::RENDER_TEXT_AND_STYLES, false, Pixel::L8);
  DALI_TEST_CHECK(bitmap);

  DALI_TEST_EQUALS(120u, bitmap.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(60u, bitmap.GetHeight(), TEST_LOCATION);
  DALI_TEST_EQUALS(Pixel::L8, bitmap.GetPixelFormat(), TEST_LOCATION);

  // Changes vertical alignment.
  controller->SetVerticalAlignment(Text::VerticalAlignment::CENTER);
  controller->Relayout(relayoutSize);

  // Renders the text and creates the final bitmap.
  bitmap = renderingController->Render(relayoutSize, Toolkit::DevelText::TextDirection::LEFT_TO_RIGHT, Typesetter::RENDER_TEXT_AND_STYLES, false, Pixel::L8);
  DALI_TEST_CHECK(bitmap);

  DALI_TEST_EQUALS(120u, bitmap.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(60u, bitmap.GetHeight(), TEST_LOCATION);
  DALI_TEST_EQUALS(Pixel::L8, bitmap.GetPixelFormat(), TEST_LOCATION);

  controller->SetVerticalAlignment(Text::VerticalAlignment::BOTTOM);
  controller->Relayout(relayoutSize);

  // Renders the text and creates the final bitmap.
  bitmap = renderingController->Render(relayoutSize, Toolkit::DevelText::TextDirection::LEFT_TO_RIGHT, Typesetter::RENDER_TEXT_AND_STYLES, false, Pixel::L8);
  DALI_TEST_CHECK(bitmap);

  DALI_TEST_EQUALS(120u, bitmap.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(60u, bitmap.GetHeight(), TEST_LOCATION);
  DALI_TEST_EQUALS(Pixel::L8, bitmap.GetPixelFormat(), TEST_LOCATION);

  tet_result(TET_PASS);

  END_TEST;
}

int UtcDaliTextRenderingControllerRenderWithCompressRGBA(void)
{
  EnvironmentVariable::SetTestEnvironmentVariable(DALI_RENDERED_GLYPH_COMPRESS_POLICY, "m");

  tet_infoline(" UtcDaliTextRenderingControllerRenderWithCompressRGBA");
  ToolkitTestApplication application;

  // Load some fonts.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();

  char*             pathNamePtr = get_current_dir_name();
  const std::string pathName(pathNamePtr);
  free(pathNamePtr);

  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/tizen/BreezeColorEmoji.ttf", EMOJI_FONT_SIZE);
  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansRegular.ttf");

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  // Configures the text controller similarly to the text-label.
  ConfigureTextLabel(controller);

  // Sets the text.
  controller->SetMarkupProcessorEnabled(true);
  controller->SetText("<font family='TizenSansRegular'>Hello world </font><font family='BreezeColorEmoji'>\xF0\x9F\x98\x81</font>");

  // Creates the text's model and relais-out the text.
  const Size relayoutSize(120.f, 60.f);
  controller->Relayout(relayoutSize);

  // Tests the rendering controller has been created.
  TypesetterPtr renderingController = Typesetter::New(controller->GetTextModel());
  DALI_TEST_CHECK(renderingController);

  // Renders the text and creates the final bitmap.
  PixelData bitmap = renderingController->Render(relayoutSize, Toolkit::DevelText::TextDirection::LEFT_TO_RIGHT);
  DALI_TEST_CHECK(bitmap);

  DALI_TEST_EQUALS(120u, bitmap.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(60u, bitmap.GetHeight(), TEST_LOCATION);
  DALI_TEST_EQUALS(Pixel::RGBA8888, bitmap.GetPixelFormat(), TEST_LOCATION);

  // Changes vertical alignment.
  controller->SetVerticalAlignment(Text::VerticalAlignment::CENTER);
  controller->Relayout(relayoutSize);

  // Renders the text and creates the final bitmap.
  bitmap = renderingController->Render(relayoutSize, Toolkit::DevelText::TextDirection::LEFT_TO_RIGHT);
  DALI_TEST_CHECK(bitmap);

  DALI_TEST_EQUALS(120u, bitmap.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(60u, bitmap.GetHeight(), TEST_LOCATION);
  DALI_TEST_EQUALS(Pixel::RGBA8888, bitmap.GetPixelFormat(), TEST_LOCATION);

  controller->SetVerticalAlignment(Text::VerticalAlignment::BOTTOM);
  controller->Relayout(relayoutSize);

  // Renders the text and creates the final bitmap.
  bitmap = renderingController->Render(relayoutSize, Toolkit::DevelText::TextDirection::LEFT_TO_RIGHT);
  DALI_TEST_CHECK(bitmap);

  DALI_TEST_EQUALS(120u, bitmap.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(60u, bitmap.GetHeight(), TEST_LOCATION);
  DALI_TEST_EQUALS(Pixel::RGBA8888, bitmap.GetPixelFormat(), TEST_LOCATION);

  tet_result(TET_PASS);

  END_TEST;
}

int UtcDaliTextRenderingControllerRenderWithCompressLuminance(void)
{
  EnvironmentVariable::SetTestEnvironmentVariable(DALI_RENDERED_GLYPH_COMPRESS_POLICY, "m");

  tet_infoline(" UtcDaliTextRenderingControllerRenderWithCompressLuminance");
  ToolkitTestApplication application;

  // Load some fonts.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();

  char*             pathNamePtr = get_current_dir_name();
  const std::string pathName(pathNamePtr);
  free(pathNamePtr);

  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/tizen/BreezeColorEmoji.ttf", EMOJI_FONT_SIZE);
  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansRegular.ttf");

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  // Configures the text controller similarly to the text-label.
  ConfigureTextLabel(controller);

  // Sets the text.
  controller->SetMarkupProcessorEnabled(true);
  controller->SetText("<font family='TizenSansRegular'>Hello world </font><font family='BreezeColorEmoji'>\xF0\x9F\x98\x81</font>");

  // Creates the text's model and relais-out the text.
  const Size relayoutSize(120.f, 60.f);
  controller->Relayout(relayoutSize);

  // Tests the rendering controller has been created.
  TypesetterPtr renderingController = Typesetter::New(controller->GetTextModel());
  DALI_TEST_CHECK(renderingController);

  // Renders the text and creates the final bitmap.
  PixelData bitmap = renderingController->Render(relayoutSize, Toolkit::DevelText::TextDirection::LEFT_TO_RIGHT, Typesetter::RENDER_TEXT_AND_STYLES, false, Pixel::L8);
  DALI_TEST_CHECK(bitmap);

  DALI_TEST_EQUALS(120u, bitmap.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(60u, bitmap.GetHeight(), TEST_LOCATION);
  DALI_TEST_EQUALS(Pixel::L8, bitmap.GetPixelFormat(), TEST_LOCATION);

  // Changes vertical alignment.
  controller->SetVerticalAlignment(Text::VerticalAlignment::CENTER);
  controller->Relayout(relayoutSize);

  // Renders the text and creates the final bitmap.
  bitmap = renderingController->Render(relayoutSize, Toolkit::DevelText::TextDirection::LEFT_TO_RIGHT, Typesetter::RENDER_TEXT_AND_STYLES, false, Pixel::L8);
  DALI_TEST_CHECK(bitmap);

  DALI_TEST_EQUALS(120u, bitmap.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(60u, bitmap.GetHeight(), TEST_LOCATION);
  DALI_TEST_EQUALS(Pixel::L8, bitmap.GetPixelFormat(), TEST_LOCATION);

  controller->SetVerticalAlignment(Text::VerticalAlignment::BOTTOM);
  controller->Relayout(relayoutSize);

  // Renders the text and creates the final bitmap.
  bitmap = renderingController->Render(relayoutSize, Toolkit::DevelText::TextDirection::LEFT_TO_RIGHT, Typesetter::RENDER_TEXT_AND_STYLES, false, Pixel::L8);
  DALI_TEST_CHECK(bitmap);

  DALI_TEST_EQUALS(120u, bitmap.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(60u, bitmap.GetHeight(), TEST_LOCATION);
  DALI_TEST_EQUALS(Pixel::L8, bitmap.GetPixelFormat(), TEST_LOCATION);

  tet_result(TET_PASS);

  END_TEST;
}

int UtcDaliTextTypesetterVerticalLineAlignment(void)
{
  tet_infoline(" UtcDaliTextTypesetter");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  // Configures the text controller similarly to the text-label.
  ConfigureTextLabel(controller);

  // Sets the text.
  controller->SetMarkupProcessorEnabled(true);
  controller->SetText("<font family='TizenSansRegular'>Hello world</font>");

  // Creates the text's model and relais-out the text.
  const Size relayoutSize(120.f, 60.f);
  controller->Relayout(relayoutSize);

  // Tests the rendering controller has been created.
  TypesetterPtr renderingController = Typesetter::New(controller->GetTextModel());
  DALI_TEST_CHECK(renderingController);

  {
    controller->SetVerticalLineAlignment(Dali::Toolkit::DevelText::VerticalLineAlignment::TOP);
    controller->Relayout(relayoutSize);

    // Renders the text and creates the final bitmap.
    auto bitmap = renderingController->Render(relayoutSize, Toolkit::DevelText::TextDirection::LEFT_TO_RIGHT);
    DALI_TEST_EQUALS(60u, bitmap.GetHeight(), TEST_LOCATION);
  }

  {
    controller->SetVerticalLineAlignment(Dali::Toolkit::DevelText::VerticalLineAlignment::MIDDLE);
    controller->Relayout(relayoutSize);

    // Renders the text and creates the final bitmap.
    auto bitmap = renderingController->Render(relayoutSize, Toolkit::DevelText::TextDirection::LEFT_TO_RIGHT);
    DALI_TEST_EQUALS(60u, bitmap.GetHeight(), TEST_LOCATION);
  }

  {
    controller->SetVerticalLineAlignment(Dali::Toolkit::DevelText::VerticalLineAlignment::BOTTOM);
    controller->Relayout(relayoutSize);

    // Renders the text and creates the final bitmap.
    auto bitmap = renderingController->Render(relayoutSize, Toolkit::DevelText::TextDirection::LEFT_TO_RIGHT);
    DALI_TEST_EQUALS(60u, bitmap.GetHeight(), TEST_LOCATION);
  }

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextTypesetterBitmapFont(void)
{
  tet_infoline("UtcDaliTextTypesetterBitmapFont ");
  ToolkitTestApplication application;

  DevelText::BitmapFontDescription fontDescription;
  fontDescription.name               = "Digits";
  fontDescription.underlinePosition  = 0.f;
  fontDescription.underlineThickness = 0.f;
  fontDescription.isColorFont        = true;

  fontDescription.glyphs.push_back({TEST_RESOURCE_DIR "/fonts/bitmap/u0031.png", "0", 34.f, 0.f});
  fontDescription.glyphs.push_back({TEST_RESOURCE_DIR "/fonts/bitmap/u0032.png", "1", 34.f, 0.f});

  TextAbstraction::BitmapFont bitmapFont;
  DevelText::CreateBitmapFont(fontDescription, bitmapFont);

  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.GetFontId(bitmapFont);

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  // Configures the text controller similarly to the text-label.
  ConfigureTextLabel(controller);

  // Sets the text.
  controller->SetMarkupProcessorEnabled(true);
  controller->SetText("<font family='Digits'><color 'value'='red'>0</color></font>");

  // Creates the text's model and relais-out the text.
  const Size relayoutSize(31.f, 34.f);
  controller->Relayout(relayoutSize);

  // Tests the rendering controller has been created.
  TypesetterPtr renderingController = Typesetter::New(controller->GetTextModel());
  DALI_TEST_CHECK(renderingController);

  controller->Relayout(relayoutSize);

  // Renders the text and creates the final bitmap.
  auto bitmap = renderingController->Render(relayoutSize, Toolkit::DevelText::TextDirection::LEFT_TO_RIGHT);

  DALI_TEST_EQUALS(31u, bitmap.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(34u, bitmap.GetHeight(), TEST_LOCATION);

  tet_result(TET_PASS);
  END_TEST;
}

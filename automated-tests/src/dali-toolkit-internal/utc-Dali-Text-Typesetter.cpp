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
#include <limits>
#include <unistd.h>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <toolkit-text-utils.h>
#include <dali-toolkit/internal/text/rendering/text-typesetter.h>
#include <dali-toolkit/internal/text/rendering/view-model.h>
#include <dali-toolkit/internal/text/text-controller.h>

using namespace Dali;
using namespace Toolkit;
using namespace Text;

namespace
{
const std::string DEFAULT_FONT_DIR( "/resources/fonts" );
const PointSize26Dot6 EMOJI_FONT_SIZE = 3840u; // 60 * 64
} // namespace

int UtcDaliTextTypesetter(void)
{
  tet_infoline(" UtcDaliTextTypesetter");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  // Tests the rendering controller has been created.
  TypesetterPtr typesetter = Typesetter::New( controller->GetTextModel() );
  DALI_TEST_CHECK( typesetter );

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
  TypesetterPtr typesetter = Typesetter::New( controller->GetTextModel() );
  DALI_TEST_CHECK( typesetter );

  // Tests the view model has been created.
  ViewModel* model = typesetter->GetViewModel();
  DALI_TEST_CHECK( NULL != model );

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextRenderingControllerRender(void)
{
  tet_infoline(" UtcDaliTextRenderingControllerRender");
  ToolkitTestApplication application;

  // Load some fonts.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();

  char* pathNamePtr = get_current_dir_name();
  const std::string pathName( pathNamePtr );
  free( pathNamePtr );

  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/BreezeColorEmoji.ttf", EMOJI_FONT_SIZE );
  fontClient.GetFontId( pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansRegular.ttf" );

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  // Configures the text controller similarly to the text-label.
  ConfigureTextLabel( controller );

  // Sets the text.
  controller->SetMarkupProcessorEnabled( true );
  controller->SetText( "<font family='TizenSansRegular'>Hello world </font><font family='BreezeColorEmoji'>\xF0\x9F\x98\x81</font>" );

  // Creates the text's model and relais-out the text.
  const Size relayoutSize( 120.f, 60.f );
  controller->Relayout( relayoutSize );

  // Tests the rendering controller has been created.
  TypesetterPtr renderingController = Typesetter::New( controller->GetTextModel() );
  DALI_TEST_CHECK( renderingController );

  // Renders the text and creates the final bitmap.
  PixelData bitmap = renderingController->Render( relayoutSize );
  DALI_TEST_CHECK( bitmap );

  DALI_TEST_EQUALS( 120u, bitmap.GetWidth(), TEST_LOCATION );
  DALI_TEST_EQUALS( 60u, bitmap.GetHeight(), TEST_LOCATION );
  DALI_TEST_EQUALS( Pixel::RGBA8888, bitmap.GetPixelFormat(), TEST_LOCATION );

  // Changes vertical alignment.
  controller->SetVerticalAlignment( Layout::VERTICAL_ALIGN_CENTER );
  controller->Relayout( relayoutSize );

  // Renders the text and creates the final bitmap.
  bitmap = renderingController->Render( relayoutSize );
  DALI_TEST_CHECK( bitmap );

  DALI_TEST_EQUALS( 120u, bitmap.GetWidth(), TEST_LOCATION );
  DALI_TEST_EQUALS( 60u, bitmap.GetHeight(), TEST_LOCATION );
  DALI_TEST_EQUALS( Pixel::RGBA8888, bitmap.GetPixelFormat(), TEST_LOCATION );

  controller->SetVerticalAlignment( Layout::VERTICAL_ALIGN_BOTTOM );
  controller->Relayout( relayoutSize );

  // Renders the text and creates the final bitmap.
  bitmap = renderingController->Render( relayoutSize );
  DALI_TEST_CHECK( bitmap );

  DALI_TEST_EQUALS( 120u, bitmap.GetWidth(), TEST_LOCATION );
  DALI_TEST_EQUALS( 60u, bitmap.GetHeight(), TEST_LOCATION );
  DALI_TEST_EQUALS( Pixel::RGBA8888, bitmap.GetPixelFormat(), TEST_LOCATION );

  tet_result(TET_PASS);
  END_TEST;
}

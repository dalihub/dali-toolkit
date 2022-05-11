/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/line-helper-functions.h>
#include <dali-toolkit/internal/text/rendering/text-typesetter.h>
#include <dali-toolkit/internal/text/rendering/view-model.h>
#include <dali-toolkit/internal/text/text-controller.h>
#include <toolkit-text-utils.h>

using namespace Dali;
using namespace Toolkit;
using namespace Text;

int UtcDaliGetPreOffsetVerticalLineAlignmentWithNegativeLineSpacing(void)
{
  tet_infoline(" UtcDaliGetPreOffsetVerticalLineAlignmentWithNegativeLineSpacing ");
  ToolkitTestApplication application;

  uint32_t expectedNumberOfLines = 2u;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  // Configures the text controller similarly to the text-label.
  ConfigureTextLabel(controller);

  // Sets the text.
  controller->SetMarkupProcessorEnabled(true);
  controller->SetTextElideEnabled(false);
  controller->SetText("<p rel-line-height=0.5>Line one Line two</p>");

  // Creates the text's model and relais-out the text.
  const Size relayoutSize(120.f, 100.f);
  controller->Relayout(relayoutSize);

  // Tests the rendering controller has been created.
  TypesetterPtr typesetter = Typesetter::New(controller->GetTextModel());
  DALI_TEST_CHECK(typesetter);

  // Tests the view model has been created.
  ViewModel* model = typesetter->GetViewModel();
  DALI_TEST_CHECK(model);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(model->GetNumberOfLines(), expectedNumberOfLines, TEST_LOCATION);
  DALI_TEST_CHECK(model->GetLines());

  const LineRun& lineOne = *(model->GetLines() + 0u);
  const LineRun& lineTwo = *(model->GetLines() + 1u);

  DALI_TEST_EQUALS(GetPreOffsetVerticalLineAlignment(lineOne, Dali::Toolkit::DevelText::VerticalLineAlignment::TOP), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPreOffsetVerticalLineAlignment(lineOne, Dali::Toolkit::DevelText::VerticalLineAlignment::MIDDLE), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPreOffsetVerticalLineAlignment(lineOne, Dali::Toolkit::DevelText::VerticalLineAlignment::BOTTOM), 0.0f, TEST_LOCATION);

  DALI_TEST_EQUALS(GetPreOffsetVerticalLineAlignment(lineTwo, Dali::Toolkit::DevelText::VerticalLineAlignment::TOP), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPreOffsetVerticalLineAlignment(lineTwo, Dali::Toolkit::DevelText::VerticalLineAlignment::MIDDLE), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPreOffsetVerticalLineAlignment(lineTwo, Dali::Toolkit::DevelText::VerticalLineAlignment::BOTTOM), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGetPreOffsetVerticalLineAlignmentWithPositiveLineSpacing(void)
{
  tet_infoline(" UtcDaliGetPreOffsetVerticalLineAlignmentWithPositiveLineSpacing ");
  ToolkitTestApplication application;

  uint32_t expectedNumberOfLines = 2u;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  // Configures the text controller similarly to the text-label.
  ConfigureTextLabel(controller);

  // Sets the text.
  controller->SetMarkupProcessorEnabled(true);
  controller->SetTextElideEnabled(false);
  controller->SetText("<p rel-line-height=2.0>Line one Line two</p>");

  // Creates the text's model and relais-out the text.
  const Size relayoutSize(120.f, 100.f);
  controller->Relayout(relayoutSize);

  // Tests the rendering controller has been created.
  TypesetterPtr typesetter = Typesetter::New(controller->GetTextModel());
  DALI_TEST_CHECK(typesetter);

  // Tests the view model has been created.
  ViewModel* model = typesetter->GetViewModel();
  DALI_TEST_CHECK(model);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(model->GetNumberOfLines(), expectedNumberOfLines, TEST_LOCATION);
  DALI_TEST_CHECK(model->GetLines());

  const LineRun& lineOne = *(model->GetLines() + 0u);
  const LineRun& lineTwo = *(model->GetLines() + 1u);

  DALI_TEST_EQUALS(GetPreOffsetVerticalLineAlignment(lineOne, Dali::Toolkit::DevelText::VerticalLineAlignment::TOP), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPreOffsetVerticalLineAlignment(lineOne, Dali::Toolkit::DevelText::VerticalLineAlignment::MIDDLE), 9.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPreOffsetVerticalLineAlignment(lineOne, Dali::Toolkit::DevelText::VerticalLineAlignment::BOTTOM), 19.0f, TEST_LOCATION);

  DALI_TEST_EQUALS(GetPreOffsetVerticalLineAlignment(lineTwo, Dali::Toolkit::DevelText::VerticalLineAlignment::TOP), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPreOffsetVerticalLineAlignment(lineTwo, Dali::Toolkit::DevelText::VerticalLineAlignment::MIDDLE), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPreOffsetVerticalLineAlignment(lineTwo, Dali::Toolkit::DevelText::VerticalLineAlignment::BOTTOM), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGetPostOffsetVerticalLineAlignmentWithNegativeLineSpacing(void)
{
  tet_infoline(" UtcDaliGetPostOffsetVerticalLineAlignmentWithNegativeLineSpacing ");
  ToolkitTestApplication application;

  uint32_t expectedNumberOfLines = 2u;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  // Configures the text controller similarly to the text-label.
  ConfigureTextLabel(controller);

  // Sets the text.
  controller->SetMarkupProcessorEnabled(true);
  controller->SetTextElideEnabled(false);
  controller->SetText("<p rel-line-height=0.5>Line one Line two</p>");

  // Creates the text's model and relais-out the text.
  const Size relayoutSize(120.f, 100.f);
  controller->Relayout(relayoutSize);

  // Tests the rendering controller has been created.
  TypesetterPtr typesetter = Typesetter::New(controller->GetTextModel());
  DALI_TEST_CHECK(typesetter);

  // Tests the view model has been created.
  ViewModel* model = typesetter->GetViewModel();
  DALI_TEST_CHECK(model);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(model->GetNumberOfLines(), expectedNumberOfLines, TEST_LOCATION);
  DALI_TEST_CHECK(model->GetLines());

  const LineRun& lineOne = *(model->GetLines() + 0u);
  const LineRun& lineTwo = *(model->GetLines() + 1u);

  DALI_TEST_EQUALS(GetPostOffsetVerticalLineAlignment(lineOne, Dali::Toolkit::DevelText::VerticalLineAlignment::TOP), -9.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPostOffsetVerticalLineAlignment(lineOne, Dali::Toolkit::DevelText::VerticalLineAlignment::MIDDLE), -9.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPostOffsetVerticalLineAlignment(lineOne, Dali::Toolkit::DevelText::VerticalLineAlignment::BOTTOM), -9.5f, TEST_LOCATION);

  DALI_TEST_EQUALS(GetPostOffsetVerticalLineAlignment(lineTwo, Dali::Toolkit::DevelText::VerticalLineAlignment::TOP), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPostOffsetVerticalLineAlignment(lineTwo, Dali::Toolkit::DevelText::VerticalLineAlignment::MIDDLE), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPostOffsetVerticalLineAlignment(lineTwo, Dali::Toolkit::DevelText::VerticalLineAlignment::BOTTOM), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGetPostOffsetVerticalLineAlignmentWithPositiveLineSpacing(void)
{
  tet_infoline(" UtcDaliGetPostOffsetVerticalLineAlignmentWithPositiveLineSpacing ");
  ToolkitTestApplication application;

  uint32_t expectedNumberOfLines = 2u;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  // Configures the text controller similarly to the text-label.
  ConfigureTextLabel(controller);

  // Sets the text.
  controller->SetMarkupProcessorEnabled(true);
  controller->SetTextElideEnabled(false);
  controller->SetText("<p rel-line-height=2.0>Line one Line two</p>");

  // Creates the text's model and relais-out the text.
  const Size relayoutSize(120.f, 100.f);
  controller->Relayout(relayoutSize);

  // Tests the rendering controller has been created.
  TypesetterPtr typesetter = Typesetter::New(controller->GetTextModel());
  DALI_TEST_CHECK(typesetter);

  // Tests the view model has been created.
  ViewModel* model = typesetter->GetViewModel();
  DALI_TEST_CHECK(model);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(model->GetNumberOfLines(), expectedNumberOfLines, TEST_LOCATION);
  DALI_TEST_CHECK(model->GetLines());

  const LineRun& lineOne = *(model->GetLines() + 0u);
  const LineRun& lineTwo = *(model->GetLines() + 1u);

  DALI_TEST_EQUALS(GetPostOffsetVerticalLineAlignment(lineOne, Dali::Toolkit::DevelText::VerticalLineAlignment::TOP), 19.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPostOffsetVerticalLineAlignment(lineOne, Dali::Toolkit::DevelText::VerticalLineAlignment::MIDDLE), 9.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPostOffsetVerticalLineAlignment(lineOne, Dali::Toolkit::DevelText::VerticalLineAlignment::BOTTOM), 0.0f, TEST_LOCATION);

  DALI_TEST_EQUALS(GetPostOffsetVerticalLineAlignment(lineTwo, Dali::Toolkit::DevelText::VerticalLineAlignment::TOP), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPostOffsetVerticalLineAlignment(lineTwo, Dali::Toolkit::DevelText::VerticalLineAlignment::MIDDLE), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPostOffsetVerticalLineAlignment(lineTwo, Dali::Toolkit::DevelText::VerticalLineAlignment::BOTTOM), 0.0f, TEST_LOCATION);
  END_TEST;
}

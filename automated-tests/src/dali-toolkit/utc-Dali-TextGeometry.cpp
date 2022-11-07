

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/controls/text-controls/text-label-devel.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-field-devel.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-editor-devel.h>
#include <dali-toolkit/devel-api/text/bitmap-font.h>
#include <dali-toolkit/devel-api/text/rendering-backend.h>
#include <dali-toolkit/devel-api/text/text-enumerations-devel.h>
#include <dali-toolkit/devel-api/text/text-utils-devel.h>
#include <dali-toolkit/devel-api/text/text-geometry-devel.h>
#include "test-text-geometry-utils.h"

using namespace Dali;
using namespace Toolkit;
using namespace Text;

void dali_textgeometry_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_textgeometry_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTextGeometryGetLineBoundingRectangleLabel(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextGeometryGetLineBoundingRectangleLabel");

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  application.GetScene().Add(label);

  float lineSpacing = -20.f;

  label.SetProperty(Actor::Property::SIZE, Vector2(450.0f, 300.f));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 10.f);
  label.SetProperty(DevelTextLabel::Property::LINE_SPACING, lineSpacing);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);
  label.SetProperty(TextLabel::Property::TEXT, "Lorem ipsum dolor sit amet, \n consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.");

  application.SendNotification();
  application.Render();

  Rect<> lineGeometry = TextGeometry::GetLineBoundingRectangle(label, 1);

  Rect<> expectedLineGeometry = {0, 16, 420, 16};

  TestTextGeometryUtils::CheckRectGeometryResult(lineGeometry, expectedLineGeometry);

  END_TEST;
}

int UtcDaliTextGeometryGetLineBoundingRectangleEditor(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextGeometryGetLineBoundingRectangleEditor");

  TextEditor editor = TextEditor::New();
  DALI_TEST_CHECK(editor);

  application.GetScene().Add(editor);

  editor.SetProperty(Actor::Property::SIZE, Vector2(160.0f, 250.f));
  editor.SetProperty(TextEditor::Property::POINT_SIZE, 10.f);
  editor.SetProperty(TextEditor::Property::TEXT, "Lorem ipsum dolor sit amet, \n consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.");

  application.SendNotification();
  application.Render();

  Rect<> lineGeometry = TextGeometry::GetLineBoundingRectangle(editor, 0);

  Rect<> expectedLineGeometry = {0, 0, 91, 36};

  TestTextGeometryUtils::CheckRectGeometryResult(lineGeometry, expectedLineGeometry);

  END_TEST;
}

int UtcDaliTextGeometryGetLineBoundingRectangleField(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextGeometryGetLineBoundingRectangleField");

  TextField field = TextField::New();
  DALI_TEST_CHECK(field);

  application.GetScene().Add(field);

  field.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  field.SetProperty(Actor::Property::SIZE, Vector2(450.0f, 350.f));
  field.SetProperty(TextField::Property::POINT_SIZE, 10.f);
  field.SetProperty(TextField::Property::TEXT, "مرحبا بالعالم");

  application.SendNotification();
  application.Render();

  Rect<> lineGeometry = TextGeometry::GetLineBoundingRectangle(field, 0);

  Rect<> expectedLineGeometry = {0, 0, 163, 36};

  TestTextGeometryUtils::CheckRectGeometryResult(lineGeometry, expectedLineGeometry);

  END_TEST;
}

int UtcDaliTextGeometryEmptyTextGetLineBoundingRectangleLabel(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextGeometryEmptyTextGetLineBoundingRectangleLabel");

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  application.GetScene().Add(label);

  label.SetProperty(Actor::Property::SIZE, Vector2(450.0f, 300.f));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 10.f);
  label.SetProperty(TextLabel::Property::TEXT, "");

  application.SendNotification();
  application.Render();

  Rect<> lineGeometry = TextGeometry::GetLineBoundingRectangle(label, 0);

  Rect<> expectedLineGeometry = {0, 0, 0, 0};

  TestTextGeometryUtils::CheckRectGeometryResult(lineGeometry, expectedLineGeometry);

  END_TEST;
}

int UtcDaliTextGeometryLineSpacingPositiveGetLineBoundingRectangleLabel(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextGeometryLineSpacingPositiveGetLineBoundingRectangleLabel");

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  application.GetScene().Add(label);

  float lineSpacing = 20.f;

  label.SetProperty(Actor::Property::SIZE, Vector2(450.0f, 300.f));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 10.f);
  label.SetProperty(DevelTextLabel::Property::LINE_SPACING, lineSpacing);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);
  label.SetProperty(TextLabel::Property::TEXT, "Lorem ipsum dolor sit amet, \n consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.");

  application.SendNotification();
  application.Render();

  Rect<> lineGeometry = TextGeometry::GetLineBoundingRectangle(label, 1);

  Rect<> expectedLineGeometry = {0, 56, 420, 56};

  TestTextGeometryUtils::CheckRectGeometryResult(lineGeometry, expectedLineGeometry);

  END_TEST;
}

int UtcDaliTextGeometryWithVerticalLineAlignmentTopGetLineBoundingRectangleLabel(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextGeometryWithVerticalLineAlignmentTopGetLineBoundingRectangleLabel");

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  application.GetScene().Add(label);

  label.SetProperty(Actor::Property::SIZE, Vector2(450.0f, 300.f));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 10.f);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);
  label.SetProperty(Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "TOP");
  label.SetProperty(TextLabel::Property::TEXT, "Lorem ipsum dolor sit amet, \n consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.");

  application.SendNotification();
  application.Render();

  Rect<> lineGeometry = TextGeometry::GetLineBoundingRectangle(label, 1);

  Rect<> expectedLineGeometry = {0, 36, 420, 36};

  TestTextGeometryUtils::CheckRectGeometryResult(lineGeometry, expectedLineGeometry);

  END_TEST;
}

int UtcDaliTextGeometryWithVerticalLineAlignmentBottomGetLineBoundingRectangleLabel(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextGeometryWithVerticalLineAlignmentBottomGetLineBoundingRectangleLabel");

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  application.GetScene().Add(label);

  label.SetProperty(Actor::Property::SIZE, Vector2(450.0f, 300.f));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 10.f);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);
  label.SetProperty(Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "BOTTOM");
  label.SetProperty(TextLabel::Property::TEXT, "Lorem ipsum dolor sit amet, \n consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.");

  application.SendNotification();
  application.Render();

  Rect<> lineGeometry = TextGeometry::GetLineBoundingRectangle(label, 1);

  Rect<> expectedLineGeometry = {0, 36, 420, 36};

  TestTextGeometryUtils::CheckRectGeometryResult(lineGeometry, expectedLineGeometry);

  END_TEST;
}
int UtcDaliTextGeometryWithEllipsisMiddleGetLineBoundingRectangleLabel(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextGeometryWithEllipsisMiddleGetLineBoundingRectangleLabel");

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  application.GetScene().Add(label);

  label.SetProperty(Actor::Property::SIZE, Vector2(450.0f, 300.f));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 10.f);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);
  label.SetProperty(TextLabel::Property::ELLIPSIS, true);
  label.SetProperty(DevelTextLabel::Property::ELLIPSIS_POSITION, DevelText::EllipsisPosition::MIDDLE);
  label.SetProperty(TextLabel::Property::TEXT, "Lorem ipsum dolor sit amet, \n consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.");

  application.SendNotification();
  application.Render();

  Rect<> lineGeometry = TextGeometry::GetLineBoundingRectangle(label, 1);

  Rect<> expectedLineGeometry = {0, 36, 420, 36};

  TestTextGeometryUtils::CheckRectGeometryResult(lineGeometry, expectedLineGeometry);

  END_TEST;
}

int UtcDaliTextGeometryWithEllipsisStartGetLineBoundingRectangleLabel(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextGeometryWithEllipsisStartGetLineBoundingRectangleLabel");

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  application.GetScene().Add(label);

  label.SetProperty(Actor::Property::SIZE, Vector2(450.0f, 300.f));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 10.f);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);
  label.SetProperty(TextLabel::Property::ELLIPSIS, true);
  label.SetProperty(DevelTextLabel::Property::ELLIPSIS_POSITION, DevelText::EllipsisPosition::START);
  label.SetProperty(TextLabel::Property::TEXT, "Lorem ipsum dolor sit amet, \n consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.");

  application.SendNotification();
  application.Render();

  Rect<> lineGeometry = TextGeometry::GetLineBoundingRectangle(label, 1);

  Rect<> expectedLineGeometry = {0, 36, 420, 36};

  TestTextGeometryUtils::CheckRectGeometryResult(lineGeometry, expectedLineGeometry);

  END_TEST;
}

int UtcDaliTextGeometryWithEllipsisEndGetLineBoundingRectangleLabel(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliTextGeometryWithEllipsisEndGetLineBoundingRectangleLabel");

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  application.GetScene().Add(label);

  label.SetProperty(Actor::Property::SIZE, Vector2(450.0f, 300.f));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 10.f);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);
  label.SetProperty(TextLabel::Property::ELLIPSIS, true);
  label.SetProperty(DevelTextLabel::Property::ELLIPSIS_POSITION, DevelText::EllipsisPosition::END);
  label.SetProperty(TextLabel::Property::TEXT, "Lorem ipsum dolor sit amet, \n consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.");

  application.SendNotification();
  application.Render();

  Rect<> lineGeometry = TextGeometry::GetLineBoundingRectangle(label, 1);

  Rect<> expectedLineGeometry = {0, 36, 420, 36};

  TestTextGeometryUtils::CheckRectGeometryResult(lineGeometry, expectedLineGeometry);

  END_TEST;
}



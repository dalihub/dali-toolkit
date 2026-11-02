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
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/text/text-geometry-devel.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-label-devel.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-editor-devel.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-field-devel.h>
#include <dali-toolkit/internal/text/text-geometry.h>
#include <dali-toolkit/internal/controls/text-controls/common-text-utils.h>
#include "../dali-toolkit-internal/dali-toolkit-test-utils/text-model-fixture.h"

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Toolkit::Text;

namespace
{
void CheckBounds(const Bounds& actual, const Bounds& expected)
{
  DALI_TEST_EQUALS(actual.x, expected.x, TEST_LOCATION);
  DALI_TEST_EQUALS(actual.y, expected.y, TEST_LOCATION);
  DALI_TEST_EQUALS(actual.width, expected.width, TEST_LOCATION);
  DALI_TEST_EQUALS(actual.height, expected.height, TEST_LOCATION);
}
}

int UtcDaliTextGeometrySyntheticLineBounds(void)
{
  auto model = TextTest::MakeModel(U"abcdefgh");
  CheckBounds(GetLineBoundingRect(model, 0u), {0.f, 0.f, 40.f, 20.f});
  CheckBounds(GetLineBoundingRect(model, 1u), {0.f, 20.f, 40.f, 20.f});
  CheckBounds(GetLineBoundingRect(model, 2u), {});
  model->mScrollPosition = Vector2(-3.f, 7.f);
  model->mVisualModel->mLines[1].alignmentOffset = 5.f;
  CheckBounds(GetLineBoundingRect(model, 1u), {2.f, 27.f, 40.f, 20.f});
  model->mVisualModel = nullptr;
  CheckBounds(GetLineBoundingRect(model, 0u), {});
  END_TEST;
}

int UtcDaliTextGeometrySyntheticCharacterBounds(void)
{
  auto model = TextTest::MakeModel(U"abcdefgh");
  CheckBounds(GetCharacterBoundingRect(model, 0u), {0.f, 15.f, 10.f, 20.f});
  CheckBounds(GetCharacterBoundingRect(model, 5u), {10.f, 35.f, 10.f, 20.f});
  model->mScrollPosition = Vector2(3.f, -4.f);
  model->mVisualModel->mLines[1].alignmentOffset = 7.f;
  CheckBounds(GetCharacterBoundingRect(model, 5u), {20.f, 31.f, 10.f, 20.f});
  CheckBounds(GetCharacterBoundingRect(model, 8u), {});
  model->mVisualModel->mLines.Clear();
  CheckBounds(GetCharacterBoundingRect(model, 0u), {});
  model->mVisualModel = nullptr;
  CheckBounds(GetCharacterBoundingRect(model, 0u), {});
  END_TEST;
}

int UtcDaliTextGeometrySyntheticHitTest(void)
{
  auto model = TextTest::MakeModel(U"abcdefgh");
  DALI_TEST_EQUALS(GetCharIndexAtPosition(model, 3.f, 1.f), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(GetCharIndexAtPosition(model, 13.f, 21.f), 5, TEST_LOCATION);
  DALI_TEST_EQUALS(GetCharIndexAtPosition(model, 33.f, 1.f), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(GetCharIndexAtPosition(model, -1.f, 1.f), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(GetCharIndexAtPosition(model, 43.f, 1.f), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(GetCharIndexAtPosition(model, 3.f, 41.f), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(GetCharIndexAtPosition(ModelPtr(), 3.f, 1.f), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(GetCharIndexAtPosition(TextTest::MakeModel(U""), 3.f, 1.f), -1, TEST_LOCATION);
  model->mVisualModel->mLines[0].alignmentOffset = 10.f;
  DALI_TEST_EQUALS(GetCharIndexAtPosition(model, 13.f, 1.f), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGeometrySyntheticRanges(void)
{
  auto model = TextTest::MakeModel(U"abcdefgh");
  Vector<Vector2> sizes, positions;
  GetTextGeometry(model, 1u, 6u, sizes, positions);
  DALI_TEST_EQUALS(sizes.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(positions.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(positions[0], Vector2(10.f, 0.f), TEST_LOCATION);
  DALI_TEST_EQUALS(positions[1], Vector2(0.f, 20.f), TEST_LOCATION);
  DALI_TEST_EQUALS(sizes[0], Vector2(30.f, 20.f), TEST_LOCATION);
  DALI_TEST_EQUALS(sizes[1], Vector2(30.f, 20.f), TEST_LOCATION);
  Vector<Vector2> reversedSizes, reversedPositions;
  GetTextGeometry(model, 6u, 1u, reversedSizes, reversedPositions);
  DALI_TEST_EQUALS(reversedSizes.Count(), sizes.Count(), TEST_LOCATION);
  for(unsigned int i = 0u; i < sizes.Count(); ++i)
  {
    DALI_TEST_EQUALS(reversedSizes[i], sizes[i], TEST_LOCATION);
    DALI_TEST_EQUALS(reversedPositions[i], positions[i], TEST_LOCATION);
  }
  GetTextGeometry(model, 100u, 200u, sizes, positions);
  DALI_TEST_CHECK(sizes.Empty() && positions.Empty());
  GetTextGeometry(model, 0u, 100u, sizes, positions);
  DALI_TEST_EQUALS(sizes.Count(), 2u, TEST_LOCATION);
  GetTextGeometry(model, 100u, 0u, sizes, positions);
  DALI_TEST_EQUALS(sizes.Count(), 2u, TEST_LOCATION);
  auto empty = TextTest::MakeModel(U"");
  GetTextGeometry(empty, 0u, 0u, sizes, positions);
  DALI_TEST_CHECK(sizes.Empty() && positions.Empty());
  END_TEST;
}

int UtcDaliTextGeometrySyntheticRangeUnion(void)
{
  auto model = TextTest::MakeModel(U"abcdefgh");
  CheckBounds(Dali::Toolkit::Internal::CommonTextUtils::GetTextBoundingRectangle(model, 1u, 6u), {0.f, 0.f, 40.f, 40.f});
  CheckBounds(Dali::Toolkit::Internal::CommonTextUtils::GetTextBoundingRectangle(model, 100u, 200u), {});
  END_TEST;
}

int UtcDaliTextGeometrySyntheticLineSpacing(void)
{
  for(float spacing : {-10.f, 0.f, 10.f})
  {
    auto model = TextTest::MakeModel(U"abcdefgh");
    model->mVisualModel->mLines[0].lineSpacing = spacing;
    model->mVisualModel->mLines[1].lineSpacing = spacing;
    CheckBounds(GetLineBoundingRect(model, 0u), {0.f, 0.f, 40.f, 20.f + spacing});
    CheckBounds(GetLineBoundingRect(model, 1u), {0.f, 20.f + spacing, 40.f, 20.f + std::max(0.f, spacing)});
  }
  END_TEST;
}

int UtcDaliTextGeometrySyntheticDirectionBlocks(void)
{
  auto model = TextTest::MakeModel(U"abcdefgh");
  model->mLogicalModel->mCharacterDirections.Resize(8u, false);
  for(unsigned int i = 2u; i < 4u; ++i) model->mLogicalModel->mCharacterDirections[i] = true;
  Vector<Vector2> sizes, positions;
  GetTextGeometry(model, 0u, 7u, sizes, positions);
  DALI_TEST_EQUALS(sizes.Count(), positions.Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(sizes.Count(), 3u, TEST_LOCATION);
  for(const auto& size : sizes) DALI_TEST_CHECK(size.x >= 0.f && size.y == 20.f);
  END_TEST;
}

int UtcDaliTextGeometrySyntheticLigatureRange(void)
{
  auto model = TextTest::MakeModel(U"fi");
  auto& visual = *model->mVisualModel;
  visual.mGlyphs.Resize(1u);
  visual.mGlyphPositions.Resize(1u);
  visual.mGlyphs[0].advance = 20.f;
  visual.mGlyphs[0].width = 17.f;
  visual.mCharactersToGlyph[1] = 0u;
  visual.mGlyphsPerCharacter[0] = 0u;
  visual.mGlyphsPerCharacter[1] = 1u;
  visual.mCharactersPerGlyph[0] = 2u;
  visual.mLines[0].glyphRun.numberOfGlyphs = 1u;
  Vector<Vector2> sizes, positions;
  GetTextGeometry(model, 1u, 1u, sizes, positions);
  DALI_TEST_EQUALS(sizes.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(sizes[0].x, 10.f, TEST_LOCATION);
  DALI_TEST_EQUALS(positions[0].x, 10.f, TEST_LOCATION);
  GetTextGeometry(model, 0u, 0u, sizes, positions);
  DALI_TEST_EQUALS(sizes.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(sizes[0].x, 10.f, TEST_LOCATION);
  DALI_TEST_EQUALS(positions[0].x, 0.f, TEST_LOCATION);
  auto ending = TextTest::MakeModel(U"afi");
  auto& endingVisual = *ending->mVisualModel;
  endingVisual.mGlyphs.Resize(2u);
  endingVisual.mGlyphPositions.Resize(2u);
  endingVisual.mGlyphs[1].advance = 20.f;
  endingVisual.mGlyphs[1].width = 17.f;
  endingVisual.mCharactersToGlyph[2] = 1u;
  endingVisual.mGlyphsPerCharacter[1] = 0u;
  endingVisual.mGlyphsPerCharacter[2] = 1u;
  endingVisual.mCharactersPerGlyph[1] = 2u;
  endingVisual.mLines[0].glyphRun.numberOfGlyphs = 2u;
  GetTextGeometry(ending, 0u, 1u, sizes, positions);
  DALI_TEST_EQUALS(sizes.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(sizes[0], Vector2(20.f, 20.f), TEST_LOCATION);
  DALI_TEST_EQUALS(positions[0], Vector2::ZERO, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGeometrySyntheticEllipsisRanges(void)
{
  for(auto position : {DevelText::EllipsisPosition::START, DevelText::EllipsisPosition::MIDDLE, DevelText::EllipsisPosition::END})
  {
    auto model = TextTest::MakeModel(U"abcdefgh", 8u);
    model->mElideEnabled = true;
    model->mEllipsisPosition = position;
    auto& visual = *model->mVisualModel;
    visual.SetEllipsisPosition(position);
    visual.SetStartIndexOfElidedGlyphs(2u);
    visual.SetEndIndexOfElidedGlyphs(5u);
    visual.SetFirstMiddleIndexOfElidedGlyphs(2u);
    visual.SetSecondMiddleIndexOfElidedGlyphs(5u);
    Vector<Vector2> sizes, positions;
    GetTextGeometry(model, 0u, 7u, sizes, positions);
    DALI_TEST_CHECK(!sizes.Empty());
    DALI_TEST_EQUALS(sizes.Count(), positions.Count(), TEST_LOCATION);
    DALI_TEST_CHECK(sizes[0].x < 80.f);
  }
  END_TEST;
}

int UtcDaliTextGeometryDeterministicLabelApi(void)
{
  ToolkitTestApplication application(480u, 800u, 96.f, 96.f);
  auto control = TextLabel::New();
  control.SetProperty(Actor::Property::SIZE, Vector2(200.f, 100.f));
  control.SetProperty(TextLabel::Property::PIXEL_SIZE, 16.f);
  control.SetProperty(TextLabel::Property::FONT_FAMILY, "Test Sans");
  control.SetProperty(TextLabel::Property::TEXT, "abcd");
  application.GetScene().Add(control);
  application.SendNotification();
  application.Render();
  auto line = TextGeometry::GetLineBoundingRectangle(control, 0u);
  auto character = TextGeometry::GetCharacterBoundingRectangle(control, 1u);
  DALI_TEST_CHECK(line.width > 0.f);
  DALI_TEST_EQUALS(character.width, 8.f, TEST_LOCATION);
  DALI_TEST_EQUALS(character.height, 16.f, TEST_LOCATION);
  CheckBounds(TextGeometry::GetLineBoundingRectangle(control, 100u), {});
  CheckBounds(TextGeometry::GetCharacterBoundingRectangle(control, 100u), {});
  control.SetProperty(TextLabel::Property::TEXT, "");
  application.SendNotification();
  application.Render();
  CheckBounds(TextGeometry::GetLineBoundingRectangle(control, 0u), {});
  CheckBounds(TextGeometry::GetCharacterBoundingRectangle(control, 0u), {});
  DALI_TEST_EQUALS(TextGeometry::GetCharacterIndexAtPosition(control, 0.f, 0.f), -1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGeometryDeterministicEditorApi(void)
{
  ToolkitTestApplication application(480u, 800u, 96.f, 96.f);
  auto control = TextEditor::New();
  control.SetProperty(Actor::Property::SIZE, Vector2(200.f, 100.f));
  control.SetProperty(TextEditor::Property::PIXEL_SIZE, 16.f);
  control.SetProperty(TextEditor::Property::FONT_FAMILY, "Test Sans");
  control.SetProperty(TextEditor::Property::TEXT, "abcd");
  application.GetScene().Add(control);
  application.SendNotification();
  application.Render();
  auto line = TextGeometry::GetLineBoundingRectangle(control, 0u);
  auto character = TextGeometry::GetCharacterBoundingRectangle(control, 1u);
  DALI_TEST_CHECK(line.width > 0.f);
  DALI_TEST_EQUALS(character.width, 8.f, TEST_LOCATION);
  DALI_TEST_EQUALS(character.height, 16.f, TEST_LOCATION);
  CheckBounds(TextGeometry::GetLineBoundingRectangle(control, 100u), {});
  CheckBounds(TextGeometry::GetCharacterBoundingRectangle(control, 100u), {});
  control.SetProperty(TextEditor::Property::TEXT, "");
  application.SendNotification();
  application.Render();
  CheckBounds(TextGeometry::GetLineBoundingRectangle(control, 0u), {});
  CheckBounds(TextGeometry::GetCharacterBoundingRectangle(control, 0u), {});
  DALI_TEST_EQUALS(TextGeometry::GetCharacterIndexAtPosition(control, 0.f, 0.f), -1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGeometryDeterministicFieldApi(void)
{
  ToolkitTestApplication application(480u, 800u, 96.f, 96.f);
  auto control = TextField::New();
  control.SetProperty(Actor::Property::SIZE, Vector2(200.f, 100.f));
  control.SetProperty(TextField::Property::PIXEL_SIZE, 16.f);
  control.SetProperty(TextField::Property::FONT_FAMILY, "Test Sans");
  control.SetProperty(TextField::Property::TEXT, "abcd");
  application.GetScene().Add(control);
  application.SendNotification();
  application.Render();
  auto line = TextGeometry::GetLineBoundingRectangle(control, 0u);
  auto character = TextGeometry::GetCharacterBoundingRectangle(control, 1u);
  DALI_TEST_CHECK(line.width > 0.f);
  DALI_TEST_EQUALS(character.width, 8.f, TEST_LOCATION);
  DALI_TEST_EQUALS(character.height, 16.f, TEST_LOCATION);
  CheckBounds(TextGeometry::GetLineBoundingRectangle(control, 100u), {});
  CheckBounds(TextGeometry::GetCharacterBoundingRectangle(control, 100u), {});
  control.SetProperty(TextField::Property::TEXT, "");
  application.SendNotification();
  application.Render();
  CheckBounds(TextGeometry::GetLineBoundingRectangle(control, 0u), {});
  CheckBounds(TextGeometry::GetCharacterBoundingRectangle(control, 0u), {});
  DALI_TEST_EQUALS(TextGeometry::GetCharacterIndexAtPosition(control, 0.f, 0.f), -1, TEST_LOCATION);
  END_TEST;
}

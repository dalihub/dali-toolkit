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
#include <dali-toolkit/internal/text/cursor-helper-functions.h>
#include <toolkit-text-utils.h>
#include <text-model-fixture.h>
#include <cmath>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Toolkit::Text;

int UtcDaliGetClosestLine(void)
{
  auto model = TextTest::MakeModel(U"abcdefgh");
  bool hit = false;
  DALI_TEST_EQUALS(GetClosestLine(model->mVisualModel, 5.f, hit), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(hit);
  DALI_TEST_EQUALS(GetClosestLine(model->mVisualModel, 25.f, hit), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(hit);
  DALI_TEST_EQUALS(GetClosestLine(model->mVisualModel, -10.f, hit), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!hit);
  DALI_TEST_EQUALS(GetClosestLine(model->mVisualModel, 100.f, hit), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!hit);
  DALI_TEST_EQUALS(CalculateLineOffset(model->mVisualModel->mLines, 0u), 0.f, TEST_LOCATION);
  DALI_TEST_EQUALS(CalculateLineOffset(model->mVisualModel->mLines, 1u), 20.f, TEST_LOCATION);
  model->mVisualModel->mLines[0].lineSpacing = -5.f;
  DALI_TEST_EQUALS(CalculateLineOffset(model->mVisualModel->mLines, 1u), 15.f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGetClosestCursorIndex(void)
{
  ToolkitTestApplication application;
  auto model = TextTest::MakeModel();
  auto metrics = Metrics::New();
  bool hit = false;
  DALI_TEST_EQUALS(GetClosestCursorIndex(model->mVisualModel, model->mLogicalModel, metrics, 0.f, 5.f, CharacterHitTest::TAP, hit), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(GetClosestCursorIndex(model->mVisualModel, model->mLogicalModel, metrics, 100.f, 5.f, CharacterHitTest::TAP, hit), 4u, TEST_LOCATION);
  for(auto mode : {CharacterHitTest::TAP, CharacterHitTest::SCROLL})
  {
    for(float x : {-20.f, 1.f, 5.f, 9.f, 15.f, 35.f, 100.f})
    {
      for(float y : {-20.f, 5.f, 100.f})
      {
        auto index = GetClosestCursorIndex(model->mVisualModel, model->mLogicalModel, metrics, x, y, mode, hit);
        DALI_TEST_CHECK(index <= 4u);
      }
    }
  }
  auto empty = TextTest::MakeModel(U"");
  DALI_TEST_EQUALS(GetClosestCursorIndex(empty->mVisualModel, empty->mLogicalModel, metrics, 0.f, 0.f, CharacterHitTest::TAP, hit), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!hit);
  END_TEST;
}

int UtcDaliGetCursorPosition(void)
{
  ToolkitTestApplication application;
  auto model = TextTest::MakeModel();
  GetCursorPositionParameters parameters;
  parameters.visualModel = model->mVisualModel;
  parameters.logicalModel = model->mLogicalModel;
  parameters.metrics = Metrics::New();
  parameters.isMultiline = true;
  for(unsigned int i = 0u; i <= 4u; ++i)
  {
    parameters.logical = i;
    CursorInfo cursor;
    GetCursorPosition(parameters, 20.f, cursor);
    DALI_TEST_EQUALS(cursor.primaryPosition.x, i * 10.f, TEST_LOCATION);
    DALI_TEST_EQUALS(cursor.primaryCursorHeight, 20.f, TEST_LOCATION);
    DALI_TEST_CHECK(!cursor.isSecondaryCursor);
  }
  END_TEST;
}

int UtcDaliFindSelectionIndices(void)
{
  ToolkitTestApplication application;
  auto model = TextTest::MakeModel(U"one two", 100u);
  auto metrics = Metrics::New();
  CharacterIndex start = 0u, end = 0u, nearest = 0u;
  DALI_TEST_CHECK(FindSelectionIndices(model->mVisualModel, model->mLogicalModel, metrics, 12.f, 5.f, start, end, nearest));
  DALI_TEST_EQUALS(start, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(end, 3u, TEST_LOCATION);
  for(float x : {-20.f, 5.f, 35.f, 55.f, 200.f})
  {
    FindSelectionIndices(model->mVisualModel, model->mLogicalModel, metrics, x, 5.f, start, end, nearest);
    DALI_TEST_CHECK(start <= end && end <= 7u && nearest <= 7u);
  }
  auto empty = TextTest::MakeModel(U"");
  DALI_TEST_CHECK(!FindSelectionIndices(empty->mVisualModel, empty->mLogicalModel, metrics, 0.f, 0.f, start, end, nearest));
  END_TEST;
}

int UtcDaliPrimaryCursorHeight(void)
{
  ToolkitTestApplication application;
  for(const auto& text : {std::string("abc אבג def"), std::string("مرحبا abc عالم"), std::string("a\nb\n"), std::string("مرحبا\nعالم\n"), std::string("á 😀👨‍👩‍👧‍👦"), std::string("  a\n\nb  "), std::string("")})
  {
    ModelPtr model;
    MetricsPtr metrics;
    Size size;
    Vector<FontDescriptionRun> descriptions;
    LayoutOptions options;
    CreateTextModel(text, Size(80.f, 200.f), descriptions, options, size, model, metrics,
                    false, LineWrap::WORD, false, DevelText::EllipsisPosition::END, 0.f, 0.f);
    for(float y : {-10.f, 5.f, 20.f, 40.f, 1000.f})
    {
      for(float x : {-100.f, 0.f, 4.f, 12.f, 24.f, 40.f, 70.f, 1000.f})
      {
        bool hit = false;
        const auto index = GetClosestCursorIndex(model->mVisualModel, model->mLogicalModel, metrics, x, y, CharacterHitTest::TAP, hit);
        DALI_TEST_CHECK(index <= model->mLogicalModel->mText.Count());
        CharacterIndex start = 0u, end = 0u, nearest = 0u;
        FindSelectionIndices(model->mVisualModel, model->mLogicalModel, metrics, x, y, start, end, nearest);
        DALI_TEST_CHECK(start <= end && end <= model->mLogicalModel->mText.Count());
      }
    }
    GetCursorPositionParameters parameters;
    parameters.visualModel = model->mVisualModel;
    parameters.logicalModel = model->mLogicalModel;
    parameters.metrics = metrics;
    parameters.isMultiline = true;
    for(unsigned int i = 0u; i <= model->mLogicalModel->mText.Count(); ++i)
    {
      parameters.logical = i;
      CursorInfo cursor;
      GetCursorPosition(parameters, 16.f, cursor);
      DALI_TEST_CHECK(std::isfinite(cursor.primaryPosition.x) && std::isfinite(cursor.primaryPosition.y));
      DALI_TEST_CHECK(cursor.primaryCursorHeight >= 0.f && cursor.secondaryCursorHeight >= 0.f);
    }
  }
  END_TEST;
}

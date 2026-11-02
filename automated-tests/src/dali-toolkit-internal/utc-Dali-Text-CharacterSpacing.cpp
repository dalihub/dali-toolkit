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
#include <dali-toolkit/internal/text/glyph-metrics-helper.h>
#include <toolkit-text-utils.h>
#include <cmath>
using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Toolkit::Text;
namespace
{
void CheckSpacing(unsigned int scenario)
{
  ToolkitTestApplication application;
  const std::string texts[] = {"abcd", "one two three", "abc אבג def", "مرحبا بالعالم", "a\nb\n"};
  for(float spacing : {-2.f, 0.f, 3.f, 10.f})
  {
    ModelPtr model;
    MetricsPtr metrics;
    Size size;
    Vector<FontDescriptionRun> descriptions;
    LayoutOptions options;
    CreateTextModel(texts[scenario], Size(scenario % 2u ? 64.f : 300.f, 300.f), descriptions,
                    options, size, model, metrics, false, LineWrap::CHARACTER, false,
                    DevelText::EllipsisPosition::END, 0.f, spacing);
    DALI_TEST_EQUALS(model->mVisualModel->GetCharacterSpacing(), spacing, TEST_LOCATION);
    DALI_TEST_CHECK(std::isfinite(size.x) && std::isfinite(size.y));
    for(const auto& line : model->mVisualModel->mLines)
    {
      DALI_TEST_CHECK(line.glyphRun.glyphIndex + line.glyphRun.numberOfGlyphs <= model->mVisualModel->mGlyphs.Count());
    }
    if(scenario == 0u)
    {
      for(unsigned int i = 0u; i < 4u; ++i)
      {
        DALI_TEST_EQUALS(model->mVisualModel->mGlyphPositions[i].x, i * (8.f + spacing), TEST_LOCATION);
      }
    }
  }
  DALI_TEST_EQUALS(GetCalculatedAdvance('a', 3.f, 8.f), 11.f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetCalculatedAdvance(0x200bu, 3.f, 0.f), 0.f, TEST_LOCATION);
}
}

int UtcDaliTextCharacterSpacingSingleLineTextArea1(void)
{
  CheckSpacing(0u);
  END_TEST;
}

int UtcDaliTextCharacterSpacingSingleLineTextArea2(void)
{
  CheckSpacing(1u);
  END_TEST;
}

int UtcDaliTextCharacterSpacingSingleLineTextArea3(void)
{
  CheckSpacing(2u);
  END_TEST;
}

int UtcDaliTextCharacterSpacingMultilineText1(void)
{
  CheckSpacing(3u);
  END_TEST;
}

int UtcDaliTextCharacterSpacingMultilineText2(void)
{
  CheckSpacing(4u);
  END_TEST;
}

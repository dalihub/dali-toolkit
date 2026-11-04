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

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/internal/text/hyphenator.h>
#include <toolkit-text-utils.h>
using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Toolkit::Text;
namespace
{
void CheckHyphens(LineWrap::Mode mode)
{
  ToolkitTestApplication application;
  auto hyphenation = TextAbstraction::Hyphenation::Get();
  const Character word[] = {'a','b','c','d','e','f','g','h','i','j','k','l'};
  auto breaks = GetWordHyphens(hyphenation, word, 12u, "unused");
  DALI_TEST_EQUALS(breaks.Count(), 12u, TEST_LOCATION);
  for(unsigned int i = 0u; i < 12u; ++i) DALI_TEST_EQUALS(breaks[i], i == 3u || i == 7u, TEST_LOCATION);
  DALI_TEST_CHECK(GetWordHyphens(hyphenation, nullptr, 0u, nullptr).Empty());
  for(float width : {32.f, 48.f, 64.f, 200.f})
  {
    ModelPtr model;
    MetricsPtr metrics;
    Size size;
    Vector<FontDescriptionRun> descriptions;
    LayoutOptions options;
    CreateTextModel("abcdefghijkl mnopqrstuvwx", Size(width, 200.f), descriptions, options,
                    size, model, metrics, false, mode, false, DevelText::EllipsisPosition::END, 0.f, 0.f);
    DALI_TEST_CHECK(!model->mVisualModel->mLines.Empty());
    DALI_TEST_CHECK(size.height > 0.f);
    if(width == 200.f) DALI_TEST_EQUALS(model->mVisualModel->mLines.Count(), 1u, TEST_LOCATION);
    for(const auto& line : model->mVisualModel->mLines)
    {
      DALI_TEST_CHECK(line.characterRun.characterIndex + line.characterRun.numberOfCharacters <= model->mLogicalModel->mText.Count());
    }
  }
}
}
int UtcDaliTextHyphenWrapping(void)
{
  CheckHyphens(static_cast<LineWrap::Mode>(DevelText::LineWrap::HYPHENATION));
  END_TEST;
}
int UtcDaliTextMixedWrapping(void)
{
  CheckHyphens(static_cast<LineWrap::Mode>(DevelText::LineWrap::MIXED));
  END_TEST;
}

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
#include <dali-toolkit/devel-api/controls/text-controls/text-spannable.h>
#include <dali-toolkit/devel-api/text/spannable-string.h>
#include <dali-toolkit/devel-api/text/spans/foreground-color-span.h>
#include <dali-toolkit/internal/controls/text-controls/text-editor-impl.h>
#include <dali-toolkit/internal/controls/text-controls/text-field-impl.h>
#include <dali-toolkit/internal/controls/text-controls/text-label-impl.h>

using namespace Dali;
using namespace Toolkit;

Text::SpannableString CreateSpannableStringForForegroundColorSpan()
{
  Text::SpannableString spannableString = Text::SpannableString::New("Hello مرحبا");
  DALI_TEST_CHECK(spannableString);

  auto isAddedGreen = spannableString.AttachSpan(
    Text::ForegroundColorSpan::New(Color::GREEN),
    Text::Range::New(5u, 7u));
  DALI_TEST_CHECK(isAddedGreen);

  return spannableString;
}

void CheckColorIndices(const Text::ColorIndex* const colorIndicesBuffer,
                       uint32_t                      numberOfIndices,
                       std::vector<uint32_t>         indicesToCheck,
                       std::vector<uint32_t>         expectedValues)
{
  DALI_TEST_CHECK(colorIndicesBuffer);

  for(uint32_t i = 0u; i < numberOfIndices; i++)
  {
    DALI_TEST_EQUALS(colorIndicesBuffer[indicesToCheck[i]], expectedValues[i], TEST_LOCATION);
  }
}

int UtcDaliToolkitTextLabelSetSpannedText(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelSetSpannedText");

  TextLabel textLabel = TextLabel::New();
  DALI_TEST_CHECK(textLabel);
  application.GetScene().Add(textLabel);

  Text::SpannableString spannableString = CreateSpannableStringForForegroundColorSpan();

  Text::SetSpannedText(textLabel, spannableString);

  application.SendNotification();
  application.Render();

  Toolkit::Internal::TextLabel& labelImpl           = GetImpl(textLabel);
  const Text::ColorIndex* const colorIndicesBuffer  = labelImpl.GetTextController()->GetTextModel()->GetColorIndices();

  CheckColorIndices(colorIndicesBuffer, 4u, {0u, 5u, 7u, 10u}, {0u, 1u, 1u, 0u});

  END_TEST;
}

int UtcDaliToolkitTextEditorSetSpannedText(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextEditorSetSpannedText");

  TextEditor textEditor = TextEditor::New();
  DALI_TEST_CHECK(textEditor);
  application.GetScene().Add(textEditor);

  Text::SpannableString spannableString = CreateSpannableStringForForegroundColorSpan();

  Text::SetSpannedText(textEditor, spannableString);

  application.SendNotification();
  application.Render();

  Toolkit::Internal::TextEditor& labelImpl          = GetImpl(textEditor);
  const Text::ColorIndex* const  colorIndicesBuffer = labelImpl.GetTextController()->GetTextModel()->GetColorIndices();

  CheckColorIndices(colorIndicesBuffer, 4u, {0u, 5u, 7u, 10u}, {0u, 1u, 1u, 0u});

  END_TEST;
}

int UtcDaliToolkitTextFieldSetSpannedText(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFieldSetSpannedText");

  TextField textField = TextField::New();
  DALI_TEST_CHECK(textField);
  application.GetScene().Add(textField);

  Text::SpannableString spannableString = CreateSpannableStringForForegroundColorSpan();

  Text::SetSpannedText(textField, spannableString);

  application.SendNotification();
  application.Render();

  Toolkit::Internal::TextField& labelImpl          = GetImpl(textField);
  const Text::ColorIndex* const colorIndicesBuffer = labelImpl.GetTextController()->GetTextModel()->GetColorIndices();

  CheckColorIndices(colorIndicesBuffer, 4u, {0u, 5u, 7u, 10u}, {0u, 1u, 1u, 0u});

  END_TEST;
}

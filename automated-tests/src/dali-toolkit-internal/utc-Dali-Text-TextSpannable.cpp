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
#include <dali-toolkit/devel-api/text/spans/font-span.h>
#include <dali-toolkit/devel-api/text/spans/foreground-color-span.h>
#include <dali-toolkit/devel-api/text/text-enumerations-devel.h>
#include <dali-toolkit/internal/controls/text-controls/text-editor-impl.h>
#include <dali-toolkit/internal/controls/text-controls/text-field-impl.h>
#include <dali-toolkit/internal/controls/text-controls/text-label-impl.h>
#include <dali-toolkit/internal/text/font-description-run.h>
#include <dali-toolkit/public-api/text/text-enumerations.h>

using namespace Dali;
using namespace Toolkit;

namespace
{
const std::string DEFAULT_FONT_DIR("/resources/fonts");
const float PIXEL_FORMAT_64_FACTOR = 64.f; ///< 64.f is used to convert from point size to 26.6 pixel format.
} // namespace

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

Text::SpannableString CreateSpannableStringForFontSpan()
{
  Text::SpannableString spannableString = Text::SpannableString::New("Hello World");
  DALI_TEST_CHECK(spannableString);

  auto isAddedFontSpan = spannableString.AttachSpan(
    Text::FontSpan::New("TizenSans",
                        45.0f,
                        Dali::TextAbstraction::FontWeight::BOLD,
                        Dali::TextAbstraction::FontWidth::SEMI_CONDENSED,
                        Dali::TextAbstraction::FontSlant::OBLIQUE),
    Text::Range::New(5u, 7u));
  DALI_TEST_CHECK(isAddedFontSpan);

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

int UtcDaliToolkitTextLabelSetSpannedText_FontSpan(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelSetSpannedText_FontSpan ");

  // Load some fonts to get the same metrics on different platforms.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(96u, 96u);

  char*             pathNamePtr = get_current_dir_name();
  const std::string pathName(pathNamePtr);
  free(pathNamePtr);

  fontClient.GetFontId(pathName + DEFAULT_FONT_DIR + "/tizen/TizenSansRegular.ttf");

  TextLabel textLabel = TextLabel::New();
  DALI_TEST_CHECK(textLabel);
  application.GetScene().Add(textLabel);

  Text::SpannableString spannableString = CreateSpannableStringForFontSpan();

  Text::SetSpannedText(textLabel, spannableString);

  application.SendNotification();
  application.Render();

  Toolkit::Internal::TextLabel&               labelImpl  = GetImpl(textLabel);
  const Vector<Dali::Toolkit::Text::FontRun>& validFontRuns = labelImpl.GetTextController()->GetTextModel()->GetFontRuns();

  DALI_TEST_EQUALS(validFontRuns.Count(), 3u, TEST_LOCATION);

  DALI_TEST_EQUALS(validFontRuns[0].fontId, validFontRuns[2].fontId, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(validFontRuns[0].fontId, validFontRuns[1].fontId, Math::MACHINE_EPSILON_100, TEST_LOCATION);

  DALI_TEST_EQUALS(validFontRuns[1].characterRun.characterIndex, 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(validFontRuns[1].characterRun.GetEndCharacterIndex(), 7u, TEST_LOCATION);
  DALI_TEST_EQUALS(validFontRuns[1].isItalicRequired, true, TEST_LOCATION);
  DALI_TEST_EQUALS(validFontRuns[1].isBoldRequired, true, TEST_LOCATION);

  TextAbstraction::FontDescription spanFontDescription;

  float expectedPointSize = 45.0f * PIXEL_FORMAT_64_FACTOR;
  float fontPointSize     = fontClient.GetPointSize(validFontRuns[1].fontId);
  DALI_TEST_EQUALS(fontPointSize, expectedPointSize, TEST_LOCATION);

  const Vector<Dali::Toolkit::Text::FontDescriptionRun>& validFontDescriptionRuns = labelImpl.GetTextController()->GetTextModel()->GetFontDescriptionRuns();
  DALI_TEST_EQUALS(validFontDescriptionRuns.Count(), 1u, TEST_LOCATION);
  std::string familyName = validFontDescriptionRuns[0].familyName;

  DALI_TEST_EQUALS(familyName, "TizenSans", TEST_LOCATION);
  DALI_TEST_EQUALS(validFontDescriptionRuns[0].size, expectedPointSize, TEST_LOCATION);
  DALI_TEST_EQUALS(validFontDescriptionRuns[0].weight, Dali::TextAbstraction::FontWeight::BOLD, TEST_LOCATION);
  DALI_TEST_EQUALS(validFontDescriptionRuns[0].width, Dali::TextAbstraction::FontWidth::SEMI_CONDENSED, TEST_LOCATION);
  DALI_TEST_EQUALS(validFontDescriptionRuns[0].slant, Dali::TextAbstraction::FontSlant::OBLIQUE, TEST_LOCATION);

  END_TEST;
}
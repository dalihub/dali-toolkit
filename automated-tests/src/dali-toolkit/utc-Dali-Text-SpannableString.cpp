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
#include <dali-toolkit/devel-api/text/spannable-string.h>
#include <dali-toolkit/devel-api/text/spans/foreground-color-span.h>

using namespace Dali;
using namespace Toolkit;

int UtcDaliToolkitTextSpannableStringNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextSpannableStringNew");

  Text::SpannableString spannableString = Text::SpannableString::New("Hello مرحبا");

  DALI_TEST_CHECK(spannableString);

  END_TEST;
}

int UtcDaliToolkitTextSpannableStringGetCharacters(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextSpannableStringGetCharacters");

  Text::SpannableString spannableString = Text::SpannableString::New("Hello مرحبا");
  DALI_TEST_CHECK(spannableString);

  Vector<uint32_t> expectedChars;
  expectedChars.PushBack('H');
  expectedChars.PushBack('e');
  expectedChars.PushBack('l');
  expectedChars.PushBack('l');
  expectedChars.PushBack('o');
  expectedChars.PushBack(' ');
  expectedChars.PushBack(0x0645); //'م'
  expectedChars.PushBack(0x0631); //'ر'
  expectedChars.PushBack(0x062D); //'ح'
  expectedChars.PushBack(0x0628); //'ب'
  expectedChars.PushBack(0x0627); //'ا'

  auto chars = spannableString.GetCharacters();
  DALI_TEST_EQUALS(expectedChars.Size(), chars.Size(), TEST_LOCATION);

  for(uint32_t i = 0; i < expectedChars.Size(); i++)
  {
    DALI_TEST_EQUALS(expectedChars[i], chars[i], TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliToolkitTextSpannableStringGetNumberOfCharacters(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextSpannableStringGetNumberOfCharacters");

  Text::SpannableString spannableString = Text::SpannableString::New("Hello مرحبا");
  DALI_TEST_CHECK(spannableString);

  DALI_TEST_EQUALS(11u, spannableString.GetNumberOfCharacters(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextSpannableStringToString(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextSpannableStringToString");
  std::string text = "Hello مرحبا";

  Text::SpannableString spannableString = Text::SpannableString::New(text);
  DALI_TEST_CHECK(spannableString);

  DALI_TEST_EQUALS(text, spannableString.ToString(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextSpannableStringAttachSpan(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextSpannableStringAttachSpan");

  Text::SpannableString spannableString = Text::SpannableString::New("Hello مرحبا");
  DALI_TEST_CHECK(spannableString);

  auto greenSpan = Dali::Toolkit::Text::ForegroundColorSpan::New(Color::GREEN);

  auto isAddedGreen = spannableString.AttachSpan(
    greenSpan,
    Dali::Toolkit::Text::Range::New(5u, 7u));
  DALI_TEST_CHECK(isAddedGreen);

  auto isAddedBlue = spannableString.AttachSpan(
    Dali::Toolkit::Text::ForegroundColorSpan::New(Color::BLUE),
    Dali::Toolkit::Text::Range::New(4u, 2u));
  DALI_TEST_CHECK(isAddedBlue);

  auto isAddedRed = spannableString.AttachSpan(
    Dali::Toolkit::Text::ForegroundColorSpan::New(Color::RED),
    Dali::Toolkit::Text::Range::New(15u, 2u));
  DALI_TEST_CHECK(!isAddedRed);

  END_TEST;
}

int UtcDaliToolkitTextSpannableStringDetachSpan(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextSpannableStringDetachSpan");

  Text::SpannableString spannableString = Text::SpannableString::New("Hello مرحبا");
  DALI_TEST_CHECK(spannableString);

  auto greenSpan = Dali::Toolkit::Text::ForegroundColorSpan::New(Color::GREEN);

  auto isAddedGreen = spannableString.AttachSpan(
    greenSpan,
    Dali::Toolkit::Text::Range::New(5u, 7u));
  DALI_TEST_CHECK(isAddedGreen);

  auto isAddedBlue = spannableString.AttachSpan(
    Dali::Toolkit::Text::ForegroundColorSpan::New(Color::BLUE),
    Dali::Toolkit::Text::Range::New(4u, 2u));
  DALI_TEST_CHECK(isAddedBlue);

  auto isRemovedGreen = spannableString.DetachSpan(greenSpan);
  DALI_TEST_CHECK(isRemovedGreen);

  auto isRemovedGreen2 = spannableString.DetachSpan(greenSpan);
  DALI_TEST_CHECK(!isRemovedGreen2);

  END_TEST;
}

int UtcDaliToolkitTextSpannableStringGetAllSpans(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextSpannableStringGetAllSpans");

  Text::SpannableString spannableString = Text::SpannableString::New("Hello مرحبا");
  DALI_TEST_CHECK(spannableString);

  auto greenSpan = Dali::Toolkit::Text::ForegroundColorSpan::New(Color::GREEN);
  spannableString.AttachSpan(
    greenSpan,
    Dali::Toolkit::Text::Range::New(5u, 7u));

  auto blueSpan = Dali::Toolkit::Text::ForegroundColorSpan::New(Color::BLUE);
  spannableString.AttachSpan(
    blueSpan,
    Dali::Toolkit::Text::Range::New(4u, 2u));

  auto redSpan = Dali::Toolkit::Text::ForegroundColorSpan::New(Color::RED);
  spannableString.AttachSpan(
    redSpan,
    Dali::Toolkit::Text::Range::New(15u, 2u));

  auto spans = spannableString.GetAllSpans();

  DALI_TEST_EQUALS(2u, spans.size(), TEST_LOCATION);

  DALI_TEST_EQUALS(greenSpan, spans[0], TEST_LOCATION);
  DALI_TEST_EQUALS(blueSpan, spans[1], TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextSpannableStringRetrieveAllSpansAndRanges(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextSpannableStringRetrieveAllSpansAndRanges");

  Text::SpannableString spannableString = Text::SpannableString::New("Hello مرحبا");
  DALI_TEST_CHECK(spannableString);

  auto greenSpan  = Dali::Toolkit::Text::ForegroundColorSpan::New(Color::GREEN);
  auto greenRange = Dali::Toolkit::Text::Range::New(5u, 7u);
  spannableString.AttachSpan(
    greenSpan,
    greenRange);

  auto blueSpan  = Dali::Toolkit::Text::ForegroundColorSpan::New(Color::BLUE);
  auto blueRange = Dali::Toolkit::Text::Range::New(4u, 2u);
  spannableString.AttachSpan(
    blueSpan,
    blueRange);

  auto redSpan  = Dali::Toolkit::Text::ForegroundColorSpan::New(Color::RED);
  auto redRange = Dali::Toolkit::Text::Range::New(15u, 2u);
  spannableString.AttachSpan(
    redSpan,
    redRange);

  std::vector<Text::BaseSpan> spans;
  std::vector<Text::Range>    ranges;

  spannableString.RetrieveAllSpansAndRanges(spans, ranges);

  DALI_TEST_EQUALS(2u, spans.size(), TEST_LOCATION);
  DALI_TEST_EQUALS(2u, ranges.size(), TEST_LOCATION);

  DALI_TEST_EQUALS(greenSpan, spans[0], TEST_LOCATION);
  DALI_TEST_EQUALS(blueSpan, spans[1], TEST_LOCATION);

  DALI_TEST_EQUALS(greenRange, ranges[0], TEST_LOCATION);
  DALI_TEST_EQUALS(blueRange, ranges[1], TEST_LOCATION);

  END_TEST;
}
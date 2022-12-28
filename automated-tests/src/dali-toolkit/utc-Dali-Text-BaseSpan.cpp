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
#include <dali-toolkit/devel-api/text/spans/background-color-span.h>
#include <dali-toolkit/devel-api/text/spans/bold-span.h>
#include <dali-toolkit/devel-api/text/spans/character-spacing-span.h>
#include <dali-toolkit/devel-api/text/spans/font-span.h>
#include <dali-toolkit/devel-api/text/spans/foreground-color-span.h>
#include <dali-toolkit/devel-api/text/spans/italic-span.h>
#include <dali-toolkit/devel-api/text/spans/strikethrough-span.h>
#include <dali-toolkit/devel-api/text/spans/underline-span.h>

using namespace Dali;
using namespace Toolkit;

int UtcDaliToolkitTextBaseSpanDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextBaseSpanDownCast");

  BaseHandle baseHandel = Text::ForegroundColorSpan::New(Color::GREEN);
  DALI_TEST_CHECK(baseHandel);

  Text::BaseSpan baseSpan = Text::BaseSpan::DownCast(baseHandel);
  DALI_TEST_CHECK(baseSpan);

  END_TEST;
}

int UtcDaliToolkitTextBaseSpanGetSpanType(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextBaseSpanGetSpanType");

  Text::BaseSpan foregroundColorSpan = Text::ForegroundColorSpan::New(Color::GREEN);
  DALI_TEST_CHECK(foregroundColorSpan);
  DALI_TEST_EQUALS(foregroundColorSpan.GetSpanType(), Dali::Toolkit::Text::SpanType::Value::FOREGROUND_COLOR, TEST_LOCATION);

  Text::BaseSpan backgroundColorSpan = Text::BackgroundColorSpan::New(Color::GREEN);
  DALI_TEST_CHECK(backgroundColorSpan);
  DALI_TEST_EQUALS(backgroundColorSpan.GetSpanType(), Dali::Toolkit::Text::SpanType::Value::BACKGROUND_COLOR, TEST_LOCATION);

  Text::BaseSpan fontSpan = Text::FontSpan::New("TizenSans",
                                                45.0f,
                                                Dali::TextAbstraction::FontWeight::BOLD,
                                                Dali::TextAbstraction::FontWidth::SEMI_CONDENSED,
                                                Dali::TextAbstraction::FontSlant::OBLIQUE);
  DALI_TEST_CHECK(fontSpan);
  DALI_TEST_EQUALS(fontSpan.GetSpanType(), Dali::Toolkit::Text::SpanType::Value::FONT, TEST_LOCATION);

  Text::BaseSpan boldSpan = Dali::Toolkit::Text::BoldSpan::New();
  DALI_TEST_CHECK(boldSpan);
  DALI_TEST_EQUALS(boldSpan.GetSpanType(), Dali::Toolkit::Text::SpanType::Value::BOLD, TEST_LOCATION);

  Text::BaseSpan italicSpan = Text::ItalicSpan::New();
  DALI_TEST_CHECK(italicSpan);
  DALI_TEST_EQUALS(italicSpan.GetSpanType(), Dali::Toolkit::Text::SpanType::Value::ITALIC, TEST_LOCATION);

  Text::BaseSpan underlineSpan = Text::UnderlineSpan::NewDashed(Color::GREEN, 5.0f, 2.0f, 3.0f);
  DALI_TEST_CHECK(underlineSpan);
  DALI_TEST_EQUALS(underlineSpan.GetSpanType(), Dali::Toolkit::Text::SpanType::Value::UNDERLINE, TEST_LOCATION);

  Text::BaseSpan strikethroughSpan = Text::StrikethroughSpan::New(Color::GREEN, 5.0f);
  DALI_TEST_CHECK(strikethroughSpan);
  DALI_TEST_EQUALS(strikethroughSpan.GetSpanType(), Dali::Toolkit::Text::SpanType::Value::STRIKETHROUGH, TEST_LOCATION);

  Text::BaseSpan characterSpacingSpan = Text::CharacterSpacingSpan::New(5.2f);
  DALI_TEST_CHECK(characterSpacingSpan);
  DALI_TEST_EQUALS(characterSpacingSpan.GetSpanType(), Dali::Toolkit::Text::SpanType::Value::CHARACTER_SPACING, TEST_LOCATION);

  END_TEST;
}

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
#include <dali-toolkit/devel-api/text/spans/font-span.h>

using namespace Dali;
using namespace Toolkit;

int UtcDaliToolkitTextFontSpanNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFontSpanNew");

  auto fontSpan = Text::FontSpan::New("DejaVu Sans",
                                      45.0f,
                                      Dali::TextAbstraction::FontWeight::BOLD,
                                      Dali::TextAbstraction::FontWidth::CONDENSED,
                                      Dali::TextAbstraction::FontSlant::ITALIC);
  DALI_TEST_CHECK(fontSpan);

  END_TEST;
}

int UtcDaliToolkitTextGetFamilyName(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextGetFamilyName");

  auto fontSpan = Text::FontSpan::New("DejaVu Sans",
                                      45.0f,
                                      Dali::TextAbstraction::FontWeight::BOLD,
                                      Dali::TextAbstraction::FontWidth::CONDENSED,
                                      Dali::TextAbstraction::FontSlant::ITALIC);
  DALI_TEST_CHECK(fontSpan);

  DALI_TEST_EQUALS("DejaVu Sans", fontSpan.GetFamilyName(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextIsFamilyNameDefined(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextIsFamilyNameDefined");

  auto fontSpan = Text::FontSpan::New("DejaVu Sans",
                                      45.0f,
                                      Dali::TextAbstraction::FontWeight::BOLD,
                                      Dali::TextAbstraction::FontWidth::CONDENSED,
                                      Dali::TextAbstraction::FontSlant::ITALIC);
  DALI_TEST_CHECK(fontSpan);

  DALI_TEST_EQUALS(true, fontSpan.IsFamilyNameDefined(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextGetWeight(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextGetWeight");

  auto fontSpan = Text::FontSpan::New("DejaVu Sans",
                                      45.0f,
                                      Dali::TextAbstraction::FontWeight::BOLD,
                                      Dali::TextAbstraction::FontWidth::CONDENSED,
                                      Dali::TextAbstraction::FontSlant::ITALIC);
  DALI_TEST_CHECK(fontSpan);

  DALI_TEST_EQUALS(Dali::TextAbstraction::FontWeight::BOLD, fontSpan.GetWeight(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextIsWeightDefined(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextIsWeightDefined");

  auto fontSpan = Text::FontSpan::New("DejaVu Sans",
                                      45.0f,
                                      Dali::TextAbstraction::FontWeight::BOLD,
                                      Dali::TextAbstraction::FontWidth::CONDENSED,
                                      Dali::TextAbstraction::FontSlant::ITALIC);
  DALI_TEST_CHECK(fontSpan);

  DALI_TEST_EQUALS(true, fontSpan.IsWeightDefined(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextGetWidth(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextGetWidth");

  auto fontSpan = Text::FontSpan::New("DejaVu Sans",
                                      45.0f,
                                      Dali::TextAbstraction::FontWeight::BOLD,
                                      Dali::TextAbstraction::FontWidth::CONDENSED,
                                      Dali::TextAbstraction::FontSlant::ITALIC);
  DALI_TEST_CHECK(fontSpan);

  DALI_TEST_EQUALS(Dali::TextAbstraction::FontWidth::CONDENSED, fontSpan.GetWidth(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextIsWidthDefined(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextIsWidthDefined");

  auto fontSpan = Text::FontSpan::New("DejaVu Sans",
                                      45.0f,
                                      Dali::TextAbstraction::FontWeight::BOLD,
                                      Dali::TextAbstraction::FontWidth::CONDENSED,
                                      Dali::TextAbstraction::FontSlant::ITALIC);
  DALI_TEST_CHECK(fontSpan);

  DALI_TEST_EQUALS(true, fontSpan.IsWidthDefined(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextGetSlant(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextGetSlant");

  auto fontSpan = Text::FontSpan::New("DejaVu Sans",
                                      45.0f,
                                      Dali::TextAbstraction::FontWeight::BOLD,
                                      Dali::TextAbstraction::FontWidth::CONDENSED,
                                      Dali::TextAbstraction::FontSlant::ITALIC);
  DALI_TEST_CHECK(fontSpan);

  DALI_TEST_EQUALS(Dali::TextAbstraction::FontSlant::ITALIC, fontSpan.GetSlant(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextIsSlantDefined(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextIsSlantDefined");

  auto fontSpan = Text::FontSpan::New("DejaVu Sans",
                                      45.0f,
                                      Dali::TextAbstraction::FontWeight::BOLD,
                                      Dali::TextAbstraction::FontWidth::CONDENSED,
                                      Dali::TextAbstraction::FontSlant::ITALIC);
  DALI_TEST_CHECK(fontSpan);

  DALI_TEST_EQUALS(true, fontSpan.IsSlantDefined(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextGetSize(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextGetSize");

  auto fontSpan = Text::FontSpan::New("DejaVu Sans",
                                      45.0f,
                                      Dali::TextAbstraction::FontWeight::BOLD,
                                      Dali::TextAbstraction::FontWidth::CONDENSED,
                                      Dali::TextAbstraction::FontSlant::ITALIC);
  DALI_TEST_CHECK(fontSpan);

  DALI_TEST_EQUALS(45.0f, fontSpan.GetSize(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextIsSizeDefined(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextIsSizeDefined");

  auto fontSpan = Text::FontSpan::New("DejaVu Sans",
                                      45.0f,
                                      Dali::TextAbstraction::FontWeight::BOLD,
                                      Dali::TextAbstraction::FontWidth::CONDENSED,
                                      Dali::TextAbstraction::FontSlant::ITALIC);
  DALI_TEST_CHECK(fontSpan);

  DALI_TEST_EQUALS(true, fontSpan.IsSizeDefined(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextFontSpanDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextFontSpanDownCast");

  Text::BaseSpan baseSpan = Text::FontSpan::New("DejaVu Sans",
                                                45.0f,
                                                Dali::TextAbstraction::FontWeight::BOLD,
                                                Dali::TextAbstraction::FontWidth::CONDENSED,
                                                Dali::TextAbstraction::FontSlant::ITALIC);
  DALI_TEST_CHECK(baseSpan);

  Text::FontSpan greenSpan = Text::FontSpan::DownCast(baseSpan);
  DALI_TEST_CHECK(greenSpan);

  END_TEST;
}

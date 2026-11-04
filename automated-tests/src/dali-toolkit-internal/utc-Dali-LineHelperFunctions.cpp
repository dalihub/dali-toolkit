/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/line-helper-functions.h>
#include <dali-toolkit/internal/text/line-run.h>

using namespace Dali::Toolkit;
using namespace Dali::Toolkit::Text;

int UtcDaliGetPreOffsetVerticalLineAlignmentWithNegativeLineSpacing(void)
{
  LineRun line{};
  line.lineSpacing = -20.f;
  DALI_TEST_EQUALS(GetPreOffsetVerticalLineAlignment(line, DevelText::VerticalLineAlignment::TOP), 0.f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPreOffsetVerticalLineAlignment(line, DevelText::VerticalLineAlignment::MIDDLE), 0.f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPreOffsetVerticalLineAlignment(line, DevelText::VerticalLineAlignment::BOTTOM), 0.f, TEST_LOCATION);
  line.lineSpacing = 0.f;
  DALI_TEST_EQUALS(GetPreOffsetVerticalLineAlignment(line, DevelText::VerticalLineAlignment::TOP), 0.f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPreOffsetVerticalLineAlignment(line, DevelText::VerticalLineAlignment::MIDDLE), 0.f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPreOffsetVerticalLineAlignment(line, DevelText::VerticalLineAlignment::BOTTOM), 0.f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGetPreOffsetVerticalLineAlignmentWithPositiveLineSpacing(void)
{
  LineRun line{};
  line.lineSpacing = 20.f;
  DALI_TEST_EQUALS(GetPreOffsetVerticalLineAlignment(line, DevelText::VerticalLineAlignment::TOP), 0.f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPreOffsetVerticalLineAlignment(line, DevelText::VerticalLineAlignment::MIDDLE), 10.f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPreOffsetVerticalLineAlignment(line, DevelText::VerticalLineAlignment::BOTTOM), 20.f, TEST_LOCATION);
  line.lineSpacing = 0.f;
  DALI_TEST_EQUALS(GetPreOffsetVerticalLineAlignment(line, DevelText::VerticalLineAlignment::TOP), 0.f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPreOffsetVerticalLineAlignment(line, DevelText::VerticalLineAlignment::MIDDLE), 0.f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPreOffsetVerticalLineAlignment(line, DevelText::VerticalLineAlignment::BOTTOM), 0.f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGetPostOffsetVerticalLineAlignmentWithNegativeLineSpacing(void)
{
  LineRun line{};
  line.lineSpacing = -20.f;
  DALI_TEST_EQUALS(GetPostOffsetVerticalLineAlignment(line, DevelText::VerticalLineAlignment::TOP), -20.f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPostOffsetVerticalLineAlignment(line, DevelText::VerticalLineAlignment::MIDDLE), -20.f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPostOffsetVerticalLineAlignment(line, DevelText::VerticalLineAlignment::BOTTOM), -20.f, TEST_LOCATION);
  line.lineSpacing = 0.f;
  DALI_TEST_EQUALS(GetPostOffsetVerticalLineAlignment(line, DevelText::VerticalLineAlignment::TOP), 0.f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPostOffsetVerticalLineAlignment(line, DevelText::VerticalLineAlignment::MIDDLE), 0.f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPostOffsetVerticalLineAlignment(line, DevelText::VerticalLineAlignment::BOTTOM), 0.f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGetPostOffsetVerticalLineAlignmentWithPositiveLineSpacing(void)
{
  LineRun line{};
  line.lineSpacing = 20.f;
  DALI_TEST_EQUALS(GetPostOffsetVerticalLineAlignment(line, DevelText::VerticalLineAlignment::TOP), 20.f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPostOffsetVerticalLineAlignment(line, DevelText::VerticalLineAlignment::MIDDLE), 10.f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPostOffsetVerticalLineAlignment(line, DevelText::VerticalLineAlignment::BOTTOM), 0.f, TEST_LOCATION);
  line.lineSpacing = 0.f;
  DALI_TEST_EQUALS(GetPostOffsetVerticalLineAlignment(line, DevelText::VerticalLineAlignment::TOP), 0.f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPostOffsetVerticalLineAlignment(line, DevelText::VerticalLineAlignment::MIDDLE), 0.f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetPostOffsetVerticalLineAlignment(line, DevelText::VerticalLineAlignment::BOTTOM), 0.f, TEST_LOCATION);
  END_TEST;
}

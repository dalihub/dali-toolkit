/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include <iostream>
#include <stdlib.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Toolkit;

void dali_textselectionpopup_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_textselectionpopup_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliToolkitTextSelectionPopupNewP(void)
{
  ToolkitTestApplication application;
  TextSelectionPopup textSelectionPopup;

  DALI_TEST_CHECK( !textSelectionPopup );

  textSelectionPopup = TextSelectionPopup::New();

  DALI_TEST_CHECK( textSelectionPopup );
  END_TEST;
}

int UtcDaliToolkitTextSelectionPopupConstructorP(void)
{
  TextSelectionPopup textSelectionPopup;

  DALI_TEST_CHECK( !textSelectionPopup );

  END_TEST;
}

int UtcDaliToolkitTextSelectionPopupCopyConstructorP(void)
{
  ToolkitTestApplication application;
  TextSelectionPopup textSelectionPopup;

  textSelectionPopup = TextSelectionPopup::New();
  TextSelectionPopup copy( textSelectionPopup );

  DALI_TEST_CHECK( copy == textSelectionPopup );

  END_TEST;
}


int UtcDaliToolkitTextSelectionPopupDestructorP(void)
{
  ToolkitTestApplication application;
  TextSelectionPopup* textSelectionPopup = new TextSelectionPopup;
  delete textSelectionPopup;

  DALI_TEST_CHECK( true );

  END_TEST;
}

int UtcDaliToolkitTextSelectionPopupAssignmentOperatorP(void)
{
  ToolkitTestApplication application;
  TextSelectionPopup textSelectionPopup;
  textSelectionPopup = TextSelectionPopup::New();
  TextSelectionPopup copy;
  copy = textSelectionPopup;

  DALI_TEST_CHECK( copy == textSelectionPopup );
  END_TEST;
}

int UtcDaliToolkitTextSelectionPopupDownCastP(void)
{
  ToolkitTestApplication application;
  TextSelectionPopup textSelectionPopup;
  textSelectionPopup = TextSelectionPopup::New();

  TextSelectionPopup cast = TextSelectionPopup::DownCast( textSelectionPopup );

  DALI_TEST_CHECK( cast );

  END_TEST;
}

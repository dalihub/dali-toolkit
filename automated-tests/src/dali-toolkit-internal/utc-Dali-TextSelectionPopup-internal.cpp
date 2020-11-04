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

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-selection-popup.h>

// THIS IS THE HEADER WE ARE TESTING
#define private public // To access private members of class, just in the internal test cases
#include <dali-toolkit/internal/controls/text-controls/text-selection-popup-impl.h>
#undef private

#if defined(ELDBUS_ENABLED)
#include <automated-tests/src/dali-toolkit-internal/dali-toolkit-test-utils/dbus-wrapper.h>
#endif

using namespace Dali;
using namespace Toolkit;

void dali_textselectionpopupinternal_startup(void)
{
  test_return_value = TET_UNDEF;
#if defined(ELDBUS_ENABLED)
  DBusWrapper::Install(std::unique_ptr<DBusWrapper>(new TestDBusWrapper));
#endif
}

void dali_textselectionpopupinternal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliToolkitTextSelectionPopupIconPropertiesN(void)
{
  ToolkitTestApplication application;
  TextSelectionPopup popup = TextSelectionPopup::New(nullptr);

  Toolkit::Internal::TextSelectionPopup& popupImpl = GetImpl(popup);
  DALI_TEST_EQUALS(std::string(), popupImpl.GetButtonImage(Toolkit::TextSelectionPopup::NONE), TEST_LOCATION);
  DALI_TEST_EQUALS(std::string(), popupImpl.GetButtonImage(Toolkit::TextSelectionPopup::Buttons(-1)), TEST_LOCATION);

  END_TEST;
}

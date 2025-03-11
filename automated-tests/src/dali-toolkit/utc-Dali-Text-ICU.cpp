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

#include <toolkit-environment-variable.h>
#include <toolkit-event-thread-callback.h>
#include <toolkit-icu.h>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-label-devel.h>
#include <dali-toolkit/internal/text/async-text/async-text-manager.h>
#include <dali-toolkit/internal/text/multi-language-support.h>
#include <locale.h>
#include <iostream>

using namespace Dali;
using namespace Toolkit;

namespace
{
static int   ASYNC_TEXT_THREAD_TIMEOUT = 5;
static bool  gAsyncTextRenderedCalled;
static float gAsyncTextRenderedWidth;
static float gAsyncTextRenderedHeight;

struct CallbackFunctor
{
  CallbackFunctor(bool* callbackFlag)
  : mCallbackFlag(callbackFlag)
  {
  }

  void operator()()
  {
    *mCallbackFlag = true;
  }
  bool* mCallbackFlag;
};

static void TestAsyncTextRendered(TextLabel control, float width, float height)
{
  tet_infoline(" TestAsyncTextRendered");
  gAsyncTextRenderedCalled = true;
  gAsyncTextRenderedWidth  = width;
  gAsyncTextRenderedHeight = height;
}

} // namespace

void dali_text_icu_startup(void)
{
  EnvironmentVariable::SetTestEnvironmentVariable("DALI_TEXT_ENABLE_ICU", "1");

  test_return_value = TET_UNDEF;
}

void dali_text_icu_cleanup(void)
{
  EnvironmentVariable::SetTestEnvironmentVariable("DALI_TEXT_ENABLE_ICU", "0");

  test_return_value = TET_PASS;
}

int UtcDaliToolkitTextICU(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextICU");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Create MultilanguageSupport & AsyncTextManager to get LocaleChangedSignal.
  Text::MultilanguageSupport::Get();
  Text::AsyncTextManager::Get();

  Adaptor &adaptor = application.GetAdaptor();
  std::string locale = "en_US";
  adaptor.LocaleChangedSignal().Emit(locale);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  float expectedWidth = 300.0f;
  float expectedHeight = 300.0f;

  // Sync test.
  label.SetProperty(TextLabel::Property::TEXT, "ฉันเป็นเอาต์บอกความร้อนของสิ่งที่ถูกหล่อหลอม ฉันไม่สามารถพูดภาษาไทยได้จริงๆ แต่ฉันสามารถช่วยเหลือคุณในการสร้างคำบรรยายสำหรับภาษาไทยได้ แต่ฉันจะต้องพึ่งความช่วยเหลือของ");
  label.SetProperty(Actor::Property::SIZE, Vector2(expectedWidth, expectedHeight));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 12);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);
  application.GetScene().Add(label);

  label.SetProperty(TextLabel::Property::LINE_WRAP_MODE, "WORD");
  DALI_TEST_EQUALS(static_cast<int>(Text::LineWrap::WORD), label.GetProperty<int>(TextLabel::Property::LINE_WRAP_MODE), TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(0u, Test::ICU::GetInitializedCount(), TEST_LOCATION);

  // Locale changed
  std::string newLocale = "th_TH";
  adaptor.LocaleChangedSignal().Emit(newLocale);

  label.SetProperty(TextLabel::Property::TEXT, "ฉันเป็นเอาต์บอกความร้อนของสิ่งที่ถูกหล่อหลอม ฉันไม่สามารถพูดภาษาไทยได้จริงๆ แต่ฉันสามารถช่วยเหลือคุณในการสร้างคำบรรยายสำหรับภาษาไทยได้ แต่ฉันจะต้องพึ่งความช่วยเหลือของ");

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(1u, Test::ICU::GetInitializedCount(), TEST_LOCATION);

  // Async test.
  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_AUTO);

  // Connect to the async text rendered signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextLabel::AsyncTextRenderedSignal(label).Connect(&TestAsyncTextRendered);

  bool asyncTextRendered = false;
  label.ConnectSignal(testTracker, "asyncTextRendered", CallbackFunctor(&asyncTextRendered));

  gAsyncTextRenderedCalled = false;
  gAsyncTextRenderedWidth  = 0.0f;
  gAsyncTextRenderedHeight = 0.0f;

  // Request render automatically.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  DALI_TEST_EQUALS(expectedWidth, gAsyncTextRenderedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncTextRenderedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(false, label.GetProperty<bool>(DevelTextLabel::Property::MANUAL_RENDERED), TEST_LOCATION);
  DALI_TEST_EQUALS(2u, Test::ICU::GetInitializedCount(), TEST_LOCATION);

  application.SendNotification();
  application.Render();

  Test::ICU::ResetInitializeFlag();
  DALI_TEST_EQUALS(0u, Test::ICU::GetInitializedCount(), TEST_LOCATION);

  tet_result(TET_PASS);
  END_TEST;
}

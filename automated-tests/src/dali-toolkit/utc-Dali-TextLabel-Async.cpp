/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

#include <toolkit-event-thread-callback.h>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-label-devel.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-style-properties-devel.h>
#include <dali-toolkit/devel-api/text/bitmap-font.h>
#include <dali-toolkit/devel-api/text/rendering-backend.h>
#include <dali-toolkit/devel-api/text/text-enumerations-devel.h>
#include <dali-toolkit/devel-api/text/text-utils-devel.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/text-abstraction/bitmap-font.h>
#include <dali/devel-api/text-abstraction/font-client.h>
#include "test-text-geometry-utils.h"

using namespace Dali;
using namespace Toolkit;

void dali_textlabel_async_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_textlabel_async_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
const std::string DEFAULT_FONT_DIR("/resources/fonts");

static int   ASYNC_TEXT_THREAD_TIMEOUT = 5;

static bool  gAsyncTextRenderedCalled;
static float gAsyncTextRenderedWidth;
static float gAsyncTextRenderedHeight;

static bool  gAsyncSizeComputedCalled;
static float gAsyncSizeComputedWidth;
static float gAsyncSizeComputedHeight;

static bool  gTextFitChangedCalled;

float ConvertToEven(float value)
{
  int intValue(static_cast<int>(value));
  return static_cast<float>(intValue + (intValue & 1));
}

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

static void TestAsyncSizeComputed(TextLabel control, float width, float height)
{
  tet_infoline(" TestAsyncSizeComputed");
  gAsyncSizeComputedCalled = true;
  gAsyncSizeComputedWidth  = width;
  gAsyncSizeComputedHeight = height;
}

static void TestTextFitChanged(TextLabel control)
{
  tet_infoline(" TestTextFitChanged");
  gTextFitChangedCalled = true;
}

} // namespace

int UtcDaliToolkitTextLabelAsyncRender01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelAsyncRender01");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  char*             pathNamePtr = get_current_dir_name();
  const std::string pathName(pathNamePtr);
  free(pathNamePtr);

  TextAbstraction::FontDescription fontDescription;
  std::string fontPath = pathName + DEFAULT_FONT_DIR + "/tizen/BreezeColorEmoji.ttf";

  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.AddCustomFontDirectory(fontPath);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  float expectedWidth  = 300.0f;
  float expectedHeight = 300.0f;

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_AUTO);
  label.SetProperty(TextLabel::Property::TEXT, "Hello world Hello world");
  label.SetProperty(Actor::Property::SIZE, Vector2(expectedWidth, expectedHeight));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 12);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);
  application.GetScene().Add(label);

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

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliToolkitTextLabelAsyncRender02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelAsyncRender02");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  float expectedWidth  = 300.0f;
  float expectedHeight = 300.0f;

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_AUTO);
  label.SetProperty(TextLabel::Property::TEXT, "Hello world Hello world");
  label.SetProperty(Actor::Property::SIZE, Vector2(expectedWidth, expectedHeight));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 12);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);

  Property::Map underline;
  underline.Clear();
  underline.Insert("enable", true);
  underline.Insert("color", Color::RED);
  underline.Insert("height", 1);
  label.SetProperty(TextLabel::Property::UNDERLINE, underline);

  Property::Map strikethrough;
  strikethrough.Clear();
  strikethrough.Insert("enable", true);
  strikethrough.Insert("color", Color::BLUE);
  strikethrough.Insert("height", 2.0f);
  label.SetProperty(DevelTextLabel::Property::STRIKETHROUGH, strikethrough);

  Property::Map outline;
  outline["color"] = Color::GREEN;
  outline["width"] = 2.0f;
  outline["offset"] = Vector2(2.0f, 2.0f);
  outline["blurRadius"] = 3.0f;
  label.SetProperty(TextLabel::Property::OUTLINE, outline);

  Property::Map shadow;
  shadow.Insert("color", Color::BLACK);
  shadow.Insert("offset", Vector2(1.0f, 1.0f));
  label.SetProperty(TextLabel::Property::SHADOW, shadow);

  application.GetScene().Add(label);

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

  application.SendNotification();
  application.Render();

  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;
  gAsyncTextRenderedWidth  = 0.0f;
  gAsyncTextRenderedHeight = 0.0f;
  expectedWidth            = 150.0f;

  label.SetProperty(Actor::Property::SIZE_WIDTH, expectedWidth);
  label.SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");
  label.SetProperty(TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER");

  // Request render automatically.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  DALI_TEST_EQUALS(expectedWidth, gAsyncTextRenderedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncTextRenderedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(false, label.GetProperty<bool>(DevelTextLabel::Property::MANUAL_RENDERED), TEST_LOCATION);

  application.SendNotification();
  application.Render();

  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;
  gAsyncTextRenderedWidth  = 0.0f;
  gAsyncTextRenderedHeight = 0.0f;
  expectedHeight           = 150.0f;

  label.SetProperty(Actor::Property::SIZE_HEIGHT, expectedHeight);
  label.SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, "BEGIN");
  label.SetProperty(TextLabel::Property::VERTICAL_ALIGNMENT, "BOTTOM");

  // Request render automatically.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  DALI_TEST_EQUALS(expectedWidth, gAsyncTextRenderedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncTextRenderedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(false, label.GetProperty<bool>(DevelTextLabel::Property::MANUAL_RENDERED), TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliToolkitTextLabelRequestAsyncRenderWithFixedSize01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelRequestAsyncRenderWithFixedSize01");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  label.SetProperty(TextLabel::Property::TEXT, "Hello world Hello world");
  label.SetProperty(Actor::Property::SIZE, Vector2(300.0f, 300.0f));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 12);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);
  application.GetScene().Add(label);

  // Connect to the async text rendered signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextLabel::AsyncTextRenderedSignal(label).Connect(&TestAsyncTextRendered);

  bool asyncTextRendered = false;
  label.ConnectSignal(testTracker, "asyncTextRendered", CallbackFunctor(&asyncTextRendered));

  gAsyncTextRenderedCalled = false;
  gAsyncTextRenderedWidth  = 0.0f;
  gAsyncTextRenderedHeight = 0.0f;

  float expectedWidth  = 200.0f;
  float expectedHeight = 200.0f;

  // Request render.
  DevelTextLabel::RequestAsyncRenderWithFixedSize(label, expectedWidth, expectedHeight);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  DALI_TEST_EQUALS(expectedWidth, gAsyncTextRenderedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncTextRenderedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(true, label.GetProperty<bool>(DevelTextLabel::Property::MANUAL_RENDERED), TEST_LOCATION);

  // RTL test.
  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;
  gAsyncTextRenderedWidth  = 0.0f;
  gAsyncTextRenderedHeight = 0.0f;

  label.SetProperty(TextLabel::Property::TEXT, "Update paragraphs with different directions. Update middle paragraphs. مرحبا بالعالم שלום עולם مرحبا بالعالم Hello world.");
  label.SetProperty(Actor::Property::LAYOUT_DIRECTION, LayoutDirection::RIGHT_TO_LEFT);

  // Request render.
  DevelTextLabel::RequestAsyncRenderWithFixedSize(label, expectedWidth, expectedHeight);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  DALI_TEST_EQUALS(expectedWidth, gAsyncTextRenderedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncTextRenderedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(true, label.GetProperty<bool>(DevelTextLabel::Property::MANUAL_RENDERED), TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliToolkitTextLabelRequestAsyncRenderWithFixedSize02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelRequestAsyncRenderWithFixedSize02");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Set the dpi of AsyncTextLoader and FontClient to be identical.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(0u, 0u);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  float labelWidth  = 300.0f;
  float labelHeight = 300.0f;

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::SYNC);
  label.SetProperty(TextLabel::Property::TEXT, "Hello world Hello world");
  label.SetProperty(Actor::Property::SIZE, Vector2(labelWidth, labelHeight));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 12);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);
  application.GetScene().Add(label);

  // Connect to the async text rendered signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextLabel::AsyncTextRenderedSignal(label).Connect(&TestAsyncTextRendered);

  bool asyncTextRendered = false;
  label.ConnectSignal(testTracker, "asyncTextRendered", CallbackFunctor(&asyncTextRendered));
  gAsyncTextRenderedCalled = false;

  // Connect to the async natural size computed signal.
  ConnectionTracker* testSizeTracker = new ConnectionTracker();
  DevelTextLabel::AsyncNaturalSizeComputedSignal(label).Connect(&TestAsyncSizeComputed);

  bool asyncSizeComputed = false;
  label.ConnectSignal(testSizeTracker, "asyncNaturalSizeComputed", CallbackFunctor(&asyncSizeComputed));

  gAsyncSizeComputedCalled = false;
  gAsyncSizeComputedWidth  = 0.0f;
  gAsyncSizeComputedHeight = 0.0f;

  // Request render, but the request will cancelled due to being in sync mode.
  DevelTextLabel::RequestAsyncRenderWithFixedSize(label, labelWidth, labelHeight);

  // Async size computation also works in sync mode.
  DevelTextLabel::RequestAsyncNaturalSize(label);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(!gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(!asyncTextRendered);

  DALI_TEST_CHECK(gAsyncSizeComputedCalled);
  DALI_TEST_CHECK(asyncSizeComputed);

  float expectedWidth  = label.GetNaturalSize().width;
  float expectedHeight = label.GetNaturalSize().height;

  gAsyncSizeComputedWidth  = ConvertToEven(gAsyncSizeComputedWidth);
  gAsyncSizeComputedHeight = ConvertToEven(gAsyncSizeComputedHeight);

  DALI_TEST_EQUALS(expectedWidth, gAsyncSizeComputedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncSizeComputedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextLabelRequestAsyncRenderWithFixedWidth01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelRequestAsyncRenderWithFixedWidth01");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Set the dpi of AsyncTextLoader and FontClient to be identical.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(0u, 0u);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  label.SetProperty(TextLabel::Property::TEXT, "Hello world Hello world");
  label.SetProperty(Actor::Property::SIZE, Vector2(300.0f, 300.0f));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 12);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);
  application.GetScene().Add(label);

  // Connect to the async text rendered signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextLabel::AsyncTextRenderedSignal(label).Connect(&TestAsyncTextRendered);

  bool asyncTextRendered = false;
  label.ConnectSignal(testTracker, "asyncTextRendered", CallbackFunctor(&asyncTextRendered));

  gAsyncTextRenderedCalled = false;
  gAsyncTextRenderedWidth  = 0.0f;
  gAsyncTextRenderedHeight = 0.0f;

  // Request render.
  DevelTextLabel::RequestAsyncRenderWithFixedWidth(label, 300.0f, std::numeric_limits<float>::infinity());

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  float expectedWidth  = 300.0f;
  float expectedHeight = label.GetHeightForWidth(expectedWidth);

  DALI_TEST_EQUALS(expectedWidth, gAsyncTextRenderedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncTextRenderedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(true, label.GetProperty<bool>(DevelTextLabel::Property::MANUAL_RENDERED), TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // RTL test.
  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;
  gAsyncTextRenderedWidth  = 0.0f;
  gAsyncTextRenderedHeight = 0.0f;

  label.SetProperty(TextLabel::Property::TEXT, "Update paragraphs with different directions. Update middle paragraphs. مرحبا بالعالم שלום עולם مرحبا بالعالم Hello world.");
  label.SetProperty(Actor::Property::LAYOUT_DIRECTION, LayoutDirection::RIGHT_TO_LEFT);

  // Request render.
  DevelTextLabel::RequestAsyncRenderWithFixedWidth(label, 300.0f, std::numeric_limits<float>::infinity());

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  expectedHeight = label.GetHeightForWidth(expectedWidth);

  DALI_TEST_EQUALS(expectedWidth, gAsyncTextRenderedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncTextRenderedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(true, label.GetProperty<bool>(DevelTextLabel::Property::MANUAL_RENDERED), TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliToolkitTextLabelRequestAsyncRenderWithFixedWidth02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelRequestAsyncRenderWithFixedWidth02");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Set the dpi of AsyncTextLoader and FontClient to be identical.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(0u, 0u);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  float labelWidth  = 300.0f;
  float labelHeight = 300.0f;

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::SYNC);
  label.SetProperty(TextLabel::Property::TEXT, "Hello world Hello world");
  label.SetProperty(Actor::Property::SIZE, Vector2(labelWidth, labelHeight));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 12);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);
  application.GetScene().Add(label);

  // Connect to the async text rendered signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextLabel::AsyncTextRenderedSignal(label).Connect(&TestAsyncTextRendered);

  bool asyncTextRendered = false;
  label.ConnectSignal(testTracker, "asyncTextRendered", CallbackFunctor(&asyncTextRendered));
  gAsyncTextRenderedCalled = false;

  // Connect to the async natural size computed signal.
  ConnectionTracker* testSizeTracker = new ConnectionTracker();
  DevelTextLabel::AsyncNaturalSizeComputedSignal(label).Connect(&TestAsyncSizeComputed);

  bool asyncSizeComputed = false;
  label.ConnectSignal(testSizeTracker, "asyncNaturalSizeComputed", CallbackFunctor(&asyncSizeComputed));

  gAsyncSizeComputedCalled = false;
  gAsyncSizeComputedWidth  = 0.0f;
  gAsyncSizeComputedHeight = 0.0f;

  // Request render, but the request will cancelled due to being in sync mode.
  DevelTextLabel::RequestAsyncRenderWithFixedWidth(label, labelWidth, std::numeric_limits<float>::infinity());

  // Async size computation also works in sync mode.
  DevelTextLabel::RequestAsyncNaturalSize(label);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(!gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(!asyncTextRendered);

  DALI_TEST_CHECK(gAsyncSizeComputedCalled);
  DALI_TEST_CHECK(asyncSizeComputed);

  float expectedWidth  = label.GetNaturalSize().width;
  float expectedHeight = label.GetNaturalSize().height;

  gAsyncSizeComputedWidth  = ConvertToEven(gAsyncSizeComputedWidth);
  gAsyncSizeComputedHeight = ConvertToEven(gAsyncSizeComputedHeight);

  DALI_TEST_EQUALS(expectedWidth, gAsyncSizeComputedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncSizeComputedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextLabelRequestAsyncRenderWithConstraint01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelRequestAsyncRenderWithConstraint01");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Set the dpi of AsyncTextLoader and FontClient to be identical.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(0u, 0u);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  label.SetProperty(TextLabel::Property::TEXT, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.");
  label.SetProperty(Actor::Property::SIZE, Vector2(300.0f, 300.0f));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 12);
  label.SetProperty(TextLabel::Property::MULTI_LINE, false);
  application.GetScene().Add(label);

  // Connect to the async text rendered signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextLabel::AsyncTextRenderedSignal(label).Connect(&TestAsyncTextRendered);

  bool asyncTextRendered = false;
  label.ConnectSignal(testTracker, "asyncTextRendered", CallbackFunctor(&asyncTextRendered));

  gAsyncTextRenderedCalled = false;
  gAsyncTextRenderedWidth  = 0.0f;
  gAsyncTextRenderedHeight = 0.0f;

  // Request render.
  DevelTextLabel::RequestAsyncRenderWithConstraint(label, std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  float expectedWidth  = label.GetNaturalSize().width;
  float expectedHeight = label.GetNaturalSize().height;

  gAsyncTextRenderedWidth  = ConvertToEven(gAsyncTextRenderedWidth);
  gAsyncTextRenderedHeight = ConvertToEven(gAsyncTextRenderedHeight);

  DALI_TEST_EQUALS(expectedWidth, gAsyncTextRenderedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncTextRenderedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(true, label.GetProperty<bool>(DevelTextLabel::Property::MANUAL_RENDERED), TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // RTL test.
  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;
  gAsyncTextRenderedWidth  = 0.0f;
  gAsyncTextRenderedHeight = 0.0f;

  label.SetProperty(TextLabel::Property::TEXT, "Update paragraphs with different directions. Update middle paragraphs. مرحبا بالعالم שלום עולם مرحبا بالعالم Hello world.");
  label.SetProperty(Actor::Property::LAYOUT_DIRECTION, LayoutDirection::RIGHT_TO_LEFT);

  // Request render.
  DevelTextLabel::RequestAsyncRenderWithConstraint(label, std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  expectedWidth  = label.GetNaturalSize().width;
  expectedHeight = label.GetNaturalSize().height;

  gAsyncTextRenderedWidth  = ConvertToEven(gAsyncTextRenderedWidth);
  gAsyncTextRenderedHeight = ConvertToEven(gAsyncTextRenderedHeight);

  DALI_TEST_EQUALS(expectedWidth, gAsyncTextRenderedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncTextRenderedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(true, label.GetProperty<bool>(DevelTextLabel::Property::MANUAL_RENDERED), TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Constraint test.
  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;
  gAsyncTextRenderedWidth  = 0.0f;
  gAsyncTextRenderedHeight = 0.0f;

  label.SetProperty(TextLabel::Property::TEXT, "Update paragraphs with different directions. Update middle paragraphs. مرحبا بالعالم שלום עולם مرحبا بالعالم Hello world.");
  label.SetProperty(TextLabel::Property::POINT_SIZE, 30);
  label.SetProperty(Actor::Property::LAYOUT_DIRECTION, LayoutDirection::LEFT_TO_RIGHT);

  expectedWidth  = 50.0f;
  expectedHeight = label.GetNaturalSize().height;

  // Request render with small constraint
  DevelTextLabel::RequestAsyncRenderWithConstraint(label, expectedWidth, expectedHeight);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  DALI_TEST_EQUALS(expectedWidth, gAsyncTextRenderedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncTextRenderedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(true, label.GetProperty<bool>(DevelTextLabel::Property::MANUAL_RENDERED), TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliToolkitTextLabelRequestAsyncRenderWithConstraint02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelRequestAsyncRenderWithConstraint02");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Set the dpi of AsyncTextLoader and FontClient to be identical.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(0u, 0u);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  float labelWidth  = 300.0f;
  float labelHeight = 300.0f;

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::SYNC);
  label.SetProperty(TextLabel::Property::TEXT, "Hello world Hello world");
  label.SetProperty(Actor::Property::SIZE, Vector2(labelWidth, labelHeight));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 12);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);
  application.GetScene().Add(label);

  // Connect to the async text rendered signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextLabel::AsyncTextRenderedSignal(label).Connect(&TestAsyncTextRendered);

  bool asyncTextRendered = false;
  label.ConnectSignal(testTracker, "asyncTextRendered", CallbackFunctor(&asyncTextRendered));
  gAsyncTextRenderedCalled = false;

  // Connect to the async natural size computed signal.
  ConnectionTracker* testSizeTracker = new ConnectionTracker();
  DevelTextLabel::AsyncNaturalSizeComputedSignal(label).Connect(&TestAsyncSizeComputed);

  bool asyncSizeComputed = false;
  label.ConnectSignal(testSizeTracker, "asyncNaturalSizeComputed", CallbackFunctor(&asyncSizeComputed));

  gAsyncSizeComputedCalled = false;
  gAsyncSizeComputedWidth  = 0.0f;
  gAsyncSizeComputedHeight = 0.0f;

  // Request render, but the request will cancelled due to being in sync mode.
  DevelTextLabel::RequestAsyncRenderWithConstraint(label, std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());

  // Async size computation also works in sync mode.
  DevelTextLabel::RequestAsyncNaturalSize(label);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(!gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(!asyncTextRendered);

  DALI_TEST_CHECK(gAsyncSizeComputedCalled);
  DALI_TEST_CHECK(asyncSizeComputed);

  float expectedWidth  = label.GetNaturalSize().width;
  float expectedHeight = label.GetNaturalSize().height;

  gAsyncSizeComputedWidth  = ConvertToEven(gAsyncSizeComputedWidth);
  gAsyncSizeComputedHeight = ConvertToEven(gAsyncSizeComputedHeight);

  DALI_TEST_EQUALS(expectedWidth, gAsyncSizeComputedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncSizeComputedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextLabelRequestAsyncNaturalSize(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelRequestAsyncNaturalSize");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Set the dpi of AsyncTextLoader and FontClient to be identical.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(0u, 0u);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  label.SetProperty(TextLabel::Property::TEXT, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.");
  label.SetProperty(TextLabel::Property::POINT_SIZE, 20);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);

  // Connect to the async natural size computed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextLabel::AsyncNaturalSizeComputedSignal(label).Connect(&TestAsyncSizeComputed);

  bool asyncSizeComputed = false;
  label.ConnectSignal(testTracker, "asyncNaturalSizeComputed", CallbackFunctor(&asyncSizeComputed));

  gAsyncSizeComputedCalled = false;
  gAsyncSizeComputedWidth  = 0.0f;
  gAsyncSizeComputedHeight = 0.0f;

  // Request size computation.
  DevelTextLabel::RequestAsyncNaturalSize(label);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncSizeComputedCalled);
  DALI_TEST_CHECK(asyncSizeComputed);

  float expectedWidth  = label.GetNaturalSize().width;
  float expectedHeight = label.GetNaturalSize().height;

  gAsyncSizeComputedWidth  = ConvertToEven(gAsyncSizeComputedWidth);
  gAsyncSizeComputedHeight = ConvertToEven(gAsyncSizeComputedHeight);

  DALI_TEST_EQUALS(expectedWidth, gAsyncSizeComputedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncSizeComputedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // RTL test.
  label.SetProperty(TextLabel::Property::TEXT, "Update paragraphs with different directions. Update middle paragraphs. مرحبا بالعالم שלום עולם مرحبا بالعالم Hello world.");
  label.SetProperty(Actor::Property::LAYOUT_DIRECTION, LayoutDirection::RIGHT_TO_LEFT);

  asyncSizeComputed        = false;
  gAsyncSizeComputedCalled = false;
  gAsyncSizeComputedWidth  = 0.0f;
  gAsyncSizeComputedHeight = 0.0f;

  // Request size computation.
  DevelTextLabel::RequestAsyncNaturalSize(label);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncSizeComputedCalled);
  DALI_TEST_CHECK(asyncSizeComputed);

  expectedWidth  = label.GetNaturalSize().width;
  expectedHeight = label.GetNaturalSize().height;

  gAsyncSizeComputedWidth  = ConvertToEven(gAsyncSizeComputedWidth);
  gAsyncSizeComputedHeight = ConvertToEven(gAsyncSizeComputedHeight);

  DALI_TEST_EQUALS(expectedWidth, gAsyncSizeComputedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncSizeComputedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliToolkitTextLabelRequestAsyncHeightForWidth(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelRequestAsyncHeightForWidth");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Set the dpi of AsyncTextLoader and FontClient to be identical.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(0u, 0u);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  label.SetProperty(TextLabel::Property::TEXT, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.");
  label.SetProperty(Actor::Property::SIZE, Vector2(300.0f, 300.0f));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 20);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);

  // Connect to the async height for width computed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextLabel::AsyncHeightForWidthComputedSignal(label).Connect(&TestAsyncSizeComputed);

  bool asyncSizeComputed = false;
  label.ConnectSignal(testTracker, "asyncHeightForWidthComputed", CallbackFunctor(&asyncSizeComputed));

  gAsyncSizeComputedCalled = false;
  gAsyncSizeComputedWidth  = 0.0f;
  gAsyncSizeComputedHeight = 0.0f;

  // Request size computation.
  DevelTextLabel::RequestAsyncHeightForWidth(label, 300.0f);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncSizeComputedCalled);
  DALI_TEST_CHECK(asyncSizeComputed);

  float expectedWidth     = 300.0f;
  float expectedHeight    = label.GetHeightForWidth(expectedWidth);
  int   expectedLineCount = label.GetProperty<int>(TextLabel::Property::LINE_COUNT);
  int   asyncLineCount    = label.GetProperty<int>(DevelTextLabel::Property::ASYNC_LINE_COUNT);

  DALI_TEST_EQUALS(expectedWidth, gAsyncSizeComputedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncSizeComputedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedLineCount, asyncLineCount, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // RTL test.
  label.SetProperty(TextLabel::Property::TEXT, "Update paragraphs with different directions. Update middle paragraphs. مرحبا بالعالم שלום עולם مرحبا بالعالم Hello world.");
  label.SetProperty(TextLabel::Property::POINT_SIZE, 14);
  label.SetProperty(Actor::Property::LAYOUT_DIRECTION, LayoutDirection::RIGHT_TO_LEFT);

  asyncSizeComputed        = false;
  gAsyncSizeComputedCalled = false;
  gAsyncSizeComputedWidth  = 0.0f;
  gAsyncSizeComputedHeight = 0.0f;

  // Request size computation.
  DevelTextLabel::RequestAsyncHeightForWidth(label, 300.0f);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncSizeComputedCalled);
  DALI_TEST_CHECK(asyncSizeComputed);

  expectedWidth     = 300.0f;
  expectedHeight    = label.GetHeightForWidth(expectedWidth);
  expectedLineCount = label.GetProperty<int>(TextLabel::Property::LINE_COUNT);
  asyncLineCount    = label.GetProperty<int>(DevelTextLabel::Property::ASYNC_LINE_COUNT);

  DALI_TEST_EQUALS(expectedWidth, gAsyncSizeComputedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncSizeComputedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedLineCount, asyncLineCount, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliToolkitTextLabelAsyncRenderTextFit01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelAsyncRenderTextFit01");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Set the dpi of AsyncTextLoader and FontClient to be identical.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(0u, 0u);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  float labelWidth  = 300.0f;
  float labelHeight = 300.0f;

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_AUTO);
  label.SetProperty(TextLabel::Property::TEXT, "Hello");
  label.SetProperty(Actor::Property::SIZE, Vector2(labelWidth, labelHeight));
  label.SetProperty(TextLabel::Property::MULTI_LINE, false);

  // Text fit
  Property::Map textFitMapSet;
  textFitMapSet["enable"]       = true;
  textFitMapSet["minSize"]      = 10.f;
  textFitMapSet["maxSize"]      = 30.f;
  textFitMapSet["stepSize"]     = 5.f;
  textFitMapSet["fontSizeType"] = "pointSize";
  label.SetProperty(Toolkit::DevelTextLabel::Property::TEXT_FIT, textFitMapSet);

  application.GetScene().Add(label);

  // Connect to the async text rendered signal.
  ConnectionTracker* testRenderTracker = new ConnectionTracker();
  DevelTextLabel::AsyncTextRenderedSignal(label).Connect(&TestAsyncTextRendered);

  bool asyncTextRendered = false;
  label.ConnectSignal(testRenderTracker, "asyncTextRendered", CallbackFunctor(&asyncTextRendered));
  gAsyncTextRenderedCalled = false;

  // Connect to the text fit changed signal.
  ConnectionTracker* testFitTracker = new ConnectionTracker();
  DevelTextLabel::TextFitChangedSignal(label).Connect(&TestTextFitChanged);

  bool textFitChanged = false;
  label.ConnectSignal(testFitTracker, "textFitChanged", CallbackFunctor(&textFitChanged));
  gTextFitChangedCalled = false;

  // Request render automatically.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  DALI_TEST_CHECK(gTextFitChangedCalled);
  DALI_TEST_CHECK(textFitChanged);

  float textFitFontSize = (label.GetProperty(Dali::Toolkit::DevelTextLabel::Property::TEXT_FIT).Get<Property::Map>())["fontSize"].Get<float>();

  // To measure expected size.
  TextLabel sizeLabel = TextLabel::New();
  DALI_TEST_CHECK(sizeLabel);

  sizeLabel.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::SYNC);
  sizeLabel.SetProperty(TextLabel::Property::TEXT, "Hello");
  sizeLabel.SetProperty(TextLabel::Property::MULTI_LINE, false);
  sizeLabel.SetProperty(TextLabel::Property::POINT_SIZE, textFitFontSize);

  DALI_TEST_EQUALS(sizeLabel.GetNaturalSize(), label.GetNaturalSize(), TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Text fit array, make sorted options.
  std::vector<DevelTextLabel::FitOption> fitOptions;
  fitOptions.push_back(DevelTextLabel::FitOption(10, 15));
  fitOptions.push_back(DevelTextLabel::FitOption(15, 20));
  fitOptions.push_back(DevelTextLabel::FitOption(20, 25));
  fitOptions.push_back(DevelTextLabel::FitOption(25, 30));
  fitOptions.push_back(DevelTextLabel::FitOption(30, 35));
  DevelTextLabel::SetTextFitArray(label, true, fitOptions);

  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;

  // Request render automatically.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  DALI_TEST_EQUALS(sizeLabel.GetNaturalSize(), label.GetNaturalSize(), TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // Text fit array, empty fit options.
  std::vector<DevelTextLabel::FitOption> emptyFitOptions;
  DevelTextLabel::SetTextFitArray(label, false, emptyFitOptions);

  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;

  // Request render automatically.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  // Text fit array, empty fit options, negative.
  DevelTextLabel::SetTextFitArray(label, true, emptyFitOptions);

  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;

  // Request render automatically.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  // Text fit array, make unsorted options.
  std::vector<DevelTextLabel::FitOption> unsortedFitOptions;
  unsortedFitOptions.push_back(DevelTextLabel::FitOption(10, 15));
  unsortedFitOptions.push_back(DevelTextLabel::FitOption(15, 10));
  unsortedFitOptions.push_back(DevelTextLabel::FitOption(20, 25));
  unsortedFitOptions.push_back(DevelTextLabel::FitOption(25, 30));
  unsortedFitOptions.push_back(DevelTextLabel::FitOption(30, 35));
  DevelTextLabel::SetTextFitArray(label, true, unsortedFitOptions);

  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;

  // Request render automatically.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  DALI_TEST_EQUALS(sizeLabel.GetNaturalSize(), label.GetNaturalSize(), TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliToolkitTextLabelAsyncRenderTextFit02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelAsyncRenderTextFit02");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Set the dpi of AsyncTextLoader and FontClient to be identical.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(0u, 0u);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  float labelWidth  = 300.0f;
  float labelHeight = 300.0f;

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  label.SetProperty(TextLabel::Property::TEXT, "Hello");
  label.SetProperty(Actor::Property::SIZE, Vector2(labelWidth, labelHeight));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 12.0f);
  label.SetProperty(TextLabel::Property::MULTI_LINE, false);

  // Text fit
  Property::Map textFitMapSet;
  textFitMapSet["enable"]       = true;
  textFitMapSet["minSize"]      = 10.f;
  textFitMapSet["maxSize"]      = 30.f;
  textFitMapSet["stepSize"]     = 5.f;
  textFitMapSet["fontSizeType"] = "pointSize";
  label.SetProperty(Toolkit::DevelTextLabel::Property::TEXT_FIT, textFitMapSet);

  application.GetScene().Add(label);

  // Connect to the async text rendered signal.
  ConnectionTracker* testRenderTracker = new ConnectionTracker();
  DevelTextLabel::AsyncTextRenderedSignal(label).Connect(&TestAsyncTextRendered);

  bool asyncTextRendered = false;
  label.ConnectSignal(testRenderTracker, "asyncTextRendered", CallbackFunctor(&asyncTextRendered));
  gAsyncTextRenderedCalled = false;

  // Connect to the text fit changed signal.
  ConnectionTracker* testFitTracker = new ConnectionTracker();
  DevelTextLabel::TextFitChangedSignal(label).Connect(&TestTextFitChanged);

  bool textFitChanged = false;
  label.ConnectSignal(testFitTracker, "textFitChanged", CallbackFunctor(&textFitChanged));
  gTextFitChangedCalled = false;

  gAsyncTextRenderedWidth  = 0.0f;
  gAsyncTextRenderedHeight = 0.0f;

  // TextFit requires a fixed size. Render with natural size.
  DevelTextLabel::RequestAsyncRenderWithConstraint(label, std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  DALI_TEST_CHECK(gTextFitChangedCalled);
  DALI_TEST_CHECK(textFitChanged);

  float textFitFontSize = (label.GetProperty(Dali::Toolkit::DevelTextLabel::Property::TEXT_FIT).Get<Property::Map>())["fontSize"].Get<float>();

  // To measure expected size.
  TextLabel sizeLabel = TextLabel::New();
  DALI_TEST_CHECK(sizeLabel);

  sizeLabel.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::SYNC);
  sizeLabel.SetProperty(TextLabel::Property::TEXT, "Hello");
  sizeLabel.SetProperty(TextLabel::Property::MULTI_LINE, false);
  sizeLabel.SetProperty(TextLabel::Property::POINT_SIZE, textFitFontSize);

  float minWidth  = sizeLabel.GetNaturalSize().width;
  float minHeight = sizeLabel.GetNaturalSize().height;

  gAsyncTextRenderedWidth  = ConvertToEven(gAsyncTextRenderedWidth);
  gAsyncTextRenderedHeight = ConvertToEven(gAsyncTextRenderedHeight);

  bool width  = gAsyncTextRenderedWidth  >= minWidth;
  bool height = gAsyncTextRenderedHeight >= minHeight;

  DALI_TEST_CHECK(width);
  DALI_TEST_CHECK(height);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliToolkitTextLabelAsyncRenderTextFit03(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelAsyncRenderTextFit03");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Set the dpi of AsyncTextLoader and FontClient to be identical.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(0u, 0u);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  float labelWidth  = 300.0f;
  float labelHeight = 300.0f;

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  label.SetProperty(Actor::Property::SIZE, Vector2(labelWidth, labelHeight));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 12.0f);
  label.SetProperty(TextLabel::Property::MULTI_LINE, false);

  std::string longText = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed non risus. Suspendisse lectus tortor, dignissim sit amet, adipiscing nec, ultricies sed, dolor. Cras elementum ultrices diam. Maecenas ligula massa, varius a, semper congue, euismod non, mi. Proin porttitor, orci nec nonummy molestie, enim est eleifend mi, non fermentum diam nisl sit amet erat. Duis semper. Vestibulum volutpat pretium libero. Vivamus at augue. In hac habitasse platea dictumst. Pellentesque eu metus. Etiam vitae tortor. Morbi vestibulum volutpat enim. Fusce vel dui. Sed vulputate odio vel purus. Aliquam at lorem. \U0001F31F";
  label.SetProperty(TextLabel::Property::TEXT, longText);

  // Text fit
  Property::Map textFitMapSet;
  textFitMapSet["enable"]       = true;
  textFitMapSet["minSize"]      = 10.0f;
  textFitMapSet["maxSize"]      = 30.0f;
  textFitMapSet["stepSize"]     = 0.5f;
  textFitMapSet["fontSizeType"] = "pointSize";
  label.SetProperty(Toolkit::DevelTextLabel::Property::TEXT_FIT, textFitMapSet);

  application.GetScene().Add(label);

  // Connect to the async text rendered signal.
  ConnectionTracker* testRenderTracker = new ConnectionTracker();
  DevelTextLabel::AsyncTextRenderedSignal(label).Connect(&TestAsyncTextRendered);

  bool asyncTextRendered = false;
  label.ConnectSignal(testRenderTracker, "asyncTextRendered", CallbackFunctor(&asyncTextRendered));
  gAsyncTextRenderedCalled = false;

  // Connect to the text fit changed signal.
  ConnectionTracker* testFitTracker = new ConnectionTracker();
  DevelTextLabel::TextFitChangedSignal(label).Connect(&TestTextFitChanged);

  bool textFitChanged = false;
  label.ConnectSignal(testFitTracker, "textFitChanged", CallbackFunctor(&textFitChanged));
  gTextFitChangedCalled = false;

  gAsyncTextRenderedWidth  = 0.0f;
  gAsyncTextRenderedHeight = 0.0f;

  float expectedWidth  = 200.0f;
  float expectedHeight = 200.0f;

  // TextFit requires a fixed size. Render with natural size.
  DevelTextLabel::RequestAsyncRenderWithFixedSize(label, expectedWidth, expectedHeight);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  DALI_TEST_CHECK(gTextFitChangedCalled);
  DALI_TEST_CHECK(textFitChanged);

  DALI_TEST_EQUALS(expectedWidth, gAsyncTextRenderedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncTextRenderedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(true, label.GetProperty<bool>(DevelTextLabel::Property::MANUAL_RENDERED), TEST_LOCATION);

  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;
  gAsyncTextRenderedWidth  = 0.0f;
  gAsyncTextRenderedHeight = 0.0f;

  // Text fit array, make sorted options.
  std::vector<DevelTextLabel::FitOption> fitOptions;
  fitOptions.push_back(DevelTextLabel::FitOption(50, 60));
  fitOptions.push_back(DevelTextLabel::FitOption(60, 70));
  fitOptions.push_back(DevelTextLabel::FitOption(70, 80));
  fitOptions.push_back(DevelTextLabel::FitOption(80, 90));
  fitOptions.push_back(DevelTextLabel::FitOption(90, 100));
  DevelTextLabel::SetTextFitArray(label, true, fitOptions);

  // TextFit requires a fixed size. Render with natural size.
  DevelTextLabel::RequestAsyncRenderWithFixedSize(label, expectedWidth, expectedHeight);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  DALI_TEST_EQUALS(expectedWidth, gAsyncTextRenderedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncTextRenderedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(true, label.GetProperty<bool>(DevelTextLabel::Property::MANUAL_RENDERED), TEST_LOCATION);

  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;
  gAsyncTextRenderedWidth  = 0.0f;
  gAsyncTextRenderedHeight = 0.0f;

  // Text fit array, make unsorted options.
  std::vector<DevelTextLabel::FitOption> unsortedFitOptions;
  unsortedFitOptions.push_back(DevelTextLabel::FitOption(50, 80));
  unsortedFitOptions.push_back(DevelTextLabel::FitOption(60, 70));
  unsortedFitOptions.push_back(DevelTextLabel::FitOption(70, 80));
  unsortedFitOptions.push_back(DevelTextLabel::FitOption(80, 90));
  unsortedFitOptions.push_back(DevelTextLabel::FitOption(90, 100));
  DevelTextLabel::SetTextFitArray(label, true, unsortedFitOptions);

  // TextFit requires a fixed size. Render with natural size.
  DevelTextLabel::RequestAsyncRenderWithFixedSize(label, expectedWidth, expectedHeight);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  DALI_TEST_EQUALS(expectedWidth, gAsyncTextRenderedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncTextRenderedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(true, label.GetProperty<bool>(DevelTextLabel::Property::MANUAL_RENDERED), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextLabelAsyncRenderAutoScroll01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelAsyncRenderAutoScroll01");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Set the dpi of AsyncTextLoader and FontClient to be identical.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(0u, 0u);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  float labelWidth  = 300.0f;
  float labelHeight = 300.0f;

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_AUTO);
  label.SetProperty(TextLabel::Property::TEXT, "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed non risus");
  label.SetProperty(Actor::Property::SIZE, Vector2(labelWidth, labelHeight));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 20);
  label.SetProperty(TextLabel::Property::MULTI_LINE, false);

  // Auto scroll
  label.SetProperty(TextLabel::Property::ENABLE_AUTO_SCROLL, true);
  label.SetProperty(TextLabel::Property::AUTO_SCROLL_STOP_MODE, TextLabel::AutoScrollStopMode::IMMEDIATE);

  application.GetScene().Add(label);

  // Connect to the async text rendered signal.
  ConnectionTracker* testRenderTracker = new ConnectionTracker();
  DevelTextLabel::AsyncTextRenderedSignal(label).Connect(&TestAsyncTextRendered);

  bool asyncTextRendered = false;
  label.ConnectSignal(testRenderTracker, "asyncTextRendered", CallbackFunctor(&asyncTextRendered));
  gAsyncTextRenderedCalled = false;

  // Request render automatically.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);
  DALI_TEST_CHECK(label.GetProperty<bool>(TextLabel::Property::ENABLE_AUTO_SCROLL));

  // stop IMMEDIATE.
  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;

  label.SetProperty(TextLabel::Property::ENABLE_AUTO_SCROLL, false);

  // Request render automatically.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);
  DALI_TEST_CHECK(!label.GetProperty<bool>(TextLabel::Property::ENABLE_AUTO_SCROLL));

  // restart.
  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;

  label.SetProperty(TextLabel::Property::ENABLE_AUTO_SCROLL, true);

  // Request render automatically.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);
  DALI_TEST_CHECK(label.GetProperty<bool>(TextLabel::Property::ENABLE_AUTO_SCROLL));

  // stop FINISH_LOOP.
  // Rendering should not be requested at this time, because the scroll does not end immediately,
  label.SetProperty(TextLabel::Property::AUTO_SCROLL_STOP_MODE, TextLabel::AutoScrollStopMode::FINISH_LOOP);
  label.SetProperty(TextLabel::Property::ENABLE_AUTO_SCROLL, false);

  // Request render automatically.
  application.SendNotification();
  application.Render();

  // The finish loop has not ended yet.
  DALI_TEST_CHECK(label.GetProperty<bool>(TextLabel::Property::ENABLE_AUTO_SCROLL));

  // stop IMMEDIATE.
  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;

  label.SetProperty(TextLabel::Property::AUTO_SCROLL_STOP_MODE, TextLabel::AutoScrollStopMode::IMMEDIATE);
  label.SetProperty(TextLabel::Property::ENABLE_AUTO_SCROLL, false);

  // Request render automatically.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);
  DALI_TEST_CHECK(!label.GetProperty<bool>(TextLabel::Property::ENABLE_AUTO_SCROLL));

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliToolkitTextLabelAsyncRenderAutoScroll02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelAsyncRenderAutoScroll02");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Set the dpi of AsyncTextLoader and FontClient to be identical.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(0u, 0u);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  float labelWidth  = 300.0f;
  float labelHeight = 300.0f;

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  label.SetProperty(Actor::Property::SIZE, Vector2(labelWidth, labelHeight));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 100);
  label.SetProperty(TextLabel::Property::MULTI_LINE, false);

  std::string text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed non risus. Suspendisse lectus tortor, dignissim sit amet, adipiscing nec, ultricies sed, dolor. Cras elementum ultrices diam. Maecenas ligula massa, varius a, semper congue, euismod non, mi. Proin porttitor, orci nec nonummy molestie, enim est eleifend mi, non fermentum diam nisl sit amet erat. Duis semper.";
  label.SetProperty(TextLabel::Property::TEXT, text);

  // Auto scroll
  label.SetProperty(TextLabel::Property::ENABLE_AUTO_SCROLL, true);
  label.SetProperty(TextLabel::Property::AUTO_SCROLL_STOP_MODE, TextLabel::AutoScrollStopMode::IMMEDIATE);

  application.GetScene().Add(label);

  // Connect to the async text rendered signal.
  ConnectionTracker* testRenderTracker = new ConnectionTracker();
  DevelTextLabel::AsyncTextRenderedSignal(label).Connect(&TestAsyncTextRendered);

  bool asyncTextRendered = false;
  label.ConnectSignal(testRenderTracker, "asyncTextRendered", CallbackFunctor(&asyncTextRendered));

  gAsyncTextRenderedCalled = false;
  gAsyncTextRenderedWidth  = 0.0f;
  gAsyncTextRenderedHeight = 0.0f;

  // Request render auto scroll.
  DevelTextLabel::RequestAsyncRenderWithConstraint(label, labelWidth, labelHeight);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  float expectedWidth  = labelWidth;
  float expectedHeight = label.GetNaturalSize().height;

  gAsyncTextRenderedHeight = ConvertToEven(gAsyncTextRenderedHeight);

  DALI_TEST_EQUALS(expectedWidth, gAsyncTextRenderedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncTextRenderedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(true, label.GetProperty<bool>(DevelTextLabel::Property::MANUAL_RENDERED), TEST_LOCATION);

  application.SendNotification();
  application.Render();

  // stop IMMEDIATE.
  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;
  gAsyncTextRenderedWidth  = 0.0f;
  gAsyncTextRenderedHeight = 0.0f;

  label.SetProperty(TextLabel::Property::ENABLE_AUTO_SCROLL, false);

  // Request render auto scroll.
  DevelTextLabel::RequestAsyncRenderWithFixedSize(label, labelWidth, labelHeight);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  expectedWidth  = labelWidth;
  expectedHeight = labelHeight;

  DALI_TEST_EQUALS(expectedWidth, gAsyncTextRenderedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncTextRenderedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(true, label.GetProperty<bool>(DevelTextLabel::Property::MANUAL_RENDERED), TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliToolkitTextLabelAsyncRenderCutout(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelAsyncRenderCutout");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Set the dpi of AsyncTextLoader and FontClient to be identical.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(0u, 0u);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  float labelWidth  = 300.0f;
  float labelHeight = 300.0f;

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_AUTO);
  label.SetProperty(TextLabel::Property::TEXT, "Hello, World");
  label.SetProperty(Actor::Property::SIZE, Vector2(labelWidth, labelHeight));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 20);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);

  // Cutout
  label.SetProperty(DevelTextLabel::Property::CUTOUT, true);
  label.SetProperty(TextLabel::Property::TEXT_COLOR, Color::BLUE);
  label.SetProperty(Control::Property::BACKGROUND, Color::RED);

  application.GetScene().Add(label);

  // Connect to the async text rendered signal.
  ConnectionTracker* testRenderTracker = new ConnectionTracker();
  DevelTextLabel::AsyncTextRenderedSignal(label).Connect(&TestAsyncTextRendered);

  bool asyncTextRendered = false;
  label.ConnectSignal(testRenderTracker, "asyncTextRendered", CallbackFunctor(&asyncTextRendered));
  gAsyncTextRenderedCalled = false;

  // Request render automatically.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;

  label.SetProperty(DevelTextLabel::Property::CUTOUT, false);

  // Request render automatically.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  application.SendNotification();
  application.Render();

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::SYNC);
  label.SetProperty(DevelTextLabel::Property::CUTOUT, true);
  label.SetProperty(Control::Property::BACKGROUND, Color::GREEN);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliToolkitTextLabelAsyncRenderHyphenation(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelAsyncRenderCutout");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Set the dpi of AsyncTextLoader and FontClient to be identical.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(0u, 0u);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  float labelWidth  = 150.0f;
  float labelHeight = 300.0f;

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_AUTO);
  label.SetProperty(TextLabel::Property::TEXT, "Hi Experimen");
  label.SetProperty(Actor::Property::SIZE, Vector2(labelWidth, labelHeight));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 35);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);

  // HYPHENATION
  // Hi Exp-
  // erimen
  label.SetProperty(TextLabel::Property::LINE_WRAP_MODE, DevelText::LineWrap::HYPHENATION);

  application.GetScene().Add(label);

  // Connect to the async text rendered signal.
  ConnectionTracker* testRenderTracker = new ConnectionTracker();
  DevelTextLabel::AsyncTextRenderedSignal(label).Connect(&TestAsyncTextRendered);

  bool asyncTextRendered = false;
  label.ConnectSignal(testRenderTracker, "asyncTextRendered", CallbackFunctor(&asyncTextRendered));
  gAsyncTextRenderedCalled = false;

  // Request render automatically.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  int expectedLineCount = label.GetProperty<int>(TextLabel::Property::LINE_COUNT);
  int asyncLineCount    = label.GetProperty<int>(DevelTextLabel::Property::ASYNC_LINE_COUNT);

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);
  DALI_TEST_EQUALS(expectedLineCount, asyncLineCount, TEST_LOCATION);

  // MIXED
  // Hi
  // Experi-
  // men
  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;

  label.SetProperty(TextLabel::Property::LINE_WRAP_MODE, DevelText::LineWrap::MIXED);

  // Request render automatically.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  expectedLineCount = label.GetProperty<int>(TextLabel::Property::LINE_COUNT);
  asyncLineCount    = label.GetProperty<int>(DevelTextLabel::Property::ASYNC_LINE_COUNT);

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);
  DALI_TEST_EQUALS(expectedLineCount, asyncLineCount, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliToolkitTextLabelAsyncRenderMarkup01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelAsyncRenderMarkup01");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Set the dpi of AsyncTextLoader and FontClient to be identical.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(0u, 0u);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  float labelWidth  = 300.0f;
  float labelHeight = 300.0f;

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_AUTO);
  label.SetProperty(TextLabel::Property::ENABLE_MARKUP, true);
  label.SetProperty(Actor::Property::SIZE, Vector2(labelWidth, labelHeight));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 20);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);
  application.GetScene().Add(label);

  // Connect to the async text rendered signal.
  ConnectionTracker* testRenderTracker = new ConnectionTracker();
  DevelTextLabel::AsyncTextRenderedSignal(label).Connect(&TestAsyncTextRendered);

  bool asyncTextRendered = false;
  label.ConnectSignal(testRenderTracker, "asyncTextRendered", CallbackFunctor(&asyncTextRendered));
  gAsyncTextRenderedCalled = false;

  label.SetProperty(TextLabel::Property::TEXT, "<color value='white'>Markup</color><color value='cyan'>Text</color>");

  // Request render automatically.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  // Emoji
  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;

  const std::string emojis = "<font family='BreezeColorEmoji' size='20'>\xF0\x9F\x98\x81 \xF0\x9F\x98\x82 \xF0\x9F\x98\x83 \xF0\x9F\x98\x84</font>";
  label.SetProperty(TextLabel::Property::TEXT, emojis);

  // Request render automatically.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  // EMOJI Sequences case for coverage.
  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;

  std::string emojiSequences =
    "Glyphs not included in the font &#xf01a;&#xf01b;&#xf01c;&#xf01d;&#xf01e;&#xf01f;\n"   //case for coverage when glyph is not included in the font
    "Text VS15 &#x262a;&#xfe0e;\n"                                                         //text presentation sequence and selector
    "Color VS16 &#x262a;&#xfe0f;\n"                                                        //emoji presentation sequence and selector
    "Default &#x262a; \n"                                                                  //default presentation
    "FamilyManWomanGirlBoy &#x1F468;&#x200D;&#x1F469;&#x200D;&#x1F467;&#x200D;&#x1F466;\n" // emoji multi zwj sequence
    "WomanScientist &#x1f469;&#x200d;&#x1f52c;\n"                                          // emoji zwj sequence
    "WomanScientistLightSkinTone&#x1F469;&#x1F3FB;&#x200D;&#x1F52C; \n"                    // emoji modifier sequence: skin tone & JWZ
    "LeftRightArrowText&#x2194;&#xfe0e;\n"                                                 // text presentation sequence and selector
    "LeftRightArrowEmoji&#x2194;&#xfe0f;\n"                                                // emoji presentation sequence and selector
    "SouthKoreaFlag&#x1f1f0;&#x1f1f7;\n"                                                   // emoji flag sequence
    "JordanFlag&#x1f1ef;&#x1f1f4;\n"                                                       // emoji flag sequence
    "EnglandFlag&#x1F3F4;&#xE0067;&#xE0062;&#xE0065;&#xE006E;&#xE0067;&#xE007F;\n"         // emoji tag sequence like England flag
    "Runner &#x1f3c3;&#x200d;&#x27a1;&#xfe0f; \n"
    "VictoryHandMediumLightSkinTone:&#x270C;&#xFE0F;&#x1F3FC;\n"                                                                // emoji modifier sequence: skin tone
    "RainbowFlag:&#x1F3F3;&#xFE0F;&#x200D;&#x1F308; \n"                                                                         // emoji zwj sequence: Rainbow Flag
    "keycap# &#x0023;&#xFE0F;&#x20E3; \n"                                                                                       // fully-qualified  emoji keycap sequence
    "keycap#_text &#x0023;&#x20E3; \n"                                                                                          // unqualified emoji keycap sequence
    "keycap3 &#x0033;&#xfe0f;&#x20e3; \n"                                                                                       // fully-qualified  emoji keycap sequence
    "keycap3_text &#x0033;&#x20e3; \n"                                                                                          // unqualified emoji keycap sequence
    "two adjacent glyphs &#x262a;&#xfe0f;&#xfe0f;&#xfe0f;&#x262a;&#xfe0f;\n"                                                    // This line should be rendered as two adjacent glyphs
    "Digit 8&#xfe0f; 8&#xfe0e; 8\n"                                                                                             // should be rendered according to selector
    "Surfing Medium Skin Female:  &#x1f3c4;&#x1f3fc;&#x200d;&#x2640;&#xfe0f;\n"                                                 // Person Surfing + Medium Skin Tone +? Zero Width Joiner + Female Sign
    "SYMBOLS_NSLCL variation selector: &#x1f170;&#xfe0f;&#x1f171;&#xfe0f;&#x1f172;&#xfe0e;&#x1f173;&#xfe0e;&#x1f174;&#xfe0e;\n" // 1F170 ~ 1F174 with variation selector, text vs emoji
    "SYMBOLS_NSLCL with VS15: &#x1f170;&#xfe0e;&#x1f171;&#xfe0e;&#x1f172;&#xfe0e;&#x1f173;&#xfe0e;&#x1f174;&#xfe0e;\n"          // 1F170 ~ 1F174 with VS15
    "SYMBOLS_NSLCL with VS16: &#x1f170;&#xfe0f;&#x1f171;&#xfe0f;&#x1f172;&#xfe0f;&#x1f173;&#xfe0f;&#x1f174;&#xfe0f;\n"          // 1F170 ~ 1F174 with VS16
    ;

  label.SetProperty(TextLabel::Property::TEXT, emojiSequences);
  label.SetProperty(TextLabel::Property::ENABLE_MARKUP, true);
  label.SetProperty(TextLabel::Property::ELLIPSIS, false);

  // Request render automatically.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliToolkitTextLabelAsyncRenderMarkup02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelAsyncRenderMarkup02");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Set the dpi of AsyncTextLoader and FontClient to be identical.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(0u, 0u);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  float labelWidth  = 300.0f;
  float labelHeight = 300.0f;

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_AUTO);
  label.SetProperty(TextLabel::Property::ENABLE_MARKUP, true);
  label.SetProperty(Actor::Property::SIZE, Vector2(labelWidth, labelHeight));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 20);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);
  application.GetScene().Add(label);

  // Connect to the async text rendered signal.
  ConnectionTracker* testRenderTracker = new ConnectionTracker();
  DevelTextLabel::AsyncTextRenderedSignal(label).Connect(&TestAsyncTextRendered);

  bool asyncTextRendered = false;
  label.ConnectSignal(testRenderTracker, "asyncTextRendered", CallbackFunctor(&asyncTextRendered));
  gAsyncTextRenderedCalled = false;

  // underline
  std::string underlineText = "start<u height='5.0f' color='green' >underline<u color='blue'>markup text</u>CDE</u>end";
  label.SetProperty(TextLabel::Property::TEXT, underlineText);

  // Request render automatically.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  // strikethrough
  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;

  std::string strikethroughText = "start<s height='5.0f' color='green' >strikethrough<s color='blue' >markup text</s>CDE</s>end";
  label.SetProperty(TextLabel::Property::TEXT, strikethroughText);

  // Request render automatically.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  // character spacing
  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;

  std::string charspacingText = "start\n<char-spacing value='5.0f'>CHAR\n</char-spacing><char-spacing value='10.0f'>SPACING\n</char-spacing>end";
  label.SetProperty(TextLabel::Property::TEXT, charspacingText);

  // Request render automatically.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  application.SendNotification();
  application.Render();

  // emoji
  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;

  std::string emojiText = "Color VS16 \u262a\ufe0f";
  label.SetProperty(TextLabel::Property::TEXT, emojiText);

  // Request render automatically.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliToolkitTextLabelAsyncRenderTiling01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelAsyncRenderTiling01");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Set the dpi of AsyncTextLoader and FontClient to be identical.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(0u, 0u);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  label.SetProperty(Actor::Property::SIZE, Vector2(300.0f, 300.0f));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 100);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);

  std::string longText = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed non risus. Suspendisse lectus tortor, dignissim sit amet, adipiscing nec, ultricies sed, dolor. Cras elementum ultrices diam. Maecenas ligula massa, varius a, semper congue, euismod non, mi. Proin porttitor, orci nec nonummy molestie, enim est eleifend mi, non fermentum diam nisl sit amet erat. Duis semper. Vestibulum volutpat pretium libero. Vivamus at augue. In hac habitasse platea dictumst. Pellentesque eu metus. Etiam vitae tortor. Morbi vestibulum volutpat enim. Fusce vel dui. Sed vulputate odio vel purus. Aliquam at lorem. \U0001F31F";
  label.SetProperty(TextLabel::Property::TEXT, longText);  application.GetScene().Add(label);

  // Connect to the async text rendered signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextLabel::AsyncTextRenderedSignal(label).Connect(&TestAsyncTextRendered);

  bool asyncTextRendered = false;
  label.ConnectSignal(testTracker, "asyncTextRendered", CallbackFunctor(&asyncTextRendered));

  gAsyncTextRenderedCalled = false;
  gAsyncTextRenderedWidth  = 0.0f;
  gAsyncTextRenderedHeight = 0.0f;

  float expectedWidth  = 100.0f;

  // Request render.
  DevelTextLabel::RequestAsyncRenderWithFixedWidth(label, expectedWidth, std::numeric_limits<float>::infinity());

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT * 2), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  float expectedHeight = label.GetHeightForWidth(expectedWidth);

  DALI_TEST_EQUALS(expectedWidth, gAsyncTextRenderedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncTextRenderedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(true, label.GetProperty<bool>(DevelTextLabel::Property::MANUAL_RENDERED), TEST_LOCATION);

  application.SendNotification();
  application.Render();

  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;
  gAsyncTextRenderedWidth  = 0.0f;
  gAsyncTextRenderedHeight = 0.0f;

  Property::Map underline;
  underline.Clear();
  underline.Insert("enable", true);
  underline.Insert("color", Color::RED);
  underline.Insert("height", 1);
  label.SetProperty(TextLabel::Property::UNDERLINE, underline);

  Property::Map strikethrough;
  strikethrough.Clear();
  strikethrough.Insert("enable", true);
  strikethrough.Insert("color", Color::BLUE);
  strikethrough.Insert("height", 2.0f);
  label.SetProperty(DevelTextLabel::Property::STRIKETHROUGH, strikethrough);

  Property::Map outline;
  outline["color"] = Color::GREEN;
  outline["width"] = 2.0f;
  outline["offset"] = Vector2(2.0f, 2.0f);
  outline["blurRadius"] = 3.0f;
  label.SetProperty(TextLabel::Property::OUTLINE, outline);

  Property::Map shadow;
  shadow.Insert("color", Color::BLACK);
  shadow.Insert("offset", Vector2(1.0f, 1.0f));
  label.SetProperty(TextLabel::Property::SHADOW, shadow);

  expectedWidth  = 100.0f;

  // Request render.
  DevelTextLabel::RequestAsyncRenderWithFixedWidth(label, expectedWidth, std::numeric_limits<float>::infinity());

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT * 2), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  expectedHeight = label.GetHeightForWidth(expectedWidth);

  DALI_TEST_EQUALS(expectedWidth, gAsyncTextRenderedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncTextRenderedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(true, label.GetProperty<bool>(DevelTextLabel::Property::MANUAL_RENDERED), TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliToolkitTextLabelRequestAsyncComputation01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelRequestAsyncComputation01");

  // Cancel async render task.

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Set the dpi of AsyncTextLoader and FontClient to be identical.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(0u, 0u);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  label.SetProperty(Actor::Property::SIZE, Vector2(300.0f, 300.0f));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 20);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);
  application.GetScene().Add(label);

  // Connect to the async text rendered signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextLabel::AsyncTextRenderedSignal(label).Connect(&TestAsyncTextRendered);

  // dummy text for test.
  TextLabel dummy1 = TextLabel::New();
  DALI_TEST_CHECK(dummy1);
  dummy1.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  dummy1.SetProperty(Actor::Property::SIZE, Vector2(300.0f, 300.0f));
  dummy1.SetProperty(TextLabel::Property::POINT_SIZE, 20);
  dummy1.SetProperty(TextLabel::Property::MULTI_LINE, true);

  TextLabel dummy2 = TextLabel::New();
  DALI_TEST_CHECK(dummy2);
  dummy2.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  dummy2.SetProperty(Actor::Property::SIZE, Vector2(300.0f, 300.0f));
  dummy2.SetProperty(TextLabel::Property::POINT_SIZE, 20);
  dummy2.SetProperty(TextLabel::Property::MULTI_LINE, true);

  bool asyncTextRendered = false;
  label.ConnectSignal(testTracker, "asyncTextRendered", CallbackFunctor(&asyncTextRendered));

  gAsyncTextRenderedCalled = false;
  gAsyncTextRenderedWidth  = 0.0f;
  gAsyncTextRenderedHeight = 0.0f;

  float expectedWidth  = 200.0f;
  float expectedHeight = 200.0f;
  float dummySize      = 100.0f;

  std::string text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.";
  dummy1.SetProperty(TextLabel::Property::TEXT, text);
  dummy2.SetProperty(TextLabel::Property::TEXT, text);
  label.SetProperty(TextLabel::Property::TEXT, text);

  // Request size computation, due to dummy's requests, text manager's loader queue is full.
  DevelTextLabel::RequestAsyncNaturalSize(dummy1);
  DevelTextLabel::RequestAsyncHeightForWidth(dummy1, dummySize);
  DevelTextLabel::RequestAsyncNaturalSize(dummy2);
  DevelTextLabel::RequestAsyncHeightForWidth(dummy2, dummySize);

  // Request render .. [Task 1]
  DevelTextLabel::RequestAsyncRenderWithFixedWidth(label, expectedWidth, std::numeric_limits<float>::infinity());

  // Request render .. [Task 2]
  // The unfinished [Task 1] will be canceled.
  DevelTextLabel::RequestAsyncRenderWithFixedSize(label, expectedWidth, expectedHeight);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(6, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  DALI_TEST_EQUALS(expectedWidth, gAsyncTextRenderedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncTextRenderedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(true, label.GetProperty<bool>(DevelTextLabel::Property::MANUAL_RENDERED), TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliToolkitTextLabelRequestAsyncComputation02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelRequestAsyncComputation02");

  // Cancel async natural size computation task.

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Set the dpi of AsyncTextLoader and FontClient to be identical.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(0u, 0u);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  label.SetProperty(Actor::Property::SIZE, Vector2(300.0f, 300.0f));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 20);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);
  application.GetScene().Add(label);

  // Connect to the async natural size computed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextLabel::AsyncNaturalSizeComputedSignal(label).Connect(&TestAsyncSizeComputed);

  // dummy text for test.
  TextLabel dummy1 = TextLabel::New();
  DALI_TEST_CHECK(dummy1);
  dummy1.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  dummy1.SetProperty(Actor::Property::SIZE, Vector2(300.0f, 300.0f));
  dummy1.SetProperty(TextLabel::Property::POINT_SIZE, 20);
  dummy1.SetProperty(TextLabel::Property::MULTI_LINE, true);

  TextLabel dummy2 = TextLabel::New();
  DALI_TEST_CHECK(dummy2);
  dummy2.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  dummy2.SetProperty(Actor::Property::SIZE, Vector2(300.0f, 300.0f));
  dummy2.SetProperty(TextLabel::Property::POINT_SIZE, 20);
  dummy2.SetProperty(TextLabel::Property::MULTI_LINE, true);

  bool asyncSizeComputed = false;
  label.ConnectSignal(testTracker, "asyncNaturalSizeComputed", CallbackFunctor(&asyncSizeComputed));

  gAsyncSizeComputedCalled = false;
  gAsyncSizeComputedWidth  = 0.0f;
  gAsyncSizeComputedHeight = 0.0f;

  float dummySize = 100.0f;

  std::string text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.";
  dummy1.SetProperty(TextLabel::Property::TEXT, text);
  dummy2.SetProperty(TextLabel::Property::TEXT, text);
  label.SetProperty(TextLabel::Property::TEXT, text);

  // Request size computation, due to dummy's requests, text manager's loader queue is full.
  DevelTextLabel::RequestAsyncNaturalSize(dummy1);
  DevelTextLabel::RequestAsyncHeightForWidth(dummy1, dummySize);
  DevelTextLabel::RequestAsyncNaturalSize(dummy2);
  DevelTextLabel::RequestAsyncHeightForWidth(dummy2, dummySize);

  // Request compute .. [Task 1]
  DevelTextLabel::RequestAsyncNaturalSize(label);

  // Request compute .. [Task 2]
  // The unfinished [Task 1] will be canceled.
  DevelTextLabel::RequestAsyncNaturalSize(label);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(6, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncSizeComputedCalled);
  DALI_TEST_CHECK(asyncSizeComputed);

  float expectedWidth  = label.GetNaturalSize().width;
  float expectedHeight = label.GetNaturalSize().height;

  gAsyncSizeComputedWidth  = ConvertToEven(gAsyncSizeComputedWidth);
  gAsyncSizeComputedHeight = ConvertToEven(gAsyncSizeComputedHeight);

  DALI_TEST_EQUALS(expectedWidth, gAsyncSizeComputedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncSizeComputedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliToolkitTextLabelRequestAsyncComputation03(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelRequestAsyncComputation03");

  // Cancel async height for width computation task.

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Set the dpi of AsyncTextLoader and FontClient to be identical.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(0u, 0u);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  label.SetProperty(Actor::Property::SIZE, Vector2(300.0f, 300.0f));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 20);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);
  application.GetScene().Add(label);

  // Connect to the async height for width computed signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextLabel::AsyncHeightForWidthComputedSignal(label).Connect(&TestAsyncSizeComputed);

  // dummy text for test.
  TextLabel dummy1 = TextLabel::New();
  DALI_TEST_CHECK(dummy1);
  dummy1.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  dummy1.SetProperty(Actor::Property::SIZE, Vector2(300.0f, 300.0f));
  dummy1.SetProperty(TextLabel::Property::POINT_SIZE, 20);
  dummy1.SetProperty(TextLabel::Property::MULTI_LINE, true);

  TextLabel dummy2 = TextLabel::New();
  DALI_TEST_CHECK(dummy2);
  dummy2.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  dummy2.SetProperty(Actor::Property::SIZE, Vector2(300.0f, 300.0f));
  dummy2.SetProperty(TextLabel::Property::POINT_SIZE, 20);
  dummy2.SetProperty(TextLabel::Property::MULTI_LINE, true);

  bool asyncSizeComputed = false;
  label.ConnectSignal(testTracker, "asyncHeightForWidthComputed", CallbackFunctor(&asyncSizeComputed));

  gAsyncSizeComputedCalled = false;
  gAsyncSizeComputedWidth  = 0.0f;
  gAsyncSizeComputedHeight = 0.0f;

  float dummySize = 100.0f;

  std::string text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.";
  dummy1.SetProperty(TextLabel::Property::TEXT, text);
  dummy2.SetProperty(TextLabel::Property::TEXT, text);
  label.SetProperty(TextLabel::Property::TEXT, text);

  // Request size computation, due to dummy's requests, text manager's loader queue is full.
  DevelTextLabel::RequestAsyncNaturalSize(dummy1);
  DevelTextLabel::RequestAsyncHeightForWidth(dummy1, dummySize);
  DevelTextLabel::RequestAsyncNaturalSize(dummy2);
  DevelTextLabel::RequestAsyncHeightForWidth(dummy2, dummySize);

  // Request compute .. [Task 1]
  DevelTextLabel::RequestAsyncHeightForWidth(label, 100.0f);

  // Request compute .. [Task 2]
  // The unfinished [Task 1] will be canceled.
  DevelTextLabel::RequestAsyncHeightForWidth(label, 300.0f);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(6, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncSizeComputedCalled);
  DALI_TEST_CHECK(asyncSizeComputed);

  float expectedWidth  = 300.0f;
  float expectedHeight = label.GetHeightForWidth(expectedWidth);

  DALI_TEST_EQUALS(expectedWidth, gAsyncSizeComputedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncSizeComputedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliToolkitTextLabelAsyncSceneDisconnection(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelAsyncSceneDisconnection");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  label.SetProperty(TextLabel::Property::TEXT, "Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
  label.SetProperty(Actor::Property::SIZE, Vector2(300.0f, 300.0f));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 12);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);
  application.GetScene().Add(label);

  // Connect to the async text rendered signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextLabel::AsyncTextRenderedSignal(label).Connect(&TestAsyncTextRendered);

  bool asyncTextRendered = false;
  label.ConnectSignal(testTracker, "asyncTextRendered", CallbackFunctor(&asyncTextRendered));

  gAsyncTextRenderedCalled = false;
  gAsyncTextRenderedWidth  = 0.0f;
  gAsyncTextRenderedHeight = 0.0f;

  float expectedWidth  = 300.0f;
  float expectedHeight = 300.0f;

  // Request render.
  DevelTextLabel::RequestAsyncRenderWithFixedSize(label, expectedWidth, expectedHeight);
  application.GetScene().Remove(label);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(!gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(!asyncTextRendered);

  END_TEST;
}

int UtcDaliToolkitTextLabelAsyncUnparentAndReset01(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelAsyncUnparentAndReset01");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  label.SetProperty(TextLabel::Property::TEXT, "Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
  label.SetProperty(Actor::Property::SIZE, Vector2(300.0f, 300.0f));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 12);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);
  application.GetScene().Add(label);

  // Connect to the async text rendered signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextLabel::AsyncTextRenderedSignal(label).Connect(&TestAsyncTextRendered);

  bool asyncTextRendered = false;
  label.ConnectSignal(testTracker, "asyncTextRendered", CallbackFunctor(&asyncTextRendered));

  gAsyncTextRenderedCalled = false;
  gAsyncTextRenderedWidth  = 0.0f;
  gAsyncTextRenderedHeight = 0.0f;

  float expectedWidth  = 300.0f;
  float expectedHeight = 300.0f;

  // Request render.
  DevelTextLabel::RequestAsyncRenderWithFixedSize(label, expectedWidth, expectedHeight);
  label.Unparent();
  label.Reset();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(!gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(!asyncTextRendered);

  END_TEST;
}

int UtcDaliToolkitTextLabelAsyncUnparentAndReset02(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelAsyncUnparentAndReset02");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  label.SetProperty(TextLabel::Property::TEXT, "Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
  label.SetProperty(Actor::Property::SIZE, Vector2(300.0f, 300.0f));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 20);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);
  application.GetScene().Add(label);

  // dummy text for test.
  TextLabel dummy1 = TextLabel::New();
  DALI_TEST_CHECK(dummy1);
  dummy1.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  dummy1.SetProperty(TextLabel::Property::TEXT, "Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
  dummy1.SetProperty(Actor::Property::SIZE, Vector2(300.0f, 300.0f));
  dummy1.SetProperty(TextLabel::Property::POINT_SIZE, 20);
  dummy1.SetProperty(TextLabel::Property::MULTI_LINE, true);

  TextLabel dummy2 = TextLabel::New();
  DALI_TEST_CHECK(dummy2);
  dummy2.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  dummy2.SetProperty(TextLabel::Property::TEXT, "Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
  dummy2.SetProperty(Actor::Property::SIZE, Vector2(300.0f, 300.0f));
  dummy2.SetProperty(TextLabel::Property::POINT_SIZE, 20);
  dummy2.SetProperty(TextLabel::Property::MULTI_LINE, true);

  TextLabel dummy3 = TextLabel::New();
  DALI_TEST_CHECK(dummy3);
  dummy3.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  dummy3.SetProperty(TextLabel::Property::TEXT, "Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
  dummy3.SetProperty(Actor::Property::SIZE, Vector2(300.0f, 300.0f));
  dummy3.SetProperty(TextLabel::Property::POINT_SIZE, 20);
  dummy3.SetProperty(TextLabel::Property::MULTI_LINE, true);

  TextLabel dummy4 = TextLabel::New();
  DALI_TEST_CHECK(dummy4);
  dummy4.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  dummy4.SetProperty(TextLabel::Property::TEXT, "Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
  dummy4.SetProperty(Actor::Property::SIZE, Vector2(300.0f, 300.0f));
  dummy4.SetProperty(TextLabel::Property::POINT_SIZE, 20);
  dummy4.SetProperty(TextLabel::Property::MULTI_LINE, true);

  // Connect to the async text rendered signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextLabel::AsyncTextRenderedSignal(label).Connect(&TestAsyncTextRendered);

  bool asyncTextRendered = false;
  label.ConnectSignal(testTracker, "asyncTextRendered", CallbackFunctor(&asyncTextRendered));
  gAsyncTextRenderedCalled = false;

  // Connect to the async natural size computed signal.
  ConnectionTracker* testNaturalSizeTracker = new ConnectionTracker();
  DevelTextLabel::AsyncNaturalSizeComputedSignal(label).Connect(&TestAsyncSizeComputed);

  bool asyncNaturalSizeComputed = false;
  label.ConnectSignal(testNaturalSizeTracker, "asyncNaturalSizeComputed", CallbackFunctor(&asyncNaturalSizeComputed));

  // Connect to the async height for width computed signal.
  ConnectionTracker* testHeightForWidthTracker = new ConnectionTracker();
  DevelTextLabel::AsyncHeightForWidthComputedSignal(label).Connect(&TestAsyncSizeComputed);

  bool asyncHeightForWidthComputed = false;
  label.ConnectSignal(testHeightForWidthTracker, "asyncHeightForWidthComputed", CallbackFunctor(&asyncHeightForWidthComputed));

  gAsyncSizeComputedCalled = false;

  float expectedWidth  = 300.0f;
  float expectedHeight = 300.0f;
  float dummySize      = 100.0f;

  // Request size computation, due to dummy's requests, text manager's loader queue is full.
  DevelTextLabel::RequestAsyncNaturalSize(dummy1);
  DevelTextLabel::RequestAsyncHeightForWidth(dummy1, dummySize);
  DevelTextLabel::RequestAsyncNaturalSize(dummy2);
  DevelTextLabel::RequestAsyncHeightForWidth(dummy2, dummySize);
  DevelTextLabel::RequestAsyncNaturalSize(dummy3);
  DevelTextLabel::RequestAsyncHeightForWidth(dummy3, dummySize);
  DevelTextLabel::RequestAsyncNaturalSize(dummy4);
  DevelTextLabel::RequestAsyncHeightForWidth(dummy4, dummySize);

  // Request render and size computation.
  DevelTextLabel::RequestAsyncRenderWithFixedSize(label, expectedWidth, expectedHeight);
  DevelTextLabel::RequestAsyncNaturalSize(label);
  DevelTextLabel::RequestAsyncHeightForWidth(label, expectedWidth);

  dummy3.Unparent();
  dummy3.Reset();

  application.SendNotification();
  application.Render();

  dummy4.Unparent();
  dummy4.Reset();

  label.Unparent();
  label.Reset();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(9, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(!gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(!asyncTextRendered);

  DALI_TEST_CHECK(!gAsyncSizeComputedCalled);
  DALI_TEST_CHECK(!asyncNaturalSizeComputed);
  DALI_TEST_CHECK(!asyncHeightForWidthComputed);

  END_TEST;
}

int UtcDaliToolkitTextLabelAsyncSetText(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelAsyncSetText");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  label.SetProperty(Actor::Property::SIZE, Vector2(300.0f, 300.0f));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 12);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);

  application.GetScene().Add(label);

  // Connect to the async text rendered signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextLabel::AsyncTextRenderedSignal(label).Connect(&TestAsyncTextRendered);

  bool asyncTextRendered = false;
  label.ConnectSignal(testTracker, "asyncTextRendered", CallbackFunctor(&asyncTextRendered));

  gAsyncTextRenderedCalled = false;
  gAsyncTextRenderedWidth  = 0.0f;
  gAsyncTextRenderedHeight = 0.0f;

  float expectedWidth  = 300.0f;
  float expectedHeight = 300.0f;

  // Request render.
  std::string text = "Hello, world!";
  label.SetProperty(TextLabel::Property::TEXT, text);
  DevelTextLabel::RequestAsyncRenderWithFixedSize(label, expectedWidth, expectedHeight);

  // Request render.
  std::string emptyText = "";
  label.SetProperty(TextLabel::Property::TEXT, emptyText);
  DevelTextLabel::RequestAsyncRenderWithFixedSize(label, expectedWidth, expectedHeight);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  DALI_TEST_EQUALS(expectedWidth, gAsyncTextRenderedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncTextRenderedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(true, label.GetProperty<bool>(DevelTextLabel::Property::MANUAL_RENDERED), TEST_LOCATION);

  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;
  gAsyncTextRenderedWidth  = 0.0f;
  gAsyncTextRenderedHeight = 0.0f;

  expectedWidth  = 300.0f;
  expectedHeight = 0.0f;

  // Request render.
  DevelTextLabel::RequestAsyncRenderWithFixedWidth(label, expectedWidth, std::numeric_limits<float>::infinity());

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  DALI_TEST_EQUALS(expectedWidth, gAsyncTextRenderedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncTextRenderedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(true, label.GetProperty<bool>(DevelTextLabel::Property::MANUAL_RENDERED), TEST_LOCATION);

  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;
  gAsyncTextRenderedWidth  = 0.0f;
  gAsyncTextRenderedHeight = 0.0f;

  expectedWidth  = 0.0f;
  expectedHeight = 0.0f;

  // Request render.
  DevelTextLabel::RequestAsyncRenderWithConstraint(label, std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  DALI_TEST_EQUALS(expectedWidth, gAsyncTextRenderedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncTextRenderedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(true, label.GetProperty<bool>(DevelTextLabel::Property::MANUAL_RENDERED), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitTextLabelAsyncTextMultiline(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitTextLabelAsyncTextMultiline");

  // Avoid a crash when core load gl resources.
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  // Set the dpi of AsyncTextLoader and FontClient to be identical.
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  fontClient.SetDpi(0u, 0u);

  TextLabel label = TextLabel::New();
  DALI_TEST_CHECK(label);

  label.SetProperty(DevelTextLabel::Property::RENDER_MODE, DevelTextLabel::Render::ASYNC_MANUAL);
  label.SetProperty(Actor::Property::SIZE, Vector2(300.0f, 300.0f));
  label.SetProperty(TextLabel::Property::POINT_SIZE, 20);
  label.SetProperty(TextLabel::Property::MULTI_LINE, true);
  label.SetProperty(TextLabel::Property::ELLIPSIS, true);

  std::string longText = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed non risus. Suspendisse lectus tortor, dignissim sit amet, adipiscing nec, ultricies sed, dolor. Cras elementum ultrices diam. Maecenas ligula massa, varius a, semper congue, euismod non, mi. Proin porttitor, orci nec nonummy molestie, enim est eleifend mi, non fermentum diam nisl sit amet erat. Duis semper. Vestibulum volutpat pretium libero. Vivamus at augue. In hac habitasse platea dictumst. Pellentesque eu metus. Etiam vitae tortor. Morbi vestibulum volutpat enim. Fusce vel dui. Sed vulputate odio vel purus. Aliquam at lorem. \U0001F31F";
  label.SetProperty(TextLabel::Property::TEXT, longText);

  application.GetScene().Add(label);

  // Connect to the async text rendered signal.
  ConnectionTracker* testTracker = new ConnectionTracker();
  DevelTextLabel::AsyncTextRenderedSignal(label).Connect(&TestAsyncTextRendered);

  bool asyncTextRendered = false;
  label.ConnectSignal(testTracker, "asyncTextRendered", CallbackFunctor(&asyncTextRendered));

  gAsyncTextRenderedCalled = false;
  gAsyncTextRenderedWidth  = 0.0f;
  gAsyncTextRenderedHeight = 0.0f;

  float expectedWidth  = 300.0f;
  float expectedHeight = 300.0f;

  // Request render.
  DevelTextLabel::RequestAsyncRenderWithFixedSize(label, expectedWidth, expectedHeight);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  DALI_TEST_EQUALS(expectedWidth, gAsyncTextRenderedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncTextRenderedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(true, label.GetProperty<bool>(DevelTextLabel::Property::MANUAL_RENDERED), TEST_LOCATION);

  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;
  gAsyncTextRenderedWidth  = 0.0f;
  gAsyncTextRenderedHeight = 0.0f;

  expectedWidth  = 300.0f;
  expectedHeight = 300.0f;

  // Request render.
  DevelTextLabel::RequestAsyncRenderWithFixedWidth(label, expectedWidth, expectedHeight);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  DALI_TEST_EQUALS(expectedWidth, gAsyncTextRenderedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncTextRenderedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(true, label.GetProperty<bool>(DevelTextLabel::Property::MANUAL_RENDERED), TEST_LOCATION);

  asyncTextRendered        = false;
  gAsyncTextRenderedCalled = false;
  gAsyncTextRenderedWidth  = 0.0f;
  gAsyncTextRenderedHeight = 0.0f;

  expectedWidth  = 300.0f;
  expectedHeight = 300.0f;

  // Request render.
  DevelTextLabel::RequestAsyncRenderWithConstraint(label, expectedWidth, expectedHeight);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gAsyncTextRenderedCalled);
  DALI_TEST_CHECK(asyncTextRendered);

  DALI_TEST_EQUALS(expectedWidth, gAsyncTextRenderedWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedHeight, gAsyncTextRenderedHeight, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(true, label.GetProperty<bool>(DevelTextLabel::Property::MANUAL_RENDERED), TEST_LOCATION);

  END_TEST;
}
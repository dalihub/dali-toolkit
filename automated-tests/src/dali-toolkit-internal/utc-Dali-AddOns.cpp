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
#include <iostream>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <test-addon-manager.h>
#include <toolkit-event-thread-callback.h>

using namespace Dali::Toolkit::Internal;

namespace
{
const char* TEST_IMAGE_FILE_NAME    = TEST_RESOURCE_DIR "/application-icon-20.png";
const char* TEST_IMAGE_FILE_NAME_9  = TEST_RESOURCE_DIR "/heartsframe.9.png";
const char* TEST_IMAGE_FILE_NAME2_9 = TEST_RESOURCE_DIR "/button-up.9.png";
int         CountFunctionCalls(const std::vector<std::string>& callstack, const std::string& function)
{
  int counter = 0;
  std::find_if(callstack.begin(), callstack.end(), [&counter, &function](const std::string& item) {
    if(item == function)
    {
      counter++;
    }
    return false;
  });

  return counter;
}

} // namespace

int UtcRenderingAddOnTestP(void)
{
  Dali::Integration::AddOnManager* addOnManager = new Test::AddOnManager();

  bool valid = addOnManager->Get() != nullptr;
  DALI_TEST_EQUALS(valid, true, TEST_LOCATION);
  auto addon        = addOnManager->GetAddOn("oo-rendering");
  auto GetCallStack = addOnManager->GetGlobalProc<std::vector<std::string>(bool)>(addon, "GetCallStack");

  ToolkitTestApplication application;
  tet_infoline("UtcRenderingAddOnTestP");

  // Load regular image view
  auto imageView = Dali::Toolkit::ImageView::New(TEST_IMAGE_FILE_NAME);
  imageView.SetProperty(Actor::Property::SIZE, Vector2(400.f, 60.f));
  imageView.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  imageView.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Load npatch image view
  auto imageView2 = Dali::Toolkit::ImageView::New(TEST_IMAGE_FILE_NAME_9);
  imageView2.SetProperty(Actor::Property::SIZE, Vector2(400.f, 60.f));
  imageView2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  imageView2.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  // Load npatch image view
  auto imageView3 = Dali::Toolkit::ImageView::New(TEST_IMAGE_FILE_NAME2_9);
  imageView3.SetProperty(Actor::Property::SIZE, Vector2(400.f, 60.f));
  imageView3.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  imageView3.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  application.GetScene().Add(imageView);
  application.GetScene().Add(imageView2);
  application.GetScene().Add(imageView3);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(::Test::WaitForEventThreadTrigger(3), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  auto callstack = GetCallStack(true);

  DALI_TEST_EQUALS(CountFunctionCalls(callstack, "GetGeometry"), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(CountFunctionCalls(callstack, "CreateGeometry"), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(CountFunctionCalls(callstack, "CreateGeometryGrid"), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(CountFunctionCalls(callstack, "BuildNPatch"), 2, TEST_LOCATION);

  delete addOnManager;

  END_TEST;
}

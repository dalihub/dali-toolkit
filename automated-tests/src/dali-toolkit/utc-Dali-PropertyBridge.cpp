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
#include <iostream>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/property-bridge/property-bridge.h>

using namespace Dali;
using namespace Toolkit;

void dali_property_bridge_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_property_bridge_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
static const char* EMPTY_STRING = "";

static const char* TestStringGetter(Actor actor, const char* propertyName)
{
  if(actor)
  {
    if(std::string(propertyName) == "NAME")
    {
      thread_local std::string value = actor.GetProperty<std::string>(Actor::Property::NAME).c_str();
      return value.c_str();
    }
  }
  return EMPTY_STRING;
}
} // namespace

int UtcDaliToolkitPropertyBridgeGetStringPropertyN(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitPropertyBridgeGetStringPropertyN");

  const char* propertyName   = "NAME";
  const char* expectedResult = "PROPERTY_BRIDGE";

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::NAME, expectedResult);
  DALI_TEST_EQUALS(expectedResult, actor.GetProperty<std::string>(Actor::Property::NAME).c_str(), TEST_LOCATION);

  std::string result = Dali::Toolkit::PropertyBridge::Get().GetStringProperty(actor, propertyName);
  DALI_TEST_EQUALS(EMPTY_STRING, result, TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitPropertyBridgeGetStringPropertyP(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitPropertyBridgeGetStringPropertyP");

  const char* propertyName   = "NAME";
  const char* expectedResult = "PROPERTY_BRIDGE";

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::NAME, expectedResult);
  DALI_TEST_EQUALS(expectedResult, actor.GetProperty<std::string>(Actor::Property::NAME).c_str(), TEST_LOCATION);

  StringGetterDelegate getter = TestStringGetter;
  DALI_TEST_CHECK(getter);

  Dali::Toolkit::PropertyBridgeRegisterStringGetter(getter);

  std::string result = Dali::Toolkit::PropertyBridge::Get().GetStringProperty(actor, propertyName);
  DALI_TEST_EQUALS(expectedResult, result.c_str(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitPropertyBridgeRegisterStringGetter(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitPropertyBridgeRegisterStringGetter");

  StringGetterDelegate getter = TestStringGetter;
  DALI_TEST_CHECK(getter);

  Dali::Toolkit::PropertyBridgeRegisterStringGetter(getter);

  END_TEST;
}

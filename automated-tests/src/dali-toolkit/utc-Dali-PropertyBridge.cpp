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
static std::unordered_map<void*, std::string> gActorStringTable;

// Normal case: return mapped NAME property
static void TestStringGetter_Default(void* refObject, const char* propertyName, std::string* out)
{
  if(!out)
  {
    return;
  }

  if(!refObject || !propertyName)
  {
    *out = std::string();
    return;
  }

  if(std::string(propertyName) != "NAME")
  {
    *out = std::string();
    return;
  }

  auto it = gActorStringTable.find(refObject);
  if(it == gActorStringTable.end())
  {
    *out = std::string(); // empty string
    return;
  }

  *out = it->second;
}

// Always empty string
static void TestStringGetter_Empty(void* /*refObject*/, const char* /*propertyName*/, std::string* out)
{
  if(!out)
  {
    return;
  }
  *out = std::string();
}

// Always null (no value)
static void TestStringGetter_Null(void* /*refObject*/, const char* /*propertyName*/, std::string* /*out*/)
{
  // intentionally no write
}

// Write
static void TestStringGetter_ChangedRequired(void* /*refObject*/, const char* /*propertyName*/, std::string* out)
{
  if(!out)
  {
    return;
  }
  *out = "ABC";
}

} // namespace

int UtcDaliToolkitPropertyBridge_GetStringProperty_N(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitPropertyBridge_GetStringProperty_N");

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::NAME, "PROPERTY_BRIDGE");

  // Getter not registered, should return empty
  std::string result = Dali::Toolkit::PropertyBridge::Get().GetStringProperty(actor, "NAME");
  DALI_TEST_EQUALS(result, std::string(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitPropertyBridge_GetStringProperty_P(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitPropertyBridge_GetStringProperty_P");

  const char* propertyName   = "NAME";
  const char* expectedResult = "PROPERTY_BRIDGE";

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::NAME, expectedResult);

  gActorStringTable[actor.GetObjectPtr()] = expectedResult;

  StringGetterDelegate getter = TestStringGetter_Default;
  Dali::Toolkit::PropertyBridgeRegisterStringGetter(getter);

  // Getter registered, should return NAME
  std::string result = Dali::Toolkit::PropertyBridge::Get().GetStringProperty(actor, propertyName);
  DALI_TEST_EQUALS(result, std::string(expectedResult), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitPropertyBridge_GetStringProperty_Empty(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitPropertyBridge_GetStringProperty_Empty");

  Actor actor = Actor::New();

  // Getter returns Empty string, result should be empty
  StringGetterDelegate getter = TestStringGetter_Empty;
  Dali::Toolkit::PropertyBridgeRegisterStringGetter(getter);

  std::string result = Dali::Toolkit::PropertyBridge::Get().GetStringProperty(actor, "ANY");
  DALI_TEST_EQUALS(result, std::string(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitPropertyBridge_GetStringProperty_Null(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitPropertyBridge_GetStringProperty_Null");

  Actor actor = Actor::New();

  // Getter writes nothing, result should be empty
  StringGetterDelegate getter = TestStringGetter_Null;
  Dali::Toolkit::PropertyBridgeRegisterStringGetter(getter);

  std::string result = Dali::Toolkit::PropertyBridge::Get().GetStringProperty(actor, "ANY");
  DALI_TEST_EQUALS(result, std::string(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitPropertyBridge_GetStringProperty_ChangedRequired(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitPropertyBridge_GetStringProperty_ChangedRequired");

  Actor actor = Actor::New();

  // Getter writes "ABC"
  StringGetterDelegate getter = TestStringGetter_ChangedRequired;
  Dali::Toolkit::PropertyBridgeRegisterStringGetter(getter);

  std::string result = Dali::Toolkit::PropertyBridge::Get().GetStringProperty(actor, "NAME");
  DALI_TEST_EQUALS(result, std::string("ABC"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliToolkitPropertyBridge_RegisterStringGetter(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliToolkitPropertyBridge_RegisterStringGetter");

  StringGetterDelegate getter = TestStringGetter_Default;
  DALI_TEST_CHECK(getter);

  Dali::Toolkit::PropertyBridgeRegisterStringGetter(getter);

  END_TEST;
}

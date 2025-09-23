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

#include <dali-test-suite-utils.h>
#include <dali/devel-api/addons/addon-base.h>
#include <cstring>

namespace
{
bool       gIsPaused            = false;
const auto ANSWER_TO_EVERYTHING = 42;
} //namespace

int StringLen(const char* str)
{
  return strlen(str);
}

int DoSum(int a, int b)
{
  return a + b;
}

bool GetLifecycleStatus()
{
  return gIsPaused;
}

struct AddOnDataInstance
{
  int GetValue()
  {
    return ANSWER_TO_EVERYTHING;
  }

  static int GetValueWithInstance(AddOnDataInstance* instance)
  {
    return instance->GetValue();
  }
};

AddOnDataInstance* CreateInstance()
{
  return new AddOnDataInstance();
}

class TestDummyAddOn : public Dali::AddOns::AddOnBase
{
public:
  void GetAddOnInfo(Dali::AddOnInfo& info) override
  {
    info.type    = Dali::AddOnType::GENERIC;
    info.name    = "SampleAddOn";
    info.version = Dali::DALI_ADDON_VERSION(1, 0, 0);
    info.next    = nullptr;
    tet_printf("SampleAddOn: GetAddOnInfo() : name = %s\n", info.name.c_str());
  }

  /**
   * Dispatch table for global functions
   * @return
   */
  Dali::AddOns::DispatchTable* GetGlobalDispatchTable() override
  {
    static Dali::AddOns::DispatchTable dispatchTable{};
    if(dispatchTable.Empty())
    {
      dispatchTable["DoSum"]              = DoSum;
      dispatchTable["StringLen"]          = StringLen;
      dispatchTable["GetLifecycleStatus"] = GetLifecycleStatus;
      dispatchTable["CreateInstance"]     = CreateInstance;
    }
    return &dispatchTable;
  }

  /**
   * Lifecycle
   */
  void OnStart() override
  {
    gIsPaused = false;
  }

  void OnStop() override
  {
    gIsPaused = true;
  }

  void OnPause() override
  {
    gIsPaused = true;
  }

  void OnResume() override
  {
    gIsPaused = false;
  }

  /**
   * Dispatch table for instance functions
   */
  Dali::AddOns::DispatchTable* GetInstanceDispatchTable() override
  {
    static Dali::AddOns::DispatchTable dispatchTable{};
    if(dispatchTable.Empty())
    {
      dispatchTable["InstanceCall"] = AddOnDataInstance::GetValueWithInstance;
    }
    return &dispatchTable;
  }
};

REGISTER_ADDON_CLASS(TestDummyAddOn);

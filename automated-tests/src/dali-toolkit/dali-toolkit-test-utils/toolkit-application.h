#ifndef DALI_TOOLKIT_TOOLKIT_APPLICATION_H
#define DALI_TOOLKIT_TOOLKIT_APPLICATION_H

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

namespace Dali
{
class Application;

/**
 * This creates a stubbed Application so that Application calls work.
 * Furthermore, it provides an interface to see if certain methods were invoked.
 */
class ToolkitApplication
{
public: // Construction & Destruction
  ToolkitApplication();
  ~ToolkitApplication();

public: // Getters
  Application& GetApplication();

public: // Signal Emissions
public: // TEST FUNCTIONS
  // Enumeration of Application methods
  enum TestFuncEnum
  {
  };

  void Reset()
  {
    mFunctionsCalled.Reset();
  }

  bool WasCalled(TestFuncEnum func)
  {
    switch(func)
    {
    }
    return false;
  }

  void ResetCallStatistics(TestFuncEnum func)
  {
    switch(func)
    {
    }
  }

private:
  struct TestFunctions
  {
    TestFunctions()
    {
    }

    void Reset()
    {
    }
  };

  TestFunctions mFunctionsCalled;

  // The Application Stub
  Application* mApplicationStub;
  friend class Application;

public: // Test static member
  static bool DECODED_IMAGES_SUPPORTED;
  static bool ADD_IDLE_SUCCESS; ///< Default as true.
};

} // namespace Dali

#endif // DALI_TOOLKIT_TOOLKIT_APPLICATION_H

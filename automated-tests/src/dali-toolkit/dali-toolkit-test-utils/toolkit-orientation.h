#ifndef __DALI_TOOLKIT_TOOLKIT_ORIENTATION_H__
#define __DALI_TOOLKIT_TOOLKIT_ORIENTATION_H__

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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/common/orientation.h>

namespace Dali
{

namespace Internal
{
namespace Adaptor
{
class Orientation;
}
}

/**
 * This creates a stubbed Orientation so that internal Toolkit Adaptor calls work.
 * Furthermore, it provides an interface to see if certain methods were invoked.
 */
class ToolkitOrientation
{
public: // Construction & Destruction

  ToolkitOrientation();
  ~ToolkitOrientation();

public: // Getters

  Orientation GetHandle();

public: // Setters

  void SetDegrees( int degrees );

public: // Signal Emissions

  void EmitChangedSignal();

public: // TEST FUNCTIONS

  // Enumeration of Adaptor methods
  enum TestFuncEnum
  {
    GetDegrees,
    GetRadians,
    ChangedSignal,
  };

  void Reset()
  {
    mFunctionsCalled.Reset();
  }

  bool WasCalled(TestFuncEnum func)
  {
    switch(func)
    {
      case GetDegrees:          return mFunctionsCalled.GetDegrees;
      case GetRadians:          return mFunctionsCalled.GetRadians;
      case ChangedSignal:       return mFunctionsCalled.ChangedSignal;
    }
    return false;
  }

  void ResetCallStatistics(TestFuncEnum func)
  {
    switch(func)
    {
      case GetDegrees:          mFunctionsCalled.GetDegrees = false; break;
      case GetRadians:          mFunctionsCalled.GetRadians = false; break;
      case ChangedSignal:       mFunctionsCalled.ChangedSignal = false; break;
    }
  }

private:

  struct TestFunctions
  {
    TestFunctions()
    : GetDegrees(false),
      GetRadians(false),
      ChangedSignal(false)
    {
    }

    void Reset()
    {
      GetDegrees = false;
      GetRadians = false;
      ChangedSignal = false;
    }

    bool GetDegrees;
    bool GetRadians;
    bool ChangedSignal;
  };

  TestFunctions mFunctionsCalled;

  // The stub
  Internal::Adaptor::Orientation* mOrientationStub;
  friend class Internal::Adaptor::Orientation;
  Orientation mOrientation; // Hold a handle ourselves.
};

} // namespace Dali

#endif // __DALI_TOOLKIT_TOOLKIT_ORIENTATION_H__

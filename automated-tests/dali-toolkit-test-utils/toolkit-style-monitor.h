#ifndef __DALI_TOOLKIT_TOOLKIT_STYLE_MONITOR_H__
#define __DALI_TOOLKIT_TOOLKIT_STYLE_MONITOR_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/common/style-monitor.h>

namespace Dali
{

namespace Internal
{
namespace Adaptor
{
class StyleMonitor;
}
}

/**
 * This creates a stubbed StyleMonitor so that internal Toolkit Adaptor calls work.
 * Furthermore, it provides an interface to see if certain methods were invoked.
 */
class ToolkitStyleMonitor
{
public: // Constants

  static const std::string DEFAULT_FONT_FAMILY;
  static const float       DEFAULT_FONT_SIZE;

public: // Construction & Destruction

  ToolkitStyleMonitor();
  ~ToolkitStyleMonitor();

public: // Getters

  StyleMonitor GetStyleMonitor();

public: // Signal Emissions

  void EmitSignalStyleChange(StyleChange styleChange);

public: // TEST FUNCTIONS

  // Enumeration of Adaptor methods
  enum TestFuncEnum
  {
    GetDefaultFontFamilyType,
    GetDefaultFontSizeType,
    SignalStyleChangeType,
  };

  void Reset()
  {
    mFunctionsCalled.Reset();
  }

  bool WasCalled(TestFuncEnum func)
  {
    switch(func)
    {
      case GetDefaultFontFamilyType:          return mFunctionsCalled.GetDefaultFontFamily;
      case GetDefaultFontSizeType:            return mFunctionsCalled.GetDefaultFontSize;
      case SignalStyleChangeType:             return mFunctionsCalled.SignalStyleChange;
    }
    return false;
  }

  void ResetCallStatistics(TestFuncEnum func)
  {
    switch(func)
    {
      case GetDefaultFontFamilyType:          mFunctionsCalled.GetDefaultFontFamily = false; break;
      case GetDefaultFontSizeType:            mFunctionsCalled.GetDefaultFontSize = false; break;
      case SignalStyleChangeType:             mFunctionsCalled.SignalStyleChange = false; break;
    }
  }

private:

  struct TestFunctions
  {
    TestFunctions()
    : GetDefaultFontFamily(false),
      GetDefaultFontSize(false),
      SignalStyleChange(false)
    {
    }

    void Reset()
    {
      GetDefaultFontFamily = false;
      GetDefaultFontSize = false;
      SignalStyleChange = false;
    }

    bool GetDefaultFontFamily;
    bool GetDefaultFontSize;
    bool SignalStyleChange;
  };

  TestFunctions mFunctionsCalled;

  // The StyleMonitor stub
  Internal::Adaptor::StyleMonitor* mStyleMonitorStub;
  friend class Internal::Adaptor::StyleMonitor;
  StyleMonitor mStyleMonitor; // Hold a handle ourselves.
};

} // namespace Dali

#endif // __DALI_TOOLKIT_TOOLKIT_STYLE_MONITOR_H__

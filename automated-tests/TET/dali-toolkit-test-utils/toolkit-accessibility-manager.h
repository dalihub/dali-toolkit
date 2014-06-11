#ifndef __DALI_TOOLKIT_ACCESSIBILITY_MANAGER_H__
#define __DALI_TOOLKIT_ACCESSIBILITY_MANAGER_H__

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

// PUBLIC INCLUDES
#include <dali/public-api/adaptor-framework/common/accessibility-manager.h>

namespace Dali
{

namespace Internal
{
namespace Adaptor
{
class AccessibilityManager;
}
}

/**
 * This creates a stubbed AccessibilityManager so that internal Toolkit Adaptor calls work.
 */
class ToolkitAccessibilityManager
{
public: // Construction & Destruction

  ToolkitAccessibilityManager();
  ~ToolkitAccessibilityManager();

public: // Getters

  AccessibilityManager GetAccessibilityManager();

public: // Signal Emissions

  AccessibilityManager::AccessibilityActionSignalV2& SignalStatusChanged();
  AccessibilityManager::AccessibilityActionSignalV2& SignalActionNext();
  AccessibilityManager::AccessibilityActionSignalV2& SignalActionPrevious();
  AccessibilityManager::AccessibilityActionSignalV2& SignalActionActivate();
  AccessibilityManager::AccessibilityActionSignalV2& SignalActionRead();
  AccessibilityManager::AccessibilityActionSignalV2& SignalActionReadNext();
  AccessibilityManager::AccessibilityActionSignalV2& SignalActionReadPrevious();
  AccessibilityManager::AccessibilityActionSignalV2& SignalActionUp();
  AccessibilityManager::AccessibilityActionSignalV2& SignalActionDown();
  AccessibilityManager::AccessibilityActionSignalV2& SignalActionClearFocus();
  AccessibilityManager::AccessibilityActionSignalV2& SignalActionBack();
  AccessibilityManager::AccessibilityActionSignalV2& SignalActionControlPanelOpen();

private:

  Internal::Adaptor::AccessibilityManager* mAccessibilityManagerStub;
  friend class Internal::Adaptor::AccessibilityManager;
  AccessibilityManager mAccessibilityManager; // Hold a handle ourselves.
};

} // namespace Dali

#endif // __DALI_TOOLKIT_ACCESSIBILITY_MANAGER_H__

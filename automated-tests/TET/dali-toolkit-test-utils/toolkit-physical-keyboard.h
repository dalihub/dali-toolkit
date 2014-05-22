#ifndef __DALI_TOOLKIT_PHYSICAL_KEYBOARD_H__
#define __DALI_TOOLKIT_PHYSICAL_KEYBOARD_H__

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

// PUBLIC INCLUDES
#include <dali/public-api/adaptor-framework/common/physical-keyboard.h>

namespace Dali
{

namespace Internal
{
namespace Adaptor
{
class PhysicalKeyboard;
}
}

/**
 * This creates a stubbed PhysicalKeyboard so that internal Toolkit Adaptor calls work.
 */
class ToolkitPhysicalKeyboard
{
public: // Constants

public: // Construction & Destruction

  ToolkitPhysicalKeyboard();
  ~ToolkitPhysicalKeyboard();

public: // Getters

  Dali::PhysicalKeyboard GetPhysicalKeyboard();

public: // Signal Emissions

  PhysicalKeyboard::Signal& StatusChangedSignal();

public: // TEST FUNCTIONS

private:

  Internal::Adaptor::PhysicalKeyboard* mPhysicalKeyboardStub;
  friend class Internal::Adaptor::PhysicalKeyboard;
  PhysicalKeyboard mPhysicalKeyboard; // Hold a handle ourselves.
};

} // namespace Dali

#endif // __DALI_TOOLKIT_PHYSICAL_KEYBOARD_H__

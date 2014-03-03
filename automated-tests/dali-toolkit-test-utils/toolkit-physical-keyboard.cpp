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

#include "toolkit-physical-keyboard.h"

#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/dali-signal-v2.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * Stub for the PhysicalKeyboard
 */
class PhysicalKeyboard : public BaseObject
{
public: // Creation & Destruction

  PhysicalKeyboard();
  PhysicalKeyboard(ToolkitPhysicalKeyboard *toolkitPhysicalKeyboard);
  ~PhysicalKeyboard();
  static Dali::PhysicalKeyboard Get();

public:

  bool IsAttached() const;

public: // Signals

  Dali::PhysicalKeyboard::Signal& StatusChangedSignal();

private:

  ToolkitPhysicalKeyboard* mToolkitPhysicalKeyboard;
  Dali::PhysicalKeyboard::Signal mStatusChangedSignal;

  bool mIsAttached;
};

namespace
{
PhysicalKeyboard* gPhysicalKeyboard = NULL;
}

PhysicalKeyboard::PhysicalKeyboard()
: mToolkitPhysicalKeyboard(NULL),
  mIsAttached(true)
{
}

PhysicalKeyboard::PhysicalKeyboard(ToolkitPhysicalKeyboard *toolkitPhysicalKeyboard)
: mToolkitPhysicalKeyboard(toolkitPhysicalKeyboard),
  mIsAttached(true)
{
}

PhysicalKeyboard::~PhysicalKeyboard()
{
}

Dali::PhysicalKeyboard PhysicalKeyboard::Get()
{
  DALI_ASSERT_ALWAYS(gPhysicalKeyboard);
  return Dali::PhysicalKeyboard(gPhysicalKeyboard);
}

bool PhysicalKeyboard::IsAttached() const
{
  return mIsAttached;
}

Dali::PhysicalKeyboard::Signal& PhysicalKeyboard::StatusChangedSignal()
{
  return mStatusChangedSignal;
}

} // namespace Adaptor

} // namespace Internal

////////////////////////////////////////////////////////////////////////////////////////////////////

ToolkitPhysicalKeyboard::ToolkitPhysicalKeyboard()
: mPhysicalKeyboardStub(new Internal::Adaptor::PhysicalKeyboard(this)),
  mPhysicalKeyboard( mPhysicalKeyboardStub )
{
  Dali::Internal::Adaptor::gPhysicalKeyboard = mPhysicalKeyboardStub;
}

ToolkitPhysicalKeyboard::~ToolkitPhysicalKeyboard()
{
}

PhysicalKeyboard ToolkitPhysicalKeyboard::GetPhysicalKeyboard()
{
  return mPhysicalKeyboard;
}

} // namespace Dali
